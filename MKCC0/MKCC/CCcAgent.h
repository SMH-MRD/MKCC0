#pragma once
#include "CBasicControl.h"
#include "framework.h"
#include "CSHAREDMEM.H"
#include "SmemMain.H"
#include "SmemAux.H"

#include "PLC_DEF.h"

//MON1----------------------------------------------------
#define AGENT_MON1_WND_X            0
#define AGENT_MON1_WND_Y            620
#define AGENT_MON1_WND_W            640
#define AGENT_MON1_WND_H            400
#define AGENT_MON1_N_CTRL           32
#define AGENT_MON1_N_WCHAR          64

#define AGENT_ID_MON1_CTRL_BASE     55500
#define AGENT_ID_MON1_STATIC_LABEL          0
#define AGENT_ID_MON1_STATIC_SLBRK_OTE      1
#define AGENT_ID_MON1_STATIC_SLBRK_COM      2
#define AGENT_ID_MON1_STATIC_SLBRK_FB       3

#define AGENT_ID_MON1_TIMER         55590
#define AGENT_PRM_MON1_TIMER_MS     150

typedef struct _ST_AGENT_MON1 {
    int timer_ms = AGENT_PRM_MON1_TIMER_MS;
    HWND hwnd_mon;
    bool is_monitor_active = false;
    wostringstream wo;

    HWND hctrl[AGENT_MON1_N_CTRL] = {
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
    };
    POINT pt[AGENT_MON1_N_CTRL] = {
        5,5, 5,30, 5,55, 5,80, 0,0, 0,0,0,0, 0,0,//Static
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,//PB
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0
    };
    SIZE sz[AGENT_MON1_N_CTRL] = {
        120,20, 615,20, 615,20, 615,20, 0,0, 0,0, 0,0, 0,0,//Static
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,//PB
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0
    };
    WCHAR text[AGENT_MON1_N_CTRL][AGENT_MON1_N_WCHAR] = {
        L"旋回ブレーキ:", L"OTE:", L"COM:", L"FB:", L"", L"", L"", L"",
        L"", L"", L"", L"", L"", L"", L"", L"",
        L"", L"", L"", L"", L"", L"", L"", L"",//PB
        L"", L"", L"", L"", L"", L"", L"", L""
    };
}ST_AGENT_MON1, * LPST_AGENT_MON1;

//MON2---------------------------------------------------
#define AGENT_MON2_WND_X            AGENT_MON1_WND_X
#define AGENT_MON2_WND_Y            620   
#define AGENT_MON2_WND_W            640
#define AGENT_MON2_WND_H            400
#define AGENT_MON2_N_CTRL           32
#define AGENT_MON2_N_WCHAR          64

#define AGENT_ID_MON2_TIMER         55591
#define AGENT_PRM_MON2_TIMER_MS     20

#define AGENT_MON2_MSG_DISP_OFF 0
#define AGENT_MON2_MSG_DISP_HEX 1
#define AGENT_MON2_MSG_DISP_DEC 2

#define AGENT_MON2_MSG_DISP_N__DATAROW      4
#define AGENT_MON2_MSG_DISP_N_DATA_COLUMN   10

#define AGENT_ID_MON2_CTRL_BASE         55540
#define AGENT_ID_MON2_STATIC_MSG        0   //メッセージ表示部
#define AGENT_ID_MON2_STATIC_INF        1   //接続情報表示部
#define AGENT_ID_MON2_STATIC_REQ_R      2   //読込要求メッセージ
#define AGENT_ID_MON2_STATIC_RES_R      3   //読込応答メッセージ
#define AGENT_ID_MON2_STATIC_REQ_W      4   //書込要求メッセージ
#define AGENT_ID_MON2_STATIC_RES_W      5   //書込応答メッセージ

#define AGENT_ID_MON2_PB_R_BLOCK_SEL    16   //読み込み表示ブロック切替PB
#define AGENT_ID_MON2_PB_W_BLOCK_SEL    17  //読み込み表示ブロック切替PB
#define AGENT_ID_MON2_PB_MSG_DISP_SEL   18  //メッセージ表示/非表示切替PB
#define AGENT_ID_MON2_PB_DISP_DEC_SEL   19 //10進/16進表示切替PB

typedef struct _ST_AGENT_MON2 {
    HWND hwnd_mon;
    int timer_ms = AGENT_PRM_MON2_TIMER_MS;
    bool is_monitor_active = false;
    int msg_disp_mode = AGENT_MON2_MSG_DISP_HEX;

    wostringstream wo_req_r, wo_res_r, wo_req_w, wo_res_w;

    HWND hctrl[AGENT_MON2_N_CTRL] = {
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
    };
    POINT pt[AGENT_MON2_N_CTRL] = {
        5,5, 5,30, 5,55, 5,100, 5,205, 5,330,0,0, 0,0,//Static
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        570,75, 570,330, 510,5, 565,5, 0,0, 0,0, 0,0, 0,0,//PB
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0
    };
    SIZE sz[AGENT_MON2_N_CTRL] = {
        615,20, 615,20, 565,40, 615,100, 615,120,565,20, 0,0, 0,0,//Static
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        50,20, 50,20, 50,20, 50,20, 0,0, 0,0, 0,0, 0,0,//PB
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0
    };
    WCHAR text[AGENT_MON2_N_CTRL][AGENT_MON2_N_WCHAR] = {
        L"MSG:", L"INF", L"REQ R", L"RES R", L"REQ W", L"RES W", L"", L"",
        L"", L"", L"", L"", L"", L"", L"", L"",
        L"次R", L"次W", L"非表示", L"10進", L"", L"", L"", L"",//PB
        L"", L"", L"", L"", L"", L"", L"", L""
    };
}ST_AGENT_MON2, * LPST_AGENT_MON2;

