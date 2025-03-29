#include "CSockLib.H"
#include "CCcEnv.h"
#include "resource.h"
#include "framework.h"
#include "AUXEQ_DEF.H"
#include "CCraneLib.H"

//ソケット
static CSockUDP* pUSockCcEnv;	//ユニキャストOTE通信受信用

ST_ENV_MON1 CCcEnv::st_mon1;
ST_ENV_MON2 CCcEnv::st_mon2;

ST_CC_ENV_INF CCcEnv::st_work;

//共有メモリクラスオブジェクト
static CSharedMem* pEnvInfObj;
static CSharedMem* pPlcIoObj;
static CSharedMem* pJobIoObj;
static CSharedMem* pPolInfObj;
static CSharedMem* pAgInfObj;
static CSharedMem* pCsInfObj;
static CSharedMem* pSimuStatObj;
static CSharedMem* pOteIoObj;

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
/*   デフォルト関数											                */
/****************************************************************************/

CCcEnv::CCcEnv() {
	pEnvInfObj = new CSharedMem;
	pPlcIoObj = new CSharedMem;
	pJobIoObj = new CSharedMem;
	pPolInfObj = new CSharedMem;
	pAgInfObj = new CSharedMem;
	pCsInfObj = new CSharedMem;
	pSimuStatObj = new CSharedMem;
	pOteIoObj = new CSharedMem;
}
CCcEnv::~CCcEnv() {
	delete pEnvInfObj;
	delete pPlcIoObj;
	delete pJobIoObj;
	delete pPolInfObj;
	delete pAgInfObj;
	delete pCsInfObj;
	delete pSimuStatObj;
	delete pOteIoObj;
}

