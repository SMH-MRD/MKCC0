#include "COteScad.h"
#include "resource.h"
#include "framework.h"
#include "CCraneLib.H"
#include "CCUILib.h"
#include "COteEnv.h"
#include "COpePanelLib.h"

extern vector<CBasicControl*>	VectCtrlObj;
extern BC_TASK_ID st_task_id;

extern CSharedMem* pOteEnvInfObj;
extern CSharedMem* pOteCsInfObj;
extern CSharedMem* pOteCcInfObj;
extern CSharedMem* pOteUiObj;

ST_OTE_SCAD_MON1 COteScad::st_mon1;
ST_OTE_SCAD_MON2 COteScad::st_mon2;

CPanelBase* pPanelBase;

extern ST_DRAWING_BASE		drawing_items;


//共有メモリ

static LPST_OTE_ENV_INF	pOteEnvInf;
static LPST_OTE_UI		pOteUI;
static LPST_OTE_CC_IF	pOteCCIf;
static LPST_OTE_CS_INF	pOteCsInf;

static COteEnv* pEnvObj;

static LARGE_INTEGER start_count_s, end_count_r;			//システムカウント
static LARGE_INTEGER frequency;								//システム周波数
static LONGLONG res_delay_max, res_delay_min = 10000000;	//C応答時間

static wostringstream wos_msg;
/****************************************************************************/
/*   デフォルト関数											                    */
/****************************************************************************/
COteScad::COteScad() {
}
COteScad::~COteScad() {
}

HRESULT COteScad::initialize(LPVOID lpParam) {

	//システム周波数読み込み
	QueryPerformanceFrequency(&frequency);

	HRESULT hr = S_OK;
	//### 出力用共有メモリ取得
	out_size = sizeof(ST_OTE_CC_IF);
	if (OK_SHMEM != pOteCcInfObj->create_smem(SMEM_OTE_CC_IF_NAME, out_size, MUTEX_OTE_CC_IF_NAME)) {
		err |= SMEM_NG_CS_INF; hr = S_FALSE;
	}
	set_outbuf(pOteCcInfObj->get_pMap());

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

	//### Environmentクラスインスタンスのポインタ取得
	pEnvObj = (COteEnv*)VectCtrlObj[st_task_id.ENV];

	//### IFウィンドウOPEN
	//WPARAM wp = MAKELONG(inf.index, WM_USER_WPH_OPEN_IF_WND);//HWORD:コマンドコード, LWORD:タスクインデックス
	//LPARAM lp = BC_ID_MON2;
	//SendMessage(inf.hwnd_opepane, WM_USER_TASK_REQ, wp, lp);

	
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
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_MON_CHECK2, L"MKCC IF");
	set_item_chk_txt();
	set_panel_tip_txt();
	//モニタ２CB状態セット	
	if (st_mon2.hwnd_mon != NULL)
		SendMessage(GetDlgItem(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK1), BM_SETCHECK, BST_CHECKED, 0L);
	else
		SendMessage(GetDlgItem(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK1), BM_SETCHECK, BST_UNCHECKED, 0L);

	return hr;
}

HRESULT COteScad::routine_work(void* pObj) {
	input();
	parse();
	output();
	return S_OK;
}

int COteScad::input() {

	return S_OK;
}

int COteScad::parse() {

	return S_OK;
}

int COteScad::close() {

	return 0;
}

