#include "pch.h"
#include "framework.h"
#include "CSHAREDMEM.H"
#include "CSubPanel.h"

extern ST_DRAWING_BASE drawing_items;

ST_PLCIF_FLT CFaultPanel::st_flt;
INT32 CFaultPanel::flt_disp_mode;
PUINT16 CFaultPanel::pfltmap;

HWND CFaultPanel::open_window() {
	InitCommonControls();//コモンコントロール初期化
	HINSTANCE hInst = GetModuleHandle(0);

	WNDCLASSEXW wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = (HINSTANCE)GetModuleHandle(0);;
	wcex.hIcon = NULL;
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)GetStockObject(COLOR_WINDOW + 1);
	wcex.lpszMenuName = TEXT("OTE FAULT");
	wcex.lpszClassName = TEXT("OTE FAULT");
	wcex.hIconSm = NULL;

	ATOM fb = RegisterClassExW(&wcex);
	//メインウィンドウ
	hWnd = CreateWindowW(TEXT("OTE FAULT"), TEXT("故障情報"), WS_CHILD | WS_BORDER,//WS_OVERLAPPEDWINDOW,
		OTE_SUB_PANEL_X, OTE_SUB_PANEL_Y, OTE_SUB_PANEL_W, OTE_SUB_PANEL_H,
		hWndParent, nullptr, hInst, nullptr);

	InvalidateRect(hWnd, NULL, TRUE);//表示更新

	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);

	return hWnd;
}

HRESULT CFaultPanel::close_window() {
	if (hWnd) {
		DestroyWindow(hWnd);
	}
	return S_OK;
}

HRESULT CFaultPanel::move_window(POINT pt) {
	if (hWnd) {
		MoveWindow(hWnd,pt.x,pt.y, OTE_SUB_PANEL_W, OTE_SUB_PANEL_H,true);
	}
	return S_OK;
}

HRESULT CFaultPanel::setup_obj() {
	//スタティック　フォルトリスト
	st_obj.faultlist = new CStaticCtrl(
		ID_OTE_FLT_PANEL_BASE + ID_OTE_FLT_PANEL_STATIC_FLTS,
		&st_work.pt[ID_OTE_FLT_PANEL_STATIC_FLTS],
		&st_work.sz[ID_OTE_FLT_PANEL_STATIC_FLTS],
		st_work.text[ID_OTE_FLT_PANEL_STATIC_FLTS]
	);

	//ラジオボタン　故障種別
	st_obj.radio_fault_type = new CRadioCtrl(5);
	st_obj.radio_fault_type->pradio[0] = new CCbCtrl(
		ID_OTE_FLT_PANEL_BASE + ID_OTE_FLT_PANEL_RADIO_ALL,
		&st_work.pt[ID_OTE_FLT_PANEL_RADIO_ALL],
		&st_work.sz[ID_OTE_FLT_PANEL_RADIO_ALL],
		st_work.text[ID_OTE_FLT_PANEL_RADIO_ALL]
	);
	st_obj.radio_fault_type->pradio[1] = new CCbCtrl(
		ID_OTE_FLT_PANEL_BASE + ID_OTE_FLT_PANEL_RADIO_HEAVY,
		&st_work.pt[ID_OTE_FLT_PANEL_RADIO_HEAVY],
		&st_work.sz[ID_OTE_FLT_PANEL_RADIO_HEAVY],
		st_work.text[ID_OTE_FLT_PANEL_RADIO_HEAVY]
	);
	st_obj.radio_fault_type->pradio[2] = new CCbCtrl(
		ID_OTE_FLT_PANEL_BASE + ID_OTE_FLT_PANEL_RADIO_LIGHT,
		&st_work.pt[ID_OTE_FLT_PANEL_RADIO_LIGHT],
		&st_work.sz[ID_OTE_FLT_PANEL_RADIO_LIGHT],
		st_work.text[ID_OTE_FLT_PANEL_RADIO_LIGHT]
	);
	st_obj.radio_fault_type->pradio[3] = new CCbCtrl(
		ID_OTE_FLT_PANEL_BASE + ID_OTE_FLT_PANEL_RADIO_IL,
		&st_work.pt[ID_OTE_FLT_PANEL_RADIO_IL],
		&st_work.sz[ID_OTE_FLT_PANEL_RADIO_IL],
		st_work.text[ID_OTE_FLT_PANEL_RADIO_IL]
	);
	st_obj.radio_fault_type->pradio[4] = new CCbCtrl(
		ID_OTE_FLT_PANEL_BASE + ID_OTE_FLT_PANEL_RADIO_REMOTE,
		&st_work.pt[ID_OTE_FLT_PANEL_RADIO_REMOTE],
		&st_work.sz[ID_OTE_FLT_PANEL_RADIO_REMOTE],
		st_work.text[ID_OTE_FLT_PANEL_RADIO_REMOTE]
	);

	//故障リセットボタン
	st_obj.pb_reset = new CPbCtrl(
		ID_OTE_FLT_PANEL_BASE + ID_OTE_FLT_PANEL_PB_FLT_RESET,
		&st_work.pt[ID_OTE_FLT_PANEL_PB_FLT_RESET],
		&st_work.sz[ID_OTE_FLT_PANEL_PB_FLT_RESET],
		st_work.text[ID_OTE_FLT_PANEL_PB_FLT_RESET]
	);

	//インターロックバイパス　チェックボックス
	st_obj.cb_il_bypass = new CCbCtrl(
		ID_OTE_FLT_PANEL_BASE + ID_OTE_FLT_PANEL_CB_IL_BYPASS,
		&st_work.pt[ID_OTE_FLT_PANEL_CB_IL_BYPASS],
		&st_work.sz[ID_OTE_FLT_PANEL_CB_IL_BYPASS],
		st_work.text[ID_OTE_FLT_PANEL_CB_IL_BYPASS]
	);
	return S_OK;
}


