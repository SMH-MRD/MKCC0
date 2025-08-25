#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS
//#include <ws2tcpip.h>
#include "framework.h"

#include "CBasicControl.h"
#include "LALANIO.h"

//LANIO関連定義
#if 0

#define LANIO_TMOUT_MS 3000
#define LANIO_N_MODEL  16

#define LANIO_N_MAX             16       //取り扱うLANIO最大値

#define LANIO_MODEL_LA_2R3A     8
#define LANIO_MODEL_LA_2A3P_P   9
#define LANIO_MODEL_LA_2R3A_V2  10
#define LANIO_MODEL_LA_3A2P_P   11
#define LANIO_MODEL_LA_5AI      12

//LAN IO AIのレンジ設定コード
#define LANIO_CODE_AI_RANGE_100mV   0
#define LANIO_CODE_AI_RANGE_1V      1
#define LANIO_CODE_AI_RANGE_10V     2
#define LANIO_CODE_AI_RANGE_30V     3
#define LANIO_CODE_AI_RANGE_20mA250 4
#define LANIO_CODE_AI_RANGE_20mA50  5
#define LANIO_CODE_AI_RANGE_THERMO  6

//LAN IO AI AD変換速度コード
#define LANIO_CODE_AI_ADCSPS_10     0
#define LANIO_CODE_AI_ADCSPS_16_6   1
#define LANIO_CODE_AI_ADCSPS_50     2
#define LANIO_CODE_AI_ADCSPS_60     3
#define LANIO_CODE_AI_ADCSPS_400    4
#define LANIO_CODE_AI_ADCSPS_1200   5
#define LANIO_CODE_AI_ADCSPS_3600   6
#define LANIO_CODE_AI_ADCSPS_14400  7

#define LANIO_RANGE_AI_MIN_AMPEARE  0
#define LANIO_RANGE_AI_MAX_AMPEARE  8388607     //0x7FFFFF
#define LANIO_RANGE_AI_MIN_VOLT     -8388608    //0x800000
#define LANIO_RANGE_AI_MAX_VOLT     8388607     //0x7FFFFF

#define LANIO_N_CH_LA_5AI            5

#endif

typedef struct _ST_AUXEQ {
#if 0
    hLANIO hlanio[LANIO_N_MAX] = { -1,-1,-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1} ;
    int laniocount = 0;
    int timeout = LANIO_TMOUT_MS;
    int lanio_id[LANIO_N_MAX];
    int lanio_stat[LANIO_N_MAX] = { -1,-1,-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };
    int lanio_model[LANIO_N_MAX];
    int lanio_ai_data[LANIO_N_CH_LA_5AI] = { -1,-1,-1, -1, -1 };
    int lanio_ai_range[LANIO_N_CH_LA_5AI] = { LANIO_CODE_AI_RANGE_10V ,LANIO_CODE_AI_RANGE_10V  ,LANIO_CODE_AI_RANGE_20mA250,LANIO_CODE_AI_RANGE_10V ,LANIO_CODE_AI_RANGE_20mA250 };
    int lanio_sps[LANIO_N_MODEL] = {
        LANIO_CODE_AI_ADCSPS_50 ,LANIO_CODE_AI_ADCSPS_50 ,LANIO_CODE_AI_ADCSPS_50 ,LANIO_CODE_AI_ADCSPS_50 ,LANIO_CODE_AI_ADCSPS_50,LANIO_CODE_AI_ADCSPS_50 ,LANIO_CODE_AI_ADCSPS_50 ,LANIO_CODE_AI_ADCSPS_50 ,
        LANIO_CODE_AI_ADCSPS_50 ,LANIO_CODE_AI_ADCSPS_50 ,LANIO_CODE_AI_ADCSPS_50 ,LANIO_CODE_AI_ADCSPS_50 ,LANIO_CODE_AI_ADCSPS_50,LANIO_CODE_AI_ADCSPS_50 ,LANIO_CODE_AI_ADCSPS_50 ,LANIO_CODE_AI_ADCSPS_50
    };
    
    int tilt_x = 0;
    int tilt_y = 0;
    int tilt_z = 0;
    WCHAR model_text[LANIO_N_MODEL][16] = {
        L"",L"",L"",L"",L"",L"",L"",L"",
        L"LA-2R3A",L"LA-2A3P-P",L"LA-2R3A-V2",L"LA-3A2P-P",L"LA-5AI",L"",L"",L""
    };
#endif
	INT16 dummy = -1;
}ST_AUXEQ,*LPST_AUXEQ;

#define AUXAG_MON1_WND_X     640+640
#define AUXAG_MON1_WND_Y     0
#define AUXAG_MON1_WND_W     320
#define AUXAG_MON1_WND_H     240
#define AUXAG_MON1_N_CTRL    32
#define AUXAG_MON1_N_WCHAR   64

#define AUXAG_ID_MON1_CTRL_BASE   75100
#define AUXAG_ID_MON1_STATIC_INF     0

