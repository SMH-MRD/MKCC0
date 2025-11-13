#pragma once

#include <windows.h>
#include <string>
#include "COpePanel.H"

#define CODE_OTE_GWIN_TYPE_MAIN				CRANE_ID_NULL
#define CODE_OTE_GWIN_TYPE_MAIN_HHGH29		CARNE_ID_HHGH29

#define GMAIN_PNL_WND_X             770
#define GMAIN_PNL_WND_Y             40
#define GMAIN_PNL_WND_W             1000
#define GMAIN_PNL_WND_H             1000

#define GMAIN_PNL_ORG_X             500//グラフィッククレーン原点
#define GMAIN_PNL_ORG_Y             500//グラフィッククレーン原点
#define GMAIN_PNL_BK_ORG_X          50 //グラフィックク背景IMG原点
#define GMAIN_PNL_BK_ORG_Y          0 //グラフィックク背景IMG原点
#define GMAIN_PNL_PIX2M				0.2//グラフィックmm/PIXEL レールスパン10m/50PIXEL
#define GMAIN_PNL_PIX2MM			200//グラフィックmm/PIXEL レールスパン10m/50PIXEL



#define GSUB_PNL_WND_X				125
#define GSUB_PNL_WND_Y				40
#define GSUB_PNL_WND_W				640
#define GSUB_PNL_WND_H				510

#define GSUB_PNL_ORG_X				138
#define GSUB_PNL_ORG_Y				500
#define GSUB_PNL_PIX2M				0.234756//グラフィックm/PIXEL(Hp35.8/164)
#define GSUB_PNL_PIX2MM				234//グラフィックm/PIXEL(Hp35.8/164)

#define ID_GMAIN_TIMER				60898
#define ID_GMAIN_TIMER_MS			100
#define ID_GSUB_TIMER				60899
#define ID_GSUB_TIMER_MS			100

class CGraphicWindow
{
private:

public:
	HINSTANCE hInstance;//インスタンスハンドル
	
	static HWND hGWnd	;//生成したパネルのウィンドウハンドル
	static HWND hGSubWnd;//生成したパネルのウィンドウハンドル
	static HWND hParentWnd;//親ウィンドウハンドル

	CGraphicWindow(HINSTANCE hInstance, HWND hParent, int _crane_id, CPanelBase* _pPanelBase);
	~CGraphicWindow();

	static int crane_id;

	static LPST_OTE_UI pUi;
	static LPST_OTE_CS_INF pCsInf;
	static LPST_OTE_CC_IF pCcIf;
	static LPST_OTE_ENV_INF pOteEnvInf;

	static CPanelBase* pPanelBase;
	
	static Gdiplus::Point mouse_pos_main;	//マウス位置
	static Gdiplus::Point org_main;			//クレーン原点位置

	static void OnPaint(HDC hdc, HWND hWnd);
	static LRESULT CALLBACK GWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK GWndProcHHGH29(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	static Gdiplus::Point mouse_pos_sub;
	static Gdiplus::Point bk_pickup_pos;
	static void OnPaintSub(HDC hdc, HWND hWnd);
	static LRESULT CALLBACK GSubWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK GSubWndProcHHGH29(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	static int close();
	static void set_up(LPST_OTE_UI _pUi, LPST_OTE_CS_INF _pCsInf, LPST_OTE_CC_IF _pCcIf, LPST_OTE_ENV_INF pOteEnvInf, int _crane_id);
		
};