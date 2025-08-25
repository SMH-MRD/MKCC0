#pragma once
#include "CBasicControl.h"
#include "framework.h"
#include "CSHAREDMEM.H"

#include "SmemMain.H"
#include "SmemOte.H"
#include "PLC_DEF.h"
#include "CValue.h"


//MON1----------------------------------------------------
#define OTE_CS_MON1_WND_X            640
#define OTE_CS_MON1_WND_Y            0
#define OTE_CS_MON1_WND_W            320
#define OTE_CS_MON1_WND_H            280
#define OTE_CS_MON1_N_CTRL           32
#define OTE_CS_MON1_N_WCHAR          64

#define OTE_CS_ID_MON1_CTRL_BASE     61500
#define OTE_CS_ID_MON1_STATIC_MSG    0
#define OTE_CS_ID_MON1_CB_VIB_ACT    16
#define OTE_CS_ID_MON1_SLIDER_VIB_R  17
#define OTE_CS_ID_MON1_SLIDER_VIB_L  18

//MON2---------------------------------------------------
#define OTE_CS_ID_MON2_CTRL_BASE         61540
#define OTE_CS_ID_MON2_STATIC_MSG        0   //メッセージ表示部
#define OTE_CS_ID_MON2_STATIC_INF        1   //接続情報表示部
#define OTE_CS_ID_MON2_STATIC_REQ_R      2   //読込要求メッセージ
#define OTE_CS_ID_MON2_STATIC_RES_R      3   //読込応答メッセージ
#define OTE_CS_ID_MON2_STATIC_REQ_W      4   //書込要求メッセージ
#define OTE_CS_ID_MON2_STATIC_RES_W      5   //書込応答メッセージ


#define OTE_CS_ID_MON2_PB_R_BLOCK_SEL    16   //読み込み表示ブロック切替PB
#define OTE_CS_ID_MON2_PB_W_BLOCK_SEL    17  //読み込み表示ブロック切替PB
#define OTE_CS_ID_MON2_PB_MSG_DISP_SEL   18  //メッセージ表示/非表示切替PB
#define OTE_CS_ID_MON2_PB_DISP_DEC_SEL   19 //10進/16進表示切替PB

#define OTE_CS_ID_MON1_TIMER         61590
#define OTE_CS_ID_MON2_TIMER         61591

#define OTE_CS_PRM_MON1_TIMER_MS     150
#define OTE_CS_PRM_MON2_TIMER_MS     40

typedef struct _ST_OTE_CS_MON1 {
    int timer_ms = OTE_CS_PRM_MON1_TIMER_MS;
    HWND hwnd_mon;
    bool is_monitor_active = false;

    HWND hctrl[OTE_CS_MON1_N_CTRL] = {
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
    };
    POINT pt[OTE_CS_MON1_N_CTRL] = {
        5,5, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,//Static
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        5,200, 100,200, 200,200, 0,0, 0,0, 0,0, 0,0, 0,0,//PB
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0
    };
    SIZE sz[OTE_CS_MON1_N_CTRL] = {
        320,180, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,//Static
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        80,20, 100,40, 100,40, 0,0, 0,0, 0,0, 0,0, 0,0,//PB
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0
    };
    WCHAR text[OTE_CS_MON1_N_CTRL][OTE_CS_MON1_N_WCHAR] = {
        L"MSG:", L"INF", L"REQ R", L"RES R", L"REQ W", L"RES W", L"", L"",
        L"", L"", L"", L"", L"", L"", L"", L"",
        L"ﾊﾞｲﾌﾞ", L"右", L"左", L"", L"", L"", L"", L"",//PB
        L"", L"", L"", L"", L"", L"", L"", L""
    };
}ST_OTE_CS_MON1, * LPST_OTE_CS_MON1;

