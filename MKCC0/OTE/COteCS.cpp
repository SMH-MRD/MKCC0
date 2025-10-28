#include "CMCProtocol.h"
#include "COteCS.h"
#include "resource.h"
#include "CGamePad.h"
#include "CCrane.h"
#include "SmemAux.h"
#include "COteEnv.h"

extern CSharedMem* pOteEnvInfObj;
extern CSharedMem* pOteCsInfObj;
extern CSharedMem* pOteCcInfObj;
extern CSharedMem* pOteUiObj;
extern ST_DEVICE_CODE g_my_code; //�[���R�[�h

extern CCrane* pCrane;

extern vector<CBasicControl*>	VectCtrlObj;
extern BC_TASK_ID st_task_id;


//�\�P�b�g
static CMCProtocol* pMCSock;				//MC�v���g�R���I�u�W�F�N�g�|�C���^

//�N���X�X�^�e�B�b�N�����o
ST_OTE_CS_MON1 COteCS::st_mon1;
ST_OTE_CS_MON2 COteCS::st_mon2;

ST_OTE_CS_INF COteCS::st_work;
ST_OTE_CS_OBJ COteCS::st_obj;

static COteEnv* pEnvObj;

//���L�������Q�Ɨp��`

//���L������
static LPST_OTE_ENV_INF	pOteEnvInf;
static LPST_OTE_UI		pOteUi;
static LPST_OTE_CC_IF	pOteCCInf;
static LPST_OTE_CS_INF	pOteCsInf;

static CGamePad* pPad = NULL;

static LONG rcv_count_plc_r = 0, snd_count_plc_r = 0, rcv_errcount_plc_r = 0;
static LONG rcv_count_plc_w = 0, snd_count_plc_w = 0, rcv_errcount_plc_w = 0;
static LARGE_INTEGER start_count_w, end_count_w, start_count_r, end_count_r;  //�V�X�e���J�E���g
static LARGE_INTEGER frequency;				//�V�X�e�����g��
static LONGLONG res_delay_max_w, res_delay_max_r;	//PLC��������

static ST_PLC_IO_WIF* pPlcWIf = NULL;
static ST_PLC_IO_RIF* pPlcRIf = NULL;

COteCS::COteCS() {
	st_obj.remote_pb.set(0); st_obj.remote_mode.set(0); st_obj.game_pad_pb.set(0);
}
COteCS::~COteCS() {
}

HRESULT COteCS::initialize(LPVOID lpParam) {

	HRESULT hr = S_OK;

	//�V�X�e�����g���ǂݍ���
	QueryPerformanceFrequency(&frequency);

	//### �o�͗p���L�������擾
	out_size = sizeof(ST_OTE_CS_INF);
	if (OK_SHMEM != pOteCsInfObj->create_smem(SMEM_OTE_CS_INF_NAME, out_size, MUTEX_OTE_CS_INF_NAME)) {
		err |= SMEM_NG_OTE_CS_INF;
	}
	set_outbuf(pOteCsInfObj->get_pMap());

	//### Environment�̃C���X�^���X�|�C���^�擾
	pEnvObj = (COteEnv*)VectCtrlObj[st_task_id.ENV];
	
	//### ���͗p���L�������擾
	pOteCCInf = (LPST_OTE_CC_IF)pOteCcInfObj->get_pMap();
	pOteEnvInf = (LPST_OTE_ENV_INF)(pOteEnvInfObj->get_pMap());
	pOteCsInf = (LPST_OTE_CS_INF)(pOteCsInfObj->get_pMap());
	pOteUi = (LPST_OTE_UI)pOteUiObj->get_pMap();

	if ((pOteEnvInf == NULL) || (pOteCsInf == NULL) || (pOteUi == NULL) || (pOteCCInf == NULL))
		hr = S_FALSE;

	if (hr == S_FALSE) {
		wos.str(L""); wos << L"Initialize : SMEM NG"; msg2listview(wos.str());
		return hr;
	};

	//### IF�E�B���h�E  MON2 OPEN PLC�ʐM
	WPARAM wp = MAKELONG(inf.index, WM_USER_WPH_OPEN_IF_WND);//HWORD:�R�}���h�R�[�h, LWORD:�^�X�N�C���f�b�N�X
	LPARAM lp = BC_ID_MON2;
	SendMessage(inf.hwnd_opepane, WM_USER_TASK_REQ, wp, lp);
	Sleep(100);
	wos.str(L"");//������
	if (st_mon2.hwnd_mon == NULL) {
		wos << L"Initialize : MON NG"; msg2listview(wos.str());
		return S_FALSE;
	}
	else {
		pMCSock = new CMCProtocol(ID_SOCK_MC_OTE_CS);
		if (pMCSock->Initialize(st_mon2.hwnd_mon, PLC_IF_TYPE_OTE) != S_OK) {
			wos << L"Initialize : MC Init NG"; msg2listview(wos.str()); wos.str(L"");
			wos << L"Err :" << pMCSock->msg_wos.str(); msg2listview(wos.str()); wos.str(L"");
			return S_FALSE;
		}
		else {
			wos << L"MCProtocol Init OK"; msg2listview(wos.str());
		}
	}
	//### GamePad�C���X�^���X
	pPad = new CGamePad();
	pPad->set_id(0);

	//###  �I�y���[�V�����p�l���ݒ�
	//Function mode RADIO1
	inf.panel_func_id = IDC_TASK_FUNC_RADIO1;
	SendMessage(GetDlgItem(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO1), BM_SETCHECK, BST_CHECKED, 0L);
	for (int i = 1; i < 6; i++)
		SendMessage(GetDlgItem(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO1 + i), BM_SETCHECK, BST_UNCHECKED, 0L);
	//���[�h�ݒ�0
	inf.mode_id = BC_ID_MODE0;
	SendMessage(GetDlgItem(inf.hwnd_opepane, IDC_TASK_MODE_RADIO0), BM_SETCHECK, BST_CHECKED, 0L);
	//���j�^�E�B���h�E�e�L�X�g	
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_MON_CHECK2, L"PLC IF");
	//���j�^�E�B���h�E�e�L�X�g	
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_MON_CHECK1, L"GPAD IF");
	set_item_chk_txt();
	set_panel_tip_txt();
	//���j�^�Q CB��ԃZ�b�g	
	if (st_mon2.hwnd_mon != NULL)
		SendMessage(GetDlgItem(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK1), BM_SETCHECK, BST_CHECKED, 0L);
	else
		SendMessage(GetDlgItem(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK1), BM_SETCHECK, BST_UNCHECKED, 0L);

	st_obj.pad_mh=new CPadNotch(pCrane->get_base_mh(), ID_HOIST);
	st_obj.pad_bh=new CPadNotch(pCrane->get_base_bh(), ID_BOOM_H);
	st_obj.pad_sl=new CPadNotch(pCrane->get_base_sl(), ID_SLEW);
	st_obj.pad_gt=new CPadNotch(pCrane->get_base_gt(), ID_GANTRY);
	st_obj.pad_ah=new CPadNotch(pCrane->get_base_ah(), ID_AHOIST);

	set_func_pb_txt();
	set_item_chk_txt();
	set_panel_tip_txt();

	return hr;
}

HRESULT COteCS::routine_work(void* pObj) {
	if (inf.total_act % 20 == 0) {
		wos.str(L""); wos << inf.status << L":" << std::setfill(L'0') << std::setw(4) << inf.act_time;
		wos << L"  OPE SRC CODE:" << hex << pOteCsInf->ope_source_mode;
		wos << L"  RMT_MODE:" << st_work.st_body.remote;

		msg2host(wos.str());
	}
	input();
	parse();
	output();
	return S_OK;
}

static UINT32	gpad_mode_last = L_OFF;


