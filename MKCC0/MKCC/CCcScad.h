#pragma once
#include "CBasicControl.h"
#include "framework.h"
#include "CSHAREDMEM.H"
#include "SmemMain.H"
#include "SmemAux.H"
#include "CMKLog.h"


#define SCAD_MON1_WND_X     1920
#define SCAD_MON1_WND_Y     0
#define SCAD_MON1_WND_W     1920
#define SCAD_MON1_WND_H     1080
#define SCAD_MON1_N_CTRL    64
#define SCAD_MON1_N_WCHAR   64

#define SCAD_ID_MON1_CTRL_BASE   63100
#define SCAD_ID_MON1_STATIC_GPAD     0

#define SCAD_ID_MON2_CTRL_BASE   63140

#define SCAD_ID_MON1_TIMER      63190
#define SCAD_ID_MON2_TIMER      63191

#define SCAD_PRM_MON1_TIMER_MS  100
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
    CScada();
    ~CScada();

    virtual HRESULT initialize(LPVOID lpParam) override;

    LRESULT CALLBACK PanelProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);

    static LRESULT CALLBACK Mon1Proc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);
    static LRESULT CALLBACK Mon2Proc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);

    static ST_SCAD_MON1 st_mon1;
    static ST_SCAD_MON2 st_mon2;

    static LPST_CC_ENV_INF		pEnv_Inf;
    static LPST_CC_CS_INF		pCS_Inf;
    static LPST_CC_PLC_IO		pPLC_IO ;
    static LPST_CC_AGENT_INF	pAgent_Inf;
    static LPST_CC_OTE_INF		pOTE_Inf;
    static LPST_CC_SIM_INF		pSim_Inf;
    static LPST_CC_POL_INF		pPolInf;
    static LPST_AUX_CS_INF		pAUX_CS_Inf;
    //タスク出力用構造体
    static ST_CC_SCAD_INF st_work;

    //Log関連
    static SYSTEMTIME systime;
    static ST_MKLOG_DB st_log_db;
    static ST_MKLOG_SOURCE  logsource;                                                     //Logデータ作成用設定データ
    static int log_elapse_count[MKLOG_N_ID_TYPE];
    static INT16(*get_func_of_logitem(INT32 code))(double d);

    int update_logsource_all(bool is_from_inifile);                                        //Log設定データの読み込み ini fileから読み込み 全ログ（初期化用）
    int update_logsource(int logtype, bool is_from_inifile);                               //Log設定データの読み込み ini fileから読み込みは選択制　個別ログ（設定更新用）
 
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

public:
    //LOG記録データセット用関数
    static INT16 empty(double d100);
    //位置FB
    static INT16 pos_fb_MH(double d100);
    static INT16 pos_fb_GT(double d100);
    static INT16 pos_fb_BH(double d100);
    static INT16 pos_fb_SL(double d100);
    static INT16 pos_fb_AH(double d100);

    //速度FB
    static INT16 v_fb_MH(double d100);
    static INT16 v_fb_GT(double d100);
    static INT16 v_fb_BH(double d100);
    static INT16 v_fb_SL(double d100);
    static INT16 v_fb_AH(double d100);

    static INT16 v_ref_MH(double d100);
    static INT16 v_ref_GT(double d100);
    static INT16 v_ref_BH(double d100);
    static INT16 v_ref_SL(double d100);
    static INT16 v_ref_AH(double d100);

    static INT16 swy1_TH_X(double d100);
    static INT16 swy1_TH_Y(double d100);
    static INT16 swy1DTH_X(double d100);
    static INT16 swy1DTH_Y(double d100);
    static INT16 swy1_Tx(double d100);
    static INT16 swy1_Ty(double d100);
    static INT16 swy1DTHWX(double d100);
    static INT16 swy1DTHWY(double d100);
    static INT16 swy1SWYAMP_X(double d100);
    static INT16 swy1SWYAMP_Y(double d100);
    static INT16 swy1SWYPH_X(double d100) ;
    static INT16 swy1SWYPH_Y(double d100) ;


    static INT16 l_rope_mh(double d100);
    static INT16 l_rope_ah(double d100);

    static INT16 plc_fault(double dindex) ;
    static INT16 plc_cab_bi(double dindex);

    static INT16 control_source(double d100);
    static INT16 e_stop(double d100);
    static INT16 auto_active_sw(double d100);

    static INT16 notch_com_mh(double d100);
    static INT16 notch_com_gt(double d100);
    static INT16 notch_com_bh(double d100);
    static INT16 notch_com_sl(double d100);
    static INT16 notch_com_ah(double d100);

    static INT16 onoff2hz(double d100);
    static INT16 onoff10hz(double d100);

    static INT16 sin1hz(double d100);
    static INT16 cos1hz(double d100);
    static INT16 sin1hz01hz(double d100);
    static INT16 cos1hz01hz(double d100);
};