#define OTE_CS_MON2_WND_X     OTE_CS_MON1_WND_X
#define OTE_CS_MON2_WND_Y     620   
#define OTE_CS_MON2_WND_W     640
#define OTE_CS_MON2_WND_H     360
#define OTE_CS_MON2_N_CTRL    32
#define OTE_CS_MON2_N_WCHAR   64

#define OTE_CS_MON2_MSG_DISP_OFF 0
#define OTE_CS_MON2_MSG_DISP_HEX 1
#define OTE_CS_MON2_MSG_DISP_DEC 2

#define N_OTE_CS_MON2_MSG_RBLK   8
#define N_OTE_CS_MON2_MSG_WBLK   8

typedef struct _ST_OTE_CS_MON2 {
    HWND hwnd_mon;
    int timer_ms = OTE_CS_PRM_MON2_TIMER_MS;
    bool is_monitor_active = false;
    int msg_disp_mode = OTE_CS_MON2_MSG_DISP_HEX;
    int read_disp_block = 0;
    int write_disp_block = 0;

    wostringstream wo_req_r, wo_res_r, wo_req_w, wo_res_w;

    HWND hctrl[OTE_CS_MON2_N_CTRL] = {
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
    };
    POINT pt[OTE_CS_MON2_N_CTRL] = {
        5,5, 5,30, 5,55, 5,80, 5,185, 5,290,0,0, 0,0,//Static
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        570,55, 570,290, 510,5, 565,5, 0,0, 0,0, 0,0, 0,0,//PB
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0
    };
    SIZE sz[OTE_CS_MON2_N_CTRL] = {
        615,20, 615,20, 565,20, 615,100, 615,100,565,20, 0,0, 0,0,//Static
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        50,20, 50,20, 50,20, 50,20, 0,0, 0,0, 0,0, 0,0,//PB
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0
    };
    WCHAR text[OTE_CS_MON2_N_CTRL][OTE_CS_MON2_N_WCHAR] = {
        L"MSG:", L"INF", L"REQ R", L"RES R", L"REQ W", L"RES W", L"", L"",
        L"", L"", L"", L"", L"", L"", L"", L"",
        L"次R", L"次W", L"非表示", L"10進", L"", L"", L"", L"",//PB
        L"", L"", L"", L"", L"", L"", L"", L""
    };
}ST_OTE_CS_MON2, * LPST_OTE_CS_MON2;
typedef struct _ST_OTE_CS_OBJ {
    CValue<INT16> remote_pb;
    CValue<INT16> remote_mode;
    CValue<INT16> game_pad_pb;
    CValue<INT16> ote_type;

    CValue<INT16> syukan_on;
    CValue<INT16> syukan_off;
    CValue<INT16> estop;
    CValue<INT16> f_reset;
    CValue<INT16> bypass;
    CValue<INT16> kidou_r;
    CValue<INT16> kidou_l;
    CValue<INT16> pan_l;
    CValue<INT16> pan_r;
    CValue<INT16> tilt_u;
    CValue<INT16> tilt_d;
    CValue<INT16> zoom_f;
    CValue<INT16> zoom_n;

    CValue<INT16> trig_l;
    CValue<INT16> trig_r;

    CPadNotch* pad_mh;
    CPadNotch* pad_bh;
    CPadNotch* pad_sl;
    CPadNotch* pad_gt;
    CPadNotch* pad_ah;
} ST_OTE_CS_OBJ, * LPST_OTE_CS_OBJ;

class COteCS :public CBasicControl
{
public:
    COteCS();
    ~COteCS();

    virtual HRESULT initialize(LPVOID lpParam) override;

    LRESULT CALLBACK PanelProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);

    static LRESULT CALLBACK Mon1Proc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);
    static LRESULT CALLBACK Mon2Proc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);

    static ST_OTE_CS_MON1 st_mon1;
    static ST_OTE_CS_MON2 st_mon2;

    //タスク出力用構造体
    static ST_OTE_CS_INF st_work;
    static ST_OTE_CS_OBJ st_obj;

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
    int parse();
    int output();
    int close();
};

