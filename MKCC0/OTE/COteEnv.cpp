#include "COteEnv.h"
#include "resource.h"
#include "CCrane.h"
#include "COteScad.h"
#include "COteAgent.h"

extern vector<CBasicControl*>	VectCtrlObj;
extern BC_TASK_ID st_task_id;

extern CSharedMem* pOteEnvInfObj;
extern CSharedMem* pOteCsInfObj;
extern CSharedMem* pOteCcInfObj;
extern CSharedMem* pOteUiObj;

extern CCrane* pCrane;
extern ST_DEVICE_CODE g_my_code;

ST_OTE_ENV_MON1 COteEnv::st_mon1;
ST_OTE_ENV_MON2 COteEnv::st_mon2;

ST_OTE_ENV_INF COteEnv::st_work;

static LPST_OTE_ENV_INF	pOteEnvInf;
static LPST_OTE_UI		pOteUI;
static LPST_OTE_CC_IF	pOteCCIf;
static LPST_OTE_CS_INF	pOteCsInf;
static COteScad* pScadObj;
static COteAgent* pAgentObj;

static LARGE_INTEGER start_count_s, end_count_r;			//システムカウント
static LARGE_INTEGER frequency;								//システム周波数
static LONGLONG res_delay_max, res_delay_min = 10000000;	//C応答時間

COteEnv::COteEnv(){
}
COteEnv::~COteEnv() {
}

HRESULT COteEnv::initialize(LPVOID lpParam) {

	//システム周波数読み込み
	QueryPerformanceFrequency(&frequency);

	HRESULT hr = S_OK;

	//### SCADAクラスインスタンスのポインタ取得
	pScadObj = (COteScad*)VectCtrlObj[st_task_id.SCAD];
	//### AGENTクラスインスタンスのポインタ取得
	pAgentObj = (COteAgent*)VectCtrlObj[st_task_id.AGENT];

	//### 共有メモリ取得
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

	//### OpeningウィンドウOPEN
	open_opening_window();

	//### 出力用共有メモリ取得
	set_func_pb_txt();
	set_item_chk_txt();
	set_panel_tip_txt();
	 
	inf.panel_func_id = IDC_TASK_FUNC_RADIO1;
	SendMessage(GetDlgItem(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO1), BM_SETCHECK, BST_CHECKED, 0L);
	for(int i=1;i<6;i++)
		SendMessage(GetDlgItem(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO1+i), BM_SETCHECK, BST_UNCHECKED, 0L);
	//モード設定0
	inf.mode_id = BC_ID_MODE0;
	SendMessage(GetDlgItem(inf.hwnd_opepane, IDC_TASK_MODE_RADIO0), BM_SETCHECK, BST_CHECKED, 0L);

	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_MODE_RADIO0, L"Product");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_MODE_RADIO1, L"Debug1");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_MODE_RADIO2, L"Debug2");

	COteEnv* pEnvObj = (COteEnv*)lpParam;
	int code = 0;

	st_work.device_code = g_my_code;
	st_work.ote_type = g_my_code.option;


	return S_OK;
}

HRESULT COteEnv::routine_work(void* pObj) {
	if (inf.total_act % 20 == 0) {
		wos.str(L""); wos << inf.status << L":" << std::setfill(L'0') << std::setw(4) << inf.act_time;
		if (pOteEnvInf->app_mode == OTE_ENV_APP_PRODUCT)		wos << L" MODE>>PRODUCT";
		else if (pOteEnvInf->app_mode == OTE_ENV_APP_DEBUG_TYPE1)	wos << L" MODE>>DEBUG1";
		else if (pOteEnvInf->app_mode == OTE_ENV_APP_DEBUG_TYPE2)	wos << L" MODE>>DEBUG2";
		else											wos << L" MODE>>??";
		msg2host(wos.str());
	}
	input();
	parse();
	output();
	return S_OK;
}

static UINT32	gpad_mode_last = L_OFF;

int COteEnv::input() {

	return S_OK;
}

