#include "CBasicControl.h"
#include "CSHAREDMEM.H"

#define CS_ID_MON1_TIMER  71190
#define CS_ID_MON2_TIMER  71191

#define CS_PRM_MON1_TIMER_MS  200
#define CS_PRM_MON2_TIMER_MS  50

#define CS_MON1_WND_X     1280
#define CS_MON1_WND_Y     0
#define CS_MON1_WND_W     320
#define CS_MON1_WND_H     240
#define CS_MON1_N_CTRL    32
#define CS_MON1_N_WCHAR   64

#define CS_MON1_N_CTRL    32
#define CS_MON1_N_WCHAR   64

#define CS_ID_MON1_CTRL_BASE   71100
#define CS_ID_MON1_STATIC_1     0

typedef struct _ST_CS_MON1 {
    int timer_ms = CS_PRM_MON1_TIMER_MS;
    HWND hwnd_mon;
    bool is_monitor_active = false; //ソケット情報を表示する内容コード

    HWND hctrl[CS_MON1_N_CTRL] = {
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
    };
    POINT pt[CS_MON1_N_CTRL] = {
        5,5, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0
    };
    SIZE sz[CS_MON1_N_CTRL] = {
        295,190, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0
    };
    WCHAR text[CS_MON1_N_CTRL][CS_MON1_N_WCHAR] = {
        L"MON1", L"", L"", L"", L"", L"", L"", L"",
        L"", L"", L"", L"", L"", L"", L"", L"",
        L"", L"", L"", L"", L"", L"", L"", L"",
        L"", L"", L"", L"", L"", L"", L"", L""
    };
}ST_CS_MON1, * LPST_CS_MON1;

#define CS_MON2_WND_X     CS_MON1_WND_X
#define CS_MON2_WND_Y     620   
#define CS_MON2_WND_W     640
#define CS_MON2_WND_H     265

#define CS_MON2_N_CTRL    32
#define CS_MON2_N_WCHAR   64

#define CS_ID_MON2_CTRL_BASE   71140

#define CS_ID_MON2_STATIC_UNI      0
#define CS_ID_MON2_LABEL_UNI       1
#define CS_ID_MON2_STATIC_MSG      2

#define CS_ID_MON2_RADIO_RCV       8
#define CS_ID_MON2_RADIO_SND       9
#define CS_ID_MON2_RADIO_INFO      10

typedef struct _ST_CS_MON2 {
    HWND hwnd_mon;
    int timer_ms    = CS_PRM_MON2_TIMER_MS;
    int sock_inf_id = CS_ID_MON2_RADIO_RCV;//ソケット情報を表示する内容コード

    bool is_monitor_active = false; //モニタ画面表示中フラグ

    wostringstream wo_uni, wo_work;

    HWND hctrl[CS_MON2_N_CTRL] = {
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
    };
    POINT pt[CS_MON2_N_CTRL] = {
        5,95, 5,50, 5,5, 0,0, 0,0, 0,0, 0,0, 0,0,
        470,5,520,5,570,5, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0
    };
    SIZE sz[CS_MON2_N_CTRL] = {
        CS_MON2_WND_W - 25,120, CS_MON2_WND_W - 25,40, CS_MON2_WND_W - 25,40, 0,0, 0,0, 0,0, 0,0,
        CS_MON2_WND_W - 180,20,40,20,40,20, 40,20,  0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0
    };
    WCHAR text[CS_MON2_N_CTRL][CS_MON2_N_WCHAR] = {
        L"UNI:", L"-", L"MSG:", L"-", L"-", L"-", L"-", L"",
        L"RCV", L"SND",L"INFO",  L"", L"", L"", L"", L"",
        L"", L"", L"", L"", L"", L"", L"", L"",
        L"", L"", L"", L"", L"", L"", L"", L""
    };

}ST_CS_MON2, * LPST_CS_MON2;

class CAuxCS : public CBasicControl
{
public:
    CAuxCS();
    ~CAuxCS();

    virtual HRESULT initialize(LPVOID lpParam) override;

    LRESULT CALLBACK PanelProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);

    static LRESULT CALLBACK Mon1Proc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);
    static LRESULT CALLBACK Mon2Proc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);

    static ST_CS_MON1 st_mon1;
    static ST_CS_MON2 st_mon2;

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