/// <summary>
/// #### ������͓��e�̎�荞��
/// #### �M���̓��e�ɂ����Source(�����,PC Win�p�l��,GPad��I�����Ď�荞��
/// </summary>
/// <returns></returns>
int COteCS::input(){

	memset(pOteCsInf->pnl_ctrl, 0, sizeof(pOteCsInf->pnl_ctrl));//�p�l�����̓N���A

	//### �����PLC�M����荞��(pOteCsInf->buf_opepnl_read��MC�v���g�R���œǍ�)
	LPST_PLC_RBUF_HHGG38 pin_opepnl = (LPST_PLC_RBUF_HHGG38)pOteCsInf->buf_opepnl_read;//�����M������// 

	//### PCWindow�p�l���M����荞��(pOteUi->pnl_ctrl[])
	//## Scad��pOteUi->pnl_ctrl[]�ɃZ�b�g����Ă���	

	//### �Q�[���p�b�h��荞��(pOteCsInf->gpad_in)
	if ((pPad != NULL) && st_work.st_body.game_pad_mode) {
		if (pPad->PollController(pPad->controllerId)) {//GamePad��Ԏ�荞�݁ˈȍ~�A���L�������ɓW�J
			st_work.st_body.game_pad_mode = CODE_PNL_COM_OFF;
		}
		//GamePad��DI�l�����L�������ɃZ�b�g�iValue�I�u�W�F�N�g�ɃZ�b�g��Bool�ɕϊ����ċ��L�������ɃZ�b�g�j
		pOteCsInf->gpad_in.syukan_on	= pPad->chk_on(st_obj.syukan_on.set(pPad->get_start()));
		pOteCsInf->gpad_in.syukan_off	= pPad->chk_on(st_obj.syukan_off.set(pPad->get_Y()));
		pOteCsInf->gpad_in.remote		= pPad->chk_on(st_obj.remote_pb.set(pPad->get_A()));
		pOteCsInf->gpad_in.estop		= pPad->chk_on(st_obj.estop.set(pPad->get_B()));
		pOteCsInf->gpad_in.f_reset		= pPad->chk_on(st_obj.f_reset.set(pPad->get_back()));
		pOteCsInf->gpad_in.bypass		= pPad->chk_on(st_obj.bypass.set(pPad->get_X()));
		pOteCsInf->gpad_in.kidou_r		= pPad->chk_on(st_obj.kidou_r.set(pPad->get_thumbr()));
		pOteCsInf->gpad_in.kidou_l		= pPad->chk_on(st_obj.kidou_l.set(pPad->get_thumbl()));
		pOteCsInf->gpad_in.pan_l		= pPad->chk_on(st_obj.pan_l.set(pPad->get_left()));
		pOteCsInf->gpad_in.pan_r		= pPad->chk_on(st_obj.pan_r.set(pPad->get_right()));
		pOteCsInf->gpad_in.tilt_u		= pPad->chk_on(st_obj.tilt_u.set(pPad->get_up()));
		pOteCsInf->gpad_in.tilt_d		= pPad->chk_on(st_obj.tilt_d.set(pPad->get_down()));
		pOteCsInf->gpad_in.zoom_f		= pPad->chk_on(st_obj.zoom_f.set(pPad->get_shoulderr()));
		pOteCsInf->gpad_in.zoom_n		= pPad->chk_on(st_obj.zoom_n.set(pPad->get_shoulderl()));
		pOteCsInf->gpad_in.trig_l		= st_obj.trig_l.set(pPad->get_trig_L());
		pOteCsInf->gpad_in.trig_r		= st_obj.trig_r.set(pPad->get_trig_R());

		//GamePad�̃A�i���O�l��Value�I�u�W�F�N�g�ɃZ�b�g�˃m�b�`���ɕϊ����ċ��L�������ɃZ�b�g
		st_obj.pad_mh->set(pPad->get_RY());
		st_obj.pad_bh->set(pPad->get_LY());
		st_obj.pad_sl->set(pPad->get_LX());
		st_obj.pad_gt->set(pPad->get_RX());

		pOteCsInf->gpad_in.pad_mh = st_obj.pad_mh->get_notch();
		pOteCsInf->gpad_in.pad_bh = st_obj.pad_bh->get_notch();
		pOteCsInf->gpad_in.pad_sl = st_obj.pad_sl->get_notch();
		pOteCsInf->gpad_in.pad_gt = st_obj.pad_gt->get_notch();
	}
	
	//## ���u�����M����荞�݁i�������^�� & ����~�� ���u�����D��j
	if (pOteCsInf->ope_source_mode & OTE_OPE_SOURCE_CODE_OPEPNL) {
	
		//����~
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::estop]			= (pin_opepnl->xin[4] & 0x0020);
	
		//����t�b�g�u���[�L(0-15)
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::sl_brk]	= pin_opepnl->ai_sl_foot;

		//�������^���X�C�b�` �i�n�[�hSW�j
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::syukan_on]		= pin_opepnl->xin[1] & 0x0200;
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::syukan_off]		= pin_opepnl->xin[1] & 0x0400;
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::fault_reset]		= pin_opepnl->xin[1] & 0x0100;
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::bypass]			= pin_opepnl->xin[1] & 0x6000;
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::alm_stop]		= pin_opepnl->xin[1] & 0x0080;
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::motor_siren]		= pin_opepnl->xin[1] & 0x8000;
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::hv_trolley]		= pin_opepnl->xin[2] & 0x0300;
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::hv_gantry]		= pin_opepnl->xin[3] & 0x3000;
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::hv_aux]			= pin_opepnl->xin[3] & 0xc000;
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::camA_adjust]		= pin_opepnl->xin[2] & 0x000f;
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::camB_adjust]		= pin_opepnl->xin[2] & 0x00f0;
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::camAselect]		= pin_opepnl->xin[2] & 0x1c00;
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::camBselect]		= pin_opepnl->xin[2] & 0xe000;
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::irisA]			= pin_opepnl->xin[0] & 0x3000;
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::irisB]			= pin_opepnl->xin[4] & 0xc000;

		//����,�A���u�i�^�b�`�p�l���j
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::remote]			= pin_opepnl->auto_sw & 0x0001;
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::auto_mode]		= pin_opepnl->auto_sw & 0x0002;
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::game_pad]		= pin_opepnl->auto_sw & 0x0004;
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::asel_mh]			= pin_opepnl->auto_sw & 0x0010;
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::asel_bh]			= pin_opepnl->auto_sw & 0x0020;
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::asel_sl]			= pin_opepnl->auto_sw & 0x0040;
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::asel_gt]			= pin_opepnl->auto_sw & 0x0080;
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::asel_ah]			= pin_opepnl->auto_sw & 0x0100;
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::ote_type]		= pin_opepnl->auto_sw & 0x0200;

		//���u�����D��i�I���^�l�[�gSW�j
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::hd_lamp1]		= pin_opepnl->lamp_sw & 0x0001;
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::hd_lamp2]		= pin_opepnl->lamp_sw & 0x0002;
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::hd_lamp3]		= pin_opepnl->lamp_sw & 0x0004;
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::mh_spd_mode]		= pin_opepnl->mh_mode_cs;
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::bh_r_mode]		= pin_opepnl->bh_mode_cs;
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::notch_aux]		= pin_opepnl->notch_L1;

		//�Ɩ�
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::hd_lamp1] = pin_opepnl->lamp_sw & 0x0001;
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::hd_lamp2] = pin_opepnl->lamp_sw & 0x0002;
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::hd_lamp3] = pin_opepnl->lamp_sw & 0x0004;
	}
	else {//�I���^�l�[�gSW�͑���䖳�����̂�PC�p�l���w�ߎ�t�iGpad�̓I���^�l�[�gSW�����j
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::hd_lamp1]		= pOteUi->pnl_ctrl[OTE_PNL_CTRLS::hd_lamp1] ;
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::hd_lamp2]		= pOteUi->pnl_ctrl[OTE_PNL_CTRLS::hd_lamp2];
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::hd_lamp3]		= pOteUi->pnl_ctrl[OTE_PNL_CTRLS::hd_lamp3];
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::mh_spd_mode]		= pOteUi->pnl_ctrl[OTE_PNL_CTRLS::mh_spd_mode];
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::bh_r_mode]		= pOteUi->pnl_ctrl[OTE_PNL_CTRLS::bh_r_mode];
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::notch_aux]		= pOteUi->pnl_ctrl[OTE_PNL_CTRLS::notch_aux];
	}
	
	//## �Q�[���p�b�h�M����荞�݁i�������^���j
	if (pOteCsInf->ope_source_mode & OTE_OPE_SOURCE_CODE_GPAD) {
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::syukan_on]		|= pOteCsInf->gpad_in.syukan_on;
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::syukan_off]		|= pOteCsInf->gpad_in.syukan_off;
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::fault_reset]		|= pOteCsInf->gpad_in.f_reset;
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::estop]			|= pOteCsInf->gpad_in.estop;
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::bypass]			|= pOteCsInf->gpad_in.bypass;
		
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::remote]			|= pOteCsInf->gpad_in.remote;

		pOteCsInf->gpad_in.kidou_r;
		pOteCsInf->gpad_in.kidou_l;
		pOteCsInf->gpad_in.pan_l;
		pOteCsInf->gpad_in.pan_r;
		pOteCsInf->gpad_in.tilt_u;
		pOteCsInf->gpad_in.tilt_d;
		pOteCsInf->gpad_in.zoom_f;
		pOteCsInf->gpad_in.zoom_n;

		//����u���[�L�y�_��(0-15)
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::sl_brk] = (pOteCsInf->gpad_in.trig_l+ pOteCsInf->gpad_in.trig_r) / 0x10;
	}
	
	//## PC Win�p�l���M����荞�݁i�������^���j
	if (pOteCsInf->ope_source_mode & OTE_OPE_SOURCE_CODE_PCPNL) {
	
		//����~
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::estop]			|= pOteUi->pnl_ctrl[OTE_PNL_CTRLS::estop];
		//����t�b�g�u���[�L
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::sl_brk]	|= pOteUi->pnl_ctrl[OTE_PNL_CTRLS::sl_brk];

		//�������^���X�C�b�` �i�n�[�hSW�j
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::syukan_on]		|=  pOteUi->pnl_ctrl[OTE_PNL_CTRLS::syukan_on]	;
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::syukan_off]		|=  pOteUi->pnl_ctrl[OTE_PNL_CTRLS::syukan_off]	;
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::fault_reset]		|=  pOteUi->pnl_ctrl[OTE_PNL_CTRLS::fault_reset];
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::bypass]			|=  pOteUi->pnl_ctrl[OTE_PNL_CTRLS::bypass]		;
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::alm_stop]		|=  pOteUi->pnl_ctrl[OTE_PNL_CTRLS::alm_stop]	;
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::motor_siren]		|=  pOteUi->pnl_ctrl[OTE_PNL_CTRLS::motor_siren];
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::hv_trolley]		|=  pOteUi->pnl_ctrl[OTE_PNL_CTRLS::hv_trolley]	;
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::hv_gantry]		|=  pOteUi->pnl_ctrl[OTE_PNL_CTRLS::hv_gantry]	;
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::hv_aux]			|=  pOteUi->pnl_ctrl[OTE_PNL_CTRLS::hv_aux]		;
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::camA_adjust]		|=  pOteUi->pnl_ctrl[OTE_PNL_CTRLS::camA_adjust];
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::camB_adjust]		|=  pOteUi->pnl_ctrl[OTE_PNL_CTRLS::camB_adjust];
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::camAselect]		|=  pOteUi->pnl_ctrl[OTE_PNL_CTRLS::camAselect]	;
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::camBselect]		|=  pOteUi->pnl_ctrl[OTE_PNL_CTRLS::camBselect]	;
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::irisA]			|=  pOteUi->pnl_ctrl[OTE_PNL_CTRLS::irisA]		;
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::irisB]			|=  pOteUi->pnl_ctrl[OTE_PNL_CTRLS::irisB]		;

		//����,�A���u�i�^�b�`�p�l���j
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::remote]			|= pOteUi->pnl_ctrl[OTE_PNL_CTRLS::remote];
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::auto_mode]		|= pOteUi->pnl_ctrl[OTE_PNL_CTRLS::auto_mode];
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::game_pad]		|= pOteUi->pnl_ctrl[OTE_PNL_CTRLS::game_pad];
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::asel_mh]			|= pOteUi->pnl_ctrl[OTE_PNL_CTRLS::asel_mh];
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::asel_bh]			|= pOteUi->pnl_ctrl[OTE_PNL_CTRLS::asel_bh];
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::asel_sl]			|= pOteUi->pnl_ctrl[OTE_PNL_CTRLS::asel_sl];
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::asel_gt]			|= pOteUi->pnl_ctrl[OTE_PNL_CTRLS::asel_gt];
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::asel_ah]			|= pOteUi->pnl_ctrl[OTE_PNL_CTRLS::asel_ah];
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::ote_type]		|= pOteUi->pnl_ctrl[OTE_PNL_CTRLS::ote_type];
	}

	//## �m�b�`�w�ߒl��荞��

	if(pOteCsInf->ope_source_mode & OTE_OPE_SOURCE_CODE_OPEPNL){
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::notch_mh]	= pin_opepnl->notch_RY0;
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::notch_bh]	= pin_opepnl->notch_LY0;
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::notch_sl]	= pin_opepnl->notch_LX0;
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::notch_gt]	= pin_opepnl->notch_R1;
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::notch_ah]	= pin_opepnl->notch_RX0;
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::notch_aux]	= pin_opepnl->notch_L1;
	}
	//�m�b�`�w�ߒl�́AGPAD Mode�̎��́AGPAD > �����
	if (pOteCsInf->ope_source_mode & OTE_OPE_SOURCE_CODE_GPAD) {
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::notch_mh]	= pOteCsInf->gpad_in.pad_mh;
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::notch_bh]	= pOteCsInf->gpad_in.pad_bh;
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::notch_sl]	= pOteCsInf->gpad_in.pad_sl;
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::notch_gt]	= pOteCsInf->gpad_in.pad_gt;
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::notch_ah]	= pOteCsInf->gpad_in.pad_ah;
	}
	//PC�p�l���̃m�b�`PB���͂͑����܂���GPAD�w��OFF�̎��̂ݗL��(���ݒ�l��0�̎��̂ݏ㏑��
	if (pOteCsInf->ope_source_mode & OTE_OPE_SOURCE_CODE_PCPNL) {
		if (!(pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::notch_mh]))	pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::notch_mh] = pOteUi->pnl_ctrl[OTE_PNL_CTRLS::notch_mh];
		if (!(pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::notch_bh]))	pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::notch_bh] = pOteUi->pnl_ctrl[OTE_PNL_CTRLS::notch_bh];
		if (!(pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::notch_sl]))	pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::notch_sl] = pOteUi->pnl_ctrl[OTE_PNL_CTRLS::notch_sl];
		if (!(pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::notch_gt]))	pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::notch_gt] = pOteUi->pnl_ctrl[OTE_PNL_CTRLS::notch_gt];
		if (!(pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::notch_ah]))	pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::notch_ah] = pOteUi->pnl_ctrl[OTE_PNL_CTRLS::notch_ah];
		if (!(pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::notch_aux]))	pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::notch_aux] = pOteUi->pnl_ctrl[OTE_PNL_CTRLS::notch_aux];
	}

	//## GOT�w�ߒl��荞�݁i�N���[���ڑ��j
	if (pOteCsInf->ope_source_mode & OTE_OPE_SOURCE_CODE_OPEPNL) {
		pOteCsInf->GOT_command = pin_opepnl->got_command;	
		pOteCsInf->GOT_crane_select = pin_opepnl->got_crane_selected;
	}

	//### ����u���[�L�w�ߐM�����`
	if (pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::sl_brk]		< 0	)	pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::sl_brk]		= 0;
	if (pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::sl_brk]		> 15)	pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::sl_brk]		= 15;
	if (pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::notch_aux]	> 0	)	pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::sl_brk]		|= AUX_SLBRK_COM_HW_BRK; //HW�u���[�L
	if (pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::notch_aux]	== -1)	pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::sl_brk]		|= AUX_SLBRK_COM_RESET; //���Z�b�g
	if (pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::notch_aux]	== -2)	pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::sl_brk]		|= AUX_SLBRK_COM_FREE; //�t���[(AUTO/MANUAL)

	return S_OK;
}

