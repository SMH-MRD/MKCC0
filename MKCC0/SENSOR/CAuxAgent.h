#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <ws2tcpip.h>
#include "framework.h"

#include "CBasicControl.h"

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

typedef struct _TILT_AUXEQ {
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
}ST_TILT_AUXEQ,*LPST_TILT_AUXEQ;

#define TILT_MON1_WND_X     640
#define TILT_MON1_WND_Y     0
#define TILT_MON1_WND_W     320
#define TILT_MON1_WND_H     240
#define TILT_MON1_N_CTRL    32
#define TILT_MON1_N_WCHAR   64

#define TILT_ID_MON1_CTRL_BASE   75100
#define TILT_ID_MON1_STATIC_INF     0

#define TILT_ID_MON2_CTRL_BASE   75140

#define TILT_ID_MON1_TIMER  75190
#define TILT_ID_MON2_TIMER  75191

#define TILT_PRM_MON1_TIMER_MS  200
#define TILT_PRM_MON2_TIMER_MS  200

typedef struct _TILT_MON1 {
	int timer_ms = TILT_PRM_MON1_TIMER_MS;
	HWND hwnd_mon;
    HWND hctrl[TILT_MON1_N_CTRL] = {
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
    };
    POINT pt[TILT_MON1_N_CTRL] = {
        5,5, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0
    };
    SIZE sz[TILT_MON1_N_CTRL] = {
        295,190, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0
    };
    WCHAR text[TILT_MON1_N_CTRL][TILT_MON1_N_WCHAR] = {
        L"INFOMATION", L"", L"", L"", L"", L"", L"", L"",
        L"", L"", L"", L"", L"", L"", L"", L"",
        L"", L"", L"", L"", L"", L"", L"", L"",
        L"", L"", L"", L"", L"", L"", L"", L""
    };
}ST_TILT_MON1, * LPST_TILT_MON1;

#define TILT_MON2_WND_X     TILT_MON1_WND_X
#define TILT_MON2_WND_Y     TILT_MON1_WND_Y + TILT_MON1_WND_H   
#define TILT_MON2_WND_W     320
#define TILT_MON2_WND_H     240

typedef struct _TILT_MON2 {
    HWND hwnd_mon;

}ST_TILT_MON2, * LPST_TILT_MON2;

class CAuxAgent :  public CBasicControl
{
public:
    CAuxAgent() { pst_work = &(st_work); };
    ~CAuxAgent() {};
    

    virtual HRESULT initialize(LPVOID lpParam) override;

    LRESULT CALLBACK PanelProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);

    static LRESULT CALLBACK Mon1Proc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);
    static LRESULT CALLBACK Mon2Proc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);

    static LPST_TILT_AUXEQ pst_work;

	static ST_TILT_MON1 st_mon1;
    static ST_TILT_MON2 st_mon2;

    ST_TILT_AUXEQ st_work;

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

