#include "CPlc.h"
#include "COMMON_DEF.h"


ST_PLC_IO_RIF CPlc::plc_io_rif;
ST_PLC_IO_WIF CPlc::plc_io_wif;

ST_PLC_IO_RIF plc_io_rdef0 = { 
//{pi16,mask,type,lp,hp}
	{NULL,BIT0,					CODE_PLCIO_BIT,		0,0},/* syukan_on*/
	{NULL,BIT1,					CODE_PLCIO_BIT,		0,0},/* syukan_off*/
	{NULL,BIT2|BIT3,			CODE_PLCIO_BITS,	2,0},/* mh_spd_cs*/
	{NULL,BIT5|BIT6|BIT7|BIT8,	CODE_PLCIO_BITS,	5,0},/* bh_mode_cs*/
	{NULL,0xFC00,				CODE_PLCIO_BITS,	10,0},/* gt_notch*/
	{NULL,0x003F,				CODE_PLCIO_BITS,	0,0},/* mh_notch*/
	{NULL,BIT13,				CODE_PLCIO_BIT,		0,0},/* alarm_stp_pb*/
	{NULL,BIT14,				CODE_PLCIO_BIT,		0,0},/* fault_reset_pb*/
	{NULL,BIT15,				CODE_PLCIO_BIT,		0,0},/* bypass_pb*/
	{NULL,BIT0,					CODE_PLCIO_BIT,		0,0},/* mlim_warn_1*/
	{NULL,BIT1,					CODE_PLCIO_BIT,		0,0},/* mlim_warn_2*/
	{NULL,BIT2,					CODE_PLCIO_BIT,		0,0},/* mlim_warn_err*/
	{NULL,BIT3,					CODE_PLCIO_BIT,		0,0},/* mlim_high_enable*/
	{NULL,BIT14,				CODE_PLCIO_BIT,		0,0},/* wind_over16*/
	{NULL,BIT15,				CODE_PLCIO_BIT,		0,0},/* estop*/
	{NULL,0x003F,				CODE_PLCIO_BITS,	0,0},/* bh_notch*/
	{NULL,0x0FC0,				CODE_PLCIO_BITS,	0,0},/* sl_notch*/
	{NULL,BIT15,				CODE_PLCIO_BIT,		0,0},/* sl_brake*/
};
ST_PLC_IO_WIF plc_io_wdef0 = { 
	//{pi16,mask,type,lp,hp}
	// PLC制御
	{NULL,BITFF,				CODE_PLCIO_WORD,	0,0},/* pc_healthy*/
	{NULL,BITFF,				CODE_PLCIO_WORD,	0,0},/* pc_ctrl_mode*/
	//運転室
	{NULL,BIT0,					CODE_PLCIO_BIT,		0,0},/* syukan_on*/
	{NULL,BIT1,					CODE_PLCIO_BIT,		0,0},/* syukan_off*/
	{NULL,BIT2|BIT3,			CODE_PLCIO_BITS,	2,0},/* mh_spd_cs*/
	{NULL,BIT5|BIT6|BIT7|BIT8,	CODE_PLCIO_BITS,	5,0},/* bh_mode_cs*/
	{NULL,0xFC00,				CODE_PLCIO_BITS,	10,0},/* gt_notch*/
	{NULL,0x003F,				CODE_PLCIO_BITS,	0,0},/* mh_notch*/
	{NULL,BIT13,				CODE_PLCIO_BIT,		0,0},/* alarm_stp_pb*/
	{NULL,BIT14,				CODE_PLCIO_BIT,		0,0},/* fault_reset_pb*/
	{NULL,BIT15,				CODE_PLCIO_BIT,		0,0},/* bypass_pb*/
	{NULL,BIT15,				CODE_PLCIO_BIT,		0,0},/* estop*/
	{NULL,0x003F,				CODE_PLCIO_BITS,	0,0},/* bh_notch*/
	{NULL,0x0FC0,				CODE_PLCIO_BITS,	0,0},/* sl_notch*/
};

