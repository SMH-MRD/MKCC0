#pragma once

#include "SPEC_DEF.h"
#include "COMMON_DEF.h"
#include "PLC_DEF.h"
#include "CPanelObj.H"
#include "CSHAREDMEM.H"

class CPanelBase {

private:
	static int _panel_id;
	static int _crane_id;

public:
	static CPanelBase* pPanel;
	static CMainPanelObj* pmainobjs;
	static CSubPanelObj* psubobjs;
	static LPST_DRAWING_BASE pdrawing_items;
	static LPST_OTE_UI puiif;
	static HRESULT hr_init_setting;
	
	CPanelBase(int crane_id, int panel_id,HWND hwnd_parent) {
		_panel_id=panel_id;
		_crane_id = crane_id;
		pmainobjs = new CMainPanelObj(hwnd_parent,crane_id);
		psubobjs = new CSubPanelObj(hwnd_parent, crane_id);
	}
	virtual ~CPanelBase(void) {
		close_drawing_base();
	}

	HRESULT set_crane_id(int crane_id) {  _crane_id = crane_id; return S_OK; }
	HRESULT set_panel_id(int panel_id) {  _panel_id = panel_id; return S_OK; }
	int get_crane_id() { return _crane_id; }
	int get_panel_id() { return _panel_id; }

	static HRESULT setup_common_base(LPST_OTE_UI pui);
	void	close_drawing_base();
	HRESULT setup_graphic();
	void	close_graphic();
	HRESULT setup_panel();
	void	close_panel();
};

