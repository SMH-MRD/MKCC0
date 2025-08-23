#pragma once

#include <windows.h>
#include <iostream>
#include <iomanip>
#include <sstream>

using namespace std;

#define BC_CODE_COM_TERMINATE    0x80000000
#define BC_CODE_COM_REPEAT_SCAN  0x00000001

#define BC_CODE_STAT_IDLE        0x00000000
#define BC_CODE_STAT_INIT_REQ    0x00000001
#define BC_CODE_STAT_INIT_WAIT   0x00000002
#define BC_CODE_STAT_RUN_REQ     0x00000004
#define BC_CODE_STAT_RUNNING     0x00000008
#define BC_CODE_STAT_ERROR       0x00000010
#define BC_CODE_STAT_INIT_ERROR  0x00000020

#define BC_EVENT_TYPE_TIME       0x00000000
#define BC_EVENT_TYPE_SEQENCE    0x00000001

#define BC_CODE_TYPE_EVENT       0x00000000
#define BC_CODE_TYPE_PERIODIC    0x00000001

#define BC_EVENT_MAX                        8
#define BC_DEFAUT_THREAD_CYCLE	            25		//定周期起動デフォルト値 msec
#define BC_LISTVIEW_ROW_MAX                 100     // リストビュー行数
#define BC_LISTVIEW_COLUMN_MAX              2       // リストビュー列数
#define BC_LISTVIEW_COLUMN_WIDTH_TIME       90      // リストビュー列幅(Time)
#define BC_LISTVIEW_COLUMN_WIDTH_MESSAGE    450     // リストビュー列幅(Message)

#define BC_N_FUNC_ITEM                      6     // リストビュー列幅(Message)
#define BC_N_ACT_ITEM                       6     // リストビュー列幅(Message)

#define BC_ID_MODE0                         0
#define BC_ID_MODE1                         1
#define BC_ID_MODE2                         2

#define BC_ID_MON1                          0
#define BC_ID_MON2                          1

#define BC_TASK_NAME_BUF_SIZE               32

#define BC_BK_COLOR_DEFAULT                 0
#define BC_BK_COLOR_LIGHT_GREEN             1
#define BC_BK_COLOR_LIGHT_YELLOW            2
#define BC_BK_COLOR_LIGHT_BLUE              3
#define BC_BK_COLOR_LIGHT_RED               4

#define BC_EVENT_REC_SET_BBK_COLOR          0x00000001

// タスクスレッドID
struct BC_TASK_ID
{
    int ENV;
    int CS;
    int SCAD;
    int POL;
    int AGENT;
    int SIM;
    int NUM_OF_THREAD;
};

//マルチメディアタイマー管理構造体
#define TARGET_RESOLUTION			1		//マルチメディアタイマー分解能 msec
#define SYSTEM_TICK_ms				20		//メインスレッド周期 msec
#define INITIAL_TASK_STACK_SIZE		16384	//タスクオブジェクトスレッド用スタックサイズ

typedef struct stKnlManageSetTag {
    WORD mmt_resolution = TARGET_RESOLUTION;			//マルチメディアタイマーの分解能
    unsigned int cycle_base = SYSTEM_TICK_ms;			//マルチメディアタイマーの分解能
    WORD KnlTick_TimerID = 0;							//マルチメディアタイマーのID
    unsigned int num_of_task = 0;						//アプリケーションで利用するスレッド数
    unsigned long sys_counter = 0;						//マルチメディア起動タイマカウンタ
    SYSTEMTIME Knl_Time = { 0,0,0,0,0,0,0,0 };			//アプリケーション開始からの経過時間
    unsigned int stackSize = INITIAL_TASK_STACK_SIZE;	//タスクの初期スタックサイズ
}ST_KNL_MANAGE_SET, * LPST_KNL_MANAGE_SET;