int CPlc::setup(int crane_id) {
	int i = 0;
	INT16* p;
	switch (crane_id) {
	case CRANE_ID_H6R602: {
		plc_io_rif = plc_io_rdef0;
		plc_io_wif = plc_io_wdef0;
	}break;
	case CARNE_ID_HHGH29: {
		//READ B220 D10408
		{
			plc_io_rif = plc_io_rdef0;
			p = pbuf_r;
			//PLC制御
			i = 0; plc_io_rif.plc_healthy.pi16 = p+i;
			i = 1; plc_io_rif.pc_ctrl_fb.pi16 = p+i;
			//運転室
			i = 6;
			plc_io_rif.syukan_on.pi16 = plc_io_rif.syukan_off.pi16 = plc_io_rif.mh_spd_cs.pi16 = plc_io_rif.bh_mode_cs.pi16 = plc_io_rif.gt_notch.pi16 = p + i;
			i = 7;
			plc_io_rif.mh_notch.pi16 = plc_io_rif.alarm_stp_pb.pi16 = plc_io_rif.fault_reset_pb.pi16 = plc_io_rif.bypass_pb.pi16 = p + i;
			i = 8;
			plc_io_rif.mlim_warn_1.pi16 = plc_io_rif.mlim_warn_2.pi16 = plc_io_rif.mlim_warn_err.pi16 = plc_io_rif.mlim_high_enable.pi16 = p + i;
			plc_io_rif.wind_over16.pi16 = plc_io_rif.estop.pi16 = p + i;
			i = 9;
			plc_io_rif.bh_notch.pi16 = plc_io_rif.sl_notch.pi16 = plc_io_rif.sl_brake.pi16 = p + i;
		}
		//WRITE
		{
			plc_io_wif = plc_io_wdef0;	//IOデータインスタンスバッファ先頭アドレス
			p = pbuf_w;					//書き込みバッファ先頭アドレス
			//PLC制御
			i = 0;plc_io_wif.pc_healthy.pi16 = p + i;
			i = 1;plc_io_wif.pc_ctrl_mode.pi16 = p + i;
			//運転室
			i = 6;
			plc_io_wif.syukan_on.pi16 = plc_io_wif.syukan_off.pi16 = plc_io_wif.mh_spd_cs.pi16 = plc_io_wif.bh_mode_cs.pi16 = plc_io_wif.gt_notch.pi16 = p + i;
			i = 7;
			plc_io_wif.mh_notch.pi16 = plc_io_wif.alarm_stp_pb.pi16 = plc_io_wif.fault_reset_pb.pi16 = plc_io_wif.bypass_pb.pi16 = p + i;
			i = 8;
			plc_io_wif.estop.pi16 = p + i;
			i = 9;
			plc_io_wif.bh_notch.pi16 = plc_io_wif.sl_notch.pi16 = p + i;
		}
	}break;
	case CARNE_ID_HHGQ18:{
		plc_io_rif = plc_io_rdef0;
		plc_io_wif = plc_io_wdef0;
	}break;
	case CARNE_ID_HHFR22:{
		plc_io_rif = plc_io_rdef0;
		plc_io_wif = plc_io_wdef0;
	}break;
	default: {
		plc_io_rif = plc_io_rdef0;
		plc_io_wif = plc_io_wdef0;
	}break;
	}
	return 0;
}

/// <summary>
/// 
/// </summary>
/// <param name="st_r_def"></param>
/// <returns></returns>
UN_IF_VALUE CPlc::rval(ST_PLC_IO_DEF st_r_def) {
	INT16 type = st_r_def.type;
	UN_IF_VALUE uval = { 0 };

	switch (type) {

	case CODE_PLCIO_BIT: {
		if (*st_r_def.pi16 & st_r_def.mask)	uval.i16 = L_ON;
		else								uval.i16 = L_OFF;
	}break;
	case CODE_PLCIO_WORD: {
		uval.i16 = *st_r_def.pi16;
	}break;
	case CODE_PLCIO_DWORD: {
		uval.i32 = *((INT32*)st_r_def.pi16);
	}break;
	case CODE_PLCIO_BITS: {
		uval.i16 = *st_r_def.pi16;
		uval.i16 &= st_r_def.mask;
		uval.i16 = uval.i16 >> st_r_def.lp;
	}break;
	case CODE_PLCIO_FLOAT: {
		uval.f = *((float*)st_r_def.pi16);
	}break;
	case CODE_PLCIO_DOUBLE: {
		uval.d = *((double*)(st_r_def.pi16));
	}break;
	default:uval.i32 = 0; break;
	}
	return uval;
}

