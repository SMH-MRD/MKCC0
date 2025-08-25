#include "CSockLib.H"
#include "CAuxCS.h"
#include "resource.h"
#include "framework.h"
#include "AUXEQ_DEF.H"

//ソケット
static CSockUDP* pUSockAuxCs;	//ユニキャストOTE通信受信用

ST_CS_MON1 CAuxCS::st_mon1;
ST_CS_MON2 CAuxCS::st_mon2;

ST_CC_CS_INF CAuxCS::st_work;

//共有メモリ参照用定義
static CSharedMem* pEnvInfObj;
static CSharedMem* pAgentInfObj;
static CSharedMem* pCsInfObj;

static LPST_AUX_ENV_INF		pEnvInf;
static LPST_AUX_CS_INF		pCsInf;
static LPST_AUX_AGENT_INF	pAgentInf;

static LONG rcv_count_u = 0, snd_count_u = 0;

/****************************************************************************/
/*   デフォルト関数											                    */
/****************************************************************************/
CAuxCS::CAuxCS() {
	// 共有メモリオブジェクトのインスタンス化
	pEnvInfObj	= new CSharedMem;
	pAgentInfObj= new CSharedMem;
	pCsInfObj	= new CSharedMem;
}

CAuxCS::~CAuxCS() {
	// 共有メモリオブジェクトの解放
	delete pEnvInfObj; 
	delete pAgentInfObj;
	delete pCsInfObj;
}

HRESULT CAuxCS::initialize(LPVOID lpParam) {
	HRESULT hr = S_OK;
	//### 出力用共有メモリ取得
	out_size = sizeof(ST_AUX_CS_INF);
	if (OK_SHMEM != pCsInfObj->create_smem(SMEM_AUX_CS_INF_NAME, sizeof(ST_AUX_CS_INF), MUTEX_AUX_CS_INF_NAME)) {
		return(FALSE);
	}
	set_outbuf(pCsInfObj->get_pMap());

	//### 入力用共有メモリ取得
	if (OK_SHMEM != pEnvInfObj->create_smem(SMEM_AUX_ENV_INF_NAME, sizeof(ST_AUX_ENV_INF), MUTEX_AUX_ENV_INF_NAME)) {
		return(FALSE);
	}
	if (OK_SHMEM != pAgentInfObj->create_smem(SMEM_AUX_AGENT_INF_NAME, sizeof(ST_AUX_AGENT_INF), MUTEX_AUX_AGENT_INF_NAME)) {
		return(FALSE);
	}

	pEnvInf		= (LPST_AUX_ENV_INF)(pEnvInfObj->get_pMap());
	pAgentInf	= (LPST_AUX_AGENT_INF)(pAgentInfObj->get_pMap());
	pCsInf		= (LPST_AUX_CS_INF)pCsInfObj->get_pMap();

	if ((pEnvInf == NULL) || (pAgentInf == NULL) || (pCsInf == NULL))
		hr = S_FALSE;

	if (hr == S_FALSE) {
		wos.str(L""); wos << L"Initialize : SMEM NG"; msg2listview(wos.str());
		return hr;
	};

	//### IFウィンドウOPEN
	WPARAM wp = MAKELONG(inf.index, WM_USER_WPH_OPEN_IF_WND);//HWORD:コマンドコード, LWORD:タスクインデックス
	LPARAM lp = BC_ID_MON2;
	SendMessage(inf.hwnd_opepane, WM_USER_TASK_REQ, wp, lp);

	//### 通信ソケットアドレスセット
	//##インスタンス生成
	pUSockAuxCs = new CSockUDP(ACCESS_TYPE_SERVER, ID_SOCK_EVENT_AUXCS_UNI);//#OTEユニキャスト受信
	//受信アドレス
	pUSockAuxCs->set_sock_addr(&pUSockAuxCs->addr_in_rcv, IP_ADDR_AUX_SERVER, IP_PORT_AUX_CS_SERVER);
	//送信先アドレス
	pUSockAuxCs->set_sock_addr(&(pUSockAuxCs->addr_in_dst), IP_ADDR_AUX_CLIENT, IP_PORT_AUX_CS_CLIENT);

	Sleep(1000);
	if (st_mon2.hwnd_mon == NULL) {
		wos << L"Err(MON2 NULL Handle!!):";
		msg2listview(wos.str()); wos.str(L"");
		return S_FALSE;
	}

#if 0
	//### 通信ソケット生成/初期化
	//##WSA初期化
	wos.str(L"");
	if (pUSockAuxCs->Initialize() != S_OK) { wos << L"Err(IniWSA):" << pUSockAuxCs->err_msg.str(); err |= SOCK_NG_UNICAST;   hr = S_FALSE; }

	if (hr == S_FALSE)msg2listview(wos.str()); wos.str(L"");

	//##ソケットソケット生成・設定
	//##ユニキャスト
	if (pUSockAuxCs->init_sock(st_mon2.hwnd_mon, pUSockAuxCs->addr_in_rcv) != S_OK) {//init_sock():bind()→非同期化まで実施
		wos << L"CS U SockErr:" << pUSockAuxCs->err_msg.str(); err |= SOCK_NG_UNICAST; hr = S_FALSE;
	}
	else wos << L"CS U Socket init OK"; msg2listview(wos.str()); wos.str(L"");

	//送信メッセージヘッダ設定（送信元受信アドレス：受信先の折り返し用）
	pCsInf->st_msg_u_snd.head.sockaddr = pUSockAuxCs->addr_in_rcv;

	if (hr == S_FALSE) {
		pUSockAuxCs->Close();				//ソケットクローズ
		close_monitor_wnd(BC_ID_MON2);		//通信モニタクローズ
		wos.str(L""); wos << L"Initialize : SOCKET NG"; msg2listview(wos.str());
		return hr;
	};
#endif
	//###  オペレーションパネル設定
	//Function mode RADIO1
	inf.panel_func_id = IDC_TASK_FUNC_RADIO1;
	SendMessage(GetDlgItem(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO1), BM_SETCHECK, BST_CHECKED, 0L);
	for (int i = 1; i < 6; i++)
		SendMessage(GetDlgItem(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO1 + i), BM_SETCHECK, BST_UNCHECKED, 0L);
	//モード設定0
	inf.mode_id = BC_ID_MODE0;
	SendMessage(GetDlgItem(inf.hwnd_opepane, IDC_TASK_MODE_RADIO0), BM_SETCHECK, BST_CHECKED, 0L);
	//モニタウィンドウテキスト	
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_MON_CHECK2, L"MAIN IF");
	set_item_chk_txt();
	set_panel_tip_txt();
	//モニタ2 CB状態セット	
	if (st_mon2.hwnd_mon != NULL)
		SendMessage(GetDlgItem(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK1), BM_SETCHECK, BST_CHECKED, 0L);
	else
		SendMessage(GetDlgItem(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK1), BM_SETCHECK, BST_UNCHECKED, 0L);

	return hr;
}