HRESULT CCcEnv::initialize(LPVOID lpParam) {
	HRESULT hr = S_OK;

	if (OK_SHMEM != pEnvInfObj->create_smem(SMEM_CRANE_STAT_CC_NAME, sizeof(ST_CC_ENV_INF), MUTEX_CRANE_STAT_CC_NAME)) return(FALSE);
	if (OK_SHMEM != pPlcIoObj->create_smem(SMEM_PLC_IO_NAME, sizeof(ST_CC_PLC_IO), MUTEX_PLC_IO_NAME)) return(FALSE);
	if (OK_SHMEM != pJobIoObj->create_smem(SMEM_JOB_IO_NAME, sizeof(ST_JOB_IO), MUTEX_JOB_IO_NAME)) return(FALSE);
	if (OK_SHMEM != pPolInfObj->create_smem(SMEM_POL_INF_CC_NAME, sizeof(ST_CC_POL_INF), MUTEX_POL_INF_CC_NAME)) return(FALSE);
	if (OK_SHMEM != pAgInfObj->create_smem(SMEM_AGENT_INF_CC_NAME, sizeof(ST_CC_AGENT_INF), MUTEX_AGENT_INF_CC_NAME)) return(FALSE);
	if (OK_SHMEM != pCsInfObj->create_smem(SMEM_CS_INF_CC_NAME, sizeof(ST_CC_CS_INF), MUTEX_CS_INF_CC_NAME)) return(FALSE);
	if (OK_SHMEM != pSimuStatObj->create_smem(SMEM_SIM_INF_CC_NAME, sizeof(ST_CC_SIM_INF), MUTEX_SIM_INF_CC_NAME)) return(FALSE);
	if (OK_SHMEM != pOteIoObj->create_smem(SMEM_OTE_INF_NAME, sizeof(ST_CC_OTE_INF), MUTEX_OTE_INF_NAME)) return(FALSE);

	pEnvInf		= (LPST_CC_ENV_INF)(pEnvInfObj->get_pMap());
	pPlcIo		= (LPST_CC_PLC_IO)(pPlcIoObj->get_pMap());
	pJobIo		= (LPST_JOB_IO)(pJobIoObj->get_pMap());
	pPolInf		= (LPST_CC_POL_INF)(pPolInfObj->get_pMap());
	pAgentInf	= (LPST_CC_AGENT_INF)(pAgInfObj->get_pMap());
	pCsInf		= (LPST_CC_CS_INF)(pCsInfObj->get_pMap());
	pSimInf		= (LPST_CC_SIM_INF)(pSimuStatObj->get_pMap());
	pOteInf		= (LPST_CC_OTE_INF)(pOteIoObj->get_pMap());

	if ((pEnvInf == NULL) || (pPlcIo == NULL) || (pJobIo == NULL) || (pPolInf == NULL) || (pAgentInf == NULL) || (pCsInf == NULL) || (pSimInf == NULL) || (pOteInf == NULL)) {
		hr = S_FALSE;
		wos.str(L""); wos << L"Initialize : SMEM NG"; msg2listview(wos.str());
		return hr;
	}


	//### IFウィンドウOPEN
	WPARAM wp = MAKELONG(inf.index, WM_USER_WPH_OPEN_IF_WND);//HWORD:コマンドコード, LWORD:タスクインデックス
	LPARAM lp = BC_ID_MON2;
	SendMessage(inf.hwnd_opepane, WM_USER_TASK_REQ, wp, lp);

	//### 通信ソケットアドレスセット
	//##インスタンス生成
	pUSockCcEnv = new CSockUDP(ACCESS_TYPE_CLIENT, ID_SOCK_EVENT_CC_ENV_UNI);//#OTEユニキャスト受信
	//受信アドレス
	pUSockCcEnv->set_sock_addr(&pUSockCcEnv->addr_in_rcv, IP_ADDR_AUX_CLIENT, IP_PORT_AUX_CS_CLIENT);
	//送信先アドレス
	pUSockCcEnv->set_sock_addr(&(pUSockCcEnv->addr_in_dst), IP_ADDR_AUX_SERVER, IP_PORT_AUX_CS_SERVER);

	Sleep(1000);
	if (st_mon2.hwnd_mon == NULL) {
		wos << L"Err(MON2 NULL Handle!!):";
		msg2listview(wos.str()); wos.str(L"");
		return S_FALSE;
	}
	//### 通信ソケット生成/初期化
	//##WSA初期化
	wos.str(L"");
	if (pUSockCcEnv->Initialize() != S_OK) { wos << L"Err(IniWSA):" << pUSockCcEnv->err_msg.str(); err |= SOCK_NG_UNICAST;   hr = S_FALSE; }

	if (hr == S_FALSE)msg2listview(wos.str()); wos.str(L"");

	//##ソケットソケット生成・設定
	//##ユニキャスト
	if (pUSockCcEnv->init_sock(st_mon2.hwnd_mon, pUSockCcEnv->addr_in_rcv) != S_OK) {//init_sock():bind()→非同期化まで実施
		wos << L"CS U SockErr:" << pUSockCcEnv->err_msg.str(); err |= SOCK_NG_UNICAST; hr = S_FALSE;
	}
	else wos << L"CS U Socket init OK"; msg2listview(wos.str()); wos.str(L"");

	//送信メッセージヘッダ設定（送信元受信アドレス：受信先の折り返し用）
	pEnvInf->st_msg_u_snd.head.sockaddr = pUSockCcEnv->addr_in_rcv;
	if (hr == S_FALSE) {
		pUSockCcEnv->Close();				//ソケットクローズ
		close_monitor_wnd(BC_ID_MON2);		//通信モニタクローズ
		wos.str(L""); wos << L"Initialize : SOCKET NG"; msg2listview(wos.str());
		return hr;
	};

	//###  オペレーションパネル設定
	set_func_pb_txt();

	inf.panel_func_id = IDC_TASK_FUNC_RADIO1;
	SendMessage(GetDlgItem(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO1), BM_SETCHECK, BST_CHECKED, 0L);
	for (int i = 1; i < 6; i++)
		SendMessage(GetDlgItem(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO1 + i), BM_SETCHECK, BST_UNCHECKED, 0L);
	//モード設定0
	inf.mode_id = BC_ID_MODE0;
	SendMessage(GetDlgItem(inf.hwnd_opepane, IDC_TASK_MODE_RADIO0), BM_SETCHECK, BST_CHECKED, 0L);
	//モニタウィンドウテキスト	
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_MON_CHECK2, L"AUX IF");
	set_item_chk_txt();
	set_panel_tip_txt();
	
	//モニタ2 CB状態セット	
	if (st_mon2.hwnd_mon != NULL)
		SendMessage(GetDlgItem(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK1), BM_SETCHECK, BST_CHECKED, 0L);
	else
		SendMessage(GetDlgItem(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK1), BM_SETCHECK, BST_UNCHECKED, 0L);

	CCcEnv* pEnvObj = (CCcEnv*)lpParam;
	int code = 0;

	pEnvInf->aux_mode = FUNC_ACTIVE;

	return S_OK;
}

HRESULT CCcEnv::routine_work(void* pObj) {
	input();
	parse();
	output();
	return S_OK;
}

static UINT32	gpad_mode_last = L_OFF;