/// <summary>
/// 
/// </summary>
/// <param name="st_w_def"></param>
/// <param name="val"></param>
/// <returns></returns>
HRESULT CPlc::wval(ST_PLC_IO_DEF st_w_def, INT16 val) {

	INT16 type = st_w_def.type;

	switch (type) {
	case CODE_PLCIO_BIT: {
		if (val) *(st_w_def.pi16) |= st_w_def.mask;
		else	 *(st_w_def.pi16) &= ~st_w_def.mask;
	}break;
	case CODE_PLCIO_WORD: {
		*(st_w_def.pi16) = val;
	}break;
	case CODE_PLCIO_DWORD: {
		*((INT32*)(st_w_def.pi16)) = val;
	}break;
	case CODE_PLCIO_BITS: {
		INT16 val16 = val;
		val16 = val16 << st_w_def.lp;
		*(st_w_def.pi16) &= ~st_w_def.mask;		
		*(st_w_def.pi16) |= val16;
	}break;
	case CODE_PLCIO_FLOAT: {
		*((float*)st_w_def.pi16) = (float)val ;
	}break;
	case CODE_PLCIO_DOUBLE: {
		*((double*)(st_w_def.pi16)) = (double)val;
	}break;
	default:*(st_w_def.pi16) = 0; break;
	}
	return S_OK;
}

HRESULT CPlc::wval(ST_PLC_IO_DEF st_w_def, INT32 val) {

	INT16 type = st_w_def.type;

	switch (type) {
	case CODE_PLCIO_BIT: {
		if (val) *(st_w_def.pi16) |= st_w_def.mask;
		else	 *(st_w_def.pi16) &= ~st_w_def.mask;
	}break;
	case CODE_PLCIO_WORD: {
		*(st_w_def.pi16) = (INT16)val;
	}break;
	case CODE_PLCIO_DWORD: {
		*((INT32*)(st_w_def.pi16)) = val;
	}break;
	case CODE_PLCIO_BITS: {
		INT16 val16 = (INT16)val;
		val16 = val16 << st_w_def.lp;
		*(st_w_def.pi16) &= ~st_w_def.mask;
		*(st_w_def.pi16) |= val16;
	}break;
	case CODE_PLCIO_FLOAT: {
		*((float*)st_w_def.pi16) = (float)val;
	}break;
	case CODE_PLCIO_DOUBLE: {
		*((double*)(st_w_def.pi16)) = (double)val;
	}break;
	default: *((INT32*)(st_w_def.pi16)) = 0; break;
	}
	return S_OK;
}

HRESULT CPlc::wval(ST_PLC_IO_DEF st_w_def, float val) {

	INT16 type = st_w_def.type;

	switch (type) {
	case CODE_PLCIO_BIT: {
		if (val) *(st_w_def.pi16) |= st_w_def.mask;
		else	 *(st_w_def.pi16) &= ~st_w_def.mask;
	}break;
	case CODE_PLCIO_WORD: {
		*(st_w_def.pi16) = (INT16)val;
	}break;
	case CODE_PLCIO_DWORD: {
		*((INT32*)(st_w_def.pi16)) = (INT32)val;
	}break;
	case CODE_PLCIO_BITS: {
		INT16 val16 = (INT16)val;
		val16 = val16 << st_w_def.lp;
		*(st_w_def.pi16) &= ~st_w_def.mask;
		*(st_w_def.pi16) |= val16;
	}break;
	case CODE_PLCIO_FLOAT: {
		*((float*)st_w_def.pi16) = val;
	}break;
	case CODE_PLCIO_DOUBLE: {
		*((double*)(st_w_def.pi16)) = (double)val;
	}break;
	default: *((float*)(st_w_def.pi16)) = 0.0; break;
	}
	return S_OK;
}

HRESULT CPlc::wval(ST_PLC_IO_DEF st_w_def, double val) {
	INT16 type = st_w_def.type;
	switch (type) {
	case CODE_PLCIO_BIT: {
		if (val) *(st_w_def.pi16) |= st_w_def.mask;
		else	 *(st_w_def.pi16) &= ~st_w_def.mask;
	}break;
	case CODE_PLCIO_WORD: {
		*(st_w_def.pi16) = (INT16)val;
	}break;
	case CODE_PLCIO_DWORD: {
		*((INT32*)(st_w_def.pi16)) = (INT32)val;
	}break;
	case CODE_PLCIO_BITS: {
		INT16 val16 = (INT16)val;
		val16 = val16 << st_w_def.lp;
		*(st_w_def.pi16) &= ~st_w_def.mask;
		*(st_w_def.pi16) |= val16;
	}break;
	case CODE_PLCIO_FLOAT: {
		*((float*)st_w_def.pi16) = (float)val;
	}break;
	case CODE_PLCIO_DOUBLE: {
		*((double*)(st_w_def.pi16)) = val;
	}break;
	default: *((double*)(st_w_def.pi16)) = 0.0; break;
	}
	return S_OK;
}