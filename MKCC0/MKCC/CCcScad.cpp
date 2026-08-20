#include "CCcScad.h"
#include "resource.h"


extern CSharedMem* pEnvInfObj;
extern CSharedMem* pPlcIoObj;
extern CSharedMem* pJobIoObj;
extern CSharedMem* pPolInfObj;
extern CSharedMem* pAgInfObj;
extern CSharedMem* pCsInfObj;
extern CSharedMem* pSimuStatObj;
extern CSharedMem* pOteInfObj;
extern CSharedMem* pAuxCsInfObj;

extern CCrane* pCrane;
extern ST_DEVICE_CODE g_my_code;

extern BC_TASK_ID st_task_id;
extern vector<CBasicControl*>	    VectCtrlObj;	    //スレッドオブジェクトのポインタ


CMKLog* pmklog;

ST_SCAD_MON1 CScada::st_mon1;
ST_SCAD_MON2 CScada::st_mon2;

ST_CC_SCAD_INF	CScada::st_work;
ST_MKLOG_SOURCE CScada::logsource;
ST_MKLOG_DB		CScada::st_log_db;
SYSTEMTIME		CScada::systime;

int CScada::log_elapse_count[MKLOG_N_ID_TYPE];

//共有メモリ
LPST_CC_ENV_INF		CScada::pEnv_Inf = NULL;
LPST_CC_CS_INF		CScada::pCS_Inf = NULL;
LPST_CC_PLC_IO		CScada::pPLC_IO = NULL;
LPST_CC_AGENT_INF	CScada::pAgent_Inf = NULL;
LPST_CC_OTE_INF		CScada::pOTE_Inf = NULL;
LPST_CC_SIM_INF		CScada::pSim_Inf = NULL;
LPST_CC_POL_INF		CScada::pPolInf = NULL;
LPST_AUX_CS_INF		CScada::pAUX_CS_Inf = NULL;


CScada::CScada() {
	pmklog = new CMKLog;
}
CScada::~CScada() {
	delete pmklog;
}

HRESULT CScada::initialize(LPVOID lpParam) {

	wos.str(L"初期化中…"); msg2host(wos.str());

	//### 入力用共有メモリ取得
	pAgent_Inf = (LPST_CC_AGENT_INF)pAgInfObj->get_pMap();
	pEnv_Inf = (LPST_CC_ENV_INF)(pEnvInfObj->get_pMap());
	pPLC_IO = (LPST_CC_PLC_IO)(pPlcIoObj->get_pMap());
	pCS_Inf = (LPST_CC_CS_INF)pCsInfObj->get_pMap();
	pOTE_Inf = (LPST_CC_OTE_INF)pOteInfObj->get_pMap();
	pSim_Inf = (LPST_CC_SIM_INF)pSimuStatObj->get_pMap();
	pPolInf = (LPST_CC_POL_INF)(pPolInfObj->get_pMap());
	pAUX_CS_Inf = (LPST_AUX_CS_INF)pAuxCsInfObj->get_pMap();
	

	//LOG設定セット
	update_logsource_all(false);

	//LOG初期設定
	CMKLog::set_host((CBasicControl*)VectCtrlObj[st_task_id.SCAD]);
	CMKLog::set_db(&st_log_db);
	CMKLog::set_log_head(MKLOG_ID_TYPE_TIME, &logsource.header_time);
	CMKLog::set_log_head(MKLOG_ID_TYPE_EVENT, &logsource.header_event);
	CMKLog::set_log_head(MKLOG_ID_TYPE_SCAT, &logsource.header_scat);
	CMKLog::set_log_head(MKLOG_ID_TYPE_TRAP, &logsource.header_trap);

	CMKLog::init_log_setting(MKLOG_ID_TYPE_TIME, L_OFF);


	//タスクパネル設定
	set_func_pb_txt();
	set_item_chk_txt();
	set_panel_tip_txt();

	inf.panel_func_id = IDC_TASK_FUNC_RADIO1;
	SendMessage(GetDlgItem(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO1), BM_SETCHECK, BST_CHECKED, 0L);
	for (int i = 1; i < 6; i++)
		SendMessage(GetDlgItem(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO1 + i), BM_SETCHECK, BST_UNCHECKED, 0L);
	//モード設定0
	inf.mode_id = BC_ID_MODE0;
	SendMessage(GetDlgItem(inf.hwnd_opepane, IDC_TASK_MODE_RADIO0), BM_SETCHECK, BST_CHECKED, 0L);

	CScada* pEnvObj = (CScada*)lpParam;
	int code = 0;
	return S_OK;
}

