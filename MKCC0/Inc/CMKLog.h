#pragma once

#include <commctrl.h>
#include <time.h>
#include <Windows.h>

#include "MKLog_DEF.h"

#include<chrono>

#include "..\Common\CBasicControl.h"



//LOGサンプリングタイマー
#define ID_LOG_TIMER_EVENT			101
#define ID_LOG_TIMER_TIME			102
#define ID_LOG_TIMER_SCAT			103
#define ID_LOG_TIMER_TRAP			104


#define MKLOG_DEF_LOG_DISP_100MS	100
#define MKLOG_DEF_LOG_DISP_1000MS	1000

#define MKLOG_CHART_WND_MAX			2

#define MKLOG_N_BRUSH				16
#define MKLOG_N_PEN					16
#define MKLOG_N_FONT				16


//COLORREF 0x00bbggrr
constexpr auto CODE_MKLOG_COLOR_BLACK	= 0x00909090;
constexpr auto CODE_MKLOG_COLOR_WHITE	= 0x00FFFFFF;
constexpr auto CODE_MKLOG_COLOR_GLAY_BK = 0x00D9D9D9;
constexpr auto CODE_MKLOG_COLOR_RED		= 0x000000FF;
constexpr auto CODE_MKLOG_COLOR_ORANGE	= 0x0000A8FF;
constexpr auto CODE_MKLOG_COLOR_GREEN	= 0x0000FF00;
constexpr auto CODE_MKLOG_COLOR_CYAN	= 0x00FFFF00;
constexpr auto CODE_MKLOG_COLOR_BLUE	= 0x00FF0000;
constexpr auto CODE_MKLOG_COLOR_PURPLE	= 0x00FF0093;
constexpr auto CODE_MKLOG_COLOR_BROWN	= 0x0000007F;
constexpr auto CODE_MKLOG_COLOR_MAZENDA = 0x00FF00FF;
constexpr auto CODE_MKLOG_COLOR_YELLOW	= 0x0000FFFF;


#define DEF_MKLOG_COLOR_BLACK			255,0x90,0x90,0x90
#define DEF_MKLOG_COLOR_BLACK0			255,0x0,0x0,0x0
#define DEF_MKLOG_COLOR_WHITE			255,0xFF,0xFF,0xFF
#define DEF_MKLOG_COLOR_GLAY_BK			255,0xD9,0xD9,0xD9
#define DEF_MKLOG_COLOR_RED				255,0xFF,0x00,0x00
#define DEF_MKLOG_COLOR_ORANGE			255,0xFF,0xA8,0x00
#define DEF_MKLOG_COLOR_GREEN			255,0x00,0xFF,0x00
#define DEF_MKLOG_COLOR_CYAN			255,0x00,0xFF,0xFF
#define DEF_MKLOG_COLOR_BLUE			255,0x00,0x00,0xFF
#define DEF_MKLOG_COLOR_PURPLE			255,0x93,0x00,0xFF
#define DEF_MKLOG_COLOR_BROWN			255,0x7F,0x00,0x00
#define DEF_MKLOG_COLOR_MAZENDA			255,0xFF,0x00,0xFF
#define DEF_MKLOG_COLOR_YELLOW			255,0x00,0xFF,0xFF
#define DEF_MKLOG_COLOR_DASH_GRAY		255,0xA0,0xA0,0xA0

typedef struct _stMKLogWorkWnd {

	UINT32 is_timer_active[MKLOG_N_ID_TYPE] = { L_OFF, L_OFF, L_OFF, L_OFF };

	//ColorPallet
	DWORD color_pallet[ID_MKLOG_NCOLOR] = {
	CODE_MKLOG_COLOR_BLACK, CODE_MKLOG_COLOR_WHITE, CODE_MKLOG_COLOR_GLAY_BK, CODE_MKLOG_COLOR_RED,
	CODE_MKLOG_COLOR_ORANGE,CODE_MKLOG_COLOR_GREEN,	CODE_MKLOG_COLOR_CYAN,	  CODE_MKLOG_COLOR_BLUE,
	CODE_MKLOG_COLOR_PURPLE,CODE_MKLOG_COLOR_BROWN,	CODE_MKLOG_COLOR_MAZENDA, CODE_MKLOG_COLOR_YELLOW
	};

	CBasicControl* phost = NULL;//ライブラリロジック利用ホストタスクインスタンス
	HWND hwnd[MKLOG_N_ID_TYPE]		=	{NULL,NULL,NULL,NULL };	//WINDOW　HANDLE	
	POINT pt_wnd[MKLOG_N_ID_TYPE]	=	{	//WINDOW　位置
		{MKLOG_WND_EVENT_X,MKLOG_WND_EVENT_Y},{MKLOG_WND_TIME_X,MKLOG_WND_TIME_Y},{MKLOG_WND_TRAP_X,MKLOG_WND_TRAP_Y},{MKLOG_WND_SCAT_X,MKLOG_WND_SCAT_Y}
	};
	SIZE  size_wnd[MKLOG_N_ID_TYPE] =	{	//WINDOW　サイズ
		{MKLOG_WND_EVENT_W,MKLOG_WND_EVENT_H},{MKLOG_WND_TIME_W,MKLOG_WND_TIME_H},{MKLOG_WND_TRAP_W,MKLOG_WND_TRAP_H},{MKLOG_WND_SCAT_W,MKLOG_WND_SCAT_H}
	};

	
	//描画関連
	HBRUSH		hbrush[MKLOG_N_BRUSH];
	HPEN		hpen[MKLOG_N_PEN];
	HFONT		hfont[MKLOG_N_FONT];

	Pen*		ppen[MKLOG_N_PEN] = { NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL };
	SolidBrush* pbrush[MKLOG_N_BRUSH] = { NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL };

	BLENDFUNCTION bf = { 0,0,0,0 };	//半透過設定構造体
	Image* pimg[MKLOG_N_ID_TYPE];
	Gdiplus::Graphics* pgraphic[MKLOG_N_ID_TYPE];

}ST_MKLOG_WORK_WND, * LPST_MKLOG_WORK_WND;

