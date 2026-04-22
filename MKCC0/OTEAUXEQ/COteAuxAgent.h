#pragma once

#include "framework.h"

#include "CBasicControl.h"

// Include files for using OpenCV.
#include <opencv2/opencv.hpp>
#include <gdiplus.h>

typedef struct _ST_OTEAUXEQ {

	INT16 dummy = -1;
}ST_OTEAUXEQ,*LPST_OTEAUXEQ;

#define AUXAG_MON1_WND_X                1280
#define AUXAG_MON1_WND_Y                0
#define AUXAG_MON1_WND_W                320
#define AUXAG_MON1_WND_H                240
#define AUXAG_MON1_N_CTRL               32
#define AUXAG_MON1_N_WCHAR              64

#define AUXAG_ID_MON1_CTRL_BASE         5100
#define AUXAG_ID_MON1_STATIC_INF        0

#define AUXAG_ID_MON1_TIMER             5190
#define AUXAG_PRM_MON1_TIMER_MS         100

typedef struct _AUXAG_MON1 {
	int timer_ms = AUXAG_PRM_MON1_TIMER_MS;
    bool is_monitor_active = false;
	HWND hwnd_mon;
    HWND hctrl[AUXAG_MON1_N_CTRL] = {
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
    };
    POINT pt[AUXAG_MON1_N_CTRL] = {
        5,5, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0
    };
    SIZE sz[AUXAG_MON1_N_CTRL] = {
        295,190, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0
    };
    WCHAR text[AUXAG_MON1_N_CTRL][AUXAG_MON1_N_WCHAR] = {
        L"INFOMATION", L"", L"", L"", L"", L"", L"", L"",
        L"", L"", L"", L"", L"", L"", L"", L"",
        L"", L"", L"", L"", L"", L"", L"", L"",
        L"", L"", L"", L"", L"", L"", L"", L""
    };
}ST_AUXAG_MON1, * LPST_AUXAG_MON1;

//MON2---------------------------------------------------
#define AUXAG_MON2_WND_X                    10
#define AUXAG_MON2_WND_Y                    10   
#define AUXAG_MON2_WND_W                    1280
#define AUXAG_MON2_WND_H                    1080
#define AUXAG_MON2_N_CTRL                   32
#define AUXAG_MON2_N_WCHAR                  64

#define AUXAG_ID_MON2_TIMER                 5191
#define AUXAG_PRM_MON2_TIMER_MS             100

#define AUXAG_ID_MON2_CTRL_BASE             5140
#define AUXAG_ID_MON2_STATIC_MSG            0   //メッセージ表示部


typedef struct _AUXAG_MON2 {
    HWND hwnd_mon;
    int timer_ms = AUXAG_PRM_MON2_TIMER_MS;
    bool is_monitor_active = false;
 
    HBITMAP hBmp_camera;	//カメラ画像用ビットマップ
    Bitmap* pbmp_camera;	//カメラ画像用ビットマップ
    HDC hdc;				//パネルへ書き込み用DC
    HDC hdc_camera;			//カメラ画像用DC

	LONG thrad_counter = 0;		//スレッド内での描画更新回数カウンタ 
  
	Gdiplus::Rect rect_camera;	//イメージ描画用矩形

    wostringstream wos;

    HWND hctrl[AUXAG_MON2_N_CTRL] = {
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
    };
    POINT pt[AUXAG_MON2_N_CTRL] = {
        5,5, 5,30, 5,55, 5,80, 5,105, 5,130,5,200, 0,0,//Static
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        20,160,75,160,130,160,185,160, 330,160, 385,160,440,160,495,160, //CB
        550,160,240,160, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0
    };
    SIZE sz[AUXAG_MON2_N_CTRL] = {
        615,20, 615,20, 615,20, 615,20, 615,20,615,20,615,80, 0,0,//Static
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        50,20, 50,20, 50,20, 50,20, 50,20, 50,20, 50,20, 50,20,//CB
        50,20,50,20, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0
    };
    WCHAR text[AUXAG_MON2_N_CTRL][AUXAG_MON2_N_WCHAR] = {
        L"MSG:", L"",  L"",  L"",  L"",  L"",  L"", L"",
        L"", L"", L"", L"", L"", L"", L"", L"",
         L"",  L"",  L"",  L"",  L"",  L"",  L"",  L"",//CB
         L"", L"FUL", L"", L"", L"", L"", L"", L""
    };

}ST_AUXAG_MON2, * LPST_AUXAG_MON2;

#define AUXAG_PRM_V_DELAY_TMOV_LV1            2.5   //タイムオーバー判定用遅延時間(sec）異常判定値
#define AUXAG_PRM_V_DELAY_TMOV_LV2            3.0   //タイムオーバー判定用遅延時間(sec）計測打ち切り判定値

class COteAuxAgent :  public CBasicControl
{
public:
    COteAuxAgent() ;
    ~COteAuxAgent();
    
    virtual HRESULT initialize(LPVOID lpParam) override;

    LRESULT CALLBACK PanelProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);

    static LRESULT CALLBACK Mon1Proc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);
    static LRESULT CALLBACK Mon2Proc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);

    static ST_AUXAG_MON1 st_mon1;
    static ST_AUXAG_MON2 st_mon2;

    ST_OTEAUXEQ st_work;
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

    // カメラキャプチャスレッド
    static void UsbCameraThreadAG();
    static void OnPaintMon2(HWND hWnd, HDC hdc);

 
private:


    // --- GDI+ 関連 ---
    ULONG_PTR m_gdiplusToken;
    static std::unique_ptr<Bitmap>   m_pOffscreenBitmap;
    static std::unique_ptr<Graphics> m_pOffscreenGraphics;

    static Graphics* pgraphic_img;	//描画用グラフィックス


    //オーバーライド
    virtual HRESULT routine_work(void* pObj) override;

    HWND open_monitor_wnd(HWND h_parent_wnd, int id);
    void close_monitor_wnd(int id);
    void show_monitor_wnd(int id);
    void hide_monitor_wnd(int id);

	static void camera_capture_start();
    static void camera_capture_stop();

    int set_outbuf(LPVOID) {//出力バッファセット
        return STAT_NG;
    }                 

    int input();//入力処理
    int parse(); 
    int output(); 
    int close();

    static HRESULT setup_graphics(HWND hwnd);
    static void clear_graphics();
};