HRESULT CScada::routine_work(void* pObj) {
	if (inf.total_act % 20 == 0) {
		wos.str(L""); wos << inf.status << L":" << std::setfill(L'0') << std::setw(4) << inf.act_time;
		msg2host(wos.str());
	}
	input();
	parse();
	output();
	return S_OK;
}

static UINT32	gpad_mode_last = L_OFF;

int CScada::input() {
	return S_OK;
}

int CScada::parse() {           //メイン処理


	//ログデータセット処理
	if ((pmklog != NULL) && (pmklog->is_log_wnd_active.i64)) {
		for (int i = MKLOG_ID_TYPE_TIME; i <= MKLOG_ID_TYPE_SCAT; i++) {
			LPST_LOG_HEADER pheader;

			if (!pmklog->is_log_wnd_active.i16[i]) continue;

			switch (i) {
			case MKLOG_ID_TYPE_TIME:pheader		= &logsource.header_time; break;
			case MKLOG_ID_TYPE_TRAP:pheader		= &logsource.header_trap; break;
			case MKLOG_ID_TYPE_EVENT:pheader	= &logsource.header_event; break;
			case MKLOG_ID_TYPE_SCAT:pheader		= &logsource.header_scat; break;
			default: pheader = &logsource.header_event; break;
			}

			if ((pmklog->get_logstatus(i) & MKLOG_CODE_LOG_REC_AND_CHART)) {
				for (int k = MKLOG_INDEX_LOG_DATA0; k < logsource.n_item[i] + MKLOG_INDEX_LOG_DATA0; k++) {
					if (st_log_db.item[pheader->code[k]].func != NULL)
						pmklog->loghot[i][k] = st_log_db.item[pheader->code[k]].func(st_log_db.item[pheader->code[k]].d100);
					if (k == 6)
						INT16 dat = pmklog->loghot[i][k];
				}
				log_elapse_count[i]++;
			}
			else {
				if (i == MKLOG_ID_TYPE_EVENT)
					log_elapse_count[i]++;
				else
					log_elapse_count[i] = 0;
			}
		}
	}

	return S_OK;
}

int CScada::close() {

	return 0;
}

/****************************************************************************/
/*   モニタウィンドウ									                    */
/****************************************************************************/
static wostringstream monwos;

LRESULT CALLBACK CScada::Mon1Proc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
	switch (msg)
	{
	case WM_CREATE: {
		InitCommonControls();//コモンコントロール初期化
		HINSTANCE hInst = (HINSTANCE)GetModuleHandle(0);
		//ウィンドウにコントロール追加
		st_mon1.hctrl[SCAD_ID_MON1_STATIC_GPAD] = CreateWindowW(TEXT("STATIC"), st_mon1.text[SCAD_ID_MON1_STATIC_GPAD], WS_CHILD | WS_VISIBLE | SS_LEFT,
			st_mon1.pt[SCAD_ID_MON1_STATIC_GPAD].x, st_mon1.pt[SCAD_ID_MON1_STATIC_GPAD].y,
			st_mon1.sz[SCAD_ID_MON1_STATIC_GPAD].cx, st_mon1.sz[SCAD_ID_MON1_STATIC_GPAD].cy,
			hWnd, (HMENU)(SCAD_ID_MON1_CTRL_BASE + SCAD_ID_MON1_STATIC_GPAD), hInst, NULL);

		//表示更新用タイマー
		SetTimer(hWnd, SCAD_ID_MON1_TIMER, st_mon1.timer_ms, NULL);

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
		KillTimer(hWnd, SCAD_ID_MON1_TIMER);
	}break;
	default:
		return DefWindowProc(hWnd, msg, wp, lp);
	}
	return S_OK;
};

LRESULT CALLBACK CScada::Mon2Proc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
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

