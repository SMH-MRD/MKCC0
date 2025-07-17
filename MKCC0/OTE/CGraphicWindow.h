#pragma once

#include <windows.h>
#include <string>
#include "CSHAREDMEM.h"
#include "COpePanel.H"

#define CODE_OTE_GWIN_TYPE_MAIN				CRANE_ID_NULL
#define CODE_OTE_GWIN_TYPE_MAIN_HHGH29		CARNE_ID_HHGH29

#define GMAIN_PNL_WND_X             130
#define GMAIN_PNL_WND_Y             40
#define GMAIN_PNL_WND_W             1635
#define GMAIN_PNL_WND_H             1000

#define ID_GMAIN_TIMER		  60899
#define ID_GMAIN_TIMER_MS	  100

class CGraphicWindow
{
private:

public:
	static HWND hGWnd;//生成したパネルのウィンドウハンドル
	static HWND hParentWnd;//親ウィンドウハンドル

	CGraphicWindow(HINSTANCE hInstance, HWND hParent, int _crane_id, CPanelBase* _pPanelBase);
	~CGraphicWindow();

	static int crane_id;
	
	static LPST_OTE_UI pUi;
	static LPST_OTE_CS_INF pCsInf;
	static LPST_OTE_CC_IF pCcIf;
	static LPST_OTE_ENV_INF pOteEnvInf;

	static CPanelBase* pPanelBase;

	static LRESULT CALLBACK GWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK GWndProcHHGH29(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	static int close();
	static void set_up(LPST_OTE_UI _pUi, LPST_OTE_CS_INF _pCsInf, LPST_OTE_CC_IF _pCcIf, LPST_OTE_ENV_INF pOteEnvInf, int _crane_id);
};



