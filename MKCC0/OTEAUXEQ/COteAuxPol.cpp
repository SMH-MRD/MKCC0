#include "COteAuxPol.h"
#include "resource.h"
#include "CSHAREDMEM.H"
#include <commctrl.h>
#include "SmemOte.H"

#pragma comment (lib, "Gdiplus.lib")

using namespace cv;
using namespace Gdiplus;


ST_AUXPOL_MON2 COteAuxPol::st_mon2;
ST_AUXPOL_MON1 COteAuxPol::st_mon1;

cv::Rect COteAuxPol::rc_mat_roi_work;
cv::Rect COteAuxPol::rc_mat_roi_criterion;
cv::Rect COteAuxPol::rc_mat_roi_work_disp;		//描画用に拡大/縮小している場合のROI
cv::Rect COteAuxPol::rc_mat_roi_criterion_disp;	//描画用に拡大/縮小している場合のROI
cv::Mat COteAuxPol::mat_roi_work;
cv::Mat COteAuxPol::mat_criterion;

std::unique_ptr<Pen>        COteAuxPol::m_pWhitePen;
std::unique_ptr<Pen>        COteAuxPol::m_pBlackPen;
std::unique_ptr<Pen>        COteAuxPol::m_pGrayPen;
std::unique_ptr<Pen>        COteAuxPol::m_pRedPen;
std::unique_ptr<Pen>        COteAuxPol::m_pYellowPen;
std::unique_ptr<Pen>        COteAuxPol::m_pGreenPen;
std::unique_ptr<Pen>        COteAuxPol::m_pLiteBluePen;
std::unique_ptr<Pen>        COteAuxPol::m_pMagentaPen;

std::unique_ptr<SolidBrush> COteAuxPol::m_pWhiteBrush;
std::unique_ptr<SolidBrush> COteAuxPol::m_pBlackBrush;
std::unique_ptr<SolidBrush> COteAuxPol::m_pGrayBrush;
std::unique_ptr<SolidBrush> COteAuxPol::m_pRedBrush;
std::unique_ptr<SolidBrush> COteAuxPol::m_pYellowBrush;
std::unique_ptr<SolidBrush> COteAuxPol::m_pGreenBrush;
std::unique_ptr<SolidBrush> COteAuxPol::m_pLiteBlueBrush;
std::unique_ptr<SolidBrush> COteAuxPol::m_pBlueBrush;
std::unique_ptr<SolidBrush> COteAuxPol::m_pMagentaBrush;


std::unique_ptr<FontFamily> COteAuxPol::m_pFontFamily;
std::unique_ptr<Font>		COteAuxPol::m_pFont18;
std::unique_ptr<Font>		COteAuxPol::m_pFont36;

std::unique_ptr<Bitmap>   COteAuxPol::m_pOffscreenBitmap;
std::unique_ptr<Graphics> COteAuxPol::m_pOffscreenGraphics;

Graphics* COteAuxPol::pgraphic_img;

LPST_AUXPOL_IMG_PROC COteAuxPol::pst_img_proc;

extern CSharedMem* pAuxEnvInfObj;
extern CSharedMem* pAuxAgentInfObj;
extern CSharedMem* pAuxCsInfObj;
extern CSharedMem* pAuxPolInfObj;
extern CSharedMem* pCsInfObj;

extern BC_TASK_ID st_task_id;
extern vector<CBasicControl*>	VectCtrlObj;	    //スレッドオブジェクトのポインタ

static COteAuxPol* pPolObj;

//共有メモリ
static LPST_OTE_AUX_ENV_INF		pAuxEnvInf = NULL;
static LPST_OTE_AUX_CS_INF		pAuxCSInf = NULL;
static LPST_OTE_AUX_AGENT_INF	pAuxAgInf = NULL;
static LPST_OTE_AUX_POL_INF		pAuxPolInf = NULL;
static LPST_OTE_CS_INF			pCSInf = NULL;

COteAuxPol::COteAuxPol() {

	// 1. GDI+ 初期化
	GdiplusStartupInput gdiplusStartupInput;
	Status status = GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);
	if (status != Ok) {
		MessageBox(nullptr, L"GDI+ の初期化に失敗しました。", L"Error", MB_ICONERROR);
	}

	// 2. 描画オブジェクトの生成 (Startupの後に実行)
	m_pWhitePen		= std::make_unique<Pen>(Color(255, 255, 255, 255), 2);
	m_pBlackPen		= std::make_unique<Pen>(Color(255, 0, 0, 0), 2);
	m_pGrayPen		= std::make_unique<Pen>(Color(255, 100, 100, 100), 2);
	m_pRedPen		= std::make_unique<Pen>(Color(255, 255, 0, 0), 2);
	m_pYellowPen	= std::make_unique<Pen>(Color(255, 255, 255, 0), 2);
	m_pGreenPen		= std::make_unique<Pen>(Color(255, 0, 255, 0), 2);
	m_pLiteBluePen	= std::make_unique<Pen>(Color(255, 0, 100, 255), 2);
	m_pMagentaPen	= std::make_unique<Pen>(Color(255, 255, 0, 255), 2);

	m_pWhiteBrush	= std::make_unique<SolidBrush>(Color(150, 255, 255, 255));
	m_pBlackBrush	= std::make_unique<SolidBrush>(Color(150, 0, 0, 0));
	m_pGrayBrush	= std::make_unique<SolidBrush>(Color(150, 100, 100, 100));
	m_pRedBrush		= std::make_unique<SolidBrush>(Color(150, 255, 0, 0));
	m_pYellowBrush	= std::make_unique<SolidBrush>(Color(150, 255, 255, 0));
	m_pGreenBrush	= std::make_unique<SolidBrush>(Color(150, 0, 255, 0));
	m_pLiteBlueBrush= std::make_unique<SolidBrush>(Color(150, 0, 100, 255));
	m_pBlueBrush	= std::make_unique<SolidBrush>(Color(150, 0, 0, 255));
	m_pMagentaBrush	= std::make_unique<SolidBrush>(Color(150, 255, 0, 255));

	m_pFontFamily = std::make_unique<FontFamily>(L"Arial");
	m_pFont18 = std::make_unique<Font>(m_pFontFamily.get(), 18, FontStyleBold, UnitPixel);
	m_pFont36 = std::make_unique<Font>(m_pFontFamily.get(), 36, FontStyleBold, UnitPixel);

	// 1. オフスクリーンバッファ（メモリ上のキャンバス）を作成
	// 描画エリアのサイズと同じ大きさの空の画像を作る
	m_pOffscreenBitmap = std::make_unique<Bitmap>(AUXPOL_MON2_CAMIMG_W, AUXPOL_MON2_WND_H, PixelFormat32bppARGB);

	// 2. そのバッファに描き込むための Graphics オブジェクトを作成
	m_pOffscreenGraphics = std::unique_ptr<Graphics>(Graphics::FromImage(m_pOffscreenBitmap.get()));

	// ★★★ ここで白く塗りつぶす ★★★
  // Color(Alpha, Red, Green, Blue) -> 全部255で不透明な白
	m_pOffscreenGraphics->Clear(Color(255, 255, 255, 255));

	// 描画品質の設定
	m_pOffscreenGraphics->SetSmoothingMode(SmoothingModeAntiAlias);

}
COteAuxPol::~COteAuxPol() {

	// GDI+ 終了処理 (オブジェクトを先に消してからShutdown)
	m_pWhitePen.reset();
	m_pBlackPen.reset();
	m_pGrayPen.reset();
	m_pRedPen.reset();
	m_pYellowPen.reset();
	m_pGreenPen.reset();
	m_pLiteBluePen.reset();
	m_pMagentaPen.reset();

	m_pWhiteBrush.reset();
	m_pBlackBrush.reset();
	m_pGrayBrush.reset();
	m_pRedBrush.reset();
	m_pYellowBrush.reset();
	m_pGreenBrush.reset();
	m_pLiteBlueBrush.reset();
	m_pBlueBrush.reset();
	m_pMagentaBrush.reset();

	m_pFont18.reset();
	m_pFont36.reset();
	m_pFontFamily.reset();
	GdiplusShutdown(m_gdiplusToken);

}

