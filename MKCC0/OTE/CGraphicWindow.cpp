#include "CGraphicWindow.h"
#include "COpePanel.h"
#include "CPanelWindow.h"
#include "CPanelObj.h"
#include <commctrl.h>       //コモンコントロール用
#include "CBasicControl.h"
#include <vector>
#include "COteEnv.h"
#include "CFaults.h"


extern vector<CBasicControl*>	VectCtrlObj;
extern BC_TASK_ID st_task_id;
extern CCrane* pCrane;

static COteEnv* pEnvObj;

HWND CGraphicWindow::hGWnd;
HWND CGraphicWindow::hParentWnd;
CPanelBase* CGraphicWindow::pPanelBase;
int CGraphicWindow::crane_id;

LPST_OTE_UI CGraphicWindow::pUi;
LPST_OTE_CS_INF CGraphicWindow::pCsInf;
LPST_OTE_CC_IF CGraphicWindow::pCcIf;
LPST_OTE_ENV_INF CGraphicWindow::pOteEnvInf;

CGraphicWindow::CGraphicWindow(HINSTANCE hInstance, HWND hParent, int _crane_id, CPanelBase* _pPanelBase) {
	pPanelBase = _pPanelBase;
	hParentWnd = hParent;
	
	const wchar_t CLASS_NAME[]			= L"GWindowClass";
	const wchar_t CLASS_NAME_HHGH29[]	= L"GWindowHHGH29Class";

	const wchar_t* pClassName;

	WNDCLASS wc = { };
	switch (_crane_id) {

	case CARNE_ID_HHGH29:
		wc.lpfnWndProc = GWndProcHHGH29;
		wc.hInstance = hInstance;
		wc.lpszClassName = pClassName = CLASS_NAME_HHGH29;
		break;
	default:
		wc.lpfnWndProc = GWndProc;
		wc.hInstance = hInstance;
		wc.lpszClassName = pClassName = CLASS_NAME;
		break;
	}
	RegisterClass(&wc);

	hGWnd = CreateWindowEx(
		0,																// Optional window styles
		pClassName,														// Window class
		L"MAIN GRAPHIC",												// Window text
		WS_CHILD | WS_BORDER,											// Window style
		GMAIN_PNL_WND_X, GMAIN_PNL_WND_Y, GMAIN_PNL_WND_W, GMAIN_PNL_WND_H,
		hParent, nullptr, hInstance, nullptr
	);

	if (hGWnd) {
		ShowWindow(hGWnd, SW_SHOW);
		UpdateWindow(hGWnd);
	}
}
CGraphicWindow::~CGraphicWindow()
{
	close();
}
int CGraphicWindow::close()
{
	DestroyWindow(hGWnd);
	return 0;
}
void CGraphicWindow::set_up(LPST_OTE_UI _pUi, LPST_OTE_CS_INF _pCsInf, LPST_OTE_CC_IF _pCcIf, LPST_OTE_ENV_INF _pOteEnvInf, int _crane_id) {
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
	}
	break;
	}
	return;
};

