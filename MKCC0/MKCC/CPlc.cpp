#include "CPlc.h"
#include "COMMON_DEF.h"


ST_PLC_IO_RIF CPlc::plc_io_rif;
ST_PLC_IO_WIF CPlc::plc_io_wif;

ST_PLC_IO_RIF plc_io_rdef0 = { 
	//{pi16,mask,type,lp,hp}
	//PLC制御
	{NULL,BITS_WORD,			CODE_PLCIO_WORD,	0,1},	//plc_healthy;
	{NULL,BITS_WORD,			CODE_PLCIO_WORD,	0,1},	//plc_ctrl_mode;
	//運転室操作台
	//B220
	{NULL,BIT0,					CODE_PLCIO_BIT,		0,0},	//syukan_on;
	{NULL,BIT1,					CODE_PLCIO_BIT_NC,	0,0},	//syukan_off;
	{NULL,BIT2 | BIT3 | BIT7,	CODE_PLCIO_BITS,	2,0},	//mh_spd_cs;
	{NULL,BIT4 | BIT5 | BIT6,	CODE_PLCIO_BITS,	4,0},	//bh_mode_cs;
	{NULL,BIT8,					CODE_PLCIO_BIT,		0,0},	//ah_use_sel;	 
	{NULL,0xFC00,				CODE_PLCIO_BITS,	10,0},	// ah_notch;
	//B230
	{NULL,0x003F,				CODE_PLCIO_BITS,	0,0},	//mh_notch;
	{NULL,BIT7 | BIT8,			CODE_PLCIO_BITS,	7,0},	//gt_spd_sel;	
	{NULL,0x7E00,				CODE_PLCIO_BITS,	9,0},	//gt_notch;	
	{NULL,BIT15,				CODE_PLCIO_BIT_NC,	0,0},	//estop;	
	//B240
	{NULL,BIT0,					CODE_PLCIO_BIT,		0,0},	//ah_under_limit;補巻制限荷重以下
	{NULL,BIT1,					CODE_PLCIO_BIT,		0,0},	//mlim_warn_1;	ﾓｰﾒﾝﾄﾘﾐｯﾀ旋回加速切替荷重以下//ah_under_limit;
	{NULL,BIT2,					CODE_PLCIO_BIT,		0,0},	//mlim_warn_2;	ﾓｰﾒﾝﾄﾘﾐｯﾀ3倍速許可荷重以下//ah_under_limit;
	{NULL,BIT3,					CODE_PLCIO_BIT,		0,0},	//mlim_90;		ﾓｰﾒﾝﾄﾘﾐｯﾀ90%荷重//ah_under_limit;
	{NULL,BIT4,					CODE_PLCIO_BIT,		0,0},	//mlim_100;		ﾓｰﾒﾝﾄﾘﾐｯﾀ100%荷重//ah_under_limit;
	{NULL,BIT5,					CODE_PLCIO_BIT,		0,0},	//mlim_under_hmh;ﾓｰﾒﾝﾄﾘﾐｯﾀ高巻荷重以下//ah_under_limit;
	{NULL,BIT6,					CODE_PLCIO_BIT,		0,0},	//mlim_normal;	ﾓｰﾒﾝﾄﾘﾐｯﾀ正常//ah_under_limit;
	{NULL,BIT7,					CODE_PLCIO_BIT,		0,0},	//wind_over16;
	{NULL,BIT8,					CODE_PLCIO_BIT,		0,0},	//alarm_stp_pb;
	{NULL,BIT9,					CODE_PLCIO_BIT,		0,0},	//fault_reset_pb;
	{NULL,BIT10,				CODE_PLCIO_BIT,		0,0},	//bypass_pb;
	{NULL,BIT11,				CODE_PLCIO_BIT,		0,0},	//mhbk_normal_ss;
	{NULL,BIT12,				CODE_PLCIO_BIT,		0,0},	//mhbk_emr_ss;
	{NULL,BIT13,				CODE_PLCIO_BIT,		0,0},	//mhbk_opn_pb;
	{NULL,BIT14,				CODE_PLCIO_BIT,		0,0},	//ahbk_emr_low_ss;
	//B250
	{NULL,0x003F,				CODE_PLCIO_BITS,	0,0},	//bh_notch;
	{NULL,BIT8,					CODE_PLCIO_BIT,		0,0},	//sl_brake;
	{NULL,0x7E00,				CODE_PLCIO_BITS,	9,0},	//sl_notch;
	{NULL,BIT15,				CODE_PLCIO_BIT,		0,0},	//sl_hydr_press_sw;	旋回油圧圧力スイッチ
	//B160
	{NULL,BIT0,					CODE_PLCIO_BIT,		0,0},	//syukan_comp_bz;	//主幹投入完了  Bz
	{NULL,BIT1,					CODE_PLCIO_BIT,		0,0},	//syukan_mc_comp;	//主幹MC投入完了
	{NULL,BIT2,					CODE_PLCIO_BIT,		0,0},	//fault_pl;			//故障ランプ
	{NULL,BIT3,					CODE_PLCIO_BIT,		0,0},	//syukairo_comp;    //主回路準備完了
	{NULL,BIT8,					CODE_PLCIO_BIT,		0,0},	//takamaki_mode;	//高巻モード
	{NULL,BIT9,					CODE_PLCIO_BIT,		0,0},	//bh_rest_mode;		//引込レストモード
	{NULL,BIT10,				CODE_PLCIO_BIT,		0,0},	//sl_fix_pl;		//旋回固定PL
	{NULL,BIT12,				CODE_PLCIO_BIT,		0,0},	//ah_ari_jc
	{NULL,BIT13,				CODE_PLCIO_BIT,		0,0},	//sl_fix_pl2;		//旋回固定PL
	{NULL,BIT14,				CODE_PLCIO_BIT,		0,0},	//douryoku_ok;		//動力電源確立
	//X0C0
	{NULL,BIT9,					CODE_PLCIO_BIT,		0,0},	//brk_mc3_fb;		//ブレーキ主幹アンサーバック
	{NULL,BIT10,				CODE_PLCIO_BIT,		0,0},	//mh_brk1_fb;		//主巻ブレーキアンサーバック
	{NULL,BIT12,				CODE_PLCIO_BIT,		0,0},	//bh_brk_fb;		//引込ブレーキアンサーバック
	{NULL,BIT13,				CODE_PLCIO_BIT,		0,0},	//gt_brk_fb;		//走行ブレーキアンサーバック

	//インバータへの指令出力内容
	{NULL,BIT0,					CODE_PLCIO_BIT,		0,0},//inv_fwd_mh
	{NULL,BIT1,					CODE_PLCIO_BIT,		0,0},//inv_rev_mh
	{NULL,BIT0,					CODE_PLCIO_BIT,		0,0},//inv_fwd_bh
	{NULL,BIT1,					CODE_PLCIO_BIT,		0,0},//inv_rev_bh
	{NULL,BIT8,					CODE_PLCIO_BIT,		0,0},//inv_fwd_sl
	{NULL,BIT9,					CODE_PLCIO_BIT,		0,0},//inv_rev_sl
	{NULL,BIT0,					CODE_PLCIO_BIT,		0,0},//inv_fwd_gt
	{NULL,BIT1,					CODE_PLCIO_BIT,		0,0},//inv_rev_gt

	//インバータ速度指令
	{NULL,BITFF,				CODE_PLCIO_WORD,	0,0},//inv_vref_mh
	{NULL,BITFF,				CODE_PLCIO_WORD,	0,0},//inv_vref_bh
	{NULL,BITFF,				CODE_PLCIO_WORD,	0,0},//inv_vref_sl
	{NULL,BITFF,				CODE_PLCIO_WORD,	0,0},//inv_vref_gt

	//インバータトルク指令
	{NULL,BITFF,				CODE_PLCIO_WORD,	0,0},//inv_trqref_mh
	{NULL,BITFF,				CODE_PLCIO_WORD,	0,0},//inv_trqref_bh

	//SIM　高速カウンタ　アブソコーダ
	{NULL,BITFF,				CODE_PLCIO_DWORD,	0,0},	//hcounter_mh
	{NULL,BITFF,				CODE_PLCIO_DWORD,	0,0},	//hcounter_bh
	{NULL,BITFF,				CODE_PLCIO_DWORD,	0,0},	//hcounter_sl
	{NULL,BITFF,				CODE_PLCIO_DWORD,	0,0},	//absocoder_mh

};
ST_PLC_IO_WIF plc_io_wdef0 = { 
	//{pi16,mask,type,lp,hp}
	//PLC制御
	{NULL,BITS_WORD,			CODE_PLCIO_WORD,	0,1},	//pc_healthy;
	{NULL,BITS_WORD,			CODE_PLCIO_WORD,	0,1},	//pc_ctrl_mode;
	//運転室操作台
	//B220
	{NULL,BIT0,					CODE_PLCIO_BIT,		0,0},	//syukan_on;
	{NULL,BIT1,					CODE_PLCIO_BIT_NC,	0,0},	//syukan_off;
	{NULL,BIT2 | BIT3 | BIT7,	CODE_PLCIO_BITS,	2,0},	//mh_spd_cs;
	{NULL,BIT4 | BIT5 | BIT6,	CODE_PLCIO_BITS,	4,0},	//bh_mode_cs;
	{NULL,BIT8,					CODE_PLCIO_BIT,		0,0},	//ah_use_sel;	 
	{NULL,0xFC00,				CODE_PLCIO_BITS,	10,0},	// ah_notch;
	//B230
	{NULL,0x003F,				CODE_PLCIO_BITS,	0,0},	//mh_notch;
	{NULL,BIT7 | BIT8,			CODE_PLCIO_BITS,	7,0},	//gt_spd_sel;	
	{NULL,0x7E00,				CODE_PLCIO_BITS,	9,0},	//gt_notch;	
	{NULL,BIT15,				CODE_PLCIO_BIT_NC,	0,0},	//estop;	
	//B240
	{NULL,BIT0,					CODE_PLCIO_BIT,		0,0},	//ah_under_limit;補巻制限荷重以下
	{NULL,BIT1,					CODE_PLCIO_BIT,		0,0},	//mlim_warn_1;	ﾓｰﾒﾝﾄﾘﾐｯﾀ旋回加速切替荷重以下//ah_under_limit;
	{NULL,BIT2,					CODE_PLCIO_BIT,		0,0},	//mlim_warn_2;	ﾓｰﾒﾝﾄﾘﾐｯﾀ3倍速許可荷重以下//ah_under_limit;
	{NULL,BIT3,					CODE_PLCIO_BIT,		0,0},	//mlim_90;		ﾓｰﾒﾝﾄﾘﾐｯﾀ90%荷重//ah_under_limit;
	{NULL,BIT4,					CODE_PLCIO_BIT,		0,0},	//mlim_100;		ﾓｰﾒﾝﾄﾘﾐｯﾀ100%荷重//ah_under_limit;
	{NULL,BIT5,					CODE_PLCIO_BIT,		0,0},	//mlim_under_hmh;ﾓｰﾒﾝﾄﾘﾐｯﾀ高巻荷重以下//ah_under_limit;
	{NULL,BIT6,					CODE_PLCIO_BIT,		0,0},	//mlim_normal;	ﾓｰﾒﾝﾄﾘﾐｯﾀ正常//ah_under_limit;
	{NULL,BIT7,					CODE_PLCIO_BIT,		0,0},	//wind_over16;
	{NULL,BIT8,					CODE_PLCIO_BIT,		0,0},	//alarm_stp_pb;
	{NULL,BIT9,					CODE_PLCIO_BIT,		0,0},	//fault_reset_pb;
	{NULL,BIT10,				CODE_PLCIO_BIT,		0,0},	//bypass_pb;
	{NULL,BIT11,				CODE_PLCIO_BIT,		0,0},	//mhbk_normal_ss;
	{NULL,BIT12,				CODE_PLCIO_BIT,		0,0},	//mhbk_emr_ss;
	{NULL,BIT13,				CODE_PLCIO_BIT,		0,0},	//mhbk_opn_pb;
	{NULL,BIT14,				CODE_PLCIO_BIT,		0,0},	//ahbk_emr_low_ss;
	//B250
	{NULL,0x003F,				CODE_PLCIO_BITS,	0,0},	//bh_notch;
	{NULL,BIT8,					CODE_PLCIO_BIT,		0,0},	//sl_brake;
	{NULL,0x7E00,				CODE_PLCIO_BITS,	9,0},	//sl_notch;
	{NULL,BIT15,				CODE_PLCIO_BIT,		0,0},	//sl_hydr_press_sw;	旋回油圧圧力スイ

	//SIM　高速カウンタ　アブソコーダ
	{NULL,BITFF,				CODE_PLCIO_DWORD,	0,0},	//hcounter_mh
	{NULL,BITFF,				CODE_PLCIO_DWORD,	0,0},	//hcounter_bh
	{NULL,BITFF,				CODE_PLCIO_DWORD,	0,0},	//hcounter_sl
	{NULL,BITFF,				CODE_PLCIO_DWORD,	0,0},	//absocoder_mh

	//SIM　INV 出力
	{NULL,BITFF,				CODE_PLCIO_WORD,	0,0},//vfb_mh;
	{NULL,BITFF,				CODE_PLCIO_WORD,	0,0},//vfb_bh;
	{NULL,BITFF,				CODE_PLCIO_WORD,	0,0},//vfb_sl;
	{NULL,BITFF,				CODE_PLCIO_WORD,	0,0},//trqref_mh
	{NULL,BITFF,				CODE_PLCIO_WORD,	0,0},//trqref_bh

};

