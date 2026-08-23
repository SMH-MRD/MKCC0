#pragma once

//@ GDI+
#include <objbase.h>//gdiplus.hのコンパイルを通すために必要
#include <gdiplus.h>

#include "..\Common\COMMON_DEF.h"
#include "mklog_itm_db.h"

using namespace Gdiplus;

//チャートクリア用メッセージID
#define MKLOG_MSGID_REFRESH_TMCHART_ITEM		500
#define MKLOG_MSGID_REFRESH_SCCHART_ITEM		501

//LOG TYPE ID
constexpr auto MKLOG_N_ID_TYPE		= 4;	//LOGのタイプ数（EVENT,TIME CHART,SCATTER,TRAP)
constexpr auto MKLOG_ID_TYPE_ALL	= 4;	//LOGのタイプ数（EVENT,TIME CHART,SCATTER,TRAP)
constexpr auto MKLOG_ID_TYPE_TIME	= 0;	//連続時間ログ,グラフ用
constexpr auto MKLOG_ID_TYPE_TRAP	= 1;	//トラップログ
constexpr auto MKLOG_ID_TYPE_EVENT	= 2;	//イベントログ【Main Window】
constexpr auto MKLOG_ID_TYPE_SCAT	= 3;	//SCATTARグラフ

//各LOG WINDOW用コント―ロール用ベースID
constexpr auto MKLOG_ID_WND_CTRL_EVENT	= 11000;
constexpr auto MKLOG_ID_WND_CTRL_TIME	= 12000;
constexpr auto MKLOG_ID_WND_CTRL_SCAT	= 13000;
constexpr auto MKLOG_ID_WND_CTRL_TRAP	= 14000;
constexpr auto MKLOG_ID_WND_CTRL_FAULT	= 15000;


//LOG操作コマンドコード
constexpr auto MKLOG_CODE_LOG_REC_STOP		= 1;
constexpr auto MKLOG_CODE_LOG_REC_START		= 2;
constexpr auto MKLOG_CODE_LOG_FILEOUT		= 3;
constexpr auto MKLOG_CODE_LOG_PAUSE			= 4;

//LOG STATUSコード
constexpr auto MKLOG_CODE_LOG_DEACTIVE			= 0x0000;//LOG停止中
constexpr auto MKLOG_CODE_LOG_RECORDING			= 0x0001;//LOG記録中
constexpr auto MKLOG_CODE_LOG_REC_PAUSED		= 0x0002;//LOG記録保留中
constexpr auto MKLOG_CODE_LOG_CHART_ACTIVE		= 0x0010;//LOG CHART表示実行中
constexpr auto MKLOG_CODE_LOG_REC_AND_CHART		= 0x0011;//LOG記録中＋CHART表示実行中
constexpr auto MKLOG_CODE_LOG_FILE_OUT_ACTIVE	= 0x0020;//LOG記録書き出し中

//LOG表示操作コマンドコード
constexpr auto MKLOG_CODE_DISP_NA			= 0x00;
constexpr auto MKLOG_CODE_DISP_LOG			= 0x10;
constexpr auto MKLOG_CODE_DISP_REAL			= 0x80;
constexpr auto MKLOG_CODE_DISP_REAL_PAUSE	= 0x81;

//WINDOW エリア
constexpr auto MKLOG_WND_EVENT_X		= 800;
constexpr auto MKLOG_WND_EVENT_Y		= 0;
constexpr auto MKLOG_WND_EVENT_W		= 640;
constexpr auto MKLOG_WND_EVENT_H		= 480;

constexpr auto MKLOG_WND_TIME_X			= 800;
constexpr auto MKLOG_WND_TIME_Y			= 1;
constexpr auto MKLOG_WND_TIME_W			= 780;
constexpr auto MKLOG_WND_TIME_H			= 760;

constexpr auto MKLOG_WND_SCAT_X			= 800;
constexpr auto MKLOG_WND_SCAT_Y			= 0;
constexpr auto MKLOG_WND_SCAT_W			= 680;
constexpr auto MKLOG_WND_SCAT_H			= 960;

constexpr auto MKLOG_WND_TRAP_X			= 800;
constexpr auto MKLOG_WND_TRAP_Y			= 0;
constexpr auto MKLOG_WND_TRAP_W			= 640;
constexpr auto MKLOG_WND_TRAP_H			= 480;

constexpr auto NAME_OF_MKLOG_INI_EVENT			= L"\\ini\\log_event";		//iniファイル名
constexpr auto NAME_OF_MKLOG_INI_TIME			= L"\\ini\\log_time";		//iniファイル名
constexpr auto NAME_OF_MKLOG_INI_TRAP			= L"\\ini\\log_trap";		//iniファイル名
constexpr auto NAME_OF_MKLOG_INI_SCAT			= L"\\ini\\log_scat";		//iniファイル名

constexpr auto FILECAPS_OF_MKLOG_EVENT			= L"log_ev";				//iniファイル名
constexpr auto FILECAPS_OF_MKLOG_TIME			= L"log_tm";				//iniファイル名
constexpr auto FILECAPS_OF_MKLOG_TRAP			= L"log_tr";				//iniファイル名
constexpr auto FILECAPS_OF_MKLOG_SCAT			= L"log_sc";				//iniファイル名

constexpr auto LOGFOLDER_OF_MKLOG				= L"C://LOG/";		//Logフォルダ
constexpr auto LOGFOLDER_OF_MKLOG_TM			= L"C://LOG/TM/";		//Logフォルダ
constexpr auto LOGFOLDER_OF_MKLOG_SC			= L"C://LOG/SC/";		//Logフォルダ
constexpr auto LOGFOLDER_OF_MKLOG_EV			= L"C://LOG/EV/";		//Logフォルダ
constexpr auto LOGFOLDER_OF_MKLOG_TR			= L"C://LOG/TR/";		//Logフォルダ


constexpr auto EXT_OF_MKLOG_INIFILE				= L"ini";					//iniファイル拡張子
constexpr auto SETTING_SECT_OF_INIFILE			= L"SETTING";				//CHART1 機能パラメータセクション
constexpr auto ITEM_SECT_OF_INIFILE				= L"ITEM";				//CHART2 機能パラメータセクション
constexpr auto SET_LOG_SCAN_KEY_OF_INIFILE		= L"LOG_SCAN_MS";
constexpr auto SET_LOG_ITEM001_KEY_OF_INIFILE	= L"ITEM";//

constexpr auto MKLOG_N_LOG_ITEM_MAX				= 64;
constexpr auto MKLOG_N_LOG_TITLE_WCH			= 16;
constexpr auto MKLOG_N_LOG_RECORD				= 2048;

constexpr auto MKLOG_PARAM_DATA_100PER_D		= 10000.0;
constexpr auto MKLOG_PARAM_DATA_100PER_I		= 10000;

constexpr auto MKLOG_INDEX_PC_CODE	= 0;//code配列
constexpr auto MKLOG_INDEX_N_ITEM	= 1;//code配列
constexpr auto MKLOG_INDEX_LOG_TYPE = 0;//d100配列
constexpr auto MKLOG_INDEX_SCAN_MS	= 1;//d100配列
constexpr auto MKLOG_INDEX_LOG_DATA0= 2;

