#pragma once

#include <windows.h>
#include <string>
#include "CSHAREDMEM.h"

#define SUB_PNL_WND_X             150
#define SUB_PNL_WND_Y             500
#define SUB_PNL_WND_W             360
#define SUB_PNL_WND_H             500

#define ID_SUB_PANEL_TIMER		  60265
#define ID_SUB_PANEL_TIMER_MS	  200

class CSubPanelWindow {
private:
	LRESULT(CALLBACK* pSubWndProc)(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) = WndProc;


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
	static HWND hWnd;//生成したパネルのウィンドウハンドル
	static HWND hParentWnd;//親ウィンドウハンドル

	static LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK WndProcFlt(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK WndProcSet(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK WndProcCom(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK WndProcCam(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK WndProcStat(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	static int close();
	static void set_up(LPST_OTE_UI _pUi, LPST_OTE_CS_INF _pCsInf, LPST_OTE_CC_IF _pCcIf, LPST_OTE_ENV_INF pOteEnvInf);
	
};

#define CODE_OTE_PNL_TYPE_MAIN			  CRANE_ID_NULL
#define CODE_OTE_PNL_TYPE_MAIN_HHGH29     CARNE_ID_HHGH29

#define MAIN_PNL_WND_X             1920
#define MAIN_PNL_WND_Y             0
#define MAIN_PNL_WND_W             1920
#define MAIN_PNL_WND_H             1080

#define ID_MAIN_PANEL_TIMER		  60264
#define ID_MAIN_PANEL_TIMER_MS	  100

class CMainPanelWindow {
private:
	LRESULT(CALLBACK* pMainWndProc)(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) = WndProc;

public:
	static HWND hWnd;//生成したパネルのウィンドウハンドル
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

	static CPanelBase** ppPanelBase;
	static CPanelBase* pPanelBase;

	static LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK WndProcHHGH29(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	static int close();
	static void set_up(LPST_OTE_UI _pUi, LPST_OTE_CS_INF _pCsInf, LPST_OTE_CC_IF _pCcIf, LPST_OTE_ENV_INF pOteEnvInf) ;
};

