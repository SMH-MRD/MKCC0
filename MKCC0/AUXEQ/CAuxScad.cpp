#include "CAuxScad.h"
#include "resource.h"
#include "framework.h"
#include "AUXEQ_DEF.H"
#include "SmemAux.H"
#include "CComm.h"
#include <Commdlg.h>
#include <commctrl.h>
#include "CCamera.h"
#include "SWYSENSOR_DEF.H"
#include "CHelper.h"

//////////////////////////////////////////////////////////////////////////////
// CAuxScada

cv::Mat CAuxScada::m_img_src;  // 元画像

HWND       CAuxScada::m_cam_dlg_hndl=NULL;
uint32_t   CAuxScada::m_sel_img;
BOOL       CAuxScada::m_cursor;
POINT      CAuxScada::m_cursor_pt;
BOOL       CAuxScada::m_scale_img_src;
SCROLLINFO CAuxScada::m_scrlinf_img_src_h;
SCROLLINFO CAuxScada::m_scrlinf_img_src_v;
BOOL       CAuxScada::m_sway_pos;
BOOL       CAuxScada::m_target_roi[(int)(ENUM_IMAGE_MASK::E_MAX)];
BOOL       CAuxScada::m_target_pos[(int)(ENUM_IMAGE_MASK::E_MAX)];
BOOL       CAuxScada::m_target_contours[(int)(ENUM_IMAGE_MASK::E_MAX)];

#define SCAD_ID_DIALOG_TIMER        1900
#define SCAD_PRM_DIALOG_SCAN_MS     1000

extern ST_DEVICE_CODE g_my_code;
extern CTeliCamLib* pCamera;//GEカメラオブジェクトへのグローバルポインタ
extern APP_INFO g_app_info;
extern CONFIG_CAMERA g_config_camera;
extern CONFIG_IMGPROC g_config_imgproc;
extern CONFIG_COMMON g_config_common;
extern INFO_ADJUST_DATA g_infoajs_data;    // 調整情報データ
extern INFO_IMGPRC_DATA g_infoprc_data;    // 画像処理情報データ


ST_SCAD_MON1 CAuxScada::st_mon1;
ST_SCAD_MON2 CAuxScada::st_mon2;

//共有メモリ参照用定義
extern CSharedMem* pEnvInfObj;
extern CSharedMem* pAgentInfObj;
extern CSharedMem* pCsInfObj;
extern CSharedMem* pScadInfObj;

static LPST_AUX_ENV_INF		pEnvInf;
static LPST_AUX_CS_INF		pCsInf;
static LPST_AUX_AGENT_INF	pAgentInf;
static LPST_AUX_SCAD_INF	pScadInf;

/****************************************************************************/
/*   デフォルト関数											                    */
/****************************************************************************/
CAuxScada::CAuxScada() {
}

CAuxScada::~CAuxScada() {
	close();
}

HRESULT CAuxScada::initialize(LPVOID lpParam) {
	HRESULT hr = S_OK;
	//### 出力用共有メモリ取得
	out_size = sizeof(ST_AUX_SCAD_INF);
	if (OK_SHMEM != pScadInfObj->create_smem(SMEM_AUX_ENV_INF_NAME, sizeof(ST_AUX_ENV_INF), MUTEX_AUX_ENV_INF_NAME)) {
		return(FALSE);
	}
	set_outbuf(pScadInfObj->get_pMap());

	//### 入力用共有メモリ取得
	if (OK_SHMEM != pCsInfObj->create_smem(SMEM_AUX_CS_INF_NAME, sizeof(ST_AUX_CS_INF), MUTEX_AUX_CS_INF_NAME)) {
		return(FALSE);
	}
	if (OK_SHMEM != pAgentInfObj->create_smem(SMEM_AUX_AGENT_INF_NAME, sizeof(ST_AUX_AGENT_INF), MUTEX_AUX_AGENT_INF_NAME)) {
		return(FALSE);
	}
	if (OK_SHMEM != pScadInfObj->create_smem(SMEM_AUX_SCAD_INF_NAME, sizeof(ST_AUX_SCAD_INF), MUTEX_AUX_SCAD_INF_NAME)) {
		return(FALSE);
	}

	pEnvInf = (LPST_AUX_ENV_INF)(pEnvInfObj->get_pMap());
	pAgentInf = (LPST_AUX_AGENT_INF)(pAgentInfObj->get_pMap());
	pCsInf = (LPST_AUX_CS_INF)pCsInfObj->get_pMap();
	pScadInf = (LPST_AUX_SCAD_INF)pScadInfObj->get_pMap();

	if ((pEnvInf == NULL) || (pAgentInf == NULL) || (pCsInf == NULL) || (pScadInf == NULL))
		hr = S_FALSE;
	if (hr == S_FALSE) {
		wos.str(L""); wos << L"Initialize : SMEM NG"; msg2listview(wos.str());
		return hr;
	};

	//### 有効機能の設定
	//旋回ブレーキ
	int enable = (g_my_code.option >> 28) & 0x0F;
	slbrk_enable = enable;
	//LANIO
	enable = (g_my_code.option >> 24) & 0x0F;
	lanio_enable = enable;
	//振れセンサー
	enable = (g_my_code.option >> 20) & 0x0F;
	sway_sensor_enable = enable;
	//走行位置検出
	enable = (g_my_code.option >> 16) & 0x0F;
	gt_sensor_enable = enable;

	std::wostringstream msg;

	//----------------------------------------------------------------------------
	if (sway_sensor_enable) {
		// メンバー変数の初期化
		m_sel_img = 0;

		m_cursor = FALSE;
		m_cursor_pt.x = 0;
		m_cursor_pt.y = 0;

		m_scale_img_src = FALSE;
		m_scrlinf_img_src_h.cbSize = sizeof(SCROLLINFO);
		m_scrlinf_img_src_h.fMask = SIF_DISABLENOSCROLL | SIF_POS | SIF_RANGE | SIF_PAGE;
		m_scrlinf_img_src_h.nMin = 0;
		m_scrlinf_img_src_h.nMax = g_config_camera.basis.roi[(int)(ENUM_AXIS::X)].size - g_config_common.img_screen_layout.width;
		m_scrlinf_img_src_h.nPage = 1;
		m_scrlinf_img_src_h.nPos = 0;
		m_scrlinf_img_src_h.nTrackPos = 0;
		m_scrlinf_img_src_v.cbSize = sizeof(SCROLLINFO);
		m_scrlinf_img_src_v.fMask = SIF_DISABLENOSCROLL | SIF_POS | SIF_RANGE | SIF_PAGE;
		m_scrlinf_img_src_v.nMin = 0;
		m_scrlinf_img_src_v.nMax = g_config_camera.basis.roi[(int)(ENUM_AXIS::Y)].size - g_config_common.img_screen_layout.height;
		m_scrlinf_img_src_v.nPage = 1;
		m_scrlinf_img_src_v.nPos = 0;
		m_scrlinf_img_src_v.nTrackPos = 0;

		m_sway_pos = TRUE;
		for (int32_t i = 0; i < (int)(ENUM_IMAGE_MASK::E_MAX); i++) {
			m_target_roi[i] = TRUE;
			m_target_pos[i] = TRUE;
			m_target_contours[i] = TRUE;
		}
	}

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
	set_item_chk_txt();
	set_panel_tip_txt();
	//モニタ2 CB状態セット	
	if (st_mon2.hwnd_mon != NULL)
		SendMessage(GetDlgItem(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK1), BM_SETCHECK, BST_CHECKED, 0L);
	else
		SendMessage(GetDlgItem(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK1), BM_SETCHECK, BST_UNCHECKED, 0L);
	return hr;
}

HRESULT CAuxScada::routine_work(void* pObj) {

	if (inf.total_act % 20 == 0) {
		wos.str(L""); wos << inf.status << L":" << std::setfill(L'0') << std::setw(4) << inf.act_time;
		msg2host(wos.str());
	}
	input();
	parse();
	output();
	return S_OK;
}

int CAuxScada::input() {

	//----------------------------------------------------------------------------
// 共有データ初期化
	return S_OK;
}

int CAuxScada::parse() {

	HBITMAP      bmp;       // 画像(bitmapファイル)
	cv::Mat      img_proc;  // 処理画像
	cv::Mat      img_mask;  // マスク画像
	cv::Mat      img_disp;  // 表示画像
	
	std::wostringstream wostr;

	if (m_cam_dlg_hndl == NULL) {
		_RPTWN(_CRT_WARN, L"%s\n", L">>>[CScada::proc_main]<Error>Object pointer");
		return S_FALSE;
	}

	return S_OK;
}

int CAuxScada::output() {          //出力処理
	return S_OK;
}

int CAuxScada::close() {
	return 0;
}

HWND CAuxScada::create_dlg_wnd()
{
	if (m_cam_dlg_hndl == NULL) {
		m_cam_dlg_hndl = CreateDialog(inf.hInstance,
			MAKEINTRESOURCE(IDD_DIALOG_IMAGE_PROC),
			nullptr,
			reinterpret_cast<DLGPROC>(cb_dlg_wnd));
		// 画像ビットマップ
		MoveWindow(GetDlgItem(m_cam_dlg_hndl, IDC_STATIC_BMP_IMAGE),
            g_config_common.img_screen_layout.x0, g_config_common.img_screen_layout.y0,
            g_config_common.img_screen_layout.width, g_config_common.img_screen_layout.height,
			false);
		// 画像スクロールバー(水平)
		MoveWindow(GetDlgItem(m_cam_dlg_hndl, IDC_SCROLLBAR_IMAGE_H),
            g_config_common.img_screen_layout.x0, g_config_common.img_screen_layout.y0 + g_config_common.img_screen_layout.height,
            g_config_common.img_screen_layout.width, DISP_IMG_SCROLL_SIZE,
			false);
		// 画像スクロールバー(垂直)
		MoveWindow(GetDlgItem(m_cam_dlg_hndl, IDC_SCROLLBAR_IMAGE_V),
            g_config_common.img_screen_layout.x0 + g_config_common.img_screen_layout.width, g_config_common.img_screen_layout.y0,
			DISP_IMG_SCROLL_SIZE, g_config_common.img_screen_layout.height,
			false);

		ShowWindow(m_cam_dlg_hndl, SW_SHOW);

		SetTimer(m_cam_dlg_hndl, SCAD_ID_DIALOG_TIMER, SCAD_PRM_DIALOG_SCAN_MS, NULL);
	}

	return m_cam_dlg_hndl;
}


