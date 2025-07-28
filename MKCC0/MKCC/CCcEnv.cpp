#include "CSockLib.H"
#include "CCcEnv.h"
#include "resource.h"
#include "framework.h"
#include "AUXEQ_DEF.H"
#include "CCrane.H"
#include "CFaults.h"
#include "phisics.h"

extern CSharedMem* pEnvInfObj;
extern CSharedMem* pPlcIoObj;
extern CSharedMem* pJobIoObj;
extern CSharedMem* pPolInfObj;
extern CSharedMem* pAgInfObj;
extern CSharedMem* pCsInfObj;
extern CSharedMem* pSimuStatObj;
extern CSharedMem* pOteInfObj;

//�\�P�b�g
static CSockUDP* pUSockCcEnv;	//���j�L���X�gOTE�ʐM��M�p
extern ST_DEVICE_CODE g_my_code;
extern CCrane* pCrane;

ST_ENV_MON1 CCcEnv::st_mon1;
ST_ENV_MON2 CCcEnv::st_mon2;

CSpec* CCcEnv::pspec;

ST_CC_ENV_INF CCcEnv::st_work;

//���L������
static LPST_CC_ENV_INF		pEnvInf;
static LPST_CC_PLC_IO		pPlcIo;
static LPST_JOB_IO			pJobIo;
static LPST_CC_POL_INF		pPolInf;
static LPST_CC_AGENT_INF	pAgentInf;
static LPST_CC_CS_INF		pCsInf;
static LPST_CC_SIM_INF		pSimInf;
static LPST_CC_OTE_INF		pOteInf;

static LONG rcv_count_u = 0, snd_count_u = 0;

/****************************************************************************/
/*   �f�t�H���g�֐�											                */
/****************************************************************************/

CCcEnv::CCcEnv() {
}
CCcEnv::~CCcEnv() {
}