// 描画処理関数
static int gwin_count = 0;
/// <summary>
/// ウィンドウのペイントイベント時に、ダブルバッファリングを用いて
/// 背景画像、アイコン画像、座標文字列を描画し、画面に転送します。
/// 描画品質の向上や透過処理も行います。
/// </summary>
/// <param name="hdc">描画対象ウィンドウのデバイスコンテキストハンドル。</param>
/// <param name="hWnd">描画対象ウィンドウのハンドル。</param>
/// ########################################################################
void CGraphicWindow::OnPaint(HDC hdc, HWND hWnd){

int width = GMAIN_PNL_WND_W;
int height = GMAIN_PNL_WND_H;

gwin_count++; if (gwin_count > 1000) gwin_count = 0; // カウントをリセット

// 1. 背景画像の描画(pbmp_bk）
pPanelBase->pgwinobjs->lmg_bk_gwindow->set(0);		// 背景画像書き込み
pPanelBase->pgwinobjs->lmg_bk_gwindow->update();	// 背景画像書き込み
pPanelBase->pgwinobjs->lmg_crane_gt_base->set(0);
pPanelBase->pgwinobjs->lmg_crane_gt_base->update();	// クレーン画像書き込み


// 2. クレーン画像の描画(pbmp_img） 
pPanelBase->pgwinobjs->lmg_crane_bm_xy->set(0);
pPanelBase->pgwinobjs->lmg_crane_bm_xy->update();	// クレーン画像書き込み
//pPanelBase->pgwinobjs->lmg_crane_bm_xy->update_with_scale_angle(40,0,1.0,1.0,0.0);	// クレーン画像書き込み
//pPanelBase->pgwinobjs->lmg_crane_bm_xy->update_with_center(gwin_count * -1.0,0);	// クレーン画像書き込み
pPanelBase->pgwinobjs->lmg_crane_potal->set(0);
pPanelBase->pgwinobjs->lmg_crane_potal->update();	// クレーン画像書き込み



// 3. Info画像の描画(pbmp_inf） 
wostringstream wo; 
wo.str(L""); wo << L"揚程： " << 50.0 + gwin_count*0.1;
pPanelBase->pgwinobjs->str_pos_mh->update(wo.str().c_str());	// 主巻位置書き込み
wo.str(L""); wo << L"半径： " << 50.0 - gwin_count * 0.1;
pPanelBase->pgwinobjs->str_pos_bh->update(wo.str().c_str());	// 半径書き込み
wo.str(L""); wo << L"旋回角： " << 0.0 + gwin_count * 0.1;
pPanelBase->pgwinobjs->str_pos_sl->update(wo.str().c_str());	// 旋回各書き込み
wo.str(L""); wo << L"走行位置： " << 100.0 + gwin_count * 0.1;
pPanelBase->pgwinobjs->str_pos_gt->update(wo.str().c_str());	// 主巻位置書き込み

// クレーン画像を背景画像に書き込み
Status drawStatus = pPanelBase->pgwinobjs->pgraphic_bk->DrawImage(
	pPanelBase->pgwinobjs->pbmp_img,
	pPanelBase->pgwinobjs->rc_panel,
	0, 0, GMAIN_PNL_WND_W, GMAIN_PNL_WND_H,
	UnitPixel,
	&pPanelBase->pgwinobjs->attr
);

// 情報画像を背景画像に書き込み
 drawStatus = pPanelBase->pgwinobjs->pgraphic_bk->DrawImage(
	pPanelBase->pgwinobjs->pbmp_inf,
	pPanelBase->pgwinobjs->rc_panel,
	0, 0, GMAIN_PNL_WND_W, GMAIN_PNL_WND_H,
	UnitPixel,
	&pPanelBase->pgwinobjs->attr
);

// バックバッファの内容を一度に画面に転送
pPanelBase->pgwinobjs->pgraphic->DrawImage(pPanelBase->pgwinobjs->pbmp_bk, 0, 0);
}