HRESULT COteScad::open_ope_window() {

	WPARAM wp = MAKELONG(inf.index, WM_USER_WPH_OPEN_IF_WND);//HWORD:コマンドコード, LWORD:タスクインデックス
	LPARAM lp = BC_ID_MON1;
	SendMessage(inf.hwnd_opepane, WM_USER_TASK_REQ, wp, lp);

	return S_OK;
}
/****************************************************************************/
/*   タスク設定タブパネルウィンドウのコールバック関数                       */
/****************************************************************************/
LRESULT CALLBACK COteScad::PanelProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp) {

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
/*   モニタウィンドウ									                    */
/****************************************************************************/
static HBRUSH hBrush;
static SolidBrush* pBrush_bk;
static int ichk;

LRESULT CALLBACK COteScad::Mon1Proc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
	
	switch (msg)
	{
	case WM_CREATE: {
		InitCommonControls();//コモンコントロール初期化
		HINSTANCE hInst = (HINSTANCE)GetModuleHandle(0);

		pPanelBase = new CPanelBase(pOteEnvInf->selected_crane, pOteEnvInf->device_code.serial_no, hWnd);
		pBrush_bk = drawing_items.pbrush[ID_PANEL_COLOR_GRAY];
		hBrush = CreateSolidBrush(RGB(64, 64, 64)); // ダークグレー

		//ウィンドウにコントロール追加

		int i;
		//STATIC,LABEL
		for (i = OTE_SCAD_ID_MON1_STATIC_UID; i <= OTE_SCAD_ID_MON1_STATIC_CRANE; i++) {
			st_mon1.hctrl[i] = CreateWindowW(TEXT("STATIC"), st_mon1.text[i], WS_CHILD | WS_VISIBLE | SS_LEFT,
				st_mon1.pt[i].x, st_mon1.pt[i].y, st_mon1.sz[i].cx, st_mon1.sz[i].cy,
				hWnd, (HMENU)(OTE_SCAD_ID_MON1_CTRL_BASE + i), hInst, NULL);
		}
		//CB
		i = OTE_SCAD_ID_MON1_CB_ESTP;
		st_mon1.hctrl[i] = CreateWindowW(TEXT("BUTTON"), st_mon1.text[i], WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX | BS_PUSHLIKE | BS_MULTILINE|BS_OWNERDRAW,
			st_mon1.pt[i].x, st_mon1.pt[i].y, st_mon1.sz[i].cx, st_mon1.sz[i].cy,
			hWnd, (HMENU)(pPanelBase->pobjs->cb_estop->id), hInst, NULL);

		//PB
		HWND hwnd = CreateWindowW(TEXT("BUTTON"), pPanelBase->pobjs->pb_csource->txt.c_str(), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_PUSHLIKE| BS_OWNERDRAW,
			pPanelBase->pobjs->pb_csource->pt.X, pPanelBase->pobjs->pb_csource->pt.Y, pPanelBase->pobjs->pb_csource->sz.Width, pPanelBase->pobjs->pb_csource->sz.Height,
			hWnd, (HMENU)(ID_MAIN_PNL_OBJ_PB_CSOURCE), hInst, NULL);


		//for (i = OTE_SCAD_ID_MON1_PB_CTRL_SOURCE; i <= OTE_SCAD_ID_MON1_PB_CONNECT; i++) {
		//	st_mon1.hctrl[i] = CreateWindowW(TEXT("BUTTON"), st_mon1.text[i], WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_PUSHLIKE|BS_OWNERDRAW,
		//		st_mon1.pt[i].x, st_mon1.pt[i].y, st_mon1.sz[i].cx, st_mon1.sz[i].cy,
		//		hWnd, (HMENU)(OTE_SCAD_ID_MON1_CTRL_BASE + i), hInst, NULL);
		//}

		//RADIO BUTTON
		for (i = OTE_SCAD_ID_MON1_RADIO_FAULT; i <= OTE_SCAD_ID_MON1_RADIO_CLEAR; i++) {
			if (i == OTE_SCAD_ID_MON1_RADIO_FAULT) {
				st_mon1.hctrl[i] = CreateWindowW(TEXT("BUTTON"), st_mon1.text[i], WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON | BS_PUSHLIKE | BS_MULTILINE | WS_GROUP,
					st_mon1.pt[i].x, st_mon1.pt[i].y, st_mon1.sz[i].cx, st_mon1.sz[i].cy,
					hWnd, (HMENU)(OTE_SCAD_ID_MON1_CTRL_BASE + i), hInst, NULL);
			}
			else {
				st_mon1.hctrl[i] = CreateWindowW(TEXT("BUTTON"), st_mon1.text[i], WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON | BS_PUSHLIKE | BS_MULTILINE,
					st_mon1.pt[i].x, st_mon1.pt[i].y, st_mon1.sz[i].cx, st_mon1.sz[i].cy,
					hWnd, (HMENU)(OTE_SCAD_ID_MON1_CTRL_BASE + i), hInst, NULL);
			}
		}

		//表示更新用タイマー
		SetTimer(hWnd, OTE_SCAD_ID_MON1_TIMER, st_mon1.timer_ms, NULL);

		break;
	}
	case WM_COMMAND: {
		int wmId = LOWORD(wp);
		// 選択されたメニューの解析:
		switch (wmId)
		{
		case ID_MAIN_PNL_OBJ_CB_ESTOP: {
			if (ichk == L_OFF)
				ichk = L_ON;
			else
				ichk = L_OFF;
		}break;
		case ID_MAIN_PNL_OBJ_PB_CSOURCE: {
			if (ichk == L_OFF)
				ichk = L_ON;
			else
				ichk = L_OFF;
		}
		default:
			return DefWindowProc(hWnd, msg, wp, lp);
		}
	}break;
	case WM_CTLCOLORSTATIC:
		SetTextColor((HDC)wp, RGB(220, 220, 220)); // ライトグレー
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

		PatBlt(hdc, 0, 0, OTE_SCAD_MON1_WND_W, OTE_SCAD_MON1_WND_H, RGB(64, 64, 64));

		EndPaint(hWnd, &ps);
	}break;
	case WM_DRAWITEM: {
		DRAWITEMSTRUCT* pDIS = (DRAWITEMSTRUCT*)lp;
		Image* image;
		Graphics gra(pDIS->hDC); 
		Rect rc(pDIS->rcItem.left, pDIS->rcItem.top, pDIS->rcItem.right - pDIS->rcItem.left, pDIS->rcItem.bottom - pDIS->rcItem.top);
//		SolidBrush br(Color(255, 64, 64, 64)); // ARGB (255:完全不透明, 255:赤, 0:緑, 0:青)
		gra.FillRectangle(pBrush_bk, rc);

		if (pDIS->CtlID == pPanelBase->pobjs->cb_estop->id) {
			
			//			if(BST_CHECKED == SendMessage(pDIS->hwndItem,BM_GETCHECK,0,0))
			if(ichk == L_ON)
				image = pPanelBase->pobjs->lmp_estop->pimg[1];
			else
				image = pPanelBase->pobjs->lmp_estop->pimg[0];

			if (image) gra.DrawImage(image, rc);
		}
		else if (ID_MAIN_PNL_OBJ_PB_CSOURCE) {
			image = pPanelBase->pobjs->lmp_csource->pimg[pPanelBase->pobjs->lmp_csource->get()];
			if(ichk)
			image = pPanelBase->pobjs->lmp_csource->pimg[1];
			else
				image = pPanelBase->pobjs->lmp_csource->pimg[2];

			if (image) gra.DrawImage(image, rc);
			gra.DrawString(
				pPanelBase->pobjs->lmp_csource->txt.c_str(),
				-1,
				pPanelBase->pobjs->lmp_csource->pFont,
				pPanelBase->pobjs->lmp_csource->frc,
				pPanelBase->pobjs->lmp_csource->pStrFormat,
				pPanelBase->pobjs->lmp_csource->pTxtBrush
				);
		}
		else;

	}return true;
	case WM_DESTROY: {
		st_mon1.hwnd_mon = NULL;
		KillTimer(hWnd, OTE_SCAD_ID_MON1_TIMER);
		DeleteObject(hBrush);

		//### オープニング画面を再表示
		pEnvObj->open_opening_window();

	}break;
	default:
		return DefWindowProc(hWnd, msg, wp, lp);
	}
	return S_OK;
};

