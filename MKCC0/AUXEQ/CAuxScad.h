#pragma once

#pragma once
#include "CBasicControl.h"
#include "framework.h"
#include "CSHAREDMEM.H"
#include "SWYSENSOR_DEF.H"

//============================================================================
// define定義
#pragma region CONSTANT_DEFINITION

#define SCAD_ID_DIALOG_TIMER        1900
#define SCAD_PRM_DIALOG_SCAN_MS     1000

#define DISP_IMG_SCROLL_SIZE        20      // 表示画像スクロールバーサイズ

#define SCALAR_WHITE            cv::Scalar(255, 255, 255)
#define SCALAR_BLUE             cv::Scalar(255, 0, 0)
#define SCALAR_GREEN            cv::Scalar(0, 255, 0)
#define SCALAR_RED              cv::Scalar(0, 0, 255)
#define SCALAR_YELLOW           cv::Scalar(0, 255, 255)
#define SCALAR_MAGENTA          cv::Scalar(255, 0, 255)
#define SCALAR_LT_BLUE          cv::Scalar(255, 255, 0)

#define LINE_TKN                1   // ライン表示太さ
#define CURSOR_LEN              10  // カーソル表示長さ
#pragma endregion CONSTANT_DEFINITION

#define SCAD_MON1_WND_X     640
#define SCAD_MON1_WND_Y     0
#define SCAD_MON1_WND_W     320
#define SCAD_MON1_WND_H     240
#define SCAD_MON1_N_CTRL    32
#define SCAD_MON1_N_WCHAR   64

#define SCAD_ID_MON1_CTRL_BASE   73100
#define SCAD_ID_MON1_STATIC_1     0

#define SCAD_ID_MON2_CTRL_BASE   73140
#define SCAD_ID_MON2_STATIC_MSG     0


#define SCAD_ID_MON1_TIMER  73190
#define SCAD_ID_MON2_TIMER  73191

#define SCAD_PRM_MON1_TIMER_MS  200
#define SCAD_PRM_MON2_TIMER_MS  200


typedef struct _ST_SCAD_MON1 {
    int timer_ms = SCAD_PRM_MON1_TIMER_MS;
    bool is_monitor_active = false;
    HWND hwnd_mon;
    HWND hctrl[SCAD_MON1_N_CTRL] = {
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
    };
    POINT pt[SCAD_MON1_N_CTRL] = {
        5,5, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0
    };
    SIZE sz[SCAD_MON1_N_CTRL] = {
        295,190, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0
    };
    WCHAR text[SCAD_MON1_N_CTRL][SCAD_MON1_N_WCHAR] = {
        L"GAME_PAD", L"", L"", L"", L"", L"", L"", L"",
        L"", L"", L"", L"", L"", L"", L"", L"",
        L"", L"", L"", L"", L"", L"", L"", L"",
        L"", L"", L"", L"", L"", L"", L"", L""
    };
}ST_SCAD_MON1, * LPST_SCAD_MON1;

#define SCAD_MON2_WND_X     SCAD_MON1_WND_X
#define SCAD_MON2_WND_Y     SCAD_MON1_WND_Y + SCAD_MON1_WND_H   
#define SCAD_MON2_WND_W     320
#define SCAD_MON2_WND_H     240

typedef struct _ST_SCAD_MON2 {
    HWND hwnd_mon;
    bool is_monitor_active = false;

    HWND hctrl[SCAD_MON1_N_CTRL] = {
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
    };
    POINT pt[SCAD_MON1_N_CTRL] = {
        5,5, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0
    };
    SIZE sz[SCAD_MON1_N_CTRL] = {
        295,190, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0
    };
    WCHAR text[SCAD_MON1_N_CTRL][SCAD_MON1_N_WCHAR] = {
        L"GAME_PAD", L"", L"", L"", L"", L"", L"", L"",
        L"", L"", L"", L"", L"", L"", L"", L"",
        L"", L"", L"", L"", L"", L"", L"", L"",
        L"", L"", L"", L"", L"", L"", L"", L""
    };

}ST_SCAD_MON2, * LPST_SCAD_MON2;

class CAuxScada : public CBasicControl
{
public:
    CAuxScada() ;
    ~CAuxScada();

    virtual HRESULT initialize(LPVOID lpParam) override;

    LRESULT CALLBACK PanelProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp) ;

    static LRESULT CALLBACK Mon1Proc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);
    static LRESULT CALLBACK Mon2Proc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);

    static ST_SCAD_MON1 st_mon1;
    static ST_SCAD_MON2 st_mon2;
    
    static cv::Mat m_img_src;   // 元画像

    static HWND       m_cam_dlg_hndl;
    static uint32_t   m_sel_img;
    static BOOL       m_cursor;
    static POINT      m_cursor_pt;
    static BOOL       m_scale_img_src;
    static SCROLLINFO m_scrlinf_img_src_h;
    static SCROLLINFO m_scrlinf_img_src_v;
    static BOOL       m_sway_pos;
    static BOOL       m_target_roi[static_cast<uint32_t>(ENUM_IMAGE_MASK::E_MAX)];
    static BOOL       m_target_pos[static_cast<uint32_t>(ENUM_IMAGE_MASK::E_MAX)];
    static BOOL       m_target_contours[static_cast<uint32_t>(ENUM_IMAGE_MASK::E_MAX)];

    //タスク出力用構造体
  
    //タブパネルのStaticテキストを設定
    virtual void set_panel_tip_txt() override;
    //タブパネルのFunctionボタンのStaticテキストを設定
    virtual void set_func_pb_txt() override;
    //タブパネルのItem chkテキストを設定
    virtual void set_item_chk_txt() override;
    //タブパネルのListViewにコメント出力
    virtual void msg2listview(wstring wstr) override;

    //パラメータ初期表示値設定
    virtual void set_PNLparam_value(float p1, float p2, float p3, float p4, float p5, float p6) override ;

    //タブパネルのFunctionボタンのリセット
    virtual void reset_panel_func_pb(HWND hDlg) override {
        return;
    };

private:
    bool sway_config_ok = false;

    int slbrk_enable;
    int lanio_enable;
    int sway_sensor_enable;
    int gt_sensor_enable;

    HWND create_dlg_wnd();  // ダイアログのオープン
    static LRESULT CALLBACK cb_dlg_wnd(HWND hwnd,uint32_t msg, WPARAM wp, LPARAM lp);  // ダイアログ画面用コールバック関数
   
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


