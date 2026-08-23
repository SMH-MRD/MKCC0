#pragma once
#include "CBasicControl.h"
#include "framework.h"
#include "CSHAREDMEM.H"
#include "SWYSENSOR_DEF.H"
#include "SmemAux.h"

//============================================================================
// define定義
#pragma region CONSTANT_DEFINITION

#define CODE_POL_MAINTE_OFF             0
#define CODE_POL_MAINTE_DBG_OVERRIDE    11
#define CODE_POL_MAINTE_COMCHECK        21
#define CODE_POL_MAINTE_DISP_P2P        31

#define MOVE_AVERAGE_COUNT      1           // 移動平均数

#pragma endregion CONSTANT_DEFINITION
//============================================================================
// 構造体/共用体定義
//----------------------------------------------------------------------------
#pragma region STRUCTURE_DEFINITION
// 振れ中心計測データ
typedef struct TAG_SWAY_ZERO_DATA {
    LARGE_INTEGER time_counter;         // 計測時間用パフォーマンスカウンター
    double        sway_min[static_cast<uint32_t>(ENUM_AXIS::E_MAX)];   // 振れ角最小値
    double        sway_max[static_cast<uint32_t>(ENUM_AXIS::E_MAX)];   // 振れ角最大値
    double        sway_zero[static_cast<uint32_t>(ENUM_AXIS::E_MAX)];  // 振れゼロ点
} SWAY_ZERO_DATA, * PSWAY_ZERO_DATA;
#pragma endregion STRUCTURE_DEFINITION

#define POL_MON1_WND_X     640
#define POL_MON1_WND_Y     0
#define POL_MON1_WND_W     320
#define POL_MON1_WND_H     240
#define POL_MON1_N_CTRL    32
#define POL_MON1_N_WCHAR   64

#define POL_ID_MON1_CTRL_BASE   74100
#define POL_ID_MON1_STATIC_1     0

#define POL_ID_MON2_CTRL_BASE   74140
#define POL_ID_MON2_STATIC_MSG     0


#define POL_ID_MON1_TIMER  74190
#define POL_ID_MON2_TIMER  74191

#define POL_PRM_MON1_TIMER_MS  200
#define POL_PRM_MON2_TIMER_MS  200


typedef struct _ST_POL_MON1 {
    int timer_ms = POL_PRM_MON1_TIMER_MS;
    bool is_monitor_active = false;
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
#define POL_MON2_N_CTRL    32
#define POL_MON2_N_WCHAR   64

#define POL_PRM_TG_DIST_DEFAULT 9.8
#define POL_PRM_T_DEFAULT       PI360
#define POL_PRM_W_DEFAULT       1.0

typedef struct _ST_POL_MON2 {
    HWND hwnd_mon;
    bool is_monitor_active = false;

    HWND hctrl[POL_MON2_N_CTRL] = {
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
    };
    POINT pt[POL_MON2_N_CTRL] = {
        5,5, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0
    };
    SIZE sz[POL_MON2_N_CTRL] = {
        295,190, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0
    };
    WCHAR text[POL_MON2_N_CTRL][POL_MON2_N_WCHAR] = {
        L"GAME_PAD", L"", L"", L"", L"", L"", L"", L"",
        L"", L"", L"", L"", L"", L"", L"", L"",
        L"", L"", L"", L"", L"", L"", L"", L"",
        L"", L"", L"", L"", L"", L"", L"", L""
    };

}ST_POL_MON2, * LPST_POL_MON2;

// 移動平均データ

#define N_AUXPOL_AVE_BUF_SIZE   20
typedef struct _ST_MOVE_AVE_DATA {
    double  data[N_AUXPOL_AVE_BUF_SIZE]; // データ
    int32_t wptr;                           // 書込みポインタ
    int32_t data_count;                     // データ数
    double  total_val;                      // 輝度積算
    double  max_val;                        // 最大輝度(移動平均後)
} ST_MOVE_AVE_DATA, * PST_MOVE_AVE_DATA;

#define POL_CODE_P2P_WAIT_F_PEAK        1
#define POL_CODE_P2P_WAIT_R_PEAK        -1
#define POL_CODE_P2P_WAIT_STOP          0

typedef struct _ST_SWAY_WORK {
    double  ph_delay_time;                              // 振れ速度検出のフィルタ他による位相遅れ補正時間
    double  sway_spd_last[(int)ENUM_AXIS::E_MAX];       // 振角速度前回値
    double  sway_peak_f[(int)ENUM_AXIS::E_MAX];         // 振れ角速度の符号が＋から－に変わったときの振れ角
    double  sway_peak_r[(int)ENUM_AXIS::E_MAX];         // 振れ角速度の符号が-から+に変わったときの振れ角
    int     peak_chk_flg[(int)ENUM_AXIS::E_MAX];        // 振れ角速度の符号切り替わり待ち状態フラグ
    double  sway_acc_chk_limit[(int)ENUM_AXIS::E_MAX];
    double  sway_acc_peak_chk_limit[(int)ENUM_AXIS::E_MAX];
    int     sway_T_task_count[(int)ENUM_AXIS::E_MAX];
    int     sway_target_range_over[(int)ENUM_AXIS::E_MAX];
    int     sway_peak_chk_count[(int)ENUM_AXIS::E_MAX];

} ST_SWAY_WORK, * PST_SWAY_WORK;



//////////////////////////////////////////////////////////////////////////////
// CAuxPol

class CAuxPol : public CBasicControl
{
public:
    CAuxPol();
    ~CAuxPol();

    static ST_SWAY_WORK st_sway_work;
     
    // メンバー関数
protected:
    static LRESULT CALLBACK Mon1Proc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);
    static LRESULT CALLBACK Mon2Proc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);

    static ST_POL_MON1 st_mon1;
    static ST_POL_MON2 st_mon2;
    
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

private:
    HRESULT init_sway_sensor();
    static int32_t maintenance_mode;
    static int32_t disp_mode;

    static ST_MOVE_AVE_DATA m_move_avrg_data; // 輝度移動平均データ
    SWAY_ZERO_DATA  m_sway_zero_data; // 振れ中心計測データ

    LARGE_INTEGER m_cycle_time_counter; // パフォーマンスカウンター現在値
    
    virtual HRESULT initialize(LPVOID lpParam) override;

    LRESULT CALLBACK PanelProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);


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

    void proc_dbg_override();    //ターゲット検出位置をデバッグ用にオーバーライド
    
	uint32_t get_opencv_image();// 画像ソース有無効判定

    //BOOL proc_center_gravity(std::vector<std::vector<cv::Point>> contours,
    //    double* outPosX,
    //    double* outPosY,
    //    int* outTgtSize,
    //    uint32_t sel); // 重心検出
    BOOL proc_center_gravity2(std::vector<std::vector<cv::Point>> contours,
        double* outPosX,
        double* outPosY,
        int* outTgtSize,
        cv::Size2i* size_expected,
        cv::Size2i* size_detected); // 重心検出

    void proc_sway(int idx);                   // 振れ検出処理

    double get_sway_p2p(int idx);              // p2pロジックで振れゼロ点,振幅,位相を求める

    void set_expstime();                // シャッタコントロール

};

