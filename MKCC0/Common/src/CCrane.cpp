#include "CCrane.h"


CCrane::CCrane(int _crane_id, INT16* plc_rbuf, INT16* plc_wbuf) {//SCC—p
	crane_id = _crane_id;
	pSpec = new CSpec(crane_id);
	pPlc = new CPlc(crane_id, plc_rbuf, plc_wbuf);
	pFlt = new CFaults(crane_id, plc_rbuf, plc_wbuf);

	pSpec->setup(crane_id);
	pPlc->setup(crane_id);
	pFlt->setup(crane_id);
	set_crane_type();

};

CCrane::CCrane(int _crane_id) {//OTE—p
	crane_id = _crane_id;
	pSpec = new CSpec(crane_id);
	pPlc = new CPlc(crane_id);
	pFlt = new CFaults(crane_id);

	pSpec->setup(crane_id);
	pPlc->setup(crane_id);
	pFlt->setup(crane_id);

	set_crane_type();
};

CCrane::~CCrane() {
	if (pSpec) delete pSpec;
	if (pPlc) delete pPlc;
	if (pFlt) delete pFlt;
	pSpec = nullptr;
	pPlc = nullptr;
	pFlt = nullptr;
};

void CCrane::set_crane_type() {
	st_crane_inf.crane_id = crane_id;
	switch (crane_id) {
	case CRANE_ID_H6R602:
	case CRANE_ID_HHGH29:
	case CRANE_ID_HHGQ18:
	{
		st_crane_inf.crane_type = CRANE_TYPE_ID_JC;
	}return;
	case CRANE_ID_HHFM08:
	{
		st_crane_inf.crane_type = CRANE_TYPE_ID_GC;
	}return;
	default:
		st_crane_inf.crane_type = CRANE_TYPE_ID_UNKNOWN;
		return ;
	}
}