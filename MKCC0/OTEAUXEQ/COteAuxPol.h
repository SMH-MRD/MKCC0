#pragma once

#include "framework.h"
#include "CBasicControl.h"
#include "SmemAux.H"

// Include files for using OpenCV.
#include <opencv2/opencv.hpp>
#include <gdiplus.h>

typedef struct _AUXPOL_PALLETE {
    SolidBrush whiteBrush;
}AUXPOL_PALLETE, * LPAUXPOL_PALLETE;

#define AUXPOL_MON1_WND_X                0
#define AUXPOL_MON1_WND_Y                0
#define AUXPOL_MON1_WND_W                320
#define AUXPOL_MON1_WND_H                240
#define AUXPOL_MON1_N_CTRL               32
#define AUXPOL_MON1_N_WCHAR              64

#define AUXPOL_DEFAULT_ROI_X             1000
#define AUXPOL_DEFAULT_ROI_Y             0   
#define AUXPOL_DEFAULT_ROI_W             50
#define AUXPOL_DEFAULT_ROI_H             50
#define AUXPOL_MARGIN_WORK_ROI           10

#define AUXPOL_CAMERA_DISP_RETIO         2  //カメラ映像を表示する時のサイズ割合（割値）


#define AUXPOL_ID_MON1_CTRL_BASE         4100
#define AUXPOL_ID_MON1_STATIC_INF        0

#define AUXPOL_ID_MON1_TIMER             4190
#define AUXPOL_PRM_MON1_TIMER_MS         100

typedef struct _AUXPOL_MON1 {
    int timer_ms = AUXPOL_PRM_MON1_TIMER_MS;
    bool is_monitor_active = false;
    HWND hwnd_mon;
    HWND hctrl[AUXPOL_MON1_N_CTRL] = {
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
    };
    POINT pt[AUXPOL_MON1_N_CTRL] = {
        5,5, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0
    };
    SIZE sz[AUXPOL_MON1_N_CTRL] = {
        295,190, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0
    };
    WCHAR text[AUXPOL_MON1_N_CTRL][AUXPOL_MON1_N_WCHAR] = {
        L"INFOMATION", L"", L"", L"", L"", L"", L"", L"",
        L"", L"", L"", L"", L"", L"", L"", L"",
        L"", L"", L"", L"", L"", L"", L"", L"",
        L"", L"", L"", L"", L"", L"", L"", L""
    };
}ST_AUXPOL_MON1, * LPST_AUXPOL_MON1;

//MON2---------------------------------------------------
#define AUXPOL_MON2_WND_X                    650
#define AUXPOL_MON2_WND_Y                    10   
#define AUXPOL_MON2_WND_W                    1000
#define AUXPOL_MON2_WND_H                    1080
#define AUXPOL_MON2_N_CTRL                   32
#define AUXPOL_MON2_N_WCHAR                  64

#define AUXPOL_ID_MON2_TIMER                 4191
#define AUXPOL_PRM_MON2_TIMER_MS             100

#define AUXPOL_ID_MON2_CTRL_BASE             4140
#define AUXPOL_ID_MON2_STATIC_LABEL          0    //ラベル
#define AUXPOL_ID_MON2_STATIC_MSG            1    //メッセージ表示部
#define AUXPOL_ID_MON2_STATIC_HL             2   //スライダーHL
#define AUXPOL_ID_MON2_STATIC_HH             3   //スライダーHH 
#define AUXPOL_ID_MON2_STATIC_SL             4   //スライダーSL
#define AUXPOL_ID_MON2_STATIC_SH             5   //スライダーSH
#define AUXPOL_ID_MON2_STATIC_VL             6   //スライダーVL
#define AUXPOL_ID_MON2_STATIC_VH             7   //スライダーVH

#define AUXPOL_ID_MON2_SLIDER_HL             16   //スライダーHL
#define AUXPOL_ID_MON2_SLIDER_HH             17   //スライダーHH 
#define AUXPOL_ID_MON2_SLIDER_SL             18   //スライダーSL
#define AUXPOL_ID_MON2_SLIDER_SH             19   //スライダーSH
#define AUXPOL_ID_MON2_SLIDER_VL             20   //スライダーVL
#define AUXPOL_ID_MON2_SLIDER_VH             21   //スライダーVH

