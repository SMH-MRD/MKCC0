
#include "CTiltSensor.h"
#include "resource.h"

LPST_TILT_SENSOR CTiltSensor::pst_work;
ST_TILT_MON1 CTiltSensor::st_mon1;
ST_TILT_MON2 CTiltSensor::st_mon2;

HRESULT CTiltSensor::initialize(LPVOID lpParam){

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


	CTiltSensor* pTiltObj = (CTiltSensor*)lpParam;
	pst_work = &(pTiltObj->st_work);

	int code = 0;

	wos.str(L"");
	if ((code = LALanioInit()) == 0) {
		wos<<L"LANIO Init NG : Code "<< LALanioGetLastError();
		msg2listview(wos.str());
		return S_FALSE;
	}
	else{
		wos <<L"LANIO Init OK : Code "<< code;
	}
	msg2listview(wos.str());

	wos.str(L"");

	if ((st_work.laniocount = LALanioSearch(st_work.timeout)) < 0) {
		wos << L"LANIO Search ERROR : Code "<< LALanioGetLastError();
		msg2listview(wos.str());
		return S_FALSE;
	}
	else if(st_work.laniocount == 0){
	//	LALanioConnectDirect(L"192,168,100,30", 1000);
		wos << L"LANIO Search CANNOT FIND";
		msg2listview(wos.str());
		return S_FALSE;
	}
	else {
		//IPアドレス確認
		wos << L"LANIO Search OK : Count " << st_work.laniocount;
		msg2listview(wos.str());

		for (int i = 0; i < st_work.laniocount; i++) {

			//ID,モデル確認
			int error = LALanioGetId(0, &st_work.lanio_id[i]);
			error = LALanioGetModel(i, &st_work.lanio_model[i]);
			wos.str(L"");
			if ((0 < st_work.lanio_model[i]) && (LANIO_N_MODEL > st_work.lanio_model[i]))
				wos << L"ID : " << st_work.lanio_id[i] << L"    MODEL : " << st_work.model_text[st_work.lanio_model[i]];
			else
				wos << L"ID : " << st_work.lanio_id[i] << L"    MODEL : ??";

			msg2listview(wos.str());

			char ipaddress[16];
			error = LALanioGetIpAddress(i, ipaddress);
			wos.str(L"");
			wos << L"IP : " << ipaddress;
			msg2listview(wos.str());

			st_work.hlanio[i] = LALanioConnect(i);
			wos.str(L"");
			if (st_work.hlanio[i] == -1) {
				wos << L"  Connect ERROR Code " << LALanioGetLastError();
				msg2listview(wos.str());
				return S_FALSE;;
			}
			else {
				wos << L" Connect OK : Handle " << st_work.hlanio;
				msg2listview(wos.str());
			}

			if (st_work.lanio_model[i] == LANIO_MODEL_LA_5AI) {

				wos.str(L"");
				wos << st_work.model_text[st_work.lanio_model[i]] << L" Set Range Code |";
				for (int j = 0; j < LANIO_N_CH_LA_5AI; j++) {
					if (LALanioSetAIRange(st_work.hlanio[i], j,st_work.lanio_ai_range[j])) {
						wos << st_work.lanio_ai_range[j] << L"|";
					}
					else {
						wos << L"NG|";
					}
				}
				msg2listview(wos.str());

				wos.str(L"");
				if( LALanioSetADCsps(st_work.hlanio[i], st_work.lanio_sps[LANIO_MODEL_LA_5AI])){
					wos << st_work.model_text[st_work.lanio_model[i]] << L" SPS set OK CODE:" << st_work.lanio_sps[LANIO_MODEL_LA_5AI];
				}
				else {
					wos << st_work.model_text[st_work.lanio_model[i]] << L" SPS set NG";
				}
				msg2listview(wos.str());

			}
		}
	}
	return S_OK;
}

HRESULT CTiltSensor::routine_work(void* pObj){
	input();
	parse();
	output();
	return S_OK;
}

int CTiltSensor::input() {

	for (int i = 0; i < st_work.laniocount; i++) {
		switch (st_work.lanio_model[i]) {
		case LANIO_MODEL_LA_5AI: {
			pst_work->lanio_stat[i] = LALanioAIAll(st_work.hlanio[i], st_work.lanio_ai_data);
		}break;
		default:break;
		}
	}

	return S_OK;
}

int CTiltSensor::close() {
	int error = LALanioEnd();
	return 0;
}

static wostringstream monwos;