//static wostringstream mon2wos;
LRESULT CALLBACK COteScad::Mon2Proc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
	switch (msg)
	{
	case WM_CREATE: {
		InitCommonControls();//コモンコントロール初期化
		HINSTANCE hInst = (HINSTANCE)GetModuleHandle(0);
		//ウィンドウにコントロール追加
		//STATIC,LABEL
		for (int i = OTE_SCAD_ID_MON2_STATIC_UNI; i <= OTE_SCAD_ID_MON2_STATIC_MSG; i++) {
			st_mon2.hctrl[i] = CreateWindowW(TEXT("STATIC"), st_mon2.text[i], WS_CHILD | WS_VISIBLE | SS_LEFT,
				st_mon2.pt[i].x, st_mon2.pt[i].y, st_mon2.sz[i].cx, st_mon2.sz[i].cy,
				hWnd, (HMENU)(OTE_SCAD_ID_MON2_CTRL_BASE + i), hInst, NULL);
		}
		//RADIO PB
		for (int i = OTE_SCAD_ID_MON2_RADIO_RCV; i <= OTE_SCAD_ID_MON2_RADIO_INFO; i++) {
			if (i == OTE_SCAD_ID_MON2_RADIO_RCV) {
				st_mon2.hctrl[i] = CreateWindowW(TEXT("BUTTON"), st_mon2.text[i], WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_PUSHLIKE | WS_GROUP,
					st_mon2.pt[i].x, st_mon2.pt[i].y, st_mon2.sz[i].cx, st_mon2.sz[i].cy,
					hWnd, (HMENU)(OTE_SCAD_ID_MON2_CTRL_BASE + i), hInst, NULL);

				st_mon2.sock_inf_id = OTE_SCAD_ID_MON2_RADIO_RCV;
				SendMessage(st_mon2.hctrl[i], BM_SETCHECK, BST_CHECKED, 0L);
			}
			else
				st_mon2.hctrl[i] = CreateWindowW(TEXT("BUTTON"), st_mon2.text[i], WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_PUSHLIKE,
					st_mon2.pt[i].x, st_mon2.pt[i].y, st_mon2.sz[i].cx, st_mon2.sz[i].cy,
					hWnd, (HMENU)(OTE_SCAD_ID_MON2_CTRL_BASE + i), hInst, NULL);
		}

		UINT rtn = SetTimer(hWnd, OTE_SCAD_ID_MON2_TIMER, OTE_SCAD_PRM_MON2_TIMER_MS, NULL);
	}break;
	case WM_COMMAND: {
		int wmId = LOWORD(wp);
		// 選択されたメニューの解析:
		int _Id = wmId - OTE_SCAD_ID_MON2_CTRL_BASE;
		// 選択されたメニューの解析:
		switch (_Id)
		{
		case OTE_SCAD_ID_MON2_RADIO_RCV: {
			st_mon2.sock_inf_id = OTE_SCAD_ID_MON2_RADIO_RCV;
		}break;
		case OTE_SCAD_ID_MON2_RADIO_SND: {
			st_mon2.sock_inf_id = OTE_SCAD_ID_MON2_RADIO_SND;
		}break;
		case OTE_SCAD_ID_MON2_RADIO_INFO: {
			st_mon2.sock_inf_id = OTE_SCAD_ID_MON2_RADIO_INFO;
		}break;
		default:
			return DefWindowProc(hWnd, msg, wp, lp);
		}break;
	}
	case WM_TIMER: {

	}break;
	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
	}break;
	case WM_DESTROY: {
		KillTimer(hWnd, OTE_SCAD_ID_MON2_TIMER);
		st_mon2.hwnd_mon = NULL;
	}break;
	default:
		return DefWindowProc(hWnd, msg, wp, lp);
	}

	return S_OK;
}

