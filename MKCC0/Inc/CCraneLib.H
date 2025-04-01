#pragma once

#include "SPEC_DEF.h"
#include "COMMON_DEF.h"
#include "NET_DEF.h"
#include "PLC_DEF.h"
#include "CPlcIo.h"

using namespace std;

typedef struct _ST_CRANE_OBJ {

	CNotchIO* notch[MOTION_ID_MAX];

	//B220-B26F
	CBitIO* pb_ctrl_source_on;
	CBitIO* pb_ctrl_source_off;
	CModeIO* cs3_mh_spd; 
	CModeIO* cs4_bh_R;
	
	CBitIO* pb_alarm_stop;
	CBitIO* pb_fault_reset;
	CBitIO* pb_il_bypass;

	CBitIO* sw_mlim_pre_warning;
	CBitIO* sw_mlim_warning;
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
	static INT16 dummy_if_wbuf[CC_MC_SIZE_W_WRITE];
	static INT16 dummy_if_rbuf[CC_MC_SIZE_W_READ];

public:
	CCraneBase(int crane_id,PINT16 buf_if_r,PINT16 buf_if_w) 
	{ 
		_crane_id = crane_id;
		if (buf_if_r == NULL) _buf_if_r = dummy_if_rbuf;
		else _buf_if_r = buf_if_r; 
		if (buf_if_w == NULL) _buf_if_w = dummy_if_rbuf;
		else _buf_if_w = buf_if_w;
		setup_crane(crane_id);
	};
	virtual ~CCraneBase(void) {};

	static ST_CRANE_OBJ objs;

	int get_id() { return _crane_id; }
	ST_SPEC get_spec() { return _spec; }

	HRESULT setup_crane(int crane_id);

};