int CPlc::setup(int crane_id) {
	int i = 0;
	INT16* p;
	//ST_PLC_IO_DEF構造体の信号定義（ビットパターン,信号タイプ,ビットシフト数,サイズ）
	//内容をセット⇒データバッファのアドレスをセット
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
			i = 1; plc_io_rif.plc_ctrl_fb.pi16 = p+i;
			//運転室
			i = 6;
			plc_io_rif.syukan_on.pi16 
			= plc_io_rif.syukan_off.pi16 
			= plc_io_rif.mh_spd_cs.pi16 
			= plc_io_rif.bh_mode_cs.pi16 
			= plc_io_rif.ah_use_sel.pi16  
			= plc_io_rif.ah_notch.pi16 
			= p + i;

			i = 7;
			plc_io_rif.mh_notch.pi16
			= plc_io_rif.gt_spd_sel.pi16
			= plc_io_rif.gt_notch.pi16
			= plc_io_rif.estop.pi16
			= p + i;
			
			i = 8;
			plc_io_rif.ah_under_limit.pi16
			= plc_io_rif.mlim_warn_1.pi16
			= plc_io_rif.mlim_warn_2.pi16
			= plc_io_rif.mlim_90.pi16
			= plc_io_rif.mlim_100.pi16
			= plc_io_rif.mlim_under_hmh.pi16
			= plc_io_rif.mlim_normal.pi16
			= plc_io_rif.wind_over16.pi16
			= plc_io_rif.alarm_stp_pb.pi16
			= plc_io_rif.fault_reset_pb.pi16
			= plc_io_rif.bypass_pb.pi16
			= plc_io_rif.mhbk_normal_ss.pi16
			= plc_io_rif.mhbk_emr_ss.pi16
			= plc_io_rif.mhbk_opn_pb.pi16
			= plc_io_rif.ahbk_emr_low_ss.pi16
			= p + i;

			i = 9;
			plc_io_rif.bh_notch.pi16
			= plc_io_rif.sl_brake.pi16
			= plc_io_rif.sl_notch.pi16
			= plc_io_rif.sl_hydr_press_sw.pi16
			= p + i;

			i = 15;
			plc_io_rif.syukan_comp_bz.pi16
			= plc_io_rif.syukan_mc_comp.pi16
			= plc_io_rif.fault_pl.pi16
			= plc_io_rif.syukairo_comp.pi16
			= plc_io_rif.takamaki_mode.pi16
			= plc_io_rif.bh_rest_mode.pi16
			= plc_io_rif.sl_fix_pl.pi16
			= plc_io_rif.ah_ari_jc.pi16
			= plc_io_rif.sl_fix_pl2.pi16
			= plc_io_rif.douryoku_ok.pi16
			= p + i;
			//電気室
			//X0C0
			i = 66;
			plc_io_rif.brk_mc3_fb.pi16
			= plc_io_rif.mh_brk1_fb.pi16
			= plc_io_rif.bh_brk_fb.pi16
			= plc_io_rif.gt_brk_fb.pi16
			= p + i;

			//インバータへの指令出力内容
			//Y0F0
			i = 57;
			plc_io_rif.inv_fwd_mh.pi16
			= plc_io_rif.inv_rev_mh.pi16
			= p + i;	//主巻インバータ指令FWD,REV
			
			i = 58;
			plc_io_rif.inv_fwd_bh.pi16
			= plc_io_rif.inv_rev_bh.pi16
			= plc_io_rif.inv_fwd_sl.pi16
			= plc_io_rif.inv_rev_sl.pi16
			= p + i;	//引込,旋回インバータ指令FWD,REV
			
			i = 59;
			plc_io_rif.inv_fwd_gt.pi16
			= plc_io_rif.inv_rev_gt.pi16
			= p + i;	//走行インバータ指令FWD,REV

			//インバータ速度指令
			plc_io_rif.inv_vref_mh.pi16		= p + 70;	//主巻インバータ速度指令
			plc_io_rif.inv_vref_bh.pi16		= p + 71;	//引込インバータ速度指令
			plc_io_rif.inv_vref_sl.pi16		= p + 72;	//旋回インバータ速度指令
			plc_io_rif.inv_vref_gt.pi16		= p + 73;	//走行インバータ速度指令
			//インバータトルク指令
			plc_io_rif.inv_trqref_mh.pi16	= p + 75;	//主巻インバータトルク指令
			plc_io_rif.inv_trqref_bh.pi16	= p + 76;	//引込インバータトルク指令

			//高速カウンタ,アブソコーダ
			plc_io_rif.hcounter_mh.pi16		= p + 80;
			plc_io_rif.hcounter_bh.pi16		= p + 84;
			plc_io_rif.hcounter_sl.pi16		= p + 86;
			plc_io_rif.absocoder_mh.pi16	= p + 88;

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
			plc_io_wif.syukan_on.pi16
				= plc_io_wif.syukan_off.pi16
				= plc_io_wif.mh_spd_cs.pi16
				= plc_io_wif.bh_mode_cs.pi16
				= plc_io_wif.ah_use_sel.pi16
				= plc_io_wif.ah_notch.pi16
				= p + i;

			i = 7;
			plc_io_wif.mh_notch.pi16
				= plc_io_wif.gt_spd_sel.pi16
				= plc_io_wif.gt_notch.pi16
				= plc_io_wif.estop.pi16
				= p + i;

			i = 8;
			plc_io_wif.ah_under_limit.pi16
				= plc_io_wif.mlim_warn_1.pi16
				= plc_io_wif.mlim_warn_2.pi16
				= plc_io_wif.mlim_90.pi16
				= plc_io_wif.mlim_100.pi16
				= plc_io_wif.mlim_under_hmh.pi16
				= plc_io_wif.mlim_normal.pi16
				= plc_io_wif.wind_over16.pi16
				= plc_io_wif.alarm_stp_pb.pi16
				= plc_io_wif.fault_reset_pb.pi16
				= plc_io_wif.bypass_pb.pi16
				= plc_io_wif.mhbk_normal_ss.pi16
				= plc_io_wif.mhbk_emr_ss.pi16
				= plc_io_wif.mhbk_opn_pb.pi16
				= plc_io_wif.ahbk_emr_low_ss.pi16
				= p + i;

			i = 9;
			plc_io_wif.bh_notch.pi16
				= plc_io_wif.sl_brake.pi16
				= plc_io_wif.sl_notch.pi16
				= plc_io_wif.sl_hydr_press_sw.pi16
				= p + i;
			//高速カウンタ,アブソコーダ
			plc_io_wif.hcounter_mh.pi16		= p + 30;
			plc_io_wif.hcounter_bh.pi16		= p + 34;
			plc_io_wif.hcounter_sl.pi16		= p + 36;
			plc_io_wif.absocoder_mh.pi16	= p + 38;

			plc_io_wif.vfb_mh.pi16			= p + 70;
			plc_io_wif.vfb_bh.pi16			= p + 71;
			plc_io_wif.vfb_sl.pi16			= p + 72;

			plc_io_wif.trqref_mh.pi16		= p + 75;
			plc_io_wif.trqref_bh.pi16		= p + 76;
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