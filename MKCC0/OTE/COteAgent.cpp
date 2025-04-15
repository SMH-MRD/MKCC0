#include "CSockLib.H"
#include "COteAgent.h"
#include "resource.h"
#include "framework.h"
#include "OTE_DEF.H"

extern CSharedMem* pOteEnvInfObj;
extern CSharedMem* pOteCsInfObj;
extern CSharedMem* pOteCcInfObj;
extern CSharedMem* pOteUiObj;

//�\�P�b�g
static CSockUDP* pUSockPC;					//���j�L���X�gPC�ʐM��M�p
static CSockUDP* pMSockOte;					//�}���`�L���X�gOTE�ʐM��M�p
static CSockUDP* pMSockPC;					//�}���`�L���X�gPC�ʐM��M�p

static SOCKADDR_IN addrin_ote_m2pc_snd;		//OTE->PC OTE�}���`�L���X�g���M��A�h���X�iPC��M�p)
static SOCKADDR_IN addrin_ote_m2ote_snd;	//PC->OTE OTE�}���`�L���X�g���M��A�h���X�iOTE��M�p)

ST_OTE_AG_MON1 COteAgent::st_mon1;
ST_OTE_AG_MON2 COteAgent::st_mon2;

ST_OTE_CC_IF COteAgent::st_work;

//���L������

static LPST_OTE_ENV_INF	pOteEnvInf;
static LPST_OTE_UI		pOteUI;
static LPST_OTE_CC_IF	pOteCCIf;
static LPST_OTE_CS_INF	pOteCsInf;

static LONG rcv_count_pc_u = 0,	 snd_count_ote_u = 0;
static LONG rcv_count_pc_m = 0,  snd_count_m2pc = 0;
static LONG rcv_count_ote_m = 0, snd_count_m2ote = 0;

static LARGE_INTEGER start_count_s, end_count_r;			//�V�X�e���J�E���g
static LARGE_INTEGER frequency;								//�V�X�e�����g��
static LONGLONG res_delay_max, res_delay_min = 10000000;	//C��������

static wostringstream wos_msg;
/****************************************************************************/
/*   �f�t�H���g�֐�											                    */
/****************************************************************************/
COteAgent::COteAgent() {
}
COteAgent::~COteAgent() {
}