HRESULT CAuxCS::routine_work(void* pObj) {
	input();
	parse();
	output();
	return S_OK;
}

int CAuxCS::input() {
	return S_OK;
}

int CAuxCS::parse() {
	return S_OK;
}

int CAuxCS::close() {
	return 0;
}

/****************************************************************************/
/*   通信関数											                    */
/****************************************************************************/
/// <summary>
/// MKCCユニキャスト電文受信処理
/// </summary>
HRESULT CAuxCS::rcv_uni_main(LPST_AUX_COM_CLI_MSG pbuf) {
	int nRtn = pUSockAuxCs->rcv_msg((char*)pbuf, sizeof(ST_PC_U_MSG));
	if (nRtn == SOCKET_ERROR) {
		if (st_mon2.sock_inf_id == CS_ID_MON2_RADIO_RCV) {
			st_mon2.wo_uni.str(L""); st_mon2.wo_uni << L"ERR rcv:" << pUSockAuxCs->err_msg.str();
			SetWindowText(st_mon2.hctrl[CS_ID_MON2_STATIC_UNI], st_mon2.wo_uni.str().c_str());
			return S_FALSE;
		}
	}
	rcv_count_u++;
	return S_OK;
}

/****************************************************************************/
/// <summary>
/// AUXEQユニキャスト電文送信処理 
/// </summary>

#if 0
LPST_AUX_COM_SERV_MSG CAuxCS::set_msg_u(BOOL is_monitor_mode, INT32 code, INT32 stat) {
	return &pCsInf->st_msg_u_snd;
}

HRESULT CAuxCS::snd_uni2main(LPST_AUX_COM_SERV_MSG pbuf, SOCKADDR_IN* p_addrin_to) {

	if (pUSockAuxCs->snd_msg((char*)pbuf, sizeof(ST_AUX_COM_SERV_MSG), *p_addrin_to) == SOCKET_ERROR) {
		if (st_mon2.sock_inf_id == CS_ID_MON2_RADIO_SND) {
			st_mon2.wo_uni.str(L""); st_mon2.wo_uni << L"ERR snd:" << pUSockAuxCs->err_msg.str();
			SetWindowText(st_mon2.hctrl[CS_ID_MON2_STATIC_UNI], st_mon2.wo_uni.str().c_str());
		}
		return S_FALSE;
	}
	snd_count_u++;
	return S_OK;
}
#endif
/****************************************************************************/
/*   モニタウィンドウ									                    */
/****************************************************************************/

