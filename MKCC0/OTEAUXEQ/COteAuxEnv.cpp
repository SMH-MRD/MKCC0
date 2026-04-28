#include "COteAuxEnv.h"
#include "resource.h"
#include "CSHAREDMEM.H"
#include "SmemOte.H"
#include "SmemAux.H"

//Namespace for using StApi.

ST_OTEAUXENV_MON1 COteAuxEnv::st_mon1;
ST_OTEAUXENV_MON2 COteAuxEnv::st_mon2;

extern CSharedMem* pAuxEnvInfObj;
extern CSharedMem* pAuxAgentInfObj;
extern CSharedMem* pAuxCsInfObj;
extern CSharedMem* pAuxPolInfObj;
extern CSharedMem* pCsInfObj;

extern BC_TASK_ID st_task_id;
extern vector<CBasicControl*>	    VectCtrlObj;	    //スレッドオブジェクトのポインタ

//共有メモリ
static LPST_OTE_AUX_ENV_INF		pAuxEnvInf = NULL;
static LPST_OTE_AUX_CS_INF		pAuxCSInf = NULL;
static LPST_OTE_AUX_AGENT_INF	pAuxAgInf = NULL;
static LPST_OTE_AUX_POL_INF		pAuxPolInf = NULL;
static LPST_OTE_CS_INF			pCSInf = NULL;

static wostringstream wos_cam;


COteAuxEnv::COteAuxEnv() {
}
COteAuxEnv::~COteAuxEnv() {
}


HRESULT COteAuxEnv::initialize(LPVOID lpParam) {

	//### 出力用共有メモリ取得
	out_size = sizeof(ST_OTE_AUX_ENV_INF);
	set_outbuf(pAuxEnvInfObj->get_pMap());

	//### 入力用共有メモリ取得
	pAuxAgInf = (LPST_OTE_AUX_AGENT_INF)pAuxAgentInfObj->get_pMap();
	pAuxEnvInf = (LPST_OTE_AUX_ENV_INF)(pAuxEnvInfObj->get_pMap());
	pAuxCSInf = (LPST_OTE_AUX_CS_INF)pAuxCsInfObj->get_pMap();
	pAuxPolInf = (LPST_OTE_AUX_POL_INF)pAuxPolInfObj->get_pMap();
	pCSInf = (LPST_OTE_CS_INF)pCsInfObj->get_pMap();

	//### 初期化
	pAuxEnvInf->video_delay_chk_func_active = L_ON;

	//モニタウィンドウテキスト	
	set_func_pb_txt();
	set_item_chk_txt();
	set_panel_tip_txt();
	return S_OK;
}

HRESULT COteAuxEnv::routine_work(void* pObj) {
	input();
	parse();
	output();

	if (inf.total_act % 20 == 0) {
		wos.str(L""); wos << inf.status << L":" << std::setfill(L'0') << std::setw(4) << inf.act_time << L" CamDelay:";
		if(pAuxEnvInf->video_delay_chk_func_active == L_ON) wos << L"ReqON";
		else wos << L"ReqOFF";
		msg2host(wos.str());
	}

	return S_OK;
}

int COteAuxEnv::input() {
	if (pCSInf->video_delay_chk_req == L_ON)
		pAuxEnvInf->video_delay_chk_func_active = L_ON;
	else
		pAuxEnvInf->video_delay_chk_func_active = L_OFF;

	//チェックデバイス強制ON
	if (pCSInf->video_delay_chk_ctrl & OTE_CS_CODE_V_DELAY_COM_FORCED_ON)
		pAuxEnvInf->video_delay_chk_device_forth_on = L_ON;
	else 
		pAuxEnvInf->video_delay_chk_device_forth_on = L_OFF;

	//自動パラメータセット要求
	if (pCSInf->video_delay_chk_ctrl & OTE_CS_CODE_V_DELAY_COM_AUTO_PRM) 
		pAuxEnvInf->video_delay_chk_auto_prm_set_req = L_ON;
	else
		pAuxEnvInf->video_delay_chk_auto_prm_set_req = L_OFF;

	//パラメータセーブ要求
	if (pCSInf->video_delay_chk_ctrl & OTE_CS_CODE_V_DELAY_COM_PRM_SAVE)
		pAuxEnvInf->video_delay_chk_prm_save_req = L_ON;
	else
		pAuxEnvInf->video_delay_chk_prm_save_req = L_OFF;

	return S_OK;
}
int COteAuxEnv::parse() {           //メイン処理
	return STAT_OK;
}
int COteAuxEnv::output() {          //出力処理

	return STAT_OK;
}
int COteAuxEnv::close() {

	return 0;
}