static int got_command_last = CRANE_ID_NULL;
int COteEnv::parse() {
	//GOT 接続コマンド処理
	if (pOteCsInf->GOT_command > got_command_last) {
		int command = pOteCsInf->GOT_command ^ got_command_last;


		if (command & OTE_OPE_GOT_COM_CONNECT_CRANE) {
			if (pOteCCIf->id_conected_crane == CRANE_ID_NULL) {//クレーン接続済でなければ
				//HRESULT hr=open_ope_window(pOteCsInf->GOT_crane_select);

				WPARAM wp = pOteCsInf->GOT_crane_select;
				LPARAM lp = BC_ID_MON1;
				SendMessage(st_mon1.hwnd_mon, WM_USER_OPEN_CRANE_WND, wp, lp);
			}

		}
	}

	return STAT_OK;
}

int COteEnv::close() {

	return 0;
}

int COteEnv::output() {
	memcpy_s(pOteEnvInf, sizeof(ST_OTE_ENV_INF), &st_work, sizeof(ST_OTE_ENV_INF));
	return STAT_OK;
}

HRESULT COteEnv::open_opening_window() {
	if (st_mon1.hwnd_mon != NULL) return S_FALSE;

	WPARAM wp = MAKELONG(inf.index, WM_USER_WPH_OPEN_IF_WND);//HWORD:コマンドコード, LWORD:タスクインデックス
	LPARAM lp = BC_ID_MON1;
	SendMessage(inf.hwnd_opepane, WM_USER_TASK_REQ, wp, lp);
	return S_OK;
}

HRESULT COteEnv::open_ope_window(int crane_id_selected) {
	delete pCrane;
	if (!(NULL == (pCrane = new CCrane(crane_id_selected))))
	{

		Sleep(100);

		if (S_OK == pAgentObj->setup_crane_if(crane_id_selected))
		{
			pOteEnvInf->selected_crane = st_work.selected_crane = crane_id_selected;
			pScadObj->open_ope_window();
			crane_id_selected = CRANE_ID_NULL;
			close_monitor_wnd(BC_ID_MON1);
		}
		else {
			pAgentObj->close_crane_if();
			pOteEnvInf->selected_crane = st_work.selected_crane = CRANE_ID_NULL;
			MessageBox(st_mon1.hwnd_mon, TEXT("通信回線を確立出来ません"), TEXT("Error"), MB_OK | MB_ICONERROR);
		}
	}
	return S_OK;
}

void COteEnv::clear_crane_if() {
	pAgentObj->close_crane_if();
	pOteEnvInf->selected_crane = st_work.selected_crane = CRANE_ID_NULL;
	return;
}


/****************************************************************************/
/*   モニタウィンドウ									                    */
/****************************************************************************/
static wostringstream monwos;
static int crane_id_selected = CRANE_ID_NULL;
static HBRUSH hBrush;

