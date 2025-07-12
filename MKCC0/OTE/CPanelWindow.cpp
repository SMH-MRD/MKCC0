#include "COpePanel.h"
#include "CPanelWindow.h"
#include "CPanelObj.h"
#include <commctrl.h>       //コモンコントロール用
#include "CBasicControl.h"
#include <vector>
#include "COteEnv.h"
#include <windows.h>
#include "CFaults.h"

extern vector<CBasicControl*>	VectCtrlObj;
extern BC_TASK_ID st_task_id;

extern CCrane* pCrane;

static COteEnv* pEnvObj;

HWND CMainPanelWindow::hPnlWnd;
HWND CMainPanelWindow::hParentWnd;
CPanelBase** CMainPanelWindow::ppPanelBase;
CPanelBase* CMainPanelWindow::pPanelBase;
int CMainPanelWindow::crane_id;
int CMainPanelWindow::wnd_code;

LPST_OTE_UI CMainPanelWindow::pUi;
LPST_OTE_CS_INF CMainPanelWindow::pCsInf;
LPST_OTE_CC_IF CMainPanelWindow::pCcIf;
LPST_OTE_ENV_INF CMainPanelWindow::pOteEnvInf;

CSubPanelWindow* CMainPanelWindow::pSubPanelWnd;

CMainPanelWindow::CMainPanelWindow(HINSTANCE hInstance, HWND hParent, int _crane_id, int _wnd_code, CPanelBase** _ppPanelBase) {
	ppPanelBase = _ppPanelBase;
	crane_id = _crane_id;
	wnd_code = _wnd_code;
	hParentWnd = hParent;

	const wchar_t CLASS_NAME[]			= L"MainWindowClass";
	const wchar_t CLASS_NAME_HHGH29[]	= L"MainWindowHHGH29";

	const wchar_t* pClassName;

	WNDCLASSEXW wcex;
	switch (crane_id) {
	case CARNE_ID_HHGH29:
		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = WndProcHHGH29;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = hInstance;
		wcex.hIcon = NULL;
		wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wcex.lpszMenuName = TEXT("OTE MAIN PANEL");
		wcex.lpszClassName = pClassName = CLASS_NAME_HHGH29;
		wcex.hIconSm = NULL;
		break;
	default:
		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = WndProcHHGH29;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = hInstance;
		wcex.hIcon = NULL;
		wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wcex.lpszMenuName = TEXT("OTE MAIN PANEL");
		wcex.lpszClassName = pClassName = CLASS_NAME;
		wcex.hIconSm = NULL;

		break;
	}

	ATOM fb = RegisterClassExW(&wcex);
	hPnlWnd = CreateWindowW(pClassName, TEXT("Operation Panel"), WS_OVERLAPPEDWINDOW,
		MAIN_PNL_WND_X, MAIN_PNL_WND_Y, MAIN_PNL_WND_W, MAIN_PNL_WND_H,
		hParentWnd , nullptr, hInstance, nullptr);

	if (hPnlWnd) {
		ShowWindow(hPnlWnd, SW_SHOW);
		UpdateWindow(hPnlWnd);
	}

	//サブウィンドウの共有メモリのポインタセット
	CSubPanelWindow::set_up(pUi, pCsInf, pCcIf, pOteEnvInf, crane_id);

	return;
}
CMainPanelWindow::~CMainPanelWindow()
{
	close();
}

void CMainPanelWindow::set_up(LPST_OTE_UI _pUi, LPST_OTE_CS_INF _pCsInf, LPST_OTE_CC_IF _pCcIf, LPST_OTE_ENV_INF _pOteEnvInf) {
	pUi			= _pUi;
	pCsInf		= _pCsInf;
	pCcIf		= _pCcIf;
	pOteEnvInf	= _pOteEnvInf;
	//### Environmentクラスインスタンスのポインタ取得
	pEnvObj = (COteEnv*)VectCtrlObj[st_task_id.ENV];
	return;
};
int CMainPanelWindow::close()
{
	DestroyWindow(hPnlWnd);
	return 0;
}