static INT16 ope_plc_cnt;
static UINT16 ope_plc_chk_cnt=0;
static INT16 pnl_ctrl_last[N_OTE_PNL_CTRL];
//#### ���[�h,�w�ߒl�ݒ�@
int COteCS::parse() 
{           
	
	//### �ُ�`�F�b�N
	{
		//## �����w���V�[�`�F�b�N
		if (ope_plc_cnt == ((LPST_PLC_RBUF_HHGG38)pOteCsInf->buf_opepnl_read)->plc_healthy) {//�O��l����ω��Ȃ�
			ope_plc_chk_cnt++;
			if(ope_plc_chk_cnt > 10) {					//PLC�ʐM�ُ�
				pOteCsInf->ope_plc_stat = L_OFF;
				pOteCsInf->ote_error |= FLTS_MASK_ERR_RPC_RPLC_COMM;
			}
		}
		else {//PLC�ʐM����
			pOteCsInf->ope_plc_stat = L_ON;
			ope_plc_chk_cnt = 0;
			pOteCsInf->ote_error &= ~FLTS_MASK_ERR_RPC_RPLC_COMM;
		}
		ope_plc_cnt = ((LPST_PLC_RBUF_HHGG38)pOteCsInf->buf_opepnl_read)->plc_healthy;

		//## ����PC�ʐM�ُ�`�F�b�N
		if(pOteCCInf->cc_comm_chk_cnt >= PRM_OTE_PC_COM_TMOV_CNT)
			pOteCsInf->ote_error |= FLTS_MASK_ERR_CPC_RPC_COMM;
		else
			pOteCsInf->ote_error &= ~FLTS_MASK_ERR_CPC_RPC_COMM;

		//## ����~�`�F�b�N
		if (pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::estop]) {
			pOteCsInf->ote_error |= FLTS_MASK_ERR_RPC_ESTP;
		}
		else {
			pOteCsInf->ote_error &= ~FLTS_MASK_ERR_RPC_ESTP;
		}

		pOteCsInf->rpc_flt_count = 0;
		for (int j = 0; j < 16; j++) {
			if (pOteCsInf->rpc_flt_count >= OTE_PC_FLT_DETECT_MAX) break;	//�\���̏ᐔ���

			if (pOteCsInf->ote_error & (1 << j)) {	//���o����̎�
				pOteCsInf->rpc_flt_codes[pOteCsInf->rpc_flt_count] = 850 + j;
				pOteCsInf->rpc_flt_count++;	//PC�̏ᐔ�J�E���g�A�b�v
			}
		}
	}
	
	//### ���[�h�ݒ�
	{
		//### ���u�����[�g����L�����ݒ�
		//���uPB�g���K���o
		if (pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::remote] && (pnl_ctrl_last[OTE_PNL_CTRLS::remote] == L_OFF)) {
			if (st_work.st_body.remote == CODE_PNL_COM_SELECTED)			// ���u���샂�[�h�I��,���F�҂��@
				st_work.st_body.remote = CODE_PNL_COM_OFF;					//		�ˁ@���u���샂�[�h�I��v��OFF
			else if (st_work.st_body.remote == CODE_PNL_COM_ACTIVE)			// ���u���샂�[�h�I��,���F��
				st_work.st_body.remote = CODE_PNL_COM_OFF;					//		�ˁ@���u���샂�[�h�I��v��OFF
			else															// ���u���j�^�[���[�h�I���@		�ˁ@���u���샂�[�h�I��
				st_work.st_body.remote = CODE_PNL_COM_SELECTED;				//		�ˁ@���u���샂�[�h�I��,���F�҂�
		}
		//����PC����̏��F�m�F�i�ʐM�w�b�_��ID����ID�Ɠ����ꍇ�ɏ��F�j
		if ((st_work.st_body.remote == CODE_PNL_COM_SELECTED) && (pOteCCInf->cc_active_ote_id == g_my_code.serial_no))
			st_work.st_body.remote = CODE_PNL_COM_ACTIVE;
		//���u����ڑ�������OFF
		if((pOteCCInf->cc_active_ote_id != g_my_code.serial_no)&&(st_work.st_body.remote == CODE_PNL_COM_ACTIVE))
			st_work.st_body.remote = CODE_PNL_COM_OFF;
		if (!(pOteUi->pc_pnl_active))
			 st_work.st_body.remote = CODE_PNL_COM_OFF;

		//�O��l�ێ�
		pnl_ctrl_last[OTE_PNL_CTRLS::remote] = pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::remote];

		//### GamePad���[�h�ݒ�
		if (CODE_TRIG_ON == st_obj.game_pad_pb.chk_trig(pOteUi->pnl_ctrl[OTE_PNL_CTRLS::game_pad])) {
			if (st_work.st_body.game_pad_mode == CODE_PNL_COM_ACTIVE)
				st_work.st_body.game_pad_mode = CODE_PNL_COM_OFF;
			else
				st_work.st_body.game_pad_mode = CODE_PNL_COM_ACTIVE;
		}
		//### �����^�C�v�ݒ�(���u�����^�C�v/�^�u���b�g/���u�f�o�C�X)
		// ���X�e�b�v�őΉ�
		//if (st_obj.ote_type.chk_trig(pOteUi->pnl_ctrl[OTE_PNL_CTRLS::ote_type])) {
		//	st_work.st_body.ote_type = st_obj.ote_type.get();
		//}

		//## ����f�o�C�X���[�h�ݒ�
		//#�����
		if (pOteCsInf->ope_plc_stat)
			pOteCsInf->ope_source_mode |= OTE_OPE_SOURCE_CODE_OPEPNL;
		else
			pOteCsInf->ope_source_mode &= ~OTE_OPE_SOURCE_CODE_OPEPNL;
		//#GamePad
		if ((pPad != NULL) && (st_work.st_body.game_pad_mode == CODE_PNL_COM_ACTIVE))
			pOteCsInf->ope_source_mode |= OTE_OPE_SOURCE_CODE_GPAD;
		else
			pOteCsInf->ope_source_mode &= ~OTE_OPE_SOURCE_CODE_GPAD;
		//#PC�p�l��
		if (pOteUi->pc_pnl_active == L_ON)
			pOteCsInf->ope_source_mode |= OTE_OPE_SOURCE_CODE_PCPNL;
		else
			pOteCsInf->ope_source_mode &= ~OTE_OPE_SOURCE_CODE_PCPNL;
		//#�g�уp�l���i���X�e�b�v�őΉ��j
		//if (pOteUi->pc_pnl_active == L_ON)
		//	pOteCsInf->ope_source_mode |= OTE_OPE_SOURCE_CODE_PCPNL;
		//else
		//	pOteCsInf->ope_source_mode &= ~OTE_OPE_SOURCE_CODE_PCPNL;
	}

	//###�����ւ̐���PC����̏o�͓��e�ݒ�
	{

	//##�����n�[�h�����v
		INT16 plc_yo_buf = 0;
		//�劲�����v
		if(pOteCCInf->st_msg_pc_u_rcv.body.st.lamp[OTE_PNL_CTRLS::syukan_on].code)	plc_yo_buf |=  0x0040;
		if(pOteCCInf->st_msg_pc_u_rcv.body.st.lamp[OTE_PNL_CTRLS::syukan_off].code)	plc_yo_buf |=  0x0080;
		//���������v
		if (pOteCCInf->st_msg_pc_u_rcv.body.st.lamp[OTE_PNL_CTRLS::hv_trolley].code)plc_yo_buf |= 0x0001;
		else																		plc_yo_buf |= 0x0002;
		if (pOteCCInf->st_msg_pc_u_rcv.body.st.lamp[OTE_PNL_CTRLS::hv_gantry].code)	plc_yo_buf |= 0x0004;
		else																		plc_yo_buf |= 0x0008;
		if (pOteCCInf->st_msg_pc_u_rcv.body.st.lamp[OTE_PNL_CTRLS::hv_aux].code)	plc_yo_buf |= 0x0010;
		else																		plc_yo_buf |= 0x0020;
		//�̏჉���v
		if (pOteCCInf->st_msg_pc_u_rcv.body.st.lamp[OTE_PNL_CTRLS::fault_lamp].code)plc_yo_buf |= 0x0100;
		//�x�񃉃��v
		if (pOteCCInf->st_msg_pc_u_rcv.body.st.lamp[OTE_PNL_CTRLS::alm_lamp].code)	plc_yo_buf |= 0x0200;
		//�u�U�[
		INT16 bz_code = (INT16)pOteCCInf->st_msg_pc_u_rcv.body.st.lamp[OTE_PNL_CTRLS::buzzer].code;
		plc_yo_buf &= 0x0FFF;
		if (bz_code) {
			if (bz_code & 0x0001)plc_yo_buf |= 0x1000;	//�̏�
			else if (bz_code & 0x0001)plc_yo_buf |= 0x1000;	//�d�̏�
			else if (bz_code & 0x0002)plc_yo_buf |= 0x2000;	//�劲����
			else if (bz_code & 0x0004)plc_yo_buf |= 0x4000;	//�a��
			else if (bz_code & 0x0008)plc_yo_buf |= 0x8000;	//�y�̏�
			else;
		}
		//#���ŃZ�b�g�������e��IF�o�b�t�@�ɃZ�b�g
		((LPST_PLC_WBUF_HHGG38)(pOteCsInf->buf_opepnl_write))->lamp1 = plc_yo_buf;

		//�努�A�������[�h�X�C�b�`
		((LPST_PLC_WBUF_HHGG38)(pOteCsInf->buf_opepnl_write))->mh_set.mode = pOteCCInf->st_msg_pc_u_rcv.body.st.lamp[OTE_PNL_CTRLS::mh_spd_mode].st.com;
		((LPST_PLC_WBUF_HHGG38)(pOteCsInf->buf_opepnl_write))->bh_set.mode = pOteCCInf->st_msg_pc_u_rcv.body.st.lamp[OTE_PNL_CTRLS::bh_r_mode].st.com;


	//##�����GOT�����v
		plc_yo_buf = 0;
		//���͊m�������v
		if (pOteCCInf->st_msg_pc_u_rcv.body.st.lamp[OTE_PNL_CTRLS::main_power].code)	plc_yo_buf |= 0x0001;
		//�������������v
		if (pOteCCInf->st_msg_pc_u_rcv.body.st.lamp[OTE_PNL_CTRLS::sl_auto_gr].code)	plc_yo_buf |= 0x0002;
		//�T�C���������v
		if (pOteCCInf->st_msg_pc_u_rcv.body.st.lamp[OTE_PNL_CTRLS::motor_siren].code)	plc_yo_buf |= 0x8000;
		//�Ɩ������v
		if (pOteCCInf->st_msg_pc_u_rcv.body.st.lamp[OTE_PNL_CTRLS::hd_lamp1].code)	plc_yo_buf |= 0x0800;
		if (pOteCCInf->st_msg_pc_u_rcv.body.st.lamp[OTE_PNL_CTRLS::hd_lamp2].code)	plc_yo_buf |= 0x1010;
		if (pOteCCInf->st_msg_pc_u_rcv.body.st.lamp[OTE_PNL_CTRLS::hd_lamp3].code)	plc_yo_buf |= 0x2000;

		//#IF�o�b�t�@�ɃZ�b�g
		((LPST_PLC_WBUF_HHGG38)(pOteCsInf->buf_opepnl_write))->lamp2 = plc_yo_buf;
		//##����쉓�u���[�h�\��
		((LPST_PLC_WBUF_HHGG38)(pOteCsInf->buf_opepnl_write))->rmt_status = st_work.st_body.remote;
	}
	return STAT_OK;
}