LRESULT CALLBACK COteEnv::Mon1Proc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
	
	switch (msg)
	{
	case WM_CREATE: {
		InitCommonControls();//コモンコントロール初期化
		HINSTANCE hInst = (HINSTANCE)GetModuleHandle(0);
		//ウィンドウにコントロール追加
		hBrush = CreateSolidBrush(RGB(64, 64, 64)); // ダークグレー
		int i;
		for (i = OTE_ENV_ID_MON1_STATIC_TITLE; i <= OTE_ENV_ID_MON1_STATIC_SELECTED; i++) {
			st_mon1.hctrl[i] = CreateWindowW(TEXT("STATIC"), st_mon1.text[i], WS_CHILD | WS_VISIBLE | SS_LEFT,
				st_mon1.pt[i].x, st_mon1.pt[i].y, st_mon1.sz[i].cx, st_mon1.sz[i].cy,
				hWnd, (HMENU)(OTE_ENV_ID_MON1_CTRL_BASE + i), hInst, NULL);
		}

		//RADIO BUTTON
		for (i = OTE_ENV_ID_MON1_RADIO_CRANE01; i <= OTE_ENV_ID_MON1_RADIO_CRANE10; i++) {
			if (i == OTE_ENV_ID_MON1_RADIO_CRANE01) {
				st_mon1.hctrl[i] = CreateWindowW(TEXT("BUTTON"), st_mon1.text[i], WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON | BS_PUSHLIKE | BS_MULTILINE | WS_GROUP,
					st_mon1.pt[i].x, st_mon1.pt[i].y, st_mon1.sz[i].cx, st_mon1.sz[i].cy,
					hWnd, (HMENU)(OTE_ENV_ID_MON1_CTRL_BASE + i), hInst, NULL);
			}
			else if (i == OTE_ENV_ID_MON1_RADIO_CRANE09) {
				st_mon1.hctrl[i] = CreateWindowW(TEXT("BUTTON"), st_mon1.text[i], WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON | BS_PUSHLIKE | BS_MULTILINE,
					st_mon1.pt[i].x, st_mon1.pt[i].y, st_mon1.sz[i].cx, st_mon1.sz[i].cy,
					hWnd, (HMENU)(OTE_ENV_ID_MON1_CTRL_BASE + i), hInst, NULL);
			}
			else {
				st_mon1.hctrl[i] = CreateWindowW(TEXT("BUTTON"), st_mon1.text[i], WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON | BS_PUSHLIKE | BS_MULTILINE,
					st_mon1.pt[i].x, st_mon1.pt[i].y, st_mon1.sz[i].cx, st_mon1.sz[i].cy,
					hWnd, (HMENU)(OTE_ENV_ID_MON1_CTRL_BASE + i), hInst, NULL);
				
				//!!!! 未実装クレーンは無効化
				EnableWindow(st_mon1.hctrl[i], FALSE);
			}
		}
		//PB
		i = OTE_ENV_ID_MON1_PB_START;
		st_mon1.hctrl[i] = CreateWindowW(TEXT("BUTTON"), st_mon1.text[i], WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON | BS_PUSHLIKE | BS_MULTILINE,
			st_mon1.pt[i].x, st_mon1.pt[i].y, st_mon1.sz[i].cx, st_mon1.sz[i].cy,
			hWnd, (HMENU)(OTE_ENV_ID_MON1_CTRL_BASE + i), hInst, NULL);

		//表示更新用タイマー
		SetTimer(hWnd, OTE_ENV_ID_MON1_TIMER, st_mon1.timer_ms, NULL);

		pOteEnvInf->selected_crane = st_work.selected_crane = crane_id_selected = CRANE_ID_NULL;

		break;
	}
	case WM_COMMAND: {
		int wmId = LOWORD(wp) - OTE_ENV_ID_MON1_CTRL_BASE;
		// 選択されたメニューの解析:
		switch (wmId)
		{
		case OTE_ENV_ID_MON1_RADIO_CRANE01: {
			crane_id_selected = CARNE_ID_HHGH29;
			SetWindowText(st_mon1.hctrl[OTE_ENV_ID_MON1_STATIC_SELECTED], st_mon1.text[OTE_ENV_ID_MON1_RADIO_CRANE01]);
		}break;

		case OTE_ENV_ID_MON1_RADIO_CRANE09: {
			crane_id_selected = CARNE_ID_PC0;
			SetWindowText(st_mon1.hctrl[OTE_ENV_ID_MON1_STATIC_SELECTED], st_mon1.text[OTE_ENV_ID_MON1_RADIO_CRANE09]);
		}break;
		case OTE_ENV_ID_MON1_RADIO_CRANE10: {
			crane_id_selected = CARNE_ID_PC0;
			SetWindowText(st_mon1.hctrl[OTE_ENV_ID_MON1_STATIC_SELECTED], st_mon1.text[OTE_ENV_ID_MON1_RADIO_CRANE10]);
		}break;

		case OTE_ENV_ID_MON1_PB_START: {
			if (pOteUI->pc_pnl_active) {
				MessageBox(hWnd, TEXT("既にクレーンが接続されています！"), TEXT("Error"), MB_OK | MB_ICONERROR);
			}
			else if (crane_id_selected != CRANE_ID_NULL) {
				open_ope_window(crane_id_selected);
			}
			else {
				MessageBox(hWnd, TEXT("クレーンを選択して下さい！"), TEXT("Error"), MB_OK | MB_ICONERROR);
			}
		}break;

		case OTE_ENV_ID_MON1_RADIO_CRANE02:
		case OTE_ENV_ID_MON1_RADIO_CRANE03:
		case OTE_ENV_ID_MON1_RADIO_CRANE04:
		case OTE_ENV_ID_MON1_RADIO_CRANE05:
		case OTE_ENV_ID_MON1_RADIO_CRANE06:
		case OTE_ENV_ID_MON1_RADIO_CRANE07:
		case OTE_ENV_ID_MON1_RADIO_CRANE08:
		{
			crane_id_selected = CRANE_ID_NULL;
			SetWindowText(st_mon1.hctrl[OTE_ENV_ID_MON1_STATIC_SELECTED], st_mon1.text[OTE_ENV_ID_MON1_STATIC_SELECTED]);
		}break;



		default:
			return DefWindowProc(hWnd, msg, wp, lp);
		}
	}break;
	case WM_USER_OPEN_CRANE_WND: {//クレーン操作ウィンドウオープン要求
		open_ope_window(LOWORD(wp));
	}break;
	case WM_CTLCOLORSTATIC:
		SetTextColor((HDC)wp, RGB(0, 150, 220)); // ダークブルー
		SetBkMode((HDC)wp, TRANSPARENT);
		return (LRESULT)hBrush; // 背景色に合わせる

	case WM_ERASEBKGND: {//ウィンドウの背景色をグレーに
		RECT rect;
		GetClientRect(hWnd, &rect);
		FillRect((HDC)wp, &rect, hBrush);
		return 1; // 背景を処理したことを示す
	}
	case WM_TIMER: {
		
	}break;

	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
	}break;
	case WM_CLOSE: {
		DestroyWindow(hWnd);
		PostQuitMessage(0);
	}
	case WM_DESTROY: {
		st_mon1.hwnd_mon = NULL;
		KillTimer(hWnd, OTE_ENV_ID_MON1_TIMER);
		DeleteObject(hBrush);
	}break;
	default:
		return DefWindowProc(hWnd, msg, wp, lp);
	}
	return S_OK;
};