static bool is_initial_draw_main = false;
LRESULT CALLBACK CMainPanelWindow::WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {

	switch (msg)
	{
	case WM_CREATE: {
		InitCommonControls();//コモンコントロール初期化
		HINSTANCE hInst = (HINSTANCE)GetModuleHandle(0);

		pPanelBase = *ppPanelBase = new CPanelBase(crane_id, CODE_OTE_PNL_TYPE_MAIN, hWnd);
		//オブジェクトのグラフィックを設定
		pPanelBase->pmainobjs->setup_graphics(hWnd);
		pPanelBase->pmainobjs->refresh_obj_graphics();

		//ウィンドウにコントロール追加
		//STATIC,LABEL
		CStaticCtrl* pst = pPanelBase->pmainobjs->txt_uid;
		pst->set_wnd(CreateWindowW(TEXT("STATIC"), pst->txt.c_str(), WS_CHILD | WS_VISIBLE | SS_LEFT,
			pst->pt.X, pst->pt.Y, pst->sz.Width, pst->sz.Height, hWnd, (HMENU)(pst->id), hInst, NULL));


		pst = pPanelBase->pmainobjs->txt_ote_type;
		pst->set_wnd(CreateWindowW(TEXT("STATIC"), pst->txt.c_str(), WS_CHILD | WS_VISIBLE | SS_LEFT,
			pst->pt.X, pst->pt.Y, pst->sz.Width, pst->sz.Height, hWnd, (HMENU)(pst->id), hInst, NULL));

		pst = pPanelBase->pmainobjs->txt_link_crane;
		pst->set_wnd(CreateWindowW(TEXT("STATIC"), pst->txt.c_str(), WS_CHILD | WS_VISIBLE | SS_LEFT,
			pst->pt.X, pst->pt.Y, pst->sz.Width, pst->sz.Height, hWnd, (HMENU)(pst->id), hInst, NULL));

		pst = pPanelBase->pmainobjs->txt_freset;
		pst->set_wnd(CreateWindowW(TEXT("STATIC"), pst->txt.c_str(), WS_CHILD | WS_VISIBLE | SS_LEFT,
			pst->pt.X, pst->pt.Y, pst->sz.Width, pst->sz.Height, hWnd, (HMENU)(pst->id), hInst, NULL));

		//CB
		CCbCtrl* pcb = pPanelBase->pmainobjs->cb_estop;
		pcb->set_wnd(CreateWindowW(TEXT("BUTTON"), pcb->txt.c_str(), WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX | BS_PUSHLIKE | BS_MULTILINE | BS_OWNERDRAW,
			pcb->pt.X, pcb->pt.Y, pcb->sz.Width, pcb->sz.Height, hWnd, (HMENU)(pcb->id), hInst, NULL));
		pPanelBase->pmainobjs->lmp_estop->set_ctrl(pcb);//ランプにボタンのボタンコントロールをセット

		//操作器
		pcb = pPanelBase->pmainobjs->cb_pnl_notch;
		pcb->set_wnd(CreateWindowW(TEXT("BUTTON"), pcb->txt.c_str(), WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX | BS_PUSHLIKE | BS_MULTILINE,
			pcb->pt.X, pcb->pt.Y, pcb->sz.Width, pcb->sz.Height, hWnd, (HMENU)(pcb->id), hInst, NULL));

		//# PBL !!OWNER DRAW
		//主幹
		CPbCtrl* ppb = pPanelBase->pmainobjs->pb_syukan_on;
		ppb->set_wnd(CreateWindowW(TEXT("BUTTON"), ppb->txt.c_str(), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_PUSHLIKE | BS_OWNERDRAW,
			ppb->pt.X, ppb->pt.Y, ppb->sz.Width, ppb->sz.Height, hWnd, (HMENU)(ppb->id), hInst, NULL));
		pPanelBase->pmainobjs->lmp_syukan_on->set_ctrl(ppb);//ランプにボタンのボタンコントロールをセット
		ppb = pPanelBase->pmainobjs->pb_syukan_off;
		ppb->set_wnd(CreateWindowW(TEXT("BUTTON"), ppb->txt.c_str(), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_PUSHLIKE | BS_OWNERDRAW,
			ppb->pt.X, ppb->pt.Y, ppb->sz.Width, ppb->sz.Height, hWnd, (HMENU)(ppb->id), hInst, NULL));
		pPanelBase->pmainobjs->lmp_syukan_off->set_ctrl(ppb);//ランプにボタンのボタンコントロールをセット
		//Remote
		ppb = pPanelBase->pmainobjs->pb_remote;
		ppb->set_wnd(CreateWindowW(TEXT("BUTTON"), ppb->txt.c_str(), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_PUSHLIKE | BS_OWNERDRAW,
			ppb->pt.X, ppb->pt.Y, ppb->sz.Width, ppb->sz.Height, hWnd, (HMENU)(ppb->id), hInst, NULL));
		pPanelBase->pmainobjs->lmp_remote->set_ctrl(ppb);//ランプにボタンのボタンコントロールをセット

		//PAD MODE
		ppb = pPanelBase->pmainobjs->pb_pad_mode;
		ppb->set_wnd(CreateWindowW(TEXT("BUTTON"), ppb->txt.c_str(), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_PUSHLIKE | BS_OWNERDRAW,
			ppb->pt.X, ppb->pt.Y, ppb->sz.Width, ppb->sz.Height, hWnd, (HMENU)(ppb->id), hInst, NULL));
		pPanelBase->pmainobjs->lmp_pad_mode->set_ctrl(ppb);//ランプにボタンのウィンドウハンドルをセット

		//PB
		//認証
		ppb = pPanelBase->pmainobjs->pb_auth;
		ppb->set_wnd(CreateWindowW(TEXT("BUTTON"), ppb->txt.c_str(), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_PUSHLIKE,
			ppb->pt.X, ppb->pt.Y, ppb->sz.Width, ppb->sz.Height, hWnd, (HMENU)(ppb->id), hInst, NULL));
		//端末設定
		ppb = pPanelBase->pmainobjs->pb_ote_type_wnd;
		ppb->set_wnd(CreateWindowW(TEXT("BUTTON"), ppb->txt.c_str(), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_PUSHLIKE,
			ppb->pt.X, ppb->pt.Y, ppb->sz.Width, ppb->sz.Height, hWnd, (HMENU)(ppb->id), hInst, NULL));
		//アシスト
		ppb = pPanelBase->pmainobjs->pb_assist_func;
		ppb->set_wnd(CreateWindowW(TEXT("BUTTON"), ppb->txt.c_str(), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_PUSHLIKE,
			ppb->pt.X, ppb->pt.Y, ppb->sz.Width, ppb->sz.Height, hWnd, (HMENU)(ppb->id), hInst, NULL));
		//クレーン選択
		ppb = pPanelBase->pmainobjs->pb_crane_sel_wnd;
		ppb->set_wnd(CreateWindowW(TEXT("BUTTON"), ppb->txt.c_str(), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_PUSHLIKE,
			ppb->pt.X, ppb->pt.Y, ppb->sz.Width, ppb->sz.Height, hWnd, (HMENU)(ppb->id), hInst, NULL));
		//故障リセット
		ppb = pPanelBase->pmainobjs->pb_freset;
		ppb->set_wnd(CreateWindowW(TEXT("BUTTON"), ppb->txt.c_str(), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_PUSHLIKE | BS_OWNERDRAW,
			ppb->pt.X, ppb->pt.Y, ppb->sz.Width, ppb->sz.Height, hWnd, (HMENU)(ppb->id), hInst, NULL));
		pPanelBase->pmainobjs->lmp_freset->set_ctrl(ppb);//ランプにボタンのボタンコントロールをセット

		//RADIO BUTTON
		pcb = pPanelBase->pmainobjs->cb_disp_mode1;
		pcb->set_wnd(CreateWindowW(TEXT("BUTTON"), pcb->txt.c_str(), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_PUSHLIKE | BS_MULTILINE | WS_GROUP,
			pcb->pt.X, pcb->pt.Y, pcb->sz.Width, pcb->sz.Height, hWnd, (HMENU)(pcb->id), hInst, NULL));
		pcb = pPanelBase->pmainobjs->cb_disp_mode2;
		pcb->set_wnd(CreateWindowW(TEXT("BUTTON"), pcb->txt.c_str(), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_PUSHLIKE | BS_MULTILINE,
			pcb->pt.X, pcb->pt.Y, pcb->sz.Width, pcb->sz.Height, hWnd, (HMENU)(pcb->id), hInst, NULL));

		pcb = pPanelBase->pmainobjs->cb_opt_flt;
		pcb->set_wnd(CreateWindowW(TEXT("BUTTON"), pcb->txt.c_str(), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_PUSHLIKE | BS_MULTILINE | WS_GROUP,
			pcb->pt.X, pcb->pt.Y, pcb->sz.Width, pcb->sz.Height, hWnd, (HMENU)(pcb->id), hInst, NULL));
		pcb = pPanelBase->pmainobjs->cb_opt_set;
		pcb->set_wnd(CreateWindowW(TEXT("BUTTON"), pcb->txt.c_str(), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_PUSHLIKE | BS_MULTILINE,
			pcb->pt.X, pcb->pt.Y, pcb->sz.Width, pcb->sz.Height, hWnd, (HMENU)(pcb->id), hInst, NULL));
		pcb = pPanelBase->pmainobjs->cb_opt_com;
		pcb->set_wnd(CreateWindowW(TEXT("BUTTON"), pcb->txt.c_str(), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_PUSHLIKE | BS_MULTILINE,
			pcb->pt.X, pcb->pt.Y, pcb->sz.Width, pcb->sz.Height, hWnd, (HMENU)(pcb->id), hInst, NULL));
		pcb = pPanelBase->pmainobjs->cb_opt_cam;
		pcb->set_wnd(CreateWindowW(TEXT("BUTTON"), pcb->txt.c_str(), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_PUSHLIKE | BS_MULTILINE,
			pcb->pt.X, pcb->pt.Y, pcb->sz.Width, pcb->sz.Height, hWnd, (HMENU)(pcb->id), hInst, NULL));
		pcb = pPanelBase->pmainobjs->cb_opt_stat;
		pcb->set_wnd(CreateWindowW(TEXT("BUTTON"), pcb->txt.c_str(), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_PUSHLIKE | BS_MULTILINE,
			pcb->pt.X, pcb->pt.Y, pcb->sz.Width, pcb->sz.Height, hWnd, (HMENU)(pcb->id), hInst, NULL));
		pcb = pPanelBase->pmainobjs->cb_opt_clr;
		pcb->set_wnd(CreateWindowW(TEXT("BUTTON"), pcb->txt.c_str(), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_PUSHLIKE | BS_MULTILINE,
			pcb->pt.X, pcb->pt.Y, pcb->sz.Width, pcb->sz.Height, hWnd, (HMENU)(pcb->id), hInst, NULL));

		//String 初回書き込みフラグをセットしてTIMERイベントで書き込み
		is_initial_draw_main = true;

		//Lamp ウィンドウハンドルセット,フリッカ設定
		//CCとの通信状態表示
		INT32 id_list[2] = { 4,0 };//2種フリッカ　緑/暗青
		pPanelBase->pmainobjs->lmp_pcr->set_wnd(hWnd);		pPanelBase->pmainobjs->lmp_pcr->setup_flick(2, 3, id_list);
		pPanelBase->pmainobjs->lmp_pcs->set_wnd(hWnd);		pPanelBase->pmainobjs->lmp_pcs->setup_flick(2, 3, id_list);
		pPanelBase->pmainobjs->lmp_plcr->set_wnd(hWnd);		pPanelBase->pmainobjs->lmp_plcr->setup_flick(2, 3, id_list);
		pPanelBase->pmainobjs->lmp_plcs->set_wnd(hWnd);		pPanelBase->pmainobjs->lmp_plcs->setup_flick(2, 3, id_list);

		//表示更新用タイマー
		SetTimer(hWnd, ID_MAIN_PANEL_TIMER, ID_MAIN_PANEL_TIMER_MS, NULL);
		break;
	}
	case WM_COMMAND: {
		int wmId = LOWORD(wp);
		// 選択されたメニューの解析:
		switch (wmId)
		{
		case ID_MAIN_PNL_OBJ_CB_ESTOP: {
			if (pPanelBase->pmainobjs->cb_estop->get() == BST_CHECKED) {
				pPanelBase->pmainobjs->cb_estop->set(BST_UNCHECKED);
				pPanelBase->pmainobjs->lmp_estop->set(L_OFF);
			}
			else {
				pPanelBase->pmainobjs->cb_estop->set(BST_CHECKED);
				pPanelBase->pmainobjs->lmp_estop->set(L_ON);
			}
		}break;
		case ID_MAIN_PNL_OBJ_PB_SYUKAN_ON: {
			pPanelBase->pmainobjs->pb_syukan_on->update(true);
		}break;
		case ID_MAIN_PNL_OBJ_PB_SYUKAN_OFF: {
			pPanelBase->pmainobjs->pb_syukan_off->update(true);
		}break;
		case ID_MAIN_PNL_OBJ_PB_REMOTE: {
			pPanelBase->pmainobjs->pb_remote->update(true);
		}break;
		case ID_MAIN_PNL_OBJ_PB_PAD_MODE: {

			pPanelBase->pmainobjs->pb_pad_mode->update(true);
		}break;
		case ID_MAIN_PNL_OBJ_PB_ASSIST_FUNC: {
			pPanelBase->pmainobjs->pb_assist_func->update(true);
		}break;
		case ID_MAIN_PNL_OBJ_PB_OTE_TYPE_WND: {
			pPanelBase->pmainobjs->pb_ote_type_wnd->update(true);
		}break;
		case ID_MAIN_PNL_OBJ_PB_CRANE_SEL_WND: {
			pPanelBase->pmainobjs->pb_crane_sel_wnd->update(true);
		}break;
		case ID_MAIN_PNL_OBJ_PB_AUTH: {
			pPanelBase->pmainobjs->pb_auth->update(true);
		}break;
		case ID_MAIN_PNL_OBJ_CB_PNL_NOTCH: {
			pPanelBase->pmainobjs->cb_pnl_notch->update();
		}break;

		case ID_MAIN_PNL_OBJ_RDO_DISP_MODE1:
		case ID_MAIN_PNL_OBJ_RDO_DISP_MODE2:
		{
			pPanelBase->pmainobjs->rdo_disp_mode->update(true);
		}break;

		case ID_MAIN_PNL_OBJ_RDO_OPT_WND_FLT:
		case ID_MAIN_PNL_OBJ_RDO_OPT_WND_SET:
		case ID_MAIN_PNL_OBJ_RDO_OPT_WND_COM:
		case ID_MAIN_PNL_OBJ_RDO_OPT_WND_CAM:
		case ID_MAIN_PNL_OBJ_RDO_OPT_WND_STAT:
		{
			HINSTANCE hInst = (HINSTANCE)GetModuleHandle(0);

			if (pSubPanelWnd == NULL) {
				pSubPanelWnd = new CSubPanelWindow(hInst, hWnd, crane_id, wmId, pPanelBase);
			}
			else {
				if (pSubPanelWnd->wnd_code == wmId) {
					delete pSubPanelWnd;
					pSubPanelWnd = NULL;
				}
				else {
					delete pSubPanelWnd;
					pSubPanelWnd = new CSubPanelWindow(hInst, hWnd,crane_id, wmId,pPanelBase);
				}
			}

			InvalidateRect(hWnd, NULL, TRUE); // ウィンドウ全体を再描画

		}break;
		case ID_MAIN_PNL_OBJ_RDO_OPT_WND_CLR:
		{
			delete pSubPanelWnd;
			pSubPanelWnd = NULL;
			InvalidateRect(hWnd, NULL, TRUE); // ウィンドウ全体を再描画
		}break;

		case ID_MAIN_PNL_OBJ_PB_FRESET: {
			pPanelBase->pmainobjs->pb_freset->update(true);
		}break;

		default:
			return DefWindowProc(hWnd, msg, wp, lp);
		}
	}break;

	case WM_LBUTTONUP: {//マウス左ボタン押下でモニタウィンドウ描画更新
		InvalidateRect(hWnd, NULL, TRUE); // ウィンドウ全体を再描画
	}
	case WM_CTLCOLORSTATIC: {//スタティックテキストの色セット
		SetTextColor((HDC)wp, RGB(220, 220, 220)); // ライトグレー
		SetBkMode((HDC)wp, TRANSPARENT);
	}return (LRESULT)GetStockObject(NULL_BRUSH); // 背景色に合わせる
	case WM_ERASEBKGND: {//ウィンドウの背景色をグレーに
		pPanelBase->pmainobjs->pgraphic->FillRectangle(pPanelBase->pmainobjs->pBrushBk, pPanelBase->pmainobjs->rc_panel);

	}return 1; // 背景を処理したことを示す
	case WM_TIMER: {
		//# LAMP(CTRL)更新
		//e-stop : PLCの認識がESTOPの時枠有表示
		INT16 code = pCcIf->st_msg_pc_u_rcv.body.st.lamp[OTE_PNL_CTRLS::estop].code;//制御PC受信バッファの指令内容
		int val = pPanelBase->pmainobjs->cb_estop->get();									//CBの状態
		//CB状態で制御PCのON表示があれば表示画像を切替
		if (val == BST_CHECKED) { if (code) val = 3; }
		else { if (code)val = 2; }
		pPanelBase->pmainobjs->lmp_estop->set(val);
		pPanelBase->pmainobjs->lmp_estop->update();

		//主幹
		code = pCcIf->st_msg_pc_u_rcv.body.st.lamp[OTE_PNL_CTRLS::syukan_on].code;
		pPanelBase->pmainobjs->lmp_syukan_on->set(code); pPanelBase->pmainobjs->lmp_syukan_on->update();
		code = pCcIf->st_msg_pc_u_rcv.body.st.lamp[OTE_PNL_CTRLS::syukan_off].code;
		pPanelBase->pmainobjs->lmp_syukan_off->set(code); pPanelBase->pmainobjs->lmp_syukan_off->update();

		//遠隔
		pPanelBase->pmainobjs->lmp_remote->set(pCsInf->st_body.remote);
		pPanelBase->pmainobjs->lmp_remote->update();

		pPanelBase->pmainobjs->lmp_pad_mode->set(pCsInf->st_body.game_pad_mode);
		pPanelBase->pmainobjs->lmp_pad_mode->update();

		//故障リセット
	//	pPanelBase->pmainobjs->lmp_freset->set(pOteCsInf->st_body.ctrl_ope[OTE_PNL_CTRLS::fault_reset]);
		if (pUi->ctrl_stat[OTE_PNL_CTRLS::fault_reset])
			pPanelBase->pmainobjs->lmp_freset->set(L_ON);
		else
			pPanelBase->pmainobjs->lmp_freset->set(L_OFF);

		pPanelBase->pmainobjs->lmp_freset->update();


		//# SwitchImg更新(ランプ）
		//CCとの通信状態表示(受信）
		if (pCcIf->cc_com_stat_r == ID_PNL_SOCK_STAT_ACT_RCV)
			pPanelBase->pmainobjs->lmp_pcr->set(ID_PANEL_LAMP_FLICK);
		else pPanelBase->pmainobjs->lmp_pcr->set(pCcIf->cc_com_stat_r);
		pPanelBase->pmainobjs->lmp_pcr->update();
		//CCとの通信状態表示(送信）
		if (pCcIf->cc_com_stat_s == ID_PNL_SOCK_STAT_ACT_SND)
			pPanelBase->pmainobjs->lmp_pcs->set(ID_PANEL_LAMP_FLICK);
		else pPanelBase->pmainobjs->lmp_pcs->set(pCcIf->cc_com_stat_s);
		pPanelBase->pmainobjs->lmp_pcs->update();
		//PLCとの通信状態表示(受信）
		pPanelBase->pmainobjs->lmp_plcr->update();
		//PLCとの通信状態表示(送信）
		pPanelBase->pmainobjs->lmp_plcs->update();

		//PB状態更新(オフディレイカウントダウン)
		pPanelBase->pmainobjs->pb_syukan_on->update(false);
		pPanelBase->pmainobjs->pb_syukan_off->update(false);
		pPanelBase->pmainobjs->pb_remote->update(false);
		pPanelBase->pmainobjs->pb_auth->update(false);
		pPanelBase->pmainobjs->pb_assist_func->update(false);
		pPanelBase->pmainobjs->pb_crane_sel_wnd->update(false);
		pPanelBase->pmainobjs->pb_ote_type_wnd->update(false);
		pPanelBase->pmainobjs->pb_pad_mode->update(false);
		pPanelBase->pmainobjs->pb_freset->update(false);

		//String更新
		if (is_initial_draw_main) {
			pPanelBase->pmainobjs->str_message->update();
			pPanelBase->pmainobjs->str_pc_com_stat->update();
			pPanelBase->pmainobjs->str_plc_com_stat->update();
			//	is_initial_draw_mon1 = false;
		}

		//GamePadチェック
		if (pCsInf->gpad_in.syukan_on) pPanelBase->pmainobjs->pb_syukan_on->update(true);
		if (pCsInf->gpad_in.syukan_off)pPanelBase->pmainobjs->pb_syukan_off->update(true);
		if (pCsInf->gpad_in.remote)pPanelBase->pmainobjs->pb_remote->update(true);
		if (pCsInf->gpad_in.estop)pPanelBase->pmainobjs->cb_estop->set(BST_CHECKED);
		if (pCsInf->gpad_in.f_reset) {//ゲームパッドの非常停止クリアはResetPBで実行
			pPanelBase->pmainobjs->pb_freset->update(true);
			pPanelBase->pmainobjs->cb_estop->set(BST_UNCHECKED);
		}

	}break;

	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);

		//PatBlt(hdc, 0, 0, OTE_SCAD_MON1_WND_W, OTE_SCAD_MON1_WND_H, RGB(64, 64, 64));

		EndPaint(hWnd, &ps);
	}break;
	case WM_DRAWITEM: {//ランプ表示を更新 TIMERイベントで状態変化チェックしてInvalidiateRectで呼び出し
		DRAWITEMSTRUCT* pDIS = (DRAWITEMSTRUCT*)lp;
		Image* image;
		Gdiplus::Graphics gra(pDIS->hDC);
		Font* pfont = NULL;
		CMainPanelObj* pos = pPanelBase->pmainobjs;
		CLampCtrl* plamp = NULL;

		if (pDIS->CtlID == pos->cb_estop->id) {
			plamp = pos->lmp_estop;	pfont = NULL;
		}
		else if (pDIS->CtlID == pos->pb_remote->id) {//リモートランプ
			plamp = pos->lmp_remote; pfont = plamp->pFont;
		}
		else if (pDIS->CtlID == pos->pb_syukan_on->id) {//主幹入ランプ
			plamp = pos->lmp_syukan_on; pfont = plamp->pFont;
		}
		else if (pDIS->CtlID == pos->pb_syukan_off->id) {//主幹切ランプ
			plamp = pos->lmp_syukan_off; pfont = plamp->pFont;
		}
		else if (pDIS->CtlID == pos->pb_pad_mode->id) {//PADランプ
			plamp = pos->lmp_pad_mode; pfont = plamp->pFont;
		}
		else if (pDIS->CtlID == pos->pb_freset->id) {//FAULTランプ
			plamp = pos->lmp_freset; pfont = plamp->pFont;
		}
		else return false;

		image = plamp->pimg[plamp->get()];
		gra.FillRectangle(pPanelBase->pmainobjs->pBrushBk, plamp->rc);											//背景色セット
		if (image) gra.DrawImage(image, plamp->rc);															//イメージ描画
		if (pfont != NULL)
			gra.DrawString(plamp->txt.c_str(), -1, pfont, plamp->frc, plamp->pStrFormat, plamp->pTxtBrush);	//テキスト描画

	}return true;
	case WM_DESTROY: {
		hWnd = NULL;
		KillTimer(hWnd, ID_MAIN_PANEL_TIMER);
		//### オープニング画面を再表示
		pEnvObj->open_opening_window();

	}break;
	default:
		return DefWindowProc(hWnd, msg, wp, lp);
	}
	return S_OK;
};
LRESULT CALLBACK CMainPanelWindow::WndProcHHGH29(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {

	switch (msg)
	{
	case WM_CREATE: {
		InitCommonControls();//コモンコントロール初期化
		HINSTANCE hInst = (HINSTANCE)GetModuleHandle(0);
		
		pPanelBase = *ppPanelBase = new CPanelBase(crane_id, CODE_OTE_PNL_TYPE_MAIN, hWnd);
		//オブジェクトのグラフィックを設定
		pPanelBase->pmainobjs->setup_graphics(hWnd);
		pPanelBase->pmainobjs->refresh_obj_graphics();

		//ウィンドウにコントロール追加
		//STATIC,LABEL
		CStaticCtrl* pst = pPanelBase->pmainobjs->txt_uid;
		pst->set_wnd(CreateWindowW(TEXT("STATIC"), pst->txt.c_str(), WS_CHILD | WS_VISIBLE | SS_LEFT,
			pst->pt.X, pst->pt.Y, pst->sz.Width, pst->sz.Height, hWnd, (HMENU)(pst->id), hInst, NULL));


		pst = pPanelBase->pmainobjs->txt_ote_type;
		pst->set_wnd(CreateWindowW(TEXT("STATIC"), pst->txt.c_str(), WS_CHILD | WS_VISIBLE | SS_LEFT,
			pst->pt.X, pst->pt.Y, pst->sz.Width, pst->sz.Height, hWnd, (HMENU)(pst->id), hInst, NULL));

		pst = pPanelBase->pmainobjs->txt_link_crane;
		pst->set_wnd(CreateWindowW(TEXT("STATIC"), pst->txt.c_str(), WS_CHILD | WS_VISIBLE | SS_LEFT,
			pst->pt.X, pst->pt.Y, pst->sz.Width, pst->sz.Height, hWnd, (HMENU)(pst->id), hInst, NULL));

		pst = pPanelBase->pmainobjs->txt_freset;
		pst->set_wnd(CreateWindowW(TEXT("STATIC"), pst->txt.c_str(), WS_CHILD | WS_VISIBLE | SS_LEFT,
			pst->pt.X, pst->pt.Y, pst->sz.Width, pst->sz.Height, hWnd, (HMENU)(pst->id), hInst, NULL));

		//CB
		CCbCtrl* pcb = pPanelBase->pmainobjs->cb_estop;
		pcb->set_wnd(CreateWindowW(TEXT("BUTTON"), pcb->txt.c_str(), WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX | BS_PUSHLIKE | BS_MULTILINE | BS_OWNERDRAW,
			pcb->pt.X, pcb->pt.Y, pcb->sz.Width, pcb->sz.Height, hWnd, (HMENU)(pcb->id), hInst, NULL));
		pPanelBase->pmainobjs->lmp_estop->set_ctrl(pcb);//ランプにボタンのボタンコントロールをセット

		//操作器
		pcb = pPanelBase->pmainobjs->cb_pnl_notch;
		pcb->set_wnd(CreateWindowW(TEXT("BUTTON"), pcb->txt.c_str(), WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX | BS_PUSHLIKE | BS_MULTILINE,
			pcb->pt.X, pcb->pt.Y, pcb->sz.Width, pcb->sz.Height, hWnd, (HMENU)(pcb->id), hInst, NULL));

		//# PBL !!OWNER DRAW
		//主幹
		CPbCtrl* ppb = pPanelBase->pmainobjs->pb_syukan_on;
		ppb->set_wnd(CreateWindowW(TEXT("BUTTON"), ppb->txt.c_str(), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_PUSHLIKE | BS_OWNERDRAW,
			ppb->pt.X, ppb->pt.Y, ppb->sz.Width, ppb->sz.Height, hWnd, (HMENU)(ppb->id), hInst, NULL));
		pPanelBase->pmainobjs->lmp_syukan_on->set_ctrl(ppb);//ランプにボタンのボタンコントロールをセット
		ppb = pPanelBase->pmainobjs->pb_syukan_off;
		ppb->set_wnd(CreateWindowW(TEXT("BUTTON"), ppb->txt.c_str(), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_PUSHLIKE | BS_OWNERDRAW,
			ppb->pt.X, ppb->pt.Y, ppb->sz.Width, ppb->sz.Height, hWnd, (HMENU)(ppb->id), hInst, NULL));
		pPanelBase->pmainobjs->lmp_syukan_off->set_ctrl(ppb);//ランプにボタンのボタンコントロールをセット
		//Remote
		ppb = pPanelBase->pmainobjs->pb_remote;
		ppb->set_wnd(CreateWindowW(TEXT("BUTTON"), ppb->txt.c_str(), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_PUSHLIKE | BS_OWNERDRAW,
			ppb->pt.X, ppb->pt.Y, ppb->sz.Width, ppb->sz.Height, hWnd, (HMENU)(ppb->id), hInst, NULL));
		pPanelBase->pmainobjs->lmp_remote->set_ctrl(ppb);//ランプにボタンのボタンコントロールをセット

		//PAD MODE
		ppb = pPanelBase->pmainobjs->pb_pad_mode;
		ppb->set_wnd(CreateWindowW(TEXT("BUTTON"), ppb->txt.c_str(), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_PUSHLIKE | BS_OWNERDRAW,
			ppb->pt.X, ppb->pt.Y, ppb->sz.Width, ppb->sz.Height, hWnd, (HMENU)(ppb->id), hInst, NULL));
		pPanelBase->pmainobjs->lmp_pad_mode->set_ctrl(ppb);//ランプにボタンのウィンドウハンドルをセット

		//PB
		//認証
		ppb = pPanelBase->pmainobjs->pb_auth;
		ppb->set_wnd(CreateWindowW(TEXT("BUTTON"), ppb->txt.c_str(), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_PUSHLIKE,
			ppb->pt.X, ppb->pt.Y, ppb->sz.Width, ppb->sz.Height, hWnd, (HMENU)(ppb->id), hInst, NULL));
		//端末設定
		ppb = pPanelBase->pmainobjs->pb_ote_type_wnd;
		ppb->set_wnd(CreateWindowW(TEXT("BUTTON"), ppb->txt.c_str(), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_PUSHLIKE,
			ppb->pt.X, ppb->pt.Y, ppb->sz.Width, ppb->sz.Height, hWnd, (HMENU)(ppb->id), hInst, NULL));
		//アシスト
		ppb = pPanelBase->pmainobjs->pb_assist_func;
		ppb->set_wnd(CreateWindowW(TEXT("BUTTON"), ppb->txt.c_str(), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_PUSHLIKE,
			ppb->pt.X, ppb->pt.Y, ppb->sz.Width, ppb->sz.Height, hWnd, (HMENU)(ppb->id), hInst, NULL));
		//クレーン選択
		ppb = pPanelBase->pmainobjs->pb_crane_sel_wnd;
		ppb->set_wnd(CreateWindowW(TEXT("BUTTON"), ppb->txt.c_str(), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_PUSHLIKE,
			ppb->pt.X, ppb->pt.Y, ppb->sz.Width, ppb->sz.Height, hWnd, (HMENU)(ppb->id), hInst, NULL));
		//故障リセット
		ppb = pPanelBase->pmainobjs->pb_freset;
		ppb->set_wnd(CreateWindowW(TEXT("BUTTON"), ppb->txt.c_str(), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_PUSHLIKE | BS_OWNERDRAW,
			ppb->pt.X, ppb->pt.Y, ppb->sz.Width, ppb->sz.Height, hWnd, (HMENU)(ppb->id), hInst, NULL));
		pPanelBase->pmainobjs->lmp_freset->set_ctrl(ppb);//ランプにボタンのボタンコントロールをセット

		//RADIO BUTTON
		pcb = pPanelBase->pmainobjs->cb_disp_mode1;
		pcb->set_wnd(CreateWindowW(TEXT("BUTTON"), pcb->txt.c_str(), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_PUSHLIKE | BS_MULTILINE | WS_GROUP,
			pcb->pt.X, pcb->pt.Y, pcb->sz.Width, pcb->sz.Height, hWnd, (HMENU)(pcb->id), hInst, NULL));
		pcb = pPanelBase->pmainobjs->cb_disp_mode2;
		pcb->set_wnd(CreateWindowW(TEXT("BUTTON"), pcb->txt.c_str(), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_PUSHLIKE | BS_MULTILINE,
			pcb->pt.X, pcb->pt.Y, pcb->sz.Width, pcb->sz.Height, hWnd, (HMENU)(pcb->id), hInst, NULL));

		pcb = pPanelBase->pmainobjs->cb_opt_flt;
		pcb->set_wnd(CreateWindowW(TEXT("BUTTON"), pcb->txt.c_str(), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_PUSHLIKE | BS_MULTILINE | WS_GROUP,
			pcb->pt.X, pcb->pt.Y, pcb->sz.Width, pcb->sz.Height, hWnd, (HMENU)(pcb->id), hInst, NULL));
		pcb = pPanelBase->pmainobjs->cb_opt_set;
		pcb->set_wnd(CreateWindowW(TEXT("BUTTON"), pcb->txt.c_str(), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_PUSHLIKE | BS_MULTILINE,
			pcb->pt.X, pcb->pt.Y, pcb->sz.Width, pcb->sz.Height, hWnd, (HMENU)(pcb->id), hInst, NULL));
		pcb = pPanelBase->pmainobjs->cb_opt_com;
		pcb->set_wnd(CreateWindowW(TEXT("BUTTON"), pcb->txt.c_str(), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_PUSHLIKE | BS_MULTILINE,
			pcb->pt.X, pcb->pt.Y, pcb->sz.Width, pcb->sz.Height, hWnd, (HMENU)(pcb->id), hInst, NULL));
		pcb = pPanelBase->pmainobjs->cb_opt_cam;
		pcb->set_wnd(CreateWindowW(TEXT("BUTTON"), pcb->txt.c_str(), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_PUSHLIKE | BS_MULTILINE,
			pcb->pt.X, pcb->pt.Y, pcb->sz.Width, pcb->sz.Height, hWnd, (HMENU)(pcb->id), hInst, NULL));
		pcb = pPanelBase->pmainobjs->cb_opt_stat;
		pcb->set_wnd(CreateWindowW(TEXT("BUTTON"), pcb->txt.c_str(), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_PUSHLIKE | BS_MULTILINE,
			pcb->pt.X, pcb->pt.Y, pcb->sz.Width, pcb->sz.Height, hWnd, (HMENU)(pcb->id), hInst, NULL));
		pcb = pPanelBase->pmainobjs->cb_opt_clr;
		pcb->set_wnd(CreateWindowW(TEXT("BUTTON"), pcb->txt.c_str(), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_PUSHLIKE | BS_MULTILINE,
			pcb->pt.X, pcb->pt.Y, pcb->sz.Width, pcb->sz.Height, hWnd, (HMENU)(pcb->id), hInst, NULL));

		//String 初回書き込みフラグをセットしてTIMERイベントで書き込み
		is_initial_draw_main = true;

		//Lamp ウィンドウハンドルセット,フリッカ設定
		//CCとの通信状態表示
		INT32 id_list[2] = { 4,0 };//2種フリッカ　緑/暗青
		pPanelBase->pmainobjs->lmp_pcr->set_wnd(hWnd);	pPanelBase->pmainobjs->lmp_pcr->setup_flick(2, 3, id_list);
		pPanelBase->pmainobjs->lmp_pcs->set_wnd(hWnd);	pPanelBase->pmainobjs->lmp_pcs->setup_flick(2, 3, id_list);
		pPanelBase->pmainobjs->lmp_plcr->set_wnd(hWnd);	pPanelBase->pmainobjs->lmp_plcr->setup_flick(2, 3, id_list);
		pPanelBase->pmainobjs->lmp_plcs->set_wnd(hWnd);	pPanelBase->pmainobjs->lmp_plcs->setup_flick(2, 3, id_list);

		//表示更新用タイマー
		SetTimer(hWnd, ID_MAIN_PANEL_TIMER, ID_MAIN_PANEL_TIMER_MS, NULL);
		break;
	}
	case WM_COMMAND: {
		int wmId = LOWORD(wp);
		// 選択されたメニューの解析:
		switch (wmId)
		{
		case ID_MAIN_PNL_OBJ_CB_ESTOP: {
			if (pPanelBase->pmainobjs->cb_estop->get() == BST_CHECKED) {
				pPanelBase->pmainobjs->cb_estop->set(BST_UNCHECKED);
				pPanelBase->pmainobjs->lmp_estop->set(L_OFF);
			}
			else {
				pPanelBase->pmainobjs->cb_estop->set(BST_CHECKED);
				pPanelBase->pmainobjs->lmp_estop->set(L_ON);
			}
		}break;
		case ID_MAIN_PNL_OBJ_PB_SYUKAN_ON: {
			pPanelBase->pmainobjs->pb_syukan_on->update(true);
		}break;
		case ID_MAIN_PNL_OBJ_PB_SYUKAN_OFF: {
			pPanelBase->pmainobjs->pb_syukan_off->update(true);
		}break;
		case ID_MAIN_PNL_OBJ_PB_REMOTE: {
			pPanelBase->pmainobjs->pb_remote->update(true);
		}break;
		case ID_MAIN_PNL_OBJ_PB_PAD_MODE: {

			pPanelBase->pmainobjs->pb_pad_mode->update(true);
		}break;
		case ID_MAIN_PNL_OBJ_PB_ASSIST_FUNC: {
			pPanelBase->pmainobjs->pb_assist_func->update(true);
		}break;
		case ID_MAIN_PNL_OBJ_PB_OTE_TYPE_WND: {
			pPanelBase->pmainobjs->pb_ote_type_wnd->update(true);
		}break;
		case ID_MAIN_PNL_OBJ_PB_CRANE_SEL_WND: {
			pPanelBase->pmainobjs->pb_crane_sel_wnd->update(true);
		}break;
		case ID_MAIN_PNL_OBJ_PB_AUTH: {
			pPanelBase->pmainobjs->pb_auth->update(true);
		}break;
		case ID_MAIN_PNL_OBJ_CB_PNL_NOTCH: {
			pPanelBase->pmainobjs->cb_pnl_notch->update();
		}break;

		case ID_MAIN_PNL_OBJ_RDO_DISP_MODE1:
		case ID_MAIN_PNL_OBJ_RDO_DISP_MODE2:
		{
			pPanelBase->pmainobjs->rdo_disp_mode->update(true);
		}break;

		case ID_MAIN_PNL_OBJ_RDO_OPT_WND_FLT:
		case ID_MAIN_PNL_OBJ_RDO_OPT_WND_SET:
		case ID_MAIN_PNL_OBJ_RDO_OPT_WND_COM:
		case ID_MAIN_PNL_OBJ_RDO_OPT_WND_CAM:
		case ID_MAIN_PNL_OBJ_RDO_OPT_WND_STAT:
		{
			HINSTANCE hInst = (HINSTANCE)GetModuleHandle(0);

			if (pSubPanelWnd == NULL) {
				pSubPanelWnd = new CSubPanelWindow(hInst, hWnd, crane_id, wmId, pPanelBase);
			}
			else {
				if (pSubPanelWnd->wnd_code == wmId) {
					delete pSubPanelWnd;
					pSubPanelWnd = NULL;
				}
				else {
					delete pSubPanelWnd;
					pSubPanelWnd = new CSubPanelWindow(hInst, hWnd, crane_id, wmId, pPanelBase);
				}
			}
			InvalidateRect(hWnd, NULL, TRUE); // ウィンドウ全体を再描画

		}break;
		case ID_MAIN_PNL_OBJ_RDO_OPT_WND_CLR:
		{
			delete pSubPanelWnd;
			pSubPanelWnd = NULL;
			InvalidateRect(hWnd, NULL, TRUE); // ウィンドウ全体を再描画
		}break;

		case ID_MAIN_PNL_OBJ_PB_FRESET: {
			pPanelBase->pmainobjs->pb_freset->update(true);
		}break;

		default:
			return DefWindowProc(hWnd, msg, wp, lp);
		}
	}break;

	case WM_LBUTTONUP: {//マウス左ボタン押下でモニタウィンドウ描画更新
		InvalidateRect(hWnd, NULL, TRUE); // ウィンドウ全体を再描画
	}
	case WM_CTLCOLORSTATIC: {//スタティックテキストの色セット
		SetTextColor((HDC)wp, RGB(220, 220, 220)); // ライトグレー
		SetBkMode((HDC)wp, TRANSPARENT);
	}return (LRESULT)GetStockObject(NULL_BRUSH); // 背景色に合わせる

	case WM_ERASEBKGND: {//ウィンドウの背景色をグレーに
		pPanelBase->pmainobjs->pgraphic->FillRectangle(pPanelBase->pmainobjs->pBrushBk, pPanelBase->pmainobjs->rc_panel);

	}return 1; // 背景を処理したことを示す

	case WM_TIMER: {
		//# LAMP(CTRL)更新
		//e-stop : PLCの認識がESTOPの時枠有表示
		INT16 code = pCcIf->st_msg_pc_u_rcv.body.st.lamp[OTE_PNL_CTRLS::estop].code;//制御PC受信バッファの指令内容
		int val = pPanelBase->pmainobjs->cb_estop->get();									//CBの状態
		//CB状態で制御PCのON表示があれば表示画像を切替
		if (val == BST_CHECKED) { if (code) val = 3; }
		else { if (code)val = 2; }
		pPanelBase->pmainobjs->lmp_estop->set(val);
		pPanelBase->pmainobjs->lmp_estop->update();

		//主幹
		code = pCcIf->st_msg_pc_u_rcv.body.st.lamp[OTE_PNL_CTRLS::syukan_on].code;
		pPanelBase->pmainobjs->lmp_syukan_on->set(code); pPanelBase->pmainobjs->lmp_syukan_on->update();
		code = pCcIf->st_msg_pc_u_rcv.body.st.lamp[OTE_PNL_CTRLS::syukan_off].code;
		pPanelBase->pmainobjs->lmp_syukan_off->set(code); pPanelBase->pmainobjs->lmp_syukan_off->update();

		//遠隔
		pPanelBase->pmainobjs->lmp_remote->set(pCsInf->st_body.remote);
		pPanelBase->pmainobjs->lmp_remote->update();

		pPanelBase->pmainobjs->lmp_pad_mode->set(pCsInf->st_body.game_pad_mode);
		pPanelBase->pmainobjs->lmp_pad_mode->update();

		//故障リセット
		if (pUi->ctrl_stat[OTE_PNL_CTRLS::fault_reset])
			pPanelBase->pmainobjs->lmp_freset->set(L_ON);
		else
			pPanelBase->pmainobjs->lmp_freset->set(L_OFF);

		pPanelBase->pmainobjs->lmp_freset->update();

		//# SwitchImg更新(ランプ）
		//CCとの通信状態表示(受信）
		if (pCcIf->cc_com_stat_r == ID_PNL_SOCK_STAT_ACT_RCV)
			pPanelBase->pmainobjs->lmp_pcr->set(ID_PANEL_LAMP_FLICK);
		else pPanelBase->pmainobjs->lmp_pcr->set(pCcIf->cc_com_stat_r);
		pPanelBase->pmainobjs->lmp_pcr->update();
		//CCとの通信状態表示(送信）
		if (pCcIf->cc_com_stat_s == ID_PNL_SOCK_STAT_ACT_SND)
			pPanelBase->pmainobjs->lmp_pcs->set(ID_PANEL_LAMP_FLICK);
		else pPanelBase->pmainobjs->lmp_pcs->set(pCcIf->cc_com_stat_s);
		pPanelBase->pmainobjs->lmp_pcs->update();
		//PLCとの通信状態表示(受信）
		pPanelBase->pmainobjs->lmp_plcr->update();
		//PLCとの通信状態表示(送信）
		pPanelBase->pmainobjs->lmp_plcs->update();

		//PB状態更新(オフディレイカウントダウン)
		pPanelBase->pmainobjs->pb_syukan_on->update(false);
		pPanelBase->pmainobjs->pb_syukan_off->update(false);
		pPanelBase->pmainobjs->pb_remote->update(false);
		pPanelBase->pmainobjs->pb_auth->update(false);
		pPanelBase->pmainobjs->pb_assist_func->update(false);
		pPanelBase->pmainobjs->pb_crane_sel_wnd->update(false);
		pPanelBase->pmainobjs->pb_ote_type_wnd->update(false);
		pPanelBase->pmainobjs->pb_pad_mode->update(false);
		pPanelBase->pmainobjs->pb_freset->update(false);

		//String更新
		if (is_initial_draw_main) {
			pPanelBase->pmainobjs->str_message->update();
			pPanelBase->pmainobjs->str_pc_com_stat->update();
			pPanelBase->pmainobjs->str_plc_com_stat->update();
			//	is_initial_draw_mon1 = false;
		}

		//GamePadチェック
		if (pCsInf->gpad_in.syukan_on) pPanelBase->pmainobjs->pb_syukan_on->update(true);
		if (pCsInf->gpad_in.syukan_off)pPanelBase->pmainobjs->pb_syukan_off->update(true);
		if (pCsInf->gpad_in.remote)pPanelBase->pmainobjs->pb_remote->update(true);
		if (pCsInf->gpad_in.estop)pPanelBase->pmainobjs->cb_estop->set(BST_CHECKED);
		if (pCsInf->gpad_in.f_reset) {//ゲームパッドの非常停止クリアはResetPBで実行
			pPanelBase->pmainobjs->pb_freset->update(true);
			pPanelBase->pmainobjs->cb_estop->set(BST_UNCHECKED);
		}

	}break;

	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);

		//PatBlt(hdc, 0, 0, OTE_SCAD_MON1_WND_W, OTE_SCAD_MON1_WND_H, RGB(64, 64, 64));

		EndPaint(hWnd, &ps);
	}break;
	case WM_DRAWITEM: {//ランプ表示を更新 TIMERイベントで状態変化チェックしてInvalidiateRectで呼び出し
		DRAWITEMSTRUCT* pDIS = (DRAWITEMSTRUCT*)lp;
		Image* image;
		Gdiplus::Graphics gra(pDIS->hDC);
		Font* pfont = NULL;
		CMainPanelObj* pos = pPanelBase->pmainobjs;
		CLampCtrl* plamp = NULL;

		if (pDIS->CtlID == pos->cb_estop->id) {
			plamp = pos->lmp_estop;	pfont = NULL;
		}
		else if (pDIS->CtlID == pos->pb_remote->id) {//リモートランプ
			plamp = pos->lmp_remote; pfont = plamp->pFont;
		}
		else if (pDIS->CtlID == pos->pb_syukan_on->id) {//主幹入ランプ
			plamp = pos->lmp_syukan_on; pfont = plamp->pFont;
		}
		else if (pDIS->CtlID == pos->pb_syukan_off->id) {//主幹切ランプ
			plamp = pos->lmp_syukan_off; pfont = plamp->pFont;
		}
		else if (pDIS->CtlID == pos->pb_pad_mode->id) {//PADランプ
			plamp = pos->lmp_pad_mode; pfont = plamp->pFont;
		}
		else if (pDIS->CtlID == pos->pb_freset->id) {//FAULTランプ
			plamp = pos->lmp_freset; pfont = plamp->pFont;
		}
		else return false;

		image = plamp->pimg[plamp->get()];
		gra.FillRectangle(pPanelBase->pmainobjs->pBrushBk, plamp->rc);											//背景色セット
		if (image) gra.DrawImage(image, plamp->rc);															//イメージ描画
		if (pfont != NULL)
			gra.DrawString(plamp->txt.c_str(), -1, pfont, plamp->frc, plamp->pStrFormat, plamp->pTxtBrush);	//テキスト描画

	}return true;

	case WM_DESTROY: {
		hWnd = NULL;
		KillTimer(hWnd, ID_MAIN_PANEL_TIMER);
//		if (pPanelBase != NULL) delete pPanelBase;
		//### オープニング画面を再表示
		pEnvObj->open_opening_window();

	}break;
	default:
		return DefWindowProc(hWnd, msg, wp, lp);
	}
	return S_OK;
};