static INT16 ote_helthy = 0; //�w���V�[�l
//#### �o�͏����@
int COteCS::output() {          

//### ���u�����PLC�ւ̏o�͏���
	//##�A�N�Z�X�p�|�C���^�Z�b�g
	LPST_PLC_WBUF_HHGG38 pPcWBuf = (LPST_PLC_WBUF_HHGG38)pOteCsInf->buf_opepnl_write;//�������݃o�b�t�@�|�C���^
	LPST_PC_U_BODY pBody = (LPST_PC_U_BODY)&pOteCCInf->st_msg_pc_u_rcv.body;

	//##������@�����p�M��
	pPcWBuf->pc_healthy = ote_helthy++;					//PC�w���V�[�l
	//�ڑ����N���[��ID
	pPcWBuf->crane_id = (INT16)(pOteCCInf->id_conected_crane & 0x0000FFFF);	//�ڑ���N���[��ID	
	
	//##GOT�̏�Ď�
	LPST_OTE_PC_FLTS_SET pFltSet = (LPST_OTE_PC_FLTS_SET)&pBody->faults_set;

	int index = 0,nflt=0;

	if (!(pOteCsInf->ote_error & FLTS_MASK_ERR_CPC_RPC_COMM)) {//����PC�Ƃ̒ʐM�f�ŃX���[
		nflt = pFltSet->set_plc_count;
		if (nflt >= N_OTE_OPE_PLC_FAULT_BUF)nflt = N_OTE_OPE_PLC_FAULT_BUF;
		//PLC �t�H���g
		for (int i = 0; i < nflt; i++, index++)((LPST_PLC_WBUF_HHGG38)
			pOteCsInf->buf_opepnl_write)->fault_code[index] = pFltSet->codes_plc[i] + 300;

		//����PC �t�H���g
		nflt = pFltSet->set_pc_count;
		if (nflt + index >= N_OTE_OPE_PLC_FAULT_BUF) nflt = N_OTE_OPE_PLC_FAULT_BUF - index;
		for (int i = 0; i < nflt; i++, index++)
			((LPST_PLC_WBUF_HHGG38)	pOteCsInf->buf_opepnl_write)->fault_code[index] = pFltSet->codes_pc[i] + 300;
	}
	//���uPC �t�H���g
	for (int i = 0; i < pOteCsInf->rpc_flt_count; i++, index++) {
		((LPST_PLC_WBUF_HHGG38)pOteCsInf->buf_opepnl_write)->fault_code[index] = pOteCsInf->rpc_flt_codes[index];
	}

	//�c���0�N���A
	for(;index< N_OTE_OPE_PLC_FAULT_BUF;index++) 
		((LPST_PLC_WBUF_HHGG38)pOteCsInf->buf_opepnl_write)->fault_code[index] = 0;

	//##GOT�^�]�Ď�
	pPcWBuf->mh_hight	= pBody->st_axis_set[ID_HOIST].pos_fb;	//�g��
	pPcWBuf->mh_load	= pBody->st_load_stat[0].m/10.0;		//�׏d0.1t�P�ʁ�t�P��
	pPcWBuf->r			= pBody->st_axis_set[ID_BOOM_H].pos_fb;	//���a
	pPcWBuf->wind_spd	= pBody->wind_spd;						//����
	
	//##GOT��ԊĎ�
	pPcWBuf->mh_set		= pBody->st_axis_set[ID_HOIST];			//�努
	pPcWBuf->bh_set		= pBody->st_axis_set[ID_BOOM_H];		//����
	pPcWBuf->sl_set		= pBody->st_axis_set[ID_SLEW];			//����
	pPcWBuf->gt_set		= pBody->st_axis_set[ID_GANTRY];		//���s

	//##����u���[�LFB�M���Z�b�g
	pPcWBuf->sl_brk_fb1 = pBody->sl_brk_fb[0];				//����u���[�LFB1
	pPcWBuf->sl_brk_fb2 = pBody->sl_brk_fb[1];				//����u���[�LFB2
	pPcWBuf->sl_brk_fb3 = pBody->sl_brk_fb[2];				//����u���[�LFB3
	pPcWBuf->sl_brk_fb4 = pBody->sl_brk_fb[3];				//����u���[�LFB4

//### ����PC�ւ̏o�͏���

	pOteCsInf->st_body.ote_err[0] = pOteCsInf->ote_error;	//���u����PC���o�̏�Z�b�g
//##�@���M�o�b�t�@���e�o�́iCS�Ŏ��W�������[�U������e�j�����L�������ɃR�s�[
	memcpy_s(&pOteCsInf->st_body, sizeof(ST_OTE_U_BODY), &st_work.st_body, sizeof(ST_OTE_U_BODY));

	return STAT_OK;
}

int COteCS::close() {
	delete pPad;
	delete st_obj.pad_mh;
	delete st_obj.pad_bh;
	delete st_obj.pad_sl;
	delete st_obj.pad_gt;
	delete st_obj.pad_ah;
	return 0;
}

/****************************************************************************/
/*   ���j�^�E�B���h�E									                    */
/****************************************************************************/
static wostringstream monwos;

