
#include "CSockLib.H"
#include "CCcCS.h"
#include "resource.h"
#include "framework.h"
#include "OTE_DEF.H"
#include "CCrane.H"

extern CSharedMem* pEnvInfObj;
extern CSharedMem* pPlcIoObj;
extern CSharedMem* pJobIoObj;
extern CSharedMem* pPolInfObj;
extern CSharedMem* pAgInfObj;
extern CSharedMem* pCsInfObj;
extern CSharedMem* pSimuStatObj;
extern CSharedMem* pOteInfObj;

extern CCrane* pCrane;
extern ST_DEVICE_CODE g_my_code;


//�\�P�b�g
static CSockUDP* pUSockOte;	//���j�L���X�gOTE�ʐM��M�p
static CSockUDP* pMSockOte;	//�}���`�L���X�gOTE�ʐM��M�p
static CSockUDP* pMSockPC;	//�}���`�L���X�gPC�ʐM��M�p

static SOCKADDR_IN addrin_pc_m2pc_snd;
static SOCKADDR_IN addrin_pc_m2ote_snd;

ST_CS_MON1 CCcCS::st_mon1;
ST_CS_MON2 CCcCS::st_mon2;

ST_CC_CS_INF CCcCS::st_cs_work;
ST_CC_OTE_INF CCcCS::st_ote_work;

//���L������
static LPST_CC_ENV_INF		pCraneStat;
static LPST_CC_CS_INF		pCS_Inf;
static LPST_CC_PLC_IO		pPLC_IO;
static LPST_CC_AGENT_INF	pAgent_Inf;
static LPST_CC_OTE_INF		pOTE_Inf;

static ST_PLC_IO_WIF* pPlcWIf = NULL;
static ST_PLC_IO_RIF* pPlcRIf = NULL;

static LONG rcv_count_ote_u = 0, snd_count_ote_u = 0;
static LONG rcv_count_pc_m = 0, snd_count_m2pc = 0;
static LONG rcv_count_ote_m = 0, snd_count_m2ote = 0;

/****************************************************************************/
/*   �f�t�H���g�֐�											                    */
/****************************************************************************/
CCcCS::CCcCS() {

}
CCcCS::~CCcCS() {

}

HRESULT CCcCS::initialize(LPVOID lpParam) {
	HRESULT hr = S_OK;
	//### �o�͗p���L�������擾
	out_size = sizeof(ST_CC_CS_INF);

	set_outbuf(pCsInfObj->get_pMap());

	//### ���L������
	pCraneStat	= (LPST_CC_ENV_INF)(pEnvInfObj->get_pMap());
	pPLC_IO		= (LPST_CC_PLC_IO)(pPlcIoObj->get_pMap());
	pOTE_Inf	= (LPST_CC_OTE_INF)pOteInfObj->get_pMap();
	pCS_Inf		= (LPST_CC_CS_INF)pCsInfObj->get_pMap();
	pAgent_Inf	= (LPST_CC_AGENT_INF)pAgInfObj->get_pMap();

	//### �N���[���I�u�W�F�N�g�擾
	if (pCrane == NULL) {
		wos.str(L""); wos << L"Initialize : CraneObject NULL"; msg2listview(wos.str());
		return S_FALSE;
	}
	else {
		pPlcWIf = &(pCrane->pPlc->plc_io_wif);
		pPlcRIf = &(pCrane->pPlc->plc_io_rif);
	}

	if((pCraneStat == NULL) || (pPLC_IO == NULL) || (pOTE_Inf == NULL) || (pCS_Inf == NULL) || (pAgent_Inf == NULL))
		hr = S_FALSE;

	if (hr == S_FALSE) {
		wos.str(L""); wos << L"Initialize : SMEM NG"; msg2listview(wos.str());
		return hr;
	};

	//### IF�E�B���h�EOPEN
	WPARAM wp = MAKELONG(inf.index,WM_USER_WPH_OPEN_IF_WND);//HWORD:�R�}���h�R�[�h, LWORD:�^�X�N�C���f�b�N�X
	LPARAM lp = BC_ID_MON2;
	SendMessage(inf.hwnd_opepane, WM_USER_TASK_REQ, wp, lp);
	
	//### �ʐM�\�P�b�g�A�h���X�Z�b�g
	//##�C���X�^���X����
	pUSockOte = new CSockUDP(ACCESS_TYPE_SERVER, ID_SOCK_EVENT_OTE_UNI);//#OTE���j�L���X�g��M
	pMSockPC = new CSockUDP(ACCESS_TYPE_CLIENT, ID_SOCK_EVENT_PC_MUL);	//#PC�}���`�L���X�g��M
	pMSockOte = new CSockUDP(ACCESS_TYPE_CLIENT, ID_SOCK_EVENT_OTE_MUL);//#OTE�}���`�L���X�g��M

	//��M�A�h���X
	pUSockOte->set_sock_addr(&pUSockOte->addr_in_rcv, OTE_IF_UNI_IP_PC, OTE_IF_UNI_PORT_PC);
	pMSockPC->set_sock_addr(&pMSockPC->addr_in_rcv,   OTE_IF_UNI_IP_PC, OTE_IF_MULTI_PORT_PC2PC);//��M�A�h���X
	pMSockOte->set_sock_addr(&pMSockOte->addr_in_rcv, OTE_IF_UNI_IP_PC, OTE_IF_MULTI_PORT_OTE2PC);//��M�A�h���X
	
	//���M��A�h���X
	pUSockOte->set_sock_addr(&(pUSockOte->addr_in_dst), OTE_IF_UNI_IP_OTE0, OTE_IF_UNI_PORT_OTE);
	pMSockPC->set_sock_addr(&addrin_pc_m2pc_snd,  OTE_IF_MULTI_IP_PC, OTE_IF_MULTI_PORT_PC2PC);
	pMSockPC->set_sock_addr(&addrin_pc_m2ote_snd, OTE_IF_MULTI_IP_PC, OTE_IF_MULTI_PORT_PC2OTE);
	
	//### �ʐM�\�P�b�g����/������
	//##WSA������
	wos.str(L"");
	if (pUSockOte->Initialize() != S_OK) { wos << L"Err(IniWSA):" << pUSockOte->err_msg.str(); err |= SOCK_NG_UNICAST;   hr = S_FALSE;}
	if (pMSockPC->Initialize()  != S_OK) { wos << L"Err(IniWSA):" << pMSockPC->err_msg.str();  err |= SOCK_NG_MULTICAST; hr = S_FALSE;}
	if (pMSockOte->Initialize() != S_OK) { wos << L"Err(IniWSA):" << pMSockOte->err_msg.str(); err |= SOCK_NG_MULTICAST; hr = S_FALSE;}
	if(hr==S_FALSE)msg2listview(wos.str()); wos.str(L"");

	if (st_mon2.hwnd_mon == NULL) {
		wos << L"Err(MON2 NULL Handle!!):";
		msg2listview(wos.str()); wos.str(L"");
		return hr;
	}

	//##�\�P�b�g�\�P�b�g�����E�ݒ�
	//##���j�L���X�g
	if (pUSockOte->init_sock(st_mon2.hwnd_mon, pUSockOte->addr_in_rcv) != S_OK) {//init_sock():bind()���񓯊����܂Ŏ��{
		wos << L"OTE U SockErr:" << pUSockOte->err_msg.str(); err |= SOCK_NG_UNICAST; hr = S_FALSE;
	}
	else wos << L"OTE U Socket init OK";msg2listview(wos.str()); wos.str(L"");
	
	//#�}���`�L���X�g
	SOCKADDR_IN addr_buf;
	pMSockPC->set_sock_addr(&addr_buf, OTE_IF_MULTI_IP_PC, NULL);//PC�}���`�L���X�g��MIP�Z�b�g,PORT�̓l�b�g���[�N�ݒ�i��2�����j�̃|�[�g
	if (pMSockPC->init_sock(st_mon2.hwnd_mon, pMSockPC->addr_in_rcv, addr_buf) != S_OK) {//init_sock_m():bind()�܂Ŏ��{ + �}���`�L���X�g�O���[�v�֓o�^
		wos << L"PC M SockErr:"<< pMSockPC->err_msg.str(); hr = S_FALSE;
	}
	else wos << L"PC M Socket init OK";	msg2listview(wos.str()); wos.str(L"");

	pMSockOte->set_sock_addr(&addr_buf, OTE_IF_MULTI_IP_OTE, NULL);//OTE�}���`�L���X�g��MIP�Z�b�g,PORT�̓l�b�g���[�N�ݒ�i��2�����j�̃|�[�g
	if (pMSockOte->init_sock(st_mon2.hwnd_mon, pMSockOte->addr_in_rcv, addr_buf) != S_OK) {
		wos << L"OTE M SockErr:" << pMSockOte->err_msg.str(); hr = S_FALSE;
	}	
	else  wos << L"OTE M Socket init OK"; msg2listview(wos.str()); wos.str(L"");

	//���M���b�Z�[�W�w�b�_�ݒ�i���M����M�A�h���X�F��M��̐܂�Ԃ��p�j
	st_ote_work.st_msg_pc_u_snd.head.addr = pUSockOte->addr_in_rcv;
	st_ote_work.st_msg_pc_m_snd.head.addr = pMSockOte->addr_in_rcv;

	if (hr == S_FALSE) {
		pUSockOte->Close();				//�\�P�b�g�N���[�Y
		pMSockPC->Close();				//�\�P�b�g�N���[�Y
		pMSockOte->Close();				//�\�P�b�g�N���[�Y
		close_monitor_wnd(BC_ID_MON2);	//�ʐM���j�^�N���[�Y
		wos.str(L""); wos << L"Initialize : SOCKET NG"; msg2listview(wos.str());
		return hr;
	};

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
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_MON_CHECK2, L"OTE IF");
	set_item_chk_txt();
	set_panel_tip_txt();
	//���j�^�QCB��ԃZ�b�g	
	if(st_mon2.hwnd_mon !=NULL)
		SendMessage(GetDlgItem(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK1), BM_SETCHECK, BST_CHECKED, 0L);
	else
		SendMessage(GetDlgItem(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK1), BM_SETCHECK, BST_UNCHECKED, 0L);

	return hr;
}

