#pragma once
#include "CBasicControl.h"
#include "framework.h"
#include "CSHAREDMEM.H"
#include "SmemOte.H"


#define OTE_SCAD_ID_MON1_TIMER          63190
#define OTE_SCAD_ID_MON2_TIMER          63191

#define OTE_SCAD_PRM_MON1_TIMER_MS      100
#define OTE_SCAD_PRM_MON2_TIMER_MS      200

#define OTE_SCAD_MON1_WND_X             1920
//#define OTE_SCAD_MON1_WND_X             0
#define OTE_SCAD_MON1_WND_Y             0
#define OTE_SCAD_MON1_WND_W             1920
//#define OTE_SCAD_MON1_WND_H             1080
#define OTE_SCAD_MON1_WND_H             1200

#define OTE_SCAD_MON1_N_CTRL            64
#define OTE_SCAD_MON1_N_WCHAR           64

#define OTE_SCAD_ID_MON1_CTRL_BASE      63100
#define OTE_SCAD_ID_MON1_STATIC_UID     0
#define OTE_SCAD_ID_MON1_STATIC_TYPE    1
#define OTE_SCAD_ID_MON1_LABEL_PCR      2
#define OTE_SCAD_ID_MON1_LABEL_PLCR     3
#define OTE_SCAD_ID_MON1_LABEL_PCS      4
#define OTE_SCAD_ID_MON1_LABEL_PLCS     5
#define OTE_SCAD_ID_MON1_STATIC_CRANE   6

#define OTE_SCAD_ID_MON1_CB_ESTP        16
#define OTE_SCAD_ID_MON1_PB_CTRL_SOURCE 17
#define OTE_SCAD_ID_MON1_PB_REMOTE      18
#define OTE_SCAD_ID_MON1_PB_MODE1       19
#define OTE_SCAD_ID_MON1_PB_MODE2       20
#define OTE_SCAD_ID_MON1_PB_AUTHENT     21
#define OTE_SCAD_ID_MON1_PB_TYPE_SET    22
#define OTE_SCAD_ID_MON1_PB_CONTROLER   23
#define OTE_SCAD_ID_MON1_PB_PAD         24
#define OTE_SCAD_ID_MON1_PB_ASSIST      25
#define OTE_SCAD_ID_MON1_PB_CONNECT     26

#define OTE_SCAD_ID_MON1_RADIO_FAULT    32
#define OTE_SCAD_ID_MON1_RADIO_SETTING  33
#define OTE_SCAD_ID_MON1_RADIO_COM      34
#define OTE_SCAD_ID_MON1_RADIO_CAMERA   35
#define OTE_SCAD_ID_MON1_RADIO_STAT     36
#define OTE_SCAD_ID_MON1_RADIO_CLEAR    37

#define OTE_SCAD_PB_W                   100
#define OTE_SCAD_PB_H1                  40
#define OTE_SCAD_PB_H2                  100

#define OTE_SCAD_PB_X1                  20
#define OTE_SCAD_PB_X2                  1780
#define OTE_SCAD_TEXT_X1                20
#define OTE_SCAD_TEXT_X2                1780

typedef struct _ST_OTE_SCAD_MON1 {
    int timer_ms = OTE_SCAD_PRM_MON1_TIMER_MS;
    HWND hwnd_mon;

    bool is_monitor_active = false;

    HWND hctrl[OTE_SCAD_MON1_N_CTRL] = {
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
    };
    POINT pt[OTE_SCAD_MON1_N_CTRL] = {
        OTE_SCAD_TEXT_X1,270, OTE_SCAD_TEXT_X2,615, OTE_SCAD_TEXT_X1,900, OTE_SCAD_TEXT_X1,930, 80,900, 80,930, OTE_SCAD_TEXT_X2,715, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        OTE_SCAD_PB_X2,30, OTE_SCAD_PB_X2,160, OTE_SCAD_PB_X1,30, OTE_SCAD_PB_X1,150, OTE_SCAD_PB_X1,195, OTE_SCAD_PB_X1,300, OTE_SCAD_PB_X2,645, OTE_SCAD_PB_X2,300,
        OTE_SCAD_PB_X2,345, OTE_SCAD_PB_X2,390, OTE_SCAD_PB_X2,760, 0,0, 0,0, 0,0, 0,0, 0,0,
        OTE_SCAD_PB_X1,500, OTE_SCAD_PB_X1,545, OTE_SCAD_PB_X1,590, OTE_SCAD_PB_X1,635, OTE_SCAD_PB_X1,680, OTE_SCAD_PB_X1,725, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0
    };
    SIZE sz[OTE_SCAD_MON1_N_CTRL] = {
        100,20, 100,20, 40,20, 40,20, 10,20, 10,20, 100,50, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        OTE_SCAD_PB_W,OTE_SCAD_PB_H2, OTE_SCAD_PB_W,OTE_SCAD_PB_H2 , OTE_SCAD_PB_W,OTE_SCAD_PB_H2, OTE_SCAD_PB_W,OTE_SCAD_PB_H1 , OTE_SCAD_PB_W,OTE_SCAD_PB_H1 , OTE_SCAD_PB_W,OTE_SCAD_PB_H1 , OTE_SCAD_PB_W,OTE_SCAD_PB_H1 , OTE_SCAD_PB_W,OTE_SCAD_PB_H1 ,
        OTE_SCAD_PB_W,OTE_SCAD_PB_H1 , OTE_SCAD_PB_W,OTE_SCAD_PB_H1 , OTE_SCAD_PB_W,OTE_SCAD_PB_H1 , 0,0, 0,0, 0,0, 0,0, 0,0,
        OTE_SCAD_PB_W,OTE_SCAD_PB_H1 , OTE_SCAD_PB_W,OTE_SCAD_PB_H1 , OTE_SCAD_PB_W,OTE_SCAD_PB_H1 , OTE_SCAD_PB_W,OTE_SCAD_PB_H1 , OTE_SCAD_PB_W,OTE_SCAD_PB_H1 , OTE_SCAD_PB_W,OTE_SCAD_PB_H1 , 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0
    };
    WCHAR text[OTE_SCAD_MON1_N_CTRL][OTE_SCAD_MON1_N_WCHAR] = {
        L"SHIMH000", L"タブレット", L"PC  R", L"PLC R", L"S", L"S", L"未設定\nXXXXXXX", L"",
        L"", L"", L"", L"", L"", L"", L"", L"",
        L"緊急停止", L"主幹", L"遠隔", L"MODE1", L"MODE2", L"認証", L"端末設定", L"操作器",
        L"PAD", L"アシスト", L"接続", L"", L"", L"", L"", L"",
        L"故障", L"設定", L"通信", L"カメラ", L"状態", L"クリア", L"", L"",
        L"", L"", L"", L"", L"", L"", L"", L"",
        L"", L"", L"", L"", L"", L"", L"", L"",
        L"", L"", L"", L"", L"", L"", L"", L""
    };
}ST_OTE_SCAD_MON1, * LPST_OTE_SCAD_MON1;