LRESULT CALLBACK COteCS::Mon1Proc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
	switch (msg)
	{
	case WM_CREATE: {
		InitCommonControls();//�R�����R���g���[��������
		HINSTANCE hInst = (HINSTANCE)GetModuleHandle(0);
		//�E�B���h�E�ɃR���g���[���ǉ�
		st_mon1.hctrl[OTE_CS_ID_MON1_STATIC_MSG] = CreateWindowW(TEXT("STATIC"), st_mon1.text[OTE_CS_ID_MON1_STATIC_MSG], WS_CHILD | WS_VISIBLE | SS_LEFT,
			st_mon1.pt[OTE_CS_ID_MON1_STATIC_MSG].x, st_mon1.pt[OTE_CS_ID_MON1_STATIC_MSG].y,
			st_mon1.sz[OTE_CS_ID_MON1_STATIC_MSG].cx, st_mon1.sz[OTE_CS_ID_MON1_STATIC_MSG].cy,
			hWnd, (HMENU)(OTE_CS_ID_MON1_CTRL_BASE + OTE_CS_ID_MON1_STATIC_MSG), hInst, NULL);

		st_mon1.hctrl[OTE_CS_ID_MON1_CB_VIB_ACT] = CreateWindowW(TEXT("BUTTON"), st_mon1.text[OTE_CS_ID_MON1_CB_VIB_ACT], WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX ,
			st_mon1.pt[OTE_CS_ID_MON1_CB_VIB_ACT].x, st_mon1.pt[OTE_CS_ID_MON1_CB_VIB_ACT].y, st_mon1.sz[OTE_CS_ID_MON1_CB_VIB_ACT].cx, st_mon1.sz[OTE_CS_ID_MON1_CB_VIB_ACT].cy,
			hWnd, (HMENU)(OTE_CS_ID_MON1_CTRL_BASE + OTE_CS_ID_MON1_CB_VIB_ACT), hInst, NULL);
			
		// �X���C�_�[�쐬�i�����[�^�[�j
		st_mon1.hctrl[OTE_CS_ID_MON1_SLIDER_VIB_L] = CreateWindowEx(0, TRACKBAR_CLASS, st_mon1.text[OTE_CS_ID_MON1_SLIDER_VIB_L],
			WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS,
			st_mon1.pt[OTE_CS_ID_MON1_SLIDER_VIB_L].x, st_mon1.pt[OTE_CS_ID_MON1_SLIDER_VIB_L].y,
			st_mon1.sz[OTE_CS_ID_MON1_SLIDER_VIB_L].cx, st_mon1.sz[OTE_CS_ID_MON1_SLIDER_VIB_L].cy,
			hWnd, (HMENU)(OTE_CS_ID_MON1_CTRL_BASE + OTE_CS_ID_MON1_SLIDER_VIB_L), hInst, nullptr);
		
		SendMessage(st_mon1.hctrl[OTE_CS_ID_MON1_SLIDER_VIB_L], TBM_SETRANGE, TRUE, MAKELPARAM(0, 100));
		SendMessage(st_mon1.hctrl[OTE_CS_ID_MON1_SLIDER_VIB_L], TBM_SETPOS, TRUE, 0);

		// �X���C�_�[�쐬�i�E���[�^�[�j
		st_mon1.hctrl[OTE_CS_ID_MON1_SLIDER_VIB_R] = CreateWindowEx(0, TRACKBAR_CLASS, st_mon1.text[OTE_CS_ID_MON1_SLIDER_VIB_R],
			WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS,
			st_mon1.pt[OTE_CS_ID_MON1_SLIDER_VIB_R].x, st_mon1.pt[OTE_CS_ID_MON1_SLIDER_VIB_R].y,
			st_mon1.sz[OTE_CS_ID_MON1_SLIDER_VIB_R].cx, st_mon1.sz[OTE_CS_ID_MON1_SLIDER_VIB_R].cy,
			hWnd, (HMENU)(OTE_CS_ID_MON1_CTRL_BASE + OTE_CS_ID_MON1_SLIDER_VIB_R), hInst, nullptr);

		SendMessage(st_mon1.hctrl[OTE_CS_ID_MON1_SLIDER_VIB_R], TBM_SETRANGE, TRUE, MAKELPARAM(0, 100));
		SendMessage(st_mon1.hctrl[OTE_CS_ID_MON1_SLIDER_VIB_R], TBM_SETPOS, TRUE, 0);
		
		//�\���X�V�p�^�C�}�[
		SetTimer(hWnd, OTE_CS_ID_MON1_TIMER, st_mon1.timer_ms, NULL);

		break;
	}
	case WM_COMMAND: {
		int wmId = LOWORD(wp);
		// �I�����ꂽ���j���[�̉��:
		switch (wmId)
		{
		case OTE_CS_ID_MON1_CTRL_BASE + OTE_CS_ID_MON1_CB_VIB_ACT: {
			if (pPad == NULL)break;
			if(BST_UNCHECKED== SendMessage(st_mon1.hctrl[OTE_CS_ID_MON1_CB_VIB_ACT], BM_GETCHECK, 0, 0))
				pPad->VibrateController(pPad->controllerId, 0, 0);
			break;
		}
		default:
			return DefWindowProc(hWnd, msg, wp, lp);
		}
	}break;
	case WM_TIMER: {

		if (pPad != NULL) {
			monwos.str(L"");
			if(pPad->error_code)
			monwos << "Controller " << pPad->controllerId << " is  Error:" << pPad->error_code <<"\n" ;
			else
				monwos << "Controller " << pPad->controllerId << " is connected\n";
			monwos << "  Buttons: " << std::hex << pPad->state.Gamepad.wButtons << std::dec << "\n";
			monwos << "  Left Thumb: (" << pPad->state.Gamepad.sThumbLX << ", " << pPad->state.Gamepad.sThumbLY << ")\n";
			monwos << "  Right Thumb: (" << pPad->state.Gamepad.sThumbRX << ", " << pPad->state.Gamepad.sThumbRY << ")\n";
			monwos << "  Triggers: L=" << (int)pPad->state.Gamepad.bLeftTrigger << ", R=" << (int)pPad->state.Gamepad.bRightTrigger << "\n\n";

			monwos << "PAD Notch MH:" << st_obj.pad_mh->get_notch() << " BH:" << st_obj.pad_bh->get_notch();
			monwos << " SL:" << st_obj.pad_sl->get_notch() << " GT:" << st_obj.pad_gt->get_notch();

		}
		SetWindowText(st_mon1.hctrl[OTE_CS_ID_MON1_STATIC_MSG], monwos.str().c_str());

	}break;
	case WM_HSCROLL: {
		//�Q�[���p�b�h�̃o�C�u�`�F�b�N
		if (pPad == NULL)break;
		if (BST_CHECKED == SendMessage(st_mon1.hctrl[OTE_CS_ID_MON1_CB_VIB_ACT], BM_GETCHECK, 0, 0)) {
			if ((HWND)lp == st_mon1.hctrl[OTE_CS_ID_MON1_SLIDER_VIB_L] || (HWND)lp == st_mon1.hctrl[OTE_CS_ID_MON1_SLIDER_VIB_R]) {
				int left = SendMessage(st_mon1.hctrl[OTE_CS_ID_MON1_SLIDER_VIB_L], TBM_GETPOS, 0, 0);
				int right = SendMessage(st_mon1.hctrl[OTE_CS_ID_MON1_SLIDER_VIB_R], TBM_GETPOS, 0, 0);
			
				left *= 65535 / 100; right *= 65535 / 100;
				
				pPad->VibrateController(pPad->controllerId, (WORD)left, (WORD)right);
			}
		}
		else {
			pPad->VibrateController(pPad->controllerId, 0, 0);
		}
	}break;
	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
	}break;
	case WM_DESTROY: {
		st_mon1.hwnd_mon = NULL;
		KillTimer(hWnd, OTE_CS_ID_MON1_TIMER);
	}break;
	default:
		return DefWindowProc(hWnd, msg, wp, lp);
	}
	return S_OK;
};

static bool is_write_req_turn = false;//�������ݗv�����M�̏��Ԃ�true
static int n_page_w = 5, i_page_w = 0, i_ref_w = 0;//���j�^��ʕ\���p
static int n_page_r = 5, i_page_r = 0, i_ref_r = 0;//���j�^��ʕ\���p