typedef struct StLogHeader {
	INT32	code[MKLOG_INDEX_LOG_DATA0 + MKLOG_N_LOG_ITEM_MAX];							//LOG ITEMコード　配列の＋２はPC ID+項目数
	double	d100[MKLOG_INDEX_LOG_DATA0 + MKLOG_N_LOG_ITEM_MAX];							//LOG ITEM100%値　配列の＋２はLOG TYPEコード+スキャンタイムmsec
	WCHAR	title[MKLOG_INDEX_LOG_DATA0 + MKLOG_N_LOG_ITEM_MAX][MKLOG_N_LOG_TITLE_WCH];	//
}ST_LOG_HEADER, * LPST_LOG_HEADER;

typedef struct StLogRecord {
	time_t time;
	INT32 id;							//イベントID or カウンタ
	INT16 data[MKLOG_N_LOG_ITEM_MAX];	//%DIO or 正規化データ（100%→10000）
}ST_LOG_RECORD, * LPST_LOG_RECORD;

//LIB側で保持、管理
typedef struct StLogFileBuf {
	INT32 log_status;
	INT32 iw;				//Write Pointer
	INT32 ir;				//Read Pointer
	INT32 n_item;			//ログ項目数
	INT32 sample_count;		//サンプリング周期カウント値
	ST_LOG_HEADER header;
	ST_LOG_RECORD records[MKLOG_N_LOG_RECORD];
}ST_LOG_FILE_BUF, * LPST_LOG_FILE_BUF;

typedef struct _stMKLOG_VAL
{
	double* pd[MKLOG_N_ID_TYPE][MKLOG_N_LOG_ITEM_MAX];  //doubleデータ生値
	INT16* pi[MKLOG_N_ID_TYPE][MKLOG_N_LOG_ITEM_MAX];   //DIOデータ生値
}ST_MKLOG_VAL, * LPST_MKLOG_VAL;


/// <summary>
/// ログ項目設定構造体（アプリ側で保持、管理）
/// </summary>
struct ST_MKLOG_SOURCE {

	INT16 n_item[MKLOG_N_ID_TYPE]		= { 16,0,0,16 };					//LOG項目数 イベントログは、イベント登録数
	double sample_ms[MKLOG_N_ID_TYPE]	= { 100.0,25.0,0.0,100.0 };			//サンプリング周期設定(msec) EVENTは0
#if 0	
	//LOGデータ値を返す関数のアドレスの配列	
	INT16 (* func[MKLOG_N_ID_TYPE][MKLOG_N_LOG_ITEM_MAX])(double d)  = {
		NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
		NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
		NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
		NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL
	};
#endif
	ST_LOG_HEADER header_time = {
		//code
		{
			1,16,
			MKLOG_ITEM_SL_SPD_REF,MKLOG_ITEM_SL_SPD_FB,MKLOG_ITEM_NOTCH_FB_SL,MKLOG_ITEM_MH_POS_FB,MKLOG_ITEM_BH_POS_FB,MKLOG_ITEM_MHL,MKLOG_ITEM_SL_BRK_PSW,MKLOG_ITEM_EMPTY,
			MKLOG_ITEM_SWY1_TH_X,MKLOG_ITEM_SWY1_TX,MKLOG_ITEM_SWY1_AMP_P2P_X,MKLOG_ITEM_SWY1_PH_TIME_X,MKLOG_ITEM_EMPTY,MKLOG_ITEM_EMPTY,MKLOG_ITEM_EMPTY,MKLOG_ITEM_EMPTY,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,248,249,0,0,0,0,0,0,250,251
		},
		//100%
		{
			0.0,100.0,			//予備, スキャンタイムmsec
			0.0,0.0,0.0,0.0,
			0.0,0.0,0.0,0.0,
			0.0,0.0,0.0,0.0,
			0.0,0.0,0.0,0.0,
			0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
			0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
			0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0
		},
		{
			//Title
				L"Time",L"COUNT",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",
				L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",
				L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",
				L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L""
			}
	};

	ST_LOG_HEADER header_trap = {
		//code
		{
			MKLOG_ITEM_TIME,MKLOG_ITEM_COUNT,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
		},
		//100%
		{
			1.0,100.0,//予備, スキャンタイムmsec
			0.0,0.0,0.00,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
			0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
			0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
			0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0
		},
		{
			//Title
				L"Time",L"COUNT",
				L"MH V FB",L"BH V FB",L"SL V FB",L"AH V FB",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",
				L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",
				L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",
				L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L""
		}
	};

	ST_LOG_HEADER header_event = {
		//code
		{
			1,16,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
		},
		//100%
		{
			0.0,100.0,//予備, スキャンタイムmsec
			0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
			0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
			0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
			0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0
		},
		{
			//Title
				L"Time",L"EVENT",
				L"MH V FB",L"BH V FB",L"SL V FB",L"AH V FB",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",
				L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",
				L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",
				L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L""
			}
	};

	ST_LOG_HEADER header_scat = {
		//code
		{
			1,16,
			16,25,20,22,0,0,0,0,17,26,21,23,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
		},
		//100%
		{
			3.0,100.0,			//予備, スキャンタイムmsec
			0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
			0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
			0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
			0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0
		},
		{
			//Title
				L"Time",L"COUNT",
				L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",
				L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",
				L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",
				L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L""
			}
	};

};

constexpr auto ID_MKLOG_NOBJ_FRAME	= 128;
constexpr auto ID_MKLOG_NOBJ_TXT	= 16;
constexpr auto ID_MKLOG_NCOLOR		= 16;

#define ID_MKLOG_COLOR_BLACK				0
#define ID_MKLOG_COLOR_WHITE				1
#define ID_MKLOG_COLOR_GLAY_BK				2	
#define ID_MKLOG_COLOR_RED					3	
#define ID_MKLOG_COLOR_ORANGE				4	
#define ID_MKLOG_COLOR_GREEN				5	
#define ID_MKLOG_COLOR_CYAN					6	
#define ID_MKLOG_COLOR_BLUE					7
#define ID_MKLOG_COLOR_PURPLE				8	
#define ID_MKLOG_COLOR_BROWN				9	
#define ID_MKLOG_COLOR_MAZENDA				10	
#define ID_MKLOG_COLOR_YELLOW				11

#define ID_MKLOG_PEN_DASH_GRAY				12


#define ID_MKLOG_FONT6						0
#define ID_MKLOG_FONT8						1
#define ID_MKLOG_FONT10						2	
#define ID_MKLOG_FONT12						3	
#define ID_MKLOG_FONT20						4	


#pragma region DEF LOG WINDOW TIME ITEM CODE//ログウィンドのコントロールアイテムインデックスコード

#define ID_MKLOG_ITEM_TMCHART_FRAME			0//メインフレーム
#define ID_MKLOG_ITEM_TMCHART_GRAPH1		1//グラフ1
#define ID_MKLOG_ITEM_TMCHART_GRAPH2		2//グラフ2

#define ID_MKLOG_ITEM_TMCHART_CB_ACT		3//Actチェックボックス
#define ID_MKLOG_ITEM_TMCHART_PB_FILE		4//ログファイル保存PB
#define ID_MKLOG_ITEM_TMCHART_PB_BMP		5//ビットマップ保存PB
#define ID_MKLOG_ITEM_TMCHART_PB_CHART		6//Chart 起動/停止PB
#define ID_MKLOG_ITEM_TMCHART_PB_LIST		7//LIST表示PB
#define ID_MKLOG_ITEM_TMCHART_PB_PAUSE		8//CHART　PAUSEPB
#define ID_MKLOG_ITEM_TMCHART_PB_REFRESH	9//CHART RESTARTPB