int CCcEnv::input() {
	return S_OK;
}

int CCcEnv::close() {
	return 0;
}

/****************************************************************************/
/*   通信関数											                    */
/****************************************************************************/
/// <summary>
/// AUXEQユニキャスト電文受信処理
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
/// PCユニキャスト電文送信処理 
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
/*   モニタウィンドウ									                    */
/****************************************************************************/
static wostringstream monwos;

LRESULT CALLBACK CCcEnv::Mon1Proc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
	switch (msg)
	{
	case WM_CREATE: {
		InitCommonControls();//コモンコントロール初期化
		HINSTANCE hInst = (HINSTANCE)GetModuleHandle(0);
		//ウィンドウにコントロール追加
		st_mon1.hctrl[ENV_ID_MON1_STATIC_GPAD] = CreateWindowW(TEXT("STATIC"), st_mon1.text[ENV_ID_MON1_STATIC_GPAD], WS_CHILD | WS_VISIBLE | SS_LEFT,
			st_mon1.pt[ENV_ID_MON1_STATIC_GPAD].x, st_mon1.pt[ENV_ID_MON1_STATIC_GPAD].y,
			st_mon1.sz[ENV_ID_MON1_STATIC_GPAD].cx, st_mon1.sz[ENV_ID_MON1_STATIC_GPAD].cy,
			hWnd, (HMENU)(ENV_ID_MON1_CTRL_BASE + ENV_ID_MON1_STATIC_GPAD), hInst, NULL);

		//表示更新用タイマー
		SetTimer(hWnd, ENV_ID_MON1_TIMER, st_mon1.timer_ms, NULL);

		break;
	}
	case WM_COMMAND: {
		int wmId = LOWORD(wp);
		// 選択されたメニューの解析:
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
		InitCommonControls();//コモンコントロール初期化
		HINSTANCE hInst = (HINSTANCE)GetModuleHandle(0);
		//ウィンドウにコントロール追加
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

		UINT rtn = SetTimer(hWnd, ENV_ID_MON2_TIMER, ENV_PRM_MON2_TIMER_MS, NULL);
		break;

	}

	case WM_TIMER: {
		//UniCast送信
		if ((pEnvInf->aux_mode != FUNC_DEACTIVE) && (pEnvInf->aux_mode != FUNC_PAUSE)) {
			snd_uni2aux(set_msg_u(false, 0, 0), &pUSockCcEnv->addr_in_dst);
		}

		//通信カウントをタイトルバーに表示
		st_mon2.wo_work.str(L""); st_mon2.wo_work << L"AUX_IF% PC_U (R:" << rcv_count_u << L" S:" << snd_count_u << L")" ;
		SetWindowText(st_mon2.hwnd_mon, st_mon2.wo_work.str().c_str());

		//モニター表示
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
			//補機プロセスからのユニキャストメッセージ受信
			rcv_uni_aux(&pEnvInf->st_msg_u_rcv);
		}break;
		case FD_WRITE: break;
		case FD_CLOSE: break;
		}
	}break;

	case WM_COMMAND: {
		int wmId = LOWORD(wp);
		// 選択されたメニューの解析:
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

	InitCommonControls();//コモンコントロール初期化
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
/*   タスク設定タブパネルウィンドウのコールバック関数                       */
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

///###	タブパネルのListViewにメッセージを出力
void CCcEnv::msg2listview(wstring wstr) {

	const wchar_t* pwc; pwc = wstr.c_str();

	inf.hwnd_msglist = GetDlgItem(inf.hwnd_opepane, IDC_LIST1);
	LVITEM item;

	item.mask = LVIF_TEXT;
	item.pszText = (wchar_t*)pwc;								// テキスト
	item.iItem = inf.panel_msglist_count % BC_LISTVIEW_ROW_MAX;	// 番号
	item.iSubItem = 1;											// サブアイテムの番号
	ListView_SetItem(inf.hwnd_msglist, &item);

	SYSTEMTIME st; TCHAR tbuf[32];
	::GetLocalTime(&st);
	wsprintf(tbuf, L"%02d:%02d:%02d.%01d", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds / 100);

	item.pszText = tbuf;   // テキスト
	item.iSubItem = 0;											// サブアイテムの番号
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
//タブパネルのEdit Box説明テキストを設定
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
//タブパネルのFunctionボタンのStaticテキストを設定
void CCcEnv::set_func_pb_txt() {
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO1, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO2, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO3, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO4, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO5, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO6, L"-");
	return;
}
//タブパネルのItem chkテキストを設定
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




