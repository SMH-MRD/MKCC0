#pragma once
#include "CBasicControl.h"

#include "framework.h"
#include "CSHAREDMEM.H"
#include "SmemMain.H"

#define POL_MON1_WND_X     640
#define POL_MON1_WND_Y     0
#define POL_MON1_WND_W     320
#define POL_MON1_WND_H     240
#define POL_MON1_N_CTRL    32
#define POL_MON1_N_WCHAR   64

#define POL_ID_MON1_CTRL_BASE   54100
#define POL_ID_MON1_STATIC_GPAD     0

#define POL_ID_MON2_CTRL_BASE   541140

#define POL_ID_MON1_TIMER  54190
#define POL_ID_MON2_TIMER  54191

#define POL_PRM_MON1_TIMER_MS  200
#define POL_PRM_MON2_TIMER_MS  200


typedef struct _ST_POL_MON1 {
    int timer_ms = POL_PRM_MON1_TIMER_MS;
    HWND hwnd_mon;
    HWND hctrl[POL_MON1_N_CTRL] = {
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
    };
    POINT pt[POL_MON1_N_CTRL] = {
        5,5, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0
    };
    SIZE sz[POL_MON1_N_CTRL] = {
        295,190, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0
    };
    WCHAR text[POL_MON1_N_CTRL][POL_MON1_N_WCHAR] = {
        L"GAME_PAD", L"", L"", L"", L"", L"", L"", L"",
        L"", L"", L"", L"", L"", L"", L"", L"",
        L"", L"", L"", L"", L"", L"", L"", L"",
        L"", L"", L"", L"", L"", L"", L"", L""
    };
}ST_POL_MON1, * LPST_POL_MON1;

#define POL_MON2_WND_X     POL_MON1_WND_X
#define POL_MON2_WND_Y     POL_MON1_WND_Y + POL_MON1_WND_H   
#define POL_MON2_WND_W     320
#define POL_MON2_WND_H     240

typedef struct _ST_POL_MON2 {
    HWND hwnd_mon;

}ST_POL_MON2, * LPST_POL_MON2;

class CPolicy : public CBasicControl
{
public:
    CPolicy();
    ~CPolicy();

    virtual HRESULT initialize(LPVOID lpParam) override;

    LRESULT CALLBACK PanelProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);

    static LRESULT CALLBACK Mon1Proc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);
    static LRESULT CALLBACK Mon2Proc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);

    static ST_POL_MON1 st_mon1;
    static ST_POL_MON2 st_mon2;

    //タスク出力用構造体
    static ST_CC_ENV_INF st_work;

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

    int input();    //入力処理

	int parse();   //メイン処理
	int output();   //出力処理
    int close();
};