#define ID_MKLOG_ITEM_TMCHART_PB_Xd2		10//X軸レンジ1/2PB
#define ID_MKLOG_ITEM_TMCHART_PB_Xx2		11//X軸レンジx2PB

#define ID_MKLOG_ITEM_TMCHART_PB_F1Yx1		12//Frame1 Y軸レンジx1PB
#define ID_MKLOG_ITEM_TMCHART_PB_F1Yx2		13//Frame1 Y軸レンジx2PB
#define ID_MKLOG_ITEM_TMCHART_PB_F1Yx4		14//Frame1 Y軸レンジx4PB
#define ID_MKLOG_ITEM_TMCHART_PB_F1YP10		15//Frame1 Y軸オフセット+10％PB
#define ID_MKLOG_ITEM_TMCHART_PB_F1YN10		16//Frame1 Y軸オフセット-10％PB
#define ID_MKLOG_ITEM_TMCHART_PB_F2Yx1		17//Frame2 Y軸レンジx1PB
#define ID_MKLOG_ITEM_TMCHART_PB_F2Yx2		18//Frame2 Y軸レンジx2PB
#define ID_MKLOG_ITEM_TMCHART_PB_F2Yx4		19//Frame2 Y軸レンジx4PB
#define ID_MKLOG_ITEM_TMCHART_PB_F2YP10		20//Frame2 Y軸オフセット+10％PB
#define ID_MKLOG_ITEM_TMCHART_PB_F2YN10		21//Frame2 Y軸オフセット-10％PB

	//	スタティック
#define ID_MKLOG_ITEM_TMCHART_ST_INF		32//テキスト　INFOMATION
#define ID_MKLOG_ITEM_TMCHART_ST_F1CODE1	33//テキスト　グラフ項目コード1
#define ID_MKLOG_ITEM_TMCHART_ST_F1CODE2	34//テキスト　グラフ項目コード2
#define ID_MKLOG_ITEM_TMCHART_ST_F1CODE3	35//テキスト　グラフ項目コード3
#define ID_MKLOG_ITEM_TMCHART_ST_F1CODE4	36//テキスト　グラフ項目コード4
#define ID_MKLOG_ITEM_TMCHART_ST_F1CODE5	37//テキスト　グラフ項目コード5
#define ID_MKLOG_ITEM_TMCHART_ST_F1CODE6	38//テキスト　グラフ項目コード6
#define ID_MKLOG_ITEM_TMCHART_ST_F1CODE7	39//テキスト　グラフ項目コード7
#define ID_MKLOG_ITEM_TMCHART_ST_F1CODE8	40//テキスト　グラフ項目コード8
#define ID_MKLOG_ITEM_TMCHART_ST_F2CODE1	41//テキスト　グラフ項目コード1
#define ID_MKLOG_ITEM_TMCHART_ST_F2CODE2	42//テキスト　グラフ項目コード2
#define ID_MKLOG_ITEM_TMCHART_ST_F2CODE3	43//テキスト　グラフ項目コード3
#define ID_MKLOG_ITEM_TMCHART_ST_F2CODE4	44//テキスト　グラフ項目コード4
#define ID_MKLOG_ITEM_TMCHART_ST_F2CODE5	45//テキスト　グラフ項目コード5
#define ID_MKLOG_ITEM_TMCHART_ST_F2CODE6	46//テキスト　グラフ項目コード6
#define ID_MKLOG_ITEM_TMCHART_ST_F2CODE7	47//テキスト　グラフ項目コード7
#define ID_MKLOG_ITEM_TMCHART_ST_F2CODE8	48//テキスト　グラフ項目コード8

#define ID_MKLOG_ITEM_TMCHART_ST_F1X_MIN	49//テキスト　Y軸min s
#define ID_MKLOG_ITEM_TMCHART_ST_F1X_MIDDLE	50//テキスト　Y軸middle s
#define ID_MKLOG_ITEM_TMCHART_ST_F1X_MAX	51//テキスト　Y軸max s
#define ID_MKLOG_ITEM_TMCHART_ST_F1Y_MIN	52//テキスト　Y軸min％
#define ID_MKLOG_ITEM_TMCHART_ST_F1Y_MIDDLE	53//テキスト　Y軸middle％
#define ID_MKLOG_ITEM_TMCHART_ST_F1Y_MAX	54//テキスト　Y軸max％
#define ID_MKLOG_ITEM_TMCHART_ST_F2X_MIN	55//テキスト　Y軸min s
#define ID_MKLOG_ITEM_TMCHART_ST_F2X_MIDDLE	56//テキスト　Y軸middle s
#define ID_MKLOG_ITEM_TMCHART_ST_F2X_MAX	57//テキスト　Y軸max s
#define ID_MKLOG_ITEM_TMCHART_ST_F2Y_MIN	58//テキスト　Y軸min％
#define ID_MKLOG_ITEM_TMCHART_ST_F2Y_MIDDLE	59//テキスト　Y軸middle％
#define ID_MKLOG_ITEM_TMCHART_ST_F2Y_MAX	60//テキスト　Y軸max％
#define ID_MKLOG_ITEM_TMCHART_ST_LABEL_X	61//テキスト　Y軸min s
#define ID_MKLOG_ITEM_TMCHART_ST_LABEL_F1Y	62//テキスト　Y軸middle s
#define ID_MKLOG_ITEM_TMCHART_ST_LABEL_F2Y	63//テキスト　Y軸max s

#define ID_MKLOG_ITEM_TMCHART_ED_F1CODE1	64//エディット　グラフ項目コード1
#define ID_MKLOG_ITEM_TMCHART_ED_F1CODE2	65//エディット　グラフ項目コード2
#define ID_MKLOG_ITEM_TMCHART_ED_F1CODE3	66//エディット　グラフ項目コード3
#define ID_MKLOG_ITEM_TMCHART_ED_F1CODE4	67//エディット　グラフ項目コード4
#define ID_MKLOG_ITEM_TMCHART_ED_F1CODE5	68//エディット　グラフ項目コード5
#define ID_MKLOG_ITEM_TMCHART_ED_F1CODE6	69//エディット　グラフ項目コード6
#define ID_MKLOG_ITEM_TMCHART_ED_F1CODE7	70//エディット　グラフ項目コード7
#define ID_MKLOG_ITEM_TMCHART_ED_F1CODE8	71//エディット　グラフ項目コード8
#define ID_MKLOG_ITEM_TMCHART_ED_F2CODE1	72//エディット　グラフ項目コード1
#define ID_MKLOG_ITEM_TMCHART_ED_F2CODE2	73//エディット　グラフ項目コード2
#define ID_MKLOG_ITEM_TMCHART_ED_F2CODE3	74//エディット　グラフ項目コード3
#define ID_MKLOG_ITEM_TMCHART_ED_F2CODE4	75//エディット　グラフ項目コード4
#define ID_MKLOG_ITEM_TMCHART_ED_F2CODE5	76//エディット　グラフ項目コード5
#define ID_MKLOG_ITEM_TMCHART_ED_F2CODE6	77//エディット　グラフ項目コード6
#define ID_MKLOG_ITEM_TMCHART_ED_F2CODE7	78//エディット　グラフ項目コード7
#define ID_MKLOG_ITEM_TMCHART_ED_F2CODE8	79//エディット　グラフ項目コード8

