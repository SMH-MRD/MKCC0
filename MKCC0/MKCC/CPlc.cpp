#include "CPlc.h"
#include "COMMON_DEF.h"


ST_PLC_IO_RDEF CPlc::plc_io_rdef;
ST_PLC_IO_WDEF CPlc::plc_io_wdef;

ST_PLC_IO_RDEF plc_io_rdef0 = { 
	{0,0,0,0}/*estop*/,	{0,0,0,0}/*csource*/,{0,0,0,0}/*faults*/
};
ST_PLC_IO_WDEF plc_io_wdef0 = { 
	{0,0,0,0 }/*estop*/, { 0,0,0,0 }/*csource*/, { 0,0,0,0 }/*faults*/
};

int CPlc::setup(int crane_id) {
	switch (crane_id) {
	case CRANE_ID_H6R602: {
		plc_io_rdef = plc_io_rdef0;
		plc_io_wdef = plc_io_wdef0;
	}break;
	case CARNE_ID_HHGH29: {
		plc_io_rdef = plc_io_rdef0;
		plc_io_wdef = plc_io_wdef0;
	}break;
	case CARNE_ID_HHGQ18:{
		plc_io_rdef = plc_io_rdef0;
		plc_io_wdef = plc_io_wdef0;
	}break;
	case CARNE_ID_HHFR22:{
		plc_io_rdef = plc_io_rdef0;
		plc_io_wdef = plc_io_wdef0;
	}break;
	default: {
		plc_io_rdef = plc_io_rdef0;
		plc_io_wdef = plc_io_wdef0;
	}break;
	}
	return 0;
}