typedef union UN_MKLOG_WND_ACTIVE {
	INT16 i16[MKLOG_N_ID_TYPE];
	INT64 i64;
};

/// <summary>
/// CHART管理構造体
/// </summary>

class CMKLog
{
public:
	CMKLog();		//staticクラスにするのでprivateにする
	~CMKLog();	//staticクラスにするのでprivateにする
public:
	static UN_MKLOG_WND_ACTIVE is_log_wnd_active;

	static std::wostringstream wos,filename;
	
	static ST_LOG_FILE_BUF logbuf[MKLOG_N_ID_TYPE];

	static LPST_MKLOG_DB pdb;

	static void set_db(LPST_MKLOG_DB pdb_) { pdb = pdb_;  return; }

	static INT16 loghot[MKLOG_N_ID_TYPE][MKLOG_N_LOG_ITEM_MAX];
	static INT32 logidhot[MKLOG_N_ID_TYPE];

	static INT32 get_logstatus(int logtype) { return logbuf[logtype].log_status; }
	static int set_logstatus(int logtype, INT32 status) { logbuf[logtype].log_status = status; return 0; };

	static LRESULT CALLBACK EventLogWndProc(HWND, UINT, WPARAM, LPARAM);//Log Window
	static LRESULT CALLBACK TimeLogWndProc(HWND, UINT, WPARAM, LPARAM);	//Time-Data Graph
	static LRESULT CALLBACK ScatLogWndProc(HWND, UINT, WPARAM, LPARAM);	//Time-Data Graph
	static LRESULT CALLBACK TrapLogWndProc(HWND, UINT, WPARAM, LPARAM);	//Time-Data Graph

	static HWND open_log_event(HWND hwnd_parent);	//Log Window 起動
	static HWND open_log_time(HWND hwnd_parent);	//Log Window 起動
	static HWND open_log_scat(HWND hwnd_parent);	//Log Window 起動
	static HWND open_log_trap(HWND hwnd_parent);	//Log Window 起動

	static unsigned __stdcall logfileout(void* params); 

	static ST_MKLOG_WORK_WND st_work_wnd;
	static ST_TIMELOG_WINDOW st_timelog_wnd;
	static ST_SCATLOG_WINDOW st_scatlog_wnd;
	static ST_LOG_TIME_CHART_CTRL st_tm_chart_ctrl;
	static ST_LOG_SCAT_CHART_CTRL st_sc_chart_ctrl;
	static LPST_LOG_HEADER phead[MKLOG_N_ID_TYPE];

	static void set_log_head(int type, LPST_LOG_HEADER phead_) { phead[type] = phead_; return; }

	static int setup_timelog_wnd(HWND hWnd);
	static int setup_scatlog_wnd(HWND hWnd);