#define ID_MKLOG_ITEM_TMCHART_MEM_GRAPH1	100//メモリ（DC）上のグラフ1
#define ID_MKLOG_ITEM_TMCHART_MEM_GRAPH2	101//メモリ（DC）上のグラフ2

#define ID_MKLOG_ITEM_TMCHART_DISP_GRAPH_H	240//表示グラフエリア高さ
#define ID_MKLOG_ITEM_TMCHART_DISP_GRAPH_W	600//表示グラフエリア幅
#define ID_MKLOG_ITEM_TMCHART_MEM_GRAPH_H	1000//メモリ（DC）上のグラフエリア高さ
#define ID_MKLOG_ITEM_TMCHART_MEM_GRAPH_W	1500//メモリ（DC）上のグラフエリア幅

#pragma endregion

#pragma region DEF LOG WINDOW SCAT ITEM CODE//ログウィンドのコントロールアイテムインデックスコード

#define ID_MKLOG_ITEM_SCCHART_FRAME			0//メインフレーム
#define ID_MKLOG_ITEM_SCCHART_GRAPH1		1//グラフ1
#define ID_MKLOG_ITEM_SCCHART_GRAPH2		2//グラフ2

#define ID_MKLOG_ITEM_SCCHART_CB_ACT		3//Actチェックボックス
#define ID_MKLOG_ITEM_SCCHART_PB_FILE		4//ログファイル保存PB
#define ID_MKLOG_ITEM_SCCHART_PB_BMP		5//ビットマップ保存PB
#define ID_MKLOG_ITEM_SCCHART_PB_CHART		6//Chart 起動/停止PB
#define ID_MKLOG_ITEM_SCCHART_PB_LIST		7//LIST表示PB
#define ID_MKLOG_ITEM_SCCHART_PB_PAUSE		8//CHART　PAUSEPB
#define ID_MKLOG_ITEM_SCCHART_PB_REFRESH	9//CHART RESTARTPB

#define ID_MKLOG_ITEM_SCCHART_PB_TIMEx2		10//表示時間x2PB
#define ID_MKLOG_ITEM_SCCHART_PB_TIMEd2		11//表示時間1/2PB

#define ID_MKLOG_ITEM_SCCHART_PB_F1Xx1		12//Frame1 X軸レンジx1PB
#define ID_MKLOG_ITEM_SCCHART_PB_F1Xx2		13//Frame1 X軸レンジx2PB
#define ID_MKLOG_ITEM_SCCHART_PB_F1Xx4		14//Frame1 X軸レンジx4PB
#define ID_MKLOG_ITEM_SCCHART_PB_F1XP10		15//Frame1 X軸オフセット+10％PB
#define ID_MKLOG_ITEM_SCCHART_PB_F1XN10		16//Frame1 X軸オフセット-10％PB
#define ID_MKLOG_ITEM_SCCHART_PB_F1Yx1		17//Frame2 Y軸レンジx1PB
#define ID_MKLOG_ITEM_SCCHART_PB_F1Yx2		18//Frame2 Y軸レンジx2PB
#define ID_MKLOG_ITEM_SCCHART_PB_F1Yx4		19//Frame2 Y軸レンジx4PB
#define ID_MKLOG_ITEM_SCCHART_PB_F1YP10		20//Frame2 Y軸オフセット+10％PB
#define ID_MKLOG_ITEM_SCCHART_PB_F1YN10		21//Frame2 Y軸オフセット-10％PB
#define ID_MKLOG_ITEM_SCCHART_PB_F2Xx1		22//Frame1 X軸レンジx1PB
#define ID_MKLOG_ITEM_SCCHART_PB_F2Xx2		23//Frame1 X軸レンジx2PB
#define ID_MKLOG_ITEM_SCCHART_PB_F2Xx4		24//Frame1 X軸レンジx4PB
#define ID_MKLOG_ITEM_SCCHART_PB_F2XP10		25//Frame1 X軸オフセット+10％PB
#define ID_MKLOG_ITEM_SCCHART_PB_F2XN10		26//Frame1 X軸オフセット-10％PB
#define ID_MKLOG_ITEM_SCCHART_PB_F2Yx1		27//Frame2 Y軸レンジx1PB
#define ID_MKLOG_ITEM_SCCHART_PB_F2Yx2		28//Frame2 Y軸レンジx2PB
#define ID_MKLOG_ITEM_SCCHART_PB_F2Yx4		29//Frame2 Y軸レンジx4PB
#define ID_MKLOG_ITEM_SCCHART_PB_F2YP10		30//Frame2 Y軸オフセット+10％PB
#define ID_MKLOG_ITEM_SCCHART_PB_F2YN10		31//Frame2 Y軸オフセット-10％PB

	//	スタティック
#define ID_MKLOG_ITEM_SCCHART_ST_INF		32//テキスト　INFOMATION
#define ID_MKLOG_ITEM_SCCHART_ST_F1CODE1	33//テキスト　グラフ項目コード1
#define ID_MKLOG_ITEM_SCCHART_ST_F1CODE2	34//テキスト　グラフ項目コード2
#define ID_MKLOG_ITEM_SCCHART_ST_F1CODE3	35//テキスト　グラフ項目コード3
#define ID_MKLOG_ITEM_SCCHART_ST_F1CODE4	36//テキスト　グラフ項目コード4
#define ID_MKLOG_ITEM_SCCHART_ST_F1CODE5	37//テキスト　グラフ項目コード5
#define ID_MKLOG_ITEM_SCCHART_ST_F1CODE6	38//テキスト　グラフ項目コード6
#define ID_MKLOG_ITEM_SCCHART_ST_F1CODE7	39//テキスト　グラフ項目コード7
#define ID_MKLOG_ITEM_SCCHART_ST_F1CODE8	40//テキスト　グラフ項目コード8
#define ID_MKLOG_ITEM_SCCHART_ST_F2CODE1	41//テキスト　グラフ項目コード1
#define ID_MKLOG_ITEM_SCCHART_ST_F2CODE2	42//テキスト　グラフ項目コード2
#define ID_MKLOG_ITEM_SCCHART_ST_F2CODE3	43//テキスト　グラフ項目コード3
#define ID_MKLOG_ITEM_SCCHART_ST_F2CODE4	44//テキスト　グラフ項目コード4
#define ID_MKLOG_ITEM_SCCHART_ST_F2CODE5	45//テキスト　グラフ項目コード5
#define ID_MKLOG_ITEM_SCCHART_ST_F2CODE6	46//テキスト　グラフ項目コード6
#define ID_MKLOG_ITEM_SCCHART_ST_F2CODE7	47//テキスト　グラフ項目コード7
#define ID_MKLOG_ITEM_SCCHART_ST_F2CODE8	48//テキスト　グラフ項目コード8

#define ID_MKLOG_ITEM_SCCHART_ST_DISPTIME	49//テキスト