HWND CScada::open_monitor_wnd(HWND h_parent_wnd, int id) {

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
		wcex.lpszMenuName = TEXT("SCAD_MON1");
		wcex.lpszClassName = TEXT("SCAD_MON1");
		wcex.hIconSm = NULL;

		ATOM fb = RegisterClassExW(&wcex);

		st_mon1.hwnd_mon = CreateWindowW(TEXT("SCAD_MON1"), TEXT("SCAD_MON1"), WS_OVERLAPPEDWINDOW,
			SCAD_MON1_WND_X, SCAD_MON1_WND_Y, SCAD_MON1_WND_W, SCAD_MON1_WND_H,
			h_parent_wnd, nullptr, hInst, nullptr);
		show_monitor_wnd(id);
	}
	else if (id == BC_ID_MON2) {
		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = Mon1Proc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = hInst;
		wcex.hIcon = NULL;
		wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wcex.lpszMenuName = TEXT("SCAD_MON2");
		wcex.lpszClassName = TEXT("SCAD_MON2");
		wcex.hIconSm = NULL;

		ATOM fb = RegisterClassExW(&wcex);

		st_mon2.hwnd_mon = CreateWindowW(TEXT("SCAD_MON2"), TEXT("SCAD_MON2"), WS_OVERLAPPEDWINDOW,
			SCAD_MON2_WND_X, SCAD_MON2_WND_Y, SCAD_MON2_WND_W, SCAD_MON2_WND_H,
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
void CScada::close_monitor_wnd(int id) {
	if (id == BC_ID_MON1)
		DestroyWindow(st_mon1.hwnd_mon);
	else if (id == BC_ID_MON2)
		DestroyWindow(st_mon2.hwnd_mon);
	else;
	return;
}
void CScada::show_monitor_wnd(int id) {
	if (id == BC_ID_MON1) {
		ShowWindow(st_mon1.hwnd_mon, SW_SHOW);
		UpdateWindow(st_mon1.hwnd_mon);
	}
	else if (id == BC_ID_MON2) {
		ShowWindow(st_mon2.hwnd_mon, SW_SHOW);
		UpdateWindow(st_mon2.hwnd_mon);
	}
	else;
	return;
}
void CScada::hide_monitor_wnd(int id) {
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
LRESULT CALLBACK CScada::PanelProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp) {

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
			case IDC_TASK_FUNC_RADIO2: {
				//LOG
				if (inf.panel_func_id == IDC_TASK_FUNC_RADIO2) {
					if (CMKLog::st_work_wnd.hwnd[MKLOG_ID_TYPE_EVENT] == NULL) {
						CMKLog::open_log_event(hDlg);
					}
					else {
						CMKLog::close_log(MKLOG_ID_TYPE_EVENT);
					}
					if (IsDlgButtonChecked(hDlg, IDC_TASK_ITEM_CHECK1) == BST_CHECKED) {
						SendMessage(GetDlgItem(hDlg, IDC_TASK_ITEM_CHECK1), BM_SETCHECK, BST_UNCHECKED, 0L);
					}
				}
				set_panel_tip_txt();  SetFocus(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT1));
				break;
			}
			default:break;
			}
		}break;
		case IDC_TASK_ITEM_CHECK2: {
			//LOG
			if (inf.panel_func_id == IDC_TASK_FUNC_RADIO2) {
				if (CMKLog::st_work_wnd.hwnd[MKLOG_ID_TYPE_TIME] == NULL) {
					CMKLog::open_log_time(hDlg);
				}
				else {
					CMKLog::close_log(MKLOG_ID_TYPE_TIME);
				}

				if (IsDlgButtonChecked(hDlg, IDC_TASK_ITEM_CHECK2) == BST_CHECKED) {
					SendMessage(GetDlgItem(hDlg, IDC_TASK_ITEM_CHECK2), BM_SETCHECK, BST_UNCHECKED, 0L);
				}
			}
			set_panel_tip_txt();  SetFocus(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT1));
		}break;
		case IDC_TASK_ITEM_CHECK3: {
			//LOG
			if (inf.panel_func_id == IDC_TASK_FUNC_RADIO2) {
				if (CMKLog::st_work_wnd.hwnd[MKLOG_ID_TYPE_SCAT] == NULL) {
					CMKLog::open_log_scat(hDlg);
				}
				else {
					CMKLog::close_log(MKLOG_ID_TYPE_SCAT);
				}
				if (IsDlgButtonChecked(hDlg, IDC_TASK_ITEM_CHECK3) == BST_CHECKED) {
					SendMessage(GetDlgItem(hDlg, IDC_TASK_ITEM_CHECK3), BM_SETCHECK, BST_UNCHECKED, 0L);
				}
			}
			set_panel_tip_txt();  SetFocus(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT1));
		}break;
		case IDC_TASK_ITEM_CHECK4: {
			//LOG
			if (inf.panel_func_id == IDC_TASK_FUNC_RADIO2) {
				if (CMKLog::st_work_wnd.hwnd[MKLOG_ID_TYPE_TRAP] == NULL) {
					CMKLog::open_log_trap(hDlg);
				}
				else {
					CMKLog::close_log(MKLOG_ID_TYPE_TRAP);
				}

				if (IsDlgButtonChecked(hDlg, IDC_TASK_ITEM_CHECK4) == BST_CHECKED) {
					SendMessage(GetDlgItem(hDlg, IDC_TASK_ITEM_CHECK4), BM_SETCHECK, BST_UNCHECKED, 0L);
				}
			}
			set_panel_tip_txt();  SetFocus(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT1));
		}break;
		case IDC_TASK_ITEM_CHECK5:
		case IDC_TASK_ITEM_CHECK6:
		break;

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
				open_monitor_wnd(inf.hwnd_parent, BC_ID_MON2);
			}
			else {
				close_monitor_wnd(BC_ID_MON2);
			}
		}break;
		}
	}
	return 0;
};