HRESULT COteAuxPol::initialize(LPVOID lpParam) {

	//### 出力用共有メモリ取得
	out_size = sizeof(ST_OTE_AUX_POL_INF);
	set_outbuf(pAuxPolInfObj->get_pMap());

	//### 入力用共有メモリ取得
	pAuxAgInf	= (LPST_OTE_AUX_AGENT_INF)pAuxAgentInfObj->get_pMap();
	pAuxEnvInf = (LPST_OTE_AUX_ENV_INF)(pAuxEnvInfObj->get_pMap());
	pAuxCSInf	= (LPST_OTE_AUX_CS_INF)pAuxCsInfObj->get_pMap();
	pAuxPolInf = (LPST_OTE_AUX_POL_INF)pAuxPolInfObj->get_pMap();
	pCSInf = (LPST_OTE_CS_INF)pCsInfObj->get_pMap();

	pPolObj = (COteAuxPol*)VectCtrlObj[st_task_id.POL];
	pst_img_proc = &(pAuxPolInf->st_img_proc);
	pst_img_proc->h_margin = pst_img_proc->s_margin = pst_img_proc->v_margin = 20;

	//### 初期化
	wos.str(L"");//初期化
	rc_mat_roi_work = { AUXPOL_DEFAULT_ROI_X, AUXPOL_DEFAULT_ROI_Y, AUXPOL_DEFAULT_ROI_W,AUXPOL_DEFAULT_ROI_H};
	rc_mat_roi_criterion = { AUXPOL_DEFAULT_ROI_X, AUXPOL_DEFAULT_ROI_Y, AUXPOL_DEFAULT_ROI_W,AUXPOL_DEFAULT_ROI_H };
	rc_mat_roi_work_disp = { AUXPOL_DEFAULT_ROI_X/ AUXPOL_CAMERA_DISP_RETIO, AUXPOL_DEFAULT_ROI_Y / AUXPOL_CAMERA_DISP_RETIO, AUXPOL_DEFAULT_ROI_W / AUXPOL_CAMERA_DISP_RETIO, AUXPOL_DEFAULT_ROI_H / AUXPOL_CAMERA_DISP_RETIO };
	rc_mat_roi_criterion_disp = { AUXPOL_DEFAULT_ROI_X / AUXPOL_CAMERA_DISP_RETIO, AUXPOL_DEFAULT_ROI_Y / AUXPOL_CAMERA_DISP_RETIO, AUXPOL_DEFAULT_ROI_W / AUXPOL_CAMERA_DISP_RETIO,AUXPOL_DEFAULT_ROI_H / AUXPOL_CAMERA_DISP_RETIO };

	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_MON_CHECK1, L"mon1");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_MON_CHECK2, L"CamChk");

	//モニタウィンドウテキスト	
	set_func_pb_txt();
	set_item_chk_txt();
	set_panel_tip_txt();
	return S_OK;
}

HRESULT COteAuxPol::routine_work(void* pObj) {
	input();
	parse();
	output();

	if (inf.total_act % 20 == 0) {
		wos.str(L""); wos << inf.status << L":" << std::setfill(L'0') << std::setw(4) << inf.act_time;
		msg2host(wos.str());
	}

	return S_OK;
}

int COteAuxPol::input() {

	return S_OK;
}
int COteAuxPol::parse() {           //メイン処理

	// 1. カメラ画像がない場合は処理しない
	if (pAuxAgInf->st_usb_cam.hsvMatFrame.empty()) {
		pst_img_proc->status &= ~AUXPOL_CODE_IMG_PROC_ENABLE;
		return AUXPOL_CODE_VIDEO_CHK_ERR;
	}
	else pst_img_proc->status |= AUXPOL_CODE_IMG_PROC_ENABLE;

		
	// 2. 画像処理の基準値(pixlがON：検出条件成立の数）がない場合は処理しない
	if (pst_img_proc->target_chk_base_count)
		pst_img_proc->status |= AUXPOL_CODE_IMG_PROC_ACTIVE;
	else
		pst_img_proc->status &= ~AUXPOL_CODE_IMG_PROC_ACTIVE;

	int tem_int = 0;
	if (pst_img_proc->sl > pst_img_proc->sh) {//彩度閾値の下限が上限より大きい場合は入れ替える
		tem_int = pst_img_proc->sl;
		pst_img_proc->sl = pst_img_proc->sh;
		pst_img_proc->sh = tem_int;
	}
	if (pst_img_proc->vl > pst_img_proc->vh) {//明度閾値の下限が上限より大きい場合は入れ替える
		tem_int = pst_img_proc->vl;
		pst_img_proc->vl = pst_img_proc->vh;
		pst_img_proc->vh = tem_int;
	}
	if (st_mon2.is_monitor_active) {

		pst_img_proc->is_target_detected = GetCraneDeviceStatus(&(pAuxPolInf->st_img_proc));
		// 4. 表示用にマスク(1ch)をRGB(3ch)へ
		cv::cvtColor(pst_img_proc->mask, pst_img_proc->hsvMat_mask, cv::COLOR_GRAY2RGB);
	}

	return STAT_OK;
}
int COteAuxPol::output() {          //出力処理

	return STAT_OK;
}
int COteAuxPol::close() {
	return 0;
}

