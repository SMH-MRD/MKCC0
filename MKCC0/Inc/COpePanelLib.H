#pragma once

#include "SPEC_DEF.h"
#include "COMMON_DEF.h"
#include "PLC_DEF.h"
#include "CPanelIo.h"
#include "CPanelGr.h"

using namespace std;

typedef struct _ST_PANEL_OBJ {
	CPblCtrl* estop;
}ST_PANEL_OBJ, * LPST_PANEL_OBJ;

typedef struct _ST_GRAPHIC_OBJ {
	//DC
	CMemDC* dc_mem0;
	CMemDC* dc_mem_back;
	CMemDC* dc_mem_info;
	CMemDC* dc_mem_graphic;

	//Img
	CImgObj* img_backgraund;
	CImgObj* img_boom_top;
	CImgObj* img_boom_side;
	CImgObj* img_boom_top_auto;
	CImgObj* img_boom_side_auto;
	CImgObj* img_slw_body;
	CImgObj* img_hook_top;
	CImgObj* img_hook_side;

}ST_GRAPHIC_OBJ, * LPST_GRAPHIC_OBJ;

class CPanelBase {

private:
	static int _panel_id;
	static int _crane_id;

public:
	static CPanelBase* pPanel;
	static ST_DRAWING_BASE drawing_items;
	static ST_GRAPHIC_OBJ* pGraphics;

	CPanelBase(int crane_id, int panel_id);
	virtual ~CPanelBase(void);

	static ST_PANEL_OBJ objs;

	HRESULT set_crane_id(int crane_id) {  _crane_id = crane_id; S_OK; }
	HRESULT set_panel_id(int panel_id) {  _panel_id = panel_id; S_OK; }
	int get_crane_id() { return _crane_id; }
	int get_panel_id() { return _panel_id; }

	HRESULT setup_drawing_base();
	void	close_drawing_base();
	HRESULT setup_graphic(int crane_id, int panel_id);
	void	close_graphic();
	HRESULT setup_panel(int crane_id, int panel_id);
	void	close_panel();

};