HWND COteScad::open_monitor_wnd(HWND h_parent_wnd, int id) {

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
		wcex.lpszMenuName = TEXT("OTE_SCAD_MON1");
		wcex.lpszClassName = TEXT("OTE_SCAD_MON1");
		wcex.hIconSm = NULL;

		ATOM fb = RegisterClassExW(&wcex);

		st_mon1.hwnd_mon = CreateWindowW(TEXT("OTE_SCAD_MON1"), TEXT("Operation Panel"), WS_OVERLAPPEDWINDOW,
			OTE_SCAD_MON1_WND_X, OTE_SCAD_MON1_WND_Y, OTE_SCAD_MON1_WND_W, OTE_SCAD_MON1_WND_H,
			h_parent_wnd, nullptr, hInst, nullptr);
		
		SetLayeredWindowAttributes(st_mon1.hwnd_mon, 0, 200, LWA_ALPHA);
		
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
		wcex.lpszMenuName = TEXT("OTE_SCAD_MON2");
		wcex.lpszClassName = TEXT("OTE_SCAD_MON2");
		wcex.hIconSm = NULL;

		ATOM fb = RegisterClassExW(&wcex);

		st_mon2.hwnd_mon = CreateWindowW(TEXT("OTE_SCAD_MON2"), TEXT("OTE_SCAD_MON2"), WS_OVERLAPPEDWINDOW,
			OTE_SCAD_MON2_WND_X, OTE_SCAD_MON2_WND_Y, OTE_SCAD_MON2_WND_W, OTE_SCAD_MON2_WND_H,
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
void COteScad::close_monitor_wnd(int id) {
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
void COteScad::show_monitor_wnd(int id) {
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
void COteScad::hide_monitor_wnd(int id) {
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

///###	タブパネルのListViewにメッセージを出力
void COteScad::msg2listview(wstring wstr) {

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
void COteScad::set_PNLparam_value(float p1, float p2, float p3, float p4, float p5, float p6) {
	wstring wstr;
	wstr += std::to_wstring(p1); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT1), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p2); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT2), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p3); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT3), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p4); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT4), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p5); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT5), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p6); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT6), wstr.c_str());
}
//タブパネルのEdit Box説明テキストを設定
void COteScad::set_panel_tip_txt() {
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
void COteScad::set_func_pb_txt() {
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO1, L"MKCC IF");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO2, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO3, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO4, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO5, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO6, L"-");
	return;
}
//タブパネルのItem chkテキストを設定
void COteScad::set_item_chk_txt() {
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
