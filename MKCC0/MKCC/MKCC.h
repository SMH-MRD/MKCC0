#pragma once

#include "resource.h"

namespace MKCC
{
    // 割り当てスレッドコード
    enum class ENUM_THREAD : unsigned int {
        ENV = 0,            // Environment
        CS,                 // Client Service
        SCAD,               // Scada
        POL,                // Policy
        AGENT,              // Agent
        SIM,                // Simulation
        NUM_OF_THREAD       // スレッド数
    };

    //-各タスクアイコン用イメージリスト設定値
#define ICON_IMG_W					32		//アイコンイメージ幅
#define ICON_IMG_H					32		//アイコンイメージ高さ

//-Tweet Message表示用Static Windowのサイズ設定値
#define MSG_WND_H					20		//Height of tweet window
#define MSG_WND_W					590		//Width of  tweet window
#define MSG_WND_ORG_X				35		//Origin X of tweet window position 
#define MSG_WND_ORG_Y				10		//Origin Y of tweet window position 
#define MSG_WND_Y_SPACE				12		//Space between tweet windows Y direction 
#define TWEET_IMG_ORG_X				5		//Origin X of bitmap tweet icon position

//-タスク設定タブ配置設定
#define TAB_DIALOG_W				610
#define TAB_DIALOG_H				320
#define TAB_POS_X					5
#define TAB_POS_Y					10
#define TAB_SIZE_H					35
#define TAB_SPACE				    20		 

#define MAIN_WND_X                  0
#define MAIN_WND_Y                  0
#define MAIN_WND_W                  640
#define MAIN_WND_H                  460
#define MAIN_WND_MIN_H              100


#define ENV_SCAN_MS			    20
#define CS_SCAN_MS				100
#define AGENT_SCAN_MS			20
#define POLICY_SCAN_MS			100
#define SCADA_SCAN_MS			100
#define SIM_SCAN_MS			    20


//マルチメディアタイマー管理構造体
#define TARGET_RESOLUTION			1		//マルチメディアタイマー分解能 msec
#define SYSTEM_TICK_ms				20		//メインスレッド周期 msec
#define INITIAL_TASK_STACK_SIZE		16384	//タスクオブジェクトスレッド用スタックサイズ

#define NAME_OF_INIFILE					L"mkcc"			//iniファイルファイル名
#define EXT_OF_INIFILE					L"ini"			//iniファイル拡張子
#define PATH_OF_INIFILE					pszInifile		//iniファイルパス

#define OPATH_SECT_OF_INIFILE			L"PATH"			//オプショナルパスセクション
#define OBJ_NAME_SECT_OF_INIFILE		L"OBJECT_NAME"	//タスクオブジェクトフルネームセクション
#define OBJ_SNAME_SECT_OF_INIFILE		L"OBJECT_SNAME"	//タスクオブジェクト略称セクション
#define NETWORK_SECT_OF_INIFILE			L"NW_PARAM"		//Communicatorタスクセクション
#define SMEM_SECT_OF_INIFILE			L"SMEM"			//共有メモリセクション
#define SYSTEM_SECT_OF_INIFILE			L"SYSTEM"		//システム設定セクション
#define SYSTEM_SECT_OF_SIMULATION		L"SIM"		    //シミュレーションセクション

//各セクションの各タスク用の情報を参照するためのキー
#define MAIN_KEY_OF_INIFILE				L"MAIN"
#define ENV_KEY_OF_INIFILE	            L"ENV"
#define CS_KEY_OF_INIFILE	            L"CLIENT"
#define POL_KEY_OF_INIFILE	            L"POL"
#define AGENT_KEY_OF_INIFILE	        L"AGENT"
#define SCAD_KEY_OF_INIFILE	            L"SCAD"
#define SIM_KEY_OF_INIFILE	            L"SIM"

    typedef struct stKnlManageSetTag {
        WORD mmt_resolution = TARGET_RESOLUTION;			//マルチメディアタイマーの分解能
        unsigned int cycle_base = SYSTEM_TICK_ms;			//マルチメディアタイマーの分解能
        WORD KnlTick_TimerID = 0;							//マルチメディアタイマーのID
        unsigned int num_of_task = 0;						//アプリケーションで利用するスレッド数
        unsigned long sys_counter = 0;						//マルチメディア起動タイマカウンタ
        SYSTEMTIME Knl_Time = { 0,0,0,0,0,0,0,0 };			//アプリケーション開始からの経過時間
        unsigned int stackSize = INITIAL_TASK_STACK_SIZE;	//タスクの初期スタックサイズ
    }ST_KNL_MANAGE_SET, * LPST_KNL_MANAGE_SET;


    //-ID定義 Mainスレッド用　5100 +α
#define ID_TASK_SET_TAB				5098
#define ID_STATUS_BAR				5099
#define IDC_OBJECT_BASE				5100


#define PRM_N_TASK_MSGLIST_ROW      100 //タスクメッセージリスト行数

    typedef struct _ST_MAIN_WND {
        HWND hWnd_status_bar;
        HWND hWnd;
    } ST_MAIN_WND, * LPST_MAIN_WND;
}

