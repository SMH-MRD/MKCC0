#pragma once
#include "CBasicControl.h"
#include "framework.h"
#include "CSHAREDMEM.H"

#define CS_SEMIAUTO_TG_SEL_DEFAULT      0
#define CS_SEMIAUTO_TG_SEL_CLEAR		0
#define CS_SEMIAUTO_TG_SEL_ACTIVE       1
#define CS_SEMIAUTO_TG_SEL_FIXED        2
#define CS_SEMIAUTO_TG_MAX				16

#define CS_ID_SEMIAUTO_TOUCH_PT			8

#define CS_ID_NOTCH_POS_HOLD			0
#define CS_ID_NOTCH_POS_TRIG			1

#define CS_CODE_OTE_REMOTE_ENABLE		0x0001	//PLCへの操作信号有効
#define CS_CODE_OTE_SOURCE_ENABLE		0x0002	//主幹ON可能

#define CS_MON1_WND_X     640
#define CS_MON1_WND_Y     0
#define CS_MON1_WND_W     320
#define CS_MON1_WND_H     240
#define CS_MON1_N_CTRL    32
#define CS_MON1_N_WCHAR   64

#define CS_ID_MON1_CTRL_BASE   51100
#define CS_ID_MON1_STATIC_GPAD     0

#define CS_ID_MON2_CTRL_BASE   51140

#define CS_ID_MON1_TIMER  51190
#define CS_ID_MON2_TIMER  51191

#define CS_PRM_MON1_TIMER_MS  200
#define CS_PRM_MON2_TIMER_MS  200


typedef struct _ST_CS_MON1 {
    int timer_ms = CS_PRM_MON1_TIMER_MS;
    HWND hwnd_mon;
    HWND hctrl[CS_MON1_N_CTRL] = {
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
    };
    POINT pt[CS_MON1_N_CTRL] = {
        5,5, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0
    };
    SIZE sz[CS_MON1_N_CTRL] = {
        295,190, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0
    };
    WCHAR text[CS_MON1_N_CTRL][CS_MON1_N_WCHAR] = {
        L"GAME_PAD", L"", L"", L"", L"", L"", L"", L"",
        L"", L"", L"", L"", L"", L"", L"", L"",
        L"", L"", L"", L"", L"", L"", L"", L"",
        L"", L"", L"", L"", L"", L"", L"", L""
    };
}ST_CS_MON1, * LPST_CS_MON1;

#define CS_MON2_WND_X     CS_MON1_WND_X
#define CS_MON2_WND_Y     CS_MON1_WND_Y + CS_MON1_WND_H   
#define CS_MON2_WND_W     320
#define CS_MON2_WND_H     240

typedef struct _ST_CS_MON2 {
    HWND hwnd_mon;

}ST_CS_MON2, * LPST_CS_MON2;

class CClientService : public CBasicControl
{
public:
    CClientService();
    ~CClientService();

    virtual HRESULT initialize(LPVOID lpParam) override;

    LRESULT CALLBACK PanelProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);

    static LRESULT CALLBACK Mon1Proc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);
    static LRESULT CALLBACK Mon2Proc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);

    static ST_CS_MON1 st_mon1;
    static ST_CS_MON2 st_mon2;

    //タスク出力用構造体
    static ST_CRANE_STAT_CC st_work;

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

    int parse() {           //メイン処理
        return STAT_NG;
    }
    int output() {          //出力処理
        return STAT_NG;
    }
    int close();
};