/****************************************************************************/
/*   LOG処理関連	                                                        */
/* 　												　                      */
/****************************************************************************/
/// <summary>
/// 
/// </summary>
/// <returns></returns>
int CScada::update_logsource_all(bool is_from_inifile) {//Log設定データの読み込み ini fileから読み込み

	int err_code = 0;

	//ログバッファヘッダ部 PCコードセット
	pmklog->set_PCcode((INT16)(g_my_code.machine_id));

	for (int i = 0; i < MKLOG_N_ID_TYPE; i++) {

		if (is_from_inifile) {
			//  Ini fileからlogsourceへの読み込み
			//	pmklog->cmap[i].set_item(k, logsource.psource[i][k], logsource.item_code[i][k], logsource.title[i][k], logsource.val100[i][k]);
		}

		LPST_LOG_HEADER pheader;
		switch (i) {
		case MKLOG_ID_TYPE_TIME:	pheader = &logsource.header_time; break;
		case MKLOG_ID_TYPE_TRAP:	pheader = &logsource.header_trap; break;
		case MKLOG_ID_TYPE_EVENT:	pheader = &logsource.header_event; break;
		case MKLOG_ID_TYPE_SCAT:	pheader = &logsource.header_scat; break;
		default:					pheader = &logsource.header_time; break;
		}

		//ログバッファヘッダ部セット	
		for (int j = 0; j < logsource.n_item[i] + MKLOG_INDEX_LOG_DATA0; j++) {
			if (err_code = pmklog->set_Item100P(i, j, pheader->d100[j]))		return err_code;
			if (err_code = pmklog->set_ItemType(i, j, pheader->code[j]))		return err_code;
			if (err_code = pmklog->set_ItemTitle(i, j, pheader->title[j]))		return err_code;
		}

		if (err_code = pmklog->set_nItem(i, logsource.n_item[i]))			return err_code;	//アイテム数
		if (err_code = pmklog->set_ScanTime(i, logsource.sample_ms[i]))		return err_code;	//サンプリング周期

	}
	
	//ログアイテムデータベースに処理関数のポインタセット
	for (int i = MKLOG_INDEX_LOG_DATA0; i < N_MKLOG_ITEM_DB; i++) {
		if (st_log_db.item[i].code != 0) {
			st_log_db.item[i].func = get_func_of_logitem(i);
		}
	}

	return 0;
}