std::wostringstream msg_wos;

LRESULT CALLBACK CFaultPanel::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

	switch (message)
	{
	case WM_TIMER: {

		//背景色決定用
		for (int k = PLC_IF_FLT_TYPE_HEAVY; k < PLC_IF_N_FLT_TYPE; k++) {
			st_flt.is_fault_exist[k] = false;
			for (int i = 0; i < PLC_IF_N_FAULT_WBUF; i++) {
				if (*(pfltmap+i) & st_flt.flt_mask[k][i]) {
					st_flt.is_fault_exist[k] = true;
					break;
				}
			}
		}

		msg_wos.str(L"");
		int i, j, flt_type;
		if (flt_disp_mode == PLC_IF_FLT_TYPE_HEAVY) {
			flt_type = PLC_IF_FLT_TYPE_HEAVY;msg_wos << L"#重故障#\n";
		}
		else if (flt_disp_mode == PLC_IF_FLT_TYPE_IL) {
			flt_type = PLC_IF_FLT_TYPE_IL;	msg_wos << L"#インターロック#\n";
		}
		else if (flt_disp_mode == PLC_IF_FLT_TYPE_LIGHT) {
			flt_type = PLC_IF_FLT_TYPE_LIGHT;msg_wos << L"#軽故障#\n";
		}
		else if (flt_disp_mode == PLC_IF_FLT_TYPE_REMOTE) {
			flt_type = PLC_IF_FLT_TYPE_LIGHT;msg_wos << L"#遠隔故障#\n";
		}
		else {
			flt_type = PLC_IF_FLT_TYPE_ALL;	msg_wos << L"#全故障#\n";
		}

		UINT16 masked_flt = 0xffff;
		UINT16 flt_count = 0;
		bool be_end = false;

		for (i = 0; i < PLC_IF_N_FAULT_WBUF; i++) {
			masked_flt = *(pfltmap + i) & st_flt.flt_mask[flt_type][i];
			if (masked_flt) {
				UINT chk_digit = 0x1;

				if (flt_count > PLC_IF_FLT_DISP_MAX)break;//表示可能数越でブレーク

				for (j = 0; j < PLC_IF_N_FAULT; j++) {
					if (flt_count > PLC_IF_FLT_DISP_MAX)break;//表示可能数越でブレーク

					if (masked_flt & chk_digit) {
						msg_wos << L"  " << st_flt.flt_msg[i][j] << L"\n";
					}
					chk_digit = chk_digit << 1;
				}
			}
			else
				continue;
		}

		SetWindowText(st_work.hctrl[ID_OTE_FLT_PANEL_STATIC_FLTS], msg_wos.str().c_str());
		InvalidateRect(hWnd, NULL, false);


	}break;
	case WM_CREATE: {
		InitCommonControls();//コモンコントロール初期化
		HINSTANCE hInst = (HINSTANCE)GetModuleHandle(0);
		
		//オブジェクト生成
		setup_obj();

		//ウィンドウにコントロール追加
		//表示切替ラジオ釦
		CCbCtrl* pcb = st_obj.radio_fault_type->pradio[0];
		pcb->set_wnd(
			CreateWindow(L"BUTTON",	pcb->txt.c_str(), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_PUSHLIKE | WS_GROUP,
				pcb->pt.X, pcb->pt.Y, pcb->sz.Width, pcb->sz.Height,hWnd, (HMENU)((LONGLONG)pcb->get_id()), hInst, NULL)
		);
		for (int i = 1, k = ID_OTE_FLT_PANEL_RADIO_HEAVY; k < ID_OTE_FLT_PANEL_RADIO_REMOTE; i++, k++) {
			pcb = st_obj.radio_fault_type->pradio[i];
			pcb->set_wnd(
				CreateWindow(L"BUTTON", pcb->txt.c_str(), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_PUSHLIKE,
					pcb->pt.X, pcb->pt.Y, pcb->sz.Width, pcb->sz.Height, hWnd, (HMENU)((LONGLONG)pcb->get_id()), hInst, NULL)
			);
		}
		//ラジオボタンの初期化
		st_obj.radio_fault_type->set(0);

		//故障リセットPB
		CPbCtrl* ppb = st_obj.pb_reset;
		ppb->set_wnd(
			CreateWindow(L"BUTTON", ppb->txt.c_str(), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_PUSHLIKE,
				ppb->pt.X, ppb->pt.Y, pcb->sz.Width, ppb->sz.Height, hWnd, (HMENU)((LONGLONG)ppb->get_id()), hInst, NULL)
		);

		//バイパスCB
		pcb = st_obj.cb_il_bypass;
		pcb->set_wnd(
			CreateWindow(L"BUTTON", pcb->txt.c_str(), WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX | BS_PUSHLIKE,
				pcb->pt.X, pcb->pt.Y, pcb->sz.Width, pcb->sz.Height, hWnd, (HMENU)((LONGLONG)pcb->get_id()), hInst, NULL)
		);


		//故障リスト表示STATIC
		CStaticCtrl* pst = st_obj.faultlist;

		pst->set_wnd(
			CreateWindow(L"BUTTON", pst->txt.c_str(), WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX | BS_PUSHLIKE,
				pst->pt.X, pst->pt.Y, pst->sz.Width, pst->sz.Height, hWnd, (HMENU)((LONGLONG)pst->get_id()), hInst, NULL)
		);

		SetTimer(hWnd, ID_OTE_FLT_PANEL_TIMER, OTE_FLT_PANEL_DISP_MS, NULL);

	}
	case WM_COMMAND: {

		int wmId = LOWORD(wParam);
		switch (wmId)
		{
		case ID_OTE_FLT_PANEL_BASE + ID_OTE_FLT_PANEL_PB_FLT_RESET:
		{
			st_obj.pb_reset->set(L_ON);//PB ON
		}break;
		case ID_OTE_FLT_PANEL_BASE + ID_OTE_FLT_PANEL_CB_IL_BYPASS:
		{
			st_obj.cb_il_bypass->get();//PB ON
		}break;

		case ID_OTE_FLT_PANEL_BASE + ID_OTE_FLT_PANEL_RADIO_ALL: {
			flt_disp_mode = ID_OTE_FLT_PANEL_RADIO_ALL;
		}break;
		case ID_OTE_FLT_PANEL_BASE + ID_OTE_FLT_PANEL_RADIO_HEAVY: {
			flt_disp_mode = ID_OTE_FLT_PANEL_RADIO_HEAVY;
		}break;
		case ID_OTE_FLT_PANEL_BASE + ID_OTE_FLT_PANEL_RADIO_IL: {
			flt_disp_mode = ID_OTE_FLT_PANEL_RADIO_IL;
		}break;
		case ID_OTE_FLT_PANEL_BASE + ID_OTE_FLT_PANEL_RADIO_LIGHT: {
			flt_disp_mode = ID_OTE_FLT_PANEL_RADIO_LIGHT;
		}break;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}

	}break;
	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		
		Graphics graphics(hdc);
		SolidBrush* pbrush = drawing_items.pbrush[ID_PANEL_COLOR_WHITE];

		if (st_flt.is_fault_exist[PLC_IF_FLT_TYPE_HEAVY])
			pbrush = drawing_items.pbrush[ID_PANEL_COLOR_RED];
		else if (st_flt.is_fault_exist[PLC_IF_FLT_TYPE_LIGHT])
			pbrush = drawing_items.pbrush[ID_PANEL_COLOR_WHITE];
		else if (st_flt.is_fault_exist[PLC_IF_FLT_TYPE_IL])
			pbrush = drawing_items.pbrush[ID_PANEL_COLOR_YELLOW];
		else;

		graphics.FillRectangle(pbrush,0,0,OTE_SUB_PANEL_W, OTE_SUB_PANEL_H);


		EndPaint(hWnd, &ps);
	}break;

	case WM_DESTROY: {
		//PostQuitMessage(0);

		KillTimer(hWnd,ID_OTE_FLT_PANEL_TIMER);
	}break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return S_OK;
}