	static void set_host(CBasicControl* phost) { 
		st_work_wnd.phost = phost; return; 
	}
	static void create_wnd_objects(int log_type);
	static void delete_wnd_objects(int log_type);

//#### LOG HEADER設定
	//LOG ファイルのPCコードセット
	static void set_PCcode(INT32 code) { for (int i = 0; i < MKLOG_N_ID_TYPE; i++) logbuf[i].header.code[MKLOG_INDEX_PC_CODE] = code;   return; }
	//LOG ファイルのITEM数セット
	static int set_nItem(int log_type, INT32 n) { 
		if ((log_type < MKLOG_ID_TYPE_TIME) || (log_type > MKLOG_ID_TYPE_SCAT))return 1;
		logbuf[log_type].n_item =logbuf[log_type].header.code[MKLOG_INDEX_N_ITEM] = n;
		
		return 0;
	}
	//LOG ファイルのSCAN TIMEセット
	static int set_ScanTime(int log_type, double scan) {
		if ((log_type < MKLOG_ID_TYPE_TIME) || (log_type > MKLOG_ID_TYPE_SCAT))return 1;//範囲外
		logbuf[log_type].header.d100[MKLOG_INDEX_SCAN_MS] = scan;   return 0;
	}
	//LOG　ITEMの100％値セット
	static int set_Item100P(int log_type, int id, double val) { 
		//ログタイプ範囲外チェック
		if ((log_type < MKLOG_ID_TYPE_TIME) || (log_type > MKLOG_ID_TYPE_SCAT))return 1;
		//ログ項目数範囲外チェック
		if ((id < 0) || (id > MKLOG_N_LOG_ITEM_MAX + MKLOG_INDEX_LOG_DATA0))return 2;

		logbuf[log_type].header.d100[id] = val;   return 0; 
	}
	//LOG　ITEMのTypeセット
	static int set_ItemType(int log_type, int id, INT32 val) { 
		//ログタイプ範囲外チェック
		if ((log_type < MKLOG_ID_TYPE_TIME) || (log_type > MKLOG_ID_TYPE_SCAT))return 1;
		//ログ項目数範囲外チェック
		if ((id < 0) || (id > MKLOG_N_LOG_ITEM_MAX + MKLOG_INDEX_LOG_DATA0))return 2;

		logbuf[log_type].header.code[id] = val;   return 0; 
	}
	//LOG　ITEMのTITLEセット
	static int set_ItemTitle(int log_type, int id, PWCHAR pwch) { 
		//ログタイプ範囲外チェック
		if ((log_type < MKLOG_ID_TYPE_TIME) || (log_type > MKLOG_ID_TYPE_SCAT))return 1;
		//ログ項目数範囲外チェック
		if ((id < 0) || (id > MKLOG_N_LOG_ITEM_MAX + MKLOG_INDEX_LOG_DATA0))return 2;
		for (int i = 0; i < MKLOG_N_LOG_TITLE_WCH; i++) {
			WCHAR wch = *(pwch + i);
			logbuf[log_type].header.title[id][i] = wch;
			if (wch == L'\0')break;
		}
		return 0; 
	}

	//#### LOG RECORDセット関連
	static INT32 get_hot_iw(int logtype) { return logbuf[logtype].iw; }
	static int set_hot_iw(int logtype, int i) { logbuf[logtype].iw = i;  return 0; };
	static INT32 get_hot_ir(int logtype) { return logbuf[logtype].ir; }
	static int set_hot_ir(int logtype, int i) { logbuf[logtype].ir = i;  return 0; };
	/// <summary>
	/// loghotの各項目をRecordバッファに書き込み
	/// </summary>
	/// <param name="logtype"></param>
	/// <param name="id">ログレコード書き込み先index</param>
	/// <returns></returns>
	static int set_record(int logtype, UINT32 id) {

		int iw = logbuf[logtype].iw;

		auto now = std::chrono::system_clock::now();
		logbuf[logtype].records[iw].time = std::chrono::system_clock::to_time_t(now);//秒単位で切り捨て

		logbuf[logtype].records[iw].id = id;

		for (int i = 0; i < logbuf[logtype].n_item; i++) logbuf[logtype].records[iw].data[i] = loghot[logtype][i];

		if(++logbuf[logtype].iw >= MKLOG_N_LOG_RECORD)logbuf[logtype].iw = 0;

		return 0;
	};
	static int clear_record(int logtype) {
		for (int i = 0; i < MKLOG_N_LOG_RECORD; i++) logbuf[logtype].records[i].id = 0;
		logbuf[logtype].iw = logbuf[logtype].ir =0;
		return 0;
	};

	int set_log_data(int logtype, int i, INT16 data) {//APP(SCADA)からセット
		if ((i < 0) || (i > MKLOG_N_LOG_ITEM_MAX + MKLOG_INDEX_LOG_DATA0))return 1;
		loghot[logtype][i] = data;
		return 0;
	}

	static int output_logfile(int logtype);
	static int close_log(int logID);						//Log Window クローズ
	static void draw_icon(HDC hdc, int log_type);
	
	//***************************************
	//CHART
	//***************************************
	static int set_graph(int chart_WND_ID);
	static void draw_bg(int log_type);
	static  void draw_graphic(int log_type, bool be_reflesh);
	static  void draw_info(int log_type);
	static  void init_graphic(int chartID);
	static  void init_log_setting(int chartID, int update_log_source);
}; 