HWND CSubPanelWindow::hPnlWnd;
HWND CSubPanelWindow::hParentWnd; 
CPanelBase* CSubPanelWindow::pPanelBase;
int CSubPanelWindow::crane_id;
int CSubPanelWindow::wnd_code;
int CSubPanelWindow::flt_disp_code = 0;
PINT16 CSubPanelWindow::pflt_plc;
LPST_OTE_UI CSubPanelWindow::pUi;
LPST_OTE_CS_INF CSubPanelWindow::pCsInf;
LPST_OTE_CC_IF CSubPanelWindow::pCcIf;
LPST_OTE_ENV_INF CSubPanelWindow::pOteEnvInf; 

CSubPanelWindow::CSubPanelWindow(HINSTANCE hInstance, HWND hParent, int _crane_id, int _wnd_code, CPanelBase* _pPanelBase) {
	pPanelBase = _pPanelBase;
	hParentWnd = hParent;
	wnd_code = _wnd_code; //ウィンドウコードをセット

    const wchar_t CLASS_NAME[] = L"SubWindowClass";
    const wchar_t CLASS_NAME_FLT[] = L"SubWindowFltClass";
    const wchar_t CLASS_NAME_SET[] = L"SubWindowSetClass";
    const wchar_t CLASS_NAME_COM[] = L"SubWindowComClass";
    const wchar_t CLASS_NAME_CAM[] = L"SubWindowCamClass";
    const wchar_t CLASS_NAME_STAT[] = L"SubWindowStatClass";

    const wchar_t* pClassName;

	WNDCLASS wc = { };
	switch (wnd_code) {
	case ID_MAIN_PNL_OBJ_RDO_OPT_WND_FLT:
        wc.lpfnWndProc = WndProcFlt;
        wc.hInstance = hInstance;
        wc.lpszClassName = pClassName = CLASS_NAME_FLT;
		break;
	case ID_MAIN_PNL_OBJ_RDO_OPT_WND_SET:
        wc.lpfnWndProc = WndProcSet;
        wc.hInstance = hInstance;
        wc.lpszClassName = pClassName = CLASS_NAME_SET;
		break;
	case ID_MAIN_PNL_OBJ_RDO_OPT_WND_COM:
        wc.lpfnWndProc = WndProcCom;
        wc.hInstance = hInstance;
        wc.lpszClassName = pClassName = CLASS_NAME_COM;
		break;
	case ID_MAIN_PNL_OBJ_RDO_OPT_WND_CAM:
        wc.lpfnWndProc = WndProcCam;
        wc.hInstance = hInstance;
        wc.lpszClassName = pClassName = CLASS_NAME_CAM;
		break;
	case ID_MAIN_PNL_OBJ_RDO_OPT_WND_STAT:
        wc.lpfnWndProc = WndProcStat;
        wc.hInstance = hInstance;
        wc.lpszClassName = pClassName = CLASS_NAME_STAT;
		break;
	default:
        wc.lpfnWndProc = WndProc;
        wc.hInstance = hInstance;
        wc.lpszClassName = pClassName = CLASS_NAME;
		break;
	}
    RegisterClass(&wc);

    hPnlWnd = CreateWindowEx(
        0,																// Optional window styles
        pClassName,														// Window class
        L"サブウィンドウ",												// Window text
        WS_CHILD | WS_BORDER | WS_THICKFRAME | WS_CAPTION,               // Window style
        SUB_PNL_WND_X, SUB_PNL_WND_Y, SUB_PNL_WND_W, SUB_PNL_WND_H,
        hParent, nullptr, hInstance, nullptr
    );

    if (hPnlWnd) {
		pPanelBase->set_panel_id(wnd_code);//パネルコードセット
        ShowWindow(hPnlWnd, SW_SHOW);
        UpdateWindow(hPnlWnd);
    }
}
CSubPanelWindow::~CSubPanelWindow()
{
    close();
}
int CSubPanelWindow::close()
{
	DestroyWindow(hPnlWnd); 
	return 0;
}
void CSubPanelWindow::set_up(LPST_OTE_UI _pUi, LPST_OTE_CS_INF _pCsInf, LPST_OTE_CC_IF _pCcIf, LPST_OTE_ENV_INF _pOteEnvInf, int _crane_id) {
	pUi = _pUi;
	pCsInf = _pCsInf;
	pCcIf = _pCcIf;
	pOteEnvInf = _pOteEnvInf;
	crane_id = _crane_id; //クレーンIDをセット
	//### Environmentクラスインスタンスのポインタ取得
	pEnvObj = (COteEnv*)VectCtrlObj[st_task_id.ENV];

	//PLCの故障情報ポインタセット
	switch (crane_id) {
	case CARNE_ID_HHGH29:
	default:
	{
		LPST_PLC_RBUF_HHGH29 prbuf = (LPST_PLC_RBUF_HHGH29)pCcIf->st_msg_pc_u_rcv.body.st.buf_io_read;
		pflt_plc = (PINT16)prbuf->plc_fault; //PLCの故障情報ポインタセット
	}
		break;
	}

	return;
};