int COteAuxPol::GetCraneDeviceStatus(LPST_AUXPOL_IMG_PROC pst_work) {

	// HSVマスク画像生成
	cv::Mat m1, m2, local_mask;

	if(pAuxAgInf->st_usb_cam.hsvMatFrame.empty()) {
		return -1;
	}

	if (pst_work->hl > pst_work->hh) {//色相閾値の下限が上限より大きい場合は、0-180の両端で範囲を設定してチェック結果をORする
		cv::inRange(pAuxAgInf->st_usb_cam.hsvMatFrame, cv::Scalar(0, pst_work->sl, pst_work->vl), cv::Scalar(pst_work->hh, pst_work->sh, pst_work->vh), m1);
		cv::inRange(pAuxAgInf->st_usb_cam.hsvMatFrame, cv::Scalar(pst_work->hl, pst_work->sl, pst_work->vl), cv::Scalar(179, pst_work->sh, pst_work->vh), m2);
		cv::bitwise_or(m1, m2, pst_work->mask);
	}
	else {
		cv::inRange(pAuxAgInf->st_usb_cam.hsvMatFrame, cv::Scalar(pst_work->hl, pst_work->sl, pst_work->vl), cv::Scalar(pst_work->hh, pst_work->sh, pst_work->vh), pst_work->mask);
	}

	// 3. マスク画像からROI内の白いピクセル数をカウント
	pst_work->whiteCountInCriterionRoi = cv::countNonZero(pst_work->mask(rc_mat_roi_criterion));	//設定した基準Roi内の白いピクセル数をカウント
	pst_work->whiteCountInWorkRoi = cv::countNonZero(pst_work->mask(rc_mat_roi_work));				//設定したRoi内の白いピクセル数をカウント	

	int roi_cnt_diff = pst_work->whiteCountInWorkRoi - pst_work->whiteCountInWorkRoi_Last;
	if (roi_cnt_diff > pst_work->target_chk_base_count / 2) {
		pst_work->is_target_detected = L_ON;
	}
	if (roi_cnt_diff < -pst_work->target_chk_base_count / 2) {
		pst_work->is_target_detected = L_OFF;
	}

	pst_work->whiteCountInWorkRoi_Last = pst_work->whiteCountInWorkRoi;

	return pst_work->is_target_detected;

}

static wostringstream monwos;
LRESULT CALLBACK COteAuxPol::Mon1Proc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
	switch (msg)
	{
	case WM_CREATE: {
		InitCommonControls();//コモンコントロール初期化
		HINSTANCE hInst = (HINSTANCE)GetModuleHandle(0);
		//ウィンドウにコントロール追加
		st_mon1.hctrl[AUXPOL_ID_MON1_STATIC_INF] = CreateWindowW(TEXT("STATIC"), st_mon1.text[AUXPOL_ID_MON1_STATIC_INF], WS_CHILD | WS_VISIBLE | SS_LEFT,
			st_mon1.pt[AUXPOL_ID_MON1_STATIC_INF].x, st_mon1.pt[AUXPOL_ID_MON1_STATIC_INF].y,
			st_mon1.sz[AUXPOL_ID_MON1_STATIC_INF].cx, st_mon1.sz[AUXPOL_ID_MON1_STATIC_INF].cy,
			hWnd, (HMENU)(AUXPOL_ID_MON1_CTRL_BASE + AUXPOL_ID_MON1_STATIC_INF), hInst, NULL);

		//表示更新用タイマー
		SetTimer(hWnd, AUXPOL_ID_MON1_TIMER, st_mon1.timer_ms, NULL);

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

		SetWindowText(st_mon1.hctrl[AUXPOL_ID_MON1_STATIC_INF], monwos.str().c_str());
	}break;

	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
	}break;
	case WM_DESTROY: {
		st_mon1.hwnd_mon = NULL;
		st_mon1.is_monitor_active = false;
		KillTimer(hWnd, AUXPOL_ID_MON1_TIMER);
	}break;
	default:
		return DefWindowProc(hWnd, msg, wp, lp);
	}
	return S_OK;
};

