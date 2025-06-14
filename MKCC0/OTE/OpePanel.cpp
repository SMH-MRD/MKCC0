// OpePaneLib.cpp : スタティック ライブラリ用の関数を定義します。
//
//#include "pch.h"
#include "framework.h"
#include "COpePanel.h"
using namespace Gdiplus;

//###   GLOBAL  #################################################
GdiplusStartupInput gdiplusStartupInput;//Gdi+初期化用
ULONG_PTR			gdiplusToken;		//Gdi+初期化,終了処理用
ST_DRAWING_BASE		drawing_items;		//描画用素材登録構造体
//###############################################################

//###   CPanelBase  #################################################
int CPanelBase::_crane_id;
int CPanelBase::_panel_id;
CPanelBase*		CPanelBase::pPanel;
CMainPanelObj*	CPanelBase::pmainobjs;
CSubPanelObj* CPanelBase::psubobjs;
LPST_DRAWING_BASE CPanelBase::pdrawing_items;
LPST_OTE_UI CPanelBase::puiif;
HRESULT CPanelBase::hr_init_setting = S_FALSE;

HRESULT CPanelBase::setup_common_base(LPST_OTE_UI pui) {
	puiif = pui;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	PANEL_COLOR_PALLET pallet;
	drawing_items.ppen[ID_PANEL_COLOR_BLACK]		= new Pen(pallet.color[COLOR_ID_BLACK]		, 2.0);
	drawing_items.ppen[ID_PANEL_COLOR_DGRAY]		= new Pen(pallet.color[COLOR_ID_DGRAY]		, 2.0);
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
	drawing_items.pbrush[ID_PANEL_COLOR_DGRAY]		= new SolidBrush(pallet.color[COLOR_ID_DGRAY]);
	drawing_items.pbrush[ID_PANEL_COLOR_WHITE]		= new SolidBrush(pallet.color[COLOR_ID_WHITE]);
	drawing_items.pbrush[ID_PANEL_COLOR_RED]		= new SolidBrush(pallet.color[COLOR_ID_SRED]);
	drawing_items.pbrush[ID_PANEL_COLOR_BLUE]		= new SolidBrush(pallet.color[COLOR_ID_SBLUE]);
	drawing_items.pbrush[ID_PANEL_COLOR_GREEN]		= new SolidBrush(pallet.color[COLOR_ID_SGREEN]);
	drawing_items.pbrush[ID_PANEL_COLOR_YELLOW]		= new SolidBrush(pallet.color[COLOR_ID_SYELLOW]);
	drawing_items.pbrush[ID_PANEL_COLOR_ORANGE]		= new SolidBrush(pallet.color[COLOR_ID_SORANGE]);
	drawing_items.pbrush[ID_PANEL_COLOR_MAZENDA]	= new SolidBrush(pallet.color[COLOR_ID_SMAZENDA]);
	drawing_items.pbrush[ID_PANEL_COLOR_BROWN]		= new SolidBrush(pallet.color[COLOR_ID_SBROWN]);
	drawing_items.pbrush[ID_PANEL_COLOR_LGRAY]		= new SolidBrush(pallet.color[COLOR_ID_LGRAY]);

	for (int i = ID_PANEL_COLOR_BROWN + 1; i < N_PANEL_COLOR_BRUSH; i++) {
		drawing_items.pbrush[i] = NULL;
	}

	drawing_items.pfamily[ID_PANEL_FONT_6] = new FontFamily(L"Arial");
	drawing_items.pfont[ID_PANEL_FONT_6 ] = new Font(drawing_items.pfamily[ID_PANEL_FONT_6], 6, FontStyleBold, UnitPixel);
	
	drawing_items.pfamily[ID_PANEL_FONT_8] = new FontFamily(L"Arial");
	drawing_items.pfont[ID_PANEL_FONT_8] = new Font(drawing_items.pfamily[ID_PANEL_FONT_8], 8, FontStyleBold, UnitPixel);
	
	drawing_items.pfamily[ID_PANEL_FONT_10] = new FontFamily(L"Arial");
	drawing_items.pfont[ID_PANEL_FONT_10] = new Font(drawing_items.pfamily[ID_PANEL_FONT_10], 10, FontStyleRegular, UnitPixel);

	drawing_items.pfamily[ID_PANEL_FONT_12] = new FontFamily(L"Arial");
	drawing_items.pfont[ID_PANEL_FONT_12] = new Font(drawing_items.pfamily[ID_PANEL_FONT_12], 12, FontStyleRegular, UnitPixel);

	drawing_items.pfamily[ID_PANEL_FONT_14] = new FontFamily(L"Arial");
	drawing_items.pfont[ID_PANEL_FONT_14] = new Font(drawing_items.pfamily[ID_PANEL_FONT_14], 14, FontStyleRegular, UnitPixel);

	drawing_items.pfamily[ID_PANEL_FONT_20] = new FontFamily(L"Arial");
	drawing_items.pfont[ID_PANEL_FONT_20] = new Font(drawing_items.pfamily[ID_PANEL_FONT_20], 20, FontStyleRegular, UnitPixel);

	drawing_items.pfamily[ID_PANEL_FONT_24] = new FontFamily(L"Arial");
	drawing_items.pfont[ID_PANEL_FONT_24] = new Font(drawing_items.pfamily[ID_PANEL_FONT_24], 24, FontStyleRegular, UnitPixel);

	drawing_items.pfamily[ID_PANEL_FONT_32] = new FontFamily(L"Arial");
	drawing_items.pfont[ID_PANEL_FONT_32] = new Font(drawing_items.pfamily[ID_PANEL_FONT_32], 32, FontStyleRegular, UnitPixel);

	drawing_items.pfamily[ID_PANEL_FONT_48] = new FontFamily(L"Arial");
	drawing_items.pfont[ID_PANEL_FONT_48] = new Font(drawing_items.pfamily[ID_PANEL_FONT_48], 48, FontStyleRegular, UnitPixel);

	for (int i = ID_PANEL_FONT_48 + 1; i < N_PANEL_FONT; i++) {
		drawing_items.pfont[i] = NULL;
		drawing_items.pfamily[i] = NULL;
	}

	drawing_items.pstrformat[ID_STR_FORMAT_CENTER] = new StringFormat();
	drawing_items.pstrformat[ID_STR_FORMAT_CENTER]->SetAlignment(StringAlignmentCenter);
	drawing_items.pstrformat[ID_STR_FORMAT_CENTER]->SetLineAlignment(StringAlignmentCenter);

	drawing_items.pstrformat[ID_STR_FORMAT_LEFT_CENTER] = new StringFormat();
	drawing_items.pstrformat[ID_STR_FORMAT_LEFT_CENTER]->SetAlignment(StringAlignmentNear);
	drawing_items.pstrformat[ID_STR_FORMAT_LEFT_CENTER]->SetLineAlignment(StringAlignmentCenter);

	drawing_items.pstrformat[ID_STR_FORMAT_LEFT_TOP] = new StringFormat();
	drawing_items.pstrformat[ID_STR_FORMAT_LEFT_TOP]->SetAlignment(StringAlignmentNear);
	drawing_items.pstrformat[ID_STR_FORMAT_LEFT_TOP]->SetLineAlignment(StringAlignmentNear);

	drawing_items.pstrformat[ID_STR_FORMAT_LEFT_BOTTOM] = new StringFormat();
	drawing_items.pstrformat[ID_STR_FORMAT_LEFT_BOTTOM]->SetAlignment(StringAlignmentNear);
	drawing_items.pstrformat[ID_STR_FORMAT_LEFT_BOTTOM]->SetLineAlignment(StringAlignmentCenter);

	drawing_items.pstrformat[ID_STR_FORMAT_RIGHT_CENTER] = new StringFormat();
	drawing_items.pstrformat[ID_STR_FORMAT_RIGHT_CENTER]->SetAlignment(StringAlignmentFar);
	drawing_items.pstrformat[ID_STR_FORMAT_RIGHT_CENTER]->SetLineAlignment(StringAlignmentCenter);

	drawing_items.pstrformat[ID_STR_FORMAT_RIGHT_TOP] = new StringFormat();
	drawing_items.pstrformat[ID_STR_FORMAT_RIGHT_TOP]->SetAlignment(StringAlignmentFar);
	drawing_items.pstrformat[ID_STR_FORMAT_RIGHT_TOP]->SetLineAlignment(StringAlignmentNear);

	drawing_items.pstrformat[ID_STR_FORMAT_RIGHT_BOTTOM] = new StringFormat();
	drawing_items.pstrformat[ID_STR_FORMAT_RIGHT_BOTTOM]->SetAlignment(StringAlignmentFar);
	drawing_items.pstrformat[ID_STR_FORMAT_RIGHT_BOTTOM]->SetLineAlignment(StringAlignmentCenter);


	pdrawing_items = &drawing_items;

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

	for (int i = 0; i < N_STRING_FORMAT; i++) {
		DeleteObject(drawing_items.pstrformat[i]);
	}

	GdiplusShutdown(gdiplusToken);
	return;
}

HRESULT CPanelBase::setup_panel() {
	return S_OK;
}
void CPanelBase::close_panel() {
	return;
}

HRESULT CPanelBase::setup_graphic() {
	return S_OK;
}
void	CPanelBase::close_graphic() {
	return;
}

