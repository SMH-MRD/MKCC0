#include "CSpec.h"
#include "COMMON_DEF.h"

ST_STRUCTURE	CSpec::st_struct;//ç\ë¢
ST_STRUCTURE	st_struct0 = {
	 /*wheel_span	*/ 20.0,	
	 /*leg_span		*/ 15.0,		
	 /*Hp			*/ 47.7,		
	 /*Lb			*/ 64.009,	
	 /*Lm			*/ 63.000,	
	 /*La_add		*/ 4.981,	
	 /*rad_mh_ah	*/ 2.371,	
	 /*La			*/ 66.566,	
	 /*rad_Lm_La	*/ 0.0537,	
	 /*Lmb			*/ 1.676,
	 /*Lma			*/ 4.981,
	 /*Lba			*/ 5.512,
	 /*Th_pba		*/ 2.00301,
	 /*Alpa_db		*/ 0.90266,
	 /*Alpa_b		*/ 0.02109,	
	 /*Alpa_a		*/ 0.05373,	
	 /*Phbm			*/ 2.20547,
	 /*Lp			*/ 25.174,
	 /*Php			*/ 1.6583,
	 /*Lmf			*/ 2.9000,
	 /*Laf			*/ 2.2910,
	 /*Th_rest		*/ -0.7113,	
	 /*D_min		*/ 41.425,	
	 /*D_rest		*/ 83.905,	
	 /*Hb_min		*/ 59.397,	
	 /*Hb_rest		*/ -41.786,	
	 /*Dttb			*/ 3.652,	
	 /*Kttb			*/ 0.08434,	
	 /*Nbh_drum		*/ 67.974,	
	 /*Load0_mh		*/ 11000.0,	
	 /*Load0_ah		*/ 1900.0	
};

//éÂä™
ST_AXIS_ITEMS	CSpec::base_mh;
ST_AUTO_SPEC	CSpec::auto_mh;
ST_REMOTE_SPEC	CSpec::rmt_mh;

//ãNïö
ST_AXIS_ITEMS	CSpec::base_bh;
ST_AUTO_SPEC	CSpec::auto_bh;
ST_REMOTE_SPEC	CSpec::rmt_bh;

//ê˘âÒ
ST_AXIS_ITEMS	CSpec::base_sl;
ST_AUTO_SPEC	CSpec::auto_sl;
ST_REMOTE_SPEC	CSpec::rmt_sl;

//ëñçs
ST_AXIS_ITEMS	CSpec::base_gt;
ST_AUTO_SPEC	CSpec::auto_gt;
ST_REMOTE_SPEC	CSpec::rmt_gt;

//ï‚ä™
ST_AXIS_ITEMS	CSpec::base_ah;
ST_AUTO_SPEC	CSpec::auto_ah;
ST_REMOTE_SPEC	CSpec::rmt_ah;

int CSpec::setup(int crane_id) {
	switch (crane_id) {
	case CRANE_ID_H6R602:{
		memcpy_s(&st_struct, sizeof(ST_STRUCTURE), &st_struct0, sizeof(ST_STRUCTURE));
	}break;
	case CARNE_ID_HHGH29:{
		memcpy_s(&st_struct, sizeof(ST_STRUCTURE), &st_struct0, sizeof(ST_STRUCTURE));
	}break;
	case CARNE_ID_HHGQ18:{
		memcpy_s(&st_struct, sizeof(ST_STRUCTURE), &st_struct0, sizeof(ST_STRUCTURE));
	}break;
	case CARNE_ID_HHFR22:{
		memcpy_s(&st_struct, sizeof(ST_STRUCTURE), &st_struct0, sizeof(ST_STRUCTURE));
	}break;
	default: {
		memcpy_s(&st_struct, sizeof(ST_STRUCTURE), &st_struct0, sizeof(ST_STRUCTURE));
	}break;
	}
	return 0;
}