/// @brief ダイアログ画面用コールバック関数
/// @param
/// @return 
/// @note
/// 
static wostringstream wosstr;
LRESULT CALLBACK CAuxScada::cb_dlg_wnd(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
  
    INT wmId = LOWORD(wp);
    switch (msg) {
    case WM_INITDIALOG: {
        //----------------------------------------------------------------------------
        // 画像表示
        {
            // スクロールバー
            SetScrollInfo(GetDlgItem(hwnd, IDC_SCROLLBAR_IMAGE_H), SB_CTL, &m_scrlinf_img_src_h, TRUE);
            SetScrollInfo(GetDlgItem(hwnd, IDC_SCROLLBAR_IMAGE_V), SB_CTL, &m_scrlinf_img_src_v, TRUE);

            // カーソル表示
            (!m_cursor) ? SendMessage(GetDlgItem(hwnd, IDC_CHECK_CURSOR), BM_SETCHECK, BST_UNCHECKED, 0) :
                SendMessage(GetDlgItem(hwnd, IDC_CHECK_CURSOR), BM_SETCHECK, BST_CHECKED, 0);

            EnableWindow(GetDlgItem(hwnd, IDC_BUTTON_CURSOR_L), m_cursor);
            EnableWindow(GetDlgItem(hwnd, IDC_BUTTON_CURSOR_R), m_cursor);
            EnableWindow(GetDlgItem(hwnd, IDC_BUTTON_CURSOR_U), m_cursor);
            EnableWindow(GetDlgItem(hwnd, IDC_BUTTON_CURSOR_D), m_cursor);

            // 縮小表示
            (!m_scale_img_src) ? SendMessage(GetDlgItem(hwnd, IDC_CHECK_IMAGE_SCALE), BM_SETCHECK, BST_UNCHECKED, 0) :
                SendMessage(GetDlgItem(hwnd, IDC_CHECK_IMAGE_SCALE), BM_SETCHECK, BST_CHECKED, 0);

            // 振れ位置表示
            (!m_sway_pos) ? SendMessage(GetDlgItem(hwnd, IDC_CHECK_SWAY_POS), BM_SETCHECK, BST_UNCHECKED, 0) :
                SendMessage(GetDlgItem(hwnd, IDC_CHECK_SWAY_POS), BM_SETCHECK, BST_CHECKED, 0);

            // ROI表示
            (!m_target_roi[(int)(ENUM_IMAGE_MASK::MASK_1)]) ? SendMessage(GetDlgItem(hwnd, IDC_CHECK_TARGET_ROI_1), BM_SETCHECK, BST_UNCHECKED, 0) :
                SendMessage(GetDlgItem(hwnd, IDC_CHECK_TARGET_ROI_1), BM_SETCHECK, BST_CHECKED, 0);
            (!m_target_roi[(int)(ENUM_IMAGE_MASK::MASK_2)]) ? SendMessage(GetDlgItem(hwnd, IDC_CHECK_TARGET_ROI_2), BM_SETCHECK, BST_UNCHECKED, 0) :
                SendMessage(GetDlgItem(hwnd, IDC_CHECK_TARGET_ROI_2), BM_SETCHECK, BST_CHECKED, 0);

            // 検出位置表示
            (!m_target_pos[(int)(ENUM_IMAGE_MASK::MASK_1)]) ? SendMessage(GetDlgItem(hwnd, IDC_CHECK_TARGET_POS_1), BM_SETCHECK, BST_UNCHECKED, 0) :
                SendMessage(GetDlgItem(hwnd, IDC_CHECK_TARGET_POS_1), BM_SETCHECK, BST_CHECKED, 0);
            (!m_target_pos[(int)(ENUM_IMAGE_MASK::MASK_2)]) ? SendMessage(GetDlgItem(hwnd, IDC_CHECK_TARGET_POS_2), BM_SETCHECK, BST_UNCHECKED, 0) :
                SendMessage(GetDlgItem(hwnd, IDC_CHECK_TARGET_POS_2), BM_SETCHECK, BST_CHECKED, 0);

            // 輪郭表示
            (!m_target_contours[(int)(ENUM_IMAGE_MASK::MASK_1)]) ? SendMessage(GetDlgItem(hwnd, IDC_CHECK_TARGET_CONTOURS_1), BM_SETCHECK, BST_UNCHECKED, 0) :
                SendMessage(GetDlgItem(hwnd, IDC_CHECK_TARGET_CONTOURS_1), BM_SETCHECK, BST_CHECKED, 0);
            (!m_target_contours[(int)(ENUM_IMAGE_MASK::MASK_2)]) ? SendMessage(GetDlgItem(hwnd, IDC_CHECK_TARGET_CONTOURS_2), BM_SETCHECK, BST_UNCHECKED, 0) :
                SendMessage(GetDlgItem(hwnd, IDC_CHECK_TARGET_CONTOURS_2), BM_SETCHECK, BST_CHECKED, 0);
        }

        //----------------------------------------------------------------------------
        // HSV
        // マスク画像1
        {
            HWND         wnd_hndl;
            wosstr.str(L"");

            // 色相H(Low)
            wnd_hndl = GetDlgItem(hwnd, IDC_SLIDER_H1_LOW);
            SendMessage(wnd_hndl, TBM_SETRANGE, TRUE, MAKELPARAM(IMAGE_HSV_H_MIN, IMAGE_HSV_H_MAX)); // レンジを指定
            SendMessage(wnd_hndl, TBM_SETTICFREQ, 20, 0);                                            // 目盛りの増分
            SendMessage(wnd_hndl, TBM_SETPOS, TRUE, g_config_imgproc.mask[(int)(ENUM_IMAGE_MASK::MASK_1)].hsv_l[(int)(ENUM_HSV_MODEL::H)]);   // 位置の設定
            SendMessage(wnd_hndl, TBM_SETPAGESIZE, 0, 1);                                            // クリック時の移動量
            wnd_hndl = GetDlgItem(hwnd, IDC_STATIC_VAL_H1_LOW); wosstr.str(L"");
            wosstr << g_config_imgproc.mask[(int)ENUM_IMAGE_MASK::MASK_1].hsv_l[(int)ENUM_HSV_MODEL::H];
            SetWindowText(wnd_hndl, wosstr.str().c_str());

            // 色相H(Upp)
            wnd_hndl = GetDlgItem(hwnd, IDC_SLIDER_H1_UPP);
            SendMessage(wnd_hndl, TBM_SETRANGE, TRUE, MAKELPARAM(IMAGE_HSV_H_MIN, IMAGE_HSV_H_MAX)); // レンジを指定
            SendMessage(wnd_hndl, TBM_SETTICFREQ, 20, 0);                                            // 目盛りの増分
            SendMessage(wnd_hndl, TBM_SETPOS, TRUE, g_config_imgproc.mask[(int)(ENUM_IMAGE_MASK::MASK_1)].hsv_u[(int)(ENUM_HSV_MODEL::H)]);   // 位置の設定
            SendMessage(wnd_hndl, TBM_SETPAGESIZE, 0, 1);                                            // クリック時の移動量
            wnd_hndl = GetDlgItem(hwnd, IDC_STATIC_VAL_H1_UPP); wosstr.str(L"");
            wosstr << g_config_imgproc.mask[(int)ENUM_IMAGE_MASK::MASK_1].hsv_u[(int)ENUM_HSV_MODEL::H];
            SetWindowText(wnd_hndl, wosstr.str().c_str());

            // 彩度S(Low)
            wnd_hndl = GetDlgItem(hwnd, IDC_SLIDER_S1_LOW);
            SendMessage(wnd_hndl, TBM_SETRANGE, TRUE, MAKELPARAM(IMAGE_HSV_S_MIN, IMAGE_HSV_S_MAX)); // レンジを指定
            SendMessage(wnd_hndl, TBM_SETTICFREQ, 25, 0);                                            // 目盛りの増分
            SendMessage(wnd_hndl, TBM_SETPOS, TRUE, g_config_imgproc.mask[(int)(ENUM_IMAGE_MASK::MASK_1)].hsv_l[(int)(ENUM_HSV_MODEL::S)]);   // 位置の設定
            SendMessage(wnd_hndl, TBM_SETPAGESIZE, 0, 1);                                            // クリック時の移動量
            wnd_hndl = GetDlgItem(hwnd, IDC_STATIC_VAL_S1_LOW); wosstr.str(L"");
            wosstr << g_config_imgproc.mask[(int)ENUM_IMAGE_MASK::MASK_1].hsv_l[(int)ENUM_HSV_MODEL::S];
            SetWindowText(wnd_hndl, wosstr.str().c_str());

            // 彩度S(Upp)
            wnd_hndl = GetDlgItem(hwnd, IDC_SLIDER_S1_UPP);
            SendMessage(wnd_hndl, TBM_SETRANGE, TRUE, MAKELPARAM(IMAGE_HSV_S_MIN, IMAGE_HSV_S_MAX)); // レンジを指定
            SendMessage(wnd_hndl, TBM_SETTICFREQ, 25, 0);                                            // 目盛りの増分
            SendMessage(wnd_hndl, TBM_SETPOS, TRUE, g_config_imgproc.mask[(int)(ENUM_IMAGE_MASK::MASK_1)].hsv_u[(int)(ENUM_HSV_MODEL::S)]);   // 位置の設定
            SendMessage(wnd_hndl, TBM_SETPAGESIZE, 0, 1);                                            // クリック時の移動量
            wnd_hndl = GetDlgItem(hwnd, IDC_STATIC_VAL_S1_UPP); wosstr.str(L"");
            wosstr << g_config_imgproc.mask[(int)ENUM_IMAGE_MASK::MASK_1].hsv_u[(int)ENUM_HSV_MODEL::S];
            SetWindowText(wnd_hndl, wosstr.str().c_str());

            // 明度V(Low)
            wnd_hndl = GetDlgItem(hwnd, IDC_SLIDER_V1_LOW);
            SendMessage(wnd_hndl, TBM_SETRANGE, TRUE, MAKELPARAM(IMAGE_HSV_V_MIN, IMAGE_HSV_V_MAX)); // レンジを指定
            SendMessage(wnd_hndl, TBM_SETTICFREQ, 25, 0);                                            // 目盛りの増分
            SendMessage(wnd_hndl, TBM_SETPOS, TRUE, g_config_imgproc.mask[(int)(ENUM_IMAGE_MASK::MASK_1)].hsv_l[(int)(ENUM_HSV_MODEL::V)]);   // 位置の設定
            SendMessage(wnd_hndl, TBM_SETPAGESIZE, 0, 1);                                            // クリック時の移動量
            wnd_hndl = GetDlgItem(hwnd, IDC_STATIC_VAL_V1_LOW); wosstr.str(L"");
            wosstr << g_config_imgproc.mask[(int)ENUM_IMAGE_MASK::MASK_1].hsv_l[(int)ENUM_HSV_MODEL::V];
            SetWindowText(wnd_hndl, wosstr.str().c_str());

            // 明度V(Upp)
            wnd_hndl = GetDlgItem(hwnd, IDC_SLIDER_V1_UPP);
            SendMessage(wnd_hndl, TBM_SETRANGE, TRUE, MAKELPARAM(IMAGE_HSV_V_MIN, IMAGE_HSV_V_MAX)); // レンジを指定
            SendMessage(wnd_hndl, TBM_SETTICFREQ, 25, 0);                                            // 目盛りの増分
            SendMessage(wnd_hndl, TBM_SETPOS, TRUE, g_config_imgproc.mask[(int)(ENUM_IMAGE_MASK::MASK_1)].hsv_u[(int)(ENUM_HSV_MODEL::V)]);   // 位置の設定
            SendMessage(wnd_hndl, TBM_SETPAGESIZE, 0, 1);                                            // クリック時の移動量
            wnd_hndl = GetDlgItem(hwnd, IDC_STATIC_VAL_V1_UPP); wosstr.str(L"");
            wosstr << g_config_imgproc.mask[(int)ENUM_IMAGE_MASK::MASK_1].hsv_u[(int)ENUM_HSV_MODEL::V];
            SetWindowText(wnd_hndl, wosstr.str().c_str());
        }
        // マスク画像2
        {
            HWND         wnd_hndl;
            wosstr.str(L"");

            // 色相H(Low)
            wnd_hndl = GetDlgItem(hwnd, IDC_SLIDER_H2_LOW);
            SendMessage(wnd_hndl, TBM_SETRANGE, TRUE, MAKELPARAM(IMAGE_HSV_H_MIN, IMAGE_HSV_H_MAX)); // レンジを指定
            SendMessage(wnd_hndl, TBM_SETTICFREQ, 20, 0);                                            // 目盛りの増分
            SendMessage(wnd_hndl, TBM_SETPOS, TRUE, g_config_imgproc.mask[(int)(ENUM_IMAGE_MASK::MASK_2)].hsv_l[(int)(ENUM_HSV_MODEL::H)]);   // 位置の設定
            SendMessage(wnd_hndl, TBM_SETPAGESIZE, 0, 1);                                            // クリック時の移動量
            wnd_hndl = GetDlgItem(hwnd, IDC_STATIC_VAL_H2_LOW); wosstr.str(L"");
            wosstr << g_config_imgproc.mask[(int)ENUM_IMAGE_MASK::MASK_2].hsv_l[(int)ENUM_HSV_MODEL::H];
            SetWindowText(wnd_hndl, wosstr.str().c_str());

            // 色相H(Upp)
            wnd_hndl = GetDlgItem(hwnd, IDC_SLIDER_H2_UPP);
            SendMessage(wnd_hndl, TBM_SETRANGE, TRUE, MAKELPARAM(IMAGE_HSV_H_MIN, IMAGE_HSV_H_MAX)); // レンジを指定
            SendMessage(wnd_hndl, TBM_SETTICFREQ, 20, 0);                                            // 目盛りの増分
            SendMessage(wnd_hndl, TBM_SETPOS, TRUE, g_config_imgproc.mask[(int)(ENUM_IMAGE_MASK::MASK_2)].hsv_u[(int)(ENUM_HSV_MODEL::H)]);   // 位置の設定
            SendMessage(wnd_hndl, TBM_SETPAGESIZE, 0, 1);                                            // クリック時の移動量
            wnd_hndl = GetDlgItem(hwnd, IDC_STATIC_VAL_H2_UPP); wosstr.str(L"");
            wosstr << g_config_imgproc.mask[(int)ENUM_IMAGE_MASK::MASK_2].hsv_u[(int)ENUM_HSV_MODEL::H];
            SetWindowText(wnd_hndl, wosstr.str().c_str());
 
            // 彩度S(Low)
            wnd_hndl = GetDlgItem(hwnd, IDC_SLIDER_S2_LOW);
            SendMessage(wnd_hndl, TBM_SETRANGE, TRUE, MAKELPARAM(IMAGE_HSV_S_MIN, IMAGE_HSV_S_MAX)); // レンジを指定
            SendMessage(wnd_hndl, TBM_SETTICFREQ, 25, 0);                                            // 目盛りの増分
            SendMessage(wnd_hndl, TBM_SETPOS, TRUE, g_config_imgproc.mask[(int)(ENUM_IMAGE_MASK::MASK_2)].hsv_l[(int)(ENUM_HSV_MODEL::S)]);   // 位置の設定
            SendMessage(wnd_hndl, TBM_SETPAGESIZE, 0, 1);                                            // クリック時の移動量
            wnd_hndl = GetDlgItem(hwnd, IDC_STATIC_VAL_S2_LOW); wosstr.str(L"");
            wosstr << g_config_imgproc.mask[(int)ENUM_IMAGE_MASK::MASK_2].hsv_l[(int)ENUM_HSV_MODEL::S];
            SetWindowText(wnd_hndl, wosstr.str().c_str());

            // 彩度S(Upp)
            wnd_hndl = GetDlgItem(hwnd, IDC_SLIDER_S2_UPP);
            SendMessage(wnd_hndl, TBM_SETRANGE, TRUE, MAKELPARAM(IMAGE_HSV_S_MIN, IMAGE_HSV_S_MAX)); // レンジを指定
            SendMessage(wnd_hndl, TBM_SETTICFREQ, 25, 0);                                            // 目盛りの増分
            SendMessage(wnd_hndl, TBM_SETPOS, TRUE, g_config_imgproc.mask[(int)(ENUM_IMAGE_MASK::MASK_2)].hsv_u[(int)(ENUM_HSV_MODEL::S)]);   // 位置の設定
            SendMessage(wnd_hndl, TBM_SETPAGESIZE, 0, 1);                                            // クリック時の移動量
            wnd_hndl = GetDlgItem(hwnd, IDC_STATIC_VAL_S2_UPP); wosstr.str(L"");
            wosstr << g_config_imgproc.mask[(int)ENUM_IMAGE_MASK::MASK_2].hsv_u[(int)ENUM_HSV_MODEL::S];
            SetWindowText(wnd_hndl, wosstr.str().c_str());
 
            // 明度V(Low)
            wnd_hndl = GetDlgItem(hwnd, IDC_SLIDER_V2_LOW);
            SendMessage(wnd_hndl, TBM_SETRANGE, TRUE, MAKELPARAM(IMAGE_HSV_V_MIN, IMAGE_HSV_V_MAX)); // レンジを指定
            SendMessage(wnd_hndl, TBM_SETTICFREQ, 25, 0);                                            // 目盛りの増分
            SendMessage(wnd_hndl, TBM_SETPOS, TRUE, g_config_imgproc.mask[(int)(ENUM_IMAGE_MASK::MASK_2)].hsv_l[(int)(ENUM_HSV_MODEL::V)]);   // 位置の設定
            SendMessage(wnd_hndl, TBM_SETPAGESIZE, 0, 1);                                            // クリック時の移動量
            wnd_hndl = GetDlgItem(hwnd, IDC_STATIC_VAL_V2_LOW); wosstr.str(L"");
            wosstr << g_config_imgproc.mask[(int)ENUM_IMAGE_MASK::MASK_2].hsv_l[(int)ENUM_HSV_MODEL::V];
            SetWindowText(wnd_hndl, wosstr.str().c_str());
 
            // 明度V(Upp)
            wnd_hndl = GetDlgItem(hwnd, IDC_SLIDER_V2_UPP);
            SendMessage(wnd_hndl, TBM_SETRANGE, TRUE, MAKELPARAM(IMAGE_HSV_V_MIN, IMAGE_HSV_V_MAX)); // レンジを指定
            SendMessage(wnd_hndl, TBM_SETTICFREQ, 25, 0);                                            // 目盛りの増分
            SendMessage(wnd_hndl, TBM_SETPOS, TRUE, g_config_imgproc.mask[(int)(ENUM_IMAGE_MASK::MASK_2)].hsv_u[(int)(ENUM_HSV_MODEL::V)]);   // 位置の設定
            SendMessage(wnd_hndl, TBM_SETPAGESIZE, 0, 1);                                            // クリック時の移動量
            wnd_hndl = GetDlgItem(hwnd, IDC_STATIC_VAL_V2_UPP); wosstr.str(L"");
            wosstr << g_config_imgproc.mask[(int)ENUM_IMAGE_MASK::MASK_2].hsv_u[(int)ENUM_HSV_MODEL::V];
            SetWindowText(wnd_hndl, wosstr.str().c_str());
        }
 
        //----------------------------------------------------------------------------
        // 画像選択
        {
            LPCTSTR str_img_row = TEXT("元画像");
            LPCTSTR str_img_mask[] = { TEXT("ﾏｽｸ画像1"), TEXT("ﾏｽｸ画像2") };
            HWND    wnd_hndl = GetDlgItem(hwnd, IDC_COMBO_IMAGE);

            SendMessage(wnd_hndl, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(str_img_row));
            for (uint32_t i = 0; i < (int)(ENUM_IMAGE_MASK::E_MAX); i++) {
                if (g_config_imgproc.mask[i].valid) {
                    SendMessage(wnd_hndl, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(str_img_mask[i]));
                }
            }
            // ウインドウ生成時にはじめに表示するデータを指定
            WPARAM index = SendMessage(wnd_hndl, CB_FINDSTRINGEXACT, -1, reinterpret_cast<LPARAM>(str_img_row));
            SendMessage(wnd_hndl, CB_SETCURSEL, index, 0);
        }

        //----------------------------------------------------------------------------
        // ノイズフィルタ
        // フィルター1(ゴマ塩)
        {
            // 種類
            uint32_t     page_size = 1;
            LPCTSTR      str_item[] = { TEXT("なし"), TEXT("中央値フィルタ"), TEXT("オープニング処理") };
            HWND         wnd_hndl;
            wosstr.str(L"");

            wnd_hndl = GetDlgItem(hwnd, IDC_COMBO_NOISEFILTER1);
            for (uint32_t i = 0; i < (int)(ENUM_NOISE_FILTER1::E_MAX); i++) {
                SendMessage(wnd_hndl, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(str_item[i]));
            }
            SendMessage(wnd_hndl, CB_SETCURSEL, g_config_imgproc.filter[(int)(ENUM_NOISE_FILTER::FILTER_1)].type, 0);
            if ((g_config_imgproc.filter[(int)(ENUM_NOISE_FILTER::FILTER_1)].type == (int)(ENUM_NOISE_FILTER1::MEDIAN)) ||
                (g_config_imgproc.filter[(int)(ENUM_NOISE_FILTER::FILTER_1)].type == (int)(ENUM_NOISE_FILTER1::OPENNING))) {
                ShowWindow(GetDlgItem(hwnd, IDC_SLIDER_NOISEFILTER1), SW_SHOW);
                ShowWindow(GetDlgItem(hwnd, IDC_STATIC_VAL_NOISEFILTER1), SW_SHOW);
                if (g_config_imgproc.filter[(int)(ENUM_NOISE_FILTER::FILTER_1)].type == (int)(ENUM_NOISE_FILTER1::MEDIAN)) {
                    page_size = 2;
                }
            }
            else {
                ShowWindow(GetDlgItem(hwnd, IDC_SLIDER_NOISEFILTER1), SW_HIDE);
                ShowWindow(GetDlgItem(hwnd, IDC_STATIC_VAL_NOISEFILTER1), SW_HIDE);
            }

            // フィルタ値
            wnd_hndl = GetDlgItem(hwnd, IDC_SLIDER_NOISEFILTER1);
            SendMessage(wnd_hndl, TBM_SETRANGE, TRUE, MAKELPARAM(1, 30));   // レンジを指定
            SendMessage(wnd_hndl, TBM_SETTICFREQ, 5, 0);                    // 目盛りの増分
            SendMessage(wnd_hndl, TBM_SETPOS, TRUE, g_config_imgproc.filter[(int)(ENUM_NOISE_FILTER::FILTER_1)].val);  // 位置の設定
            SendMessage(wnd_hndl, TBM_SETPAGESIZE, 0, page_size);           // クリック時の移動量
            wnd_hndl = GetDlgItem(hwnd, IDC_STATIC_VAL_NOISEFILTER1); wosstr.str(L"");
            wosstr << g_config_imgproc.filter[(int)(ENUM_NOISE_FILTER::FILTER_1)].val;
            SetWindowText(wnd_hndl, wosstr.str().c_str());
        }
        // フィルター2(穴埋め)
        {
            // 種類
            uint32_t     page_size = 1;
            LPCTSTR      str_item[] = { TEXT("なし"), TEXT("クロージング処理"), TEXT("中央値フィルタ") };
            HWND         wnd_hndl;
            wosstr.str(L"");

            wnd_hndl = GetDlgItem(hwnd, IDC_COMBO_NOISEFILTER2);
            for (uint32_t i = 0; i < (int)(ENUM_NOISE_FILTER2::E_MAX); i++) {
                SendMessage(wnd_hndl, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(str_item[i]));
            }
            SendMessage(wnd_hndl, CB_SETCURSEL, g_config_imgproc.filter[(int)(ENUM_NOISE_FILTER::FILTER_2)].type, 0);
            if (g_config_imgproc.filter[(int)(ENUM_NOISE_FILTER::FILTER_2)].type == (int)(ENUM_NOISE_FILTER2::CLOSING)) {
                ShowWindow(GetDlgItem(hwnd, IDC_SLIDER_NOISEFILTER2), SW_SHOW);
                ShowWindow(GetDlgItem(hwnd, IDC_STATIC_VAL_NOISEFILTER2), SW_SHOW);
            }
            else if (g_config_imgproc.filter[(int)(ENUM_NOISE_FILTER::FILTER_2)].type == (int)(ENUM_NOISE_FILTER2::MEDIAN)) {
                ShowWindow(GetDlgItem(hwnd, IDC_SLIDER_NOISEFILTER2), SW_SHOW);
                ShowWindow(GetDlgItem(hwnd, IDC_STATIC_VAL_NOISEFILTER2), SW_SHOW);
            }
            else {
                ShowWindow(GetDlgItem(hwnd, IDC_SLIDER_NOISEFILTER2), SW_HIDE);
                ShowWindow(GetDlgItem(hwnd, IDC_STATIC_VAL_NOISEFILTER2), SW_HIDE);
            }

            // フィルタ値
            wnd_hndl = GetDlgItem(hwnd, IDC_SLIDER_NOISEFILTER2);
            SendMessage(wnd_hndl, TBM_SETRANGE, TRUE, MAKELPARAM(1, 30));   // レンジを指定
            SendMessage(wnd_hndl, TBM_SETTICFREQ, 5, 0);                    // 目盛りの増分
            SendMessage(wnd_hndl, TBM_SETPOS, TRUE, g_config_imgproc.filter[(int)(ENUM_NOISE_FILTER::FILTER_2)].val);  // 位置の設定
            SendMessage(wnd_hndl, TBM_SETPAGESIZE, 0, page_size);           // クリック時の移動量

            wnd_hndl = GetDlgItem(hwnd, IDC_STATIC_VAL_NOISEFILTER2);
            wosstr << g_config_imgproc.filter[(int)(ENUM_NOISE_FILTER::FILTER_2)].val;
            SetWindowText(wnd_hndl, wosstr.str().c_str());
        }

        //----------------------------------------------------------------------------
        // カメラ露光時間
        {
            HWND         wnd_hndl;
            wosstr.str(L"");

            wnd_hndl = GetDlgItem(hwnd, IDC_SLIDER_CAMERA_EXPOSURE); wosstr.str(L"");
            SendMessage(wnd_hndl, TBM_SETRANGEMIN, TRUE, (int)(g_config_camera.expstime.val_min)); // レンジを指定
            SendMessage(wnd_hndl, TBM_SETRANGEMAX, TRUE, (int)(g_config_camera.expstime.val_max)); // レンジを指定
            SendMessage(wnd_hndl, TBM_SETTICFREQ, 1000, 0);                                             // 目盛りの増分
            SendMessage(wnd_hndl, TBM_SETPOS, TRUE, (int) (g_infoprc_data.exps_time));        // 位置の設定
            SendMessage(wnd_hndl, TBM_SETPAGESIZE, 0, 1);                                               // クリック時の移動量
            if (g_config_camera.expstime.auto_control) {
                SendMessage(GetDlgItem(hwnd, IDC_CHECK_CAMERA_EXPOSURE), BM_SETCHECK, BST_UNCHECKED, 0);
                ShowWindow(GetDlgItem(hwnd, IDC_SLIDER_CAMERA_EXPOSURE), SW_HIDE);
            }
            else {
                SendMessage(GetDlgItem(hwnd, IDC_CHECK_CAMERA_EXPOSURE), BM_SETCHECK, BST_CHECKED, 0);
                ShowWindow(GetDlgItem(hwnd, IDC_SLIDER_CAMERA_EXPOSURE), SW_SHOW);
            }

            wnd_hndl = GetDlgItem(hwnd, IDC_STATIC_VAL_CAMERA_EXPOSURE);
            wosstr <<static_cast<int>(g_infoprc_data.exps_time);
            SetWindowText(wnd_hndl, wosstr.str().c_str());

            if (g_config_common.img_source_camera) {
                EnableWindow(GetDlgItem(hwnd, IDC_STATIC_TITLE_CAMERA_EXPOSURE), TRUE);
                EnableWindow(GetDlgItem(hwnd, IDC_CHECK_CAMERA_EXPOSURE), TRUE);
                EnableWindow(GetDlgItem(hwnd, IDC_SLIDER_CAMERA_EXPOSURE), TRUE);
                EnableWindow(GetDlgItem(hwnd, IDC_STATIC_VAL_CAMERA_EXPOSURE), TRUE);
            }
            else {
                EnableWindow(GetDlgItem(hwnd, IDC_STATIC_TITLE_CAMERA_EXPOSURE), FALSE);
                EnableWindow(GetDlgItem(hwnd, IDC_CHECK_CAMERA_EXPOSURE), FALSE);
                EnableWindow(GetDlgItem(hwnd, IDC_SLIDER_CAMERA_EXPOSURE), FALSE);
                EnableWindow(GetDlgItem(hwnd, IDC_STATIC_VAL_CAMERA_EXPOSURE), FALSE);
            }
        }

        //----------------------------------------------------------------------------
        // ROI有効
        {
            HWND wnd_hndl;

            wnd_hndl = GetDlgItem(hwnd, IDC_CHECK_ROI);
            (g_config_imgproc.roi.valid) ? SendMessage(wnd_hndl, BM_SETCHECK, BST_CHECKED, 0) :
                SendMessage(wnd_hndl, BM_SETCHECK, BST_UNCHECKED, 0);
        }

        //----------------------------------------------------------------------------
        // 吊点～ターゲット間距離
        {
            HWND         wnd_hndl;
 
            g_infoajs_data.target_distance = EXTN_TARGETDIST_MIN;
            wnd_hndl = GetDlgItem(hwnd, IDC_SLIDER_TARGET_LEN);
            SendMessage(wnd_hndl, TBM_SETRANGEMIN, TRUE, (UINT)EXTN_TARGETDIST_MIN);                    // レンジを指定
            SendMessage(wnd_hndl, TBM_SETRANGEMAX, TRUE, (UINT)EXTN_TARGETDIST_MAX);                    // レンジを指定
            SendMessage(wnd_hndl, TBM_SETTICFREQ, 1000, 0);                                             // 目盛りの増分
            SendMessage(wnd_hndl, TBM_SETPOS, TRUE, static_cast<UINT>(g_infoajs_data.target_distance)); // 位置の設定
            SendMessage(wnd_hndl, TBM_SETPAGESIZE, 0, 1);                                               // クリック時の移動量
            if (g_infoajs_data.target_distance_fixed) {
                SendMessage(GetDlgItem(hwnd, IDC_CHECK_TARGET_LEN), BM_SETCHECK, BST_CHECKED, 0);
                ShowWindow(GetDlgItem(hwnd, IDC_SLIDER_TARGET_LEN), SW_SHOW);
            }
            else
            {
                SendMessage(GetDlgItem(hwnd, IDC_CHECK_TARGET_LEN), BM_SETCHECK, BST_UNCHECKED, 0);
                ShowWindow(GetDlgItem(hwnd, IDC_SLIDER_TARGET_LEN), SW_HIDE);
            }
            wosstr.str(L"");
            wosstr <<(UINT)(g_infoajs_data.target_distance);
            SetWindowText(GetDlgItem(hwnd, IDC_STATIC_VAL_TARGET_LEN), wosstr.str().c_str());
        }
        //----------------------------------------------------------------------------
        // カメラ情報
        {
             SetWindowText(GetDlgItem(hwnd, IDC_STATIC_IMAGE_STAT), L"-");
             SetWindowText(GetDlgItem(hwnd, IDC_STATIC_IMG_GRAB_TIME), L"-");
        }

        //----------------------------------------------------------------------------
        // 傾斜計情報
        {
            wosstr.str(L"");
         
            SetWindowText(GetDlgItem(hwnd, IDC_STATIC_TILT_STAT), L"-");
            SetWindowText(GetDlgItem(hwnd, IDC_STATIC_TILT_DEG_X), L"-");
            SetWindowText(GetDlgItem(hwnd, IDC_STATIC_TILT_DEG_Y), L"-");
            SetWindowText(GetDlgItem(hwnd, IDC_STATIC_TILT_RAD_X), L"-");
            SetWindowText(GetDlgItem(hwnd, IDC_STATIC_TILT_RAD_Y), L"-");
            SetWindowText(GetDlgItem(hwnd, IDC_STATIC_TILT_SPD_DEG_X), L"-");
            SetWindowText(GetDlgItem(hwnd, IDC_STATIC_TILT_SPD_DEG_Y), L"-");
            SetWindowText(GetDlgItem(hwnd, IDC_STATIC_TILT_SPD_RAD_X), L"-");
            SetWindowText(GetDlgItem(hwnd, IDC_STATIC_TILT_SPD_RAD_Y), L"-");
        }

        //----------------------------------------------------------------------------
        // 処理情報
        {
            SetWindowText(GetDlgItem(hwnd, IDC_STATIC_TARGET_POS_X_1), L"-");
            SetWindowText(GetDlgItem(hwnd, IDC_STATIC_TARGET_POS_Y_1), L"-");
            SetWindowText(GetDlgItem(hwnd, IDC_STATIC_TARGET_POS_X_2), L"-");
            SetWindowText(GetDlgItem(hwnd, IDC_STATIC_TARGET_POS_Y_2), L"-");
            SetWindowText(GetDlgItem(hwnd, IDC_STATIC_SWAY_X), L"-");
            SetWindowText(GetDlgItem(hwnd, IDC_STATIC_SWAY_Y), L"-");
            SetWindowText(GetDlgItem(hwnd, IDC_STATIC_SWAY_SPD_X), L"-");
            SetWindowText(GetDlgItem(hwnd, IDC_STATIC_SWAY_SPD_Y), L"-");
            SetWindowText(GetDlgItem(hwnd, IDC_STATIC_SWAY_ZERO_X), L"-");
             SetWindowText(GetDlgItem(hwnd, IDC_STATIC_SWAY_ZERO_Y), L"-");
        }
    }break;
    case WM_HSCROLL: {
        //lpのハンドルを見て処理実行判断
        //----------------------------------------------------------------------------
        // スクロールバー
        // 画像表示
        {
            HWND    wnd_hndl = GetDlgItem(hwnd, IDC_SCROLLBAR_IMAGE_H);
            int32_t pt = 20;

            if (wnd_hndl == reinterpret_cast<HWND>(lp)) {
                switch (LOWORD(wp)) {
                case SB_PAGELEFT:
                    if ((m_scrlinf_img_src_h.nPos -= pt) < m_scrlinf_img_src_h.nMin) {
                        m_scrlinf_img_src_h.nPos = m_scrlinf_img_src_h.nMin;
                    }
                    break;
                case SB_PAGERIGHT:
                    if ((m_scrlinf_img_src_h.nPos += pt) > m_scrlinf_img_src_h.nMax) {
                        m_scrlinf_img_src_h.nPos = m_scrlinf_img_src_h.nMax;
                    }
                    break;
                case SB_LINELEFT:
                    if ((m_scrlinf_img_src_h.nPos -= 1) < m_scrlinf_img_src_h.nMin) {
                        m_scrlinf_img_src_h.nPos = m_scrlinf_img_src_h.nMin;
                    }
                    break;
                case SB_LINERIGHT:
                    if ((m_scrlinf_img_src_h.nPos += 1) > m_scrlinf_img_src_h.nMax) {
                        m_scrlinf_img_src_h.nPos = m_scrlinf_img_src_h.nMax;
                    }
                    break;
                case SB_THUMBPOSITION:
                case SB_THUMBTRACK:
                    m_scrlinf_img_src_h.nPos = HIWORD(wp);
                    break;
                default:
                    break;
                }
                SetScrollInfo(wnd_hndl, SB_CTL, &m_scrlinf_img_src_h, TRUE);
            }
        }

        //----------------------------------------------------------------------------
        // HSV
        // マスク画像1
        {
            int32_t      pos;
            // 色相H
            if (GetDlgItem(hwnd, IDC_SLIDER_H1_LOW) == reinterpret_cast<HWND>(lp)) {
                pos = static_cast<int32_t>(SendMessage(GetDlgItem(hwnd, IDC_SLIDER_H1_LOW), TBM_GETPOS, 0, 0));
                wosstr.str(L""); wosstr << pos;
                SetWindowText(GetDlgItem(hwnd, IDC_STATIC_VAL_H1_LOW), wosstr.str().c_str());
                g_config_imgproc.mask[(int)(ENUM_IMAGE_MASK::MASK_1)].hsv_l[(int)(ENUM_HSV_MODEL::H)] = pos;
            }
            else if (GetDlgItem(hwnd, IDC_SLIDER_H1_UPP) == (HWND)lp)
            {
                pos = static_cast<int32_t>(SendMessage(GetDlgItem(hwnd, IDC_SLIDER_H1_UPP), TBM_GETPOS, 0, 0));
                wosstr.str(L""); wosstr << pos;
                SetWindowText(GetDlgItem(hwnd, IDC_STATIC_VAL_H1_UPP), wosstr.str().c_str());
                g_config_imgproc.mask[(int)(ENUM_IMAGE_MASK::MASK_1)].hsv_u[(int)(ENUM_HSV_MODEL::H)] = pos;
            }
            else {
                ;
            }
            // 彩度S
            if (GetDlgItem(hwnd, IDC_SLIDER_S1_LOW) == reinterpret_cast<HWND>(lp)) {
                pos = static_cast<int32_t>(SendMessage(GetDlgItem(hwnd, IDC_SLIDER_S1_LOW), TBM_GETPOS, 0, 0));
                wosstr.str(L""); wosstr << pos;
                SetWindowText(GetDlgItem(hwnd, IDC_STATIC_VAL_S1_LOW), wosstr.str().c_str());
                g_config_imgproc.mask[(int)(ENUM_IMAGE_MASK::MASK_1)].hsv_l[(int)(ENUM_HSV_MODEL::S)] = pos;
            }
            else if (GetDlgItem(hwnd, IDC_SLIDER_S1_UPP) == reinterpret_cast<HWND>(lp)) {
                pos = static_cast<int32_t>(SendMessage(GetDlgItem(hwnd, IDC_SLIDER_S1_UPP), TBM_GETPOS, 0, 0));
                wosstr.str(L""); wosstr << pos;
                SetWindowText(GetDlgItem(hwnd, IDC_STATIC_VAL_S1_UPP), wosstr.str().c_str());
                g_config_imgproc.mask[(int)(ENUM_IMAGE_MASK::MASK_1)].hsv_u[(int)(ENUM_HSV_MODEL::S)] = pos;
            }
            else {
                ;
            }
            // 明度V
            if (GetDlgItem(hwnd, IDC_SLIDER_V1_LOW) == reinterpret_cast<HWND>(lp)) {
                pos = static_cast<int32_t>(SendMessage(GetDlgItem(hwnd, IDC_SLIDER_V1_LOW), TBM_GETPOS, 0, 0));
                wosstr.str(L""); wosstr << pos;
                SetWindowText(GetDlgItem(hwnd, IDC_STATIC_VAL_V1_LOW), wosstr.str().c_str());
                g_config_imgproc.mask[(int)(ENUM_IMAGE_MASK::MASK_1)].hsv_l[(int)(ENUM_HSV_MODEL::V)] = pos;
            }
            else if (GetDlgItem(hwnd, IDC_SLIDER_V1_UPP) == reinterpret_cast<HWND>(lp)) {
                pos = static_cast<int32_t>(SendMessage(GetDlgItem(hwnd, IDC_SLIDER_V1_UPP), TBM_GETPOS, 0, 0));
                wosstr.str(L""); wosstr << pos;
                SetWindowText(GetDlgItem(hwnd, IDC_STATIC_VAL_V1_UPP), wosstr.str().c_str());
                g_config_imgproc.mask[(int)(ENUM_IMAGE_MASK::MASK_1)].hsv_u[(int)(ENUM_HSV_MODEL::V)] = pos;
            }
            else {
                ;
            }
        }
        // マスク画像2
        {
            int32_t      pos;

            // 色相H
            if (GetDlgItem(hwnd, IDC_SLIDER_H2_LOW) == reinterpret_cast<HWND>(lp)) {
                pos = static_cast<int32_t>(SendMessage(GetDlgItem(hwnd, IDC_SLIDER_H2_LOW), TBM_GETPOS, 0, 0));
                wosstr.str(L""); wosstr << pos;
                SetWindowText(GetDlgItem(hwnd, IDC_STATIC_VAL_H2_LOW), wosstr.str().c_str());
                g_config_imgproc.mask[(int)(ENUM_IMAGE_MASK::MASK_2)].hsv_l[(int)(ENUM_HSV_MODEL::H)] = pos;
            }
            else if (GetDlgItem(hwnd, IDC_SLIDER_H2_UPP) == reinterpret_cast<HWND>(lp)) {
                pos = static_cast<int32_t>(SendMessage(GetDlgItem(hwnd, IDC_SLIDER_H2_UPP), TBM_GETPOS, 0, 0));
                wosstr.str(L""); wosstr << pos;
                SetWindowText(GetDlgItem(hwnd, IDC_STATIC_VAL_H2_UPP), wosstr.str().c_str());
                g_config_imgproc.mask[(int)(ENUM_IMAGE_MASK::MASK_2)].hsv_u[(int)(ENUM_HSV_MODEL::H)] = pos;
            }
            else {
                ;
            }
            // 彩度S
            if (GetDlgItem(hwnd, IDC_SLIDER_S2_LOW) == reinterpret_cast<HWND>(lp)) {
                pos = static_cast<int32_t>(SendMessage(GetDlgItem(hwnd, IDC_SLIDER_S2_LOW), TBM_GETPOS, 0, 0));
                wosstr.str(L""); wosstr << pos;
                SetWindowText(GetDlgItem(hwnd, IDC_STATIC_VAL_S2_LOW), wosstr.str().c_str());
                g_config_imgproc.mask[(int)(ENUM_IMAGE_MASK::MASK_2)].hsv_l[(int)(ENUM_HSV_MODEL::S)] = pos;
            }
            else if (GetDlgItem(hwnd, IDC_SLIDER_S2_UPP) == reinterpret_cast<HWND>(lp)) {
                pos = static_cast<int32_t>(SendMessage(GetDlgItem(hwnd, IDC_SLIDER_S2_UPP), TBM_GETPOS, 0, 0));
                wosstr.str(L""); wosstr << pos;
                SetWindowText(GetDlgItem(hwnd, IDC_STATIC_VAL_S2_UPP), wosstr.str().c_str());
                g_config_imgproc.mask[(int)(ENUM_IMAGE_MASK::MASK_2)].hsv_u[(int)(ENUM_HSV_MODEL::S)] = pos;
            }
            else {
                ;
            }
            // 明度V
            if (GetDlgItem(hwnd, IDC_SLIDER_V2_LOW) == reinterpret_cast<HWND>(lp)) {
                pos = static_cast<int32_t>(SendMessage(GetDlgItem(hwnd, IDC_SLIDER_V2_LOW), TBM_GETPOS, 0, 0));
                wosstr.str(L""); wosstr << pos;
                SetWindowText(GetDlgItem(hwnd, IDC_STATIC_VAL_V2_LOW), wosstr.str().c_str());
                g_config_imgproc.mask[(int)(ENUM_IMAGE_MASK::MASK_2)].hsv_l[(int)(ENUM_HSV_MODEL::V)] = pos;
            }
            else if (GetDlgItem(hwnd, IDC_SLIDER_V2_UPP) == reinterpret_cast<HWND>(lp)) {
                pos = static_cast<int32_t>(SendMessage(GetDlgItem(hwnd, IDC_SLIDER_V2_UPP), TBM_GETPOS, 0, 0));
                wosstr.str(L""); wosstr << pos;
                SetWindowText(GetDlgItem(hwnd, IDC_STATIC_VAL_V2_UPP), wosstr.str().c_str());
                g_config_imgproc.mask[(int)(ENUM_IMAGE_MASK::MASK_2)].hsv_u[(int)(ENUM_HSV_MODEL::V)] = pos;
            }
            else {
                ;
            }
        }

        //----------------------------------------------------------------------------
        // ノイズフィルタ
        // ゴマ塩
        {
            int32_t      pos;

            if (GetDlgItem(hwnd, IDC_SLIDER_NOISEFILTER1) == reinterpret_cast<HWND>(lp)) {
                pos = static_cast<int32_t>(SendMessage(GetDlgItem(hwnd, IDC_SLIDER_NOISEFILTER1), TBM_GETPOS, 0, 0));
                if (g_config_imgproc.filter[(int)(ENUM_NOISE_FILTER::FILTER_1)].type == (int)(ENUM_NOISE_FILTER1::MEDIAN)) {
                    if ((pos % 2) == 0) {
                        pos = pos + 1;
                    }
                    SendMessage(GetDlgItem(hwnd, IDC_SLIDER_NOISEFILTER1), TBM_SETPOS, TRUE, pos);  // 位置の設定
                }
                wosstr.str(L""); wosstr << pos;
                SetWindowText(GetDlgItem(hwnd, IDC_STATIC_VAL_NOISEFILTER1), wosstr.str().c_str());
                g_config_imgproc.filter[(int)(ENUM_NOISE_FILTER::FILTER_1)].val = pos;
            }
        }
        // 穴埋め
        {
            int32_t      pos;

            if (GetDlgItem(hwnd, IDC_SLIDER_NOISEFILTER2) == reinterpret_cast<HWND>(lp)) {
                pos = static_cast<int32_t>(SendMessage(GetDlgItem(hwnd, IDC_SLIDER_NOISEFILTER2), TBM_GETPOS, 0, 0));
                wosstr.str(L""); wosstr << pos;
                SetWindowText(GetDlgItem(hwnd, IDC_STATIC_VAL_NOISEFILTER2), wosstr.str().c_str());
                g_config_imgproc.filter[(int)(ENUM_NOISE_FILTER::FILTER_2)].val = pos;
            }
        }

        //----------------------------------------------------------------------------
        // カメラ露光時間
        {
            int32_t pos;

            if (GetDlgItem(hwnd, IDC_SLIDER_CAMERA_EXPOSURE) == reinterpret_cast<HWND>(lp)) {
                pos = static_cast<int32_t>(SendMessage(GetDlgItem(hwnd, IDC_SLIDER_CAMERA_EXPOSURE), TBM_GETPOS, 0, 0));
                g_config_camera.expstime.val = static_cast<double>(pos);
            }
        }

        //----------------------------------------------------------------------------
        // ターゲット間距離
        {
            int32_t      pos;

            if (GetDlgItem(hwnd, IDC_SLIDER_TARGET_LEN) == reinterpret_cast<HWND>(lp)) {
                pos = static_cast<int32_t>(SendMessage(GetDlgItem(hwnd, IDC_SLIDER_TARGET_LEN), TBM_GETPOS, 0, 0));
                wosstr.str(L""); wosstr << pos;
                SetWindowText(GetDlgItem(hwnd, IDC_STATIC_VAL_TARGET_LEN), wosstr.str().c_str());
                g_infoajs_data.target_distance = static_cast<double>(pos);
            }
        }
    }break;
    case WM_VSCROLL: {
        //----------------------------------------------------------------------------
        // スクロールバー
        // 画像表示
        {
            HWND    wnd_hndl = GetDlgItem(hwnd, IDC_SCROLLBAR_IMAGE_V);
            int32_t pt = 20;

            if (wnd_hndl == reinterpret_cast<HWND>(lp)) {
                switch (LOWORD(wp)) {
                case SB_PAGEUP:
                    if ((m_scrlinf_img_src_v.nPos -= pt) < m_scrlinf_img_src_v.nMin) {
                        m_scrlinf_img_src_v.nPos = m_scrlinf_img_src_v.nMin;
                    }
                    break;
                case SB_PAGEDOWN:
                    if ((m_scrlinf_img_src_v.nPos += pt) > m_scrlinf_img_src_v.nMax) {
                        m_scrlinf_img_src_v.nPos = m_scrlinf_img_src_v.nMax;
                    }
                    break;
                case SB_LINEUP:
                    if ((m_scrlinf_img_src_v.nPos -= 1) < m_scrlinf_img_src_v.nMin) {
                        m_scrlinf_img_src_v.nPos = m_scrlinf_img_src_v.nMin;
                    }
                    break;
                case SB_LINEDOWN:
                    if ((m_scrlinf_img_src_v.nPos += 1) > m_scrlinf_img_src_v.nMax) {
                        m_scrlinf_img_src_v.nPos = m_scrlinf_img_src_v.nMax;
                    }
                    break;
                case SB_THUMBPOSITION:
                case SB_THUMBTRACK:
                    m_scrlinf_img_src_v.nPos = HIWORD(wp);
                    break;
                default:
                    break;
                }
                SetScrollInfo(wnd_hndl, SB_CTL, &m_scrlinf_img_src_v, TRUE);
            }
        }
    }break;
    case WM_COMMAND: {
        // 選択されたメニューの解析:
        switch (wmId) {
        case IDC_BUTTON_IMAGE_SAVE:
            (m_img_src.empty() != TRUE) ? cv::imwrite(CStrHelper::conv_string(g_config_common.img_output_fname), m_img_src) :
                MessageBox(hwnd, L"保存する画像がありません。", L"エラー", MB_OK);
            break;
        case IDC_CHECK_CURSOR:
            (BST_CHECKED == SendMessage(GetDlgItem(hwnd, IDC_CHECK_CURSOR), BM_GETCHECK, 0, 0)) ? m_cursor = TRUE :
                m_cursor = FALSE;
            EnableWindow(GetDlgItem(hwnd, IDC_BUTTON_CURSOR_L), m_cursor);
            EnableWindow(GetDlgItem(hwnd, IDC_BUTTON_CURSOR_R), m_cursor);
            EnableWindow(GetDlgItem(hwnd, IDC_BUTTON_CURSOR_U), m_cursor);
            EnableWindow(GetDlgItem(hwnd, IDC_BUTTON_CURSOR_D), m_cursor);
            break;
        case IDC_BUTTON_CURSOR_L:
        {
            LONG pos = m_cursor_pt.x;
            if (GetAsyncKeyState(VK_SHIFT) & 0x8000) {
                pos -= 50;
            }
            else if (GetAsyncKeyState(VK_CONTROL) & 0x8000) {
                pos -= 10;
            }
            else {
                pos -= 1;
            }
            if (pos < 0) {
                pos = m_img_src.cols - 1;
            }
            m_cursor_pt.x = pos;
        }
        break;
        case IDC_BUTTON_CURSOR_R:
        {
            LONG pos = m_cursor_pt.x;
            if (GetAsyncKeyState(VK_SHIFT) & 0x8000) {
                pos += 50;
            }
            else if (GetAsyncKeyState(VK_CONTROL) & 0x8000) {
                pos += 10;
            }
            else {
                pos += 1;
            }
            if (pos >= static_cast<LONG>(m_img_src.cols)) {
                pos = 0;
            }
            m_cursor_pt.x = pos;
        }
        break;
        case IDC_BUTTON_CURSOR_U:
        {
            LONG pos = m_cursor_pt.y;
            if (GetAsyncKeyState(VK_SHIFT) & 0x8000) {
                pos -= 50;
            }
            else if (GetAsyncKeyState(VK_CONTROL) & 0x8000) {
                pos -= 10;
            }
            else {
                pos -= 1;
            }
            if (pos < 0) {
                pos = m_img_src.rows - 1;
            }
            m_cursor_pt.y = pos;
        }
        break;
        case IDC_BUTTON_CURSOR_D:
        {
            LONG pos = m_cursor_pt.y;
            if (GetAsyncKeyState(VK_SHIFT) & 0x8000) {
                pos += 50;
            }
            else if (GetAsyncKeyState(VK_CONTROL) & 0x8000) {
                pos += 10;
            }
            else {
                pos += 1;
            }
            if (pos >= static_cast<LONG>(m_img_src.rows)) {
                pos = 0;
            }
            m_cursor_pt.y = pos;
        }
        break;
        case IDC_CHECK_IMAGE_SCALE:
            if (BST_CHECKED == SendMessage(GetDlgItem(hwnd, IDC_CHECK_IMAGE_SCALE), BM_GETCHECK, 0, 0)) {
                m_scale_img_src = TRUE;
                ShowWindow(GetDlgItem(hwnd, IDC_SCROLLBAR_IMAGE_H), SW_HIDE);
                ShowWindow(GetDlgItem(hwnd, IDC_SCROLLBAR_IMAGE_V), SW_HIDE);
            }
            else {
                m_scale_img_src = FALSE;
                ShowWindow(GetDlgItem(hwnd, IDC_SCROLLBAR_IMAGE_H), SW_SHOW);
                ShowWindow(GetDlgItem(hwnd, IDC_SCROLLBAR_IMAGE_V), SW_SHOW);
            }
            break;
        case IDC_COMBO_IMAGE:
            if (HIWORD(wp) == CBN_SELCHANGE) {
                m_sel_img = static_cast<UINT>(SendMessage(GetDlgItem(hwnd, IDC_COMBO_IMAGE), CB_GETCURSEL, 0, 0));
            }
            break;
        case IDC_CHECK_SWAY_POS:
            (BST_CHECKED == SendMessage(GetDlgItem(hwnd, IDC_CHECK_SWAY_POS), BM_GETCHECK, 0, 0)) ? m_sway_pos = TRUE :
                m_sway_pos = FALSE;
            break;
        case IDC_CHECK_TARGET_ROI_1:
            (BST_CHECKED == SendMessage(GetDlgItem(hwnd, IDC_CHECK_TARGET_ROI_1), BM_GETCHECK, 0, 0)) ? m_target_roi[(int)(ENUM_IMAGE_MASK::MASK_1)] = TRUE :
                m_target_roi[(int)(ENUM_IMAGE_MASK::MASK_1)] = FALSE;
            break;
        case IDC_CHECK_TARGET_ROI_2:
            (BST_CHECKED == SendMessage(GetDlgItem(hwnd, IDC_CHECK_TARGET_ROI_2), BM_GETCHECK, 0, 0)) ? m_target_roi[(int)(ENUM_IMAGE_MASK::MASK_2)] = TRUE :
                m_target_roi[(int)(ENUM_IMAGE_MASK::MASK_2)] = FALSE;
            break;
        case IDC_CHECK_TARGET_POS_1:
            (BST_CHECKED == SendMessage(GetDlgItem(hwnd, IDC_CHECK_TARGET_POS_1), BM_GETCHECK, 0, 0)) ? m_target_pos[(int)(ENUM_IMAGE_MASK::MASK_1)] = TRUE :
                m_target_pos[(int)(ENUM_IMAGE_MASK::MASK_1)] = FALSE;
            break;
        case IDC_CHECK_TARGET_POS_2:
            (BST_CHECKED == SendMessage(GetDlgItem(hwnd, IDC_CHECK_TARGET_POS_2), BM_GETCHECK, 0, 0)) ? m_target_pos[(int)(ENUM_IMAGE_MASK::MASK_2)] = TRUE :
                m_target_pos[(int)(ENUM_IMAGE_MASK::MASK_2)] = FALSE;
            break;
        case IDC_CHECK_TARGET_CONTOURS_1:
            (BST_CHECKED == SendMessage(GetDlgItem(hwnd, IDC_CHECK_TARGET_CONTOURS_1), BM_GETCHECK, 0, 0)) ? m_target_contours[(int)(ENUM_IMAGE_MASK::MASK_1)] = TRUE :
                m_target_contours[(int)(ENUM_IMAGE_MASK::MASK_1)] = FALSE;
            break;
        case IDC_CHECK_TARGET_CONTOURS_2:
            (BST_CHECKED == SendMessage(GetDlgItem(hwnd, IDC_CHECK_TARGET_CONTOURS_2), BM_GETCHECK, 0, 0)) ? m_target_contours[(int)(ENUM_IMAGE_MASK::MASK_2)] = TRUE :
                m_target_contours[(int)(ENUM_IMAGE_MASK::MASK_2)] = FALSE;
            break;
        case IDC_COMBO_NOISEFILTER1:
            if (HIWORD(wp) == CBN_SELCHANGE) {
                uint32_t     sel;
                wosstr.str(L"");
                int32_t      pos;

                sel = (int)(SendMessage(GetDlgItem(hwnd, IDC_COMBO_NOISEFILTER1), CB_GETCURSEL, 0, 0));
                if ((sel == (int)(ENUM_NOISE_FILTER1::MEDIAN)) || (sel == (int)(ENUM_NOISE_FILTER1::OPENNING))) {
                    ShowWindow(GetDlgItem(hwnd, IDC_SLIDER_NOISEFILTER1), SW_SHOW);
                    ShowWindow(GetDlgItem(hwnd, IDC_STATIC_VAL_NOISEFILTER1), SW_SHOW);
                    if (sel == (int)(ENUM_NOISE_FILTER1::MEDIAN)) {
                        SendMessage(GetDlgItem(hwnd, IDC_SLIDER_NOISEFILTER1), TBM_SETPAGESIZE, 0, 2);  // クリック時の移動量
                        pos = static_cast<int32_t>(SendMessage(GetDlgItem(hwnd, IDC_SLIDER_NOISEFILTER1), TBM_GETPOS, 0, 0));
                        if ((pos % 2) == 0) {
                            pos = pos + 1;
                        }
                        SendMessage(GetDlgItem(hwnd, IDC_SLIDER_NOISEFILTER1), TBM_SETPOS, TRUE, pos);  // 位置の設定
                        wosstr << pos;
                        SetWindowText(GetDlgItem(hwnd, IDC_STATIC_VAL_NOISEFILTER1), wosstr.str().c_str());
                        g_config_imgproc.filter[(int)(ENUM_NOISE_FILTER::FILTER_1)].val = pos;
                    }
                    else {
                        SendMessage(GetDlgItem(hwnd, IDC_SLIDER_NOISEFILTER1), TBM_SETPAGESIZE, 0, 1);  // クリック時の移動量
                    }
                }
                else {
                    ShowWindow(GetDlgItem(hwnd, IDC_SLIDER_NOISEFILTER1), SW_HIDE);
                    ShowWindow(GetDlgItem(hwnd, IDC_STATIC_VAL_NOISEFILTER1), SW_HIDE);
                }
                g_config_imgproc.filter[(int)(ENUM_NOISE_FILTER::FILTER_1)].type = sel;
            }
            break;
        case IDC_COMBO_NOISEFILTER2:
            if (HIWORD(wp) == CBN_SELCHANGE)
            {
                uint32_t sel;

                sel = (int)(SendMessage(GetDlgItem(hwnd, IDC_COMBO_NOISEFILTER2), CB_GETCURSEL, 0, 0));
                if ((sel == (int)(ENUM_NOISE_FILTER2::CLOSING)) || (sel == (int)(ENUM_NOISE_FILTER2::MEDIAN))) {
                    ShowWindow(GetDlgItem(hwnd, IDC_SLIDER_NOISEFILTER2), SW_SHOW);
                    ShowWindow(GetDlgItem(hwnd, IDC_STATIC_VAL_NOISEFILTER2), SW_SHOW);
                    SendMessage(GetDlgItem(hwnd, IDC_SLIDER_NOISEFILTER2), TBM_SETPAGESIZE, 0, 1);  // クリック時の移動量
                }
                else {
                    ShowWindow(GetDlgItem(hwnd, IDC_SLIDER_NOISEFILTER2), SW_HIDE);
                    ShowWindow(GetDlgItem(hwnd, IDC_STATIC_VAL_NOISEFILTER2), SW_HIDE);
                }
                g_config_imgproc.filter[(int)(ENUM_NOISE_FILTER::FILTER_2)].type = sel;
            }
            break;
        case IDC_CHECK_CAMERA_EXPOSURE:
            if (BST_CHECKED == SendMessage(GetDlgItem(hwnd, IDC_CHECK_CAMERA_EXPOSURE), BM_GETCHECK, 0, 0)) {
                g_config_camera.expstime.auto_control = FALSE;
                ShowWindow(GetDlgItem(hwnd, IDC_SLIDER_CAMERA_EXPOSURE), SW_SHOW);
            }
            else {
                g_config_camera.expstime.auto_control = TRUE;
                ShowWindow(GetDlgItem(hwnd, IDC_SLIDER_CAMERA_EXPOSURE), SW_HIDE);
            }
            break;
        case IDC_CHECK_ROI:
            (BST_CHECKED == SendMessage(GetDlgItem(hwnd, IDC_CHECK_ROI), BM_GETCHECK, 0, 0)) ? g_config_imgproc.roi.valid = 1 :
                g_config_imgproc.roi.valid = 0;
            break;
        case IDC_CHECK_TARGET_LEN:
            if (BST_CHECKED == SendMessage(GetDlgItem(hwnd, IDC_CHECK_TARGET_LEN), BM_GETCHECK, 0, 0)) {
                g_infoajs_data.target_distance_fixed = TRUE;
                ShowWindow(GetDlgItem(hwnd, IDC_SLIDER_TARGET_LEN), SW_SHOW);
            }
            else {
                g_infoajs_data.target_distance_fixed = FALSE;
                ShowWindow(GetDlgItem(hwnd, IDC_SLIDER_TARGET_LEN), SW_HIDE);
            }
            break;
        case IDCANCEL:
            //          PostQuitMessage(0);
            SendMessage(hwnd, WM_CLOSE, 0, 0);
            break;
        }
    }break;
    case WM_TIMER: {

    }break;
    case WM_PAINT:
        break;
    case WM_CLOSE:
        KillTimer(m_cam_dlg_hndl, SCAD_ID_DIALOG_TIMER);
        EndDialog(hwnd, LOWORD(wp));
        m_cam_dlg_hndl = NULL;
        return TRUE;
        //      PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, msg, wp, lp);
    }
    return FALSE;

}