HRESULT CCcCS::routine_work(void* pObj) {
	if (inf.total_act % 20 == 0) {
		wos.str(L""); wos << inf.status << L":" << std::setfill(L'0') << std::setw(4) << inf.act_time;
		msg2host(wos.str());
	}
	input();
	parse();
	output();
	return S_OK;
}

int CCcCS::input() {


	return S_OK;
}

int CCcCS::parse() {
	//����L���[���ʐM�r�؂�J�E���^�@����܂Ŏ������J�E���g�A�b�v�@�J�E���g�͑���L���[���L�ŃN���A
	if (!(st_ote_work.ope_ote_silent_cnt & 0xFFFFFF00)) st_ote_work.ope_ote_silent_cnt++;
	
	//PLC�f�[�^���
	//st_ote_work.st_body�̓��e�����M�o�b�t�@�ɃR�s�[�����
	UN_LAMP_COM *plamp_com = st_ote_work.st_body.lamp;
	if (!pPLC_IO->plc_enable) {	//PLC�ʐM�����ő���֘A���[�h�N���A
		st_ote_work.st_ote_ctrl.id_ope_active	= OTE_NON_OPEMODE_ACTIVE;
		st_ote_work.st_ote_ctrl.gpad_mode		= L_OFF;
		st_ote_work.st_ote_ctrl.auto_sel		= L_OFF;
		st_ote_work.st_ote_ctrl.auto_mode		= L_OFF;
		//�����v�N���A
		memset(plamp_com, 0, sizeof(UN_LAMP_COM) * N_OTE_PNL_CTRL);	
	}
	else {
		//�N���[���I�u�W�F�N�g����PLCIF�o�b�t�@�̐M���ǂݎ��˃����v�o��
		plamp_com[OTE_PNL_CTRLS::estop].st.com		= pCrane->pPlc->rval(pPlcRIf->estop).i16;

		if (pCrane->pPlc->rval(pPlcRIf->syukan_mc_comp).i16 ) {
			plamp_com[OTE_PNL_CTRLS::syukan_on].st.com  = CODE_PNL_COM_OFF;
			plamp_com[OTE_PNL_CTRLS::syukan_off].st.com = CODE_PNL_COM_ON;
		}
		else {
			plamp_com[OTE_PNL_CTRLS::syukan_on].st.com = CODE_PNL_COM_ON;
			plamp_com[OTE_PNL_CTRLS::syukan_off].st.com = CODE_PNL_COM_OFF;
		}
		 pCrane->pPlc->rval(pPlcRIf->syukan_on).i16;
		 pCrane->pPlc->rval(pPlcRIf->syukan_off).i16;


		plamp_com[OTE_PNL_CTRLS::fault_reset].st.com= pCrane->pPlc->rval(pPlcRIf->fault_reset_pb).i16;
		plamp_com[OTE_PNL_CTRLS::bypass].st.com		= CODE_PNL_COM_ON;

		//PLC��CS�X�C�b�`�̏��
		plamp_com[OTE_PNL_CTRLS::mh_spd_mode].st.com 
			= CPlcCSHelper::get_mode_by_code(pCrane->pPlc->rval(pPlcRIf->mh_spd_cs).i16, PLC_IO_CS_MH_SPD_MODE, g_my_code.serial_no);
		plamp_com[OTE_PNL_CTRLS::bh_r_mode].st.com
			= CPlcCSHelper::get_mode_by_code(pCrane->pPlc->rval(pPlcRIf->bh_mode_cs).i16, PLC_IO_CS_BH_R_MODE, g_my_code.serial_no);

		//�m�b�`�M��FB
		INT16 notch = pCrane->pPlc->rval(pPlcRIf->mh_notch).i16;
		plamp_com[OTE_PNL_CTRLS::notch_mh].st.com	= CNotchHelper::get_notch4_by_code(&notch,0);
		notch = pCrane->pPlc->rval(pPlcRIf->bh_notch).i16;
		plamp_com[OTE_PNL_CTRLS::notch_bh].st.com	= CNotchHelper::get_notch4_by_code(&notch,0);
		notch = pCrane->pPlc->rval(pPlcRIf->sl_notch).i16;
		plamp_com[OTE_PNL_CTRLS::notch_sl].st.com	= CNotchHelper::get_notch4_by_code(&notch,0);
		notch = pCrane->pPlc->rval(pPlcRIf->gt_notch).i16;
		plamp_com[OTE_PNL_CTRLS::notch_gt].st.com	= CNotchHelper::get_notch4_by_code(&notch,0);
	}

	return S_OK;
}
int CCcCS::output() {          //�o�͏���
	//���L�������o�͏���
	//CS_INF ����R���g���[�����i���[�h���j
	memcpy_s(&(pCS_Inf->cs_ctrl), sizeof(ST_CC_CS_CTRL), &st_cs_work.cs_ctrl, sizeof(ST_CC_CS_CTRL));
	
	//OTE�ڑ����i���[�h���j
	//CS_INF ����R���g���[�����i���[�h���j
	memcpy_s(&(pOTE_Inf->st_ote_ctrl), sizeof(ST_CC_OTE_CTRL), &st_ote_work.st_ote_ctrl, sizeof(ST_CC_OTE_CTRL));
	return STAT_OK;
}