LRESULT CALLBACK CAuxCS::Mon1Proc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
	switch (msg)
	{
	case WM_CREATE: {
		InitCommonControls();//コモンコントロール初期化
		HINSTANCE hInst = (HINSTANCE)GetModuleHandle(0);
		//ウィンドウにコントロール追加
		st_mon1.hctrl[CS_ID_MON1_STATIC_1] = CreateWindowW(TEXT("STATIC"), st_mon1.text[CS_ID_MON1_STATIC_1], WS_CHILD | WS_VISIBLE | SS_LEFT,
			st_mon1.pt[CS_ID_MON1_STATIC_1].x, st_mon1.pt[CS_ID_MON1_STATIC_1].y,
			st_mon1.sz[CS_ID_MON1_STATIC_1].cx, st_mon1.sz[CS_ID_MON1_STATIC_1].cy,
			hWnd, (HMENU)(CS_ID_MON1_CTRL_BASE + CS_ID_MON1_STATIC_1), hInst, NULL);

		//表示更新用タイマー
		SetTimer(hWnd, CS_ID_MON1_TIMER, st_mon1.timer_ms, NULL);

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
		KillTimer(hWnd, CS_ID_MON1_TIMER);
	}break;
	default:
		return DefWindowProc(hWnd, msg, wp, lp);
	}
	return S_OK;
};

