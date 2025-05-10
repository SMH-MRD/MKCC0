#pragma once
#include "CSpec.h"
#include "CPlc.h"
#include "CFaults.h"

class CCrane
{
private:
	int crane_id;
public:
	CCrane(int _crane_id,INT16* plc_rbuf,INT16* plc_wbuf) {
		crane_id = _crane_id;
		pSpec = new CSpec(crane_id);
		pPlc = new CPlc(crane_id, plc_rbuf, plc_wbuf);
		pFlt = new CFaults(crane_id, plc_rbuf, plc_wbuf);
	};
	virtual ~CCrane() {
		if (pSpec) delete pSpec;
		if (pPlc) delete pPlc;
		if (pFlt) delete pFlt;
		pSpec = nullptr;
		pPlc = nullptr;
		pFlt = nullptr;
	};

	CSpec* pSpec;
	CPlc* pPlc;
	CFaults* pFlt;

	int get_id() { return crane_id; };

	LPST_PLC_IO_RIF get_plc_rif() {
		if (pPlc != NULL) return &(pPlc->plc_io_rif); 
		return NULL;
	}

	LPST_PLC_IO_WIF get_plc_wif() {
		if (pPlc != NULL) return &(pPlc->plc_io_wif);
		return NULL;
	}

	LPST_FAULT_LIST get_flt_list() {
		if (pFlt != NULL) return &(pFlt->flt_list);
		return NULL;
	}
	LPST_STRUCTURE	 get_st_struct(){if(pSpec != NULL)return &pSpec->st_struct; return NULL;}
	LPST_AXIS_ITEMS	 get_base_mh() { if (pSpec != NULL)return &pSpec->base_mh; return NULL; }
	LPST_AUTO_SPEC	 get_auto_mh() { if (pSpec != NULL)return &pSpec->auto_mh; return NULL; }
	LPST_REMOTE_SPEC get_rmt_mh() { if (pSpec != NULL)return &pSpec->rmt_mh; return NULL;	}
	LPST_AXIS_ITEMS	 get_base_bh() { if (pSpec != NULL)return &pSpec->base_bh; return NULL; }
	LPST_AUTO_SPEC	 get_auto_bh() { if (pSpec != NULL)return &pSpec->auto_bh; return NULL; }
	LPST_REMOTE_SPEC get_rmt_bh() { if (pSpec != NULL)return &pSpec->rmt_bh; return NULL;	}
	LPST_AXIS_ITEMS	 get_base_sl() { if (pSpec != NULL)return &pSpec->base_sl; return NULL; }
	LPST_AUTO_SPEC	 get_auto_sl() { if (pSpec != NULL)return &pSpec->auto_sl; return NULL; }
	LPST_REMOTE_SPEC get_rmt_sl() { if (pSpec != NULL)return &pSpec->rmt_sl; return NULL;	}
	LPST_AXIS_ITEMS	 get_base_gt() { if (pSpec != NULL)return &pSpec->base_gt; return NULL; }
	LPST_AUTO_SPEC	 get_auto_gt() { if (pSpec != NULL)return &pSpec->auto_gt; return NULL; }
	LPST_REMOTE_SPEC get_rmt_gt() { if (pSpec != NULL)return &pSpec->rmt_gt; return NULL;	}
	LPST_AXIS_ITEMS	 get_base_ah() { if (pSpec != NULL)return &pSpec->base_ah; return NULL; }
	LPST_AUTO_SPEC	 get_auto_ah() { if (pSpec != NULL)return &pSpec->auto_ah; return NULL; }
	LPST_REMOTE_SPEC get_rmt_ah() { if (pSpec != NULL)return &pSpec->rmt_ah;return NULL;	}
};

