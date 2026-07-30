#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "framework.h"
#include "CBasicControl.h"
#include "AUXEQ_DEF.H"
#include "SWYSENSOR_DEF.H"

// Include files for using OpenCV.
#include <opencv2/opencv.hpp>
#include <gdiplus.h>


#define AUXAG_MON1_WND_X     1280
#define AUXAG_MON1_WND_Y     0
#define AUXAG_MON1_WND_W     640
#define AUXAG_MON1_WND_H     480
#define AUXAG_MON1_N_CTRL    32
#define AUXAG_MON1_N_WCHAR   64

#define AUXAG_ID_MON1_CTRL_BASE   5100
#define AUXAG_ID_MON1_STATIC_INF     0

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
        625,40, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
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
#define AUXAG_MON2_WND_X                    1280
#define AUXAG_MON2_WND_Y                    620   
#define AUXAG_MON2_WND_W                    640
#define AUXAG_MON2_WND_H                    400
#define AUXAG_MON2_N_CTRL                   32
#define AUXAG_MON2_N_WCHAR                  64

#define AUXAG_ID_MON2_TIMER                 5191
#define AUXAG_PRM_MON2_TIMER_MS             100

#define AUXAG_ID_MON2_CTRL_BASE             5140
#define AUXAG_ID_MON2_STATIC_MSG            0   //メッセージ表示部
#define AUXAG_ID_MON2_STATIC_INF            1   //接続情報表示部
#define AUXAG_ID_MON2_STATIC_REQ_R          2   //読込要求メッセージ
#define AUXAG_ID_MON2_STATIC_RES_R          3   //読込応答メッセージ
#define AUXAG_ID_MON2_STATIC_REQ_W          4   //書込要求メッセージ
#define AUXAG_ID_MON2_STATIC_RES_W          5   //書込応答メッセージ
#define AUXAG_ID_MON2_STATIC_MAIN_INF       6   //MAINロジック指令情報

#define AUXAG_ID_MON2_CB_COM_LEVEL_BIT0     16  //SLBRK　DBG出力ビットセット
#define AUXAG_ID_MON2_CB_COM_LEVEL_BIT1     17  //SLBRK　DBG出力ビットセット
#define AUXAG_ID_MON2_CB_COM_LEVEL_BIT2     18  //SLBRK　DBG出力ビットセット
#define AUXAG_ID_MON2_CB_COM_LEVEL_BIT3     19  //SLBRK　DBG出力ビットセット
#define AUXAG_ID_MON2_CB_COM_HW_BRK         20  //SLBRK　DBG出力ビットセット
#define AUXAG_ID_MON2_CB_COM_RST            21  //SLBRK　DBG出力ビットセット
#define AUXAG_ID_MON2_CB_COM_EMG            22  //SLBRK　DBG出力ビットセット
#define AUXAG_ID_MON2_CB_COM_AUTOSEL        23  //SLBRK　DBG出力ビットセット
#define AUXAG_ID_MON2_CB_MODE_SLBRK_DBG     24  //SLBRK　DBGモードセット
#define AUXAG_ID_MON2_CB_COM_LEVEL_FULL     25  //SLBRK　DBG出力ビット最大値セット

#define AUXAG_MON2_MSG_DISP_OFF             0
#define AUXAG_MON2_MSG_DISP_HEX             1
#define AUXAG_MON2_MSG_DISP_DEC             2

typedef struct _AUXAG_MON2 {
    HWND hwnd_mon;
    int timer_ms = AUXAG_PRM_MON2_TIMER_MS;
    bool is_monitor_active = false;
    int msg_disp_mode = AUXAG_MON2_MSG_DISP_OFF;
	int slbrk_dbg_mode = 0;//0:通常 1:debugモード

    wostringstream wo_req_r, wo_res_r, wo_req_w, wo_res_w;

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
        L"MSG:", L"INF", L"REQ R", L"RES R", L"REQ W", L"RES W", L"MAIN CS", L"",
        L"", L"", L"", L"", L"", L"", L"", L"",
        L"LV0", L"LV1", L"LV2", L"LV3", L"HWB", L"RST", L"EMG", L"AUT",//CB
        L"DBG", L"FUL", L"", L"", L"", L"", L"", L""
    };

}ST_AUXAG_MON2, * LPST_AUXAG_MON2;

class CAuxAgent :  public CBasicControl
{
public:
    CAuxAgent();
    ~CAuxAgent();
    
    virtual HRESULT initialize(LPVOID lpParam) override;
  
    static CONFIG_COMMON    m_cnfgcmn;      // 共通設定
    static CONFIG_CAMERA    m_cnfgcam;      // カメラ設定
    static CONFIG_IMGPROC   m_cnfgprc;      // 画像処理条件設定
    static INFO_ADJUST_DATA m_infoajs_data; // 調整情報データ
    static INFO_IMGPRC_DATA m_infoprc_data; // 画像処理情報データ
    static INFO_SYSTEM_DATA m_infosys_data; // システム情報データ

    // GEカメラ
    void SaveParameters_GECamera();
    void LoadParameters_GECamera();
    static void camera_capture_start();
    static void camera_capture_stop();
    static int update_camera_parameter_base();

    static HRESULT GECameraStart();
    static HRESULT GECameraStop();
    static void OnPaintMon1(HWND hWnd, HDC hdc);

    LRESULT CALLBACK PanelProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);

    static LRESULT CALLBACK Mon1Proc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);
    static LRESULT CALLBACK Mon2Proc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);
    
    static ST_AUXAG_MON1 st_mon1;
    static ST_AUXAG_MON2 st_mon2;

    static CONFIG_CAMERA st_cnfgcam;

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
        	
private:
    bool sway_config_ok = false;

    int slbrk_enable;
    int lanio_enable;
    int sway_sensor_enable;
    int gt_sensor_enable;

    // --- GDI+ 関連 ---
    ULONG_PTR m_gdiplusToken;
    static std::unique_ptr<Bitmap>   m_pOffscreenBitmap;
    static std::unique_ptr<Graphics> m_pOffscreenGraphics;
    static Graphics* pgraphic_img;	//描画用グラフィックス
    static HRESULT setup_graphics(HWND hwnd);
    static void clear_graphics();

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
};