#define ID_MKLOG_ITEM_SCCHART_ST_F1X_MIN	50//テキスト　Y軸min s
#define ID_MKLOG_ITEM_SCCHART_ST_F1X_MIDDLE	51//テキスト　Y軸middle s
#define ID_MKLOG_ITEM_SCCHART_ST_F1X_MAX	52//テキスト　Y軸max s
#define ID_MKLOG_ITEM_SCCHART_ST_F1Y_MIN	53//テキスト　Y軸min％
#define ID_MKLOG_ITEM_SCCHART_ST_F1Y_MIDDLE	54//テキスト　Y軸middle％
#define ID_MKLOG_ITEM_SCCHART_ST_F1Y_MAX	55//テキスト　Y軸max％
#define ID_MKLOG_ITEM_SCCHART_ST_F2X_MIN	56//テキスト　Y軸min s
#define ID_MKLOG_ITEM_SCCHART_ST_F2X_MIDDLE	57//テキスト　Y軸middle s
#define ID_MKLOG_ITEM_SCCHART_ST_F2X_MAX	58//テキスト　Y軸max s
#define ID_MKLOG_ITEM_SCCHART_ST_F2Y_MIN	59//テキスト　Y軸min％
#define ID_MKLOG_ITEM_SCCHART_ST_F2Y_MIDDLE	60//テキスト　Y軸middle％
#define ID_MKLOG_ITEM_SCCHART_ST_F2Y_MAX	61//テキスト　Y軸max％

#define ID_MKLOG_ITEM_SCCHART_ED_F1CODE1	64//エディット　グラフ項目コード1
#define ID_MKLOG_ITEM_SCCHART_ED_F1CODE2	65//エディット　グラフ項目コード2
#define ID_MKLOG_ITEM_SCCHART_ED_F1CODE3	66//エディット　グラフ項目コード3
#define ID_MKLOG_ITEM_SCCHART_ED_F1CODE4	67//エディット　グラフ項目コード4
#define ID_MKLOG_ITEM_SCCHART_ED_F1CODE5	68//エディット　グラフ項目コード5
#define ID_MKLOG_ITEM_SCCHART_ED_F1CODE6	69//エディット　グラフ項目コード6
#define ID_MKLOG_ITEM_SCCHART_ED_F1CODE7	70//エディット　グラフ項目コード7
#define ID_MKLOG_ITEM_SCCHART_ED_F1CODE8	71//エディット　グラフ項目コード8
#define ID_MKLOG_ITEM_SCCHART_ED_F2CODE1	72//エディット　グラフ項目コード1
#define ID_MKLOG_ITEM_SCCHART_ED_F2CODE2	73//エディット　グラフ項目コード2
#define ID_MKLOG_ITEM_SCCHART_ED_F2CODE3	74//エディット　グラフ項目コード3
#define ID_MKLOG_ITEM_SCCHART_ED_F2CODE4	75//エディット　グラフ項目コード4
#define ID_MKLOG_ITEM_SCCHART_ED_F2CODE5	76//エディット　グラフ項目コード5
#define ID_MKLOG_ITEM_SCCHART_ED_F2CODE6	77//エディット　グラフ項目コード6
#define ID_MKLOG_ITEM_SCCHART_ED_F2CODE7	78//エディット　グラフ項目コード7
#define ID_MKLOG_ITEM_SCCHART_ED_F2CODE8	79//エディット　グラフ項目コード8

#define ID_MKLOG_ITEM_SCCHART_ST_LABEL_FX1	80//テキスト　Y軸min s
#define ID_MKLOG_ITEM_SCCHART_ST_LABEL_FY1	81//テキスト　Y軸middle s
#define ID_MKLOG_ITEM_SCCHART_ST_LABEL_FX2	82//テキスト　Y軸max s
#define ID_MKLOG_ITEM_SCCHART_ST_LABEL_FY2	83//テキスト　Y軸middle s
#define ID_MKLOG_ITEM_SCCHART_ST_LABEL_DISPTIME	84//テキスト
#define ID_MKLOG_ITEM_SCCHART_ST_LIST1		85//LIST テキスト
#define ID_MKLOG_ITEM_SCCHART_ST_LIST2		86//LIST テキスト

#define ID_MKLOG_ITEM_SCCHART_MEM_GRAPH1	100//メモリ（DC）上のグラフ1
#define ID_MKLOG_ITEM_SCCHART_MEM_GRAPH2	101//メモリ（DC）上のグラフ2


#define ID_MKLOG_ITEM_SCCHART_DISP_GRAPH_H	400//表示グラフエリア高さ
#define ID_MKLOG_ITEM_SCCHART_DISP_GRAPH_W	400//表示グラフエリア幅
#define ID_MKLOG_ITEM_SCCHART_MEM_GRAPH_H	1600//メモリ（DC）上のグラフエリア高さ
#define ID_MKLOG_ITEM_SCCHART_MEM_GRAPH_W	1600//メモリ（DC）上のグラフエリア幅

#define ID_MKLOG_ITEM_SCCHART_DISP_N_POINT	400	//表示グラフポイント数

#pragma endregion


#define MKLOG_N_HBMAP				4
#define ID_MKLOG_HBMAP_MEM0			0
#define ID_MKLOG_HBMAP_BG			1
#define ID_MKLOG_HBMAP_MEM_GR		2
#define ID_MKLOG_HBMAP_MEM_INFO		3

#define MKLOG_N_HDC					4
#define ID_MKLOG_HDC_MEM0			0
#define ID_MKLOG_HDC_BG				1//メイン背景用デバイスコンテキスト
#define ID_MKLOG_HDC_MEM_GR			2//メイン合成用デバイスコンテキスト
#define ID_MKLOG_HDC_MEM_INFO		3//メインIF表示用デバイスコンテキスト


#define MKLOG_N_GRAPHIC				4
#define ID_MKLOG_GR_MEM0			0
#define ID_MKLOG_GR_BG				1
#define ID_MKLOG_GR_MEM_GR			2
#define ID_MKLOG_GR_MEM_INFO		3

#define MKLOG_N_IMAGE				4
#define ID_MKLOG_IMAGE_MEM0			0
#define ID_MKLOG_IMAGE_BG			1
#define ID_MKLOG_IMAGE_MEM_GR		2
#define ID_MKLOG_IMAGE_MEM_INFO		3

#define MKLOG_N_GRAPH				4
#define ID_MKLOG_PLOT_GRAPH1_A		0	//アナログ用グラフィックエリア
#define ID_MKLOG_PLOT_GRAPH2_A		1	//アナログ用グラフィックエリア
#define ID_MKLOG_PLOT_GRAPH1_D		2	//デジタル用グラフィックエリア
#define ID_MKLOG_PLOT_GRAPH2_D		3	//デジタル用グラフィックエリア

#define MKLOG_N_AREA				16
#define ID_MKLOG_TM_GR_AREA1		0
#define ID_MKLOG_TM_GR_AREA2		1
#define ID_MKLOG_TM_CHART_AREA1_1	2	//フレーム１の前半エリア
#define ID_MKLOG_TM_CHART_AREA1_2	3	//フレーム１の後半エリア
#define ID_MKLOG_TM_CHART_AREA2_1	4	//フレーム２の前半エリア
#define ID_MKLOG_TM_CHART_AREA2_2	5	//フレーム２の後半エリア
#define ID_MKLOG_TM_BG_PLANE_AREA	6	

#define ID_MKLOG_SC_GR_AREA1		0
#define ID_MKLOG_SC_GR_AREA2		1
#define ID_MKLOG_SC_BG_PLANE_AREA	2	


#define MKLOG_N_LINE			64
#define MKLOG_N_CHART_MAX		16
#define ID_MKLOG_TM_CHART1_1	0
#define ID_MKLOG_TM_CHART1_2	1
#define ID_MKLOG_TM_CHART1_3	2
#define ID_MKLOG_TM_CHART1_4	3
#define ID_MKLOG_TM_CHART1_5	4
#define ID_MKLOG_TM_CHART1_6	5
#define ID_MKLOG_TM_CHART1_7	6
#define ID_MKLOG_TM_CHART1_8	7
#define ID_MKLOG_TM_CHART2_1	8
#define ID_MKLOG_TM_CHART2_2	9
#define ID_MKLOG_TM_CHART2_3	10
#define ID_MKLOG_TM_CHART2_4	11
#define ID_MKLOG_TM_CHART2_5	12
#define ID_MKLOG_TM_CHART2_6	13
#define ID_MKLOG_TM_CHART2_7	14
#define ID_MKLOG_TM_CHART2_8	15