int CCcCS::close() {

	return 0;
}

/****************************************************************************/
/*   �ʐM�֐�											                    */
/****************************************************************************/
/// <summary>
/// OTE���j�L���X�g�d����M����
/// </summary>
 
static ST_OTE_U_MSG chkbuf_u_msg;
HRESULT CCcCS::rcv_uni_ote(LPST_OTE_U_MSG pbuf) {
	//# ����L���[���Ƃ̒ʐM�f�ŗL���[���N���A
	if (st_ote_work.ope_ote_silent_cnt > OTE_IF_RELEASE_COUNTUP) {
		st_ote_work.st_ote_ctrl.id_ope_active = OTE_NON_OPEMODE_ACTIVE;			//�ێ�IP�A�h���X�N���A
		st_ote_work.st_ote_ctrl.addr_in_active_ote.sin_addr.S_un.S_addr = 0;	//�ێ�IP�A�h���X�N���A
		st_ote_work.st_ote_ctrl.active_ote_type = OTE_STAT_TYPE_UNKOWN;
	}

	//# ��U�`�F�b�N�o�b�t�@�Ɏ�M(����L��/�����[���؂蕪���ׁ̈j
	if (SOCKET_ERROR == pUSockOte->rcv_msg((char*)(&chkbuf_u_msg), sizeof(ST_PC_U_MSG))) {
		if (st_mon2.sock_inf_id == CS_ID_MON2_RADIO_RCV) {
			st_mon2.wo_uni.str(L""); st_mon2.wo_uni << L"ERR rcv:" << pUSockOte->err_msg.str();
			SetWindowText(st_mon2.hctrl[CS_ID_MON2_STATIC_UNI], st_mon2.wo_uni.str().c_str());
			return S_FALSE;
		}
	}

	//# ����L��/�����؂蕪������
	//## ��M�d���w�b�_���ɑ��샂�[�h�ڑ��v��������Γo�^�A�Ȃ���΃��j�^�p�o�b�t�@�Ɏ�M�d���R�s�[
	if (st_ote_work.st_ote_ctrl.id_ope_active == OTE_NON_OPEMODE_ACTIVE) {	//����L���[������
		//���M���̒[�������j�^���[�h���瑀�샂�[�h�ւ̗v���L�Œ[���Z�b�g
		if ((chkbuf_u_msg.head.status == OTE_STAT_MODE_OPE) && (chkbuf_u_msg.head.code == OTE_CODE_REQ_OPE_ACTIVE)
			&& pPLC_IO->plc_enable) {//���M���������[�g�v����PLC�w���V�[OK
			st_ote_work.st_ote_ctrl.id_ope_active = chkbuf_u_msg.head.myid.serial_no;
			st_ote_work.st_ote_ctrl.addr_in_active_ote = pUSockOte->addr_in_from;	//�^�]�L��IP�ێ�
			*pbuf = chkbuf_u_msg;													//�^�]�p�o�b�t�@�ɃR�s�[
			st_ote_work.ope_ote_silent_cnt = 0;										//�T�C�����g�J�E���^�N���A
		}
		else st_ote_work.st_msg_ote_u_rcv_mon = chkbuf_u_msg;						//���j�^�v���Ώۗp�o�b�t�@�ɃR�s�[
	}

	//����L���[�����o�^�ςő��M���A�h���X���o�^���e�ƈ�v�̎�
	//## ��M�d���w�b�_���Ƀ��j�^���[�h�ڑ��v��������Α���[���o�^�����A�Ȃ���ΐ���p�o�b�t�@�Ɏ�M�d���R�s�[
	else if (st_ote_work.st_ote_ctrl.addr_in_active_ote.sin_addr.S_un.S_addr 
		== pUSockOte->addr_in_from.sin_addr.S_un.S_addr) {//���M��IP���ێ�����IP�ƈ�v
		//���샂�[�h�̒[������܂�J�E���^�N���A
		st_ote_work.ope_ote_silent_cnt = 0;
		//���M�������j�^�v�����[���̃��[�h�����j�^�̎�
		if ((chkbuf_u_msg.head.code == OTE_CODE_REQ_MON) || (chkbuf_u_msg.head.status == OTE_STAT_MODE_MON)) {
			st_ote_work.st_ote_ctrl.id_ope_active = OTE_NON_OPEMODE_ACTIVE;	//�L��ID����
			pUSockOte->addr_in_from.sin_addr.S_un.S_addr = 0;				//�ێ�IP�A�h���X�N���A
			st_ote_work.st_msg_ote_u_rcv_mon = chkbuf_u_msg;				//���j�^�v���Ώۗp�o�b�t�@�ɃR�s�[
		}
		else {
			*pbuf = chkbuf_u_msg;											//����p�o�b�t�@�ɃR�s�[
		}
	}
	//�o�^����L���[���������āA���̑��̒[������̎�M��
	else {
		st_ote_work.st_msg_ote_u_rcv_mon = chkbuf_u_msg;					//���j�^�v���Ώۗp�o�b�t�@�ɃR�s�[
	}
	
	//# �w�b�_���ً̋}��~�w�߂͖������Ɏ�荞��
	if (chkbuf_u_msg.head.code & OTE_MASK_REQ_STOP)
		st_ote_work.st_ote_ctrl.stop_req_mode |= OTE_MASK_REQ_STOP;
	if (chkbuf_u_msg.head.code & OTE_MASK_REQ_FAULT_RESET)
		st_ote_work.st_ote_ctrl.stop_req_mode &= ~OTE_MASK_REQ_STOP;

	rcv_count_ote_u++;
	return S_OK;
}
/****************************************************************************/
/// <summary>
/// PC�}���`�L���X�g�d����M���� 
/// </summary>
HRESULT CCcCS::rcv_mul_pc(LPST_PC_M_MSG pbuf) {
	int nRtn = pMSockPC->rcv_msg((char*)pbuf, sizeof(ST_PC_M_MSG));
	if (nRtn == SOCKET_ERROR) {
		if (st_mon2.sock_inf_id == CS_ID_MON2_RADIO_RCV) {
			st_mon2.wo_uni.str(L""); st_mon2.wo_mpc << L"ERR rcv:" << pMSockPC->err_msg.str();
			SetWindowText(st_mon2.hctrl[CS_ID_MON2_STATIC_MUL_PC], st_mon2.wo_uni.str().c_str());
			return S_FALSE;
		}
	}
	rcv_count_pc_m++;
	return S_OK;
}
/****************************************************************************/
/// <summary>
/// OTE�}���`�L���X�g�d����M���� 
/// </summary>
HRESULT CCcCS::rcv_mul_ote(LPST_OTE_M_MSG pbuf) {
	int nRtn = pMSockOte->rcv_msg((char*)pbuf, sizeof(ST_OTE_M_MSG));
	if (nRtn == SOCKET_ERROR) {
		if (st_mon2.sock_inf_id == CS_ID_MON2_RADIO_RCV) {
			st_mon2.wo_mote.str(L""); st_mon2.wo_mote << L"ERR rcv:" << pMSockOte->err_msg.str();
			SetWindowText(st_mon2.hctrl[CS_ID_MON2_STATIC_MUL_OTE], st_mon2.wo_mote.str().c_str());
			return S_FALSE;
		}
	}
	rcv_count_ote_m++;
	return S_OK;
}
/****************************************************************************/
/// <summary>
/// PC���j�L���X�g�d�����M���� 
/// </summary>