LRESULT CALLBACK COteCS::Mon2Proc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
	switch (msg)
	{
	case WM_CREATE: {
		InitCommonControls();//�R�����R���g���[��������
		HINSTANCE hInst = (HINSTANCE)GetModuleHandle(0);

		//�E�B���h�E�ɃR���g���[���ǉ�
		//STATIC,LABEL
		for (int i = OTE_CS_ID_MON2_STATIC_MSG; i <= OTE_CS_ID_MON2_STATIC_RES_W; i++) {
			st_mon2.hctrl[i] = CreateWindowW(TEXT("STATIC"), st_mon2.text[i], WS_CHILD | WS_VISIBLE | SS_LEFT,
				st_mon2.pt[i].x, st_mon2.pt[i].y, st_mon2.sz[i].cx, st_mon2.sz[i].cy,
				hWnd, (HMENU)(OTE_CS_ID_MON2_CTRL_BASE + i), hInst, NULL);
		}
		//PB
		for (int i = OTE_CS_ID_MON2_PB_R_BLOCK_SEL; i <= OTE_CS_ID_MON2_PB_DISP_DEC_SEL; i++) {
			st_mon2.hctrl[i] = CreateWindowW(TEXT("BUTTON"), st_mon2.text[i], WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_PUSHLIKE,
				st_mon2.pt[i].x, st_mon2.pt[i].y, st_mon2.sz[i].cx, st_mon2.sz[i].cy,
				hWnd, (HMENU)(OTE_CS_ID_MON2_CTRL_BASE + i), hInst, NULL);
		}

		UINT rtn = SetTimer(hWnd, OTE_CS_ID_MON2_TIMER, OTE_CS_PRM_MON2_TIMER_MS, NULL);

		//UINT rtn = SetTimer(hWnd, OTE_CS_ID_MON2_TIMER, 10, NULL);

				//IF�f�[�^�\���y�[�W���v�Z
		n_page_w = OTE_MC_SIZE_W_WRITE / (OTE_CS_MON2_MSG_DISP_N__DATAROW * OTE_CS_MON2_MSG_DISP_N_DATA_COLUMN);
		if (OTE_MC_SIZE_W_WRITE % (OTE_CS_MON2_MSG_DISP_N__DATAROW * OTE_CS_MON2_MSG_DISP_N_DATA_COLUMN)) n_page_w++;
		n_page_r = OTE_MC_SIZE_W_READ / (OTE_CS_MON2_MSG_DISP_N__DATAROW * OTE_CS_MON2_MSG_DISP_N_DATA_COLUMN);
		if (OTE_MC_SIZE_W_READ % (OTE_CS_MON2_MSG_DISP_N__DATAROW * OTE_CS_MON2_MSG_DISP_N_DATA_COLUMN)) n_page_r++;

		monwos.str(L""); monwos << L"R:" << i_page_r + 1 << L"/" << n_page_r;
		SetWindowText(st_mon2.hctrl[OTE_CS_ID_MON2_PB_R_BLOCK_SEL], monwos.str().c_str());

		monwos.str(L""); monwos << L"W:" << i_page_w + 1 << L"/" << n_page_w;
		SetWindowText(st_mon2.hctrl[OTE_CS_ID_MON2_PB_W_BLOCK_SEL], monwos.str().c_str());


		st_mon2.msg_disp_mode = OTE_CS_MON2_MSG_DISP_HEX;
		monwos.str(L""); monwos << L"�\����";
		SetWindowText(st_mon2.hctrl[OTE_CS_ID_MON2_PB_MSG_DISP_SEL], monwos.str().c_str());
		monwos.str(L""); monwos << L"16�i";
		SetWindowText(st_mon2.hctrl[OTE_CS_ID_MON2_PB_DISP_DEC_SEL], monwos.str().c_str());
	}break;
	case WM_TIMER: {
		if (pMCSock == NULL)break;
		//3E�t�H�[�}�b�g D�f�o�C�X�������ݗv�����M
		if (is_write_req_turn) {//�������ݗv�����M
			st_mon2.wo_req_w.str(L"");
			//3E�t�H�[�}�b�g D�f�o�C�X�������ݗv�����M
			if (pMCSock->send_write_req_D_3E(pOteCsInf->buf_opepnl_write) != S_OK) {
				st_mon2.wo_req_w << L"ERROR : send_read_req_D_3E()\n";
			}
			else snd_count_plc_w++;

			if ((st_mon2.msg_disp_mode != OTE_CS_MON2_MSG_DISP_OFF) && st_mon2.is_monitor_active) {

				st_mon2.wo_req_w << L"Sw>>"
					<< L"#sub:" << std::hex << pMCSock->mc_req_msg_w.subcode
					<< L"#serial:" << pMCSock->mc_req_msg_w.serial
					<< L"#NW:" << pMCSock->mc_req_msg_w.nNW
					<< L"#PC:" << pMCSock->mc_req_msg_w.nPC
					<< L"#UIO:" << pMCSock->mc_req_msg_w.nUIO
					<< L"#Ucd:" << pMCSock->mc_req_msg_w.nUcode
					<< L"#len:" << pMCSock->mc_req_msg_w.len
					<< L"#tm:" << pMCSock->mc_req_msg_w.timer
					<< L"#com:" << pMCSock->mc_req_msg_w.com
					<< L"#scom:" << pMCSock->mc_req_msg_w.scom << L"\n"
					<< L"#d_no:" << pMCSock->mc_req_msg_w.d_no
					<< L"#d_code:" << pMCSock->mc_req_msg_w.d_code
					<< L"#n_dev:" << pMCSock->mc_req_msg_w.n_device << L"\n";

				//st_mon2.wo_req_w << L"PC Helthy:" << pOteCsInf->buf_opepnl_write[0];
				//�f�[�^����1�y�[�W10WORD�@4�s�Őؑ֕\��
				for (int i = 0; i < OTE_CS_MON2_MSG_DISP_N__DATAROW; i++) {
					int no = OTE_MC_ADDR_W_WRITE + i_page_w * OTE_CS_MON2_MSG_DISP_N_DATA_COLUMN * OTE_CS_MON2_MSG_DISP_N__DATAROW + i * OTE_CS_MON2_MSG_DISP_N_DATA_COLUMN;
					st_mon2.wo_req_w << L"D" << dec << no << L" |";
					if (st_mon2.msg_disp_mode == OTE_CS_MON2_MSG_DISP_HEX)	st_mon2.wo_req_w << hex;
					for (int j = 0; j < OTE_CS_MON2_MSG_DISP_N_DATA_COLUMN; j++) {
						if (st_mon2.msg_disp_mode == OTE_CS_MON2_MSG_DISP_HEX)
							st_mon2.wo_req_w << std::setw(4) << std::setfill(L'0') << pOteCsInf->buf_opepnl_write[i_ref_w + i * OTE_CS_MON2_MSG_DISP_N_DATA_COLUMN + j] << L"|";
						else
							st_mon2.wo_req_w << std::setw(6) << std::setfill(L' ') << pOteCsInf->buf_opepnl_write[i_ref_w + i * OTE_CS_MON2_MSG_DISP_N_DATA_COLUMN + j] << L"|";
					}
					st_mon2.wo_req_w << L"\n";
				}


				SetWindowText(st_mon2.hctrl[OTE_CS_ID_MON2_STATIC_REQ_W], st_mon2.wo_req_w.str().c_str());
			}

			QueryPerformanceCounter(&start_count_w);  // �������ݗv�����M���J�E���g�l��荞��
			is_write_req_turn = false;
		}
		else {
			st_mon2.wo_req_r.str(L"");
			//�ǂݏo���v�����M
			if (pMCSock->send_read_req_D_3E() != S_OK) {
				st_mon2.wo_req_r << L"ERROR : send_read_req_D_3E()";
			}
			else snd_count_plc_r++;

			if ((st_mon2.msg_disp_mode != OTE_CS_MON2_MSG_DISP_OFF) && st_mon2.is_monitor_active) {
				st_mon2.wo_req_r << L"Sr>>"
					<< L"#sub:" << std::hex << pMCSock->mc_req_msg_r.subcode
					<< L"#serial:" << pMCSock->mc_req_msg_r.serial
					<< L"#NW:" << pMCSock->mc_req_msg_r.nNW
					<< L"#PC:" << pMCSock->mc_req_msg_r.nPC
					<< L"#UIO:" << pMCSock->mc_req_msg_r.nUIO
					<< L"#Ucd:" << pMCSock->mc_req_msg_r.nUcode
					<< L"#len:" << pMCSock->mc_req_msg_r.len
					<< L"#tm:" << pMCSock->mc_req_msg_r.timer
					<< L"#com:" << pMCSock->mc_req_msg_r.com
					<< L"#scom:" << pMCSock->mc_req_msg_r.scom
					<< L"#d_no:" << pMCSock->mc_req_msg_r.d_no
					<< L"#d_code:" << pMCSock->mc_req_msg_r.d_code
					<< L"#n_dev:" << pMCSock->mc_req_msg_r.n_device;

				SetWindowText(st_mon2.hctrl[OTE_CS_ID_MON2_STATIC_REQ_R], st_mon2.wo_req_r.str().c_str());
			}

			QueryPerformanceCounter(&start_count_r);  // �������ݗv�����M���J�E���g�l��荞��
			is_write_req_turn = true;
		}

		//���ʕ\��
		if (st_mon2.is_monitor_active) {
			//�J�E���^�\��
			monwos.str(L""); monwos << L"RCV:R " << rcv_count_plc_r
				<< L"  W " << rcv_count_plc_w
				<< L"    SND:R " << snd_count_plc_r
				<< L"  W " << snd_count_plc_w
				<< L"    ERR:R " << rcv_errcount_plc_r
				<< L"  W " << rcv_errcount_plc_w
				<< L"    �x����s:R " << res_delay_max_r
				<< L"  W " << res_delay_max_w;

			SetWindowText(hWnd, monwos.str().c_str());

			SOCKADDR_IN	addr;
			if (pMCSock != NULL) {
				addr = pMCSock->get_addrin_rcv(); monwos.str(L"");
				monwos << L"IP R:" << addr.sin_addr.S_un.S_un_b.s_b1 << L"." << addr.sin_addr.S_un.S_un_b.s_b2 << L"." << addr.sin_addr.S_un.S_un_b.s_b3 << L"." << addr.sin_addr.S_un.S_un_b.s_b4 << L":"
					<< htons(addr.sin_port) << L" ";
				addr = pMCSock->get_addrin_snd();
				monwos << L" S:" << addr.sin_addr.S_un.S_un_b.s_b1 << L"." << addr.sin_addr.S_un.S_un_b.s_b2 << L"." << addr.sin_addr.S_un.S_un_b.s_b3 << L"." << addr.sin_addr.S_un.S_un_b.s_b4 << L":"
					<< htons(addr.sin_port) << L" ";
				addr = pMCSock->get_addrin_from();
				monwos << L" F:" << addr.sin_addr.S_un.S_un_b.s_b1 << L"." << addr.sin_addr.S_un.S_un_b.s_b2 << L"." << addr.sin_addr.S_un.S_un_b.s_b3 << L"." << addr.sin_addr.S_un.S_un_b.s_b4 << L":"
					<< htons(addr.sin_port) << L" ";
				SetWindowText(st_mon2.hctrl[OTE_CS_ID_MON2_STATIC_INF], monwos.str().c_str());
			}
		}
	}break;
	case WM_COMMAND: {
		int wmId = LOWORD(wp);
		// �I�����ꂽ���j���[�̉��:
		switch (wmId - OTE_CS_ID_MON2_CTRL_BASE)
		{
		case OTE_CS_ID_MON2_PB_R_BLOCK_SEL: {
			if (++i_page_r >= n_page_r)i_page_r = 0;
			monwos.str(L""); monwos << L"R:" << i_page_r + 1 << L"/" << n_page_r;
			SetWindowText(st_mon2.hctrl[OTE_CS_ID_MON2_PB_R_BLOCK_SEL], monwos.str().c_str());
			i_ref_r = i_page_r * OTE_CS_MON2_MSG_DISP_N__DATAROW * OTE_CS_MON2_MSG_DISP_N_DATA_COLUMN;
		}break;
		case OTE_CS_ID_MON2_PB_W_BLOCK_SEL: {
			if (++i_page_w >= n_page_w)i_page_w = 0;
			monwos.str(L""); monwos << L"W:" << i_page_w + 1 << L"/" << n_page_w;
			SetWindowText(st_mon2.hctrl[OTE_CS_ID_MON2_PB_W_BLOCK_SEL], monwos.str().c_str());
			i_ref_w = i_page_w * OTE_CS_MON2_MSG_DISP_N__DATAROW * OTE_CS_MON2_MSG_DISP_N_DATA_COLUMN;
		}break;
		case OTE_CS_ID_MON2_PB_MSG_DISP_SEL: {
			if (st_mon2.msg_disp_mode != OTE_CS_MON2_MSG_DISP_OFF) {
				st_mon2.msg_disp_mode = OTE_CS_MON2_MSG_DISP_OFF;
				monwos.str(L""); monwos << L"��\��";
				SetWindowText(st_mon2.hctrl[OTE_CS_ID_MON2_PB_MSG_DISP_SEL], monwos.str().c_str());
				monwos.str(L""); monwos << L"-:";
				SetWindowText(st_mon2.hctrl[OTE_CS_ID_MON2_PB_DISP_DEC_SEL], monwos.str().c_str());
			}
			else {
				st_mon2.msg_disp_mode = OTE_CS_MON2_MSG_DISP_HEX;
				monwos.str(L""); monwos << L"�\����";
				SetWindowText(st_mon2.hctrl[OTE_CS_ID_MON2_PB_MSG_DISP_SEL], monwos.str().c_str());
				monwos.str(L""); monwos << L"16�i";
				SetWindowText(st_mon2.hctrl[OTE_CS_ID_MON2_PB_DISP_DEC_SEL], monwos.str().c_str());
			}
		}break;
		case OTE_CS_ID_MON2_PB_DISP_DEC_SEL: {
			if (st_mon2.msg_disp_mode != OTE_CS_MON2_MSG_DISP_OFF) {
				if (st_mon2.msg_disp_mode == OTE_CS_MON2_MSG_DISP_HEX) {
					st_mon2.msg_disp_mode = OTE_CS_MON2_MSG_DISP_DEC;
					monwos.str(L""); monwos << L"10�i";
					SetWindowText(st_mon2.hctrl[OTE_CS_ID_MON2_PB_DISP_DEC_SEL], monwos.str().c_str());
				}
				else {
					st_mon2.msg_disp_mode = OTE_CS_MON2_MSG_DISP_HEX;
					monwos.str(L""); monwos << L"16�i";
					SetWindowText(st_mon2.hctrl[OTE_CS_ID_MON2_PB_DISP_DEC_SEL], monwos.str().c_str());
				}
			}
		}break;
		default:
			return DefWindowProc(hWnd, msg, wp, lp);
		}
	}break;
	case ID_SOCK_MC_OTE_CS://MC�\�P�b�g��M�C�x���g
	{
		if (pMCSock == NULL)break;
		int nEvent = WSAGETSELECTEVENT(lp);
		st_mon2.wo_res_r.str(L"");
		st_mon2.wo_res_w.str(L"");
		switch (nEvent) {
		case FD_READ: {
			UINT nRtn = pMCSock->rcv_msg_3E(pOteCsInf->buf_opepnl_read);
			if (nRtn == MC_RES_READ) {//�ǂݏo������
				rcv_count_plc_r++;
				if ((st_mon2.msg_disp_mode != OTE_CS_MON2_MSG_DISP_OFF) && st_mon2.is_monitor_active) {
					st_mon2.wo_res_r	<< L"Rr>>"
						<< L"#sub:"		<< std::hex << pMCSock->mc_res_msg_r.subcode
						<< L"#serial:"	<< pMCSock->mc_res_msg_r.serial
						<< L"#NW:"		<< pMCSock->mc_res_msg_r.nNW
						<< L"#PC:"		<< pMCSock->mc_res_msg_r.nPC
						<< L"#UIO:"		<< pMCSock->mc_res_msg_r.nUIO
						<< L"#Ucd:"		<< pMCSock->mc_res_msg_r.nUcode
						<< L"#len:"		<< pMCSock->mc_res_msg_r.len
						<< L"#end:"		<< pMCSock->mc_res_msg_r.endcode << L"\n";

	//				st_mon2.wo_res_r	<< L"PLC HEALTHY:" << pOteCsInf->buf_opepnl_read[0] << L"\n";
					for (int i = 0; i < OTE_CS_MON2_MSG_DISP_N__DATAROW; i++) {
						int no = OTE_MC_ADDR_W_READ + i_page_r * OTE_CS_MON2_MSG_DISP_N_DATA_COLUMN * OTE_CS_MON2_MSG_DISP_N__DATAROW + i * OTE_CS_MON2_MSG_DISP_N_DATA_COLUMN;
						st_mon2.wo_res_r << L"D" << dec << no << L" |";
						if (st_mon2.msg_disp_mode == OTE_CS_MON2_MSG_DISP_HEX)	st_mon2.wo_res_r << hex;
						for (int j = 0; j < OTE_CS_MON2_MSG_DISP_N_DATA_COLUMN; j++) {
							if (st_mon2.msg_disp_mode == OTE_CS_MON2_MSG_DISP_HEX)
								st_mon2.wo_res_r << std::setw(4) << std::setfill(L'0') << pOteCsInf->buf_opepnl_read[i_ref_r + i * OTE_CS_MON2_MSG_DISP_N_DATA_COLUMN + j] << L"|";
							else
								st_mon2.wo_res_r << std::setw(6) << std::setfill(L' ') << pOteCsInf->buf_opepnl_read[i_ref_r + i * OTE_CS_MON2_MSG_DISP_N_DATA_COLUMN + j] << L"|";
						}
						st_mon2.wo_res_r << L"\n";
					}

					SetWindowText(st_mon2.hctrl[OTE_CS_ID_MON2_STATIC_RES_R], st_mon2.wo_res_r.str().c_str());

					QueryPerformanceCounter(&end_count_r);    // ���݂̃J�E���g��
					LONGLONG lspan = (end_count_r.QuadPart - start_count_r.QuadPart) * 1000000L / frequency.QuadPart;// ���Ԃ̊Ԋu[usec]
					if (res_delay_max_r < lspan) res_delay_max_r = lspan;
					if (rcv_count_plc_r % 40 == 0) {
						res_delay_max_r = 0;
					}
				}
			}
			else if (nRtn == MC_RES_WRITE) {

				rcv_count_plc_w++;
				if ((st_mon2.msg_disp_mode != OTE_CS_MON2_MSG_DISP_OFF) && st_mon2.is_monitor_active) {

					st_mon2.wo_res_w << L"Rw>>"
						<< L"#sub:" << std::hex << pMCSock->mc_res_msg_w.subcode
						<< L"#serial:" << pMCSock->mc_res_msg_w.serial
						<< L"#NW:" << pMCSock->mc_res_msg_w.nNW
						<< L"#PC:" << pMCSock->mc_res_msg_w.nPC
						<< L"#UIO:" << pMCSock->mc_res_msg_w.nUIO
						<< L"#Ucd:" << pMCSock->mc_res_msg_w.nUcode
						<< L"#len:" << pMCSock->mc_res_msg_w.len
						<< L"#end:" << pMCSock->mc_res_msg_w.endcode;
				}
				SetWindowText(st_mon2.hctrl[OTE_CS_ID_MON2_STATIC_RES_W], st_mon2.wo_res_w.str().c_str());

				QueryPerformanceCounter(&end_count_w);    // ���݂̃J�E���g��
				LONGLONG lspan = (end_count_w.QuadPart - start_count_w.QuadPart) * 1000000L / frequency.QuadPart;// ���Ԃ̊Ԋu[usec]
				if (res_delay_max_w < lspan) res_delay_max_w = lspan;
				if (rcv_count_plc_w % 40 == 0) {
					res_delay_max_w = 0;
				}
			}
			else {
				int err_code = WSAGetLastError();
				if (is_write_req_turn) {
					st_mon2.wo_res_r << L"PLC READ RES_ERR  CODE:err_code" << err_code;
					rcv_errcount_plc_r++;
					SetWindowText(st_mon2.hctrl[OTE_CS_ID_MON2_STATIC_RES_R], st_mon2.wo_res_r.str().c_str());
				}
				else {
					st_mon2.wo_res_w << L"PLC WRITE RES_ERR  CODE:err_code" << err_code;
					rcv_errcount_plc_w++;
					SetWindowText(st_mon2.hctrl[OTE_CS_ID_MON2_STATIC_RES_W], st_mon2.wo_res_w.str().c_str());
				}
			}
		}break;
		default: break;
		}
	}break;
	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
	}break;
	case WM_DESTROY: {
		st_mon2.hwnd_mon = NULL;
	}break;
	default:
		return DefWindowProc(hWnd, msg, wp, lp);
	}
	return S_OK;
}
HWND COteCS::open_monitor_wnd(HWND h_parent_wnd, int id) {

	InitCommonControls();//�R�����R���g���[��������
	HINSTANCE hInst = GetModuleHandle(0);

	WNDCLASSEXW wcex;
	ATOM fb = RegisterClassExW(&wcex);

	if (id == BC_ID_MON1) {
		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = Mon1Proc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = hInst;
		wcex.hIcon = NULL;
		wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wcex.lpszMenuName = TEXT("CC_AG_MON1");
		wcex.lpszClassName = TEXT("CC_AG_MON1");
		wcex.hIconSm = NULL;

		ATOM fb = RegisterClassExW(&wcex);

		st_mon1.hwnd_mon = CreateWindowW(TEXT("CC_AG_MON1"), TEXT("CC_AG_MON1"), WS_OVERLAPPEDWINDOW,
			OTE_CS_MON1_WND_X, OTE_CS_MON1_WND_Y, OTE_CS_MON1_WND_W, OTE_CS_MON1_WND_H,
			h_parent_wnd, nullptr, hInst, nullptr);
		show_monitor_wnd(id);

		monwos.str(L"");
		if (st_mon1.hwnd_mon != NULL) monwos << L"Succeed : MON1 open";
		else                          monwos << L"!! Failed : MON1 open";
		msg2listview(monwos.str());

		return st_mon1.hwnd_mon;
	}
	else if (id == BC_ID_MON2) {//�ʐM�p�E�B���h�E
		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = Mon2Proc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = hInst;
		wcex.hIcon = NULL;
		wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wcex.lpszMenuName = TEXT("CC_AG_MON2");
		wcex.lpszClassName = TEXT("CC_AG_MON2");
		wcex.hIconSm = NULL;

		ATOM fb = RegisterClassExW(&wcex);

		st_mon2.hwnd_mon = CreateWindowW(TEXT("CC_AG_MON2"), TEXT("CC_AG_MON2"), WS_OVERLAPPEDWINDOW,
			OTE_CS_MON2_WND_X, OTE_CS_MON2_WND_Y, OTE_CS_MON2_WND_W, OTE_CS_MON2_WND_H,
			h_parent_wnd, nullptr, hInst, nullptr);

		//show_monitor_wnd(id);MON2�̓I�[�v�����\�����Ȃ�
		wos.str(L"");
		if (st_mon2.hwnd_mon != NULL) wos << L"Succeed : MON2 open";
		else                          wos << L"!! Failed : MON2 open";
		msg2listview(wos.str());

		return st_mon2.hwnd_mon;
	}
	else
	{
		return NULL;
	};

	return NULL;
}
void COteCS::close_monitor_wnd(int id) {
	wos.str(L"");
	if (id == BC_ID_MON1) {
		DestroyWindow(st_mon1.hwnd_mon);
		wos << L"MON1 closed";
	}
	else if (id == BC_ID_MON2) {
		DestroyWindow(st_mon2.hwnd_mon);
		wos << L"MON2 closed";
	}
	else;
	msg2listview(wos.str());
	return;
}
void COteCS::show_monitor_wnd(int id) {
	if ((id == BC_ID_MON1) && (st_mon1.hwnd_mon != NULL)) {
		ShowWindow(st_mon1.hwnd_mon, SW_SHOW);
		UpdateWindow(st_mon1.hwnd_mon);
		st_mon1.is_monitor_active = true;
	}
	else if ((id == BC_ID_MON2) && (st_mon2.hwnd_mon != NULL)) {
		ShowWindow(st_mon2.hwnd_mon, SW_SHOW);
		UpdateWindow(st_mon2.hwnd_mon);
		st_mon2.is_monitor_active = true;
	}
	else;
	return;
}
void COteCS::hide_monitor_wnd(int id) {
	if ((id == BC_ID_MON1) && (st_mon1.hwnd_mon != NULL)) {
		ShowWindow(st_mon1.hwnd_mon, SW_HIDE);
		st_mon1.is_monitor_active = false;
	}
	else if ((id == BC_ID_MON2) && (st_mon2.hwnd_mon != NULL)) {
		ShowWindow(st_mon2.hwnd_mon, SW_HIDE);
		st_mon2.is_monitor_active = false;
	}
	else;
	return;
}