#define ID_MKLOG_TM_CHART1_X0	32
#define ID_MKLOG_TM_CHART1_Y0	33
#define ID_MKLOG_TM_CHART2_X0	34
#define ID_MKLOG_TM_CHART2_Y0	35

#define ID_MKLOG_SC_CHART1_1	0
#define ID_MKLOG_SC_CHART1_2	1
#define ID_MKLOG_SC_CHART2_1	2
#define ID_MKLOG_SC_CHART2_2	3
#define ID_MKLOG_SC_CHART3_1	4
#define ID_MKLOG_SC_CHART3_2	5
#define ID_MKLOG_SC_CHART4_1	6
#define ID_MKLOG_SC_CHART4_2	7
#define ID_MKLOG_SC_CHART5_1	8
#define ID_MKLOG_SC_CHART5_2	9
#define ID_MKLOG_SC_CHART6_1	10
#define ID_MKLOG_SC_CHART6_2	11
#define ID_MKLOG_SC_CHART7_1	12
#define ID_MKLOG_SC_CHART7_2	13
#define ID_MKLOG_SC_CHART8_1	14
#define ID_MKLOG_SC_CHART8_2	15

#define ID_MKLOG_SC_CHART1_X0	32
#define ID_MKLOG_SC_CHART1_Y0	33
#define ID_MKLOG_SC_CHART2_X0	34
#define ID_MKLOG_SC_CHART2_Y0	35

#define ID_MKLOG_ITEM_TXT_NAME  0
#define ID_MKLOG_ITEM_TXT_100   100


typedef struct StMklogArea {
	INT x;
	INT y;
	INT w;
	INT h;
}ST_MKLOG_AREA, *LPST_MKLOG_AREA;

typedef struct StMklogLine {
	INT x1;
	INT y1;
	INT x2;
	INT y2;
}ST_MKLOG_LINE, * LPST_MKLOG_LINE;

typedef struct StLogTmChartCtrl {
	UINT32 icon_draw			= L_OFF;//グラフ色表示アイコン表示したかどうかのフラグ
	UINT32 req_bg_update		= L_OFF;//背景更新フラグ（レンジ変更等で実行）
	UINT32 req_graph_update		= L_OFF;//チャートグラフ描画更新フラグ（WM＿PAINTでの描画実行制御
	UINT32 is_disp_over_range	= L_OFF;//チャートが表示エリア（画面表示エリアx2）を1巡したかどうかのフラグ
	
	UINT32 chart_item_txt_mode = ID_MKLOG_ITEM_TXT_NAME;

	INT32 plot_counter			= 0;//チャート描画時間軸PIXの更新タイミング判定用（スキャン毎にカウント）
	INT32 plot_count_limit		= 1;//チャート描画時間軸PIXの更新タイミング判定値）
	INT32 plot_xshift_pix		= 1;
			
	LONGLONG eraps_ms				= 0;			//チャート起動からの経過時間ms
	UINT scan_time_ms				= 100;
	INT ms_per_pix					= 100;			//Pixel当りのms数
	
	INT disp_offset_y[MKLOG_N_GRAPH]= { 0,0,-100,-100 };//Y軸の表示0点オフセット
	//メモリグラフの0点オフセット
	//0:CHART1のアナログ　1：CHART2のアナログ　2:CHART1のデジタル　3：CHART2のデジタル　のY軸0点位置
	INT mem_offset_y[MKLOG_N_GRAPH] = { ID_MKLOG_ITEM_TMCHART_MEM_GRAPH_H/2,ID_MKLOG_ITEM_TMCHART_MEM_GRAPH_H *3 / 2,ID_MKLOG_ITEM_TMCHART_MEM_GRAPH_H,ID_MKLOG_ITEM_TMCHART_MEM_GRAPH_H*2 };

	INT disp_mag_yc[MKLOG_N_GRAPH]	= { 100,100,100,100 };	//Y軸の倍率％
	
	INT pix_disp_range				= ID_MKLOG_ITEM_TMCHART_DISP_GRAPH_W;			//Chart表示領域の時間幅pix
	INT pix_time_plot_range			= ID_MKLOG_ITEM_TMCHART_DISP_GRAPH_W*2;			//Chartプロットエリアの時間軸幅pix
	
	INT pt_time_plot_pix			= 0;			//プロット点の時間軸PIXEL
	INT pt_time_plot_pix_last		= 0;			//プロット点の時間軸PIXEL前回値
	INT pt_time_erase_pix			= 0;			//プロット点クリアの時間軸PIXEL
	INT pix_dot_size				= 5;			//ポイント表示自のサイズ
	INT division_time_axis			= 60;			//時間軸の目盛り表示間隔PIX
	INT division_y_axis				= 20;			//Y軸の目盛り表示間隔PIX

	INT plot_item_code[MKLOG_N_CHART_MAX]	= { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};	//チャート項目画面設定コード
	INT plot_item_offset[MKLOG_N_CHART_MAX] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };	//チャート毎のオフセット
	INT color_chart_item[MKLOG_N_CHART_MAX] = {
		ID_MKLOG_COLOR_RED,ID_MKLOG_COLOR_ORANGE,ID_MKLOG_COLOR_GREEN,ID_MKLOG_COLOR_CYAN,ID_MKLOG_COLOR_BLUE,ID_MKLOG_COLOR_PURPLE,ID_MKLOG_COLOR_BROWN,ID_MKLOG_COLOR_MAZENDA,
		ID_MKLOG_COLOR_RED,ID_MKLOG_COLOR_ORANGE,ID_MKLOG_COLOR_GREEN,ID_MKLOG_COLOR_CYAN,ID_MKLOG_COLOR_BLUE,ID_MKLOG_COLOR_PURPLE,ID_MKLOG_COLOR_BROWN,ID_MKLOG_COLOR_MAZENDA
	};
	INT chart_item_set_code[MKLOG_N_CHART_MAX] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };

	INT chart_item_disp_y0[MKLOG_N_CHART_MAX] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };

	ST_MKLOG_AREA area[MKLOG_N_AREA];
	ST_MKLOG_LINE line[MKLOG_N_LINE];

}ST_LOG_TIME_CHART_CTRL, * LPST_LOG_TIME_CHART_CTRL;

