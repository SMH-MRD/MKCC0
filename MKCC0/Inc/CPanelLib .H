#pragma once

#include "SPEC_DEF.h"
#include "COMMON_DEF.h"
#include "PLC_DEF.h"
#include "CPanelIo.h"

using namespace std;

#define ID_PANEL_COLOR_BLACK	0
#define ID_PANEL_COLOR_GRAY		1
#define ID_PANEL_COLOR_WHITE	2
#define ID_PANEL_COLOR_RED		3
#define ID_PANEL_COLOR_GREEN	4
#define ID_PANEL_COLOR_BLUE		5
#define ID_PANEL_COLOR_YELLOW	6
#define ID_PANEL_COLOR_ORANGE	7

#define ID_PANEL_FONT_8			0
#define ID_PANEL_FONT_10		1
#define ID_PANEL_FONT_12		2
#define ID_PANEL_FONT_14		3
#define ID_PANEL_FONT_18		4
#define ID_PANEL_FONT_24		5
#define ID_PANEL_FONT_36		6
#define ID_PANEL_FONT_48		7

#define N_PANEL_COLOR_PEN		8
#define N_PANEL_COLOR_BRUSH		8
#define N_PANEL_COLOR_FONT		8

typedef struct _ST_DRAWING_ITEMS {
	Pen*		ppen[N_PANEL_COLOR_PEN];
	SolidBrush* pbrush[N_PANEL_COLOR_BRUSH];
	HFONT		hfont[N_PANEL_COLOR_FONT];
}ST_DRAWING_ITEMS,LPST_DRAWING_ITEMS;

typedef struct _ST_PANEL_OBJ {
	//Healthy
	CPblVal* healthy;

	//CRANONTROL
	CPblVal* flg_pc_ctrl;
	CPblVal* sw_remote_mode;
	CPblVal* flg_panel_sim_mode;

	CPblVal* pb_ctrl_source_on;
	CPblVal* pb_ctrl_source_off;
	CPblVal* cs3_mh_spd;
	CPblVal* cs4_bh_R;

	CPblVal* pb_alarm_stop;
	CPblVal* pb_fault_reset;
	CPblVal* pb_il_bypass;

	CPblVal* pb_cab_estop;

	CPblVal* sw_slw_brk;
}ST_PANEL_OBJ, * LPST_PANEL_OBJ;

class CPanelBase {

private:
	static int _panel_id;
	static int _crane_id;
	static ST_DRAWING_ITEMS drawing_items;

public:
	static CPanelBase* pPanel;
	CPanelBase(int crane_id, int panel_id);
	virtual ~CPanelBase(void);

	static ST_PANEL_OBJ objs;

	HRESULT set_crane_id(int crane_id) {  _crane_id = crane_id; S_OK; }
	HRESULT set_panel_id(int panel_id) {  _panel_id = panel_id; S_OK; }
	int get_crane_id() { return _crane_id; }
	int get_panel_id() { return _panel_id; }

	HRESULT setup_drawing_base();
	HRESULT setup_panel(int crane_id, int panel_id);

};