LRESULT CALLBACK COteAuxPol::Mon2Proc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
	switch (msg)
	{
	case WM_CREATE: {
		InitCommonControls();//コモンコントロール初期化
		HINSTANCE hInst = (HINSTANCE)GetModuleHandle(0);
		setup_graphics(hWnd);

		//STATIC,LABEL
		CreateWindowW(TEXT("STATIC"), L"HL", WS_CHILD | WS_VISIBLE | SS_LEFT, 650, 55, 20, 20,hWnd, (HMENU)(AUXPOL_ID_MON2_CTRL_BASE), hInst, NULL);
		CreateWindowW(TEXT("STATIC"), L"HH", WS_CHILD | WS_VISIBLE | SS_LEFT, 650, 95, 20, 20, hWnd, (HMENU)(AUXPOL_ID_MON2_CTRL_BASE), hInst, NULL);
		CreateWindowW(TEXT("STATIC"), L"SL", WS_CHILD | WS_VISIBLE | SS_LEFT, 650, 155, 20, 20, hWnd, (HMENU)(AUXPOL_ID_MON2_CTRL_BASE), hInst, NULL);
		CreateWindowW(TEXT("STATIC"), L"SH", WS_CHILD | WS_VISIBLE | SS_LEFT, 650, 195, 20, 20, hWnd, (HMENU)(AUXPOL_ID_MON2_CTRL_BASE), hInst, NULL);
		CreateWindowW(TEXT("STATIC"), L"VL", WS_CHILD | WS_VISIBLE | SS_LEFT, 650, 255, 20, 20, hWnd, (HMENU)(AUXPOL_ID_MON2_CTRL_BASE), hInst, NULL);
		CreateWindowW(TEXT("STATIC"), L"VH", WS_CHILD | WS_VISIBLE | SS_LEFT, 650, 295, 20, 20, hWnd, (HMENU)(AUXPOL_ID_MON2_CTRL_BASE), hInst, NULL);

		for (int i = AUXPOL_ID_MON2_STATIC_MSG; i <= AUXPOL_ID_MON2_STATIC_MSG; i++) {
			st_mon2.hctrl[i] = CreateWindowW(TEXT("STATIC"), st_mon2.text[i], WS_CHILD | WS_VISIBLE | SS_LEFT,
				st_mon2.pt[i].x, st_mon2.pt[i].y, st_mon2.sz[i].cx, st_mon2.sz[i].cy,
				hWnd, (HMENU)(AUXPOL_ID_MON2_CTRL_BASE + i), hInst, NULL);
		}
		for (int i = AUXPOL_ID_MON2_STATIC_HL; i <= AUXPOL_ID_MON2_STATIC_VH; i++) {
			st_mon2.hctrl[i] = CreateWindowW(TEXT("STATIC"), st_mon2.text[i], WS_CHILD | WS_VISIBLE | SS_RIGHT,
				st_mon2.pt[i].x, st_mon2.pt[i].y, st_mon2.sz[i].cx, st_mon2.sz[i].cy,
				hWnd, (HMENU)(AUXPOL_ID_MON2_CTRL_BASE + i), hInst, NULL);
		}
		monwos.str(L""); monwos << pst_img_proc->hl;
		SetWindowText(st_mon2.hctrl[AUXPOL_ID_MON2_STATIC_HL], monwos.str().c_str());
		monwos.str(L""); monwos << pst_img_proc->hh;
		SetWindowText(st_mon2.hctrl[AUXPOL_ID_MON2_STATIC_HH], monwos.str().c_str());
		monwos.str(L""); monwos << pst_img_proc->sl;
		SetWindowText(st_mon2.hctrl[AUXPOL_ID_MON2_STATIC_SL], monwos.str().c_str());
		monwos.str(L""); monwos << pst_img_proc->sh;
		SetWindowText(st_mon2.hctrl[AUXPOL_ID_MON2_STATIC_SH], monwos.str().c_str());
		monwos.str(L""); monwos << pst_img_proc->vl;
		SetWindowText(st_mon2.hctrl[AUXPOL_ID_MON2_STATIC_VL], monwos.str().c_str());
		monwos.str(L""); monwos << pst_img_proc->vh;
		SetWindowText(st_mon2.hctrl[AUXPOL_ID_MON2_STATIC_VH], monwos.str().c_str());

		// スライダー配置 (x=660の位置に縦に並べる)
		st_mon2.hctrl[AUXPOL_ID_MON2_SLIDER_HL] = CreateSlider(hWnd, AUXPOL_ID_MON2_CTRL_BASE + AUXPOL_ID_MON2_SLIDER_HL, 680, 50, -1, 180, pst_img_proc->hl);
		st_mon2.hctrl[AUXPOL_ID_MON2_SLIDER_HH] = CreateSlider(hWnd, AUXPOL_ID_MON2_CTRL_BASE + AUXPOL_ID_MON2_SLIDER_HH, 680, 90, -1, 180, pst_img_proc->hh);
		st_mon2.hctrl[AUXPOL_ID_MON2_SLIDER_SL] = CreateSlider(hWnd, AUXPOL_ID_MON2_CTRL_BASE + AUXPOL_ID_MON2_SLIDER_SL, 680, 150, -1, 256, pst_img_proc->sl);
		st_mon2.hctrl[AUXPOL_ID_MON2_SLIDER_SH] = CreateSlider(hWnd, AUXPOL_ID_MON2_CTRL_BASE + AUXPOL_ID_MON2_SLIDER_SH, 680, 190, -1, 256, pst_img_proc->sh);
		st_mon2.hctrl[AUXPOL_ID_MON2_SLIDER_VL] = CreateSlider(hWnd, AUXPOL_ID_MON2_CTRL_BASE + AUXPOL_ID_MON2_SLIDER_VL, 680, 250, -1, 256, pst_img_proc->vl);
		st_mon2.hctrl[AUXPOL_ID_MON2_SLIDER_VH] = CreateSlider(hWnd, AUXPOL_ID_MON2_CTRL_BASE + AUXPOL_ID_MON2_SLIDER_VH, 680, 290, -1, 256, pst_img_proc->vh);

		//PB
		for (int i = AUXPOL_ID_MON2_PB_GET_CRITERION; i <= AUXPOL_ID_MON2_PB_SET_ROI; i++) {
				st_mon2.hctrl[i] = CreateWindowW(TEXT("BUTTON"), st_mon2.text[i], WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_PUSHLIKE ,
					st_mon2.pt[i].x, st_mon2.pt[i].y, st_mon2.sz[i].cx, st_mon2.sz[i].cy,
					hWnd, (HMENU)(AUXPOL_ID_MON2_CTRL_BASE + i), hInst, NULL);
		}

		//CB
		for (int i = AUXPOL_ID_MON2_CB_DISP_ROI_CRITERION; i <= AUXPOL_ID_MON2_CB_DISP_ROI_WORK; i++) {
		st_mon2.hctrl[i] = CreateWindowW(TEXT("BUTTON"), st_mon2.text[i], WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX | SS_CENTER,
			st_mon2.pt[i].x, st_mon2.pt[i].y, st_mon2.sz[i].cx, st_mon2.sz[i].cy,
			hWnd, (HMENU)(AUXPOL_ID_MON2_CTRL_BASE + i), hInst, NULL);
		}
		st_mon2.flg_disp_cri_roi = false;
		st_mon2.flg_disp_work_roi = false;

		//タイマー起動
		UINT rtn = SetTimer(hWnd, AUXPOL_ID_MON2_TIMER, AUXPOL_PRM_MON2_TIMER_MS, NULL);

		st_mon2.disp_mode = AUXPOL_CODE_MON2_DISP_HSV_MASK;

		break;

	}
	case WM_TIMER: {
		UpdateMon2(hWnd, st_mon2.hdc);
	}break;
	case WM_COMMAND: {
		int wmId = LOWORD(wp);
		// 選択されたメニューの解析:
		switch (wmId - AUXPOL_ID_MON2_CTRL_BASE)
		{
		case AUXPOL_ID_MON2_PB_GET_CRITERION: {
			if (pst_img_proc->status & AUXPOL_CODE_IMG_PROC_ENABLE) {
				rc_mat_roi_criterion_disp = get_hsv_criterion();
		
				HSV_autoCalibrate();
				//ROI WORKをセット
				rc_mat_roi_work_disp = set_work_roi(true);//true:rc_mat_roi_criterion+αの領域を設定

				//基準判定用Matをセット
				mat_criterion = pAuxAgInf->st_usb_cam.hsvMatFrame(rc_mat_roi_criterion);
				//基準カウント数取得
				cv::Mat mask;
				cv::Mat m1,m2;

				if (pst_img_proc->hl > pst_img_proc->hh) {
					cv::inRange(mat_criterion, cv::Scalar(0, pst_img_proc->sl, pst_img_proc->vl), cv::Scalar(pst_img_proc->hh, pst_img_proc->sh, pst_img_proc->vh), m1);
					cv::inRange(mat_criterion, cv::Scalar(pst_img_proc->hl, pst_img_proc->sl, pst_img_proc->vl), cv::Scalar(179, pst_img_proc->sh, pst_img_proc->vh), m2);
					cv::bitwise_or(m1, m2, mask);
				}
				else {
					cv::inRange(mat_criterion, cv::Scalar(pst_img_proc->hl, pst_img_proc->sl, pst_img_proc->vl), cv::Scalar(pst_img_proc->hh, pst_img_proc->sh, pst_img_proc->vh), mask);
				}
							
				pst_img_proc->target_chk_base_count = cv::countNonZero(mask);
			}
			st_mon2.flg_dispDragging = false;
		}break;
		case AUXPOL_ID_MON2_PB_SET_ROI: {
			rc_mat_roi_work_disp = set_work_roi(false);//false:マウス選択範囲の領域を設定
			st_mon2.flg_dispDragging = false;
		}break;
		case AUXPOL_ID_MON2_CB_DISP_ROI_CRITERION: {
			if (BST_CHECKED == SendMessage(st_mon2.hctrl[AUXPOL_ID_MON2_CB_DISP_ROI_CRITERION], BM_GETCHECK, 0, 0)) {
				st_mon2.flg_disp_cri_roi = true;
			}
			else {
				st_mon2.flg_disp_cri_roi = false;
			}
		}break;
		case AUXPOL_ID_MON2_CB_DISP_ROI_WORK: {
			if (BST_CHECKED == SendMessage(st_mon2.hctrl[AUXPOL_ID_MON2_CB_DISP_ROI_WORK], BM_GETCHECK, 0, 0)) {
				st_mon2.flg_disp_work_roi = true;
			}
			else {
				st_mon2.flg_disp_work_roi = false;
			}
		}break;
		default:
			return DefWindowProc(hWnd, msg, wp, lp);
		}
	}break;

	case WM_LBUTTONDOWN:{
		st_mon2.flg_dispDragging = true;
		st_mon2.m_isDragging = true;
		st_mon2.m_startPt = { LOWORD(lp), HIWORD(lp) };
		st_mon2.m_currPt = st_mon2.m_startPt;
	}break;
	
	case WM_MOUSEMOVE:{
		if (st_mon2.m_isDragging) {
			st_mon2.m_currPt = { LOWORD(lp), HIWORD(lp) };
			int x = std::min((int)st_mon2.m_startPt.x, (int)st_mon2.m_currPt.x);
			int y = std::min((int)st_mon2.m_startPt.y, (int)st_mon2.m_currPt.y);
			int w = abs((int)st_mon2.m_currPt.x - (int)st_mon2.m_startPt.x);
			int h = abs((int)st_mon2.m_currPt.y - (int)st_mon2.m_startPt.y);
			if (w > 2 && h > 2) st_mon2.rc_selected = cv::Rect(x, y, w, h);
		}
	}break;
	case WM_LBUTTONUP: {
		if (st_mon2.m_isDragging) {
			st_mon2.m_isDragging = false;
			int x = std::min((int)st_mon2.m_startPt.x, (int)st_mon2.m_currPt.x);
			int y = std::min((int)st_mon2.m_startPt.y, (int)st_mon2.m_currPt.y);
			int w = abs((int)st_mon2.m_currPt.x - (int)st_mon2.m_startPt.x);
			int h = abs((int)st_mon2.m_currPt.y - (int)st_mon2.m_startPt.y);
			if (w > 2 && h > 2) st_mon2.rc_selected = cv::Rect(x, y, w, h);
		}
	}break;
	case WM_RBUTTONDOWN: {
			st_mon2.flg_dispDragging = false;
	}break;
	case WM_HSCROLL: {
		int id = GetDlgCtrlID((HWND)lp);
		int pos = (int)SendMessage((HWND)lp, TBM_GETPOS, 0, 0);
		if (id == AUXPOL_ID_MON2_CTRL_BASE + AUXPOL_ID_MON2_SLIDER_HL) {
			pst_img_proc->hl = pos;monwos.str(L""); monwos << pst_img_proc->hl;
			SetWindowText(st_mon2.hctrl[AUXPOL_ID_MON2_STATIC_HL], monwos.str().c_str());
		}
		if (id == AUXPOL_ID_MON2_CTRL_BASE + AUXPOL_ID_MON2_SLIDER_HH) {
			pst_img_proc->hh = pos; monwos.str(L""); monwos << pst_img_proc->hh;
			SetWindowText(st_mon2.hctrl[AUXPOL_ID_MON2_STATIC_HH], monwos.str().c_str());
		}
		if (id == AUXPOL_ID_MON2_CTRL_BASE + AUXPOL_ID_MON2_SLIDER_SL) {
			pst_img_proc->sl = pos; monwos.str(L""); monwos << pst_img_proc->sl;
			SetWindowText(st_mon2.hctrl[AUXPOL_ID_MON2_STATIC_SL], monwos.str().c_str());
		}
		if (id == AUXPOL_ID_MON2_CTRL_BASE + AUXPOL_ID_MON2_SLIDER_SH) {
			pst_img_proc->sh = pos; monwos.str(L""); monwos << pst_img_proc->sh;
			SetWindowText(st_mon2.hctrl[AUXPOL_ID_MON2_STATIC_SH], monwos.str().c_str());
		}
		if (id == AUXPOL_ID_MON2_CTRL_BASE + AUXPOL_ID_MON2_SLIDER_VL) {
			pst_img_proc->vl = pos; monwos.str(L""); monwos << pst_img_proc->vl;
			SetWindowText(st_mon2.hctrl[AUXPOL_ID_MON2_STATIC_VL], monwos.str().c_str());
		}
		if (id == AUXPOL_ID_MON2_CTRL_BASE + AUXPOL_ID_MON2_SLIDER_VH) {
			pst_img_proc->vh = pos; monwos.str(L""); monwos << pst_img_proc->vh;
			SetWindowText(st_mon2.hctrl[AUXPOL_ID_MON2_STATIC_VH], monwos.str().c_str());
		}
		return 0;
	}
	case WM_PAINT: {

		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);

		break;
	}break;

	//case WM_ERASEBKGND:
	//	// 背景を描画
	//	return 0;

	case WM_DESTROY: {
		st_mon2.hwnd_mon = NULL;
		st_mon2.is_monitor_active = false;
		KillTimer(hWnd, AUXPOL_ID_MON2_TIMER);
	}break;
	default:
		return DefWindowProc(hWnd, msg, wp, lp);
	}
	return S_OK;
}

