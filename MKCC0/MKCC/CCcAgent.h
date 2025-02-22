#pragma once
#include "CBasicControl.h"
#include "framework.h"
#include "CSHAREDMEM.H"

//MON1----------------------------------------------------
#define AGENT_MON1_WND_X            640
#define AGENT_MON1_WND_Y            0
#define AGENT_MON1_WND_W            320
#define AGENT_MON1_WND_H            240
#define AGENT_MON1_N_CTRL           32
#define AGENT_MON1_N_WCHAR          64

#define AGENT_ID_MON1_CTRL_BASE     51500
#define AGENT_ID_MON1_STATIC_MSG    0

//MON2---------------------------------------------------
#define AGENT_ID_MON2_CTRL_BASE         51540
#define AGENT_ID_MON2_STATIC_MSG        0   //メッセージ表示部
#define AGENT_ID_MON2_STATIC_RCV_H      1   //受信ヘッダ部
#define AGENT_ID_MON2_STATIC_SND_H      2   //送信ヘッダ部
#define AGENT_ID_MON2_STATIC_RCV_B      3   //受信ボディ部
#define AGENT_ID_MON2_STATIC_SND_B      4   //送信ボディ部
#define AGENT_ID_MON2_STATIC_RCV_B_SEL  5   //受信表示ブロック選択内容
#define AGENT_ID_MON2_PB_RCV_B_SEL      6   //受信表示ブロック切替PB

#define AGENT_ID_MON1_TIMER         51590
#define AGENT_ID_MON2_TIMER         51591

#define AGENT_PRM_MON1_TIMER_MS     150
#define AGENT_PRM_MON2_TIMER_MS     30


typedef struct _ST_AGENT_MON1 {
    int timer_ms = AGENT_PRM_MON1_TIMER_MS;
    HWND hwnd_mon;
    bool is_monitor_active = false;

    HWND hctrl[AGENT_MON1_N_CTRL] = {
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
    };
    POINT pt[AGENT_MON1_N_CTRL] = {
        5,5, 5,30, 5,95, 5,50, 5,115,  5,160, 160,160, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0
    };
    SIZE sz[AGENT_MON1_N_CTRL] = {
        615,20, 615,20, 615,20, 615,20, 615,20, 150,20, 40,20, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0
    };
    WCHAR text[AGENT_MON1_N_CTRL][AGENT_MON1_N_WCHAR] = {
        L"MSG:", L"", L"", L"", L"", L"", L"", L"",
        L"", L"", L"", L"", L"", L"", L"", L"",
        L"", L"", L"", L"", L"", L"", L"", L"",
        L"", L"", L"", L"", L"", L"", L"", L""
    };
}ST_AGENT_MON1, * LPST_AGENT_MON1;

#define AGENT_MON2_WND_X     AGENT_MON1_WND_X
#define AGENT_MON2_WND_Y     AGENT_MON1_WND_Y + AGENT_MON1_WND_H   
#define AGENT_MON2_WND_W     320
#define AGENT_MON2_WND_H     240
#define AGENT_MON2_N_CTRL    32
#define AGENT_MON2_N_WCHAR   64

typedef struct _ST_AGENT_MON2 {
    HWND hwnd_mon;
    int timer_ms = AGENT_PRM_MON2_TIMER_MS;
    bool is_monitor_active = false;

    wostringstream wo_uni, wo_mpc, wo_mote, wo_work;

    HWND hctrl[AGENT_MON2_N_CTRL] = {
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
    };
    POINT pt[AGENT_MON2_N_CTRL] = {
        5,5, 5,30, 5,95, 5,50, 5,115,  5,160, 160,160, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0
    };
    SIZE sz[AGENT_MON2_N_CTRL] = {
        615,20, 615,20, 615,20, 615,20, 615,20, 150,20, 40,20, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0
    };
    WCHAR text[AGENT_MON2_N_CTRL][AGENT_MON2_N_WCHAR] = {
        L"-", L"-", L"-", L"UNI:", L"MPC:", L"MOTE:",L"MSG:", L"RCV",
        L"SND",L"INFO",  L"", L"", L"", L"", L"", L"",
        L"", L"", L"", L"", L"", L"", L"", L"",
        L"", L"", L"", L"", L"", L"", L"", L""
    };


}ST_AGENT_MON2, * LPST_AGENT_MON2;

class CAgent : public CBasicControl
{
public:
    CAgent();
    ~CAgent();

    virtual HRESULT initialize(LPVOID lpParam) override;

    LRESULT CALLBACK PanelProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);

    static LRESULT CALLBACK Mon1Proc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);
    static LRESULT CALLBACK Mon2Proc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);

    static ST_AGENT_MON1 st_mon1;
    static ST_AGENT_MON2 st_mon2;

    //タスク出力用構造体
    static ST_CC_AGENT_INF st_work;

    //PLC IF関連
    static HRESULT rcv_uni_ote(LPST_PC_U_MSG pbuf);
    static HRESULT rcv_mul_pc(LPST_PC_M_MSG pbuf);
    static HRESULT rcv_mul_ote(LPST_OTE_M_MSG pbuf);

    static LPST_OTE_U_MSG set_msg_u(BOOL is_ope_mode, INT32 code, INT32 stat);
    static HRESULT snd_uni2pc(LPST_OTE_U_MSG pbuf, SOCKADDR_IN* p_addrin_to);

    static LPST_OTE_M_MSG set_msg_m();
    static HRESULT snd_mul2pc(LPST_OTE_M_MSG pbuf);
    static HRESULT snd_mul2ote(LPST_OTE_M_MSG pbuf);

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



