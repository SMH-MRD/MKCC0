#pragma once

#include "CBasicControl.h"
#include "framework.h"
#include "CSHAREDMEM.H"
#include "SmemOte.H"


#define OTE_ENV_MON1_WND_X              1200
#define OTE_ENV_MON1_WND_Y              0
#define OTE_ENV_MON1_WND_W              640
#define OTE_ENV_MON1_WND_H              480
#define OTE_ENV_MON1_N_CTRL             32
#define OTE_ENV_MON1_N_WCHAR            64

#define OTE_ENV_ID_MON1_CTRL_BASE       60100
#define OTE_ENV_ID_MON1_STATIC_TITLE    0
#define OTE_ENV_ID_MON1_STATIC_JC       1
#define OTE_ENV_ID_MON1_STATIC_GC       2
#define OTE_ENV_ID_MON1_STATIC_TEST     3
#define OTE_ENV_ID_MON1_STATIC_SELECTED 4

#define OTE_ENV_ID_MON1_RADIO_CRANE01   16
#define OTE_ENV_ID_MON1_RADIO_CRANE02   17
#define OTE_ENV_ID_MON1_RADIO_CRANE03   18
#define OTE_ENV_ID_MON1_RADIO_CRANE04   19
#define OTE_ENV_ID_MON1_RADIO_CRANE05   20
#define OTE_ENV_ID_MON1_RADIO_CRANE06   21
#define OTE_ENV_ID_MON1_RADIO_CRANE07   22
#define OTE_ENV_ID_MON1_RADIO_CRANE08   23
#define OTE_ENV_ID_MON1_RADIO_CRANE09   24
#define OTE_ENV_ID_MON1_RADIO_CRANE10   25

#define OTE_ENV_ID_MON1_PB_START        31


#define OTE_ENV_ID_MON1_TIMER           60190
#define OTE_ENV_ID_MON2_TIMER           60191

#define OTE_ENV_PRM_MON1_TIMER_MS       200
#define OTE_ENV_PRM_MON2_TIMER_MS       200


typedef struct _ST_OTE_ENV_MON1 {
    int timer_ms = OTE_ENV_PRM_MON1_TIMER_MS;
    HWND hwnd_mon;
    HWND hctrl[OTE_ENV_MON1_N_CTRL] = {
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
    };
    POINT pt[OTE_ENV_MON1_N_CTRL] = {
        20,20, 20,140, 20,240, 20,340, 180,60, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        80,110, 180,110, 280,110, 380,110, 480,110, 80,210, 180,210, 280,210,
        80,310, 180,310, 0,0, 0,0, 0,0, 0,0, 0,0, 80,60
    };
    SIZE sz[OTE_ENV_MON1_N_CTRL] = {
        600,20, 80,20, 80,20, 80,20, 160,20, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        80,80, 80,80, 80,80, 80,80, 80,80, 80,80, 80,80, 80,80,
        80,80, 80,80, 0,0, 0,0, 0,0, 0,0, 0,0, 80,20
    };
    WCHAR text[OTE_ENV_MON1_N_CTRL][OTE_ENV_MON1_N_WCHAR] = {
        L"今治造船/SHIMH　クレーン遠隔システム", L"JC", L"GC", L"TEST", L"クレーン未選択", L"", L"", L"",
        L"", L"", L"", L"", L"", L"", L"", L"",
        L"西多度津102号", L"-", L"-", L"-", L"-", L"-", L"-", L"-",
        L"CHECK",L"-", L"", L"", L"", L"", L"", L"START"
    };
}ST_OTE_ENV_MON1, * LPST_OTE_ENV_MON1;

#define OTE_ENV_MON2_WND_X     OTE_ENV_MON1_WND_X
#define OTE_ENV_MON2_WND_Y     OTE_ENV_MON1_WND_Y + OTE_ENV_MON1_WND_H   
#define OTE_ENV_MON2_WND_W     320
#define OTE_ENV_MON2_WND_H     240

#define OTE_ENV_ID_MON2_CTRL_BASE   60140

typedef struct _ST_OTE_ENV_MON2 {
    HWND hwnd_mon;

}ST_OTE_ENV_MON2, * LPST_OTE_ENV_MON2;

class COteEnv : public CBasicControl
{
public:
    COteEnv();
    ~COteEnv();

     virtual HRESULT initialize(LPVOID lpParam) override;

    LRESULT CALLBACK PanelProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);

    static LRESULT CALLBACK Mon1Proc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);
    static LRESULT CALLBACK Mon2Proc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);

    static ST_OTE_ENV_MON1 st_mon1;
    static ST_OTE_ENV_MON2 st_mon2;

    static ST_OTE_ENV_INF st_work;

    HRESULT open_opening_window();
    static HRESULT open_ope_window(int crane_id_selected);
	void clear_crane_if();

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
	bool is_opening_end = false;

     //オーバーライド

    virtual HRESULT routine_work(void* pObj) override;

    HWND open_monitor_wnd(HWND h_parent_wnd, int id);
   static void close_monitor_wnd(int id);
   static void show_monitor_wnd(int id);
   static void hide_monitor_wnd(int id);

 
    int set_outbuf(LPVOID) {//出力バッファセット
        return STAT_NG;
    }

    int input();//入力処理

    int parse();          //メイン処理
    int output();          //出力処理
    int close();
};