HRESULT COteAgent::initialize(LPVOID lpParam) {

	//�V�X�e�����g���ǂݍ���
	QueryPerformanceFrequency(&frequency);

	HRESULT hr = S_OK;	
	//### �o�͗p���L�������擾
	out_size = sizeof(ST_OTE_CC_IF);
	if (OK_SHMEM != pOteCcInfObj->create_smem(SMEM_OTE_CC_IF_NAME, out_size, MUTEX_OTE_CC_IF_NAME)) {
		err |= SMEM_NG_CS_INF; hr = S_FALSE;
	}
	set_outbuf(pOteCcInfObj->get_pMap());

	//### ���L�������擾
	pOteCCIf = (LPST_OTE_CC_IF)(pOteCcInfObj->get_pMap());
	pOteCsInf = (LPST_OTE_CS_INF)(pOteCsInfObj->get_pMap());
	pOteUI = (LPST_OTE_UI)pOteUiObj->get_pMap();
	pOteEnvInf = (LPST_OTE_ENV_INF)pOteEnvInfObj->get_pMap();

	if ((pOteCCIf == NULL) || (pOteCsInf == NULL) || (pOteUI == NULL) || (pOteEnvInf == NULL))
		hr = S_FALSE;

	if (hr == S_FALSE) {
		wos.str(L""); wos << L"Initialize : SMEM NG"; msg2listview(wos.str());
		return hr;
	};

	//### IF�E�B���h�EOPEN
	WPARAM wp = MAKELONG(inf.index, WM_USER_WPH_OPEN_IF_WND);//HWORD:�R�}���h�R�[�h, LWORD:�^�X�N�C���f�b�N�X
	LPARAM lp = BC_ID_MON2;
	SendMessage(inf.hwnd_opepane, WM_USER_TASK_REQ, wp, lp);

	//### �ʐM�\�P�b�g�A�h���X�Z�b�g
	// //##�C���X�^���X����
	pUSockPC = new CSockUDP(ACCESS_TYPE_SERVER, ID_SOCK_EVENT_OTE_UNI);
	pMSockPC = new CSockUDP(ACCESS_TYPE_CLIENT, ID_SOCK_EVENT_PC_MUL);
	pMSockOte = new CSockUDP(ACCESS_TYPE_CLIENT, ID_SOCK_EVENT_OTE_MUL);

	//��M�A�h���X �I�I�y���z��M�A�h���X�̓A�_�v�^����ǂݎ��ݒ�
	pUSockPC->set_sock_addr( &pUSockPC->addr_in_rcv,  OTE_IF_UNI_IP_OTE0, OTE_IF_UNI_PORT_OTE);
	pMSockPC->set_sock_addr( &pMSockPC->addr_in_rcv,  OTE_IF_UNI_IP_OTE0, OTE_IF_MULTI_PORT_PC2OTE);//��M�A�h���X
	pMSockOte->set_sock_addr(&pMSockOte->addr_in_rcv, OTE_IF_UNI_IP_OTE0, OTE_IF_MULTI_PORT_OTE2OTE);//��M�A�h���X
	
	//���M��A�h���X
	pUSockPC->set_sock_addr(&(pUSockPC->addr_in_dst), OTE_IF_UNI_IP_OTE0, OTE_IF_UNI_PORT_PC);//���M��A�h���X
	pMSockPC->set_sock_addr(&addrin_ote_m2pc_snd,  OTE_IF_MULTI_IP_OTE,  OTE_IF_MULTI_PORT_OTE2PC); //���M��A�h���X
	pMSockPC->set_sock_addr(&addrin_ote_m2ote_snd, OTE_IF_MULTI_IP_OTE,  OTE_IF_MULTI_PORT_OTE2OTE);//���M��A�h���X

	//### WSA������
	wos.str(L"");//������
	if (pUSockPC->Initialize()  != S_OK) {wos << L"Err(IniWSA):" << pUSockPC->err_msg.str();  err |= SOCK_NG_UNICAST; hr = S_FALSE;}
	if (pMSockPC->Initialize()  != S_OK) {wos << L"Err(IniWSA):" << pMSockPC->err_msg.str();  err |= SOCK_NG_MULTICAST; hr = S_FALSE;}
	if (pMSockOte->Initialize() != S_OK) {wos << L"Err(IniWSA):" << pMSockOte->err_msg.str(); err |= SOCK_NG_MULTICAST; hr = S_FALSE;}
	if (hr == S_FALSE)msg2listview(wos.str()); wos.str(L"");

	Sleep(1000);
	if (st_mon2.hwnd_mon == NULL) {
		wos << L"Err(MON2 NULL Handle!!):";
		msg2listview(wos.str()); wos.str(L"");
		return S_FALSE;
	}

	//##�\�P�b�g�\�P�b�g�����E�ݒ�
	//##���j�L���X�g
	if (pUSockPC->init_sock(st_mon2.hwnd_mon, pUSockPC->addr_in_rcv) != S_OK) {//init_sock():bind()���񓯊����܂Ŏ��{
		wos << L"OTE U SockErr:" << pUSockPC->err_msg.str(); err |= SOCK_NG_UNICAST; hr = S_FALSE;
	}
	else wos << L"OTE U Socket init OK";msg2listview(wos.str()); wos.str(L"");

	//##�}���`�L���X�g
	SOCKADDR_IN addr_buf;
	pMSockPC->set_sock_addr(&addr_buf, OTE_IF_MULTI_IP_PC, NULL);//PC�}���`�L���X�g��MIP�Z�b�g,PORT�̓l�b�g���[�N�ݒ�i��2�����j�̃|�[�g
	if (pMSockPC->init_sock(st_mon2.hwnd_mon, pMSockPC->addr_in_rcv, addr_buf) != S_OK) {//init_sock_m():bind()�܂Ŏ��{ + �}���`�L���X�g�O���[�v�֓o�^
		wos << L"PC M SockErr:" << pMSockPC->err_msg.str(); hr = S_FALSE;
	}
	else wos << L"PC M Socket init OK"; msg2listview(wos.str());wos.str(L"");
	
	pMSockOte->set_sock_addr(&addr_buf, OTE_IF_MULTI_IP_OTE, NULL);//OTE�}���`�L���X�g��MIP�Z�b�g,PORT�̓l�b�g���[�N�ݒ�i��2�����j�̃|�[�g
	if (pMSockOte->init_sock(st_mon2.hwnd_mon, pMSockOte->addr_in_rcv, addr_buf) != S_OK) {
		wos << L"OTE M SockErr:" << pMSockOte->err_msg.str(); hr = S_FALSE;;
	}
	else  wos << L"OTE M Socket init OK"; msg2listview(wos.str()); wos.str(L"");

	//���M���b�Z�[�W�w�b�_�ݒ�i���M����M�A�h���X�F��M��̐܂�Ԃ��p�j
	pOteCCIf->st_msg_ote_u_snd.head.addr = pUSockPC->addr_in_rcv;
	pOteCCIf->st_msg_ote_m_snd.head.addr = pMSockOte->addr_in_rcv;
		
	if (hr == S_FALSE) {
		pUSockPC->Close();				//�\�P�b�g�N���[�Y
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
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_MON_CHECK2, L"MKCC IF");
	set_item_chk_txt();
	set_panel_tip_txt();


	//���j�^2CB��ԃZ�b�g	
	if (st_mon2.hwnd_mon != NULL)
		SendMessage(GetDlgItem(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK1), BM_SETCHECK, BST_CHECKED, 0L);
	else
		SendMessage(GetDlgItem(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK1), BM_SETCHECK, BST_UNCHECKED, 0L);

	return hr;
}