/// <summary>
/// 
/// </summary>
/// <param name="is_monitor_mode"></param>
/// <param name="code"></param>
/// <param name="stat"></param>
/// <returns></returns>
LPST_PC_U_MSG CCcCS::set_msg_u(BOOL is_monitor_mode, INT32 code, INT32 stat) {
	
	//#Header��
	st_ote_work.st_msg_pc_u_snd.head.myid	= pCraneStat->device_code;
	st_ote_work.st_msg_pc_u_snd.head.addr	= pUSockOte->addr_in_rcv;
	st_ote_work.st_msg_pc_u_snd.head.code	= code;
	st_ote_work.st_msg_pc_u_snd.head.status = stat;
	st_ote_work.st_msg_pc_u_snd.head.tgid	= st_ote_work.st_ote_ctrl.id_ope_active;
	
	//#Body��
	memcpy_s(&st_ote_work.st_msg_pc_u_snd.body, sizeof(ST_PC_U_BODY), &st_ote_work.st_body, sizeof(ST_PC_U_BODY) );

	return &st_ote_work.st_msg_pc_u_snd;
}

HRESULT CCcCS::snd_uni2ote(LPST_PC_U_MSG pbuf, SOCKADDR_IN* p_addrin_to) {

	if (pUSockOte->snd_msg((char*)pbuf, sizeof(ST_PC_U_MSG), *p_addrin_to) == SOCKET_ERROR) {
		if (st_mon2.sock_inf_id == CS_ID_MON2_RADIO_SND) {
			st_mon2.wo_uni.str(L""); st_mon2.wo_uni << L"ERR snd:" << pUSockOte->err_msg.str();
			SetWindowText(st_mon2.hctrl[CS_ID_MON2_STATIC_UNI], st_mon2.wo_uni.str().c_str());
		}
		return S_FALSE;
	}
	snd_count_ote_u++;
	return S_OK;
}
/****************************************************************************/
/// <summary>
/// PC�}���`�L���X�g�d�����M���� 
/// </summary>

//�}���`�L���X�g���b�Z�[�W�Z�b�g
LPST_PC_M_MSG CCcCS::set_msg_m(INT32 code, INT32 stat) {	
	return &st_ote_work.st_msg_pc_m_snd;
}

//PC�֑��M
HRESULT CCcCS::snd_mul2pc(LPST_PC_M_MSG pbuf) {
	if (pMSockPC->snd_msg((char*)pbuf, sizeof(ST_PC_M_MSG), addrin_pc_m2pc_snd) == SOCKET_ERROR) {
	
		if (st_mon2.sock_inf_id == CS_ID_MON2_RADIO_SND) {
			st_mon2.wo_mpc.str(L""); st_mon2.wo_mpc << L"ERR snd:" << pMSockPC->err_msg.str();
			SetWindowText(st_mon2.hctrl[CS_ID_MON2_STATIC_MUL_PC], st_mon2.wo_mpc.str().c_str());
		}
		return S_FALSE;
	}
	snd_count_m2pc++;
	return S_OK;
}

//OTE�֑��M
HRESULT CCcCS::snd_mul2ote(LPST_PC_M_MSG pbuf) {
	if (pMSockOte->snd_msg((char*)pbuf, sizeof(ST_PC_M_MSG), addrin_pc_m2ote_snd) == SOCKET_ERROR) {

		if (st_mon2.sock_inf_id == CS_ID_MON2_RADIO_SND) {
			st_mon2.wo_mote.str(L""); st_mon2.wo_mote << L"ERR snd:" << pMSockOte->err_msg.str();
			SetWindowText(st_mon2.hctrl[CS_ID_MON2_STATIC_MUL_OTE], st_mon2.wo_mote.str().c_str());
		}
		return S_FALSE;
	}
	snd_count_m2ote++;
	return S_OK;
}

/****************************************************************************/
/*   ���j�^�E�B���h�E									                    */
/****************************************************************************/

LRESULT CALLBACK CCcCS::Mon1Proc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
	switch (msg)
	{
	case WM_CREATE: {
		InitCommonControls();//�R�����R���g���[��������
		HINSTANCE hInst = (HINSTANCE)GetModuleHandle(0);
		//�E�B���h�E�ɃR���g���[���ǉ�
		st_mon1.hctrl[CS_ID_MON1_STATIC_1] = CreateWindowW(TEXT("STATIC"), st_mon1.text[CS_ID_MON1_STATIC_1], WS_CHILD | WS_VISIBLE | SS_LEFT,
			st_mon1.pt[CS_ID_MON1_STATIC_1].x, st_mon1.pt[CS_ID_MON1_STATIC_1].y,
			st_mon1.sz[CS_ID_MON1_STATIC_1].cx, st_mon1.sz[CS_ID_MON1_STATIC_1].cy,
			hWnd, (HMENU)(CS_ID_MON1_CTRL_BASE + CS_ID_MON1_STATIC_1), hInst, NULL);

		//�\���X�V�p�^�C�}�[
		SetTimer(hWnd, CS_ID_MON1_TIMER, st_mon1.timer_ms, NULL);

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
		KillTimer(hWnd, CS_ID_MON1_TIMER);
	}break;
	default:
		return DefWindowProc(hWnd, msg, wp, lp);
	}
	return S_OK;
};