LRESULT CALLBACK CSubPanelWindow::WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CREATE: {
        SetWindowText(hwnd, L"DefaultWnd");
    }break;

    case WM_DESTROY:
        // PostQuitMessage(0);
        return 0;
    case WM_CLOSE:
        DestroyWindow(hwnd);
        return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}


static bool is_flt_bk_update_required, is_flt_mask_update_required;
static int  code_flt_bk = 0, cnt_disp_update_required = 0;

LRESULT CALLBACK CSubPanelWindow::WndProcFlt(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_CREATE: {

		InitCommonControls();//コモンコントロール初期化
		HINSTANCE hInst = (HINSTANCE)GetModuleHandle(0);
		//グラフィックオブジェクトの初期化
		pPanelBase->psubobjs->setup_graphics(hwnd);
		pPanelBase->psubobjs->refresh_obj_graphics();
			
		pPanelBase->psubobjs->colorkey.SetValue(Color::Black);//黒を透過
		Status status = pPanelBase->psubobjs->attr.SetColorKey(
			pPanelBase->psubobjs->colorkey,
			pPanelBase->psubobjs->colorkey,
			ColorAdjustTypeDefault // DefaultではなくBitmapを指定する方が明確
		);

		SetWindowText(hwnd, L"故障情報");
		pPanelBase->psubobjs->n_disp_page = 2; //ページ数
		pPanelBase->psubobjs->n_disp_page = 0; //ページ番号

		
		//表示更新用タイマー
		SetTimer(hwnd, ID_SUB_PANEL_TIMER, ID_SUB_PANEL_TIMER_MS, NULL);

		//ウィンドウにコントロール追加
		//PB 
		 CPbCtrl* ppb = pPanelBase->psubobjs->pb_disp_flt_plcmap;
		ppb->set_wnd(CreateWindowW(TEXT("BUTTON"), ppb->txt.c_str(), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_PUSHLIKE,
			ppb->pt.X, ppb->pt.Y, ppb->sz.Width, ppb->sz.Height, hwnd, (HMENU)(ppb->id), hInst, NULL));
		//CPbCtrl* ppb = pPanelBase->psubobjs->pb_flt_next;
		//ppb->set_wnd(CreateWindowW(TEXT("BUTTON"), ppb->txt.c_str(), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_PUSHLIKE,
		//	ppb->pt.X, ppb->pt.Y, ppb->sz.Width, ppb->sz.Height, hwnd, (HMENU)(ppb->id), hInst, NULL));
		//ppb = pPanelBase->psubobjs->pb_flt_back;
		//ppb->set_wnd(CreateWindowW(TEXT("BUTTON"), ppb->txt.c_str(), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_PUSHLIKE,
		//	ppb->pt.X, ppb->pt.Y, ppb->sz.Width, ppb->sz.Height, hwnd, (HMENU)(ppb->id), hInst, NULL));

		//CB
		CCbCtrl* pcb = pPanelBase->psubobjs->cb_disp_flt_heavy1;
		pcb->set_wnd(CreateWindowW(TEXT("BUTTON"), pcb->txt.c_str(), WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX | BS_PUSHLIKE | BS_MULTILINE,
			pcb->pt.X, pcb->pt.Y, pcb->sz.Width, pcb->sz.Height, hwnd, (HMENU)(pcb->id), hInst, NULL));
		pcb = pPanelBase->psubobjs->cb_disp_flt_light;
		pcb->set_wnd(CreateWindowW(TEXT("BUTTON"), pcb->txt.c_str(), WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX | BS_PUSHLIKE | BS_MULTILINE,
			pcb->pt.X, pcb->pt.Y, pcb->sz.Width, pcb->sz.Height, hwnd, (HMENU)(pcb->id), hInst, NULL));
		pcb = pPanelBase->psubobjs->cb_disp_interlock;
		pcb->set_wnd(CreateWindowW(TEXT("BUTTON"), pcb->txt.c_str(), WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX | BS_PUSHLIKE | BS_MULTILINE,
			pcb->pt.X, pcb->pt.Y, pcb->sz.Width, pcb->sz.Height, hwnd, (HMENU)(pcb->id), hInst, NULL));
		pcb = pPanelBase->psubobjs->cb_flt_bypass;
		pcb->set_wnd(CreateWindowW(TEXT("BUTTON"), pcb->txt.c_str(), WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX | BS_PUSHLIKE | BS_MULTILINE,
			pcb->pt.X, pcb->pt.Y, pcb->sz.Width, pcb->sz.Height, hwnd, (HMENU)(pcb->id), hInst, NULL));
		pcb = pPanelBase->psubobjs->cb_disp_flt_heavy2;
		pcb->set_wnd(CreateWindowW(TEXT("BUTTON"), pcb->txt.c_str(), WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX | BS_PUSHLIKE | BS_MULTILINE,
			pcb->pt.X, pcb->pt.Y, pcb->sz.Width, pcb->sz.Height, hwnd, (HMENU)(pcb->id), hInst, NULL));
		pcb = pPanelBase->psubobjs->cb_disp_flt_heavy3;
		pcb->set_wnd(CreateWindowW(TEXT("BUTTON"), pcb->txt.c_str(), WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX | BS_PUSHLIKE | BS_MULTILINE,
			pcb->pt.X, pcb->pt.Y, pcb->sz.Width, pcb->sz.Height, hwnd, (HMENU)(pcb->id), hInst, NULL));
		
		//背景
		//Switch Image Windowハンドルセット（パネルウィンドウ）
		pPanelBase->psubobjs->img_flt_bk->set_wnd(hwnd);
		pPanelBase->psubobjs->img_flt_bk->set(0);
		pPanelBase->psubobjs->img_flt_bk->update();

		is_flt_bk_update_required = true;//背景再描画フラグON
		is_flt_mask_update_required = true; //マスク更新フラグOFF
		code_flt_bk = 0;				 //背景色初期値0

		//初期値セット
		SendMessage(pPanelBase->psubobjs->cb_disp_flt_heavy1->hWnd, BM_SETCHECK, BST_CHECKED, 0);
		SendMessage(pPanelBase->psubobjs->cb_disp_flt_heavy2->hWnd, BM_SETCHECK, BST_CHECKED, 0);
		SendMessage(pPanelBase->psubobjs->cb_disp_flt_heavy3->hWnd, BM_SETCHECK, BST_CHECKED, 0);
		SendMessage(pPanelBase->psubobjs->cb_disp_flt_light->hWnd, BM_SETCHECK, BST_CHECKED, 0);
		SendMessage(pPanelBase->psubobjs->cb_disp_interlock->hWnd, BM_SETCHECK, BST_CHECKED, 0);

		flt_disp_code |= FAULT_HEAVY1 | FAULT_HEAVY2 | FAULT_HEAVY3 | FAULT_LIGHT | FAULT_INTERLOCK;

		//バイパスは初期値OFF
		SendMessage(pPanelBase->psubobjs->cb_flt_bypass->hWnd, BM_SETCHECK, BST_UNCHECKED, 0); 
			
	}break;

	case WM_LBUTTONUP: {//マウス左ボタン押下でモニタウィンドウ描画更新
		InvalidateRect(hwnd, NULL, TRUE); // ウィンドウ全体を再描画
	}
	case WM_CTLCOLORSTATIC: {//スタティックテキストの色セット
		SetTextColor((HDC)wParam, RGB(220, 220, 220)); // ライトグレー
		SetBkMode((HDC)wParam, TRANSPARENT);
	}return (LRESULT)GetStockObject(NULL_BRUSH); // 背景色に合わせる

	case WM_ERASEBKGND: {//ウィンドウの背景色をグレーに
		pPanelBase->psubobjs->pgraphic->FillRectangle(pPanelBase->psubobjs->pBrushBk, pPanelBase->psubobjs->rc_panel);
	}return 1; // 背景を処理したことを示す

	case WM_TIMER: {
		cnt_disp_update_required++; //更新カウンタ
		
		//故障発生/クリアチェック
		int chk_result = pCrane->pFlt->chk_flt_trig();
		if (chk_result)cnt_disp_update_required = 0;//変化があったら表示を更新

		if ((cnt_disp_update_required % 5)&&(is_flt_bk_update_required == false))break; //5回に1回更新

		pCrane->pFlt->set_disp_buf(flt_disp_code);

		int flt_code,iflt;
		for (int i = 0; i < N_PLC_FAULT_BUF;i++) {
			INT16 mask = 1;
			for (int j = 0; j < 16; j++) {
				mask = mask << j;
				if (pCrane->pFlt->faults_disp[i] & mask) {
					iflt = 16 * i + j;
					LPWCH pwch = pCrane->pFlt->flt_list.faults[iflt].item;
					flt_code = iflt + 301;
				}
			}
		}

		//マスク更新
		//背景更新
		{
			is_flt_bk_update_required = false;
			if (flt_disp_code & FAULT_HEAVY1) {
				if (code_flt_bk != 3) is_flt_bk_update_required = true;
				pPanelBase->psubobjs->img_flt_bk->set(code_flt_bk = 3);
			}
			else if (flt_disp_code & FAULT_HEAVY2) {
				if (code_flt_bk != 3) is_flt_bk_update_required = true;
				pPanelBase->psubobjs->img_flt_bk->set(code_flt_bk = 3);
			}
			else if (flt_disp_code & FAULT_HEAVY3) {
				if (code_flt_bk != 3) is_flt_bk_update_required = true;
				pPanelBase->psubobjs->img_flt_bk->set(code_flt_bk = 3);
			}
			else if (flt_disp_code & FAULT_LIGHT) {
				if (code_flt_bk != 2) is_flt_bk_update_required = true;
				pPanelBase->psubobjs->img_flt_bk->set(code_flt_bk = 2);
			}
			else if (flt_disp_code & FAULT_INTERLOCK) {
				if (code_flt_bk != 1) is_flt_bk_update_required = true;
				pPanelBase->psubobjs->img_flt_bk->set(code_flt_bk = 1);
			}
			else {
				if (code_flt_bk != 0) is_flt_bk_update_required = true;
				pPanelBase->psubobjs->img_flt_bk->set(code_flt_bk = 0);
			}

			if (is_flt_bk_update_required)pPanelBase->psubobjs->img_flt_bk->update();
		}

	}break;

	case WM_COMMAND: {
		INT16 code = 0;
		int wmId = LOWORD(wParam);
		// 選択されたメニューの解析:
		switch (wmId)
		{
		case ID_SUB_PNL_FLT_OBJ_PB_NEXT: {
			pPanelBase->psubobjs->i_disp_page++;
			if (pPanelBase->psubobjs->i_disp_page >= pPanelBase->psubobjs->n_disp_page)
				pPanelBase->psubobjs->i_disp_page = 0; //ページ番号をリセット
		}break;
		case ID_SUB_PNL_FLT_OBJ_PB_BACK: {
			pPanelBase->psubobjs->i_disp_page--;
			if (pPanelBase->psubobjs->i_disp_page < 0)
				pPanelBase->psubobjs->i_disp_page = pPanelBase->psubobjs->n_disp_page - 1; //ページ番号をリセット
		}break;

		case ID_SUB_PNL_FLT_OBJ_CB_HEAVY1: {
			flt_disp_code &= ~FAULT_HEAVY1;
			if (BST_CHECKED == SendMessage(pPanelBase->psubobjs->cb_disp_flt_heavy1->hWnd, BM_GETCHECK, 0, 0)) {
				is_flt_bk_update_required =true;
				flt_disp_code |= FAULT_HEAVY1;
			}
		}break;
		case ID_SUB_PNL_FLT_OBJ_CB_HEAVY2: {
			flt_disp_code &= ~FAULT_HEAVY2;
			if (BST_CHECKED == SendMessage(pPanelBase->psubobjs->cb_disp_flt_heavy2->hWnd, BM_GETCHECK, 0, 0)) {
				is_flt_bk_update_required = true;
				flt_disp_code |= FAULT_HEAVY2;
			}
		}break;
		case ID_SUB_PNL_FLT_OBJ_CB_HEAVY3:	{
			flt_disp_code &= ~FAULT_HEAVY3;
			if (BST_CHECKED == SendMessage(pPanelBase->psubobjs->cb_disp_flt_heavy3->hWnd, BM_GETCHECK, 0, 0)) {
				is_flt_bk_update_required = true;
				flt_disp_code |= FAULT_HEAVY3;
			}
		}break;
		case ID_SUB_PNL_FLT_OBJ_CB_LITE		:{
			flt_disp_code &= ~FAULT_LIGHT;
			if (BST_CHECKED == SendMessage(pPanelBase->psubobjs->cb_disp_flt_light->hWnd, BM_GETCHECK, 0, 0)) {
				is_flt_bk_update_required = true;
				flt_disp_code |= FAULT_LIGHT;
			}
		}break;
		case ID_SUB_PNL_FLT_OBJ_CB_IL		:{
			flt_disp_code &= ~FAULT_INTERLOCK;
			if (BST_CHECKED == SendMessage(pPanelBase->psubobjs->cb_disp_interlock->hWnd, BM_GETCHECK, 0, 0)) {
				is_flt_bk_update_required = true;
				flt_disp_code |= FAULT_INTERLOCK;
			}
		}break;
		case ID_SUB_PNL_FLT_OBJ_CB_BYPASS	:{
		}break;

		case ID_SUB_PNL_FLT_OBJ_PB_PLCMAP: {//メッセージボックスでPLCの故障情報を表示
			wostringstream wos;
			wos << hex ;
			for (int i = 0; i < N_PLC_FAULT_BUF; i++) {
				wos  << std::setw(4) << std::setfill(L'0')<< pflt_plc[i] << L"\n";
			}
			MessageBox(hwnd, wos.str().c_str(), L"PLC FAULT MAP", MB_OK | MB_ICONINFORMATION);
		}break;


		default:
			return DefWindowProc(hPnlWnd, uMsg, wParam, lParam);
		}
	}break;

	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);

		Gdiplus::Bitmap* pbmp_bk = Gdiplus::Bitmap::FromHBITMAP(pPanelBase->psubobjs->hBmp_bk, NULL);
		Gdiplus::Bitmap* pbmp_inf = Gdiplus::Bitmap::FromHBITMAP(pPanelBase->psubobjs->hBmp_inf, NULL);
		Rect destRect(0, 0, SUB_PNL_WND_W, SUB_PNL_WND_H);

		pPanelBase->psubobjs->pgraphic->DrawImage(pbmp_bk, destRect, 0,0,SUB_PNL_WND_W, SUB_PNL_WND_H,UnitPixel);

		PatBlt(pPanelBase->psubobjs->hdc_inf, 0, 0, SUB_PNL_WND_W, SUB_PNL_WND_H, BLACKNESS);

		pPanelBase->psubobjs->str_flt_message->update(); //故障メッセージ更新


		Status drawStatus = pPanelBase->psubobjs->pgraphic->DrawImage(pbmp_inf, destRect, 0, 0, SUB_PNL_WND_W, SUB_PNL_WND_H, UnitPixel, &pPanelBase->psubobjs->attr);
		if (drawStatus != Ok){
			drawStatus = drawStatus;
		}

		EndPaint(hwnd, &ps);
	}break;
	case WM_DRAWITEM: {//ランプ表示を更新 TIMERイベントで状態変化チェックしてInvalidiateRectで呼び出し
		DRAWITEMSTRUCT* pDIS = (DRAWITEMSTRUCT*)lParam;

		Gdiplus::Graphics gra(pDIS->hDC);
		Font* pfont = NULL;
		CSubPanelObj* pos = pPanelBase->psubobjs;
		CLampCtrl* plamp = NULL;

	}return true;
	case WM_DESTROY:
		//表示更新用タイマー
		KillTimer(hPnlWnd, ID_SUB_PANEL_TIMER);
		// PostQuitMessage(0);
		return 0;
	case WM_CLOSE:
		DestroyWindow(hwnd);
		return 0;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
