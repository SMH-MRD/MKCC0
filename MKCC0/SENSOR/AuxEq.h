#pragma once

#include "resource.h"

namespace AUXEQ
{

#define N_AUXEQ_TASK                  6

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

#define AUXEQ_WND_X                0+640+640
#define AUXEQ_WND_Y                0
#define AUXEQ_WND_W                640
#define AUXEQ_WND_H                460
#define AUXEQ_WND_MIN_H            100


#define TILT_SCAN_MS			100
#define CS_SCAN_MS				100


//マルチメディアタイマー管理構造体
#define TARGET_RESOLUTION			1		//マルチメディアタイマー分解能 msec
#define SYSTEM_TICK_ms				20		//メインスレッド周期 msec
#define INITIAL_TASK_STACK_SIZE		16384	//タスクオブジェクトスレッド用スタックサイズ

#define NAME_OF_INIFILE					L"ote"			//iniファイルファイル名
#define EXT_OF_INIFILE					L"ini"			//iniファイル拡張子
#define PATH_OF_INIFILE					pszInifile		//iniファイルパス

#define OPATH_SECT_OF_INIFILE			L"PATH"			//オプショナルパスセクション
#define OBJ_NAME_SECT_OF_INIFILE		L"OBJECT_NAME"	//タスクオブジェクトフルネームセクション
#define OBJ_SNAME_SECT_OF_INIFILE		L"OBJECT_SNAME"	//タスクオブジェクト略称セクション
#define NETWORK_SECT_OF_INIFILE			L"NW_PARAM"		//Communicatorタスクセクション
#define SMEM_SECT_OF_INIFILE			L"SMEM"			//共有メモリセクション
#define SYSTEM_SECT_OF_INIFILE			L"SYSTEM"		//システム設定セクション

//各セクションの各タスク用の情報を参照するためのキー
#define MAIN_KEY_OF_INIFILE				L"MAIN"
#define TILT_KEY_OF_INIFILE	            L"TILT"
#define CS_KEY_OF_INIFILE	            L"CLIENT"




    //-ID定義 Mainスレッド用　2100 +α
#define ID_TASK_SET_TAB				2098
#define ID_STATUS_BAR				2099
#define IDC_OBJECT_BASE				2100


#define PRM_N_TASK_MSGLIST_ROW      100 

    typedef struct stSensorWnd {
        HWND hWnd_status_bar;
        HWND hWnd;
    } ST_AUXEQ_WND, * LPST__AUXEQ_WND;

}