/****************************************************************************/
/*   モニタウィンドウ									                    */
/****************************************************************************/

LRESULT CALLBACK CAuxScada::Mon1Proc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
	switch (msg)
	{
	case WM_CREATE: {
		InitCommonControls();//コモンコントロール初期化
		HINSTANCE hInst = (HINSTANCE)GetModuleHandle(0);
		//ウィンドウにコントロール追加
		st_mon1.hctrl[SCAD_ID_MON1_STATIC_1] = CreateWindowW(TEXT("STATIC"), st_mon1.text[SCAD_ID_MON1_STATIC_1], WS_CHILD | WS_VISIBLE | SS_LEFT,
			st_mon1.pt[SCAD_ID_MON1_STATIC_1].x, st_mon1.pt[SCAD_ID_MON1_STATIC_1].y,
			st_mon1.sz[SCAD_ID_MON1_STATIC_1].cx, st_mon1.sz[SCAD_ID_MON1_STATIC_1].cy,
			hWnd, (HMENU)(SCAD_ID_MON1_CTRL_BASE + SCAD_ID_MON1_STATIC_1), hInst, NULL);

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

static wostringstream mon2wos;
LRESULT CALLBACK CAuxScada::Mon2Proc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
	switch (msg)
	{
	case WM_CREATE: {
		InitCommonControls();//コモンコントロール初期化
		HINSTANCE hInst = (HINSTANCE)GetModuleHandle(0);
		//ウィンドウにコントロール追加
		//STATIC,LABEL
		for (int i = SCAD_ID_MON2_STATIC_MSG; i <= SCAD_ID_MON2_STATIC_MSG; i++) {
			st_mon2.hctrl[i] = CreateWindowW(TEXT("STATIC"), st_mon2.text[i], WS_CHILD | WS_VISIBLE | SS_LEFT,
				st_mon2.pt[i].x, st_mon2.pt[i].y, st_mon2.sz[i].cx, st_mon2.sz[i].cy,
				hWnd, (HMENU)(SCAD_ID_MON2_CTRL_BASE + i), hInst, NULL);
		}

		SetTimer(hWnd, SCAD_ID_MON2_TIMER, SCAD_PRM_MON2_TIMER_MS, NULL);

	}break;
	case WM_COMMAND: {
		int wmId = LOWORD(wp);

		int _Id = wmId - SCAD_ID_MON2_CTRL_BASE;
		// 選択されたメニューの解析:
		switch (_Id)
		{

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
		KillTimer(hWnd, SCAD_ID_MON2_TIMER);
		st_mon2.hwnd_mon = NULL;
	}break;
	default:
		return DefWindowProc(hWnd, msg, wp, lp);
	}
	return S_OK;
}

HWND CAuxScada::open_monitor_wnd(HWND h_parent_wnd, int id) {

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

		st_mon1.hwnd_mon = CreateWindowW(TEXT("SCAD_MON1"), TEXT("SCAD_MON1"), WS_OVERLAPPEDWINDOW,
			SCAD_MON1_WND_X, SCAD_MON1_WND_Y, SCAD_MON1_WND_W, SCAD_MON1_WND_H,
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
		wcex.lpszMenuName = TEXT("SCAD_MON2");
		wcex.lpszClassName = TEXT("SCAD_MON2");
		wcex.hIconSm = NULL;

		ATOM fb = RegisterClassExW(&wcex);

		st_mon2.hwnd_mon = CreateWindowW(TEXT("SCAD_MON2"), TEXT("SCAD_MON2"), WS_OVERLAPPEDWINDOW,
			SCAD_MON2_WND_X, SCAD_MON2_WND_Y, SCAD_MON2_WND_W, SCAD_MON2_WND_H,
			h_parent_wnd, nullptr, hInst, nullptr);

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
void CAuxScada::close_monitor_wnd(int id) {
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
void CAuxScada::show_monitor_wnd(int id) {
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
void CAuxScada::hide_monitor_wnd(int id) {
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
LRESULT CALLBACK CAuxScada::PanelProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp) {

	switch (msg) {
	case WM_USER_TASK_REQ: {
		if (HIWORD(wp) == WM_USER_WPH_OPEN_IF_WND) {
			if (lp == BC_ID_MON1) st_mon1.hwnd_mon = open_monitor_wnd(hDlg, (int)lp);
			if (lp == BC_ID_MON2) st_mon2.hwnd_mon = open_monitor_wnd(hDlg, (int)lp);
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
			case IDC_TASK_FUNC_RADIO1:
                if (m_cam_dlg_hndl != NULL) {
                    SendMessage(m_cam_dlg_hndl, WM_CLOSE, 0, 0);
               //     DestroyWindow(m_cam_dlg_hndl);
                    CheckDlgButton(hDlg, LOWORD(wp), BST_UNCHECKED);
                }
                else{
                    if (create_dlg_wnd() != NULL) {
                        CheckDlgButton(hDlg, LOWORD(wp), BST_CHECKED);
                    }
                    else {
                        CheckDlgButton(hDlg, LOWORD(wp), BST_UNCHECKED);
                    }
                }
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
void CAuxScada::msg2listview(wstring wstr) {

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
void CAuxScada::set_PNLparam_value(float p1, float p2, float p3, float p4, float p5, float p6) {
	wstring wstr;
	wstr += std::to_wstring(p1); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT1), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p2); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT2), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p3); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT3), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p4); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT4), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p5); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT5), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p6); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT6), wstr.c_str());
}
//タブパネルのEdit Box説明テキストを設定
void CAuxScada::set_panel_tip_txt() {
	wstring wstr_type; wstring wstr;
	switch (inf.panel_func_id) {
    case IDC_TASK_FUNC_RADIO1:
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
void CAuxScada::set_func_pb_txt() {
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO1, L"SWAY");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO2, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO3, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO4, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO5, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO6, L"-");
	return;
}
//タブパネルのItem chkテキストを設定
void CAuxScada::set_item_chk_txt() {
	wstring wstr_type; wstring wstr;
	switch (inf.panel_func_id) {
	case IDC_TASK_FUNC_RADIO1: {
		SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK1, L"Dlg");
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