HWND COteAuxPol::open_monitor_wnd(HWND h_parent_wnd, int id) {

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
		wcex.lpszMenuName = TEXT("AUXPOL_MON1");
		wcex.lpszClassName = TEXT("AUXPOL_MON1");
		wcex.hIconSm = NULL;

		ATOM fb = RegisterClassExW(&wcex);

		st_mon1.hwnd_mon = CreateWindowW(TEXT("AUXPOL_MON1"), TEXT("AUXPOL_MON1"), WS_OVERLAPPEDWINDOW,
			AUXPOL_MON1_WND_X, AUXPOL_MON1_WND_Y, AUXPOL_MON1_WND_W, AUXPOL_MON1_WND_H,
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
	//	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wcex.hbrBackground = (HBRUSH)GetStockObject(LTGRAY_BRUSH);
		wcex.lpszMenuName = TEXT("AUXPOL_MON2");
		wcex.lpszClassName = TEXT("AUXPOL_MON2");
		wcex.hIconSm = NULL;

		ATOM fb = RegisterClassExW(&wcex);

		st_mon2.hwnd_mon = CreateWindowW(TEXT("AUXPOL_MON2"), TEXT("USB CAMERA"), WS_OVERLAPPEDWINDOW,
			AUXPOL_MON2_WND_X, AUXPOL_MON2_WND_Y, AUXPOL_MON2_WND_W, AUXPOL_MON2_WND_H,
			h_parent_wnd, nullptr, hInst, nullptr);
		show_monitor_wnd(id);
	}
	else
	{
		return NULL;
	};

	return NULL;
}
void COteAuxPol::close_monitor_wnd(int id) {
	if (id == BC_ID_MON1)
		DestroyWindow(st_mon1.hwnd_mon);
	else if (id == BC_ID_MON2)
		DestroyWindow(st_mon2.hwnd_mon);
	else;
	return;
}
void COteAuxPol::show_monitor_wnd(int id) {
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
void COteAuxPol::hide_monitor_wnd(int id) {
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
LRESULT CALLBACK COteAuxPol::PanelProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp) {

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
			set_item_chk_txt();
		}break;

		case IDC_TASK_ITEM_CHECK1:
		case IDC_TASK_ITEM_CHECK2:
		case IDC_TASK_ITEM_CHECK3:
		case IDC_TASK_ITEM_CHECK4:
		case IDC_TASK_ITEM_CHECK5:
		case IDC_TASK_ITEM_CHECK6:
		{
			if (inf.panel_func_id == IDC_TASK_FUNC_RADIO1) {

				if (pAuxAgInf->st_usb_cam.bgrMatFrame.empty()) break;

				if (LOWORD(wp) == IDC_TASK_ITEM_CHECK1) {
					st_mon2.disp_mode = AUXPOL_CODE_MON2_DISP_ROWMAT;
				}
				else if (LOWORD(wp) == IDC_TASK_ITEM_CHECK2) {
					st_mon2.disp_mode = AUXPOL_CODE_MON2_DISP_HSV_MASK;
				}
				else if (LOWORD(wp) == IDC_TASK_ITEM_CHECK3) {
				}
				else if (LOWORD(wp) == IDC_TASK_ITEM_CHECK4) {
				}
				else if (LOWORD(wp) == IDC_TASK_ITEM_CHECK5) {
				}
				else if (LOWORD(wp) == IDC_TASK_ITEM_CHECK6) {
					st_mon2.disp_mode = AUXPOL_CODE_MON2_DISP_DEFAULT;
				}

				for(int i = 0; i < BC_N_ACT_ITEM; i++) {
					if (LOWORD(wp) == IDC_TASK_ITEM_CHECK1 + i) continue;
					SendMessage(GetDlgItem(hDlg, IDC_TASK_ITEM_CHECK1 + i), BM_SETCHECK, BST_UNCHECKED, 0L);
				}	
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
			InvalidateRect(st_mon2.hwnd_mon, NULL, TRUE);
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
void COteAuxPol::msg2listview(wstring wstr) {

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

void COteAuxPol::set_PNLparam_value(float p1, float p2, float p3, float p4, float p5, float p6) {
	wstring wstr;
	wstr += std::to_wstring(p1); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT1), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p2); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT2), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p3); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT3), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p4); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT4), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p5); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT5), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p6); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT6), wstr.c_str());
}

