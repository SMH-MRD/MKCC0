#pragma once

#pragma once
#include "CBasicControl.h"
#include "framework.h"
#include "CSHAREDMEM.H"

#define SCAD_MON1_WND_X     640
#define SCAD_MON1_WND_Y     0
#define SCAD_MON1_WND_W     320
#define SCAD_MON1_WND_H     240
#define SCAD_MON1_N_CTRL    32
#define SCAD_MON1_N_WCHAR   64

#define SCAD_ID_MON1_CTRL_BASE   73100
#define SCAD_ID_MON1_STATIC_GPAD     0

#define SCAD_ID_MON2_CTRL_BASE   73140

#define SCAD_ID_MON1_TIMER  73190
#define SCAD_ID_MON2_TIMER  73191

#define SCAD_PRM_MON1_TIMER_MS  200
#define SCAD_PRM_MON2_TIMER_MS  200


typedef struct _ST_SCAD_MON1 {
    int timer_ms = SCAD_PRM_MON1_TIMER_MS;
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

}ST_SCAD_MON2, * LPST_SCAD_MON2;

class CScada : public CBasicControl
{
public:
    CScada() {};
    ~CScada() {};

    virtual HRESULT initialize(LPVOID lpParam) override { return S_OK; };

    LRESULT CALLBACK PanelProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp) { return S_FALSE; };

    static LRESULT CALLBACK Mon1Proc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp) { return S_FALSE; };
    static LRESULT CALLBACK Mon2Proc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp) { return S_FALSE; };

    static ST_SCAD_MON1 st_mon1;
    static ST_SCAD_MON2 st_mon2;

    //タスク出力用構造体
    static ST_CC_ENV_INF st_work;

    //タブパネルのStaticテキストを設定
    virtual void set_panel_tip_txt() override { return; };
    //タブパネルのFunctionボタンのStaticテキストを設定
    virtual void set_func_pb_txt() override { return; };
    //タブパネルのItem chkテキストを設定
    virtual void set_item_chk_txt() override { return; };
    //タブパネルのListViewにコメント出力
    virtual void msg2listview(wstring wstr) override { return; };

    //パラメータ初期表示値設定
    virtual void set_PNLparam_value(float p1, float p2, float p3, float p4, float p5, float p6) override { return; };

    //タブパネルのFunctionボタンのリセット
    virtual void reset_panel_func_pb(HWND hDlg) override { return; };

private:

    //オーバーライド

    virtual HRESULT routine_work(void* pObj) override { return S_OK; };

    HWND open_monitor_wnd(HWND h_parent_wnd, int id) { return h_parent_wnd; };
    void close_monitor_wnd(int id) { return; };
    void show_monitor_wnd(int id) { return; };
    void hide_monitor_wnd(int id) { return; };


    int set_outbuf(LPVOID) {//出力バッファセット
        return STAT_NG;
    }

    int input() { return 0; };//入力処理

    int parse() {           //メイン処理
        return STAT_NG;
    }
    int output() {          //出力処理
        return STAT_NG;
    }
    int close() { return 0; };
};




class CAuxScad
{
};