#define AUXAG_ID_MON1_TIMER             75190
#define AUXAG_ID_MON2_TIMER             75191

#define AUXAG_PRM_MON1_TIMER_MS         100
#define AUXAG_PRM_MON2_TIMER_MS         100

typedef struct _AUXAG_MON1 {
	int timer_ms = AUXAG_PRM_MON1_TIMER_MS;
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
        295,190, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
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

#define AUXAG_MON2_WND_X     AUXAG_MON1_WND_X
#define AUXAG_MON2_WND_Y     620   
#define AUXAG_MON2_WND_W     640
#define AUXAG_MON2_WND_H     400
#define AUXAG_MON2_N_CTRL    32
#define AUXAG_MON2_N_WCHAR   64

//MON2---------------------------------------------------
#define AUXAG_ID_MON2_CTRL_BASE         75140
#define AUXAG_ID_MON2_STATIC_MSG        0   //メッセージ表示部
#define AUXAG_ID_MON2_STATIC_INF        1   //接続情報表示部
#define AUXAG_ID_MON2_STATIC_REQ_R      2   //読込要求メッセージ
#define AUXAG_ID_MON2_STATIC_RES_R      3   //読込応答メッセージ
#define AUXAG_ID_MON2_STATIC_REQ_W      4   //書込要求メッセージ
#define AUXAG_ID_MON2_STATIC_RES_W      5   //書込応答メッセージ


#define AGENT_ID_MON2_CB_COM_LEVEL_BIT0     16   //読み込み表示ブロック切替PB
#define AGENT_ID_MON2_CB_COM_LEVEL_BIT1     17  //読み込み表示ブロック切替PB
#define AGENT_ID_MON2_CB_COM_LEVEL_BIT2     18  //メッセージ表示/非表示切替PB
#define AGENT_ID_MON2_CB_COM_LEVEL_BIT3     19 //10進/16進表示切替PB
#define AGENT_ID_MON2_CB_COM_HW_BRK         20   //読み込み表示ブロック切替PB
#define AGENT_ID_MON2_CB_COM_RST            21  //読み込み表示ブロック切替PB
#define AGENT_ID_MON2_CB_COM_EMG            22  //メッセージ表示/非表示切替PB
#define AGENT_ID_MON2_CB_COM_AUTOSEL        23 //10進/16進表示切替PB



#define AUXAG_MON2_MSG_DISP_OFF 0
#define AUXAG_MON2_MSG_DISP_HEX 1
#define AUXAG_MON2_MSG_DISP_DEC 2


#define AUXAG_MON2_MSG_DISP_N__DATAROW      4
#define AUXAG_MON2_MSG_DISP_N_DATA_COLUMN   10

typedef struct _AUXAG_MON2 {
    HWND hwnd_mon;
    int timer_ms = AUXAG_PRM_MON2_TIMER_MS;
    bool is_monitor_active = false;
    int msg_disp_mode = AUXAG_MON2_MSG_DISP_HEX;

    wostringstream wo_req_r, wo_res_r, wo_req_w, wo_res_w;

    HWND hctrl[AUXAG_MON2_N_CTRL] = {
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
    };
    POINT pt[AUXAG_MON2_N_CTRL] = {
        5,5, 5,30, 5,55, 5,100, 5,205, 5,330,0,0, 0,0,//Static
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        570,75, 570,330, 510,5, 565,5, 0,0, 0,0, 0,0, 0,0,//PB
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0
    };
    SIZE sz[AUXAG_MON2_N_CTRL] = {
        615,20, 615,20, 565,40, 615,100, 615,120,565,20, 0,0, 0,0,//Static
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        50,20, 50,20, 50,20, 50,20, 0,0, 0,0, 0,0, 0,0,//PB
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0
    };
    WCHAR text[AUXAG_MON2_N_CTRL][AUXAG_MON2_N_WCHAR] = {
        L"MSG:", L"INF", L"REQ R", L"RES R", L"REQ W", L"RES W", L"", L"",
        L"", L"", L"", L"", L"", L"", L"", L"",
        L"次R", L"次W", L"非表示", L"10進", L"", L"", L"", L"",//PB
        L"", L"", L"", L"", L"", L"", L"", L""
    };

}ST_AUXAG_MON2, * LPST_AUXAG_MON2;

class CAuxAgent :  public CBasicControl
{
public:
    CAuxAgent() { pst_work = &(st_work); };
    ~CAuxAgent() {};
    

    virtual HRESULT initialize(LPVOID lpParam) override;

    LRESULT CALLBACK PanelProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);

     static LRESULT CALLBACK Mon1Proc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);
    static LRESULT CALLBACK Mon2Proc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);

    static LPST_AUXEQ pst_work;

	static ST_AUXAG_MON1 st_mon1;
    static ST_AUXAG_MON2 st_mon2;

    ST_AUXEQ st_work;

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

