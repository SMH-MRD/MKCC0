// OpePaneLib.cpp : スタティック ライブラリ用の関数を定義します。
//

#include "pch.h"
#include "framework.h"
#include "CPanelLib .H"

using namespace Gdiplus;

int CPanelBase::_crane_id;
ST_DRAWING_ITEMS CPanelBase::drawing_items;
ST_PANEL_OBJ CPanelBase::objs;
CPanelBase* CPanelBase::pPanel;

HRESULT CPanelBase::setup_drawing_base() {

	PANEL_COLOR_PALLET pallet;
	drawing_items.ppen[ID_PANEL_COLOR_BLACK]	= new Pen(pallet.color[COLOR_ID_BLACK],2.0);
	drawing_items.ppen[ID_PANEL_COLOR_GRAY]		= new Pen(pallet.color[COLOR_ID_LGRAY], 2.0);
	drawing_items.ppen[ID_PANEL_COLOR_WHITE]	= new Pen(pallet.color[COLOR_ID_WHITE], 2.0);
	drawing_items.ppen[ID_PANEL_COLOR_RED]		= new Pen(pallet.color[COLOR_ID_SRED], 2.0);
	drawing_items.ppen[ID_PANEL_COLOR_GREEN]	= new Pen(pallet.color[COLOR_ID_SGREEN], 2.0);
	drawing_items.ppen[ID_PANEL_COLOR_BLUE]		= new Pen(pallet.color[COLOR_ID_SBLUE], 2.0);
	drawing_items.ppen[ID_PANEL_COLOR_YELLOW]	= new Pen(pallet.color[COLOR_ID_SYELLOW], 2.0);
	drawing_items.ppen[ID_PANEL_COLOR_ORANGE]	= new Pen(pallet.color[COLOR_ID_SORANGE], 2.0);

	return S_OK;
}
HRESULT CPanelBase::setup_panel(int crane_id, int panel_id) {
	return S_OK;
}