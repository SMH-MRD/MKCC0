#pragma once
#include <Windows.h>
#include <string>
#include "FAULT_DEF.h"
#include "CPanelIo.h"

#define OTE_SUB_PANEL_X				5			//SUBパネル表示位置X
#define OTE_SUB_PANEL_Y				5			//SUBパネル表示位置Y
#define OTE_SUB_PANEL_W				280			//SUBパネルWINDOW幅
#define OTE_SUB_PANEL_H				260			//SUBパネルWINDOW高さ

#define PLC_IF_FLT_DISP_MAX			10

#define N_OTE_FLT_PANEL_CTRL		16

#define ID_OTE_FLT_PANEL_TIMER			63350
#define OTE_FLT_PANEL_DISP_MS			200

#define ID_OTE_FLT_PANEL_BASE			63300
#define ID_OTE_FLT_PANEL_STATIC_FLTS	0

#define ID_OTE_FLT_PANEL_RADIO_ALL		8
#define ID_OTE_FLT_PANEL_RADIO_HEAVY	9
#define ID_OTE_FLT_PANEL_RADIO_LIGHT	10
#define ID_OTE_FLT_PANEL_RADIO_IL		11
#define ID_OTE_FLT_PANEL_RADIO_REMOTE	12
#define ID_OTE_FLT_PANEL_PB_FLT_RESET	13
#define ID_OTE_FLT_PANEL_CB_IL_BYPASS	14

typedef struct _ST_FLT_PNL_OBJ {
	CStaticCtrl* faultlist;
	CRadioCtrl*  radio_fault_type;
	CPbCtrl* pb_reset;
	CCbCtrl* cb_il_bypass;
}ST_FLT_PNL_OBJ, * LPST_FLT_PNL_OBJ;

typedef struct  _ST_FLT_PNL {
	HWND hctrl[N_OTE_FLT_PANEL_CTRL] = {	
		NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
		NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL
	};
	Point pt[N_OTE_FLT_PANEL_CTRL] = {
		Point(5,5), Point(5,30), Point(5,55), Point(5,80), Point(5,185), Point(5,290),Point(0,0), Point(0,0),
		Point(0,0), Point(0,0),  Point(0,0),  Point(0,0),  Point(0,0),   Point(0,0),  Point(0,0), Point(0,0)
	};
	Size sz[N_OTE_FLT_PANEL_CTRL] = {
		Size(100,150), Size(100,150), Size(100,150), Size(100,150), Size(100,150), Size(100,150),Size(0,0), Size(0,0),
		Size(50,40),   Size(50,40),   Size(50,40),   Size(50,40),   Size(50,40),   Size(50,40),  Size(50,40),Size(50,40)
	};
	WCHAR text[N_OTE_FLT_PANEL_CTRL][PLC_IF_N_MSG_CHAR] = {
		L"受信無し", L"", L"", L"", L"", L"", L"", L"",
		L"全故障", L"重故障", L"軽故障", L"IL", L"遠隔", L"リセット", L"バイパス", L"",
	};	//表示文字列
}ST_FLT_PNL, * LPST_FLT_PNL;

class CFaultPanel
{
public:
	CFaultPanel(INT32 _crane_id, HWND _hparent_wnd, PUINT16 _pfltmap) { 
		crane_id = _crane_id; 
		hWnd = NULL;
		hWndParent = _hparent_wnd;
		pfltmap = _pfltmap;
	};
	~CFaultPanel() {};
	
	HWND hWnd;			//自身用ウィンドウハンドル
	HWND hWndParent;	//親ウィンドウハンドル
	INT32 crane_id;
	static PUINT16 pfltmap;	//故障マップポインタ
	static ST_PLCIF_FLT st_flt;
	static INT32 flt_disp_mode;
	static ST_FLT_PNL st_work;
	static ST_FLT_PNL_OBJ st_obj;

	HWND open_window();
	HRESULT close_window();
	HRESULT move_window(POINT pt);
	static HRESULT setup_obj();

	static LRESULT CALLBACK WndProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);

};