LRESULT CALLBACK CTiltSensor::Mon1Proc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
	switch (msg)
	{
	case WM_CREATE: {
		InitCommonControls();//コモンコントロール初期化
		HINSTANCE hInst = (HINSTANCE)GetModuleHandle(0);
		//ウィンドウにコントロール追加
		st_mon1.hctrl[TILT_ID_MON1_STATIC_INF] = CreateWindowW(TEXT("STATIC"), st_mon1.text[TILT_ID_MON1_STATIC_INF], WS_CHILD | WS_VISIBLE | SS_LEFT,
			st_mon1.pt[TILT_ID_MON1_STATIC_INF].x, st_mon1.pt[TILT_ID_MON1_STATIC_INF].y, 
			st_mon1.sz[TILT_ID_MON1_STATIC_INF].cx, st_mon1.sz[TILT_ID_MON1_STATIC_INF].cy, 
			hWnd, (HMENU)(TILT_ID_MON1_CTRL_BASE + TILT_ID_MON1_STATIC_INF), hInst, NULL);

		//表示更新用タイマー
		SetTimer(hWnd, TILT_ID_MON1_TIMER, st_mon1.timer_ms, NULL);

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

		for (int i = 0; i < pst_work->laniocount; i++) {
			if (pst_work->lanio_model[i] == LANIO_MODEL_LA_5AI) {
				monwos << L"AI DATA  STATUS[" << pst_work->lanio_stat[i] << L"]\n";
				for (int j = 0; j < LANIO_N_CH_LA_5AI; j++) {
					monwos << L"Ch" << j + 1 << L": " << pst_work->lanio_ai_data[j] << L"\n";
				}
			}
		}

		SetWindowText(st_mon1.hctrl[TILT_ID_MON1_STATIC_INF], monwos.str().c_str());
	}break;

	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
	}break;
	case WM_DESTROY: {
		st_mon1.hwnd_mon= NULL;	
		KillTimer(hWnd, TILT_ID_MON1_TIMER);
	}break;
	default:
		return DefWindowProc(hWnd, msg, wp, lp);
	}
	return S_OK;
};

LRESULT CALLBACK CTiltSensor::Mon2Proc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
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

HWND CTiltSensor::open_monitor_wnd(HWND h_parent_wnd, int id) {
	
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
		wcex.lpszMenuName = TEXT("TILT_MON1");
		wcex.lpszClassName = TEXT("TILT_MON1");
		wcex.hIconSm = NULL;

		ATOM fb = RegisterClassExW(&wcex);

		st_mon1.hwnd_mon = inf.hwnd_mon1 = CreateWindowW(TEXT("TILT_MON1"), TEXT("TILT_MON1"), WS_OVERLAPPEDWINDOW,
			TILT_MON1_WND_X, TILT_MON1_WND_Y, TILT_MON1_WND_W, TILT_MON1_WND_H,
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
		wcex.lpszMenuName = TEXT("TILT_MON2");
		wcex.lpszClassName = TEXT("TILT_MON2");
		wcex.hIconSm = NULL;

		ATOM fb = RegisterClassExW(&wcex);

		st_mon2.hwnd_mon=inf.hwnd_mon2 = CreateWindowW(TEXT("TILT_MON2"), TEXT("TILT_MON2"), WS_OVERLAPPEDWINDOW,
			TILT_MON2_WND_X, TILT_MON2_WND_Y, TILT_MON2_WND_W, TILT_MON2_WND_H,
			h_parent_wnd, nullptr, hInst, nullptr);

		show_monitor_wnd(id);
		return inf.hwnd_mon2;
	}
	else
	{
		return NULL;
	};

	return NULL;
}
void CTiltSensor::close_monitor_wnd(int id) {
	if (id == BC_ID_MON1)
		DestroyWindow(inf.hwnd_mon1);
	else if (id == BC_ID_MON2)
		DestroyWindow(inf.hwnd_mon2);
	else;
	return;
}
void CTiltSensor::show_monitor_wnd(int id) { 
	if (id == BC_ID_MON1) {
		ShowWindow(inf.hwnd_mon1, SW_SHOW);
		UpdateWindow(inf.hwnd_mon1);
	}
	else if (id == BC_ID_MON2) {
		ShowWindow(inf.hwnd_mon2, SW_SHOW);
		UpdateWindow(inf.hwnd_mon2);
	}
	else;
	return;
}
void CTiltSensor::hide_monitor_wnd(int id) { 
	if (id == BC_ID_MON1)
		ShowWindow(inf.hwnd_mon1, SW_HIDE);
	else if (id == BC_ID_MON2)
		ShowWindow(inf.hwnd_mon2, SW_HIDE);
	else;	
	return;
}
											
/****************************************************************************/
/*   タスク設定タブパネルウィンドウのコールバック関数                       */
/****************************************************************************/
LRESULT CALLBACK CTiltSensor::PanelProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp) {

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
				open_monitor_wnd(inf.hwnd_parent,BC_ID_MON1);
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

///###	タブパネルのListViewにメッセージを出力
void CTiltSensor::msg2listview(wstring wstr) {

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

void CTiltSensor::set_PNLparam_value(float p1, float p2, float p3, float p4, float p5, float p6) {
	wstring wstr;
	wstr += std::to_wstring(p1); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT1), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p2); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT2), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p3); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT3), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p4); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT4), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p5); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT5), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p6); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT6), wstr.c_str());
}

//タブパネルのStaticテキストを設定
void CTiltSensor::set_panel_tip_txt() {
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
void CTiltSensor::set_func_pb_txt() {
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO1, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO2, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO3, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO4, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO5, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO6, L"-");
	return;
}
//タブパネルのItem chkテキストを設定
void CTiltSensor::set_item_chk_txt() {
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK1, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK2, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK3, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK4, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK5, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK6, L"-");
	return;
}

