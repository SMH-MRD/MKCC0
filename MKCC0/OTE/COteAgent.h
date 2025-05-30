#pragma once
#include "CBasicControl.h"
#include "framework.h"
#include "CSHAREDMEM.H"
#include "CPanelIo.h"


#define OTE_AG_SEMIAUTO_TG_SEL_DEFAULT      0
#define OTE_AG_SEMIAUTO_TG_SEL_CLEAR		0
#define OTE_AG_SEMIAUTO_TG_SEL_ACTIVE       1
#define OTE_AG_SEMIAUTO_TG_SEL_FIXED        2
#define OTE_AG_SEMIAUTO_TG_MAX				16

#define OTE_AG_ID_SEMIAUTO_TOUCH_PT			8

#define OTE_AG_ID_NOTCH_POS_HOLD			0
#define OTE_AG_ID_NOTCH_POS_TRIG			1

#define OTE_AG_CODE_OTE_REMOTE_ENABLE		0x0001	//PLCへの操作信号有効
#define OTE_AG_CODE_OTE_SOURCE_ENABLE		0x0002	//主幹ON可能

#define OTE_AG_ID_MON1_TIMER  61190
#define OTE_AG_ID_MON2_TIMER  61191

#define OTE_AG_PRM_MON1_TIMER_MS  200
#define OTE_AG_PRM_MON2_TIMER_MS  200

#define OTE_AG_MON1_WND_X     640
#define OTE_AG_MON1_WND_Y     0
#define OTE_AG_MON1_WND_W     320
#define OTE_AG_MON1_WND_H     240

#define OTE_AG_MON1_N_CTRL    32
#define OTE_AG_MON1_N_WCHAR   128

#define OTE_AG_ID_MON1_CTRL_BASE   65100
#define OTE_AG_ID_MON1_STATIC_1      0
#define OTE_AG_ID_MON1_GDI_STR_1     1


typedef struct _ST_OTE_AG_MON1 {
    int timer_ms = OTE_AG_PRM_MON1_TIMER_MS;
    HWND hwnd_mon;
    HDC hdc;
    Graphics *pgraphic;
    CStringGdi* str_message;

    bool is_monitor_active = false;

    HWND hctrl[OTE_AG_MON1_N_CTRL] = {
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
    };
    POINT pt[OTE_AG_MON1_N_CTRL] = {
        5,5, 5,5, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0
    };
    SIZE sz[OTE_AG_MON1_N_CTRL] = {
        295,190, 295,190, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0
    };
    WCHAR text[OTE_AG_MON1_N_CTRL][OTE_AG_MON1_N_WCHAR] = {
        L"MON1", L"String", L"", L"", L"", L"", L"", L"",
        L"", L"", L"", L"", L"", L"", L"", L"",
        L"", L"", L"", L"", L"", L"", L"", L"",
        L"", L"", L"", L"", L"", L"", L"", L""
    };
}ST_OTE_AG_MON1, * LPST_OTE_AG_MON1;

#define OTE_AG_MON2_WND_X     OTE_AG_MON1_WND_X
#define OTE_AG_MON2_WND_Y     620   
#define OTE_AG_MON2_WND_W     640
#define OTE_AG_MON2_WND_H     345

#define OTE_AG_MON2_N_CTRL    32
#define OTE_AG_MON2_N_WCHAR   64

#define OTE_AG_ID_MON2_CTRL_BASE   65140
#define OTE_AG_ID_MON2_STATIC_UNI      0
#define OTE_AG_ID_MON2_STATIC_MUL_PC   1
#define OTE_AG_ID_MON2_STATIC_MUL_OTE  2
#define OTE_AG_ID_MON2_LABEL_UNI       3
#define OTE_AG_ID_MON2_LABEL_MUL_PC    4
#define OTE_AG_ID_MON2_LABEL_MUL_OTE   5
#define OTE_AG_ID_MON2_STATIC_MSG      6

#define OTE_AG_ID_MON2_RADIO_RCV       16
#define OTE_AG_ID_MON2_RADIO_SND       17
#define OTE_AG_ID_MON2_RADIO_INFO      18