//タブパネルのStaticテキストを設定
void COteAuxPol::set_panel_tip_txt() {
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
void COteAuxPol::set_func_pb_txt() {
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO1, L"SelImg");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO2, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO3, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO4, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO5, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO6, L"-");
	return;
}
//タブパネルのItem chkテキストを設定
void COteAuxPol::set_item_chk_txt() {
	if(inf.panel_func_id == IDC_TASK_FUNC_RADIO1) {
		SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK1, L"BGRMat");
		SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK2, L"HSVMask");
		SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK3, L"");
		SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK4, L"");
		SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK5, L"");
		SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK6, L"default");
	}
	else {
		SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK1, L"-");
		SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK2, L"-");
		SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK3, L"-");
		SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK4, L"-");
		SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK5, L"-");
		SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK6, L"-");
	}
	return;
}

void COteAuxPol::clear_graphics() {
	if (pgraphic_img != NULL)		delete pgraphic_img;
	DeleteDC(st_mon2.hdc);
	return;
}

HRESULT COteAuxPol::setup_graphics(HWND hwnd) {
	clear_graphics();
	RECT rect; GetClientRect(hwnd, &rect);

	st_mon2.hdc = GetDC(hwnd);
	if (st_mon2.hdc == NULL) return E_FAIL;

	//グラフィックオブジェクト作成
	pgraphic_img = new Gdiplus::Graphics(st_mon2.hdc);
	
	if (pgraphic_img == NULL) return E_FAIL;
	pgraphic_img->SetSmoothingMode(SmoothingModeAntiAlias);
	pgraphic_img->SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
	pgraphic_img->SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);

	return S_OK;
}