/****************************************************************************/
/*   �^�X�N�ݒ�^�u�p�l���E�B���h�E�̃R�[���o�b�N�֐�                       */
/****************************************************************************/
LRESULT CALLBACK COteCS::PanelProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp) {

	switch (msg) {

	case WM_USER_TASK_REQ: {
		if (HIWORD(wp) == WM_USER_WPH_OPEN_IF_WND) {
			wos.str(L"");
			if (lp == BC_ID_MON1) st_mon1.hwnd_mon = open_monitor_wnd(hDlg, lp);
			if (lp == BC_ID_MON2) st_mon2.hwnd_mon = open_monitor_wnd(hDlg, lp);
		}
		else if (wp == WM_USER_WPH_CLOSE_IF_WND) 	close_monitor_wnd(lp);
		else;
	}break;

	case WM_COMMAND:
		switch (LOWORD(wp)) {
		case IDC_TASK_FUNC_RADIO1:
		case IDC_TASK_FUNC_RADIO2:
		case IDC_TASK_FUNC_RADIO3:
		case IDC_TASK_FUNC_RADIO4:
		case IDC_TASK_FUNC_RADIO5:
		case IDC_TASK_FUNC_RADIO6:
		{
			inf.panel_func_id = LOWORD(wp);
			set_panel_tip_txt();
			set_item_chk_txt();
			set_PNLparam_value(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
		}break;

		case IDC_TASK_ITEM_CHECK1: {
			switch (inf.panel_func_id) {
			case IDC_TASK_FUNC_RADIO4:
				set_item_chk_txt();
				break;
			default:break;
			}

		}break;
		case IDC_TASK_ITEM_CHECK2:
		case IDC_TASK_ITEM_CHECK3:
		case IDC_TASK_ITEM_CHECK4:
		case IDC_TASK_ITEM_CHECK5:
		case IDC_TASK_ITEM_CHECK6:
		{
			if (IsDlgButtonChecked(hDlg, LOWORD(wp)) == BST_CHECKED)
				inf.panel_act_chk[inf.panel_func_id - IDC_TASK_FUNC_RADIO1][LOWORD(wp) - IDC_TASK_ITEM_CHECK1] = true;
			else
				inf.panel_act_chk[inf.panel_func_id - IDC_TASK_FUNC_RADIO1][LOWORD(wp) - IDC_TASK_ITEM_CHECK1] = false;
		}break;

		case IDSET:
		{
			wstring wstr, wstr_tmp;

			wstr += L"Param 1(d):";
			int n = GetDlgItemText(hDlg, IDC_TASK_EDIT1, (LPTSTR)wstr_tmp.c_str(), 128);
			msg2listview(wstr);

		}break;
		case IDRESET:
		{
			set_PNLparam_value(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);

		}break;

		case IDC_TASK_MODE_RADIO0:
		{
			inf.mode_id = BC_ID_MODE0;
		}break;
		case IDC_TASK_MODE_RADIO1:
		{
			inf.mode_id = BC_ID_MODE1;
		}break;
		case IDC_TASK_MODE_RADIO2:
		{
			inf.mode_id = BC_ID_MODE2;
		}break;

		case IDC_TASK_MON_CHECK1:
		{
			if (IsDlgButtonChecked(hDlg, IDC_TASK_MON_CHECK1) == BST_CHECKED) {
				open_monitor_wnd(inf.hwnd_parent, BC_ID_MON1);
			}
			else {
				close_monitor_wnd(BC_ID_MON1);
			}
		}break;

		case IDC_TASK_MON_CHECK2: {

			if (IsDlgButtonChecked(hDlg, IDC_TASK_MON_CHECK2) == BST_CHECKED) {
				show_monitor_wnd(BC_ID_MON2);
			}
			else {
				hide_monitor_wnd(BC_ID_MON2);
			}
		}break;
		}
	}
	return 0;
};

///###	�^�u�p�l����ListView�Ƀ��b�Z�[�W���o��
void COteCS::msg2listview(wstring wstr) {

	const wchar_t* pwc; pwc = wstr.c_str();

	inf.hwnd_msglist = GetDlgItem(inf.hwnd_opepane, IDC_LIST1);
	LVITEM item;

	item.mask = LVIF_TEXT;
	item.pszText = (wchar_t*)pwc;								// �e�L�X�g
	item.iItem = inf.panel_msglist_count % BC_LISTVIEW_ROW_MAX;	// �ԍ�
	item.iSubItem = 1;											// �T�u�A�C�e���̔ԍ�
	ListView_SetItem(inf.hwnd_msglist, &item);

	SYSTEMTIME st; TCHAR tbuf[32];
	::GetLocalTime(&st);
	wsprintf(tbuf, L"%02d:%02d:%02d.%01d", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds / 100);

	item.pszText = tbuf;   // �e�L�X�g
	item.iSubItem = 0;											// �T�u�A�C�e���̔ԍ�
	ListView_SetItem(inf.hwnd_msglist, &item);

	//InvalidateRect(inf.hWnd_msgList, NULL, TRUE);
	inf.panel_msglist_count++;
	return;
}
void COteCS::set_PNLparam_value(float p1, float p2, float p3, float p4, float p5, float p6) {
	wstring wstr;
	wstr += std::to_wstring(p1); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT1), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p2); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT2), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p3); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT3), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p4); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT4), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p5); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT5), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p6); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT6), wstr.c_str());
}
//�^�u�p�l����Edit Box�����e�L�X�g��ݒ�
void COteCS::set_panel_tip_txt() {
	wstring wstr_type; wstring wstr;
	switch (inf.panel_func_id) {
	case IDC_TASK_FUNC_RADIO4: {
		wstr = L"1:-";
		SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_STATIC_ITEM3), wstr.c_str());
		wstr = L"2:-";
		SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_STATIC_ITEM4), wstr.c_str());
		wstr = L"3:-";
		SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_STATIC_ITEM5), wstr.c_str());
		wstr = L"4:-";
		SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_STATIC_ITEM6), wstr.c_str());
		wstr = L"5:-";
		SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_STATIC_ITEM7), wstr.c_str());
		wstr = L"6:-";
		SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_STATIC_ITEM8), wstr.c_str());
	}break;
	case IDC_TASK_FUNC_RADIO1:
	case IDC_TASK_FUNC_RADIO2:
	case IDC_TASK_FUNC_RADIO3:
	case IDC_TASK_FUNC_RADIO5:
	case IDC_TASK_FUNC_RADIO6:
	default:
	{
		wstr = L"1:-";
		SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_STATIC_ITEM3), wstr.c_str());
		wstr = L"2:-";
		SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_STATIC_ITEM4), wstr.c_str());
		wstr = L"3:-";
		SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_STATIC_ITEM5), wstr.c_str());
		wstr = L"4:-";
		SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_STATIC_ITEM6), wstr.c_str());
		wstr = L"5:-";
		SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_STATIC_ITEM7), wstr.c_str());
		wstr = L"6:-";
		SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_STATIC_ITEM8), wstr.c_str());
	}break;
	}
	return;
}
//�^�u�p�l����Function�{�^����Static�e�L�X�g��ݒ�
void COteCS::set_func_pb_txt() {
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO1, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO2, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO3, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO4, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO5, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO6, L"-");
	return;
}
//�^�u�p�l����Item chk�e�L�X�g��ݒ�
void COteCS::set_item_chk_txt() {
	wstring wstr_type; wstring wstr;
	switch (inf.panel_func_id) {
	case IDC_TASK_FUNC_RADIO4: {
		SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK1, L"-");
		SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK2, L"-");
		SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK3, L"-");
		SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK4, L"-");
		SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK5, L"-");
		SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK6, L"-");
	}break;
	case IDC_TASK_FUNC_RADIO1:
	case IDC_TASK_FUNC_RADIO2:
	case IDC_TASK_FUNC_RADIO3:
	case IDC_TASK_FUNC_RADIO5:
	case IDC_TASK_FUNC_RADIO6:
	default:
		SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK1, L"-");
		SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK2, L"-");
		SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK3, L"-");
		SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK4, L"-");
		SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK5, L"-");
		SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK6, L"-");
		break;
	}
	return;
}