#define OTE_SCAD_MON2_WND_X     OTE_SCAD_MON1_WND_X
#define OTE_SCAD_MON2_WND_Y     620   
#define OTE_SCAD_MON2_WND_W     640
#define OTE_SCAD_MON2_WND_H     345

#define OTE_SCAD_MON2_N_CTRL    32
#define OTE_SCAD_MON2_N_WCHAR   64

#define OTE_SCAD_ID_MON2_CTRL_BASE   63140
#define OTE_SCAD_ID_MON2_STATIC_UNI      0
#define OTE_SCAD_ID_MON2_STATIC_MUL_PC   1
#define OTE_SCAD_ID_MON2_STATIC_MUL_OTE  2
#define OTE_SCAD_ID_MON2_LABEL_UNI       3
#define OTE_SCAD_ID_MON2_LABEL_MUL_PC    4
#define OTE_SCAD_ID_MON2_LABEL_MUL_OTE   5
#define OTE_SCAD_ID_MON2_STATIC_MSG      6
#define OTE_SCAD_ID_MON2_RADIO_RCV       7
#define OTE_SCAD_ID_MON2_RADIO_SND       8
#define OTE_SCAD_ID_MON2_RADIO_INFO      9

typedef struct _ST_OTE_SCAD_MON2 {
    HWND hwnd_mon;
    int timer_ms = OTE_SCAD_PRM_MON2_TIMER_MS;
    int sock_inf_id = OTE_SCAD_ID_MON2_RADIO_RCV; //ソケット情報を表示する内容コード

    bool is_monitor_active = false;

    wostringstream wo_uni, wo_mpc, wo_mote, wo_work;

    HWND hctrl[OTE_SCAD_MON2_N_CTRL] = {
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
    };
    POINT pt[OTE_SCAD_MON2_N_CTRL] = {
        5,50, 5,135, 5,220, 5,30, 5,115, 5,200, 5,5, 470,5,
        520,5, 570,5,  0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0
    };
    SIZE sz[OTE_SCAD_MON2_N_CTRL] = {
        OTE_SCAD_MON2_WND_W - 25,60, OTE_SCAD_MON2_WND_W - 25,60, OTE_SCAD_MON2_WND_W - 25,60, OTE_SCAD_MON2_WND_W - 25,20, OTE_SCAD_MON2_WND_W - 25,20, OTE_SCAD_MON2_WND_W - 25,20, OTE_SCAD_MON2_WND_W - 180,20, 40,20,
        40,20, 40,20,  0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0
    };
    WCHAR text[OTE_SCAD_MON2_N_CTRL][OTE_SCAD_MON2_N_WCHAR] = {
        L"-", L"-", L"-", L"", L"", L"",L"", L"",
        L"",L"",  L"", L"", L"", L"", L"", L"",
        L"", L"", L"", L"", L"", L"", L"", L"",
        L"", L"", L"", L"", L"", L"", L"", L""
    };

}ST_OTE_SCAD_MON2, * LPST_OTE_SCAD_MON2;

class COteScad : public CBasicControl
{
public:
    COteScad();
    ~COteScad();
    virtual HRESULT initialize(LPVOID lpParam) override;

    virtual LRESULT CALLBACK PanelProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp) override;

    static LRESULT CALLBACK Mon1Proc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);
    static LRESULT CALLBACK Mon2Proc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);

    static ST_OTE_SCAD_MON1 st_mon1;
    static ST_OTE_SCAD_MON2 st_mon2;

    static ST_OTE_UI st_work;

    void set_panel_io();

	HRESULT open_ope_window();

    //タスク出力用構造体
  
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
};