LRESULT CALLBACK CGraphicWindow::GWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_CREATE: {
		//表示更新用タイマー
		SetTimer(hwnd, ID_GMAIN_TIMER, ID_GMAIN_TIMER_MS, NULL);
	}break;

	case WM_LBUTTONUP: {//マウス左ボタン押下でモニタウィンドウ描画更新
		InvalidateRect(hwnd, NULL, FALSE); // ウィンドウ全体を再描画
	}
	case WM_CTLCOLORSTATIC: {//スタティックテキストの色セット
		SetTextColor((HDC)wParam, RGB(220, 220, 220)); // ライトグレー
		SetBkMode((HDC)wParam, TRANSPARENT);
	}return (LRESULT)GetStockObject(NULL_BRUSH); // 背景色に合わせる

	case WM_ERASEBKGND: {//ウィンドウの背景色をグレーに
		pPanelBase->psubobjs->pgraphic->FillRectangle(pPanelBase->pgwinobjs->pBrushBk, pPanelBase->pgwinobjs->rc_panel);
	}return 1; // 背景を処理したことを示す

	case WM_NOTIFY: {
	}break;

	case WM_TIMER: {

	}break;
	case WM_COMMAND: {

	}break;
	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);

		Gdiplus::Bitmap* pbmp_bk = Gdiplus::Bitmap::FromHBITMAP(pPanelBase->pgwinobjs->hBmp_bk, NULL);
		Gdiplus::Bitmap* pbmp_inf = Gdiplus::Bitmap::FromHBITMAP(pPanelBase->pgwinobjs->hBmp_inf, NULL);
		Rect destRect(0, 0, GMAIN_PNL_WND_W, GMAIN_PNL_WND_H);

		//背景画像描画
		pPanelBase->pgwinobjs->pgraphic->DrawImage(pbmp_bk, destRect, 0, 0, GMAIN_PNL_WND_W, GMAIN_PNL_WND_H, UnitPixel);

		//情報画像の背景を黒く塗りつぶしてメッセージ書き込み
		PatBlt(pPanelBase->pgwinobjs->hdc_inf, 0, 0, GMAIN_PNL_WND_W, GMAIN_PNL_WND_H, BLACKNESS);
		pPanelBase->pgwinobjs->str_pos_mh->update(); //故障メッセージ更新

		Status drawStatus = pPanelBase->pgwinobjs->pgraphic->DrawImage(pbmp_inf, destRect, 0, 0, GMAIN_PNL_WND_W, GMAIN_PNL_WND_H, UnitPixel, &pPanelBase->pgwinobjs->attr);

		EndPaint(hwnd, &ps);
	}break;
	case WM_DRAWITEM: {//ランプ表示を更新 TIMERイベントで状態変化チェックしてInvalidiateRectで呼び出し
		DRAWITEMSTRUCT* pDIS = (DRAWITEMSTRUCT*)lParam;

		Gdiplus::Graphics gra(pDIS->hDC);
		Font* pfont = NULL;
		CSubPanelObj* pos = pPanelBase->psubobjs;
		CLampCtrl* plamp = NULL;

	}return true;
	case WM_DESTROY: {
		//表示更新用タイマー
		KillTimer(hwnd, ID_GMAIN_TIMER);
		// PostQuitMessage(0);
	}return 0;
	case WM_CLOSE: {
		DestroyWindow(hwnd);
	}return 0;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
LRESULT CALLBACK CGraphicWindow::GWndProcHHGH29(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_CREATE: {
		//グラフィックオブジェクトの初期化
		pPanelBase->pgwinobjs->setup_graphics(hwnd);
		pPanelBase->pgwinobjs->refresh_obj_graphics();

		pPanelBase->psubobjs->colorkey.SetValue(Color::Black);//黒を透過
		Status status = pPanelBase->pgwinobjs->attr.SetColorKey(
			pPanelBase->pgwinobjs->colorkey,
			pPanelBase->pgwinobjs->colorkey,
			ColorAdjustTypeDefault // DefaultではなくBitmapを指定する方が明確
		);

		//表示更新用タイマー
		SetTimer(hwnd, ID_GMAIN_TIMER, ID_GMAIN_TIMER_MS, NULL);

	}break;

	case WM_LBUTTONUP: {//マウス左ボタン押下でモニタウィンドウ描画更新
		InvalidateRect(hwnd, NULL, FALSE); // ウィンドウ全体を再描画
	}
	case WM_CTLCOLORSTATIC: {//スタティックテキストの色セット
		SetTextColor((HDC)wParam, RGB(220, 220, 220)); // ライトグレー
		SetBkMode((HDC)wParam, TRANSPARENT);
	}return (LRESULT)GetStockObject(NULL_BRUSH); // 背景色に合わせる

	case WM_ERASEBKGND: {//ウィンドウの背景色を書き込み
		pPanelBase->pgwinobjs->lmg_bk_gwindow->set(0);
		pPanelBase->pgwinobjs->lmg_bk_gwindow->update();
	}return 1; // 背景を処理したことを示す

	case WM_NOTIFY: {
	}break;

	case WM_TIMER: {
		InvalidateRect(hwnd, NULL, false);
	}break;
	case WM_COMMAND: {

	}break;
	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);

		OnPaint(hdc, hwnd); // 描画処理関数を呼び出す

		EndPaint(hwnd, &ps);
	}break;
	case WM_DRAWITEM: {//ランプ表示を更新 TIMERイベントで状態変化チェックしてInvalidiateRectで呼び出し
		DRAWITEMSTRUCT* pDIS = (DRAWITEMSTRUCT*)lParam;
		Gdiplus::Graphics gra(pDIS->hDC);
	}return true;
	case WM_DESTROY: {
		//表示更新用タイマー
	
		// PostQuitMessage(0);
	}return 0;
	case WM_CLOSE: {
		DestroyWindow(hwnd);
	}return 0;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
