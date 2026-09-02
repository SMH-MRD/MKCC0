#pragma once

#include "CBasicControl.h"

#include "framework.h"
#include "CSHAREDMEM.H"
#include "SmemMain.H"
#include "SmemOte.H"
#include "CMob.H"

#define SIM_MON1_WND_X     640
#define SIM_MON1_WND_Y     0
#define SIM_MON1_WND_W     320
#define SIM_MON1_WND_H     240
#define SIM_MON1_N_CTRL    32
#define SIM_MON1_N_WCHAR   64

#define SIM_ID_MON1_CTRL_BASE   56100
#define SIM_ID_MON1_STATIC_INF0     0

#define SIM_ID_MON2_CTRL_BASE   56140

#define SIM_ID_MON1_TIMER  56190
#define SIM_ID_MON2_TIMER  56191

#define SIM_PRM_MON1_TIMER_MS  200
#define SIM_PRM_MON2_TIMER_MS  200

#define SIM_PRM_WIND_SPEED_DEFAULT      5.5     //風速[m/s]初期値
#define SIM_PRM_WIND_DIR_DEFAULT        10.0    //風向[deg]初期値
#define SIM_PRM_BHR_DEFAULT             25.0    //旋回半径[m]初期値
#define SIM_PRM_MH＿LOAD_DEFAULT         0.0    //吊荷荷重[ton]初期値
#define SIM_PRM_GT_POS_DEFAULT          50.0    //走行位置[m]初期値
#define SIM_PRM_MH_HEIGHT_DEFAULT       30.0    //揚程[m]初期値

typedef struct _ST_SIM_MON1 {
    int timer_ms = SIM_PRM_MON1_TIMER_MS;
    wostringstream wo;

    HWND hwnd_mon;
    HWND hctrl[SIM_MON1_N_CTRL] = {
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
    };
    POINT pt[SIM_MON1_N_CTRL] = {
        5,5, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0
    };
    SIZE sz[SIM_MON1_N_CTRL] = {
        295,190, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0
    };
    WCHAR text[SIM_MON1_N_CTRL][SIM_MON1_N_WCHAR] = {
        L"-", L"", L"", L"", L"", L"", L"", L"",
        L"", L"", L"", L"", L"", L"", L"", L"",
        L"", L"", L"", L"", L"", L"", L"", L"",
        L"", L"", L"", L"", L"", L"", L"", L""
    };
}ST_SIM_MON1, * LPST_SIM_MON1;

#define SIM_MON2_WND_X     SIM_MON1_WND_X
#define SIM_MON2_WND_Y     SIM_MON1_WND_Y + SIM_MON1_WND_H   
#define SIM_MON2_WND_W     320
#define SIM_MON2_WND_H     240

typedef struct _ST_SIM_MON2 {
    HWND hwnd_mon;

}ST_SIM_MON2, * LPST_SIM_MON2;

typedef struct _ST_SIM_LOAD {//負荷
    double m;	//質量
    double wx;	//幅
    double dy;	//奥行
    double hz;	//高さ
}ST_SIM_LOAD, * LPST_SIM_LOAD;

typedef struct _ST_SIM_AXIS {//軸
    double mtrq;					//モータトルクfb
    ST_SIM_LOAD load;				//軸負荷（巻は荷重）
    ST_MOVE_SET	ref;				//軸指令
    ST_MOVE_SET	fb;					//軸座標
    ST_MOVE_SET	nd;					//ドラム回転動作(速度はrps)
    INT32  i_layer;				    //ドラム現在層数
	INT32  hcount;				    //高速カウンタ値
	INT32  absocoder;				//アブソコーダ値
    double  n_layer;				//ドラム現在層巻取数
    double  l_drum;					//ドラム巻取り量
	INT32   brake;					//ブレーキ状態
	double RpsPGCntSec;             //1RPM時のPG１秒カウント値
    double  RpsABSOCntSec;              //1RPM時のアブソコーダ１秒カウント値
}ST_SIM_AXIS, * LPST_SIM_AXIS;

typedef struct _ST_CC_SIM_WORK {
    //共有メモリ出力
    DWORD mode;
    DWORD helthy_cnt;

    double dt;
    INT32 sl_cnt_pg360;//旋回360°のPGカウント値
 
    ST_JC_PLC_IO_R st_plc_w;			                //PLC IO書き込みIF
    
    ST_SIM_AXIS axis[MOTION_ID_MAX];				//軸計算値
    double weight_mh;					            //フック質量 
 
    Vector3 L, vL,L2, vL2;							//ﾛｰﾌﾟﾍﾞｸﾄﾙ(主巻振れ）(補巻振れ）
    double kbh;										//引込半径に依存する速度、加速度補正係数   
    ST_MOVE_SET	d;									//ポスト‐起伏シーブ間状態（距離・速度・加速度）
    ST_MOVE_SET	db;									//ジブポスト‐起伏シーブ間状態（距離・速度・加速度）
    ST_MOVE_SET	ph;									//φ
    ST_MOVE_SET	phb;								//φ
    ST_MOVE_SET	th;									//θ

    double rad_cam_x, rad_cam_y, w_cam_x, w_cam_y;	//カメラ座標振れ角,振れ角速度
    double T;										//主巻振れ周期
    double Tah;										//補巻振れ周期
    double w;										//主巻振れ角周波数
    double wah;										//補巻振れ角周波数
    double tht_swx_mh, tht_swy_mh, tht_swx_ah, tht_swy_ah;

    INT16 trq30[MOTION_ID_MAX] = { 600,600,600,600,600,600,600,600 };//ブレーキ解放用30%トルク設定値
    INT16 trq[MOTION_ID_MAX] = { 4000,4000,4000,4000,4000,4000,4000,4000 };  //トルク設定値

}ST_CC_SIM_WORK, * LPST_CC_SIM_WORK;

class CSim : public CBasicControl
{

public:
    CSim();
    ~CSim();

	int crane_id;
	int crane_type;
    virtual HRESULT initialize(LPVOID lpParam) override;

	CSimJC*     pSimJC  = nullptr;
    CSimGC*     pSimGC  = nullptr;
    CSimOHC*    pSimOHC = nullptr;

    LRESULT CALLBACK PanelProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);

    static LRESULT CALLBACK Mon1Proc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);
    static LRESULT CALLBACK Mon2Proc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);

    static ST_SIM_MON1 st_mon1;
    static ST_SIM_MON2 st_mon2;

    //タスク出力用構造体
    static ST_CC_SIM_INF st_sim_inf;
    static ST_CC_SIM_WORK st_work;

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
     static CSpec* pspec;

    void setup_JC(int id);
    void setup_GC(int id);
    void setup_OHC(int id);


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

	//クレーンタイプ別計算関数
	HRESULT init_drm_motion_JC(int id);         //ドラムパラメータ設定(巻取量,層数,速度,加速度）
    HRESULT set_sensor_fb_JC(int id);           //高速カウンタ,アブソコーダ,LS他
    
    HRESULT init_drm_motion_GC(int id);         //ドラムパラメータ設定(巻取量,層数,速度,加速度）
    HRESULT set_sensor_fb_GC(int id);           //高速カウンタ,アブソコーダ,LS他
    
    HRESULT init_drm_motion_OHC(int id);         //ドラムパラメータ設定(巻取量,層数,速度,加速度）
    HRESULT set_sensor_fb_OHC(int id);           //高速カウンタ,アブソコーダ,LS他
    
};