#define AUXPOL_ID_MON2_PB_GET_CRITERION      24   //基準取り込みPB
#define AUXPOL_ID_MON2_PB_SET_ROI            25   //ROI設定PB
#define AUXPOL_ID_MON2_CB_DISP_ROI_CRITERION 26   //基準取り込みROI表示
#define AUXPOL_ID_MON2_CB_DISP_ROI_WORK      27   //ROI設定ROI表示


#define AUXPOL_CODE_MON2_DISP_DEFAULT        0   //デフォルト表示
#define AUXPOL_CODE_MON2_DISP_ROWMAT         1   //MAT生表示
#define AUXPOL_CODE_MON2_DISP_HSV_MASK       2   //MATROI表示

#define AUXPOL_MON2_CAMIMG_W                 640
#define AUXPOL_MON2_CAMIMG_H                 540

typedef struct _AUXPOL_MON2 {
    HWND hwnd_mon;
    bool is_monitor_active = false;
    bool flg_disp_cri_roi = false;
    bool flg_disp_work_roi = false;
    bool m_isDragging = false;
    bool flg_dispDragging = false;
    POINT m_startPt, m_currPt;
    cv::Rect rc_selected;

	int timer_ms = AUXPOL_PRM_MON2_TIMER_MS;            //表示更新用タイマーの周期
 
	INT32 disp_mode = AUXPOL_CODE_MON2_DISP_DEFAULT;	//表示モード
    HDC hdc;				//パネルへ書き込み用DC
 
    wostringstream wos;

    HWND hctrl[AUXPOL_MON2_N_CTRL] = {
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
    };
    POINT pt[AUXPOL_MON2_N_CTRL] = {
        0,0, 650,20, 900,55, 900,95, 900,155, 900,195, 900,255, 900,295,//Static
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        660,50, 660,90, 660,150, 660,190, 660,250, 660,290, 0,0, 0,0,//Slider
        660,350, 760,350, 660,385, 760,385, 0,0, 0,0, 0,0, 0,0,
    };
    SIZE sz[AUXPOL_MON2_N_CTRL] = {
        0,0, 300,20, 50,20, 50,20, 50,20, 50,20, 50,20, 50,20,  //Static
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,     //Slider
        80,30, 80,30, 80,30, 80,30, 0,0, 0,0, 0,0, 0,0
    };
    WCHAR text[AUXPOL_MON2_N_CTRL][AUXPOL_MON2_N_WCHAR] = {
        L"LABEL:", L"HSV MASK 閾値",  L"-",  L"-",  L"-",  L"-",  L"-", L"-",
        L"", L"", L"", L"", L"", L"", L"", L"",
        L"",  L"",  L"",  L"",  L"",  L"",  L"",  L"",     //Slider
        L"基準更新", L"ROI SET", L"表示", L"表示", L"", L"", L"", L""
    };

}ST_AUXPOL_MON2, * LPST_AUXPOL_MON2;

#define AUXPOL_CODE_IMG_PROC_ENABLE       0x00000001   //ベースのMatデータ有効
#define AUXPOL_CODE_IMG_PROC_ACTIVE       0x00000002   //検出実行中


#define AUXPOL_CODE_VIDEO_CHK_ERR		    -1	//映像チェック異常
#define AUXPOL_CODE_VIDEO_CHK_DETECT_ON	    1	//映像チェックON検出
#define AUXPOL_CODE_VIDEO_CHK_DETECT_OFF	2	//映像チェックOFF検出

class COteAuxPol : public CBasicControl
{
public:
    COteAuxPol();
    ~COteAuxPol();

    virtual HRESULT initialize(LPVOID lpParam) override;