/// <summary>
/// 
/// </summary>
/// <param name="logtype"></param>
/// <param name="is_from_inifile"></param>
/// <returns></returns>
int CScada::update_logsource(int logtype, bool is_from_inifile) {
	int err_code = 0;

	if (is_from_inifile) {
		//  Ini fileからlogsourceへの読み込み
		//	pmklog->cmap[i].set_item(k, logsource.psource[i][k], logsource.item_code[i][k], logsource.title[i][k], logsource.val100[i][k]);
	}

	LPST_LOG_HEADER pheader;
	switch (logtype) {
	case MKLOG_ID_TYPE_TIME:	pheader = &logsource.header_time; break;
	case MKLOG_ID_TYPE_TRAP:	pheader = &logsource.header_trap; break;
	case MKLOG_ID_TYPE_EVENT:	pheader = &logsource.header_event; break;
	case MKLOG_ID_TYPE_SCAT:	pheader = &logsource.header_scat; break;
	default:					pheader = &logsource.header_time; break;
	}
	//ログバッファヘッダ部セット
	if (err_code = pmklog->set_nItem(logtype, logsource.n_item[logtype]))return err_code;		//アイテム数
	if (err_code = pmklog->set_ScanTime(logtype, logsource.sample_ms[logtype]))return err_code;

	for (int j = 0; j < logsource.n_item[logtype]; j++) {
		if (err_code = pmklog->set_Item100P(logtype, j, pheader->d100[j])) return err_code;
		if (err_code = pmklog->set_ItemType(logtype, j, pheader->code[j]))return err_code;
		if (err_code = pmklog->set_ItemTitle(logtype, j, pheader->title[j]))return err_code;
	}
	return 0;
}

/// <summary>
/// ログのアイテムコードからログ記録用データを出力する関数のポインタを返す関数
/// </summary>
/// <param name="code"></param>
/// <returns></returns>
INT16(*CScada::get_func_of_logitem(INT32 code))(double d) {
	switch (code) {
	case MKLOG_ITEM_MH_POS_FB:return pos_fb_MH;
	case MKLOG_ITEM_BH_POS_FB:return pos_fb_BH;
	case MKLOG_ITEM_SL_POS_FB:return pos_fb_SL;
	case MKLOG_ITEM_AH_POS_FB:return pos_fb_AH;

	case MKLOG_ITEM_MH_SPD_FB:return v_fb_MH;
	case MKLOG_ITEM_BH_SPD_FB:return v_fb_BH;
	case MKLOG_ITEM_AH_SPD_FB:return v_fb_AH;
	case MKLOG_ITEM_GT_SPD_FB:return v_fb_GT;
	case MKLOG_ITEM_SL_SPD_FB:return v_fb_SL;

	case MKLOG_ITEM_MH_SPD_REF:return v_ref_MH;
	case MKLOG_ITEM_BH_SPD_REF:return v_ref_BH;
	case MKLOG_ITEM_SL_SPD_REF:return v_ref_SL;
	case MKLOG_ITEM_AH_SPD_REF:return v_ref_AH;
	case MKLOG_ITEM_GT_SPD_REF:return v_ref_GT;

	case MKLOG_ITEM_SWY1_TH_X:return swy1_TH_X;
	case MKLOG_ITEM_SWY1_TH_Y:return swy1_TH_Y;
	case MKLOG_ITEM_SWY1_DTH_X:return swy1DTH_X;
	case MKLOG_ITEM_SWY1_DTH_Y:return swy1DTH_Y;
	case MKLOG_ITEM_SWY1_TX:return swy1_Tx;
	case MKLOG_ITEM_SWY1_TY:return swy1_Ty;

	case MKLOG_ITEM_SWY1_AMP_CAL_X:return swy1SWYAMP_CAL_X;
	case MKLOG_ITEM_SWY1_AMP_CAL_Y:return swy1SWYAMP_CAL_Y;
	case MKLOG_ITEM_SWY1_PH_CAL_X:return swy1SWYPH_CAL_X;
	case MKLOG_ITEM_SWY1_PH_CAL_Y:return swy1SWYPH_CAL_Y;
	case MKLOG_ITEM_SWY1_AMP_P2P_X:return swy1SWYAMP_P2P_X;
	case MKLOG_ITEM_SWY1_AMP_P2P_Y:return swy1SWYAMP_P2P_Y;
	case MKLOG_ITEM_SWY1_PH_TIME_X:return swy1SWYPH_TIME_X;
	case MKLOG_ITEM_SWY1_PH_TIME_Y:return swy1SWYPH_TIME_Y;
	case MKLOG_ITEM_MHL:return l_rope_mh;

	case MKLOG_ITEM_CONTROL_SOURCE:return control_source;
	case MKLOG_ITEM_CONTROL_ESTOP:return e_stop;
	case MKLOG_ITEM_GRIP_SWITCH:return auto_active_sw;

	case MKLOG_ITEM_NOTCH_COM_MH:return notch_com_mh;
	case MKLOG_ITEM_NOTCH_COM_GT:return notch_com_gt;
	case MKLOG_ITEM_NOTCH_COM_BH:return notch_com_bh;
	case MKLOG_ITEM_NOTCH_COM_SL:return notch_com_sl;
	case MKLOG_ITEM_NOTCH_COM_AH:return notch_com_ah;

	case MKLOG_ITEM_SIN_1HZ:return sin1hz;
	case MKLOG_ITEM_COS_1HZ:return cos1hz;
	case MKLOG_ITEM_SIN_1HZ01HZ:return sin1hz01hz;
	case MKLOG_ITEM_COS_1HZ01HZ:return cos1hz01hz;

	case MKLOG_ITEM_ONOFF2HZ:return onoff2hz;
	case MKLOG_ITEM_ONOFF10HZ:return onoff10hz;

	default:return empty;
	}
}