static wostringstream mon2wos;
LRESULT CALLBACK CCcCS::Mon2Proc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
	switch (msg)
	{
	case WM_CREATE: {
		InitCommonControls();//�R�����R���g���[��������
		HINSTANCE hInst = (HINSTANCE)GetModuleHandle(0);
		//�E�B���h�E�ɃR���g���[���ǉ�
		//STATIC,LABEL
		int i;
		for ( i = CS_ID_MON2_STATIC_UNI; i <= CS_ID_MON2_STATIC_MSG; i++){
		st_mon2.hctrl[i] = CreateWindowW(TEXT("STATIC"), st_mon2.text[i], WS_CHILD | WS_VISIBLE | SS_LEFT,
			st_mon2.pt[i].x, st_mon2.pt[i].y,st_mon2.sz[i].cx, st_mon2.sz[i].cy,
			hWnd, (HMENU)(CS_ID_MON2_CTRL_BASE + i), hInst, NULL);
		}
		//RADIO PB
		for (i = CS_ID_MON2_RADIO_RCV; i <= CS_ID_MON2_RADIO_INFO; i++) {
			if (i == CS_ID_MON2_RADIO_RCV) {
				st_mon2.hctrl[i] = CreateWindowW(TEXT("BUTTON"), st_mon2.text[i], WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_PUSHLIKE | WS_GROUP,
					st_mon2.pt[i].x, st_mon2.pt[i].y, st_mon2.sz[i].cx, st_mon2.sz[i].cy,
					hWnd, (HMENU)(CS_ID_MON2_CTRL_BASE + i), hInst, NULL);

				st_mon2.sock_inf_id = CS_ID_MON2_RADIO_RCV;
				SendMessage(st_mon2.hctrl[i], BM_SETCHECK, BST_CHECKED, 0L);
			}
			else
				st_mon2.hctrl[i] = CreateWindowW(TEXT("BUTTON"), st_mon2.text[i], WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_PUSHLIKE,
					st_mon2.pt[i].x, st_mon2.pt[i].y, st_mon2.sz[i].cx, st_mon2.sz[i].cy,
					hWnd, (HMENU)(CS_ID_MON2_CTRL_BASE + i), hInst, NULL);
		}

		//CHECK BOX
		i = CS_ID_MON2_CB_BODY;

		st_mon2.hctrl[i] = CreateWindowW(TEXT("BUTTON"), st_mon2.text[i], WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX ,
			st_mon2.pt[i].x, st_mon2.pt[i].y, st_mon2.sz[i].cx, st_mon2.sz[i].cy,
			hWnd, (HMENU)(CS_ID_MON2_CTRL_BASE + i), hInst, NULL);

		//PB
		for (i = CS_ID_MON2_PB_PAGE_UP_UNI; i <= CS_ID_MON2_PB_PAGE_UP_MOT; i++) {
				st_mon2.hctrl[i] = CreateWindowW(TEXT("BUTTON"), st_mon2.text[i], WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
					st_mon2.pt[i].x, st_mon2.pt[i].y, st_mon2.sz[i].cx, st_mon2.sz[i].cy,
					hWnd, (HMENU)(CS_ID_MON2_CTRL_BASE + i), hInst, NULL);
		}

		UINT rtn = SetTimer(hWnd, CS_ID_MON2_TIMER, CS_PRM_MON2_TIMER_MS,NULL);

	}break;
	case WM_COMMAND: {
		int wmId = LOWORD(wp);

		int _Id = wmId - CS_ID_MON2_CTRL_BASE;
		// �I�����ꂽ���j���[�̉��:
		switch (_Id)
		{

		case CS_ID_MON2_RADIO_RCV: {
			st_mon2.sock_inf_id = CS_ID_MON2_RADIO_RCV;
		}break;
		case CS_ID_MON2_RADIO_SND: {
			st_mon2.sock_inf_id = CS_ID_MON2_RADIO_SND;
		}break;
		case CS_ID_MON2_RADIO_INFO: {
			st_mon2.sock_inf_id = CS_ID_MON2_RADIO_INFO;
		}break;

		case CS_ID_MON2_CB_BODY       :{
			if (BST_CHECKED == SendMessage(st_mon2.hctrl[CS_ID_MON2_CB_BODY], BM_GETCHECK, 0, 0))
				st_mon2.is_body_disp_mode = true;
			else
				st_mon2.is_body_disp_mode = false;
		}break;
		case CS_ID_MON2_PB_PAGE_UP_UNI:{
			if(++st_mon2.ipage_uni >= CS_ID_MON2_N_PAGE_UP_UNI)st_mon2.ipage_uni=0;
		}break;
		case CS_ID_MON2_PB_PAGE_UP_MPC:{
			if (++st_mon2.ipage_mpc >= CS_ID_MON2_N_PAGE_UP_MPC)st_mon2.ipage_mpc = 0;
		}break;
		case CS_ID_MON2_PB_PAGE_UP_MOT:{
			if (++st_mon2.ipage_mot >= CS_ID_MON2_N_PAGE_UP_MOT)st_mon2.ipage_mot = 0;
		}break;
		default:
			return DefWindowProc(hWnd, msg, wp, lp);
		}
	}break;
	case WM_TIMER: {
		//UniCast���M
			//�܂�Ԃ����M
		//MultiCast���M
		snd_mul2pc(set_msg_m(0,0));		//PC�֑��M
		snd_mul2ote(set_msg_m(0, 0));	//OTE�֑��M

		//�ʐM�J�E���g���^�C�g���o�[�ɕ\��
		st_mon2.wo_work.str(L""); st_mon2.wo_work << L"MKCC_IF% PC_U (R:" << rcv_count_ote_u << L" S:" << snd_count_ote_u << L")  PC_M (R:" << rcv_count_pc_m << L")  OTE_M (R:" << rcv_count_ote_m << L" S PC:" << snd_count_m2pc << L"  S OTE:" << snd_count_m2ote << L")";
		SetWindowText(st_mon2.hwnd_mon, st_mon2.wo_work.str().c_str());

		//���j�^�[�\��
		if (st_mon2.is_monitor_active) {
			SOCKADDR_IN	addr;
			//���x����IP���o��
			if (pUSockOte != NULL) {
				addr = pUSockOte->addr_in_rcv; st_mon2.wo_work.str(L"");
				st_mon2.wo_work <<L"UNI>>IP R:"<<addr.sin_addr.S_un.S_un_b.s_b1<< L"." << addr.sin_addr.S_un.S_un_b.s_b2 << L"." << addr.sin_addr.S_un.S_un_b.s_b3 << L"." << addr.sin_addr.S_un.S_un_b.s_b4 << L":"
								<< htons(addr.sin_port) << L" ";
				addr = pUSockOte->addr_in_dst;
				st_mon2.wo_work << L" S:" << addr.sin_addr.S_un.S_un_b.s_b1 << L"." << addr.sin_addr.S_un.S_un_b.s_b2 << L"." << addr.sin_addr.S_un.S_un_b.s_b3 << L"." << addr.sin_addr.S_un.S_un_b.s_b4 << L":"
					<< htons(addr.sin_port) << L" ";
				addr = pUSockOte->addr_in_from; ;
				st_mon2.wo_work << L" F:" << addr.sin_addr.S_un.S_un_b.s_b1 << L"." << addr.sin_addr.S_un.S_un_b.s_b2 << L"." << addr.sin_addr.S_un.S_un_b.s_b3 << L"." << addr.sin_addr.S_un.S_un_b.s_b4 << L":"
					<< htons(addr.sin_port) << L" ";
				SetWindowText(st_mon2.hctrl[CS_ID_MON2_LABEL_UNI], st_mon2.wo_work.str().c_str());
			}
			if (pMSockPC != NULL) {
				addr = pMSockPC->addr_in_rcv; st_mon2.wo_work.str(L"");
				st_mon2.wo_work << L"MPC>>IP R:" << addr.sin_addr.S_un.S_un_b.s_b1 << L"." << addr.sin_addr.S_un.S_un_b.s_b2 << L"." << addr.sin_addr.S_un.S_un_b.s_b3 << L"." << addr.sin_addr.S_un.S_un_b.s_b4 << L":"
					<< htons(addr.sin_port) << L" ";
				addr = pMSockPC->addr_in_dst;
				st_mon2.wo_work << L" S:" << addr.sin_addr.S_un.S_un_b.s_b1 << L"." << addr.sin_addr.S_un.S_un_b.s_b2 << L"." << addr.sin_addr.S_un.S_un_b.s_b3 << L"." << addr.sin_addr.S_un.S_un_b.s_b4 << L":"
					<< htons(addr.sin_port) << L" ";
				addr = pMSockPC->addr_in_from; ;
				st_mon2.wo_work << L" F:" << addr.sin_addr.S_un.S_un_b.s_b1 << L"." << addr.sin_addr.S_un.S_un_b.s_b2 << L"." << addr.sin_addr.S_un.S_un_b.s_b3 << L"." << addr.sin_addr.S_un.S_un_b.s_b4 << L":"
					<< htons(addr.sin_port) << L" ";
				SetWindowText(st_mon2.hctrl[CS_ID_MON2_LABEL_MUL_PC], st_mon2.wo_work.str().c_str());
			}
			if (pMSockOte != NULL) {
				addr = pMSockOte->addr_in_rcv; st_mon2.wo_work.str(L"");
				st_mon2.wo_work << L"MOTE>>IP R:" << addr.sin_addr.S_un.S_un_b.s_b1 << L"." << addr.sin_addr.S_un.S_un_b.s_b2 << L"." << addr.sin_addr.S_un.S_un_b.s_b3 << L"." << addr.sin_addr.S_un.S_un_b.s_b4 << L":"
					<< htons(addr.sin_port) << L" ";
				addr = pMSockOte->addr_in_dst;
				st_mon2.wo_work << L" S:" << addr.sin_addr.S_un.S_un_b.s_b1 << L"." << addr.sin_addr.S_un.S_un_b.s_b2 << L"." << addr.sin_addr.S_un.S_un_b.s_b3 << L"." << addr.sin_addr.S_un.S_un_b.s_b4 << L":"
					<< htons(addr.sin_port) << L" ";
				addr = pMSockOte->addr_in_from; ;
				st_mon2.wo_work << L" F:" << addr.sin_addr.S_un.S_un_b.s_b1 << L"." << addr.sin_addr.S_un.S_un_b.s_b2 << L"." << addr.sin_addr.S_un.S_un_b.s_b3 << L"." << addr.sin_addr.S_un.S_un_b.s_b4 << L":"
					<< htons(addr.sin_port) << L" ";
				SetWindowText(st_mon2.hctrl[CS_ID_MON2_LABEL_MUL_OTE], st_mon2.wo_work.str().c_str());
			}

			//�d�����e�\���o��
			st_mon2.wo_uni.str(L""); st_mon2.wo_mpc.str(L""); st_mon2.wo_mote.str(L"");
			if (st_mon2.sock_inf_id == CS_ID_MON2_RADIO_RCV) {
				if (st_mon2.is_body_disp_mode) {
					LPST_OTE_U_BODY pb0 = &pOTE_Inf->st_msg_ote_u_rcv.body.st;
					LPST_PC_M_BODY	pb1 = &pOTE_Inf->st_msg_pc_m_rcv.body.st;
					LPST_OTE_M_BODY pb2 = &pOTE_Inf->st_msg_ote_m_rcv.body.st;

					st_mon2.wo_uni << L"[BODY P"<< st_mon2.ipage_uni <<L"]" ;

					if (st_mon2.ipage_uni == 0) {
						st_mon2.wo_uni << L"@ESTOP:" << pb0->ctrl_ope[OTE_PNL_CTRLS::estop]
							<< L"@�劲��:" << pb0->ctrl_ope[OTE_PNL_CTRLS::syukan_on]
							<< L"@�劲��:" << pb0->ctrl_ope[OTE_PNL_CTRLS::syukan_off]
							<< L"@RMT:" << pb0->ctrl_ope[OTE_PNL_CTRLS::remote]
							<< L"@FRESET:" << pb0->ctrl_ope[OTE_PNL_CTRLS::fault_reset]
							<< L"@BYPASS:" << pb0->ctrl_ope[OTE_PNL_CTRLS::bypass]
							<< L"@MH_MODE:" << pb0->ctrl_ope[OTE_PNL_CTRLS::mh_spd_mode]
							<< L"@BH_MODE:" << pb0->ctrl_ope[OTE_PNL_CTRLS::bh_r_mode]

							<< L"\n@notch mh:" << pb0->ctrl_ope[OTE_PNL_CTRLS::notch_mh]
							<< L" bh:" << pb0->ctrl_ope[OTE_PNL_CTRLS::notch_bh]
							<< L" sl:" << pb0->ctrl_ope[OTE_PNL_CTRLS::notch_sl]
							<< L" gt:" << pb0->ctrl_ope[OTE_PNL_CTRLS::notch_gt];
					}
					else if (st_mon2.ipage_uni == 1) {
						st_mon2.wo_uni	<< L"@GPAD@ #Notch mh:" << pb0->gpad_in.pad_mh << L" bh:" << pb0->gpad_in.pad_bh << L" sl:" << pb0->gpad_in.pad_sl << L" gt:" << pb0->gpad_in.pad_gt;
						st_mon2.wo_uni	<< L" estop:" << pb0->gpad_in.estop << L" �劲off:" << pb0->gpad_in.syukan_off << L" �劲on:" << pb0->gpad_in.syukan_on << L" ���u:" << pb0->gpad_in.remote
							<< L"\n freset:" << pb0->gpad_in.f_reset << L" bypass:" << pb0->gpad_in.bypass << L" kidoul:" << pb0->gpad_in.kidou_l << L" kidour:" << pb0->gpad_in.kidou_r << L" trigl:" << pb0->gpad_in.trig_l << L" trigr:" << pb0->gpad_in.trig_r<< L"\n";
						st_mon2.wo_uni	<< L" panl:" << pb0->gpad_in.pan_l << L" panr:" << pb0->gpad_in.pan_r << L" tild:" << pb0->gpad_in.tilt_d << L" tilu:" << pb0->gpad_in.tilt_u << L" zmf:" << pb0->gpad_in.zoom_f << L" zmn:" << pb0->gpad_in.zoom_n;
					}
					else if (st_mon2.ipage_uni == 2) {
					}
					else {

					}

					st_mon2.wo_mpc << L"[BODY P" << st_mon2.ipage_mpc << L"]" << L"\n";
					st_mon2.wo_mote << L"[BODY P" << st_mon2.ipage_mot << L"]" << L"\n";

					st_mon2.wo_mpc << L"[BODY P"<< st_mon2.ipage_mpc <<L"]" << L"\n";
					st_mon2.wo_mote<< L"[BODY P"<< st_mon2.ipage_mot <<L"]" << L"\n";
				}
				else {
					LPST_OTE_HEAD ph0 = &pOTE_Inf->st_msg_ote_u_rcv.head;
					LPST_OTE_HEAD ph1 = &pOTE_Inf->st_msg_pc_m_rcv.head;
					LPST_OTE_HEAD ph2 = &pOTE_Inf->st_msg_ote_m_rcv.head;
					st_mon2.wo_uni	<< L"[HEAD]" << L" ID:"<<ph0->myid.crane_id << L" PC:" << ph0->myid.pc_type << L" Seral:" << ph0->myid.serial_no << L" Opt:" << ph0->myid.option 
								<< L" IP:" << ph0->addr.sin_addr.S_un.S_un_b.s_b1 << L"." << ph0->addr.sin_addr.S_un.S_un_b.s_b2 << L"." << ph0->addr.sin_addr.S_un.S_un_b.s_b3 << L"." << ph0->addr.sin_addr.S_un.S_un_b.s_b4 << L":" << htons(ph0->addr.sin_port)<<L"\n"
								<< L"       CODE:" << ph0->code << L" STAT:" << ph0->status << L" TGID:" << ph0->tgid <<L"\n";
					st_mon2.wo_mpc << L"[HEAD]" << L"CODE:" << ph1->code << L"\n";
					st_mon2.wo_mote<< L"[HEAD]" << L"CODE:" << ph2->code << L"\n";
				}
			}
			else if (st_mon2.sock_inf_id == CS_ID_MON2_RADIO_SND) {
				if (st_mon2.is_body_disp_mode) {
					LPST_PC_U_BODY pb0 = &st_ote_work.st_msg_pc_u_snd.body.st;
					LPST_PC_M_BODY pb1 = &st_ote_work.st_msg_pc_m_snd.body.st;

					st_mon2.wo_uni << L"[BODY P" << st_mon2.ipage_uni << L"]";

					if (st_mon2.ipage_uni == 0) {
						st_mon2.wo_uni << L"@ESTOP:" << pb0->lamp[OTE_PNL_CTRLS::estop].code
							<< L"@�劲��:" << pb0->lamp[OTE_PNL_CTRLS::syukan_on].code
							<< L"@�劲��:" << pb0->lamp[OTE_PNL_CTRLS::syukan_off].code
							<< L"@RMT:" << pb0->lamp[OTE_PNL_CTRLS::remote].code
							<< L"@FRESET:" << pb0->lamp[OTE_PNL_CTRLS::fault_reset].code
							<< L"@BYPASS:" << pb0->lamp[OTE_PNL_CTRLS::bypass].code
							<< L"@MH_MODE:" << pb0->lamp[OTE_PNL_CTRLS::mh_spd_mode].code
							<< L"@BH_MODE:" << pb0->lamp[OTE_PNL_CTRLS::bh_r_mode].code;

						st_mon2.wo_mpc << L"[BODY P" << st_mon2.ipage_mpc << L"]" << L"\n";
						st_mon2.wo_mote << L"[BODY P" << st_mon2.ipage_mot << L"]" << L"\n";
					}
					else {
						st_mon2.wo_uni << L"[BODY P"<< st_mon2.ipage_uni <<L"]" << L"\n";
						st_mon2.wo_mpc << L"[BODY P"<< st_mon2.ipage_mpc <<L"]" << L"\n";
						st_mon2.wo_mote<< L"[BODY P"<< st_mon2.ipage_mot <<L"]" << L"\n";
					}
				}
				else {
					LPST_OTE_HEAD ph0 = &st_ote_work.st_msg_pc_u_snd.head;
					LPST_OTE_HEAD ph1 = &st_ote_work.st_msg_pc_m_snd.head;
					st_mon2.wo_uni << L"[HEAD]" << L"ID:" << ph0->myid.crane_id << L" PC:" << ph0->myid.pc_type << L" Seral:" << ph0->myid.serial_no << L" Opt:" << ph0->myid.option;
					st_mon2.wo_uni << L" IP:" << ph0->addr.sin_addr.S_un.S_un_b.s_b1 << L"." << ph0->addr.sin_addr.S_un.S_un_b.s_b2 << L"." << ph0->addr.sin_addr.S_un.S_un_b.s_b3 << L"." << ph0->addr.sin_addr.S_un.S_un_b.s_b4 << L":" << htons(ph0->addr.sin_port) << L"\n";
					st_mon2.wo_uni << L"       CODE:" << ph0->code << L" STAT:" << ph0->status << L" TGID:" << ph0->tgid << L"\n";

					st_mon2.wo_mpc << L"[HEAD]" << L"CODE:" << ph1->code << L"\n";
					st_mon2.wo_mote << L"[HEAD] -\n";
				}
			}
			else {
				st_mon2.wo_uni  << L"No Message"; 
				st_mon2.wo_mpc  << L"No Message"; 
				st_mon2.wo_mote << L"No Message"; 
			}
			SetWindowText(st_mon2.hctrl[CS_ID_MON2_STATIC_UNI], st_mon2.wo_uni.str().c_str());
			SetWindowText(st_mon2.hctrl[CS_ID_MON2_STATIC_MUL_PC], st_mon2.wo_mpc.str().c_str());
			SetWindowText(st_mon2.hctrl[CS_ID_MON2_STATIC_MUL_OTE], st_mon2.wo_mote.str().c_str());
		}

	}break;

	case ID_SOCK_EVENT_OTE_UNI: {
		int nEvent = WSAGETSELECTEVENT(lp);
		switch (nEvent) {
		case FD_READ: {
			//OTE����̃��j�L���X�g���b�Z�[�W��M
			if(rcv_uni_ote(&pOTE_Inf->st_msg_ote_u_rcv) == S_OK){
				//�܂�Ԃ��A���T�o�b�N ���M���֕ԑ�
				st_ote_work.addr_in_from_oteu = pUSockOte->addr_in_from;
				pUSockOte->addr_in_dst.sin_family = AF_INET;
				pUSockOte->addr_in_dst.sin_port = htons(OTE_IF_UNI_PORT_OTE);
				pUSockOte->addr_in_dst.sin_addr = pUSockOte->addr_in_from.sin_addr;

				if (st_ote_work.st_ote_ctrl.id_ope_active == OTE_NON_OPEMODE_ACTIVE) //���샂�[�h�̒[����
					snd_uni2ote(set_msg_u(true, 0, st_ote_work.st_ote_ctrl.id_ope_active), &pUSockOte->addr_in_dst);
				else 
					snd_uni2ote(set_msg_u(false, 0, st_ote_work.st_ote_ctrl.id_ope_active), &pUSockOte->addr_in_dst);
			}

		}break;
		case FD_WRITE: break;
		case FD_CLOSE: break;
		}

		//OTE�ʐM�w�b�_�ɋً}��~�v���L
		if (st_ote_work.st_msg_ote_u_rcv.head.code == OTE_CODE_REQ_ESTP) {
			st_ote_work.st_ote_ctrl.stop_req_mode |= OTE_STOP_REQ_MODE_ESTOP;
		}

	}break;
	case ID_SOCK_EVENT_OTE_MUL: {
		int nEvent = WSAGETSELECTEVENT(lp);
		switch (nEvent) {
		case FD_READ: {
			rcv_mul_ote(&st_ote_work.st_msg_ote_m_rcv);
		}break;
		case FD_WRITE: break;
		case FD_CLOSE: break;
		}
	}break;
	case ID_SOCK_EVENT_PC_MUL: {
		int nEvent = WSAGETSELECTEVENT(lp);
		switch (nEvent) {
		case FD_READ: {
			rcv_mul_pc(&st_ote_work.st_msg_pc_m_rcv);
		}break;
		case FD_WRITE: break;
		case FD_CLOSE: break;
		}
	}break;

	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
	}break;
	case WM_DESTROY: {
		KillTimer(hWnd, CS_ID_MON2_TIMER);
		st_mon2.hwnd_mon = NULL;
	}break;
	default:
		return DefWindowProc(hWnd, msg, wp, lp);
	}
	return S_OK;
}

