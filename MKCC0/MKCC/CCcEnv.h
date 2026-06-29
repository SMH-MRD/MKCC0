#pragma once
#include "CBasicControl.h"
#include "framework.h"
#include "CSHAREDMEM.H"
#include "SmemMain.H"

#define ENV_MON1_WND_X     0
#define ENV_MON1_WND_Y     0
#define ENV_MON1_WND_W     320
#define ENV_MON1_WND_H     240
#define ENV_MON1_N_CTRL    32
#define ENV_MON1_N_WCHAR   64

#define ENV_ID_MON1_CTRL_BASE   50100
#define ENV_ID_MON1_STATIC_GPAD     0

#define ENV_ID_MON1_TIMER  50190
#define ENV_ID_MON2_TIMER  50191

#define ENV_PRM_MON1_TIMER_MS  200
#define ENV_PRM_MON2_TIMER_MS  50

typedef struct _ST_ENV_MON1 {
    bool is_monitor_active = false;

    int timer_ms = ENV_PRM_MON1_TIMER_MS;
    HWND hwnd_mon;
    HWND hctrl[ENV_MON1_N_CTRL] = {
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
    };
    POINT pt[ENV_MON1_N_CTRL] = {
        5,5, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0
    };
    SIZE sz[ENV_MON1_N_CTRL] = {
        295,190, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0
    };
    WCHAR text[ENV_MON1_N_CTRL][ENV_MON1_N_WCHAR] = {
        L"ENV_MON1", L"", L"", L"", L"", L"", L"", L"",
        L"", L"", L"", L"", L"", L"", L"", L"",
        L"", L"", L"", L"", L"", L"", L"", L"",
        L"", L"", L"", L"", L"", L"", L"", L""
    };
}ST_ENV_MON1, * LPST_ENV_MON1;

#define ENV_MON2_WND_X     ENV_MON1_WND_X
#define ENV_MON2_WND_Y     620   
#define ENV_MON2_WND_W     640
#define ENV_MON2_WND_H     265

#define ENV_MON2_N_CTRL    32
#define ENV_MON2_N_WCHAR   64

#define ENV_ID_MON2_CTRL_BASE   50140

#define ENV_ID_MON2_LABEL_SOCK      0
#define ENV_ID_MON2_STATIC_SOCK     1
#define ENV_ID_MON2_STATIC_MSG      2
#define ENV_ID_MON2_RADIO_RCV       8
#define ENV_ID_MON2_RADIO_SND       9
#define ENV_ID_MON2_RADIO_INFO      10

typedef struct _ST_ENV_MON2 {
    HWND hwnd_mon;
    int timer_ms = ENV_PRM_MON2_TIMER_MS;
    int sock_inf_id = ENV_ID_MON2_RADIO_RCV; //ソケット情報を表示する内容コード

    bool is_monitor_active = false;

    wostringstream wo_uni, wo_work;

    HWND hctrl[ENV_MON2_N_CTRL] = {
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
    };
    POINT pt[ENV_MON2_N_CTRL] = {
        5,95, 5,50, 5,5, 0,0, 0,0, 0,0, 0,0, 0,0,
        470,5,520,5,570,5, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0
    };
    SIZE sz[ENV_MON2_N_CTRL] = {
        ENV_MON2_WND_W - 25,120, ENV_MON2_WND_W - 25,40, ENV_MON2_WND_W - 25,40, 0,0, 0,0, 0,0, 0,0,
        ENV_MON2_WND_W - 180,20,40,20,40,20, 40,20,  0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0
    };
    WCHAR text[ENV_MON2_N_CTRL][ENV_MON2_N_WCHAR] = {
        L"UNI:", L"-", L"MSG:", L"-", L"-", L"-", L"-", L"",
        L"RCV", L"SND",L"INFO",  L"", L"", L"", L"", L"",
        L"", L"", L"", L"", L"", L"", L"", L"",
        L"", L"", L"", L"", L"", L"", L"", L""
    };

}ST_ENV_MON2, * LPST_ENV_MON2;

class CCcEnv : public CBasicControl
{
public:
    CCcEnv() ;
    ~CCcEnv();

    static ST_ENV_MON1 st_mon1;
    static ST_ENV_MON2 st_mon2;
    static ST_CC_ENV_INF st_work;//タスク出力用構造体

    virtual HRESULT initialize(LPVOID lpParam) override;

    LRESULT CALLBACK PanelProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);
    static LRESULT CALLBACK Mon1Proc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);
    static LRESULT CALLBACK Mon2Proc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);
    
    virtual void set_panel_tip_txt() override;          //タブパネルのStaticテキストを設定
    virtual void set_func_pb_txt() override;            //タブパネルのFunctionボタンのStaticテキストを設定
    virtual void set_item_chk_txt() override;           //タブパネルのItem chkテキストを設定
    virtual void msg2listview(wstring wstr) override;   //タブパネルのListViewにコメント出力
    virtual void set_PNLparam_value(float p1, float p2, float p3, float p4, float p5, float p6) override;//パラメータ初期表示値設定
    virtual void reset_panel_func_pb(HWND hDlg) override { return; };//タブパネルのFunctionボタンのリセット

private:
    static CSpec* pspec;

    INT16 plc_falt_detected_hold[N_PLC_FAULT_BUF];
    INT16 plc_falt_detected_trig_on[N_PLC_FAULT_BUF];
    INT16 plc_falt_detected_trig_off[N_PLC_FAULT_BUF];
    INT16 plc_enable_hold = 0;

    INT16 pc_falt_detected_hold[N_PC_FAULT_BUF];
    INT16 pc_falt_detected_trig_on[N_PC_FAULT_BUF];
    INT16 pc_falt_detected_trig_off[N_PC_FAULT_BUF];
    INT16 pc_enable_hold = 0;

    double lbh_d0;//起伏入限時の起伏ドラム巻取り量

    //オーバーライド
    virtual HRESULT routine_work(void* pObj) override;
    int input();//入力処理
    int parse();
    int output();
    int close();
    int set_outbuf(LPVOID) {//出力バッファセット
        return STAT_NG;
    }

	void set_faults_info();
    void refresh_faults_info();

 //   void set_drum_param(int id);
  //  static HRESULT set_drum_stat(int id);
	HRESULT(*fp_set_drum_stat)(int id) = NULL; //ドラムの状態をセットする関数ポインタ   

    void set_param_JC(int id);
    void set_param_GC(int id);
    void set_param_OHC(int id);
    HRESULT(*fp_set_stat)(int id) = NULL;       //クレーンの状態をセットする関数ポインタ   
    static HRESULT set_stat_JC(int id);
    static HRESULT set_stat_GC(int id);
    static HRESULT set_stat_OHC(int id);
 

    HWND open_monitor_wnd(HWND h_parent_wnd, int id);
    void close_monitor_wnd(int id);
    void show_monitor_wnd(int id);
    void hide_monitor_wnd(int id);
};


