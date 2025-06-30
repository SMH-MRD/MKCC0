#pragma once

#include "CBasicControl.h"

#include "framework.h"
#include "CSHAREDMEM.H"

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
}ST_SIM_AXIS, * LPST_SIM_AXIS;

typedef struct _ST_CC_SIM_WORK {
    //共有メモリ出力
    DWORD mode;
    DWORD helthy_cnt;
 
    ST_AUX_ENV_INF sway_io;
	ST_PLC_IO_WIF st_plc_w;			                //PLC IO書き込みIF

    ST_SWAY_SERVER_MSG rcv_msg;
    ST_SWAY_CLIENT_MSG snd_msg;
    
    ST_SIM_AXIS axis[MOTION_ID_MAX];				//軸計算値

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

}ST_CC_SIM_WORK, * LPST_CC_SIM_WORK;

class CSim : public CBasicControl
{
public:
    CSim();
    ~CSim();

    virtual HRESULT initialize(LPVOID lpParam) override;

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

	HRESULT init_drm_motion();  //ドラムパラメータ設定(巻取量,層数,速度,加速度）
    HRESULT set_drm_condition();//ドラム状態設定(ブレーキ,極限,荷重）
	HRESULT set_drm_motion();   //ドラム動作設定(加速度,速度,位置,層）
    HRESULT set_sensor_fb();            //高速カウンタ,アブソコーダ,LS他
	
    HRESULT calc_axis_motion();         //軸荷動作計算
    HRESULT calc_load_motion();         //吊荷動作計算
	HRESULT calc_plc_output();          //軸状態出力（PLC IO用）

};