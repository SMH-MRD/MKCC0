#pragma once
#include "CBasicControl.h"

#include "framework.h"
#include "CSHAREDMEM.H"
#include "SmemMain.H"
#include "SmemAux.H"

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


#define PTN_ORDINARY                0x00000000  //通常
#define PTN_1SHOT_AS                0x00000001  //ワンショット振止
#define PTN_HALF_T_AS               0x00000002  //半周期振止
#define PTN_2SHOT_AS                0x00000004  //インチング振れ止め
#define PTN_2SHOT_MOVE0             0x00000008  //インチング移動(振れ抑制無し）
#define PTN_2SHOT_MOVE_PLUS         0x00000010  //インチング移動(振れ抑制有）
#define PTN_FULL_T                  0x00000020  //1周期振止

#define POLICY_PTN_OK               1
#define POLICY_PTN_NG               0

#define N_AUTO_PARAM                8

#define SPD_FB_DELAY_TIME           0.3             //速度指令-FB遅れ時間
#define FINE_POS_TIMELIMIT          50.0             //ファインポジショニング制限時間
#define POL_TM_OVER_CHECK_COUNTms   120000

#define POL_ID_START_POINT          0
#define POL_ID_END_POINT            1

#define POL_PRM_FB_DELAY_SLEW           0.3
#define POL_PRM_FB_DELAY_BH             0.3
#define POL_PRM_FB_DELAY_BH_SIM         0.3
#define POL_PRM_FB_DELAY_SLEW_SIM       0.3

#define CODE_POLICY_DEBUG_DEACTIVE           0
#define CODE_POLICY_DEBUG_AS_1SHOT          0x0001
#define CODE_POLICY_DEBUG_AS_2SHOT          0x0002
#define CODE_POLICY_DEBUG_SIM_MODE          0x0004

/// <summary>
/// Policyタスクのコマンドワーク構造体
/// </summary>
typedef struct stPolicyComWork {
    double T[MOTION_ID_MAX];	                                        //振れ周期
    double w[MOTION_ID_MAX];	                                        //振れ角周波数
    double w2[MOTION_ID_MAX];	                                        //振れ角周波数2乗
    double pos[MOTION_ID_MAX];	                                        //現在位置
    double v[MOTION_ID_MAX];	                                        //モータの現在速度
    double a_abs[MOTION_ID_MAX][2];	                                    //モータの加速度　絶対値(スタート位置,目標位置）
    double a_hp_abs[MOTION_ID_MAX][2];	                                //吊点の加速度　絶対値
    double vmax_abs[MOTION_ID_MAX];                                     //モータの最大速度
    double acc_time2Vmax[MOTION_ID_MAX];                                //最小加速時間
    double dec_time2Vmax[MOTION_ID_MAX];                                //最小減速時間
    double dist_for_target[MOTION_ID_MAX];                              //目標までの距離符号あり
    double dist_for_target_abs[MOTION_ID_MAX];                          //目標までの距離符号あり
    double pp_th0[MOTION_ID_MAX][ACCDEC_MAX];                           //位相平面の回転中心
    double vfb_delay_time[MOTION_ID_MAX] = { 0.3,0.3,0.3,POL_PRM_FB_DELAY_BH, POL_PRM_FB_DELAY_SLEW,0.3 }; //フィードバック遅れ時間調整値

    ST_POS_TARGETS target;                      //目標位置
    int motion_dir[MOTION_ID_MAX];              //移動方向
    unsigned int agent_scan_ms;                //AGENTタスクのスキャンタイム
    double agent_scan;                         //AGENTタスクのスキャンタイム 秒

}ST_POLICY_COM_WORK, * LPST_POLICY_COM_WORK;

class CCcPol : public CBasicControl
{
public:
    CCcPol();
    ~CCcPol();

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

    //AGENTアクセス関数
    LPST_COMMAND_SET req_command(LPST_JOB_SET pjob_set);         //Agentからの要求に応じて実行コマンドをセットして返す
    int update_command_status(LPST_COMMAND_SET pcom, int code);  //Agentからのコマンド実行状況報告を受付,次のコマンドあるときはそれを返す

    void set_dbg_mode(int command);
    int debug_mode;
 
private:
    int crane_id = 0;

    ST_CC_POL_INF       st_inf_work;
    ST_POLICY_COM_WORK  st_com_work;
    int command_id;

    HRESULT(*fp_fault_check)(int id) = NULL;  //異常チェック   
    static HRESULT fault_check_JC(int crane_id);
    static HRESULT fault_check_GC(int crane_id);
    static HRESULT fault_check_OHC(int crane_id);
    
    LPST_COMMAND_SET        setup_job_command(LPST_JOB_SET pjob, int icom);  //実行する半自動のコマンドをセットする
    LPST_POLICY_COM_WORK    set_com_workbuf(LPST_COMMAND_SET pcom);

    int set_seq_semiauto_bh(int jobtype, LPST_MOTION_SEQ pseq, bool is_fbtype, LPST_POLICY_COM_WORK pwork);
    int set_seq_semiauto_slw(int jobtype, LPST_MOTION_SEQ pseq, bool is_fbtype, LPST_POLICY_COM_WORK pwork);
    int set_seq_semiauto_mh(int jobtype, LPST_MOTION_SEQ pseq, bool is_fbtype, LPST_POLICY_COM_WORK pwork);
    int set_seq_semiauto_ah(int jobtype, LPST_MOTION_SEQ pseq, bool is_fbtype, LPST_POLICY_COM_WORK pwork);

    const double param_auto[MOTION_ID_MAX][N_AUTO_PARAM] =
    {
      {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
      {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
      {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
      {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
      {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
      {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
      {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
      {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}
    };

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