    LRESULT CALLBACK PanelProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);

    static LRESULT CALLBACK Mon1Proc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);
    static LRESULT CALLBACK Mon2Proc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);

    static ST_AUXPOL_MON1 st_mon1;
    static ST_AUXPOL_MON2 st_mon2;

    //タブパネルのStaticテキストを設定
    virtual void set_panel_tip_txt() override;
    //タブパネルのFunctionボタンのStaticテキストを設定
    virtual void set_func_pb_txt() override;
    //タブパネルのItem chkテキストを設定
    virtual void set_item_chk_txt() override;
    //タブパネルのListViewにコメント出力
    virtual void msg2listview(wstring wstr) override;

    //パラメータ初期表示値設定
    virtual void set_PNLparam_value(float p1, float p2, float p3, float p4, float p5, float p6) override;

    //タブパネルのFunctionボタンのリセット
    virtual void reset_panel_func_pb(HWND hDlg) override { return; };

    static void UpdateMon2(HWND hWnd, HDC hdc);
    static HRESULT setup_graphics(HWND hwnd);
    static void clear_graphics();

    static LPST_AUXPOL_IMG_PROC pst_img_proc;

    int GetCraneDeviceStatus(LPST_AUXPOL_IMG_PROC pst_work);
 
private:

    // --- GDI+ 関連 ---
    ULONG_PTR m_gdiplusToken;
   static  std::unique_ptr<Pen>        m_pWhitePen;
   static  std::unique_ptr<Pen>        m_pBlackPen;
   static  std::unique_ptr<Pen>        m_pGrayPen;
   static  std::unique_ptr<Pen>        m_pRedPen;
   static  std::unique_ptr<Pen>        m_pYellowPen;
   static  std::unique_ptr<Pen>        m_pGreenPen;
   static  std::unique_ptr<Pen>        m_pLiteBluePen;
   static  std::unique_ptr<Pen>        m_pMagentaPen;

   static std::unique_ptr<SolidBrush> m_pWhiteBrush;
   static std::unique_ptr<SolidBrush> m_pBlackBrush;
   static std::unique_ptr<SolidBrush> m_pGrayBrush;
   static std::unique_ptr<SolidBrush> m_pRedBrush;
   static std::unique_ptr<SolidBrush> m_pYellowBrush;
   static std::unique_ptr<SolidBrush> m_pGreenBrush;
   static std::unique_ptr<SolidBrush> m_pBlueBrush;
   static std::unique_ptr<SolidBrush> m_pLiteBlueBrush;
   static std::unique_ptr<SolidBrush> m_pMagentaBrush;

    static std::unique_ptr<FontFamily> m_pFontFamily;
    static std::unique_ptr<Font> m_pFont18;
    static std::unique_ptr<Font> m_pFont36;

    static std::unique_ptr<Bitmap>   m_pOffscreenBitmap;
    static std::unique_ptr<Graphics> m_pBackGroundGraphics; //背景描画用ビットマップ
    static std::unique_ptr<Graphics> m_pOffscreenGraphics;

    static Graphics* pgraphic_img;	//描画用グラフィックス
	static cv::Rect rc_mat_roi_work;            //画像処理対象範囲ROI（実画像）
    static cv::Rect rc_mat_roi_criterion;       //検出対象範囲設定ROI（実画像）
    static cv::Rect rc_mat_roi_work_disp;       //画像処理対象範囲ROI（表示モニタ画像）
    static cv::Rect rc_mat_roi_criterion_disp;  //検出対象範囲設定ROI（表示モニタ画像）
    static cv::Mat mat_roi_work;
    static cv::Mat mat_criterion;
        
    //オーバーライド

    virtual HRESULT routine_work(void* pObj) override;

    HWND open_monitor_wnd(HWND h_parent_wnd, int id);
    void close_monitor_wnd(int id);
    void show_monitor_wnd(int id);
    void hide_monitor_wnd(int id);

    int set_outbuf(LPVOID) {//出力バッファセット
        return STAT_NG;
    }

    int input();//入力処理
    int parse();
    int output();
    int close();

    static std::wstring GetTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time_t_now = std::chrono::system_clock::to_time_t(now);
        struct tm t; localtime_s(&t, &time_t_now);
        std::wstringstream wss;
        wss << std::put_time(&t, L"%H:%M:%S");
        return wss.str();
    }
    static HWND CreateSlider(HWND parent, int id, int x, int y, int minV, int maxV, int initV);
    static cv::Rect get_hsv_criterion();
    static cv::Rect set_work_roi(bool is_criterion_base);
    static void HSV_autoCalibrate();
    static void UpdateSliderPos();
    static int GetModeBinCenter(const cv::Mat& singleChannelMat, int maxVal, int binWidth);
	
};

