#pragma once
#include "CBasicControl.h"
#include "framework.h"
#include "CSHAREDMEM.H"
#include "SWYSENSOR_DEF.H"

//============================================================================
// define定義
#pragma region CONSTANT_DEFINITION

#define CODE_POL_MAINTE_OFF         0
#define CODE_POL_MAINTE_COMCHECK    21

#define MOVE_AVERAGE_COUNT      1   // 移動平均数

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

//////////////////////////////////////////////////////////////////////////////
// CAuxPol

class CAuxPol : public CBasicControl
{
public:
    CAuxPol();
    ~CAuxPol();

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
    int slbrk_enable;
    int lanio_enable;
    int sway_sensor_enable;
    int gt_sensor_enable;
 
    static CONFIG_COMMON    m_cnfgcmn;      // 共通設定
    static CONFIG_CAMERA    m_cnfgcam;      // カメラ設定
    static CONFIG_MOUNTING  m_cnfgmnt;      // 取付寸法設定
    static CONFIG_IMGPROC   m_cnfgprc;      // 画像処理条件設定
    static INFO_ADJUST_DATA m_infoajs_data; // 調整情報データ
    static INFO_IMGPRC_DATA m_infoprc_data; // 画像処理情報データ

    static int32_t maintenance_mode;

    static ST_MOVE_AVE_DATA m_move_avrg_data; // 輝度移動平均データ
    SWAY_ZERO_DATA    m_sway_zero_data; // 振れ中心計測データ

    LARGE_INTEGER m_cycle_time_counter; // パフォーマンスカウンター現在値
    int32_t       m_cycle_time;         // 実行周期[ms]

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

    void proc_comchk_mode();    // 制御PCとのIF CHECK　MODE

    BOOL proc_center_gravity(std::vector<std::vector<cv::Point>> contours,
        double* outPosX,
        double* outPosY,
        int* outTgtSize,
        uint32_t sel); // 重心検出
    BOOL proc_center_gravity2(std::vector<std::vector<cv::Point>> contours,
        double* outPosX,
        double* outPosY,
        int* outTgtSize,
        cv::Size2i* size_expected,
        cv::Size2i* size_detected); // 重心検出

    void proc_sway(void);                   // 振れ検出処理

    double get_sway_zero(uint32_t idx);                       // 振れゼロ点設定処理
    double get_sway_zero();                                    // 振れゼロ点設定処理

    void set_expstime();                // シャッタコントロール

    void set_tab_func_txt(void);            // タブパネルのRadio ButtonのStaticテキストを設定
    void set_tab_tip_txt(void);              // タブパネルの説明用Staticテキストを設定
    void set_tab(HWND hDlg,
        uint32_t msg,
        WPARAM wp,
        LPARAM lp);        // タブパネルの設定
};

