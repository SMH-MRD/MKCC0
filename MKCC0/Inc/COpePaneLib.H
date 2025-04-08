#pragma once

#include "SPEC_DEF.h"
#include "COMMON_DEF.h"
#include "NET_DEF.h"
#include "PLC_DEF.h"
#include "CPanelIo.h"

using namespace std;

typedef struct _ST_CRANE_OBJ {
	//Healthy
	CWordIO* healthy;

	//CRANE CONTROL
	CBitIO* flg_pc_ctrl;
	CBitIO* sw_remote_mode;
	CBitIO* flg_panel_sim_mode;
	
	//B220-B26F
	CNotchIO* notch[MOTION_ID_MAX];

	CBitIO* pb_ctrl_source_on;
	CBitIO* pb_ctrl_source_off;
	CModeIO* cs3_mh_spd; 
	CModeIO* cs4_bh_R;
	
	CBitIO* pb_alarm_stop;
	CBitIO* pb_fault_reset;
	CBitIO* pb_il_bypass;

	CBitIO* sw_mlim_50t_lower;
	CBitIO* sw_mlim_x3_enable;
	CBitIO* sw_mlim_90_warning;
	CBitIO* sw_mlim_100_warning;
	CBitIO* sw_mlim_error;
	CBitIO* sw_mlim_hhoist_enable;
	CBitIO* sw_wind_over16;
	CBitIO* pb_cab_estop;

	CBitIO* sw_slw_brk;
}ST_CRANE_OBJ, * LPST_CRANE_OBJ;

class CCraneBase {

private:
	static int _crane_id;
	static ST_SPEC _spec;
	static PINT16 _buf_if_r, _buf_if_w;

public:
	static INT16 dummy_if_wbuf[CC_MC_SIZE_W_WRITE];
	static INT16 dummy_if_rbuf[CC_MC_SIZE_W_READ];
	static CCraneBase* pCrane;
	CCraneBase(int crane_id);
	CCraneBase(int crane_id, PINT16 buf_if_r, PINT16 buf_if_w);
	virtual ~CCraneBase(void);

	static ST_CRANE_OBJ objs;

	int get_id() { return _crane_id; }
	ST_SPEC get_spec() { return _spec; }

	HRESULT setup_crane(int crane_id);

};