void COteAuxPol::UpdateMon2(HWND hWnd, HDC hdc) {

	m_pOffscreenGraphics->Clear(Color(200, 200, 200));
	std::lock_guard<std::mutex> lock(pAuxAgInf->st_usb_cam.g_mtx);
	int width = pAuxAgInf->st_usb_cam.bgrMatFrame.cols / AUXPOL_CAMERA_DISP_RETIO;
	int height = pAuxAgInf->st_usb_cam.bgrMatFrame.rows / AUXPOL_CAMERA_DISP_RETIO;
	float h_text = 10.0f;

	if (!pAuxAgInf->st_usb_cam.bgrMatFrame.empty()) {
		// --- 手順1: Bitmapバッファへ順次描画 ---
		// カメラ画像
		Gdiplus::Bitmap bitmap(pAuxAgInf->st_usb_cam.bgrMatFrame.cols, pAuxAgInf->st_usb_cam.bgrMatFrame.rows,
			(int)pAuxAgInf->st_usb_cam.bgrMatFrame.step, PixelFormat24bppRGB, pAuxAgInf->st_usb_cam.bgrMatFrame.data);

		m_pOffscreenGraphics->DrawImage(&bitmap, 0, 0, width, height);

		//テキスト描画
		std::wstring timeStr = GetTimestamp();
		m_pOffscreenGraphics->DrawString(timeStr.c_str(), -1, m_pFont18.get(), PointF(5.0f, h_text), m_pWhiteBrush.get());

		//ROI描画
		if (st_mon2.flg_dispDragging) {
			monwos.str(L""); h_text += 25.0f;
			monwos << L"(" << st_mon2.rc_selected.x << L"," << st_mon2.rc_selected.y << L")" << L" (" << st_mon2.rc_selected.x + st_mon2.rc_selected.width << L"," << st_mon2.rc_selected.y + st_mon2.rc_selected.height << L")";
			m_pOffscreenGraphics->DrawString(monwos.str().c_str(), -1, m_pFont18.get(), PointF(5.0f, h_text), m_pGrayBrush.get());
			m_pOffscreenGraphics->DrawRectangle(m_pGrayPen.get(), st_mon2.rc_selected.x, st_mon2.rc_selected.y, st_mon2.rc_selected.width, st_mon2.rc_selected.height);
		}
		if(st_mon2.flg_disp_cri_roi) {
			monwos.str(L""); h_text += 25.0f;
			monwos << L"(" << rc_mat_roi_criterion.x << L"," << rc_mat_roi_criterion.y << L")" << L" (" << rc_mat_roi_criterion.x + rc_mat_roi_criterion.width<< L"," << rc_mat_roi_criterion.y + rc_mat_roi_criterion.height << L")" 
				<< L" HSV:[" << pst_img_proc->hCenter<< L"," << pst_img_proc->sCenter << L"," << pst_img_proc->vCenter << L"]"
				<< L" Cnt:" << pst_img_proc->whiteCountInCriterionRoi << L"(" << pst_img_proc->target_chk_base_count << L")";
			m_pOffscreenGraphics->DrawString(monwos.str().c_str(), -1, m_pFont18.get(), PointF(5.0f, h_text), m_pGreenBrush.get());
			m_pOffscreenGraphics->DrawRectangle(m_pGreenPen.get(), rc_mat_roi_criterion_disp.x, rc_mat_roi_criterion_disp.y, rc_mat_roi_criterion_disp.width, rc_mat_roi_criterion_disp.height);
		}
		if(st_mon2.flg_disp_work_roi) {
			
			monwos.str(L""); h_text += 25.0f;
			monwos << L"(" << rc_mat_roi_work.x << L"," << rc_mat_roi_work.y << L")" << L" (" << rc_mat_roi_work.x + rc_mat_roi_work.width << L"," << rc_mat_roi_work.y + rc_mat_roi_work.height << L")" ;
			m_pOffscreenGraphics->DrawString(monwos.str().c_str(), -1, m_pFont18.get(), PointF(5.0f, h_text), m_pMagentaBrush.get());
			m_pOffscreenGraphics->DrawRectangle(m_pMagentaPen.get(), rc_mat_roi_work_disp.x, rc_mat_roi_work_disp.y, rc_mat_roi_work_disp.width, rc_mat_roi_work_disp.height);
		}


	}

	if (st_mon2.disp_mode == AUXPOL_CODE_MON2_DISP_HSV_MASK){
		if (!pst_img_proc->hsvMat_mask.empty()) {
			// --- 手順1: Bitmapバッファへ順次描画 ---
			Gdiplus::Bitmap bitmap(pst_img_proc->hsvMat_mask.cols, pst_img_proc->hsvMat_mask.rows,
				(int)pst_img_proc->hsvMat_mask.step, PixelFormat24bppRGB, pst_img_proc->hsvMat_mask.data);
			m_pOffscreenGraphics->DrawImage(&bitmap, 0, height, width, height);
			
			//テキスト描画
			monwos.str(L"●");
			if (pst_img_proc->status & AUXPOL_CODE_IMG_PROC_ACTIVE) {
				if (pst_img_proc->is_target_detected)
					m_pOffscreenGraphics->DrawString(monwos.str().c_str(), -1, m_pFont36.get(), PointF(5.0f, 5.0f + height), m_pGreenBrush.get());
				else
					m_pOffscreenGraphics->DrawString(monwos.str().c_str(), -1, m_pFont36.get(), PointF(5.0f, 5.0f + height), m_pBlueBrush.get());
			}
			else {
				m_pOffscreenGraphics->DrawString(monwos.str().c_str(), -1, m_pFont36.get(), PointF(5.0f, 5.0f + height), m_pWhiteBrush.get());
			}
			
			//ROI描画
			if (st_mon2.flg_disp_cri_roi) {
				m_pOffscreenGraphics->DrawRectangle(m_pGreenPen.get(), rc_mat_roi_criterion_disp.x, rc_mat_roi_criterion_disp.y+ height, rc_mat_roi_criterion_disp.width, rc_mat_roi_criterion_disp.height);
			}
			if (st_mon2.flg_disp_work_roi) {
				m_pOffscreenGraphics->DrawRectangle(m_pMagentaPen.get(), rc_mat_roi_work_disp.x, rc_mat_roi_work_disp.y + height, rc_mat_roi_work_disp.width, rc_mat_roi_work_disp.height);
			}
		}
	}

	// --- 手順2: 完成したバッファを画面(HDC)へ一気に転送する ---
	pgraphic_img->DrawImage(m_pOffscreenBitmap.get(), 0, 0);

	return;
}

// スライダー作成補助
HWND COteAuxPol::CreateSlider(HWND parent, int id, int x, int y, int minV, int maxV, int initV) {
	HWND h = CreateWindowEx(0, TRACKBAR_CLASS, NULL, WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS | TBS_TOP | TBS_TOOLTIPS,
		x, y, 200, 30, parent, (HMENU)id, GetModuleHandle(NULL), NULL);
	SendMessage(h, TBM_SETRANGE, TRUE, MAKELONG(minV, maxV));
	SendMessage(h, TBM_SETPOS, TRUE, initV);
	SendMessage(h, TBM_SETPAGESIZE, 0, (LPARAM)1);
	return h;
}

