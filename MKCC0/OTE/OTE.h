#pragma once

#include "resource.h"

namespace OTE
{ 
#define N_OTE_TASK                  6

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

#define OTE_WND_X                0+640
#define OTE_WND_Y                0
#define OTE_WND_W                640
#define OTE_WND_H                460
#define OTE_WND_MIN_H            100


#define ENV_SCAN_MS			    100
#define CS_SCAN_MS				100
#define AGENT_SCAN_MS			100
#define POLICY_SCAN_MS			100
#define SCADA_SCAN_MS			100

#define NAME_OF_INIFILE					L"ote"			//iniファイルファイル名
#define EXT_OF_INIFILE					L"ini"			//iniファイル拡張子
#define PATH_OF_INIFILE					pszInifile		//iniファイルパス

#define OPATH_SECT_OF_INIFILE			L"PATH"			//オプショナルパスセクション
#define OBJ_NAME_SECT_OF_INIFILE		L"OBJECT_NAME"	//タスクオブジェクトフルネームセクション
#define OBJ_SNAME_SECT_OF_INIFILE		L"OBJECT_SNAME"	//タスクオブジェクト略称セクション
#define NETWORK_SECT_OF_INIFILE			L"NW_PARAM"		//Communicatorタスクセクション
#define SMEM_SECT_OF_INIFILE			L"SMEM"			//共有メモリセクション
#define SYSTEM_SECT_OF_INIFILE			L"SYSTEM"		//システム設定セクション

 //システム関連キー
#define ODER_CODE_KEY_OF_INIFILE        L"ORDER_CODE"
#define PC_TYPE_KEY_OF_INIFILE          L"PC_TYPE"      
#define PC_SERIAL_KEY_OF_INIFILE        L"PC_SERIAL"
#define PC_OPTION_KEY_OF_INIFILE        L"PC_OPTION"

//各セクションの各タスク用の情報を参照するためのキー
#define MAIN_KEY_OF_INIFILE				L"MAIN"
#define ENV_KEY_OF_INIFILE	            L"ENV"
#define CS_KEY_OF_INIFILE	            L"CLIENT"
#define POL_KEY_OF_INIFILE	            L"POL"
#define AGENT_KEY_OF_INIFILE	        L"AGENT"
#define SCAD_KEY_OF_INIFILE	            L"SCAD"

//-ID定義 Mainスレッド用　3100 +α
#define ID_TASK_SET_TAB				3098
#define ID_STATUS_BAR				3099
#define IDC_OBJECT_BASE				3100

#define PRM_N_TASK_MSGLIST_ROW      100 

typedef struct stOteWnd {
    HWND hWnd_status_bar;
    HWND hWnd;
} ST_OTE_WND, * LPST_OTE_WND;

}