typedef struct StLogScChartCtrl {
	UINT32 icon_draw = L_OFF;//グラフ色表示アイコン表示したかどうかのフラグ
	UINT32 req_bg_update = L_OFF;//背景更新フラグ（レンジ変更等で実行）
	UINT32 req_graph_update = L_OFF;//チャートグラフ描画更新フラグ（WM＿PAINTでの描画実行制御
	UINT32 is_disp_over_range = L_OFF;//チャートが表示エリア（画面表示エリアx2）を1巡したかどうかのフラグ

	UINT32 chart_item_txt_mode = ID_MKLOG_ITEM_TXT_NAME;

	LONGLONG eraps_ms = 0;			//チャート起動からの経過時間ms
	UINT scan_time_ms = 100;

	INT disp_offset_x[MKLOG_N_GRAPH] = { 0,0,0,0};//Y軸の表示0点オフセット
	INT disp_offset_y[MKLOG_N_GRAPH] = { 0,0,0,0 };//Y軸の表示0点オフセット
	INT disp_mag_xc[MKLOG_N_GRAPH] = { 100,100,100,100 };	//X軸の倍率％
	INT disp_mag_yc[MKLOG_N_GRAPH] = { 100,100,100,100 };	//Y軸の倍率％

	INT plot_disp_range = ID_MKLOG_ITEM_SCCHART_DISP_N_POINT;					//Chart表示領域の時間幅pix

	INT pt_plot_index			= 0;			//プロット点の時間軸PIXEL
	INT pt_plot_index_last		= 0;			//プロット点の時間軸PIXEL前回値
	INT pt_plot_erase_index		= 0;			//プロット点クリアの時間軸PIXEL
	INT pix_dot_size			= 4;			//ポイント表示のサイズ
	INT pix_half_dot_size		= 2;			//ポイント表示のサイズ
	INT division_x_axis			= 20;			//X軸の目盛り表示間隔PIX
	INT division_y_axis			= 20;			//Y軸の目盛り表示間隔PIX

	INT plot_item_code[MKLOG_N_CHART_MAX] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };	//チャート項目画面設定コード
	INT plot_item_offset[MKLOG_N_CHART_MAX] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };	//チャート毎のオフセット
	INT color_chart_item[MKLOG_N_CHART_MAX] = {
		ID_MKLOG_COLOR_RED,ID_MKLOG_COLOR_RED,ID_MKLOG_COLOR_GREEN,ID_MKLOG_COLOR_GREEN,ID_MKLOG_COLOR_BLUE,ID_MKLOG_COLOR_BLUE,ID_MKLOG_COLOR_MAZENDA,ID_MKLOG_COLOR_MAZENDA,
		ID_MKLOG_COLOR_RED,ID_MKLOG_COLOR_RED,ID_MKLOG_COLOR_GREEN,ID_MKLOG_COLOR_GREEN,ID_MKLOG_COLOR_BLUE,ID_MKLOG_COLOR_BLUE,ID_MKLOG_COLOR_MAZENDA,ID_MKLOG_COLOR_MAZENDA,
	};
	INT chart_item_set_code[MKLOG_N_CHART_MAX] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };

	ST_MKLOG_AREA area[MKLOG_N_AREA];
	ST_MKLOG_LINE line[MKLOG_N_LINE];

}ST_LOG_SCAT_CHART_CTRL, * LPST_LOG_SCAT_CHART_CTRL;


#define MKLOG_TM_DISP_X_MARGINE					55
#define MKLOG_TM_DISP_H_MARGINE					30

typedef struct StTimeLogWindow {

	HBITMAP hBmp[MKLOG_N_HBMAP]	 = { NULL,NULL,NULL,NULL };			//ビットマップハンドル
	HDC		hdc[MKLOG_N_HDC] = { NULL,NULL,NULL,NULL };			//メモリデバイスコンテキスト

	Image*		pimg[MKLOG_N_IMAGE];
	Graphics*	pgraphic[MKLOG_N_GRAPHIC];

	//Main Frame
	HWND hwnd_obj[ID_MKLOG_NOBJ_FRAME] = {
		NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
		NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
		NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
		NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
		NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
		NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
		NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
		NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
		NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
		NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
		NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
		NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
		NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL
	};

	UINT32 stat_obj[ID_MKLOG_NOBJ_FRAME] = {
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0
	};

	POINT pt_obj[ID_MKLOG_NOBJ_FRAME]	= {
		{MKLOG_WND_TIME_X,MKLOG_WND_TIME_Y},{MKLOG_TM_DISP_X_MARGINE,105},{MKLOG_TM_DISP_X_MARGINE,425},{30,15},{605,12},{647,12},{697,12},{702,50},{675,590},{675,640},			//0-9
		{675,350},{720,350},{674,127},{674,161},{674,193},{710,127},{710,161},{674,445},{674,479},{674,511},//10-
		{710,445},{710,489},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},								//20-
		{0,0},{0,0},{90,15},{55,87},{130,87},{205,87},{280,87},{355,87},{430,87},{505,87},					//30-
		{580,87},{55,407},{130,407},{205,407},{280,407},{355,407},{430,407},{505,407},{580,407},{105,620},	//40-
		{355,620},{625,620},{0,590},{0,490},{0,390},{105,1620},{355,1620},{625,1620},{0,1590},{0,1490},		//50-
		{0,1390},{705,325},{698,100},{698,420},{55,59},{130,59},{205,59},{280,59},{355,59},{430,59},		//60-
		{505,59},{580,59},{55,379},{130,379},{205,379},{280,379},{355,379},{430,379},{505,379},{580,379},	//70-
		{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},										//80-
		{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},										//90-
		{0,0},{0,ID_MKLOG_ITEM_TMCHART_MEM_GRAPH_H},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},		//100-
		{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},										//110-
		{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0}														//120-128
	};

	SIZE  size_obj[ID_MKLOG_NOBJ_FRAME] = {
		{MKLOG_WND_TIME_W,MKLOG_WND_TIME_H},{600,240},{600,240},{60,20},{40,30},{50,30},{50,30},{50,30},{80,40},{80,40},	//0-9
		{30,25},{30,25},{30,25},{30,25},{30,25},{30,25},{30,25},{30,25},{30,25},{30,25},		//10-
		{30,25},{30,25},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},						//20-
		{0,0},{0,0},{500,20},{70,15},{70,15},{70,15},{70,15},{70,15},{70,15},{70,15},			//30-
		{70,15},{70,15},{70,15},{70,15},{70,15},{70,15},{70,15},{70,15},{70,15},{30,20},		//40-
		{30,20},{30,20},{45,20},{45,20},{45,20},{30,20},{30,20},{30,20},{45,20},{45,20},		//50-
		{45,20},{15,20},{15,20},{15,20},{50,25},{50,25},{50,25},{50,25},{50,25},{50,25},		//60-
		{50,25},{50,25},{50,25},{50,25},{50,25},{50,25},{50,25},{50,25},{50,25},{50,25},		//70-
		{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},							//80-
		{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},							//90-
		{ID_MKLOG_ITEM_TMCHART_MEM_GRAPH_W,ID_MKLOG_ITEM_TMCHART_MEM_GRAPH_H},{ID_MKLOG_ITEM_TMCHART_MEM_GRAPH_W,ID_MKLOG_ITEM_TMCHART_MEM_GRAPH_H},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},					//100-
		{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},							//110-
		{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0}											//120-128
	};

	WCHAR tx_obj[ID_MKLOG_NOBJ_FRAME][ID_MKLOG_NOBJ_TXT] = {
		L"FRAME",L"G1",L"G2",L"act",L"FILE",L"BMP",L"CHART",L"100%",L"PAUSE",L"REFRESH",	//0-9
		L"x2",L"/2",L"x1",L"x2",L"x4",L"+10",L"-10",L"x1",L"x2",L"x4",						//10-
		L"+10",L"-10",L"x1",L"x2",L"x4",L"+10",L"-10",L"x1",L"x2",L"x4",					//20-
		L"+10",L"-10",L"-",L"-",L"-",L"-",L"-",L"-",L"-",L"-",								//30-
		L"-",L"-",L"-",L"-",L"-",L"-",L"-",L"-",L"-",L"10s",								//40-
		L"30s",L"50s",L"-100%",L"   0%",L" 100%",L"10s",L"30s",L"50s",L"-100%",L"   0%",	//50-
		L" 100%",L"X",L"Y",L"Y",	L"0",L"0",L"0",L"0",L"0",L"0",							//60-
		L"0",L"0",L"0",L"0",L"0",L"0",L"0",L"0",L"0",L"0",									//70-
		L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",											//80-
		L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",											//90-
		L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",											//100-
		L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",											//110-
		L"",L"",L"",L"",L"",L"",L"",L""														//120-128
	};

	RECT rc[ID_MKLOG_NOBJ_FRAME]; 

	}ST_TIMELOG_WINDOW, *LPST_TIME_LOG_WINDOW;