LRESULT CALLBACK CSubPanelWindow::WndProcSet(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_CREATE: {
		InitCommonControls();//コモンコントロール初期化
		HINSTANCE hInst = (HINSTANCE)GetModuleHandle(0);

		pPanelBase->psubobjs->setup_graphics(hwnd);
		pPanelBase->psubobjs->refresh_obj_graphics();

		SetWindowText(hwnd, L"設定");
		//表示更新用タイマー
		SetTimer(hwnd, ID_SUB_PANEL_TIMER, ID_SUB_PANEL_TIMER_MS, NULL);

		//ウィンドウにコントロール追加
		//RADIO BUTTON
		CCbCtrl* pcb = pPanelBase->psubobjs->cb_mh_spd_mode0;
		pcb->set_wnd(CreateWindowW(TEXT("BUTTON"), pcb->txt.c_str(), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_PUSHLIKE | BS_MULTILINE | WS_GROUP,
			pcb->pt.X, pcb->pt.Y, pcb->sz.Width, pcb->sz.Height, hwnd, (HMENU)(pcb->id), hInst, NULL));
		pcb = pPanelBase->psubobjs->cb_mh_spd_mode1;
		pcb->set_wnd(CreateWindowW(TEXT("BUTTON"), pcb->txt.c_str(), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_PUSHLIKE | BS_MULTILINE,
			pcb->pt.X, pcb->pt.Y, pcb->sz.Width, pcb->sz.Height, hwnd, (HMENU)(pcb->id), hInst, NULL));
		pcb = pPanelBase->psubobjs->cb_mh_spd_mode2;
		pcb->set_wnd(CreateWindowW(TEXT("BUTTON"), pcb->txt.c_str(), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_PUSHLIKE | BS_MULTILINE,
			pcb->pt.X, pcb->pt.Y, pcb->sz.Width, pcb->sz.Height, hwnd, (HMENU)(pcb->id), hInst, NULL));

		pcb = pPanelBase->psubobjs->cb_bh_r_mode0;
		pcb->set_wnd(CreateWindowW(TEXT("BUTTON"), pcb->txt.c_str(), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_PUSHLIKE | BS_MULTILINE | WS_GROUP,
			pcb->pt.X, pcb->pt.Y, pcb->sz.Width, pcb->sz.Height, hwnd, (HMENU)(pcb->id), hInst, NULL));
		pcb = pPanelBase->psubobjs->cb_bh_r_mode1;
		pcb->set_wnd(CreateWindowW(TEXT("BUTTON"), pcb->txt.c_str(), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_PUSHLIKE | BS_MULTILINE,
			pcb->pt.X, pcb->pt.Y, pcb->sz.Width, pcb->sz.Height, hwnd, (HMENU)(pcb->id), hInst, NULL));
		pcb = pPanelBase->psubobjs->cb_bh_r_mode2;
		pcb->set_wnd(CreateWindowW(TEXT("BUTTON"), pcb->txt.c_str(), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_PUSHLIKE | BS_MULTILINE,
			pcb->pt.X, pcb->pt.Y, pcb->sz.Width, pcb->sz.Height, hwnd, (HMENU)(pcb->id), hInst, NULL));

		//Switch Image Windowハンドルセット（パネルウィンドウ）
		pPanelBase->psubobjs->lmp_mh_spd_mode->set_wnd(hwnd);
		pPanelBase->psubobjs->lmp_mh_spd_mode->set_wnd(hwnd);

		//初期値セット
		if (pCcIf->st_msg_pc_u_rcv.body.st.lamp[OTE_PNL_CTRLS::mh_spd_mode].st.com == CODE_MODE0) {
			SendMessage(pPanelBase->psubobjs->cb_mh_spd_mode0->hWnd, BM_SETCHECK, BST_CHECKED, 0);
		}
		else if (pCcIf->st_msg_pc_u_rcv.body.st.lamp[OTE_PNL_CTRLS::mh_spd_mode].st.com == CODE_MODE1) {
			SendMessage(pPanelBase->psubobjs->cb_mh_spd_mode1->hWnd, BM_SETCHECK, BST_CHECKED, 0);
		}
		else if (pCcIf->st_msg_pc_u_rcv.body.st.lamp[OTE_PNL_CTRLS::mh_spd_mode].st.com == CODE_MODE2) {
			SendMessage(pPanelBase->psubobjs->cb_mh_spd_mode2->hWnd, BM_SETCHECK, BST_CHECKED, 0);
		}
		else {
			SendMessage(pPanelBase->psubobjs->cb_mh_spd_mode0->hWnd, BM_SETCHECK, BST_CHECKED, 0);;
		}
		//起伏モード
		if (pCcIf->st_msg_pc_u_rcv.body.st.lamp[OTE_PNL_CTRLS::bh_r_mode].st.com == CODE_MODE0) {
			SendMessage(pPanelBase->psubobjs->cb_bh_r_mode0->hWnd, BM_SETCHECK, BST_CHECKED, 0);
		}
		else if (pCcIf->st_msg_pc_u_rcv.body.st.lamp[OTE_PNL_CTRLS::bh_r_mode].st.com == CODE_MODE1) {
			SendMessage(pPanelBase->psubobjs->cb_bh_r_mode1->hWnd, BM_SETCHECK, BST_CHECKED, 0);
		}
		else if (pCcIf->st_msg_pc_u_rcv.body.st.lamp[OTE_PNL_CTRLS::bh_r_mode].st.com == CODE_MODE2) {
			SendMessage(pPanelBase->psubobjs->cb_bh_r_mode2->hWnd, BM_SETCHECK, BST_CHECKED, 0);
		}
		else {
			SendMessage(pPanelBase->psubobjs->cb_bh_r_mode0->hWnd, BM_SETCHECK, BST_CHECKED, 0);;
		}

	}break;

	case WM_LBUTTONUP: {//マウス左ボタン押下でモニタウィンドウ描画更新
		InvalidateRect(hwnd, NULL, TRUE); // ウィンドウ全体を再描画
	}
	case WM_CTLCOLORSTATIC: {//スタティックテキストの色セット
		SetTextColor((HDC)wParam, RGB(220, 220, 220)); // ライトグレー
		SetBkMode((HDC)wParam, TRANSPARENT);
	}return (LRESULT)GetStockObject(NULL_BRUSH); // 背景色に合わせる

	case WM_ERASEBKGND: {//ウィンドウの背景色をグレーに
		pPanelBase->psubobjs->pgraphic->FillRectangle(pPanelBase->psubobjs->pBrushBk, pPanelBase->psubobjs->rc_panel);
	}return 1; // 背景を処理したことを示す

	case WM_TIMER: {
		//# Switching Image更新
		//巻速度モード
		INT16 code = (INT16)pCcIf->st_msg_pc_u_rcv.body.st.lamp[OTE_PNL_CTRLS::mh_spd_mode].st.com;
		pPanelBase->psubobjs->lmp_mh_spd_mode->set(code); 
		pPanelBase->psubobjs->lmp_mh_spd_mode->update();

		//起伏モード
		code = (INT16)pCcIf->st_msg_pc_u_rcv.body.st.lamp[OTE_PNL_CTRLS::bh_r_mode].st.com;
		pPanelBase->psubobjs->lmp_bh_r_mode->set(code);
		pPanelBase->psubobjs->lmp_bh_r_mode->update();

	}break;

	case WM_COMMAND: {
		INT16 code = 0;
		int wmId = LOWORD(wParam);
		// 選択されたメニューの解析:
		switch (wmId)
		{
		case ID_SUB_PNL_SET_OBJ_RDO_MHSPD_0: 
		case ID_SUB_PNL_SET_OBJ_RDO_MHSPD_1: 
		case ID_SUB_PNL_SET_OBJ_RDO_MHSPD_2: {
			code = pPanelBase->psubobjs->rdo_mh_spd_mode->update(true);
		}break;
		case ID_SUB_PNL_SET_OBJ_RDO_BHR_0: 
		case ID_SUB_PNL_SET_OBJ_RDO_BHR_1: 
		case ID_SUB_PNL_SET_OBJ_RDO_BHR_2: {
			code = pPanelBase->psubobjs->rdo_bh_r_mode->update(true);
			}break;

		default:
			return DefWindowProc(hPnlWnd, uMsg, wParam, lParam);
		}
	}break;

	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);
		EndPaint(hwnd, &ps);
	}break;
	case WM_DRAWITEM: {//ランプ表示を更新 TIMERイベントで状態変化チェックしてInvalidiateRectで呼び出し
		DRAWITEMSTRUCT* pDIS = (DRAWITEMSTRUCT*)lParam;

		Gdiplus::Graphics gra(pDIS->hDC);
		Font* pfont = NULL;
		CSubPanelObj* pos = pPanelBase->psubobjs;
		CLampCtrl* plamp = NULL;

		//if (pDIS->CtlID == pos->cb_bh_r_mode0->id) {
		//	plamp = pos->lmp_bh_r_mode;	pfont = plamp->pFont;
		//}
		//if (pDIS->CtlID == pos->cb_bh_r_mode1->id) {
		//	plamp = pos->lmp_bh_r_mode;	pfont = plamp->pFont;
		//}
		//if (pDIS->CtlID == pos->cb_bh_r_mode2->id) {
		//	plamp = pos->lmp_bh_r_mode;	pfont = plamp->pFont;
		//}
		//if (pDIS->CtlID == pos->cb_mh_spd_mode0->id) {
		//	plamp = pos->lmp_mh_spd_mode;	pfont = plamp->pFont;
		//}
		//if (pDIS->CtlID == pos->cb_mh_spd_mode0->id) {
		//	plamp = pos->lmp_mh_spd_mode;	pfont = plamp->pFont;
		//}
		//if (pDIS->CtlID == pos->cb_mh_spd_mode0->id) {
		//	plamp = pos->lmp_mh_spd_mode;	pfont = plamp->pFont;
		//}

		//image = plamp->pimg[plamp->get()];
		//gra.FillRectangle(pPanelBase->psubobjs->pBrushBk, plamp->rc);	//背景色セット
		//if (image) gra.DrawImage(image, plamp->rc);						//イメージ描画
		//if (pfont != NULL)
		//	gra.DrawString(plamp->txt.c_str(), -1, pfont, plamp->frc, plamp->pStrFormat, plamp->pTxtBrush);	//テキスト描画

	}return true;
	case WM_DESTROY:
		//表示更新用タイマー
		KillTimer(hPnlWnd, ID_SUB_PANEL_TIMER);
	//	pPanelBase->psubobjs->clear_graghics();
		// PostQuitMessage(0);
		return 0;
	case WM_CLOSE:
		DestroyWindow(hwnd);
		return 0;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);

}
LRESULT CALLBACK CSubPanelWindow::WndProcCom(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CREATE: {
        SetWindowText(hwnd, L"通信");
    }break;

    case WM_DESTROY:
        // PostQuitMessage(0);
        return 0;
    case WM_CLOSE:
        DestroyWindow(hwnd);
        return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
LRESULT CALLBACK CSubPanelWindow::WndProcCam(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CREATE: {
        SetWindowText(hwnd, L"カメラ");
    }break;

    case WM_DESTROY:
        // PostQuitMessage(0);
        return 0;
    case WM_CLOSE:
        DestroyWindow(hwnd);
        return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
LRESULT CALLBACK CSubPanelWindow::WndProcStat(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_CREATE: {

		InitCommonControls();//コモンコントロール初期化
		HINSTANCE hInst = (HINSTANCE)GetModuleHandle(0);

		//グラフィックオブジェクトの初期化
		pPanelBase->psubobjs->setup_graphics(hwnd);
		pPanelBase->psubobjs->refresh_obj_graphics();
		//重ね合わせ画像透過色設定
		pPanelBase->psubobjs->colorkey.SetValue(Color::Black);//黒を透過
		Status status = pPanelBase->psubobjs->attr.SetColorKey(
			pPanelBase->psubobjs->colorkey,
			pPanelBase->psubobjs->colorkey,
			ColorAdjustTypeDefault // DefaultではなくBitmapを指定する方が明確
		);

		SetWindowText(hwnd, L"状態監視");
		pPanelBase->psubobjs->n_disp_page = 2; //ページ数
		pPanelBase->psubobjs->n_disp_page = 0; //ページ番号


		//表示更新用タイマー
		SetTimer(hwnd, ID_SUB_PANEL_TIMER, ID_SUB_PANEL_TIMER_MS, NULL);

		//ウィンドウにコントロール追加
		//LABEL 
		CreateWindowW(TEXT("STATIC"), L"方向  目標速度 速度指令 速度FB ﾄﾙｸFB", WS_CHILD | WS_VISIBLE | SS_LEFT,
			60, 20, 355, 30, hwnd, (HMENU)(100), hInst, NULL);
		CreateWindowW(TEXT("STATIC"), L"巻上", WS_CHILD | WS_VISIBLE | SS_LEFT, 
			5, 70, 40, 30, hwnd, (HMENU)(100), hInst, NULL);				   
		CreateWindowW(TEXT("STATIC"), L"引込", WS_CHILD | WS_VISIBLE | SS_LEFT,
			5, 105, 40, 30, hwnd, (HMENU)(100), hInst, NULL);				   
		CreateWindowW(TEXT("STATIC"), L"旋回", WS_CHILD | WS_VISIBLE | SS_LEFT,
			5, 140, 40, 30, hwnd, (HMENU)(100), hInst, NULL);
		CreateWindowW(TEXT("STATIC"), L"走行", WS_CHILD | WS_VISIBLE | SS_LEFT,
			5, 175, 40, 30, hwnd, (HMENU)(100), hInst, NULL);

		//STATIC 
		CStaticCtrl* pst = pPanelBase->psubobjs->st_mh_notch_dir;
		pst->set_wnd(CreateWindowW(TEXT("STATIC"), pst->txt.c_str(), WS_CHILD | WS_VISIBLE | SS_RIGHT,
			pst->pt.X, pst->pt.Y, pst->sz.Width, pst->sz.Height, hwnd, (HMENU)(pst->id), hInst, NULL));
		pst = pPanelBase->psubobjs->st_bh_notch_dir;
		pst->set_wnd(CreateWindowW(TEXT("STATIC"), pst->txt.c_str(), WS_CHILD | WS_VISIBLE | SS_RIGHT,
			pst->pt.X, pst->pt.Y, pst->sz.Width, pst->sz.Height, hwnd, (HMENU)(pst->id), hInst, NULL));
		pst = pPanelBase->psubobjs->st_sl_notch_dir;
		pst->set_wnd(CreateWindowW(TEXT("STATIC"), pst->txt.c_str(), WS_CHILD | WS_VISIBLE | SS_RIGHT,
			pst->pt.X, pst->pt.Y, pst->sz.Width, pst->sz.Height, hwnd, (HMENU)(pst->id), hInst, NULL));
		pst = pPanelBase->psubobjs->st_gt_notch_dir;
		pst->set_wnd(CreateWindowW(TEXT("STATIC"), pst->txt.c_str(), WS_CHILD | WS_VISIBLE | SS_RIGHT,
			pst->pt.X, pst->pt.Y, pst->sz.Width, pst->sz.Height, hwnd, (HMENU)(pst->id), hInst, NULL));

		pst = pPanelBase->psubobjs->st_mh_target_v;
		pst->set_wnd(CreateWindowW(TEXT("STATIC"), pst->txt.c_str(), WS_CHILD | WS_VISIBLE | SS_RIGHT,
			pst->pt.X, pst->pt.Y, pst->sz.Width, pst->sz.Height, hwnd, (HMENU)(pst->id), hInst, NULL));
		pst = pPanelBase->psubobjs->st_bh_target_v;
		pst->set_wnd(CreateWindowW(TEXT("STATIC"), pst->txt.c_str(), WS_CHILD | WS_VISIBLE | SS_RIGHT,
			pst->pt.X, pst->pt.Y, pst->sz.Width, pst->sz.Height, hwnd, (HMENU)(pst->id), hInst, NULL));
		pst = pPanelBase->psubobjs->st_sl_target_v;
		pst->set_wnd(CreateWindowW(TEXT("STATIC"), pst->txt.c_str(), WS_CHILD | WS_VISIBLE | SS_RIGHT,
			pst->pt.X, pst->pt.Y, pst->sz.Width, pst->sz.Height, hwnd, (HMENU)(pst->id), hInst, NULL));
		pst = pPanelBase->psubobjs->st_gt_target_v;
		pst->set_wnd(CreateWindowW(TEXT("STATIC"), pst->txt.c_str(), WS_CHILD | WS_VISIBLE | SS_RIGHT,
			pst->pt.X, pst->pt.Y, pst->sz.Width, pst->sz.Height, hwnd, (HMENU)(pst->id), hInst, NULL));

		pst = pPanelBase->psubobjs->st_mh_ref_v;
		pst->set_wnd(CreateWindowW(TEXT("STATIC"), pst->txt.c_str(), WS_CHILD | WS_VISIBLE | SS_RIGHT,
			pst->pt.X, pst->pt.Y, pst->sz.Width, pst->sz.Height, hwnd, (HMENU)(pst->id), hInst, NULL));
		pst = pPanelBase->psubobjs->st_bh_ref_v;
		pst->set_wnd(CreateWindowW(TEXT("STATIC"), pst->txt.c_str(), WS_CHILD | WS_VISIBLE | SS_RIGHT,
			pst->pt.X, pst->pt.Y, pst->sz.Width, pst->sz.Height, hwnd, (HMENU)(pst->id), hInst, NULL));
		pst = pPanelBase->psubobjs->st_sl_ref_v;
		pst->set_wnd(CreateWindowW(TEXT("STATIC"), pst->txt.c_str(), WS_CHILD | WS_VISIBLE | SS_RIGHT,
			pst->pt.X, pst->pt.Y, pst->sz.Width, pst->sz.Height, hwnd, (HMENU)(pst->id), hInst, NULL));
		pst = pPanelBase->psubobjs->st_gt_ref_v;
		pst->set_wnd(CreateWindowW(TEXT("STATIC"), pst->txt.c_str(), WS_CHILD | WS_VISIBLE | SS_RIGHT,
			pst->pt.X, pst->pt.Y, pst->sz.Width, pst->sz.Height, hwnd, (HMENU)(pst->id), hInst, NULL));

		pst = pPanelBase->psubobjs->st_mh_fb_v;
		pst->set_wnd(CreateWindowW(TEXT("STATIC"), pst->txt.c_str(), WS_CHILD | WS_VISIBLE | SS_RIGHT,
			pst->pt.X, pst->pt.Y, pst->sz.Width, pst->sz.Height, hwnd, (HMENU)(pst->id), hInst, NULL));
		pst = pPanelBase->psubobjs->st_bh_fb_v;
		pst->set_wnd(CreateWindowW(TEXT("STATIC"), pst->txt.c_str(), WS_CHILD | WS_VISIBLE | SS_RIGHT,
			pst->pt.X, pst->pt.Y, pst->sz.Width, pst->sz.Height, hwnd, (HMENU)(pst->id), hInst, NULL));
		pst = pPanelBase->psubobjs->st_sl_fb_v;
		pst->set_wnd(CreateWindowW(TEXT("STATIC"), pst->txt.c_str(), WS_CHILD | WS_VISIBLE | SS_RIGHT,
			pst->pt.X, pst->pt.Y, pst->sz.Width, pst->sz.Height, hwnd, (HMENU)(pst->id), hInst, NULL));
		pst = pPanelBase->psubobjs->st_gt_fb_v;
		pst->set_wnd(CreateWindowW(TEXT("STATIC"), pst->txt.c_str(), WS_CHILD | WS_VISIBLE | SS_RIGHT,
			pst->pt.X, pst->pt.Y, pst->sz.Width, pst->sz.Height, hwnd, (HMENU)(pst->id), hInst, NULL));

		pst = pPanelBase->psubobjs->st_mh_ref_trq;
		pst->set_wnd(CreateWindowW(TEXT("STATIC"), pst->txt.c_str(), WS_CHILD | WS_VISIBLE | SS_RIGHT,
			pst->pt.X, pst->pt.Y, pst->sz.Width, pst->sz.Height, hwnd, (HMENU)(pst->id), hInst, NULL));
		pst = pPanelBase->psubobjs->st_bh_ref_trq;
		pst->set_wnd(CreateWindowW(TEXT("STATIC"), pst->txt.c_str(), WS_CHILD | WS_VISIBLE | SS_RIGHT,
			pst->pt.X, pst->pt.Y, pst->sz.Width, pst->sz.Height, hwnd, (HMENU)(pst->id), hInst, NULL));
	}break;

	case WM_LBUTTONUP: {//マウス左ボタン押下でモニタウィンドウ描画更新
		InvalidateRect(hwnd, NULL, TRUE); // ウィンドウ全体を再描画
	}
	case WM_CTLCOLORSTATIC: {//スタティックテキストの色セット
		SetTextColor((HDC)wParam, RGB(220, 220, 220)); // ライトグレー
		SetBkMode((HDC)wParam, TRANSPARENT);
	}return (LRESULT)GetStockObject(BLACK_BRUSH); // 背景色に合わせる

	case WM_ERASEBKGND: {//ウィンドウの背景色をグレーに
		pPanelBase->psubobjs->pgraphic->FillRectangle(pPanelBase->psubobjs->pBrushBk, pPanelBase->psubobjs->rc_panel);
	}return 1; // 背景を処理したことを示す

	case WM_TIMER: {
		//InvalidateRect(pPanelBase->psubobjs->st_mh_ref_v->hWnd, NULL, TRUE);
		switch (crane_id) {
		case CARNE_ID_HHGH29: {
			LPST_PLC_RBUF_HHGH29 p_plc_rbuf = (LPST_PLC_RBUF_HHGH29)pCcIf->st_msg_pc_u_rcv.body.st.buf_io_read;
			wostringstream wos;

			if (pCcIf->st_msg_pc_u_rcv.body.st.st_motion_stat[ID_HOIST].notch_ref > 0)
				SetWindowText(pPanelBase->psubobjs->st_mh_notch_dir->hWnd, L"+");
			else if (pCcIf->st_msg_pc_u_rcv.body.st.st_motion_stat[ID_HOIST].notch_ref < 0)
				SetWindowText(pPanelBase->psubobjs->st_mh_notch_dir->hWnd, L"-");
			else
				SetWindowText(pPanelBase->psubobjs->st_mh_notch_dir->hWnd, L"0");

			if (pCcIf->st_msg_pc_u_rcv.body.st.st_motion_stat[ID_BOOM_H].notch_ref > 0)
				SetWindowText(pPanelBase->psubobjs->st_bh_notch_dir->hWnd, L"+");
			else if (pCcIf->st_msg_pc_u_rcv.body.st.st_motion_stat[ID_BOOM_H].notch_ref < 0)
				SetWindowText(pPanelBase->psubobjs->st_bh_notch_dir->hWnd, L"-");
			else
				SetWindowText(pPanelBase->psubobjs->st_bh_notch_dir->hWnd, L"0");

			if (pCcIf->st_msg_pc_u_rcv.body.st.st_motion_stat[ID_SLEW].notch_ref > 0)
				SetWindowText(pPanelBase->psubobjs->st_sl_notch_dir->hWnd, L"+");
			else if (pCcIf->st_msg_pc_u_rcv.body.st.st_motion_stat[ID_SLEW].notch_ref < 0)
				SetWindowText(pPanelBase->psubobjs->st_sl_notch_dir->hWnd, L"-");
			else
				SetWindowText(pPanelBase->psubobjs->st_sl_notch_dir->hWnd, L"0");

			if (pCcIf->st_msg_pc_u_rcv.body.st.st_motion_stat[ID_GANTRY].notch_ref > 0)
				SetWindowText(pPanelBase->psubobjs->st_gt_notch_dir->hWnd, L"+");
			else if (pCcIf->st_msg_pc_u_rcv.body.st.st_motion_stat[ID_GANTRY].notch_ref < 0)
				SetWindowText(pPanelBase->psubobjs->st_gt_notch_dir->hWnd, L"-");
			else
				SetWindowText(pPanelBase->psubobjs->st_gt_notch_dir->hWnd, L"0");
		
			//目標速度
			wos.str(L""); wos << p_plc_rbuf->cv_tg[0];
			SetWindowText(pPanelBase->psubobjs->st_mh_target_v->hWnd, wos.str().c_str());
			wos.str(L""); wos << p_plc_rbuf->cv_tg[1];
			SetWindowText(pPanelBase->psubobjs->st_bh_target_v->hWnd, wos.str().c_str());
			wos.str(L""); wos << p_plc_rbuf->cv_tg[2];
			SetWindowText(pPanelBase->psubobjs->st_sl_target_v->hWnd, wos.str().c_str());
			wos.str(L""); wos << p_plc_rbuf->cv_tg[3];
			SetWindowText(pPanelBase->psubobjs->st_gt_target_v->hWnd, wos.str().c_str());

			//速度指令
			wos.str(L""); wos << p_plc_rbuf->inv_vref[0]; 
			SetWindowText(pPanelBase->psubobjs->st_mh_ref_v->hWnd, wos.str().c_str());
			wos.str(L""); wos << p_plc_rbuf->inv_vref[1];
			SetWindowText(pPanelBase->psubobjs->st_bh_ref_v->hWnd, wos.str().c_str());
			wos.str(L""); wos << p_plc_rbuf->inv_vref[2];
			SetWindowText(pPanelBase->psubobjs->st_sl_ref_v->hWnd, wos.str().c_str());
			wos.str(L""); wos << p_plc_rbuf->inv_vref[3];
			SetWindowText(pPanelBase->psubobjs->st_gt_ref_v->hWnd, wos.str().c_str());

			//速度FB
			wos.str(L""); wos << p_plc_rbuf->inv_vfb[0];
			SetWindowText(pPanelBase->psubobjs->st_mh_fb_v->hWnd, wos.str().c_str());
			wos.str(L""); wos << p_plc_rbuf->inv_vfb[1];
			SetWindowText(pPanelBase->psubobjs->st_bh_fb_v->hWnd, wos.str().c_str());
			wos.str(L""); wos << p_plc_rbuf->inv_vfb[2];
			SetWindowText(pPanelBase->psubobjs->st_sl_fb_v->hWnd, wos.str().c_str());
			wos.str(L""); wos << p_plc_rbuf->inv_vref[3];
			SetWindowText(pPanelBase->psubobjs->st_gt_fb_v->hWnd, wos.str().c_str());

			//ﾄﾙｸ指令
			wos.str(L""); wos << p_plc_rbuf->inv_trq[0];
			SetWindowText(pPanelBase->psubobjs->st_mh_ref_trq->hWnd, wos.str().c_str());
			wos.str(L""); wos << p_plc_rbuf->inv_trq[1];
			SetWindowText(pPanelBase->psubobjs->st_bh_ref_trq->hWnd, wos.str().c_str());



		}break;
		default: {
			SetWindowText(pPanelBase->psubobjs->st_mh_notch_dir->hWnd, L"?");
			SetWindowText(pPanelBase->psubobjs->st_bh_notch_dir->hWnd, L"?");
			SetWindowText(pPanelBase->psubobjs->st_sl_notch_dir->hWnd, L"?");
			SetWindowText(pPanelBase->psubobjs->st_gt_notch_dir->hWnd, L"?");

			SetWindowText(pPanelBase->psubobjs->st_mh_target_v->hWnd, L"?");
			SetWindowText(pPanelBase->psubobjs->st_bh_target_v->hWnd, L"?");
			SetWindowText(pPanelBase->psubobjs->st_sl_target_v->hWnd, L"?");
			SetWindowText(pPanelBase->psubobjs->st_gt_target_v->hWnd, L"?");

			SetWindowText(pPanelBase->psubobjs->st_mh_ref_v->hWnd, L"?");
			SetWindowText(pPanelBase->psubobjs->st_bh_ref_v->hWnd, L"?");
			SetWindowText(pPanelBase->psubobjs->st_sl_ref_v->hWnd, L"?");
			SetWindowText(pPanelBase->psubobjs->st_gt_ref_v->hWnd, L"?");

			SetWindowText(pPanelBase->psubobjs->st_mh_fb_v->hWnd, L"?");
			SetWindowText(pPanelBase->psubobjs->st_bh_fb_v->hWnd, L"?");
			SetWindowText(pPanelBase->psubobjs->st_sl_fb_v->hWnd, L"?");
			SetWindowText(pPanelBase->psubobjs->st_gt_fb_v->hWnd, L"?");

			SetWindowText(pPanelBase->psubobjs->st_mh_ref_trq->hWnd, L"?");
			SetWindowText(pPanelBase->psubobjs->st_bh_ref_trq->hWnd, L"?");

		}break;
		}

	}break;

	case WM_COMMAND: {
		INT16 code = 0;
		int wmId = LOWORD(wParam);
		// 選択されたメニューの解析:
		switch (wmId)
		{
		case ID_SUB_PNL_STAT_OBJ_PB_NEXT: {
			pPanelBase->psubobjs->i_disp_page++;
			if (pPanelBase->psubobjs->i_disp_page >= pPanelBase->psubobjs->n_disp_page)
				pPanelBase->psubobjs->i_disp_page = 0; //ページ番号をリセット
		}break; 
		case ID_SUB_PNL_STAT_OBJ_PB_BACK: {
			pPanelBase->psubobjs->i_disp_page--;
			if (pPanelBase->psubobjs->i_disp_page < 0)
				pPanelBase->psubobjs->i_disp_page = pPanelBase->psubobjs->n_disp_page - 1; //ページ番号をリセット
		}break;
		default:
			return DefWindowProc(hPnlWnd, uMsg, wParam, lParam);
		}
	}break;

	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);
		PatBlt(hdc, 0, 0, SUB_PNL_WND_W, SUB_PNL_WND_H, BLACKNESS);
		EndPaint(hwnd, &ps);
	}break;
	case WM_DRAWITEM: {//ランプ表示を更新 TIMERイベントで状態変化チェックしてInvalidiateRectで呼び出し
		DRAWITEMSTRUCT* pDIS = (DRAWITEMSTRUCT*)lParam;

		Gdiplus::Graphics gra(pDIS->hDC);
		Font* pfont = NULL;
		CSubPanelObj* pos = pPanelBase->psubobjs;
		CLampCtrl* plamp = NULL;
	
	}return true;
	case WM_DESTROY:
		//表示更新用タイマー
		KillTimer(hPnlWnd, ID_SUB_PANEL_TIMER);
		// PostQuitMessage(0);
		return 0;
	case WM_CLOSE:
		DestroyWindow(hwnd);
		return 0;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