HRESULT COteAgent::routine_work(void* pObj) {
	input();
	parse();
	output();
	return S_OK;
}

int COteAgent::input() {

	return S_OK;
}

int COteAgent::parse() {

	return S_OK;
}

int COteAgent::close() {

	return 0;
}

/****************************************************************************/
/*   �^�X�N�ݒ�^�u�p�l���E�B���h�E�̃R�[���o�b�N�֐�                       */
/****************************************************************************/
LRESULT CALLBACK COteAgent::PanelProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp) {

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
/****************************************************************************/
/*   �ʐM�֐�											                    */
/****************************************************************************/
/// <summary>
/// OTE���j�L���X�g�d����M����
/// </summary>
HRESULT COteAgent::rcv_uni_ote(LPST_PC_U_MSG pbuf) {
	int nRtn = pUSockPC->rcv_msg((char*)pbuf, sizeof(ST_PC_U_MSG));
	if (nRtn == SOCKET_ERROR) {
		if (st_mon2.sock_inf_id == OTE_AG_ID_MON2_RADIO_RCV) {
			st_mon2.wo_uni.str(L""); st_mon2.wo_uni << L"ERR rcv:" << pUSockPC->err_msg.str();
			SetWindowText(st_mon2.hctrl[OTE_AG_ID_MON2_STATIC_UNI], st_mon2.wo_uni.str().c_str());
		}
		return S_FALSE;
	}
	rcv_count_pc_u++;
	return S_OK;
}
/****************************************************************************/
/// <summary>
/// PC�}���`�L���X�g�d����M���� 
/// </summary>
HRESULT COteAgent::rcv_mul_pc(LPST_PC_M_MSG pbuf) {
	int nRtn = pMSockPC->rcv_msg((char*)pbuf, sizeof(ST_PC_M_MSG));
	if (nRtn == SOCKET_ERROR) {
		if (st_mon2.sock_inf_id == OTE_AG_ID_MON2_RADIO_RCV) {
			st_mon2.wo_mpc.str(L""); st_mon2.wo_mpc << L"ERR rcv:" << pMSockPC->err_msg.str();
			SetWindowText(st_mon2.hctrl[OTE_AG_ID_MON2_STATIC_MUL_PC], st_mon2.wo_mpc.str().c_str());
		}
		return S_FALSE;
	}
	rcv_count_pc_m++;
	return S_OK;
}
/****************************************************************************/
/// <summary>
/// OTE�}���`�L���X�g�d����M���� 
/// </summary>
HRESULT COteAgent::rcv_mul_ote(LPST_OTE_M_MSG pbuf) {
	int nRtn = pMSockOte->rcv_msg((char*)pbuf, sizeof(ST_OTE_M_MSG));
	if (nRtn == SOCKET_ERROR) {
		if (st_mon2.sock_inf_id == OTE_AG_ID_MON2_RADIO_RCV) {
			st_mon2.wo_mote.str(L""); st_mon2.wo_mote << L"ERR rcv:" << pMSockOte->err_msg.str();
			SetWindowText(st_mon2.hctrl[OTE_AG_ID_MON2_STATIC_MUL_OTE], st_mon2.wo_mote.str().c_str());
		}
		return S_FALSE;
	}
	rcv_count_ote_m++;
	return S_OK;
}
/****************************************************************************/
/// <summary>
/// OTE���j�L���X�g�d�����M���� 
/// </summary>
LPST_OTE_U_MSG COteAgent::set_msg_u(BOOL is_monitor_mode, INT32 code, INT32 stat) {

	pOteCCIf->st_msg_ote_u_snd.head.myid = pOteEnvInf->device_code;
	pOteCCIf->st_msg_ote_u_snd.head.addr = pUSockPC->addr_in_rcv;
	pOteCCIf->st_msg_ote_u_snd.head.code = code;
	pOteCCIf->st_msg_ote_u_snd.head.status = stat;
	pOteCCIf->st_msg_ote_u_snd.head.tgid = 0;

	return &pOteCCIf->st_msg_ote_u_snd;
}
HRESULT COteAgent::snd_uni2pc(LPST_OTE_U_MSG pbuf, SOCKADDR_IN* p_addrin_to) {
	if (pUSockPC->snd_msg((char*)pbuf, sizeof(ST_PC_U_MSG), *p_addrin_to) == SOCKET_ERROR) {
		if (st_mon2.sock_inf_id == OTE_AG_ID_MON2_RADIO_SND) {
			st_mon2.wo_uni.str(L""); st_mon2.wo_uni << L"ERR snd:" << pUSockPC->err_msg.str();
			SetWindowText(st_mon2.hctrl[OTE_AG_ID_MON2_STATIC_UNI], st_mon2.wo_uni.str().c_str());
		}
		return S_FALSE;
	}
	snd_count_ote_u++;
	return S_OK;
}
/****************************************************************************/
/// <summary>
/// OTE�}���`�L���X�g�d�����M���� 
/// </summary>
//�}���`�L���X�g���b�Z�[�W�Z�b�g
LPST_OTE_M_MSG COteAgent::set_msg_m() {
	return &pOteCCIf->st_msg_ote_m_snd;
}
//PC�֑��M
HRESULT COteAgent::snd_mul2pc(LPST_OTE_M_MSG pbuf) {
	if (pMSockPC->snd_msg((char*)pbuf, sizeof(ST_OTE_M_MSG), addrin_ote_m2pc_snd) == SOCKET_ERROR) {
		if (st_mon2.sock_inf_id == OTE_AG_ID_MON2_RADIO_SND) {
			st_mon2.wo_mpc.str(L""); st_mon2.wo_mpc << L"ERR snd:" << pMSockPC->err_msg.str();
			SetWindowText(st_mon2.hctrl[OTE_AG_ID_MON2_STATIC_MUL_PC], st_mon2.wo_mpc.str().c_str());
		}
		return S_FALSE;
	}
	snd_count_m2pc++;
	return S_OK;
}
//OTE�֑��M
HRESULT COteAgent::snd_mul2ote(LPST_OTE_M_MSG pbuf) {
	if (pMSockOte->snd_msg((char*)pbuf, sizeof(ST_OTE_M_MSG), addrin_ote_m2ote_snd) == SOCKET_ERROR) {
		st_mon2.wo_mote.str(L""); st_mon2.wo_mote << L"ERR snd:" << pMSockOte->err_msg.str();
		SetWindowText(st_mon2.hctrl[OTE_AG_ID_MON2_STATIC_MUL_PC], st_mon2.wo_mpc.str().c_str());
		wos_msg.str(L""); wos_msg << pMSockPC->err_msg.str();
		return S_FALSE;
	}
	snd_count_m2ote++;
	return S_OK;
}

/****************************************************************************/
/*   ���j�^�E�B���h�E									                    */
/****************************************************************************/
LRESULT CALLBACK COteAgent::Mon1Proc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
	switch (msg)
	{
	case WM_CREATE: {
		InitCommonControls();//�R�����R���g���[��������
		HINSTANCE hInst = (HINSTANCE)GetModuleHandle(0);
		//�E�B���h�E�ɃR���g���[���ǉ�
		st_mon1.hctrl[OTE_AG_ID_MON1_STATIC_1] = CreateWindowW(TEXT("STATIC"), st_mon1.text[OTE_AG_ID_MON1_STATIC_1], WS_CHILD | WS_VISIBLE | SS_LEFT,
			st_mon1.pt[OTE_AG_ID_MON1_STATIC_1].x, st_mon1.pt[OTE_AG_ID_MON1_STATIC_1].y,
			st_mon1.sz[OTE_AG_ID_MON1_STATIC_1].cx, st_mon1.sz[OTE_AG_ID_MON1_STATIC_1].cy,
			hWnd, (HMENU)(OTE_AG_ID_MON1_CTRL_BASE + OTE_AG_ID_MON1_STATIC_1), hInst, NULL);

		//�\���X�V�p�^�C�}�[
		SetTimer(hWnd, OTE_AG_ID_MON1_TIMER, st_mon1.timer_ms, NULL);

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
		KillTimer(hWnd, OTE_AG_ID_MON1_TIMER);
	}break;
	default:
		return DefWindowProc(hWnd, msg, wp, lp);
	}
	return S_OK;
};

