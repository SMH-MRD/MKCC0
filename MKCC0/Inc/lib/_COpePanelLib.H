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
	static CMainPanelObj* pobjs;
	static LPST_DRAWING_BASE pdrawing_items;
	
	CPanelBase(int crane_id, int panel_id,HWND hwnd_parent) {
		setup_drawing_base();
		pobjs = new CMainPanelObj(hwnd_parent);
	}
	virtual ~CPanelBase(void) {
		close_drawing_base();
	}

	HRESULT set_crane_id(int crane_id) {  _crane_id = crane_id; return S_OK; }
	HRESULT set_panel_id(int panel_id) {  _panel_id = panel_id; return S_OK; }
	int get_crane_id() { return _crane_id; }
	int get_panel_id() { return _panel_id; }

	HRESULT setup_drawing_base();
	void	close_drawing_base();
	HRESULT setup_graphic(int crane_id, int panel_id);
	void	close_graphic();
	HRESULT setup_panel(int crane_id, int panel_id);
	void	close_panel();

};