HRESULT CCcEnv::initialize(LPVOID lpParam) {
	HRESULT hr = S_OK;

	pEnvInf		= (LPST_CC_ENV_INF)(pEnvInfObj->get_pMap());
	pPlcIo		= (LPST_CC_PLC_IO)(pPlcIoObj->get_pMap());
	pJobIo		= (LPST_JOB_IO)(pJobIoObj->get_pMap());
	pPolInf		= (LPST_CC_POL_INF)(pPolInfObj->get_pMap());
	pAgentInf	= (LPST_CC_AGENT_INF)(pAgInfObj->get_pMap());
	pCsInf		= (LPST_CC_CS_INF)(pCsInfObj->get_pMap());
	pSimInf		= (LPST_CC_SIM_INF)(pSimuStatObj->get_pMap());
	pOteInf		= (LPST_CC_OTE_INF)(pOteInfObj->get_pMap());

	if ((pEnvInf == NULL) || (pPlcIo == NULL) || (pJobIo == NULL) || (pPolInf == NULL) || (pAgentInf == NULL) || (pCsInf == NULL) || (pSimInf == NULL) || (pOteInf == NULL)) {
		hr = S_FALSE;
		wos.str(L""); wos << L"Initialize : SMEM NG"; msg2listview(wos.str());
		return hr;
	}

	//### �v�Z�p�p�����[�^������
	if(pCrane == NULL) {
		wos.str(L""); wos << L"Initialize : Crane Object NG"; msg2listview(wos.str());
		return S_FALSE;
	}
	pspec = pCrane->pSpec;

	//### �h�����p�����[�^������
	set_drum_param();
	//### �ΏۃN���[���p�֐��|�C���^�Z�b�g
	fp_set_drum_stat = set_drum_stat;

	//### IF�E�B���h�EOPEN
	WPARAM wp = MAKELONG(inf.index, WM_USER_WPH_OPEN_IF_WND);//HWORD:�R�}���h�R�[�h, LWORD:�^�X�N�C���f�b�N�X
	LPARAM lp = BC_ID_MON2;
	SendMessage(inf.hwnd_opepane, WM_USER_TASK_REQ, wp, lp);

	//### �ʐM�\�P�b�g�A�h���X�Z�b�g
	//##�C���X�^���X����
	pUSockCcEnv = new CSockUDP(ACCESS_TYPE_CLIENT, ID_SOCK_EVENT_CC_ENV_UNI);//#OTE���j�L���X�g��M
	//��M�A�h���X
	pUSockCcEnv->set_sock_addr(&pUSockCcEnv->addr_in_rcv, IP_ADDR_AUX_CLIENT, IP_PORT_AUX_CS_CLIENT);
	//���M��A�h���X
	pUSockCcEnv->set_sock_addr(&(pUSockCcEnv->addr_in_dst), IP_ADDR_AUX_SERVER, IP_PORT_AUX_CS_SERVER);

	Sleep(1000);
	if (st_mon2.hwnd_mon == NULL) {
		wos << L"Err(MON2 NULL Handle!!):";
		msg2listview(wos.str()); wos.str(L"");
		return S_FALSE;
	}
	//### �ʐM�\�P�b�g����/������
	//##WSA������
	wos.str(L"");
	if (pUSockCcEnv->Initialize() != S_OK) { wos << L"Err(IniWSA):" << pUSockCcEnv->err_msg.str(); err |= SOCK_NG_UNICAST;   hr = S_FALSE; }

	if (hr == S_FALSE)msg2listview(wos.str()); wos.str(L"");

	//##�\�P�b�g�\�P�b�g�����E�ݒ�
	//##���j�L���X�g
	if (pUSockCcEnv->init_sock(st_mon2.hwnd_mon, pUSockCcEnv->addr_in_rcv) != S_OK) {//init_sock():bind()���񓯊����܂Ŏ��{
		wos << L"CS U SockErr:" << pUSockCcEnv->err_msg.str(); err |= SOCK_NG_UNICAST; hr = S_FALSE;
	}
	else wos << L"CS U Socket init OK"; msg2listview(wos.str()); wos.str(L"");

	//���M���b�Z�[�W�w�b�_�ݒ�i���M����M�A�h���X�F��M��̐܂�Ԃ��p�j
	pEnvInf->st_msg_u_snd.head.sockaddr = pUSockCcEnv->addr_in_rcv;
	if (hr == S_FALSE) {
		pUSockCcEnv->Close();				//�\�P�b�g�N���[�Y
		close_monitor_wnd(BC_ID_MON2);		//�ʐM���j�^�N���[�Y
		wos.str(L""); wos << L"Initialize : SOCKET NG"; msg2listview(wos.str());
		return hr;
	};

	//###  �I�y���[�V�����p�l���ݒ�
	set_func_pb_txt();

	inf.panel_func_id = IDC_TASK_FUNC_RADIO1;
	SendMessage(GetDlgItem(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO1), BM_SETCHECK, BST_CHECKED, 0L);
	for (int i = 1; i < 6; i++)
		SendMessage(GetDlgItem(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO1 + i), BM_SETCHECK, BST_UNCHECKED, 0L);
	//���[�h�ݒ�0
	inf.mode_id = BC_ID_MODE0;
	SendMessage(GetDlgItem(inf.hwnd_opepane, IDC_TASK_MODE_RADIO0), BM_SETCHECK, BST_CHECKED, 0L);
	//���j�^�E�B���h�E�e�L�X�g	
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_MON_CHECK2, L"AUX IF");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_MODE_RADIO0, L"Product");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_MODE_RADIO1, L"Emulator");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_MODE_RADIO2, L"Simulator");

	set_item_chk_txt();
	set_panel_tip_txt();
	
	//���j�^2�I��CB��ԃZ�b�g	
	if (st_mon2.hwnd_mon != NULL)
		SendMessage(GetDlgItem(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK1), BM_SETCHECK, BST_CHECKED, 0L);
	else
		SendMessage(GetDlgItem(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK1), BM_SETCHECK, BST_UNCHECKED, 0L);

	CCcEnv* pEnvObj = (CCcEnv*)lpParam;
	int code = 0;
		
	st_work.aux_mode = FUNC_ACTIVE;

	st_work.device_code = g_my_code;

	plc_enable_hold = 0;

	return S_OK;
}

