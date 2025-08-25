#pragma once

#include <windows.h>
#include <string>
#include "CSHAREDMEM.h"
#include "SmemMain.H"
#include "SmemOte.H"

#include "CGraphicWindow.h"

#define SUB_PNL_WND_X             125
#define SUB_PNL_WND_Y             540
#define SUB_PNL_WND_W             640
#define SUB_PNL_WND_H             500

#define ID_SUB_PANEL_TIMER		  60265
#define ID_SUB_PANEL_TIMER_MS	  200

class CSubPanelWindow {

public:
	CSubPanelWindow(HINSTANCE hInstance, HWND hParent, int _crane_id, int wnd_code, CPanelBase* pPanelBase);
	~CSubPanelWindow();

	static int crane_id;
	static int wnd_code;
	
	static LPST_OTE_UI pUi;
	static LPST_OTE_CS_INF pCsInf;
	static LPST_OTE_CC_IF pCcIf;
	static LPST_OTE_ENV_INF pOteEnvInf;

	static CPanelBase* pPanelBase;
	static HWND hPnlWnd;//生成したパネルのウィンドウハンドル
	static HWND hParentWnd;//親ウィンドウハンドル

	static PINT16 pflt_plc;

	static LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK WndProcFlt(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK WndProcSet(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK WndProcCom(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK WndProcCam(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK WndProcStat(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	static void SetFltToListView(HWND hlv,int code,int i);
	static void ClearFltListView(HWND hlv, bool is_init,int i);
	static void DeleteFltListItem(HWND hlv,int i);
	static void OnPaintFlt(HDC hdc,HWND hwnd);
	

	static int close();
	static void set_up(LPST_OTE_UI _pUi, LPST_OTE_CS_INF _pCsInf, LPST_OTE_CC_IF _pCcIf, LPST_OTE_ENV_INF pOteEnvInf,int _crane_id);
	
};

#define CODE_OTE_PNL_TYPE_MAIN			  CRANE_ID_NULL
#define CODE_OTE_PNL_TYPE_MAIN_HHGH29     CARNE_ID_HHGH29

#define MAIN_PNL_WND_X             1920
#define MAIN_PNL_WND_Y             0
#define MAIN_PNL_WND_W             1920
#define MAIN_PNL_WND_H             1080

#define ID_MAIN_PANEL_TIMER		  60264
#define ID_MAIN_PANEL_TIMER_MS	  100

/// <summary>
/// MainWindowのクレーンごとのコールバック関数を定義するクラス
/// MainWindowは、Scadaのメインパネルウィンドウであり、クレーンごとに異なる動作をするためのクラスです。
/// </summary>
class CMainPanelWindow {
private:
	LRESULT(CALLBACK* pMainWndProc)(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) = WndProc;

public:
	static HWND hPnlWnd;//生成したパネルのウィンドウハンドル
	static HWND hParentWnd;//親ウィンドウハンドル

	CMainPanelWindow(HINSTANCE hInstance, HWND hParent, int _crane_id, int wnd_code, CPanelBase** _ppPanelBase);
	~CMainPanelWindow();

	static int crane_id;
	static int wnd_code;

	static LPST_OTE_UI pUi;
	static LPST_OTE_CS_INF pCsInf;
	static LPST_OTE_CC_IF pCcIf;
	static LPST_OTE_ENV_INF pOteEnvInf;

	static CSubPanelWindow* pSubPanelWnd;
	static CGraphicWindow* pGWnd;

	static CPanelBase** ppPanelBase;
	static CPanelBase* pPanelBase;

	static LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK WndProcHHGH29(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	static int close();
	static void set_up(LPST_OTE_UI _pUi, LPST_OTE_CS_INF _pCsInf, LPST_OTE_CC_IF _pCcIf, LPST_OTE_ENV_INF pOteEnvInf) ;
};