//static wostringstream mon2wos;
LRESULT CALLBACK COteAgent::Mon2Proc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
	switch (msg)
	{
	case WM_CREATE: {
		InitCommonControls();//�R�����R���g���[��������
		HINSTANCE hInst = (HINSTANCE)GetModuleHandle(0);
		//�E�B���h�E�ɃR���g���[���ǉ�
		//STATIC,LABEL
		for (int i = OTE_AG_ID_MON2_STATIC_UNI; i <= OTE_AG_ID_MON2_STATIC_MSG; i++) {
			st_mon2.hctrl[i] = CreateWindowW(TEXT("STATIC"), st_mon2.text[i], WS_CHILD | WS_VISIBLE | SS_LEFT,
				st_mon2.pt[i].x, st_mon2.pt[i].y, st_mon2.sz[i].cx, st_mon2.sz[i].cy,
				hWnd, (HMENU)(OTE_AG_ID_MON2_CTRL_BASE + i), hInst, NULL);
		}
		//RADIO PB
		for (int i = OTE_AG_ID_MON2_RADIO_RCV; i <= OTE_AG_ID_MON2_RADIO_INFO; i++) {
			if (i == OTE_AG_ID_MON2_RADIO_RCV) {
				st_mon2.hctrl[i] = CreateWindowW(TEXT("BUTTON"), st_mon2.text[i], WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_PUSHLIKE | WS_GROUP,
					st_mon2.pt[i].x, st_mon2.pt[i].y, st_mon2.sz[i].cx, st_mon2.sz[i].cy,
					hWnd, (HMENU)(OTE_AG_ID_MON2_CTRL_BASE + i), hInst, NULL);

				st_mon2.sock_inf_id = OTE_AG_ID_MON2_RADIO_RCV;
				SendMessage(st_mon2.hctrl[i], BM_SETCHECK, BST_CHECKED, 0L);
			}
			else
				st_mon2.hctrl[i] = CreateWindowW(TEXT("BUTTON"), st_mon2.text[i], WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_PUSHLIKE,
					st_mon2.pt[i].x, st_mon2.pt[i].y, st_mon2.sz[i].cx, st_mon2.sz[i].cy,
					hWnd, (HMENU)(OTE_AG_ID_MON2_CTRL_BASE + i), hInst, NULL);
		}

		UINT rtn = SetTimer(hWnd, OTE_AG_ID_MON2_TIMER, OTE_AG_PRM_MON2_TIMER_MS, NULL);
	}break;
	case WM_COMMAND: {
		int wmId = LOWORD(wp);
		// �I�����ꂽ���j���[�̉��:
		int _Id = wmId - OTE_AG_ID_MON2_CTRL_BASE;
		// �I�����ꂽ���j���[�̉��:
		switch (_Id)
		{
		case OTE_AG_ID_MON2_RADIO_RCV: {
			st_mon2.sock_inf_id = OTE_AG_ID_MON2_RADIO_RCV;
		}break;
		case OTE_AG_ID_MON2_RADIO_SND: {
			st_mon2.sock_inf_id = OTE_AG_ID_MON2_RADIO_SND;
		}break;
		case OTE_AG_ID_MON2_RADIO_INFO: {
			st_mon2.sock_inf_id = OTE_AG_ID_MON2_RADIO_INFO;
		}break;
		default:
			return DefWindowProc(hWnd, msg, wp, lp);
		}break;
	}
	case WM_TIMER: {
		//UniCast���M
		if(pOteCCIf->id_ope_active)
			snd_uni2pc(set_msg_u(false, 0, 0), &pUSockPC->addr_in_dst);
		else 
			snd_uni2pc(set_msg_u(true, 0, 0), &pUSockPC->addr_in_dst);

		QueryPerformanceCounter(&start_count_s);  // ���M���J�E���g�l��荞��

		//MultiCast���M
		snd_mul2pc(set_msg_m()); 	//PC�֑��M
		snd_mul2ote(set_msg_m());	//OTE�֑��M

		//���j�^�[�\��
		if (st_mon2.is_monitor_active) {
			SOCKADDR_IN	addr;
			if (pUSockPC != NULL) {
				addr = pUSockPC->addr_in_rcv; st_mon2.wo_work.str(L"");
				st_mon2.wo_work << L"UNI>>IP R:" << addr.sin_addr.S_un.S_un_b.s_b1 << L"." << addr.sin_addr.S_un.S_un_b.s_b2 << L"." << addr.sin_addr.S_un.S_un_b.s_b3 << L"." << addr.sin_addr.S_un.S_un_b.s_b4 << L":"
					<< htons(addr.sin_port) << L" ";
				addr = pUSockPC->addr_in_dst;
				st_mon2.wo_work << L" S:" << addr.sin_addr.S_un.S_un_b.s_b1 << L"." << addr.sin_addr.S_un.S_un_b.s_b2 << L"." << addr.sin_addr.S_un.S_un_b.s_b3 << L"." << addr.sin_addr.S_un.S_un_b.s_b4 << L":"
					<< htons(addr.sin_port) << L" ";
				addr = pUSockPC->addr_in_from; ;
				st_mon2.wo_work << L" F:" << addr.sin_addr.S_un.S_un_b.s_b1 << L"." << addr.sin_addr.S_un.S_un_b.s_b2 << L"." << addr.sin_addr.S_un.S_un_b.s_b3 << L"." << addr.sin_addr.S_un.S_un_b.s_b4 << L":"
					<< htons(addr.sin_port) << L" ";
				SetWindowText(st_mon2.hctrl[OTE_AG_ID_MON2_LABEL_UNI], st_mon2.wo_work.str().c_str());
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
				SetWindowText(st_mon2.hctrl[OTE_AG_ID_MON2_LABEL_MUL_PC], st_mon2.wo_work.str().c_str());
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
				SetWindowText(st_mon2.hctrl[OTE_AG_ID_MON2_LABEL_MUL_OTE], st_mon2.wo_work.str().c_str());
			}

			st_mon2.wo_uni.str(L""); st_mon2.wo_mpc.str(L""); st_mon2.wo_mote.str(L"");
			if (st_mon2.sock_inf_id == OTE_AG_ID_MON2_RADIO_RCV) {

				LPST_OTE_HEAD	ph0 = &pOteCCIf->st_msg_pc_u_rcv.head;
				LPST_PC_U_BODY pb0 = &pOteCCIf->st_msg_pc_u_rcv.body;
				st_mon2.wo_uni << L"[HEAD]" << L"ID:" << ph0->myid.crane_id << L" PC:" << ph0->myid.pc_type << L" Seral:" << ph0->myid.serial_no << L" Opt:" << ph0->myid.option << L"\n"
								<< L" IP:" << ph0->addr.sin_addr.S_un.S_un_b.s_b1 << L"." << ph0->addr.sin_addr.S_un.S_un_b.s_b2 << L"." << ph0->addr.sin_addr.S_un.S_un_b.s_b3 << L"." << ph0->addr.sin_addr.S_un.S_un_b.s_b4 << L":" << htons(addr.sin_port) << L"\n";
				st_mon2.wo_uni << L"[BODY]" << L"�劲:" << pb0->ctrl_source;

				LPST_OTE_HEAD  ph1 = &pOteCCIf->st_msg_pc_m_rcv.head;
				LPST_PC_M_BODY pb1 = &pOteCCIf->st_msg_pc_m_rcv.body;
				st_mon2.wo_mpc << L"[HEAD]" << L" CODE:" << ph1->code << L"\n";
				st_mon2.wo_mpc << L"[BODY]";

				LPST_OTE_HEAD  ph2	= &pOteCCIf->st_msg_ote_m_rcv.head;
				LPST_OTE_M_BODY pb2 = &pOteCCIf->st_msg_ote_m_rcv.body;
				st_mon2.wo_mote << L"[HEAD]" << L"CODE:" << ph2->code << L"\n";
				st_mon2.wo_mote << L"[BODY]";
			}
			else if (st_mon2.sock_inf_id == OTE_AG_ID_MON2_RADIO_SND) {

				LPST_OTE_HEAD	ph0 = &pOteCCIf->st_msg_ote_u_snd.head;
				LPST_OTE_U_BODY  pb0 = &pOteCCIf->st_msg_ote_u_snd.body;
				st_mon2.wo_uni << L"[HEAD]" << L" ID:" << ph0->myid.crane_id << L"PC:" << ph0->myid.pc_type << L"Seral:" << ph0->myid.serial_no << L"Opt:" << ph0->myid.option
								<< L"       IP:" << ph0->addr.sin_addr.S_un.S_un_b.s_b1 << L"." << ph0->addr.sin_addr.S_un.S_un_b.s_b2 << L"." << ph0->addr.sin_addr.S_un.S_un_b.s_b3 << L"." << ph0->addr.sin_addr.S_un.S_un_b.s_b4 << L":" << htons(addr.sin_port)
								<< L" CODE:" << ph0->code << L" STAT:" << ph0->status << L" TGID:" << ph0->tgid << L"\n";
				st_mon2.wo_uni << L"[BODY]";

				st_mon2.wo_mpc << L"[HEAD] -\n";
				st_mon2.wo_mpc << L"[BODY] -";
				
				LPST_OTE_HEAD  ph1 = &pOteCCIf->st_msg_ote_m_snd.head;
				LPST_OTE_M_BODY pb1 = &pOteCCIf->st_msg_ote_m_snd.body;
				st_mon2.wo_mote << L"[HEAD]" << L"CODE:" << ph1->code << L"\n";
				st_mon2.wo_mote << L"[BODY]";
			}
			else {
				st_mon2.wo_uni << L"No Message";
				st_mon2.wo_mpc << L"No Message";
				st_mon2.wo_mote << L"No Message";
			}
			SetWindowText(st_mon2.hctrl[OTE_AG_ID_MON2_STATIC_UNI], st_mon2.wo_uni.str().c_str());
			SetWindowText(st_mon2.hctrl[OTE_AG_ID_MON2_STATIC_MUL_PC], st_mon2.wo_mpc.str().c_str());
			SetWindowText(st_mon2.hctrl[OTE_AG_ID_MON2_STATIC_MUL_OTE], st_mon2.wo_mote.str().c_str());

			//�ʐM�J�E���g���^�C�g���o�[�ɕ\��
			st_mon2.wo_work.str(L""); st_mon2.wo_work << L"MKCC_IF% PC_U (R:" << rcv_count_pc_u << L" S:" << snd_count_ote_u << L")  PC_M (R:" << rcv_count_pc_m << L")  OTE_M (R:" << rcv_count_ote_m << L" S PC:" << snd_count_m2pc << L"  S OTE:" << snd_count_m2ote << L")";
			SetWindowText(st_mon2.hwnd_mon, st_mon2.wo_work.str().c_str());

			//�����x�����Ԃ�MSG�ɕ\��
			st_mon2.wo_work.str(L""); st_mon2.wo_work << L"�����x��(��sec) MAX:" << res_delay_max << L" MIN:" << res_delay_min;
			SetWindowText(st_mon2.hctrl[OTE_AG_ID_MON2_STATIC_MSG], st_mon2.wo_work.str().c_str());
		}

	}break;
	case ID_SOCK_EVENT_OTE_UNI: {
		int nEvent = WSAGETSELECTEVENT(lp);
		switch (nEvent) {
		case FD_READ: {
			rcv_uni_ote(&pOteCCIf->st_msg_pc_u_rcv);

			QueryPerformanceCounter(&end_count_r);    // ������M���̃J�E���g��
			LONGLONG lspan = (end_count_r.QuadPart - start_count_s.QuadPart) * 1000000L / frequency.QuadPart;// ���Ԃ̊Ԋu[usec]
			if (res_delay_max < lspan) res_delay_max = lspan;
			if (res_delay_min > lspan) res_delay_min = lspan;
			if (rcv_count_pc_u % 50 == 0) {
				res_delay_max = 0;
				res_delay_min = 10000000;
			}

		}break;
		case FD_WRITE: break;
		case FD_CLOSE: break;
		}
		//OTE�ʐM�w�b�_�ɋً}��~�v���L
		if (pOteCCIf->st_msg_pc_u_rcv.head.code == OTE_CODE_REQ_ESTP) {
			pOteCCIf->stop_req_mode |= OTE_STOP_REQ_MODE_ESTOP;
		}

	}break;
	case ID_SOCK_EVENT_OTE_MUL: {
		int nEvent = WSAGETSELECTEVENT(lp);
		switch (nEvent) {
		case FD_READ: {
			rcv_mul_ote(&pOteCCIf->st_msg_ote_m_rcv);
		}break;
		case FD_WRITE: break;
		case FD_CLOSE: break;
		default:break;
		}
	}break;
	case ID_SOCK_EVENT_PC_MUL: {
		int nEvent = WSAGETSELECTEVENT(lp);
		switch (nEvent) {
		case FD_READ: {
			rcv_mul_pc(&pOteCCIf->st_msg_pc_m_rcv);
		}break;
		case FD_WRITE: break;
		case FD_CLOSE: break;
		default:break;
		}
	}break;
	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
	}break;
	case WM_DESTROY: {
		KillTimer(hWnd, OTE_AG_ID_MON2_TIMER);
		st_mon2.hwnd_mon = NULL;
	}break;
	default:
		return DefWindowProc(hWnd, msg, wp, lp);
	}

	return S_OK;
}