void CCcEnv::set_drum_param() {//�o�̓o�b�t�@�Z�b�g
	for(int i = 0; i < N_DRUM_LAYER; i++) {
		pEnvInf->Cdrm[ID_HOIST][i]	= (pspec->base_mh.Ddrm0 + (double)i	* pspec->base_mh.dDdrm) * PI180;
		pEnvInf->Cdrm[ID_BOOM_H][i] = (pspec->base_bh.Ddrm0 + (double)i * pspec->base_bh.dDdrm) * PI180;
		pEnvInf->Cdrm[ID_SLEW][i]	= (pspec->base_sl.Ddrm0 + (double)i * pspec->base_sl.dDdrm) * PI180;
		pEnvInf->Cdrm[ID_GANTRY][i] = (pspec->base_gt.Ddrm0 + (double)i * pspec->base_gt.dDdrm) * PI180;
		//�����努�h���� �w���ג��a��BH���g�p
		pEnvInf->Cdrm[ID_BH_HST][i] = (pspec->base_mh.Ddrm1 + (double)i * pspec->base_bh.dDdrm) * PI180;

		if(i == 0) {//1�w������
			pEnvInf->Ldrm[ID_HOIST][i]	= pEnvInf->Cdrm[ID_HOIST][i]	* pspec->base_mh.Ndmizo0;
			pEnvInf->Ldrm[ID_BOOM_H][i] = pEnvInf->Cdrm[ID_BOOM_H][i]	* pspec->base_bh.Ndmizo0;
			pEnvInf->Ldrm[ID_SLEW][i]	= pEnvInf->Cdrm[ID_SLEW][i]		* pspec->base_sl.Ndmizo0;
			pEnvInf->Ldrm[ID_GANTRY][i] = pEnvInf->Cdrm[ID_GANTRY][i]	* pspec->base_gt.Ndmizo0;
			//�����努�h���� 
			pEnvInf->Ldrm[ID_BH_HST][i] = pEnvInf->Cdrm[ID_BH_HST][i] * pspec->base_mh.Ndmizo1;//Ndmizo1�͈����努�h�����̍a��
		}
		else {
			pEnvInf->Ldrm[ID_HOIST][i]	= pEnvInf->Ldrm[ID_HOIST][i-1]		+pEnvInf->Cdrm[ID_HOIST][i] * pspec->base_mh.Ndmizo0;
			pEnvInf->Ldrm[ID_BOOM_H][i] = pEnvInf->Ldrm[ID_BOOM_H][i - 1]	+pEnvInf->Cdrm[ID_BOOM_H][i] * pspec->base_bh.Ndmizo0;
			pEnvInf->Ldrm[ID_SLEW][i]	= pEnvInf->Ldrm[ID_SLEW][i - 1]		+pEnvInf->Cdrm[ID_SLEW][i] * pspec->base_sl.Ndmizo0;
			pEnvInf->Ldrm[ID_GANTRY][i] = pEnvInf->Ldrm[ID_GANTRY][i - 1]	+pEnvInf->Cdrm[ID_GANTRY][i] * pspec->base_gt.Ndmizo0;
			pEnvInf->Ldrm[ID_BH_HST][i] = pEnvInf->Ldrm[ID_BH_HST][i - 1]	+pEnvInf->Cdrm[ID_BH_HST][i] * pspec->base_mh.Ndmizo1;
		}
	}

	pEnvInf->Lspan[ID_HOIST]	= pspec->base_mh.Lfull;
	pEnvInf->Lspan[ID_BOOM_H]	= pspec->base_bh.Lfull;
	pEnvInf->Lspan[ID_SLEW]		= pspec->base_sl.Lfull;
	pEnvInf->Lspan[ID_GANTRY]	= pspec->base_gt.Lfull;
	return;
}
HRESULT CCcEnv::set_drum_stat() {
	//#��]���Z�b�g
	//�努�h������]�@(abs fb - �v���Z�b�g�J�E���g�j/�h����1��]abs cnt + �v���Z�b�g�h������]��
	pEnvInf->crane_stat.nd[ID_HOIST].p = (pPlcIo->stat_mh.absocoder_fb- pspec->base_mh.CntAbsSet0)/ pspec->base_mh.CntAbsR + pspec->base_mh.NdrmAbsSet0;
	//�N���h������]�@�@(pg fb - �v���Z�b�g�J�E���g�j/�h����1��]pg cnt + �v���Z�b�g�h������]��
	pEnvInf->crane_stat.nd[ID_BOOM_H].p = (pPlcIo->stat_bh.hcount_fb - pspec->base_bh.CntPgSet0) / pspec->base_bh.CntPgDrumR + pspec->base_bh.NdrmPgSet0;
	//����h������]�@�@(pg fb - �v���Z�b�g�J�E���g�j/�h����1��]pg cnt + �v���Z�b�g�h������]��
	pEnvInf->crane_stat.nd[ID_SLEW].p = (pPlcIo->stat_sl.hcount_fb - pspec->base_sl.CntPgSet0) / pspec->base_sl.CntPgDrumR + pspec->base_sl.NdrmPgSet0;
	//���s�h������]�@(abs fb - �v���Z�b�g�J�E���g�j/�h����1��]abs cnt + �v���Z�b�g�h������]��
	pEnvInf->crane_stat.nd[ID_GANTRY].p = (pPlcIo->stat_gt.absocoder_fb - pspec->base_gt.CntAbsSet0) / pspec->base_gt.CntAbsR + pspec->base_gt.NdrmAbsSet0;

	//#��]���x�Z�b�g �}0.1���P�� �x�[�X���x��100���� inv fb/1000*��i��]��
	//�努
	pEnvInf->crane_stat.nd[ID_HOIST].v = (double)pPlcIo->stat_mh.inv_fb_v * pspec->base_mh.Rpm_rated/1000.0;
	//�N��
	pEnvInf->crane_stat.nd[ID_BOOM_H].v = (double)pPlcIo->stat_bh.inv_fb_v * pspec->base_bh.Rpm_rated / 1000.0;
	//����
	pEnvInf->crane_stat.nd[ID_SLEW].v = (double)pPlcIo->stat_sl.inv_fb_v * pspec->base_sl.Rpm_rated / 1000.0;
	//���s
	pEnvInf->crane_stat.nd[ID_GANTRY].v = (double)pPlcIo->stat_gt.inv_fb_v * pspec->base_gt.Rpm_rated / 1000.0;

	//#d �h�����w�Z�b�g
	double rd = pspec->base_bh.NdrmPgSet0 - pEnvInf->crane_stat.nd[ID_BOOM_H].p;	//�������̉�]��
	double chk_n = 0.7;// �����l�@83.3-21*4	5�w����萔
	double lout = 0.0;

	for(int i= 0; i < 5; i++) {
		if (rd < chk_n) {
			lout += rd * pEnvInf->Cdrm[ID_BOOM_H][4-i];	//������ index4=5�w
			break;
		}
		lout += chk_n * pEnvInf->Cdrm[ID_BOOM_H][4-i];	//������
		rd -= chk_n;
		chk_n = pspec->base_bh.Ndmizo0;	
	}
	pEnvInf->crane_stat.d.p = pspec->st_struct.d0 + lout/pspec->base_bh.Nwire0;	//������
	
	return S_OK;
}