LRESULT CALLBACK COteEnv::Mon2Proc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
	switch (msg)
	{
	case WM_CREATE: {
		InitCommonControls();//コモンコントロール初期化
		HINSTANCE hInst = (HINSTANCE)GetModuleHandle(0);
		//ウィンドウにコントロール追加

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

HWND COteEnv::open_monitor_wnd(HWND h_parent_wnd, int id) {

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
		wcex.lpszMenuName = TEXT("OTE_ENV_MON1");
		wcex.lpszClassName = TEXT("OTE_ENV_MON1");
		wcex.hIconSm = NULL;

		ATOM fb = RegisterClassExW(&wcex);

		st_mon1.hwnd_mon = CreateWindowW(TEXT("OTE_ENV_MON1"), TEXT("Opening"), WS_OVERLAPPEDWINDOW,
			OTE_ENV_MON1_WND_X, OTE_ENV_MON1_WND_Y, OTE_ENV_MON1_WND_W, OTE_ENV_MON1_WND_H,
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
		wcex.lpszMenuName = TEXT("OTE_ENV_MON2");
		wcex.lpszClassName = TEXT("OTE_ENV_MON2");
		wcex.hIconSm = NULL;

		ATOM fb = RegisterClassExW(&wcex);

		st_mon2.hwnd_mon = CreateWindowW(TEXT("OTE_ENV_MON2"), TEXT("OTE_ENV_MON2"), WS_OVERLAPPEDWINDOW,
			OTE_ENV_MON2_WND_X, OTE_ENV_MON2_WND_Y, OTE_ENV_MON2_WND_W, OTE_ENV_MON2_WND_H,
			h_parent_wnd, nullptr, hInst, nullptr);

		show_monitor_wnd(id);
		return st_mon2.hwnd_mon;
	}
	else
	{
		return NULL;
	};

	return NULL;
}

void COteEnv::close_monitor_wnd(int id) {
	if (id == BC_ID_MON1)
		DestroyWindow(st_mon1.hwnd_mon);
	else if (id == BC_ID_MON2)
		DestroyWindow(st_mon2.hwnd_mon);
	else;
	return;
}
void COteEnv::show_monitor_wnd(int id) {
	if (id == BC_ID_MON1) {
		//ShowWindow(st_mon1.hwnd_mon, SW_SHOW);
		ShowWindow(st_mon1.hwnd_mon, SW_MAXIMIZE);
		UpdateWindow(st_mon1.hwnd_mon);
		InvalidateRect(st_mon1.hwnd_mon, NULL, TRUE);
	}
	else if (id == BC_ID_MON2) {
		ShowWindow(st_mon2.hwnd_mon, SW_SHOW);
		UpdateWindow(st_mon2.hwnd_mon);
		InvalidateRect(st_mon1.hwnd_mon, NULL, TRUE);
	}
	else;
	return;
}
void COteEnv::hide_monitor_wnd(int id) {
	if (id == BC_ID_MON1)
		ShowWindow(st_mon1.hwnd_mon, SW_HIDE);
	else if (id == BC_ID_MON2)
		ShowWindow(st_mon2.hwnd_mon, SW_HIDE);
	else;
	return;
}

/****************************************************************************/
/*   タスク設定タブパネルウィンドウのコールバック関数                       */
/****************************************************************************/
LRESULT CALLBACK COteEnv::PanelProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp) {

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
			inf.mode_id = st_work.app_mode = OTE_ENV_APP_PRODUCT;
		}break;
		case IDC_TASK_MODE_RADIO1:
		{
			inf.mode_id = st_work.app_mode = OTE_ENV_APP_DEBUG_TYPE1;
		}break;
		case IDC_TASK_MODE_RADIO2:
		{
			inf.mode_id = st_work.app_mode = OTE_ENV_APP_DEBUG_TYPE2;
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
				open_monitor_wnd(inf.hwnd_parent, BC_ID_MON2);
			}
			else {
				close_monitor_wnd(BC_ID_MON2);
			}
		}break;
		}break;
	case WM_USER_TASK_REQ: {
		if (HIWORD(wp) == WM_USER_WPH_OPEN_IF_WND) {
			wos.str(L"");
			if (lp == BC_ID_MON1) st_mon1.hwnd_mon = open_monitor_wnd(hDlg, lp);
			if (lp == BC_ID_MON2) st_mon2.hwnd_mon = open_monitor_wnd(hDlg, lp);
		}
		else if (wp == WM_USER_WPH_CLOSE_IF_WND) 	close_monitor_wnd(lp);
		else;
	}break;
	default:break;
	}

	return 0;
};

///###	タブパネルのListViewにメッセージを出力
void COteEnv::msg2listview(wstring wstr) {

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
void COteEnv::set_PNLparam_value(float p1, float p2, float p3, float p4, float p5, float p6) {
	wstring wstr;
	wstr += std::to_wstring(p1); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT1), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p2); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT2), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p3); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT3), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p4); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT4), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p5); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT5), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p6); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT6), wstr.c_str());
}
//タブパネルのEdit Box説明テキストを設定
void COteEnv::set_panel_tip_txt() {
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
void COteEnv::set_func_pb_txt() {
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO1, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO2, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO3, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO4, L"GPAD");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO5, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO6, L"-");
	return;
}
//タブパネルのItem chkテキストを設定
void COteEnv::set_item_chk_txt() {
	wstring wstr_type; wstring wstr;
	switch (inf.panel_func_id) {
	case IDC_TASK_FUNC_RADIO4: {
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