HWND COteAgent::open_monitor_wnd(HWND h_parent_wnd, int id) {

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
		wcex.lpszMenuName = TEXT("OTE_AG_MON1");
		wcex.lpszClassName = TEXT("OTE_AG_MON1");
		wcex.hIconSm = NULL;

		ATOM fb = RegisterClassExW(&wcex);

		st_mon1.hwnd_mon = CreateWindowW(TEXT("OTE_AG_MON1"), TEXT("OTE_AG_MON1"), WS_OVERLAPPEDWINDOW,
			OTE_AG_MON1_WND_X, OTE_AG_MON1_WND_Y, OTE_AG_MON1_WND_W, OTE_AG_MON1_WND_H,
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
		wcex.lpszMenuName = TEXT("OTE_AG_MON2");
		wcex.lpszClassName = TEXT("OTE_AG_MON2");
		wcex.hIconSm = NULL;

		ATOM fb = RegisterClassExW(&wcex);

		st_mon2.hwnd_mon = CreateWindowW(TEXT("OTE_AG_MON2"), TEXT("OTE_AG_MON2"), WS_OVERLAPPEDWINDOW,
			OTE_AG_MON2_WND_X, OTE_AG_MON2_WND_Y, OTE_AG_MON2_WND_W, OTE_AG_MON2_WND_H,
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
void COteAgent::close_monitor_wnd(int id) {
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
void COteAgent::show_monitor_wnd(int id) {
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
void COteAgent::hide_monitor_wnd(int id) {
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

///###	�^�u�p�l����ListView�Ƀ��b�Z�[�W���o��
void COteAgent::msg2listview(wstring wstr) {

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
void COteAgent::set_PNLparam_value(float p1, float p2, float p3, float p4, float p5, float p6) {
	wstring wstr;
	wstr += std::to_wstring(p1); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT1), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p2); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT2), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p3); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT3), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p4); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT4), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p5); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT5), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p6); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT6), wstr.c_str());
}
//�^�u�p�l����Edit Box�����e�L�X�g��ݒ�
void COteAgent::set_panel_tip_txt() {
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
void COteAgent::set_func_pb_txt() {
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO1, L"MKCC IF");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO2, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO3, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO4, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO5, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO6, L"-");
	return;
}
//�^�u�p�l����Item chk�e�L�X�g��ݒ�
void COteAgent::set_item_chk_txt() {
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