HRESULT CCcEnv::routine_work(void* pObj) {
	if (inf.total_act % 20 == 0) {
		wos.str(L""); wos << inf.status << L":" << inf.dt;
		//wos.str(L""); wos << inf.status << L":" << std::setfill(L'0') << std::setw(4) << inf.act_time;
		if (inf.mode_id == MODE_ENV_APP_SIMURATION)		wos  << L" MODE>>SIMULATOR";
		else if (inf.mode_id == MODE_ENV_APP_EMURATOR)	wos  << L" MODE>>EMULATOR";
		else											wos  << L" MODE>>PRODUCT";
		msg2host(wos.str());
	}
	
	input();
	parse();
	output();
	return S_OK;
}

static UINT32	gpad_mode_last = L_OFF;

int CCcEnv::input() {
	return S_OK;
}

int CCcEnv::parse() { 

	//�h������ԃZ�b�g
	fp_set_drum_stat();
	//�N���p,���񔼌a
	double d=pEnvInf->crane_stat.d.p,	Lb = pspec->st_struct.Lb,	Ha = pspec->st_struct.Ha;
	pEnvInf->crane_stat.th.p = PI90 - acos((d*d - Lb*Lb - Ha*Ha)/(-2.0*Lb*Ha));	//�N���p�x
	pEnvInf->crane_stat.r.p = Lb*cos(pEnvInf->crane_stat.th.p);					//���񔼌a

	
	pEnvInf->crane_stat.m = pPlcIo->weight;//�׏d
	
	LPST_PLC_RBUF_HHGH29 pPlcRbuf = (LPST_PLC_RBUF_HHGH29)pPlcIo->buf_io_read;
	//�g��
	pEnvInf->crane_stat.vm[ID_HOIST].p = pPlcIo->h_mh;
	//����p�x
	pEnvInf->crane_stat.vm[ID_SLEW].p = (double)(pPlcRbuf->hcount_fb[ID_PLC_HCOUNT_SL] - pspec->base_sl.CntPgSet0) / pspec->base_sl.Kp ;//����p�x

	
	//�̏���Z�b�g

	if (pPlcIo->plc_enable) {
		if(plc_enable_hold) {	//PLC�L����ԕω������i�O����L���j
			set_faults_info();
		}
		else {
			refresh_faults_info();
		}
	}
	plc_enable_hold = pPlcIo->plc_enable;	//PLC�L����ԕێ�

	return STAT_OK; 
}
int CCcEnv::output() {          //�o�͏���
	//���L�������ɒ��ڏ������݂ɂ���@memcpy_s(pEnvInf, sizeof(ST_CC_ENV_INF), &st_work, sizeof(ST_CC_ENV_INF));
	return STAT_OK;
}

int CCcEnv::close() {
	return 0;
}

/****************************************************************************/
/*   �̏���											                    */
/****************************************************************************/