#define OTE_AG_ID_MON2_CB_BODY         19
#define OTE_AG_ID_MON2_PB_PAGE_UP_UNI  20
#define OTE_AG_ID_MON2_PB_PAGE_UP_MPC  21
#define OTE_AG_ID_MON2_PB_PAGE_UP_MOT  22

#define OTE_AG_ID_MON2_N_PAGE_UP_UNI   4
#define OTE_AG_ID_MON2_N_PAGE_UP_MPC   2
#define OTE_AG_ID_MON2_N_PAGE_UP_MOT   2

typedef struct _ST_OTE_AG_MON2 {
    HWND hwnd_mon;
    int timer_ms = OTE_AG_PRM_MON2_TIMER_MS;
    int sock_inf_id = OTE_AG_ID_MON2_RADIO_RCV; //ソケット情報を表示する内容コード

   int ipage_uni = 0, ipage_mpc = 0, ipage_mot = 0;

    bool is_monitor_active = false; //モニタ画面表示中フラグ
    bool is_body_disp_mode = false; // 電文表示モードBODY

  	wostringstream wo_uni,wo_mpc,wo_mote,wo_work;

    HWND hctrl[OTE_AG_MON2_N_CTRL] = {
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
    };
    POINT pt[OTE_AG_MON2_N_CTRL] = {
        5,50, 5,135, 5,220, 5,30, 5,115, 5,200, 5,5,0,0,  
        0,0, 0,0,  0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
         470,5,520,5,570,5, 380,285, 455,285, 510,285, 565,285, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0
    };
    SIZE sz[OTE_AG_MON2_N_CTRL] = {
        OTE_AG_MON2_WND_W - 25,60, OTE_AG_MON2_WND_W - 25,60, OTE_AG_MON2_WND_W - 25,60, OTE_AG_MON2_WND_W - 25,20, OTE_AG_MON2_WND_W - 25,20, OTE_AG_MON2_WND_W - 25,20, OTE_AG_MON2_WND_W - 180,20, 0,0,
        0,0, 0,0,  0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        40,20, 40,20, 40,20, 65,20, 45,20, 45,20, 45,20, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0
    };
    WCHAR text[OTE_AG_MON2_N_CTRL][OTE_AG_MON2_N_WCHAR] = {
       L"-", L"-", L"-", L"UNI:", L"MPC:", L"MOTE:",L"MSG:",L"", 
       L"", L"",  L"", L"", L"", L"", L"", L"",
       L"RCV",  L"SND",L"INFO", L"BODY", L"UNI+", L"MPC+", L"MOT+", L"",
       L"", L"", L"", L"", L"", L"", L"", L""
    };

}ST_OTE_AG_MON2, * LPST_OTE_AG_MON2;

class COteAgent : public CBasicControl
{
public:
	COteAgent();
	~COteAgent();
    virtual HRESULT initialize(LPVOID lpParam) override;

    virtual LRESULT CALLBACK PanelProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp) override;

    static LRESULT CALLBACK Mon1Proc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);
    static LRESULT CALLBACK Mon2Proc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);

    static ST_OTE_AG_MON1 st_mon1;
    static ST_OTE_AG_MON2 st_mon2;

    //タスク出力用構造体
    static ST_OTE_CC_IF st_work;

    static HRESULT rcv_uni_ote(LPST_PC_U_MSG pbuf);
    static HRESULT rcv_mul_pc(LPST_PC_M_MSG pbuf);
    static HRESULT rcv_mul_ote(LPST_OTE_M_MSG pbuf);

    static LPST_OTE_U_MSG set_msg_u(BOOL is_ope_mode, INT32 code, INT32 stat);
    static HRESULT snd_uni2pc(LPST_OTE_U_MSG pbuf, SOCKADDR_IN* p_addrin_to);

    static LPST_OTE_M_MSG set_msg_m();
    static HRESULT snd_mul2pc(LPST_OTE_M_MSG pbuf);
    static HRESULT snd_mul2ote(LPST_OTE_M_MSG pbuf);
    
    int setup_crane_if(int crane_id);
    static int close_crane_if();

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
    static void update_sock_stat();

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
    int output();           //出力処理
    int close();
};