#define AGENT_PRM_SLBK_CHK_COUNT_STANDBY   20   //スレーブブレーキチェックシーケンスカウント値　400msec
#define AGENT_PRM_SLBK_CHK_COUNT_RUNNING   200  //スレーブブレーキチェックシーケンスカウント値　6sec

#define AGENT_AUTO_TRIG_ACK_COUNT                       10
#define AGENT_CHECK_LARGE_SWAY_m2                       1.0     //起動時に初期振れ大とみなす振れ量mの2乗
#define AGENT_CHECK_HST_POS_CLEAR_RANGE                 2.0     //自動巻上時に引込、旋回開始可能な巻上到達距離
#define AGENT_CHECK_BH_POS_CLEAR_HST_DOWN_RANGE         2.0     //自動巻下可能な引込到達距離m
#define AGENT_CHECK_SLW_POS_CLEAR_HST_DOWN_RANGE_rad    0.17    //自動巻下可能な旋回到達距離m
#define AGENT_CHECK_BH_POS_CLEAR_SLW_RANGE              3.0     //旋回開始可能な引込到達距離m
#define AGENT_CHECK_SLW_POS_CLEAR_BH_RANGE_rad          0.5    //引込開始可能な旋回到達距離rad

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
    static ST_CC_PLC_IO st_work_plcio;

    //PLC IF関連
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
	int crane_id = 0;
    static int ote_option_site_estop;
    static std::mutex m_AgInfMutex;  // 共有メモリアクセス保護用ミューテックス
    static bool is_site_estop_detected;

    static double cal_step(LPST_COMMAND_SET pCom, int motion);      //自動指令出力値の計算
	static double cal_dist4target(int motion, bool is_abs_answer);  //目標位置までの距離計算
    //オーバーライド
    virtual HRESULT routine_work(void* pObj) override;

    HWND open_monitor_wnd(HWND h_parent_wnd, int id);
    void close_monitor_wnd(int id);
    void show_monitor_wnd(int id);
    void hide_monitor_wnd(int id);


    HRESULT(*fp_trans_plc_io_read)(int id) = NULL;  //PLC読み出しデータを共有メモリに展開   
    HRESULT(*fp_plc_io_write)(int id) = NULL;       //ドラムの状態をセットする関数ポインタ  
    HRESULT(*fp_aux_equipment)(int id) = NULL;       //ドラムの状態をセットする関数ポインタ 

    HRESULT(*fp_set_ref_mh)(int id) = NULL;         //巻速度指令値出力関数ポインタ
    HRESULT(*fp_set_ref_ah)(int id) = NULL;         //補巻速度指令値出力関数ポインタ
    HRESULT(*fp_set_ref_x)(int id) = NULL;          //走行速度指令値出力関数ポインタ
    HRESULT(*fp_set_ref_ph)(int id) = NULL;         //旋回速度指令値出力関数ポインタ
    HRESULT(*fp_set_ref_y)(int id) = NULL;          //引込,トロリー速度指令値出力関数ポインタ

	static HRESULT trans_plc_io_read_JC(int crane_id);
    static HRESULT trans_plc_io_read_GC(int crane_id);
    static HRESULT trans_plc_io_read_OHC(int crane_id);

    static HRESULT plc_io_write_JC(int crane_id);
    static HRESULT plc_io_write_GC(int crane_id);
    static HRESULT plc_io_write_OHC(int crane_id);

	//速度指令値出力関数巻速度指令値出力
    static HRESULT set_ref_mh_JC(int crane_id);     //巻速度指令値出力
    static HRESULT set_ref_ah_JC(int crane_id);     //補巻速度指令値出力
    static HRESULT set_ref_gt_JC(int crane_id);     //走行速度指令値出力
    static HRESULT set_ref_slew_JC(int crane_id);   //旋回速度指令値出力
    static HRESULT set_ref_bh_JC(int crane_id);     //引込速度指令値出力

    static HRESULT set_ref_mh_GC(int crane_id);     //巻速度指令値出力
    static HRESULT set_ref_ah_GC(int crane_id);     //補巻速度指令値出力
    static HRESULT set_ref_gt_GC(int crane_id);     //走行速度指令値出力
    static HRESULT set_ref_slew_GC(int crane_id);   //旋回速度指令値出力
    static HRESULT set_ref_trolly_GC(int crane_id);  //トロリー速度指令値出力

    static HRESULT set_ref_mh_OHC(int crane_id);     //巻速度指令値出力
    static HRESULT set_ref_ah_OHC(int crane_id);     //補巻速度指令値出力
    static HRESULT set_ref_gt_OHC(int crane_id);     //走行速度指令値出力
    static HRESULT set_ref_slew_OHC(int crane_id);  //旋回速度指令値出力
    static HRESULT set_ref_trolly_OHC(int crane_id);     //トロリー速度指令値出力

	//補助機器の処理関数　リモコン非常停止，旋回ブレーキ等
    static HRESULT aux_equipment_JC(int crane_id);
    static HRESULT aux_equipment_GC(int crane_id);
    static HRESULT aux_equipment_OHC(int crane_id);

    int set_outbuf(LPVOID) {//出力バッファセット
        return STAT_NG;
    }

    int input();//入力処理
    int parse();            //メイン処理
    int output(); 
    int close();

	static int manage_slbrk();
};