void CCcEnv::set_faults_info() {
	PINT16 pflt_rbuf = pCrane->pFlt->prfltbuf;

	//�������X�V
	for (int i = FAULT_TYPE::BASE; i <= FAULT_TYPE::IL; i++) {
		for (int j = 0; j < N_PLC_FAULT_BUF; j++) {
			pEnvInf->crane_stat.fault_list.faults_detected_map[i][j] = pflt_rbuf[j]& pCrane->pFlt->flt_list.fault_mask[FAULT_TYPE::BASE][j];
		}
	}

	if (inf.act_count % 50 == 0){//1�b��1��g���K���o�X�V	
		SYSTEMTIME systime; GetSystemTime(&systime);

		for (int j = 0; j < N_PLC_FAULT_BUF; j++) {
			falt_detected_trig_on[j] = (falt_detected_hold[j]^ pEnvInf->crane_stat.fault_list.faults_detected_map[FAULT_TYPE::BASE][j])& pEnvInf->crane_stat.fault_list.faults_detected_map[FAULT_TYPE::BASE][j];
			falt_detected_trig_off[j] = falt_detected_hold[j] ^ pEnvInf->crane_stat.fault_list.faults_detected_map[FAULT_TYPE::BASE][j]& falt_detected_hold[j];
	
			INT16 chk_bit;
			if (falt_detected_trig_on[j]) {
				for (int k = 0; k < 16; k++) {
					chk_bit = 1 << k;	//�`�F�b�N�r�b�g
					if (falt_detected_trig_on[j] & chk_bit) {
						pEnvInf->crane_stat.fault_list.history[pEnvInf->crane_stat.fault_list.iw_history].systime = systime;	//����
						pEnvInf->crane_stat.fault_list.history[pEnvInf->crane_stat.fault_list.iw_history].code = j*16+k;		//�̏�R�[�h
						++pEnvInf->crane_stat.fault_list.history[pEnvInf->crane_stat.fault_list.iw_history].code;				//++�̏�R�[�h�͂P����J�n
						pEnvInf->crane_stat.fault_list.history[pEnvInf->crane_stat.fault_list.iw_history].status = CODE_TRIG_ON;//���
					
						pEnvInf->crane_stat.fault_list.iw_history++;	//�������݃|�C���^�X�V
						pEnvInf->crane_stat.fault_list.history_count++;	//���R�[�h���X�V
						if (pEnvInf->crane_stat.fault_list.iw_history >= N_FAULTS_HISTORY_BUF)pEnvInf->crane_stat.fault_list.iw_history = 0;
						if (pEnvInf->crane_stat.fault_list.history_count > N_FAULTS_HISTORY_BUF)pEnvInf->crane_stat.fault_list.history_count = N_FAULTS_HISTORY_BUF;
					}
				}
			}
			if (falt_detected_trig_off[j]) {
				for (int k = 0; k < 16; k++) {
					chk_bit = 1 << k;	//�`�F�b�N�r�b�g
					if (falt_detected_trig_off[j] & chk_bit) {
						pEnvInf->crane_stat.fault_list.history[pEnvInf->crane_stat.fault_list.iw_history].systime = systime;	//����
						pEnvInf->crane_stat.fault_list.history[pEnvInf->crane_stat.fault_list.iw_history].code = j * 16 + k;	//�̏�R�[�h
						++pEnvInf->crane_stat.fault_list.history[pEnvInf->crane_stat.fault_list.iw_history].code;				//++�̏�R�[�h�͂P����J�n
						pEnvInf->crane_stat.fault_list.history[pEnvInf->crane_stat.fault_list.iw_history].status = CODE_TRIG_OFF;//���

						pEnvInf->crane_stat.fault_list.iw_history++;	//�������݃|�C���^�X�V
						pEnvInf->crane_stat.fault_list.history_count++;	//���R�[�h���X�V
						if (pEnvInf->crane_stat.fault_list.iw_history >= N_FAULTS_HISTORY_BUF)pEnvInf->crane_stat.fault_list.iw_history = 0;
						if (pEnvInf->crane_stat.fault_list.history_count > N_FAULTS_HISTORY_BUF)pEnvInf->crane_stat.fault_list.history_count = N_FAULTS_HISTORY_BUF;

					}
				}
			}
		}

		//�g���K���o�O��l�ێ�
		for (int j = 0; j < N_PLC_FAULT_BUF; j++) falt_detected_hold[j] = pEnvInf->crane_stat.fault_list.faults_detected_map[FAULT_TYPE::BASE][j];
	}

	return;
}

void CCcEnv::refresh_faults_info() {
	PINT16 pflt_rbuf = pCrane->pFlt->prfltbuf;

	for (int i = FAULT_TYPE::BASE; i <= FAULT_TYPE::IL; i++) {
		for (int j = 0; j < N_PLC_FAULT_BUF; j++) {
			pEnvInf->crane_stat.fault_list.faults_detected_map[i][j] = pflt_rbuf[j] & pCrane->pFlt->flt_list.fault_mask[FAULT_TYPE::BASE][j];
		}
	}
	//�O��l=����l,�g���K���o����
	for (int j = 0; j < N_PLC_FAULT_BUF; j++) {
		falt_detected_hold[j] = pEnvInf->crane_stat.fault_list.faults_detected_map[FAULT_TYPE::BASE][j];
		falt_detected_trig_on[j] = 0;	//�g���KON����
		falt_detected_trig_off[j] = 0;	//�g���KOFF����
	}


	return;
}

