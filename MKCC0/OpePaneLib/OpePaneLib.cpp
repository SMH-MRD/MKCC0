// OpePaneLib.cpp : スタティック ライブラリ用の関数を定義します。
//
#include "pch.h"
#include "framework.h"
#include "COpePanelLib.h"
#include "CSHAREDMEM.H"

using namespace Gdiplus;

//###   GLOBAL  #################################################
GdiplusStartupInput gdiplusStartupInput;//Gdi+初期化用
ULONG_PTR			gdiplusToken;		//Gdi+初期化,終了処理用
ST_DRAWING_BASE		drawing_items;		//描画用素材登録構造体
//###############################################################

int CPanelBase::_crane_id;
int CPanelBase::_panel_id;
CPanelBase*		CPanelBase::pPanel;
CMainPanelObj* CPanelBase::pobjs;

HRESULT CPanelBase::setup_drawing_base() {
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	PANEL_COLOR_PALLET pallet;
	drawing_items.ppen[ID_PANEL_COLOR_BLACK]		= new Pen(pallet.color[COLOR_ID_BLACK]		, 2.0);
	drawing_items.ppen[ID_PANEL_COLOR_GRAY]			= new Pen(pallet.color[COLOR_ID_LGRAY]		, 2.0);
	drawing_items.ppen[ID_PANEL_COLOR_WHITE]		= new Pen(pallet.color[COLOR_ID_WHITE]		, 2.0);
	drawing_items.ppen[ID_PANEL_COLOR_RED]			= new Pen(pallet.color[COLOR_ID_SRED]		, 2.0);
	drawing_items.ppen[ID_PANEL_COLOR_GREEN]		= new Pen(pallet.color[COLOR_ID_SGREEN]		, 2.0);
	drawing_items.ppen[ID_PANEL_COLOR_BLUE]			= new Pen(pallet.color[COLOR_ID_SBLUE]		, 2.0);
	drawing_items.ppen[ID_PANEL_COLOR_YELLOW]		= new Pen(pallet.color[COLOR_ID_SYELLOW]	, 2.0);
	drawing_items.ppen[ID_PANEL_COLOR_ORANGE]		= new Pen(pallet.color[COLOR_ID_SORANGE]	, 2.0);
	drawing_items.ppen[ID_PANEL_COLOR_MAZENDA]		= new Pen(pallet.color[COLOR_ID_SMAZENDA]	, 2.0);
	drawing_items.ppen[ID_PANEL_COLOR_BROWN]		= new Pen(pallet.color[COLOR_ID_SBROWN]		, 2.0);

	for (int i = ID_PANEL_COLOR_BROWN +1 ; i < N_PANEL_COLOR_PEN; i++) {
		drawing_items.ppen[i]=NULL;
	}

	drawing_items.pbrush[ID_PANEL_COLOR_BLACK]		= new SolidBrush(pallet.color[COLOR_ID_BLACK]);
	drawing_items.pbrush[ID_PANEL_COLOR_WHITE]		= new SolidBrush(pallet.color[COLOR_ID_LGRAY]);
	drawing_items.pbrush[ID_PANEL_COLOR_GRAY]		= new SolidBrush(pallet.color[COLOR_ID_WHITE]);
	drawing_items.pbrush[ID_PANEL_COLOR_RED]		= new SolidBrush(pallet.color[COLOR_ID_SRED]);
	drawing_items.pbrush[ID_PANEL_COLOR_BLUE]		= new SolidBrush(pallet.color[COLOR_ID_SGREEN]);
	drawing_items.pbrush[ID_PANEL_COLOR_GREEN]		= new SolidBrush(pallet.color[COLOR_ID_SBLUE]);
	drawing_items.pbrush[ID_PANEL_COLOR_YELLOW]		= new SolidBrush(pallet.color[COLOR_ID_SYELLOW]);
	drawing_items.pbrush[ID_PANEL_COLOR_ORANGE]		= new SolidBrush(pallet.color[COLOR_ID_SORANGE]);
	drawing_items.pbrush[ID_PANEL_COLOR_MAZENDA]	= new SolidBrush(pallet.color[COLOR_ID_SMAZENDA]);
	drawing_items.pbrush[ID_PANEL_COLOR_BROWN]		= new SolidBrush(pallet.color[COLOR_ID_SBROWN]);

	for (int i = ID_PANEL_COLOR_BROWN + 1; i < N_PANEL_COLOR_BRUSH; i++) {
		drawing_items.pbrush[i] = NULL;
	}

	drawing_items.pfamily[ID_PANEL_FONT_6] = new FontFamily(L"Arial");
	drawing_items.pfont[ID_PANEL_FONT_6 ] = new Font(drawing_items.pfamily[ID_PANEL_FONT_6], 6, FontStyleBold, UnitPixel);
	
	drawing_items.pfamily[ID_PANEL_FONT_8] = new FontFamily(L"Arial");
	drawing_items.pfont[ID_PANEL_FONT_8] = new Font(drawing_items.pfamily[ID_PANEL_FONT_8], 8, FontStyleBold, UnitPixel);
	
	drawing_items.pfamily[ID_PANEL_FONT_10] = new FontFamily(L"Arial");
	drawing_items.pfont[ID_PANEL_FONT_10] = new Font(drawing_items.pfamily[ID_PANEL_FONT_10], 10, FontStyleBold, UnitPixel);

	drawing_items.pfamily[ID_PANEL_FONT_12] = new FontFamily(L"Arial");
	drawing_items.pfont[ID_PANEL_FONT_12] = new Font(drawing_items.pfamily[ID_PANEL_FONT_12], 12, FontStyleBold, UnitPixel);

	drawing_items.pfamily[ID_PANEL_FONT_14] = new FontFamily(L"Arial");
	drawing_items.pfont[ID_PANEL_FONT_14] = new Font(drawing_items.pfamily[ID_PANEL_FONT_14], 14, FontStyleBold, UnitPixel);

	drawing_items.pfamily[ID_PANEL_FONT_20] = new FontFamily(L"Arial");
	drawing_items.pfont[ID_PANEL_FONT_20] = new Font(drawing_items.pfamily[ID_PANEL_FONT_20], 20, FontStyleBold, UnitPixel);

	drawing_items.pfamily[ID_PANEL_FONT_24] = new FontFamily(L"Arial");
	drawing_items.pfont[ID_PANEL_FONT_24] = new Font(drawing_items.pfamily[ID_PANEL_FONT_24], 24, FontStyleBold, UnitPixel);

	drawing_items.pfamily[ID_PANEL_FONT_32] = new FontFamily(L"Arial");
	drawing_items.pfont[ID_PANEL_FONT_32] = new Font(drawing_items.pfamily[ID_PANEL_FONT_32], 32, FontStyleBold, UnitPixel);

	drawing_items.pfamily[ID_PANEL_FONT_48] = new FontFamily(L"Arial");
	drawing_items.pfont[ID_PANEL_FONT_48] = new Font(drawing_items.pfamily[ID_PANEL_FONT_48], 48, FontStyleBold, UnitPixel);

	for (int i = ID_PANEL_FONT_48 + 1; i < N_PANEL_FONT; i++) {
		drawing_items.pfont[i] = NULL;
		drawing_items.pfamily[i] = NULL;
	}

	return S_OK;
}

void CPanelBase::close_drawing_base() {
	for (int i = 0; i < N_PANEL_COLOR_PEN; i++) {
		delete drawing_items.ppen[i];
	}
	for (int i = 0; i < N_PANEL_COLOR_BRUSH; i++) {
		delete drawing_items.pbrush[i];
	}
	for (int i = 0; i < N_PANEL_FONT; i++) {
		DeleteObject(drawing_items.pfont[i]);
	}

	GdiplusShutdown(gdiplusToken);
	return;
}

HRESULT CPanelBase::setup_panel(int crane_id, int panel_id) {
	set_crane_id(crane_id);
	set_panel_id(panel_id);
	pPanel = this;
	return S_OK;
}
void CPanelBase::close_panel() {
	return;
}

HRESULT CPanelBase::setup_graphic(int crane_id, int panel_id) {
	return S_OK;
}
void	CPanelBase::close_graphic() {
	return;
}