// --- ROI内の最頻度値からHSV範囲を自動計算 ---
void COteAuxPol::HSV_autoCalibrate() {

	// 選択範囲が画像内にあるか確認して切り出し
	// 表示画面は元画像を縮小している
	int width = pAuxAgInf->st_usb_cam.hsvMatFrame.cols;
	int height = pAuxAgInf->st_usb_cam.hsvMatFrame.rows;
	//AND領域の矩形取得
	cv::Rect valid = rc_mat_roi_criterion & cv::Rect(0, 0, width, height);
	if (valid.width < 0 || valid.height < 0) return;
		
	// HSV各チャンネルに分離
	cv::Mat roiHsv = pAuxAgInf->st_usb_cam.hsvMatFrame(valid);
	std::vector<cv::Mat> channels;
	cv::split(roiHsv, channels);

	// 1. Hue の最頻値を求める (0-179 の範囲)
	pst_img_proc->hCenter = GetModeBinCenter(channels[0], 180, 10);

	// 2. Saturation の最頻値を求める (0-255 の範囲)
	pst_img_proc->sCenter = GetModeBinCenter(channels[1], 256, 10);

	// 3. Value の最頻値を求める (0-255 の範囲)
	pst_img_proc->vCenter = GetModeBinCenter(channels[2], 256, 10);

	// 4. 上下限値の設定 (±10)
	pst_img_proc->hl = pst_img_proc->hCenter - pst_img_proc->h_margin;
	pst_img_proc->hh = pst_img_proc->hCenter + pst_img_proc->h_margin;
	// Hueは環状なので、0-179の範囲に収める
	if (pst_img_proc->hl < 0) pst_img_proc->hl += 180;
	if (pst_img_proc->hl > 179) pst_img_proc->hl -= 180;

	pst_img_proc->sl = std::max(0, pst_img_proc->sCenter - pst_img_proc->s_margin);
	pst_img_proc->sh = std::min(255, pst_img_proc->sCenter + pst_img_proc->s_margin);

	pst_img_proc->vl = std::max(0, pst_img_proc->vCenter - pst_img_proc->v_margin);
	pst_img_proc->vh = std::min(255, pst_img_proc->vCenter + pst_img_proc->v_margin);

	// スライダーのつまみ位置を更新
	UpdateSliderPos();
	return;
}
void COteAuxPol::UpdateSliderPos() {
	SendMessage(st_mon2.hctrl[AUXPOL_ID_MON2_SLIDER_HL], TBM_SETPOS, TRUE, pst_img_proc->hl);
	SendMessage(st_mon2.hctrl[AUXPOL_ID_MON2_SLIDER_HH], TBM_SETPOS, TRUE, pst_img_proc->hh);
	SendMessage(st_mon2.hctrl[AUXPOL_ID_MON2_SLIDER_SL], TBM_SETPOS, TRUE, pst_img_proc->sl);
	SendMessage(st_mon2.hctrl[AUXPOL_ID_MON2_SLIDER_SH], TBM_SETPOS, TRUE, pst_img_proc->sh);
	SendMessage(st_mon2.hctrl[AUXPOL_ID_MON2_SLIDER_VL], TBM_SETPOS, TRUE, pst_img_proc->vl);
	SendMessage(st_mon2.hctrl[AUXPOL_ID_MON2_SLIDER_VH], TBM_SETPOS, TRUE, pst_img_proc->vh);

	monwos.str(L""); monwos << pst_img_proc->hl;
	SetWindowText(st_mon2.hctrl[AUXPOL_ID_MON2_STATIC_HL], monwos.str().c_str());
	monwos.str(L""); monwos << pst_img_proc->hh;
	SetWindowText(st_mon2.hctrl[AUXPOL_ID_MON2_STATIC_HH], monwos.str().c_str());
	monwos.str(L""); monwos << pst_img_proc->sl;
	SetWindowText(st_mon2.hctrl[AUXPOL_ID_MON2_STATIC_SL], monwos.str().c_str());
	monwos.str(L""); monwos << pst_img_proc->sh;
	SetWindowText(st_mon2.hctrl[AUXPOL_ID_MON2_STATIC_SH], monwos.str().c_str());
	monwos.str(L""); monwos << pst_img_proc->vl;
	SetWindowText(st_mon2.hctrl[AUXPOL_ID_MON2_STATIC_VL], monwos.str().c_str());
	monwos.str(L""); monwos << pst_img_proc->vh;
	SetWindowText(st_mon2.hctrl[AUXPOL_ID_MON2_STATIC_VH], monwos.str().c_str());
}
cv::Rect COteAuxPol::get_hsv_criterion(){

	cv::Rect rc_disp(
		AUXPOL_DEFAULT_ROI_X/AUXPOL_CAMERA_DISP_RETIO,
		AUXPOL_DEFAULT_ROI_Y/AUXPOL_CAMERA_DISP_RETIO,
		AUXPOL_DEFAULT_ROI_W/AUXPOL_CAMERA_DISP_RETIO,
		AUXPOL_DEFAULT_ROI_H/AUXPOL_CAMERA_DISP_RETIO
	);

	if (st_mon2.flg_dispDragging) {

		// 選択範囲が画像内にあるか確認して切り出し
		// 表示画面は元画像を縮小している
		int width_disp = pAuxAgInf->st_usb_cam.hsvMatFrame.cols / AUXPOL_CAMERA_DISP_RETIO;
		int height_disp = pAuxAgInf->st_usb_cam.hsvMatFrame.rows / AUXPOL_CAMERA_DISP_RETIO;
		//AND領域の矩形取得
		st_mon2.rc_selected = st_mon2.rc_selected & cv::Rect(0, 0, width_disp, height_disp);

		//画像内にある時は選択範囲
		if (!(st_mon2.rc_selected.width < 0 || st_mon2.rc_selected.height < 0)) {
			rc_disp = st_mon2.rc_selected;
		}
	}
	rc_mat_roi_criterion = { 
		rc_disp.x * AUXPOL_CAMERA_DISP_RETIO,
		rc_disp.y * AUXPOL_CAMERA_DISP_RETIO,
		rc_disp.width * AUXPOL_CAMERA_DISP_RETIO, 
		rc_disp.height * AUXPOL_CAMERA_DISP_RETIO 
	};

	return rc_disp;
}
cv::Rect COteAuxPol::set_work_roi(bool is_criterion_base) {
	cv::Rect rc_disp(
		AUXPOL_DEFAULT_ROI_X / AUXPOL_CAMERA_DISP_RETIO,
		AUXPOL_DEFAULT_ROI_Y / AUXPOL_CAMERA_DISP_RETIO,
		AUXPOL_DEFAULT_ROI_W / AUXPOL_CAMERA_DISP_RETIO,
		AUXPOL_DEFAULT_ROI_H / AUXPOL_CAMERA_DISP_RETIO
	); 

	if (is_criterion_base) {	//基準値設定後の自動設定
		rc_disp.x		= rc_mat_roi_criterion_disp.x - AUXPOL_MARGIN_WORK_ROI;
		rc_disp.y		= rc_mat_roi_criterion_disp.y	- AUXPOL_MARGIN_WORK_ROI;
		rc_disp.width	= rc_mat_roi_criterion_disp.width	+ AUXPOL_MARGIN_WORK_ROI * 2;
		rc_disp.height	= rc_mat_roi_criterion_disp.height	+ AUXPOL_MARGIN_WORK_ROI * 2;

		// 選択範囲が画像内にあるか確認して切り出し
		// 表示画面は元画像を縮小している
		int width_disp	= pAuxAgInf->st_usb_cam.hsvMatFrame.cols / AUXPOL_CAMERA_DISP_RETIO;
		int height_disp = pAuxAgInf->st_usb_cam.hsvMatFrame.rows / AUXPOL_CAMERA_DISP_RETIO;
		//AND領域の矩形取得
		rc_disp = rc_disp & cv::Rect(0, 0, width_disp, height_disp);

		//画像内にない時は基準判定エリア
		if (rc_disp.width < 0 || rc_disp.height < 0)
			rc_disp = rc_mat_roi_criterion_disp;
	}
	else {	//基準値設定後の自動設定
		rc_disp = st_mon2.rc_selected;
	}
	
	//画像選択範囲→カメラ画像の実際の範囲に変換
	rc_mat_roi_work = { 
		rc_disp.x * AUXPOL_CAMERA_DISP_RETIO,
		rc_disp.y * AUXPOL_CAMERA_DISP_RETIO,
		rc_disp.width * AUXPOL_CAMERA_DISP_RETIO, 
		rc_disp.height * AUXPOL_CAMERA_DISP_RETIO 
	};
	return rc_disp;
}

// --- ヒストグラムから最も頻度の高いレンジの中央値を求める補助関数 ---
int COteAuxPol::GetModeBinCenter(const cv::Mat& singleChannelMat, int maxVal, int binWidth) {
	if (singleChannelMat.empty()) return 0;

	// ビンの個数を計算 (例: 180 / 10 = 18個)
	int numBins = (maxVal + binWidth - 1) / binWidth;
	std::vector<int> hist(numBins, 0);

	// ヒストグラムの作成 (各ピクセルがどのビンに入るかカウント)
	for (int r = 0; r < singleChannelMat.rows; ++r) {
		for (int c = 0; c < singleChannelMat.cols; ++c) {
			int val = singleChannelMat.at<uchar>(r, c);
			int binIdx = val / binWidth;
			if (binIdx >= 0 && binIdx < numBins) {
				hist[binIdx]++;
			}
		}
	}

	// 最多頻度のビンを探す
	auto it = std::max_element(hist.begin(), hist.end());
	int maxBinIdx = (int)std::distance(hist.begin(), it);

	// ビンの中心値を返す (例: binIdx 0 なら 5)
	return (maxBinIdx * binWidth) + (binWidth / 2);
}