HWND CCcCS::open_monitor_wnd(HWND h_parent_wnd, int id) {

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
		wcex.lpszMenuName = TEXT("CS_MON1");
		wcex.lpszClassName = TEXT("CS_MON1");
		wcex.hIconSm = NULL;

		ATOM fb = RegisterClassExW(&wcex);

		st_mon1.hwnd_mon = CreateWindowW(TEXT("CS_MON1"), TEXT("CS_MON1"), WS_OVERLAPPEDWINDOW,
			CS_MON1_WND_X, CS_MON1_WND_Y, CS_MON1_WND_W, CS_MON1_WND_H,
			h_parent_wnd, nullptr, hInst, nullptr);
		show_monitor_wnd(id);

		wos.str(L"");
		if (st_mon1.hwnd_mon != NULL) wos << L"Succeed : MON1 open";
		else                          wos << L"!! Failed : MON1 open";
		msg2listview(wos.str());

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
		wcex.lpszMenuName = TEXT("CS_MON2");
		wcex.lpszClassName = TEXT("CS_MON2");
		wcex.hIconSm = NULL;

		ATOM fb = RegisterClassExW(&wcex);

		st_mon2.hwnd_mon = CreateWindowW(TEXT("CS_MON2"), TEXT("CS_MON2"), WS_OVERLAPPEDWINDOW,
			CS_MON2_WND_X, CS_MON2_WND_Y, CS_MON2_WND_W, CS_MON2_WND_H,
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
void CCcCS::close_monitor_wnd(int id) {
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
void CCcCS::show_monitor_wnd(int id) {
	if ((id == BC_ID_MON1)&&(st_mon1.hwnd_mon != NULL)) {
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
void CCcCS::hide_monitor_wnd(int id) {
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
LRESULT CALLBACK CCcCS::PanelProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp) {

	switch (msg) {
	case WM_USER_TASK_REQ: {
		if (HIWORD(wp) == WM_USER_WPH_OPEN_IF_WND) {
			if (lp == BC_ID_MON1) st_mon1.hwnd_mon = open_monitor_wnd(hDlg, lp);
			if (lp == BC_ID_MON2) st_mon2.hwnd_mon = open_monitor_wnd(hDlg, lp);	
		}
		else if (wp == WM_USER_WPH_CLOSE_IF_WND) close_monitor_wnd(lp);	
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
//				close_monitor_wnd(BC_ID_MON2);
				hide_monitor_wnd(BC_ID_MON2);
			}
		}break;
		}
	}
	return 0;
};

///###	�^�u�p�l����ListView�Ƀ��b�Z�[�W���o��
void CCcCS::msg2listview(wstring wstr) {

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
void CCcCS::set_PNLparam_value(float p1, float p2, float p3, float p4, float p5, float p6) {
	wstring wstr;
	wstr += std::to_wstring(p1); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT1), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p2); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT2), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p3); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT3), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p4); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT4), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p5); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT5), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p6); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT6), wstr.c_str());
}
//�^�u�p�l����Edit Box�����e�L�X�g��ݒ�
void CCcCS::set_panel_tip_txt() {
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
void CCcCS::set_func_pb_txt() {
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO1, L"OTE IF");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO2, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO3, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO4, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO5, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO6, L"-");
	return;
}
//�^�u�p�l����Item chk�e�L�X�g��ݒ�
void CCcCS::set_item_chk_txt() {
	wstring wstr_type; wstring wstr;
	switch (inf.panel_func_id) {
	case IDC_TASK_FUNC_RADIO1: {
		SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK1, L"WND");
		SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK2, L"-");
		SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK3, L"-");
		SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK4, L"-");
		SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK5, L"-");
		SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK6, L"-");
	}break;

	case IDC_TASK_FUNC_RADIO2:
	case IDC_TASK_FUNC_RADIO3:
	case IDC_TASK_FUNC_RADIO4:
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