typedef struct _tagTHREAD_INFO {
    // オブジェクト識別情報
    std::wstring name;  // オブジェクト名
    std::wstring sname; // オブジェクト略称
    HBITMAP      hbmp;  // オブジェクト識別用ビットマップ
    HINSTANCE	hinstance; // インスタンスハンドル

    // スレッド設定内容
    int32_t  index;                     // スレッドIndex
    uint32_t id;                        // スレッドID
    HANDLE   hndl;                      // スレッドハンドル
    HANDLE   hevents[BC_EVENT_MAX];     // イベントハンドル
    int32_t  n_active_events;           // 有効なイベント数

    uint32_t cycle_ms;                  // スレッド実行設定周期
    uint32_t cycle_count;               // スレッド実行設定周期 Tick count(ms/system tick)
    uint32_t retry_count;               // スレッド実行設定周期 Tick count(ms/system tick)
    int32_t  trigger_type;              // スレッド起動条件 定周期 or イベント
    int32_t  priority;                  // スレッドのプライオリティ
    int32_t  status;                    // スレッド制御状態フラグ 0:繰り返し 1:中断 それ以外:逐次処理:
    int32_t  command;                    // スレッド制御状態フラグ 0:繰り返し 1:中断 それ以外:逐次処理:

    // スレッドモニタ情報
    LARGE_INTEGER sys_count;
    LARGE_INTEGER sys_freq;
    
    LONGLONG    start_time;        // 現スキャンのスレッド開始時間
    LONGLONG    act_time;          // 1スキャンのスレッド実処理時間
    LONGLONG    period;            // スレッドループ周期実績
    LONGLONG    act_count;         // スレッドループカウンタ
    LONGLONG    total_act;         // 起動積算カウンタ
    LONGLONG	event_triggered;   // スレッドトリガイベント
    uint32_t    time_over_count;   // 予定周期をオーバーした回数

	LONGLONG    cnt_unit_ms;       // パフォーマンスカウンタの単位時間(1msあたりのカウント数)
	LONGLONG    cnt_unit_us;      // パフォーマンスカウンタの単位時間(1usあたりのカウント数)
	double      dt;                // 実績スキャン（μ秒単位のカウント数）

    // 関連ウィンドウハンドル
    HWND hwnd_parent;       // 親ウィンドウのハンドル
    HWND hwnd_msgstatics;   // 親ウィンドウメッセージ表示用ウィンドウへのハンドル
    HWND hwnd_opepane;      // 自メインウィンドウのハンドル(メインフレーム上に配置,タブ選択で表示）
    HWND hwnd_msglist;      // 自メインウィンドウのメッセージ表示用リストコントロールへのハンドル
    HINSTANCE hInstance;    // アプリケーションのインスタンス

    // 操作パネル関連
    int32_t panel_msglist_count;                                // パネルメッセージリストのカウント数
    int32_t panel_func_id;                                      // パネルfunctionボタンの選択内容
    bool panel_act_chk[BC_N_FUNC_ITEM][BC_N_ACT_ITEM];          // パネルtypeボタンの選択内容
    int32_t mode_id;                                            // 
    
    // 外部インターフェース
    unsigned long* psys_counter;    // メインシステムカウンターの参照先ポインタ

    _tagTHREAD_INFO()
        : name(L"no name")
        , sname(L"TBD")
        , hbmp(NULL)
        , hInstance(NULL)
        , index(0)
        , id(0)
        , hndl(NULL)
        , hevents{}
        , n_active_events(1)
        , cycle_ms(BC_DEFAUT_THREAD_CYCLE)
        , cycle_count(4)
        , retry_count(100)
        , trigger_type(0)
        , priority(THREAD_PRIORITY_NORMAL)
        , status(BC_CODE_STAT_IDLE)
        , command(BC_CODE_COM_REPEAT_SCAN)
        , sys_count{0}
        , sys_freq{1000}
        , start_time(0)
        , act_time(0)
        , period(100)
        , act_count(0)
        , total_act(0)
        , event_triggered(0)
        , time_over_count(1000)

        ,cnt_unit_ms(0) 
        ,cnt_unit_us(0) 
        ,dt(1.0)   

        , hwnd_parent(NULL)
        , hwnd_msgstatics(NULL)
        , hwnd_opepane(NULL)
        , hwnd_msglist(NULL)
        , hinstance(NULL)
        , panel_msglist_count(0)
        , panel_func_id(0)
        , panel_act_chk{
            false, false, false, false, false, false,
            false, false, false, false, false, false,
            false, false, false, false, false, false,
            false, false, false, false, false, false,
            false, false, false, false, false, false,
            false, false, false, false, false, false
        }
        , mode_id(0)
        , psys_counter(NULL)
    {
    }
} THREAD_INFO, * PTHREAD_INFO;

class CBasicControl //基本制御クラス
{
public:
    THREAD_INFO inf;
    wostringstream wos;
    ULONGLONG err;

    LPVOID poutput = NULL;     //結果出力メモリ
    DWORD  out_size = 0;        //出力バッファのサイズ

    CBasicControl() { inf.status = BC_CODE_STAT_IDLE; }
    ~CBasicControl() { delete inf.hbmp; }
 
    unsigned WINAPI run(void* param) ;        //スレッド実行

    virtual HRESULT __stdcall initialize(void* param) ; 

    //自タスク設定タブパネルウィンドウ処理関数
    virtual LRESULT CALLBACK PanelProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);

    //タブパネルのStaticテキストを設定
    virtual void set_panel_tip_txt();
    //タブパネルのFunctionボタンのStaticテキストを設定
    virtual void set_func_pb_txt();
    //タブパネルのItem chkテキストを設定
    virtual void set_item_chk_txt();
    //メインパネルのStaticWindowにコメント出力
    virtual void msg2host(wstring wstr);
    virtual void set_color_of_hostmsg(int txtr, int txtg, int txtb, int bkr, int bkg, int bkb);

    //タブパネルのListViewにコメント出力
    virtual void msg2listview(wstring wstr);

    //パラメータ初期表示値設定
    virtual void set_PNLparam_value(float p1, float p2, float p3, float p4, float p5, float p6) ;

    //タブパネルのFunctionボタンのリセット
    virtual void reset_panel_func_pb(HWND hDlg) { return; };
 
    virtual int req_command(WORD com_code, WORD param, void* pparam) { return 0; };

   virtual int set_outbuf(LPVOID) { return 0; };         //出力バッファセット
   virtual int close() { return 0; }

private:
 
    virtual int input(void* param) { return 0; }                    //入力処理
    virtual int parse(void* param) { return 0; }                    //メイン処理
    virtual int output(void* param) { return 0; }                   //出力処理

	virtual HRESULT routine_work(void* param) { 
        if (inf.total_act % 20 == 0) {
            wos.str(L""); wos << inf.status << L":" << std::setfill(L'0') << std::setw(4) << inf.act_time;
            msg2host(wos.str());
        }
		input(param);
        parse(param);
        output(param);
        return S_OK; 
    };
};