static wostringstream mon2wos;
LRESULT CALLBACK CAuxCS::Mon2Proc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
	switch (msg)
	{
	case WM_CREATE: {
		InitCommonControls();//コモンコントロール初期化
		HINSTANCE hInst = (HINSTANCE)GetModuleHandle(0);
		//ウィンドウにコントロール追加
		//STATIC,LABEL
		for (int i = CS_ID_MON2_STATIC_UNI; i <= CS_ID_MON2_STATIC_MSG; i++) {
			st_mon2.hctrl[i] = CreateWindowW(TEXT("STATIC"), st_mon2.text[i], WS_CHILD | WS_VISIBLE | SS_LEFT,
				st_mon2.pt[i].x, st_mon2.pt[i].y, st_mon2.sz[i].cx, st_mon2.sz[i].cy,
				hWnd, (HMENU)(CS_ID_MON2_CTRL_BASE + i), hInst, NULL);
		}
		//RADIO PB
		for (int i = CS_ID_MON2_RADIO_RCV; i <= CS_ID_MON2_RADIO_INFO; i++) {
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

		UINT rtn = SetTimer(hWnd, CS_ID_MON2_TIMER, CS_PRM_MON2_TIMER_MS, NULL);

	}break;
	case WM_COMMAND: {
		int wmId = LOWORD(wp);

		int _Id = wmId - CS_ID_MON2_CTRL_BASE;
		// 選択されたメニューの解析:
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
		default:
			return DefWindowProc(hWnd, msg, wp, lp);
		}
	}break;
	case WM_TIMER: {
		//UniCast送信
		//折り返し送信
		//通信カウントをタイトルバーに表示
		st_mon2.wo_work.str(L""); st_mon2.wo_work << L"MKCC_IF% PC_U (R:" << rcv_count_u << L" S:" << snd_count_u  << L")";
		SetWindowText(st_mon2.hwnd_mon, st_mon2.wo_work.str().c_str());

		//モニター表示
		if (st_mon2.is_monitor_active) {
			SOCKADDR_IN	addr;
			if (pUSockAuxCs != NULL) {
				addr = pUSockAuxCs->addr_in_rcv; st_mon2.wo_work.str(L"");
				st_mon2.wo_work << L"UNI>>IP R:" << addr.sin_addr.S_un.S_un_b.s_b1 << L"." << addr.sin_addr.S_un.S_un_b.s_b2 << L"." << addr.sin_addr.S_un.S_un_b.s_b3 << L"." << addr.sin_addr.S_un.S_un_b.s_b4 << L":"
					<< htons(addr.sin_port) << L" ";
				addr = pUSockAuxCs->addr_in_dst;
				st_mon2.wo_work << L" S:" << addr.sin_addr.S_un.S_un_b.s_b1 << L"." << addr.sin_addr.S_un.S_un_b.s_b2 << L"." << addr.sin_addr.S_un.S_un_b.s_b3 << L"." << addr.sin_addr.S_un.S_un_b.s_b4 << L":"
					<< htons(addr.sin_port) << L" ";
				addr = pUSockAuxCs->addr_in_from; ;
				st_mon2.wo_work << L" F:" << addr.sin_addr.S_un.S_un_b.s_b1 << L"." << addr.sin_addr.S_un.S_un_b.s_b2 << L"." << addr.sin_addr.S_un.S_un_b.s_b3 << L"." << addr.sin_addr.S_un.S_un_b.s_b4 << L":"
					<< htons(addr.sin_port) << L" ";
				SetWindowText(st_mon2.hctrl[CS_ID_MON2_LABEL_UNI], st_mon2.wo_work.str().c_str());
			}
#if 0
			st_mon2.wo_uni.str(L"");
			if (st_mon2.sock_inf_id == CS_ID_MON2_RADIO_RCV) {
				LPST_AUX_COM_MSG_HEAD	ph0 = &pCsInf->st_msg_u_rcv.head;
				LPST_AUX_COM_CLI_BODY	pb0 = &pCsInf->st_msg_u_rcv.body;
				st_mon2.wo_uni << L"[HEAD]" << L"CODE:" << ph0->id << L"\n";
				st_mon2.wo_uni << L"[BODY]" << L"OPEMODE:" << pb0->mode;
			}
			else if (st_mon2.sock_inf_id == CS_ID_MON2_RADIO_SND) {
				LPST_AUX_COM_MSG_HEAD	ph0 = &pCsInf->st_msg_u_snd.head;
				LPST_AUX_COM_SERV_BODY  pb0 = &pCsInf->st_msg_u_snd.body;
				st_mon2.wo_uni << L"[HEAD]" << L"CODE:" << ph0->id << L"\n";
				st_mon2.wo_uni << L"[BODY]";
			}
			else {
				st_mon2.wo_uni << L"No Message";
			}
			SetWindowText(st_mon2.hctrl[CS_ID_MON2_STATIC_UNI], st_mon2.wo_uni.str().c_str());
#endif
		}
	}break;

	case ID_SOCK_EVENT_AUXCS_UNI: {
#if 0
		int nEvent = WSAGETSELECTEVENT(lp);
		switch (nEvent) {
		case FD_READ: {
			//OTEからのユニキャストメッセージ受信
			if (rcv_uni_main(&pCsInf->st_msg_u_rcv) == S_OK) {
				//折り返しアンサバック 送信元へ返送
				pCsInf->addrin_from = pUSockAuxCs->addr_in_from;
				pUSockAuxCs->addr_in_dst.sin_family = AF_INET;
				pUSockAuxCs->addr_in_dst.sin_port = htons(IP_PORT_AUX_CS_CLIENT);
				pUSockAuxCs->addr_in_dst.sin_addr = pUSockAuxCs->addr_in_from.sin_addr;

				snd_uni2main(set_msg_u(true, 0, 0), &pUSockAuxCs->addr_in_dst);
			}
		}break;
		case FD_WRITE: break;
		case FD_CLOSE: break;
		}
#endif
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

HWND CAuxCS::open_monitor_wnd(HWND h_parent_wnd, int id) {

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
	else if (id == BC_ID_MON2) {//通信用ウィンドウ
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

		//show_monitor_wnd(id);MON2はオープン時表示しない

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
void CAuxCS::close_monitor_wnd(int id) {
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
void CAuxCS::show_monitor_wnd(int id) {
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
void CAuxCS::hide_monitor_wnd(int id) {
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
LRESULT CALLBACK CAuxCS::PanelProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp) {

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

///###	タブパネルのListViewにメッセージを出力
void CAuxCS::msg2listview(wstring wstr) {

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
void CAuxCS::set_PNLparam_value(float p1, float p2, float p3, float p4, float p5, float p6) {
	wstring wstr;
	wstr += std::to_wstring(p1); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT1), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p2); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT2), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p3); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT3), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p4); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT4), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p5); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT5), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p6); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT6), wstr.c_str());
}
//タブパネルのEdit Box説明テキストを設定
void CAuxCS::set_panel_tip_txt() {
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
void CAuxCS::set_func_pb_txt() {
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO1, L"OTE IF");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO2, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO3, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO4, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO5, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO6, L"-");
	return;
}
//タブパネルのItem chkテキストを設定
void CAuxCS::set_item_chk_txt() {
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

