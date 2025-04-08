#pragma once

#include "SPEC_DEF.h"
#include "COMMON_DEF.h"
#include "NET_DEF.h"
#include "PLC_DEF.h"
#include "CPanelIo.h"

using namespace std;

typedef struct _ST_PANEL_OBJ {
	//Healthy
	CValue* healthy;

	//CRANE CONTROL
	CValue* flg_pc_ctrl;
	CValue* sw_remote_mode;
	CValue* flg_panel_sim_mode;
	
	//B220-B26F
	CNotchIO* notch[MOTION_ID_MAX];

	CValue* pb_ctrl_source_on;
	CValue* pb_ctrl_source_off;
	CValue* cs3_mh_spd; 
	CValue* cs4_bh_R;
	
	CValue* pb_alarm_stop;
	CValue* pb_fault_reset;
	CValue* pb_il_bypass;
		
	CValue* pb_cab_estop;

	CValue* sw_slw_brk;
}ST_PANEL_OBJ, * LPST_PANEL_OBJ;

class CPanelBase {

private:
	static int _panel_id;
	static int _crane_id;

public:
	static CPanelBase* pPanel;
	CPanelBase(int crane_id, int panel_id);
	virtual ~CPanelBase(void);

	static ST_Panel_OBJ objs;

	HRESULT set_crane_id(int crane_id) {  _crane_id = crane_id; S_OK; }
	HRESULT set_panel_id(int panel_id) {  _panel_id = panel_id; S_OK; }
	int get_crane_id() { return _crane_id; }
	int get_crane_id() { return _panel_id; }

	HRESULT setup_panel(int crane_id);

};