/****************************************************************************/
/*   �ʐM�֐�											                    */
/****************************************************************************/
/// <summary>
/// AUXEQ���j�L���X�g�d����M����
/// </summary>
HRESULT CCcEnv::rcv_uni_aux(LPST_AUX_COM_SERV_MSG pbuf) {
	int nRtn = pUSockCcEnv->rcv_msg((char*)pbuf, sizeof(ST_AUX_COM_SERV_MSG));
	if (nRtn == SOCKET_ERROR) {
		if (st_mon2.sock_inf_id == ENV_ID_MON2_RADIO_RCV) {
			st_mon2.wo_uni.str(L""); st_mon2.wo_uni << L"ERR rcv:" << pUSockCcEnv->err_msg.str();
			SetWindowText(st_mon2.hctrl[ENV_ID_MON2_STATIC_MSG], st_mon2.wo_uni.str().c_str());
			return S_FALSE;
		}
	}
	rcv_count_u++;
	return S_OK;
}

/****************************************************************************/
/// <summary>
/// PC���j�L���X�g�d�����M���� 
/// </summary>
LPST_AUX_COM_CLI_MSG CCcEnv::set_msg_u(BOOL is_monitor_mode, INT32 code, INT32 stat) {
	return &pEnvInf->st_msg_u_snd;
}

HRESULT CCcEnv::snd_uni2aux(LPST_AUX_COM_CLI_MSG pbuf, SOCKADDR_IN* p_addrin_to) {

	if (pUSockCcEnv->snd_msg((char*)pbuf, sizeof(ST_AUX_COM_CLI_MSG), *p_addrin_to) == SOCKET_ERROR) {
		if (st_mon2.sock_inf_id == ENV_ID_MON2_RADIO_SND) {
			st_mon2.wo_uni.str(L""); st_mon2.wo_uni << L"ERR snd:" << pUSockCcEnv->err_msg.str();
			SetWindowText(st_mon2.hctrl[ENV_ID_MON2_STATIC_MSG], st_mon2.wo_uni.str().c_str());
		}
		return S_FALSE;
	}
	snd_count_u++;
	return S_OK;
}


/****************************************************************************/
/*   ���j�^�E�B���h�E									                    */
/****************************************************************************/
static wostringstream monwos;

LRESULT CALLBACK CCcEnv::Mon1Proc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
	switch (msg)
	{
	case WM_CREATE: {
		InitCommonControls();//�R�����R���g���[��������
		HINSTANCE hInst = (HINSTANCE)GetModuleHandle(0);
		//�E�B���h�E�ɃR���g���[���ǉ�
		st_mon1.hctrl[ENV_ID_MON1_STATIC_GPAD] = CreateWindowW(TEXT("STATIC"), st_mon1.text[ENV_ID_MON1_STATIC_GPAD], WS_CHILD | WS_VISIBLE | SS_LEFT,
			st_mon1.pt[ENV_ID_MON1_STATIC_GPAD].x, st_mon1.pt[ENV_ID_MON1_STATIC_GPAD].y,
			st_mon1.sz[ENV_ID_MON1_STATIC_GPAD].cx, st_mon1.sz[ENV_ID_MON1_STATIC_GPAD].cy,
			hWnd, (HMENU)(ENV_ID_MON1_CTRL_BASE + ENV_ID_MON1_STATIC_GPAD), hInst, NULL);

		//�\���X�V�p�^�C�}�[
		SetTimer(hWnd, ENV_ID_MON1_TIMER, st_mon1.timer_ms, NULL);

		break;
	}
	case WM_COMMAND: {
		int wmId = LOWORD(wp);
		// �I�����ꂽ���j���[�̉��:
		switch (wmId)
		{
		case 1:break;
		default:
			return DefWindowProc(hWnd, msg, wp, lp);
		}
	}break;
	case WM_TIMER: {
	}break;

	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
	}break;
	case WM_DESTROY: {
		st_mon1.hwnd_mon = NULL;
		KillTimer(hWnd, ENV_ID_MON1_TIMER);
	}break;
	default:
		return DefWindowProc(hWnd, msg, wp, lp);
	}
	return S_OK;
};

