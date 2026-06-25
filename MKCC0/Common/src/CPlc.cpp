#include "COMMON_DEF.h"
#include "CPlc.h"
#include ".\Cranes\HHGH2900_Plc.h"
#include ".\Cranes\HHGQ1800_Plc.h"
#include ".\Cranes\HHFM0800_Plc.h"

int CPlc::setup(int machine_id) {

	//#####　 1.ST_PLC_IO_RIF構造体の初期化　###########
	ST_PLC_IO_DEF io_def = { NULL,BITOFF,CODE_PLCIO_WORD,0,0,0 };
	for(int i = 0; i < N_ST_PLC_IO_DEF; i++) {
		un_plc_io_rif.io[i] = un_plc_io_wif.io[i] = io_def;
	}

	//#####   2.IO参照情報のセット　         ###########
	int i = 0;
	INT16* p;
	//ST_PLC_IO_DEF構造体の信号定義（ビットパターン,信号タイプ,ビットシフト数,サイズ）
	//内容をセット⇒データバッファのアドレスをセット
	switch (machine_id) {
	case CRANE_ID_H6R602: break;
	case CRANE_ID_HHGH29: {
		un_plc_io_rif.JC = plc_io_rdef_hhgh29;
		un_plc_io_wif.JC = plc_io_wdef_hhgh29;	//IOデータインスタンスバッファ先頭アドレス
	}break;
	case CRANE_ID_HHGQ18:{
		un_plc_io_rif.JC = plc_io_rdef_hhgq18;
		un_plc_io_wif.JC = plc_io_wdef_hhgq18;	//IOデータインスタンスバッファ先頭アドレス
	}break;
	case CRANE_ID_HHFM08: {
		un_plc_io_rif.GC = plc_io_rdef_hhfm08;
		un_plc_io_wif.GC = plc_io_wdef_hhfm08;	//IOデータインスタンスバッファ先頭アドレス
	}break;
	default:break;
	}

	// ###### 3.データ参照用アドレスの設定         ###########

	for (int i = 0; i < N_ST_PLC_IO_DEF; i++) {
		un_plc_io_rif.io[i].pi16 = pbuf_r + un_plc_io_rif.io[i].index;
		un_plc_io_wif.io[i].pi16 = pbuf_w + un_plc_io_wif.io[i].index;
	}
		return 0;
}

/// <summary>
/// PLC読み込みバッファからの取り込み関数
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
	case CODE_PLCIO_BIT_NC: {//Normal Close
		if (*st_r_def.pi16 & st_r_def.mask)	uval.i16 = L_OFF;
		else								uval.i16 = L_ON;
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
		uval.i16 = uval.i16 >> st_r_def.shift;
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
/// PLC書き込みバッファの更新用関数
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
	case CODE_PLCIO_BIT_NC: {
		if (val) *(st_w_def.pi16) &= ~st_w_def.mask;
		else	 *(st_w_def.pi16) |= st_w_def.mask;
	}break;
	case CODE_PLCIO_WORD: {
		*(st_w_def.pi16) = val;
	}break;
	case CODE_PLCIO_DWORD: {
		*((INT32*)(st_w_def.pi16)) = val;
	}break;
	case CODE_PLCIO_BITS: {
		INT16 val16 = val;
		val16 = val16 << st_w_def.shift;
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
	case CODE_PLCIO_BIT_NC: {
		if (val) *(st_w_def.pi16) &= ~st_w_def.mask;
		else	 *(st_w_def.pi16) |= st_w_def.mask;
	}break;
	case CODE_PLCIO_WORD: {
		*(st_w_def.pi16) = (INT16)val;
	}break;
	case CODE_PLCIO_DWORD: {
		*((INT32*)(st_w_def.pi16)) = val;
	}break;
	case CODE_PLCIO_BITS: {
		INT16 val16 = (INT16)val;
		val16 = val16 << st_w_def.shift;
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
		val16 = val16 << st_w_def.shift;
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
		val16 = val16 << st_w_def.shift;
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