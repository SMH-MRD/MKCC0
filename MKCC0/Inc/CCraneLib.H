#pragma once

#include "SPEC_DEF.h"
#include "COMMON_DEF.h"
#include "NET_DEF.h"
#include "PLC_DEF.h"
#include "CPlcIo.h"

using namespace std;

typedef struct _ST_CRANE_OBJ {
	LIST_ID_W_WRITE		id_list_w;
	LIST_ID_W_READ		id_list_r;

	//B220-B26F
	CBitIO* ctrl_source_on_pb;
	CBitIO* ctrl_source_off_pb;
	CModeIO* mh_spd_low_cs; 
	CModeIO* bh_normal_cs; 
	CBitIO* mh_high_spd;
	CBitIO* ah_multi;
	CBitIO* ah_single;
	CNotchIO* notch[MOTION_ID_MAX]; 

	CBitIO* load_swayless;
	CModeIO* gt_spd_cs;
	CBitIO* cab_estp_pb;
	CBitIO* whip_5t_ng;
	CBitIO* mlim_acc_low;
	CBitIO* mlim_3times;
	CBitIO* mlim_load_90;
	CBitIO* mlim_load_100;
	CBitIO* mlim_high_load;
	CBitIO* mlim_normal;
	CBitIO* wind_spd16;
	CBitIO* bz_stop;
	CBitIO* fault_reset;
	CBitIO* il_bypass;
	CBitIO* mhbrk_opn_normal;
	CBitIO* mhbrk_opn_abnormal;
	CBitIO* mh_emr_open;
	CBitIO* ah_emr_lower;
	CBitIO* trace;
	CModeIO* ah_spd_cs;
	CBitIO* foot_brk;
	CBitIO* siren;
	CBitIO* camera_ghange;
	CBitIO* slew_fix;
	CBitIO* camera_sel_east;
	CBitIO* camera_sel_conj;
	CBitIO* camera_sel_west;
	CBitIO* high_pow_ON;
	CBitIO* high_pow_OFF;
	CBitIO* slew_notch5;
	CModeIO* jib_approch_cs;
	CModeIO* gt_warm_shock_cs;
	CBitIO* ah_high_pos_load;
}ST_CRANE_OBJ, * LPST_CRANE_OBJ;

class CCraneBase {

private:
	static ST_SPEC _spec;
	static ST_CRANE_OBJ _cran_obj;

public:
	CCraneBase() {};
	virtual ~CCraneBase(void) {};

	static ST_SPEC get_spec();
	static void set_spec_H6R602();
	static void set_spec_HHGH29();
	static void set_spec_HHGQ18();
	static void set_spec_HHFR22();
	
	static void setup_plc_io_H6R602();
	static void setup_plc_io_HHGH29();
	static void setup_plc_io_HHGQ18();
	static void setup_plc_io_HHFR22();

};