LRESULT CALLBACK CCcEnv::Mon2Proc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
	switch (msg)
	{
	case WM_CREATE: {
		InitCommonControls();//�R�����R���g���[��������
		HINSTANCE hInst = (HINSTANCE)GetModuleHandle(0);
		//�E�B���h�E�ɃR���g���[���ǉ�
		//STATIC,LABEL
		for (int i = ENV_ID_MON2_LABEL_SOCK; i <= ENV_ID_MON2_STATIC_MSG; i++) {
			st_mon2.hctrl[i] = CreateWindowW(TEXT("STATIC"), st_mon2.text[i], WS_CHILD | WS_VISIBLE | SS_LEFT,
				st_mon2.pt[i].x, st_mon2.pt[i].y, st_mon2.sz[i].cx, st_mon2.sz[i].cy,
				hWnd, (HMENU)(ENV_ID_MON2_CTRL_BASE + i), hInst, NULL);
		}
		//RADIO PB
		for (int i = ENV_ID_MON2_RADIO_RCV; i <= ENV_ID_MON2_RADIO_INFO; i++) {
			if (i == ENV_ID_MON2_RADIO_RCV) {
				st_mon2.hctrl[i] = CreateWindowW(TEXT("BUTTON"), st_mon2.text[i], WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_PUSHLIKE | WS_GROUP,
					st_mon2.pt[i].x, st_mon2.pt[i].y, st_mon2.sz[i].cx, st_mon2.sz[i].cy,
					hWnd, (HMENU)(ENV_ID_MON2_CTRL_BASE + i), hInst, NULL);

				st_mon2.sock_inf_id = ENV_ID_MON2_RADIO_RCV;
				SendMessage(st_mon2.hctrl[i], BM_SETCHECK, BST_CHECKED, 0L);
			}
			else
				st_mon2.hctrl[i] = CreateWindowW(TEXT("BUTTON"), st_mon2.text[i], WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_PUSHLIKE,
					st_mon2.pt[i].x, st_mon2.pt[i].y, st_mon2.sz[i].cx, st_mon2.sz[i].cy,
					hWnd, (HMENU)(ENV_ID_MON2_CTRL_BASE + i), hInst, NULL);
		}

		UINT_PTR rtn = SetTimer(hWnd, ENV_ID_MON2_TIMER, ENV_PRM_MON2_TIMER_MS, NULL);
		break;

	}

	case WM_TIMER: {
		//UniCast���M
		if ((pEnvInf->aux_mode != FUNC_DEACTIVE) && (pEnvInf->aux_mode != FUNC_PAUSE)) {
			snd_uni2aux(set_msg_u(false, 0, 0), &pUSockCcEnv->addr_in_dst);
		}

		//�ʐM�J�E���g���^�C�g���o�[�ɕ\��
		st_mon2.wo_work.str(L""); st_mon2.wo_work << L"AUX_IF% PC_U (R:" << rcv_count_u << L" S:" << snd_count_u << L")" ;
		SetWindowText(st_mon2.hwnd_mon, st_mon2.wo_work.str().c_str());

		//���j�^�[�\��
		if (st_mon2.is_monitor_active) {
			SOCKADDR_IN	addr;
			if (pUSockCcEnv != NULL) {
				addr = pUSockCcEnv->addr_in_rcv; st_mon2.wo_work.str(L"");
				st_mon2.wo_work << L"UNI>>IP R:" << addr.sin_addr.S_un.S_un_b.s_b1 << L"." << addr.sin_addr.S_un.S_un_b.s_b2 << L"." << addr.sin_addr.S_un.S_un_b.s_b3 << L"." << addr.sin_addr.S_un.S_un_b.s_b4 << L":"
					<< htons(addr.sin_port) << L" ";
				addr = pUSockCcEnv->addr_in_dst;
				st_mon2.wo_work << L" S:" << addr.sin_addr.S_un.S_un_b.s_b1 << L"." << addr.sin_addr.S_un.S_un_b.s_b2 << L"." << addr.sin_addr.S_un.S_un_b.s_b3 << L"." << addr.sin_addr.S_un.S_un_b.s_b4 << L":"
					<< htons(addr.sin_port) << L" ";
				addr = pUSockCcEnv->addr_in_from; ;
				st_mon2.wo_work << L" F:" << addr.sin_addr.S_un.S_un_b.s_b1 << L"." << addr.sin_addr.S_un.S_un_b.s_b2 << L"." << addr.sin_addr.S_un.S_un_b.s_b3 << L"." << addr.sin_addr.S_un.S_un_b.s_b4 << L":"
					<< htons(addr.sin_port) << L" ";
				SetWindowText(st_mon2.hctrl[ENV_ID_MON2_STATIC_SOCK], st_mon2.wo_work.str().c_str());
			}

			st_mon2.wo_uni.str(L"");
			if (st_mon2.sock_inf_id == ENV_ID_MON2_RADIO_RCV) {
				LPST_AUX_COM_MSG_HEAD	ph0 = &pEnvInf->st_msg_u_rcv.head;
				LPST_AUX_COM_SERV_BODY  pb0 = &pEnvInf->st_msg_u_rcv.body;
				st_mon2.wo_uni << L"[HEAD]" << L"CODE:" << ph0->code << L"\n";
				st_mon2.wo_uni << L"[BODY]" ;
			}
			else if (st_mon2.sock_inf_id == ENV_ID_MON2_RADIO_SND) {

				LPST_AUX_COM_MSG_HEAD	ph0 = &pEnvInf->st_msg_u_snd.head;
				LPST_AUX_COM_CLI_BODY  pb0 = &pEnvInf->st_msg_u_snd.body;
				st_mon2.wo_uni << L"[HEAD]" << L"CODE:" << ph0->code << L"\n";
				st_mon2.wo_uni << L"[BODY]";
			}
			else {
				st_mon2.wo_uni << L"No Message";
			}
			SetWindowText(st_mon2.hctrl[ENV_ID_MON2_LABEL_SOCK], st_mon2.wo_uni.str().c_str());
		}
	}break;

	case ID_SOCK_EVENT_CC_ENV_UNI: {
		int nEvent = WSAGETSELECTEVENT(lp);
		switch (nEvent) {
		case FD_READ: {
			//��@�v���Z�X����̃��j�L���X�g���b�Z�[�W��M
			rcv_uni_aux(&pEnvInf->st_msg_u_rcv);
		}break;
		case FD_WRITE: break;
		case FD_CLOSE: break;
		}
	}break;

	case WM_COMMAND: {
		int wmId = LOWORD(wp);
		// �I�����ꂽ���j���[�̉��:
		switch (wmId)
		{
		case 1:break;
		default:
			return DefWindowProc(hWnd, msg, wp, lp);
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

HWND CCcEnv::open_monitor_wnd(HWND h_parent_wnd, int id) {

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
		wcex.lpszMenuName = TEXT("ENV_MON1");
		wcex.lpszClassName = TEXT("ENV_MON1");
		wcex.hIconSm = NULL;

		ATOM fb = RegisterClassExW(&wcex);

		st_mon1.hwnd_mon = CreateWindowW(TEXT("ENV_MON1"), TEXT("ENV_MON1"), WS_OVERLAPPEDWINDOW,
			ENV_MON1_WND_X, ENV_MON1_WND_Y, ENV_MON1_WND_W, ENV_MON1_WND_H,
			h_parent_wnd, nullptr, hInst, nullptr);
		show_monitor_wnd(id);
		return st_mon1.hwnd_mon;
	}
	else if (id == BC_ID_MON2) {
		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = Mon2Proc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = hInst;
		wcex.hIcon = NULL;
		wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wcex.lpszMenuName = TEXT("ENV_MON2");
		wcex.lpszClassName = TEXT("ENV_MON2");
		wcex.hIconSm = NULL;

		ATOM fb = RegisterClassExW(&wcex);

		st_mon2.hwnd_mon = CreateWindowW(TEXT("ENV_MON2"), TEXT("ENV_MON2"), WS_OVERLAPPEDWINDOW,
			ENV_MON2_WND_X, ENV_MON2_WND_Y, ENV_MON2_WND_W, ENV_MON2_WND_H,
			h_parent_wnd, nullptr, hInst, nullptr);

		//show_monitor_wnd(id);
		return st_mon2.hwnd_mon;
	}
	else
	{
		return NULL;
	};

	return NULL;
}
void CCcEnv::close_monitor_wnd(int id) {
	if (id == BC_ID_MON1)
		DestroyWindow(st_mon1.hwnd_mon);
	else if (id == BC_ID_MON2)
		DestroyWindow(st_mon2.hwnd_mon);
	else;
	return;
}
void CCcEnv::show_monitor_wnd(int id) {
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
	return;
}
void CCcEnv::hide_monitor_wnd(int id) {
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
LRESULT CALLBACK CCcEnv::PanelProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp) {

	switch (msg) {
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
			inf.mode_id = pEnvInf->app_mode = MODE_ENV_APP_PRODUCT;
		}break;
		case IDC_TASK_MODE_RADIO1:
		{
			inf.mode_id = pEnvInf->app_mode = MODE_ENV_APP_EMURATOR;
		}break;
		case IDC_TASK_MODE_RADIO2:
		{
			inf.mode_id = pEnvInf->app_mode = MODE_ENV_APP_SIMURATION;
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

	case WM_USER_TASK_REQ: {
		if (HIWORD(wp) == WM_USER_WPH_OPEN_IF_WND) {
			wos.str(L"");
			if (lp == BC_ID_MON1) st_mon1.hwnd_mon = open_monitor_wnd(hDlg, lp);
			if (lp == BC_ID_MON2) st_mon2.hwnd_mon = open_monitor_wnd(hDlg, lp);
		}
		else if (wp == WM_USER_WPH_CLOSE_IF_WND) 	close_monitor_wnd(lp);
		else;
	}break;
	}
	return 0;
};

///###	�^�u�p�l����ListView�Ƀ��b�Z�[�W���o��
void CCcEnv::msg2listview(wstring wstr) {

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
void CCcEnv::set_PNLparam_value(float p1, float p2, float p3, float p4, float p5, float p6) {
	wstring wstr;
	wstr += std::to_wstring(p1); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT1), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p2); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT2), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p3); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT3), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p4); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT4), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p5); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT5), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p6); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT6), wstr.c_str());
}
//�^�u�p�l����Edit Box�����e�L�X�g��ݒ�
void CCcEnv::set_panel_tip_txt() {
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
void CCcEnv::set_func_pb_txt() {
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO1, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO2, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO3, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO4, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO5, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO6, L"-");
	return;
}
//�^�u�p�l����Item chk�e�L�X�g��ݒ�
void CCcEnv::set_item_chk_txt() {
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




