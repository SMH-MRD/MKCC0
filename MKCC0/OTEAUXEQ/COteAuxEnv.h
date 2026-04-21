#pragma once

#include "CBasicControl.h"
#include "framework.h"
#include "COteAuxEnv.h"


#define AUXENV_MON1_WND_X                1280
#define AUXENV_MON1_WND_Y                0
#define AUXENV_MON1_WND_W                320
#define AUXENV_MON1_WND_H                240
#define AUXENV_MON1_N_CTRL               32
#define AUXENV_MON1_N_WCHAR              64

#define AUXENV_ID_MON1_CTRL_BASE         5100
#define AUXENV_ID_MON1_STATIC_INF        0

#define AUXENV_ID_MON1_TIMER             5190
#define AUXENV_PRM_MON1_TIMER_MS         100

typedef struct _OTEAUXENV_MON1 {
    int timer_ms = AUXENV_PRM_MON1_TIMER_MS;
    bool is_monitor_active = false;
    HWND hwnd_mon;
    HWND hctrl[AUXENV_MON1_N_CTRL] = {
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
    };
    POINT pt[AUXENV_MON1_N_CTRL] = {
        5,5, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0
    };
    SIZE sz[AUXENV_MON1_N_CTRL] = {
        295,190, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0
    };
    WCHAR text[AUXENV_MON1_N_CTRL][AUXENV_MON1_N_WCHAR] = {
        L"INFOMATION", L"", L"", L"", L"", L"", L"", L"",
        L"", L"", L"", L"", L"", L"", L"", L"",
        L"", L"", L"", L"", L"", L"", L"", L"",
        L"", L"", L"", L"", L"", L"", L"", L""
    };
}ST_OTEAUXENV_MON1, * LPST_OTEAUXENV_MON1;


//MON2---------------------------------------------------
#define AUXENV_MON2_WND_X                    10
#define AUXENV_MON2_WND_Y                    10   
#define AUXENV_MON2_WND_W                    1280
#define AUXENV_MON2_WND_H                    1080
#define AUXENV_MON2_N_CTRL                   32
#define AUXENV_MON2_N_WCHAR                  64

#define AUXENV_ID_MON2_TIMER                 5191
#define AUXENV_PRM_MON2_TIMER_MS             100

#define AUXENV_ID_MON2_CTRL_BASE             5140
#define AUXENV_ID_MON2_STATIC_MSG            0   //メッセージ表示部


typedef struct _OTE_AUXENV_MON2 {
    HWND hwnd_mon;
    int timer_ms = AUXENV_PRM_MON2_TIMER_MS;
    bool is_monitor_active = false;

 
    LONG thrad_counter = 0;		//スレッド内での描画更新回数カウンタ 

    Gdiplus::Rect rect_camera;	//イメージ描画用矩形

    wostringstream wos;

    HWND hctrl[AUXENV_MON2_N_CTRL] = {
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
    };
    POINT pt[AUXENV_MON2_N_CTRL] = {
        5,5, 5,30, 5,55, 5,80, 5,105, 5,130,5,200, 0,0,//Static
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        20,160,75,160,130,160,185,160, 330,160, 385,160,440,160,495,160, //CB
        550,160,240,160, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0
    };
    SIZE sz[AUXENV_MON2_N_CTRL] = {
        615,20, 615,20, 615,20, 615,20, 615,20,615,20,615,80, 0,0,//Static
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        50,20, 50,20, 50,20, 50,20, 50,20, 50,20, 50,20, 50,20,//CB
        50,20,50,20, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0
    };
    WCHAR text[AUXENV_MON2_N_CTRL][AUXENV_MON2_N_WCHAR] = {
        L"MSG:", L"",  L"",  L"",  L"",  L"",  L"", L"",
        L"", L"", L"", L"", L"", L"", L"", L"",
         L"",  L"",  L"",  L"",  L"",  L"",  L"",  L"",//CB
         L"", L"FUL", L"", L"", L"", L"", L"", L""
    };

}ST_OTEAUXENV_MON2, * LPST_OTEAUXENV_MON2;

class COteAuxEnv : public CBasicControl
{
public:
    COteAuxEnv();
    ~COteAuxEnv();

    virtual HRESULT initialize(LPVOID lpParam) override;

    LRESULT CALLBACK PanelProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);

    static LRESULT CALLBACK Mon1Proc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);
    static LRESULT CALLBACK Mon2Proc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);

    static ST_OTEAUXENV_MON1 st_mon1;
    static ST_OTEAUXENV_MON2 st_mon2;

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