/// <summary>
/// ライブラリからのインターフェイス（処理要求）関数
/// </summary>
/// <param name="com_code"></param>
/// <param name="param"></param>
/// <param name="pparam"></param>
/// <returns></returns>
int CScada::req_command(WORD com_code, WORD param, void* pparam) {
	switch (com_code) {
	case MKLOG_MSGID_REFRESH_TMCHART_ITEM: {
		for (int i = 0; i < MKLOG_N_CHART_MAX; i++) {
			logsource.header_time.code[i + 2] = *((int*)pparam + i);
			logsource.header_time.d100[i + 2] = st_log_db.item[logsource.header_time.code[i + 2]].d100;
			for (int j = 0; j < MKLOG_N_LOG_TITLE_WCH; j++)logsource.header_time.title[i + 2][j] = st_log_db.item[logsource.header_time.code[i + 2]].title[j];
		}
	}break;
	case MKLOG_MSGID_REFRESH_SCCHART_ITEM: {
		for (int i = 0; i < MKLOG_N_CHART_MAX; i++) {
			logsource.header_scat.code[i + 2] = *((int*)pparam + i);
			logsource.header_scat.d100[i + 2] = st_log_db.item[logsource.header_scat.code[i + 2]].d100;
			for (int j = 0; j < MKLOG_N_LOG_TITLE_WCH; j++)logsource.header_scat.title[i + 2][j] = st_log_db.item[logsource.header_scat.code[i + 2]].title[j];
		}
	}break;
	default:break;
	}
	return 0;
}


///###	タブパネルのListViewにメッセージを出力
void CScada::msg2listview(wstring wstr) {

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
void CScada::set_PNLparam_value(float p1, float p2, float p3, float p4, float p5, float p6) {
	wstring wstr;
	wstr += std::to_wstring(p1); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT1), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p2); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT2), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p3); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT3), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p4); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT4), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p5); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT5), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p6); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT6), wstr.c_str());
}
//タブパネルのEdit Box説明テキストを設定
void CScada::set_panel_tip_txt() {
	wstring wstr_type; wstring wstr;
	switch (inf.panel_func_id) {
	case IDC_TASK_FUNC_RADIO2: {
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
void CScada::set_func_pb_txt() {
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO1, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO2, L"LOG");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO3, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO4, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO5, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO6, L"-");
	return;
}
//タブパネルのItem chkテキストを設定
void CScada::set_item_chk_txt() {
	wstring wstr_type; wstring wstr;
	switch (inf.panel_func_id) {
	case IDC_TASK_FUNC_RADIO2: {
		SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK1, L"EVENT");
		SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK2, L"TIME");
		SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK3, L"SCAT");
		SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK4, L"-");
		SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK5, L"-");
		SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK6, L"-");
	}break;
	case IDC_TASK_FUNC_RADIO1:
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