static wostringstream monwos;
LRESULT CALLBACK COteAuxEnv::Mon1Proc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
	switch (msg)
	{
	case WM_CREATE: {
		InitCommonControls();//コモンコントロール初期化
		HINSTANCE hInst = (HINSTANCE)GetModuleHandle(0);
		//ウィンドウにコントロール追加
		st_mon1.hctrl[AUXENV_ID_MON1_STATIC_INF] = CreateWindowW(TEXT("STATIC"), st_mon1.text[AUXENV_ID_MON1_STATIC_INF], WS_CHILD | WS_VISIBLE | SS_LEFT,
			st_mon1.pt[AUXENV_ID_MON1_STATIC_INF].x, st_mon1.pt[AUXENV_ID_MON1_STATIC_INF].y,
			st_mon1.sz[AUXENV_ID_MON1_STATIC_INF].cx, st_mon1.sz[AUXENV_ID_MON1_STATIC_INF].cy,
			hWnd, (HMENU)(AUXENV_ID_MON1_CTRL_BASE + AUXENV_ID_MON1_STATIC_INF), hInst, NULL);

		//表示更新用タイマー
		SetTimer(hWnd, AUXENV_ID_MON1_TIMER, st_mon1.timer_ms, NULL);

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
		monwos.str(L"");

		SetWindowText(st_mon1.hctrl[AUXENV_ID_MON1_STATIC_INF], monwos.str().c_str());
	}break;

	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
	}break;
	case WM_DESTROY: {
		st_mon1.hwnd_mon = NULL;
		st_mon1.is_monitor_active = false;
		KillTimer(hWnd, AUXENV_ID_MON1_TIMER);
	}break;
	default:
		return DefWindowProc(hWnd, msg, wp, lp);
	}
	return S_OK;
};

LRESULT CALLBACK COteAuxEnv::Mon2Proc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
	switch (msg)
	{
	case WM_CREATE: {
		InitCommonControls();//コモンコントロール初期化

		//	setup_graphics(hWnd);
		HINSTANCE hInst = (HINSTANCE)GetModuleHandle(0);
		//ウィンドウにコントロール追加
		break;

	}
	case WM_TIMER: {;
	}break;
	case WM_COMMAND: {
		int wmId = LOWORD(wp);
		// 選択されたメニューの解析:
		switch (wmId - AUXENV_ID_MON2_CTRL_BASE)
		{

		default:
			return DefWindowProc(hWnd, msg, wp, lp);
		}
	}break;
	case WM_PAINT: {

		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;
	}break;
	case WM_DESTROY: {
		st_mon2.hwnd_mon = NULL;
		st_mon2.is_monitor_active = false;

	}break;
	default:
		return DefWindowProc(hWnd, msg, wp, lp);
	}
	return S_OK;
}