//SCAT ##############################################################
#define MKLOG_SC_DISP_X_MARGINE					55
#define MKLOG_SC_DISP_H_MARGINE					30

typedef struct StScatLogWindow {

	HBITMAP hBmp[MKLOG_N_HBMAP] = { NULL,NULL,NULL,NULL };			//ビットマップハンドル
	HDC		hdc[MKLOG_N_HDC] = { NULL,NULL,NULL,NULL };			//メモリデバイスコンテキスト

	Image* pimg[MKLOG_N_IMAGE];
	Gdiplus::Graphics* pgraphic[MKLOG_N_GRAPHIC];

	//Main Frame
	HWND hwnd_obj[ID_MKLOG_NOBJ_FRAME] = {
		NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
		NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
		NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
		NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
		NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
		NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
		NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
		NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
		NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
		NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
		NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
		NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
		NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL
	};

	UINT32 stat_obj[ID_MKLOG_NOBJ_FRAME] = {
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0
	};

	POINT pt_obj[ID_MKLOG_NOBJ_FRAME] = {
		{MKLOG_WND_TIME_X,MKLOG_WND_TIME_Y},{MKLOG_SC_DISP_X_MARGINE,45},{MKLOG_SC_DISP_X_MARGINE,475},{30,15},{505,12},{547,12},{597,12},{602,50},{460,413},{565,413},			//0-9
		{563,480},{613,480},{470,107},{470,141},{470,175},{508,107},{508,141},{574,107},{574,141},{574,175},								//10-
		{612,107},{612,141},{470,556},{470,590},{470,622},{508,556},{508,590},{574,556},{574,590},{574,622},								//20-
		{612,556},{612,590},{90,15},{460,231},{565,231},{460,276},{565,276},{460,321},{565,321},{460,366},									//30-
		{ 565,366 }, { 460,678 }, { 565,678 }, { 460,723 }, { 565,723 }, { 460,768 }, { 565,768 }, { 460,818 }, { 565,813 },{465,490},		//40-
		{50,450},{230,450},{410,450},{0,425},{0,235},{0,45},{50,880},{230,880},{410,880},{0,860},											//50-
		{0,670},{ 0, 470 }, {490,100},{490,425} , { 460,205 }, { 565,205 }, { 460,250 }, { 565,250 }, { 460,295 }, { 565,295 },				//60-
		{ 460,340 }, { 565,340 }, { 460,650 }, { 565,650 }, { 460,695 }, { 565,695 }, { 460,740 }, { 565,740 }, { 460,785 }, { 565,785 },	//70-
		{489,85},{593,85},{489,534},{593,534},{465,465},{460,340},{460,785},{0,0},{0,0},{0,0},														//80-
		{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},																		//90-
		{0,0},{0,ID_MKLOG_ITEM_SCCHART_MEM_GRAPH_H},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},										//100-
		{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},																		//110-
		{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0}																						//120-128
	};

	SIZE  size_obj[ID_MKLOG_NOBJ_FRAME] = {
		{MKLOG_WND_SCAT_W,MKLOG_WND_SCAT_H},{ID_MKLOG_ITEM_SCCHART_DISP_GRAPH_W,ID_MKLOG_ITEM_SCCHART_DISP_GRAPH_H},{ID_MKLOG_ITEM_SCCHART_DISP_GRAPH_W,ID_MKLOG_ITEM_SCCHART_DISP_GRAPH_H},{60,20},{40,30},{50,30},{50,30},{50,30},{80,40},{80,40},	//0-9
		{30,25},{30,25},{30,25},{30,25},{30,25},{30,25},{30,25},{30,25},{30,25},{30,25},		//10-
		{30,25},{30,25},{30,25},{30,25},{30,25},{30,25},{30,25},{30,25},{30,25},{30,25},		//20-
		{30,25},{30,25},{400,20},{70,15},{70,15},{70,15},{70,15},{70,15},{70,15},{70,15},		//30-
		{70,15},{70,15},{70,15},{70,15},{70,15},{70,15},{70,15},{70,15},{70,15},{80,20},		//40-
		{45,20},{45,20},{30,20},{30,20},{30,20},{45,20},{45,20},{45,20},{45,20},{45,20},		//50-
		{45,20},{45,20},{15,20},{15,20},{50,25},{50,25},{50,25},{50,25},{50,25},{50,25},		//60-
		{50,25},{50,25},{50,25},{50,25},{50,25},{50,25},{50,25},{50,25},{50,25},{50,25},		//70-
		{15,20},{15,20},{15,20},{15,20},{80,20},{2000,200},{200,200},{0,0},{0,0},{0,0},			//80-
		{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},							//90-
		{ID_MKLOG_ITEM_SCCHART_MEM_GRAPH_W,ID_MKLOG_ITEM_SCCHART_MEM_GRAPH_H},{ID_MKLOG_ITEM_SCCHART_MEM_GRAPH_W,ID_MKLOG_ITEM_SCCHART_MEM_GRAPH_H},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},					//100-
		{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},							//110-
		{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0}											//120-128
	};

	WCHAR tx_obj[ID_MKLOG_NOBJ_FRAME][ID_MKLOG_NOBJ_TXT] = {
		L"FRAME",L"G1",L"G2",L"act",L"FILE",L"BMP",L"CHART",L"100%",L"PAUSE",L"REFRESH",			//0-9
		L"x2",L"/2",L"x1",L"x2",L"x4",L"+10",L"-10",L"x1",L"x2",L"x4",								//10-
		L"+10",L"-10",L"x1",L"x2",L"x4",L"+10",L"-10",L"x1",L"x2",L"x4",							//20-
		L"+10",L"-10",L"-",L"-",L"-",L"-",L"-",L"-",L"-",L"-",										//30-
		L"-",L"-",L"-",L"-",L"-",L"-",L"-",L"-",L"-",L"???",										//40-
		L"-100%",L"   0%",L" 100%",L"-100%",L"   0%",L" 100%",L"-100%",L"   0%",L" 100%",L" -100%",	//50-
		L"   0%",L" 100%",L"0",L"0",L"0",L"0",L"0",L"0",L"0",L"0",									//60-
		L"0",L"0",L"0",L"0",L"0",L"0",L"0",L"0",L"0",L"0",											//70-
		L"X",L"Y",L"X",L"Y",L"DispTime",L"LIST1",L"LIST2",L"",L"",L"",								//80-
		L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",													//90-
		L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",													//100-
		L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",													//110-
		L"",L"",L"",L"",L"",L"",L"",L""																//120-128
	};

	RECT rc[ID_MKLOG_NOBJ_FRAME];

}ST_SCATLOG_WINDOW, * LPST_SCATLOG_WINDOW;