HWND COteAuxEnv::open_monitor_wnd(HWND h_parent_wnd, int id) {

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
		wcex.lpszMenuName = TEXT("AUXENV_MON1");
		wcex.lpszClassName = TEXT("AUXENV_MON1");
		wcex.hIconSm = NULL;

		ATOM fb = RegisterClassExW(&wcex);

		st_mon1.hwnd_mon = CreateWindowW(TEXT("AUXENV_MON1"), TEXT("AUXENV_MON1"), WS_OVERLAPPEDWINDOW,
			AUXENV_MON1_WND_X, AUXENV_MON1_WND_Y, AUXENV_MON1_WND_W, AUXENV_MON1_WND_H,
			h_parent_wnd, nullptr, hInst, nullptr);
		show_monitor_wnd(id);
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
		wcex.lpszMenuName = TEXT("AUXENV_MON2");
		wcex.lpszClassName = TEXT("AUXENV_MON2");
		wcex.hIconSm = NULL;

		ATOM fb = RegisterClassExW(&wcex);

		st_mon2.hwnd_mon = CreateWindowW(TEXT("AUXENV_MON2"), TEXT("AUXENV_MON2"), WS_OVERLAPPEDWINDOW,
			AUXENV_MON2_WND_X, AUXENV_MON2_WND_Y, AUXENV_MON2_WND_W, AUXENV_MON2_WND_H,
			h_parent_wnd, nullptr, hInst, nullptr);
		return st_mon2.hwnd_mon;
	}
	else
	{
		return NULL;
	};

	return NULL;
}
void COteAuxEnv::close_monitor_wnd(int id) {
	if (id == BC_ID_MON1)
		DestroyWindow(st_mon1.hwnd_mon);
	else if (id == BC_ID_MON2)
		DestroyWindow(st_mon2.hwnd_mon);
	else;
	return;
}
void COteAuxEnv::show_monitor_wnd(int id) {
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
void COteAuxEnv::hide_monitor_wnd(int id) {
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
LRESULT CALLBACK COteAuxEnv::PanelProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp) {

	switch (msg) {
	case WM_COMMAND: {
		switch (LOWORD(wp)) {
		case IDC_TASK_FUNC_RADIO1:
		case IDC_TASK_FUNC_RADIO2:
		case IDC_TASK_FUNC_RADIO3:
		case IDC_TASK_FUNC_RADIO4:
		case IDC_TASK_FUNC_RADIO5:
		case IDC_TASK_FUNC_RADIO6:
		{
			inf.panel_func_id = LOWORD(wp); set_panel_tip_txt();
			set_PNLparam_value(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
			for (int i = 0; i < BC_N_ACT_ITEM; i++) {
				if (inf.panel_act_chk[wp - IDC_TASK_FUNC_RADIO1][i])
					SendMessage(GetDlgItem(hDlg, IDC_TASK_ITEM_CHECK1 + i), BM_SETCHECK, BST_CHECKED, 0L);
				else
					SendMessage(GetDlgItem(hDlg, IDC_TASK_ITEM_CHECK1 + i), BM_SETCHECK, BST_UNCHECKED, 0L);
			}
		}break;

		case IDC_TASK_ITEM_CHECK1:
		case IDC_TASK_ITEM_CHECK2:
		case IDC_TASK_ITEM_CHECK3:
		case IDC_TASK_ITEM_CHECK4:
		case IDC_TASK_ITEM_CHECK5:
		case IDC_TASK_ITEM_CHECK6:
		{
			if (inf.panel_func_id == IDC_TASK_FUNC_RADIO1) {
			}

			if (IsDlgButtonChecked(hDlg, LOWORD(wp)) == BST_CHECKED)
				inf.panel_act_chk[inf.panel_func_id - IDC_TASK_FUNC_RADIO1][LOWORD(wp) - IDC_TASK_ITEM_CHECK1] = true;
			else
				inf.panel_act_chk[inf.panel_func_id - IDC_TASK_FUNC_RADIO1][LOWORD(wp) - IDC_TASK_ITEM_CHECK1] = false;
		}break;

		case IDSET:
		{
		}break;
		case IDRESET:
		{
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

		case IDC_TASK_MON_CHECK1:{
			if (IsDlgButtonChecked(hDlg, IDC_TASK_MON_CHECK1) == BST_CHECKED) {
				open_monitor_wnd(inf.hwnd_parent, BC_ID_MON1);
			}
			else {
				close_monitor_wnd(BC_ID_MON1);
			}
		}break;

		case IDC_TASK_MON_CHECK2: {
			if (IsDlgButtonChecked(hDlg, IDC_TASK_MON_CHECK1) == BST_CHECKED) {
				open_monitor_wnd(inf.hwnd_parent, BC_ID_MON1);
			}
			else {
				close_monitor_wnd(BC_ID_MON1);
			}
		}break;
		}
	}break;

	case WM_USER_TASK_REQ: {
		if (HIWORD(wp) == WM_USER_WPH_OPEN_IF_WND) {
			wos.str(L"");
			if (lp == BC_ID_MON1) st_mon1.hwnd_mon = open_monitor_wnd(hDlg, (int)lp);
			if (lp == BC_ID_MON2) st_mon2.hwnd_mon = open_monitor_wnd(hDlg, (int)lp);
		}
		else if (wp == WM_USER_WPH_CLOSE_IF_WND) 	close_monitor_wnd(lp);
		else;
	}break;

	default:break;
	}
	return 0;
};

///###	タブパネルのListViewにメッセージを出力
void COteAuxEnv::msg2listview(wstring wstr) {

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

void COteAuxEnv::set_PNLparam_value(float p1, float p2, float p3, float p4, float p5, float p6) {
	wstring wstr;
	wstr += std::to_wstring(p1); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT1), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p2); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT2), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p3); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT3), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p4); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT4), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p5); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT5), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p6); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT6), wstr.c_str());
}

//タブパネルのStaticテキストを設定
void COteAuxEnv::set_panel_tip_txt() {
	wstring wstr_type; wstring wstr;
	switch (inf.panel_func_id) {
	case IDC_TASK_FUNC_RADIO1:
	case IDC_TASK_FUNC_RADIO2:
	case IDC_TASK_FUNC_RADIO3:
	case IDC_TASK_FUNC_RADIO4:
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
void COteAuxEnv::set_func_pb_txt() {
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO1, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO2, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO3, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO4, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO5, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO6, L"-");
	return;
}
//タブパネルのItem chkテキストを設定
void COteAuxEnv::set_item_chk_txt() {
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK1, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK2, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK3, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK4, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK5, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK6, L"-");
	return;
}



