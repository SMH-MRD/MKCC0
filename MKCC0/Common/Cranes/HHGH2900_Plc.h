// ################################################################################
// ##############  PLC IO設定ファイル　　      　　　##############################
// ##############  西多度津70tJC 102号（HHGH2900）   ##############################
// ################################################################################
//西多度津70tJC102号の定義
ST_HHGH29_PLC_R plc_io_rdef_hhgh29 = {
	//{pi16,mask,type,lp,hp}
	//PLC制御
	{NULL,BITS_WORD,			CODE_PLCIO_WORD,	0,1},	//plc_healthy;
	{NULL,BITS_WORD,			CODE_PLCIO_WORD,	0,1},	//plc_ctrl_mode;
	//運転室操作台
	//B220
	{NULL,BIT0,					CODE_PLCIO_BIT,		0,0},	//syukan_on;
	{NULL,BIT1,					CODE_PLCIO_BIT_NC,	0,0},	//syukan_off;
	{NULL,BIT2 | BIT3 | BIT7,	CODE_PLCIO_BITS,	0,0},	//mh_spd_cs;
	{NULL,BIT4 | BIT5 | BIT6,	CODE_PLCIO_BITS,	0,0},	//bh_mode_cs;
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
	//B2B0
	{NULL,BITFF,				CODE_PLCIO_WORD,	0,0},	// fault_bz;		故障ブザー	
	{NULL,BIT4,					CODE_PLCIO_BIT,		0,0},	//auto_kyusi		旋回自動給脂装置ランプ
	//B160
	{NULL,BIT0,					CODE_PLCIO_BIT,		0,0},	//syukan_comp_bz;	//主幹投入完了  Bz
	{NULL,BIT1,					CODE_PLCIO_BIT,		0,0},	//syukan_mc_comp;	//主幹MC投入完了
	{NULL,BIT2,					CODE_PLCIO_BIT,		0,0},	//fault_pl;			//故障ランプ
	{NULL,BIT3,					CODE_PLCIO_BIT,		0,0},	//syukairo_comp;    //主回路準備完了
	{NULL,BIT8,					CODE_PLCIO_BIT,		0,0},	//takamaki_mode;	//高巻モード
	{NULL,BIT9,					CODE_PLCIO_BIT,		0,0},	//bh_rest_mode;		//引込レストモード
	{NULL,BIT11,				CODE_PLCIO_BIT,		0,0},	//mercury_lamp_sw1;	水銀灯スイッチ1sl_fix_pl;L
	{NULL,BIT12,				CODE_PLCIO_BIT,		0,0},	//mercury_lamp_sw2;	水銀灯スイッチ2ah_ari_jc
	{NULL,BIT13,				CODE_PLCIO_BIT,		0,0},	//mercury_lamp_sw3;	水銀灯スイッチ3sl_fix_pl2;
	{NULL,BIT14,				CODE_PLCIO_BIT,		0,0},	//douryoku_ok;		//動力電源確立;
	{NULL,BIT15,				CODE_PLCIO_BIT,		0,0},	//siren_sw;			モータサイレンスイッチ

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

	//インバータ速度FB
	{NULL,BITFF,				CODE_PLCIO_WORD,	0,0},//inv_vfb_mh
	{NULL,BITFF,				CODE_PLCIO_WORD,	0,0},//inv_vfb_bh
	{NULL,BITFF,				CODE_PLCIO_WORD,	0,0},//inv_vfb_sl
	{NULL,BITFF,				CODE_PLCIO_WORD,	0,0},//inv_vfb_gt

	//目標速度％
	{NULL,BITFF,				CODE_PLCIO_WORD,	0,0},//target_v_m
	{NULL,BITFF,				CODE_PLCIO_WORD,	0,0},//target_v_b
	{NULL,BITFF,				CODE_PLCIO_WORD,	0,0},//target_v_s
	{NULL,BITFF,				CODE_PLCIO_WORD,	0,0},//target_v_g

	//インバータトルク指令
	{NULL,BITFF,				CODE_PLCIO_WORD,	0,0},//inv_trqref_mh
	{NULL,BITFF,				CODE_PLCIO_WORD,	0,0},//inv_trqref_bh

	//SIM　高速カウンタ　アブソコーダ
	{NULL,BITFF,				CODE_PLCIO_DWORD,	0,0},	//hcounter_mh
	{NULL,BITFF,				CODE_PLCIO_DWORD,	0,0},	//hcounter_bh
	{NULL,BITFF,				CODE_PLCIO_DWORD,	0,0},	//hcounter_sl

	{NULL,BITFF,				CODE_PLCIO_DWORD,	0,0},	//absocoder_mh
	{NULL,BITFF,				CODE_PLCIO_DWORD,	0,0},	//absocoder_gt
	//荷重
	{NULL,BITFF,				CODE_PLCIO_WORD,	0,0},//荷重
	//揚程　旋回半径
	{NULL,BITFF,				CODE_PLCIO_DWORD,	0,0},	//h_mh_mm
	{NULL,BITFF,				CODE_PLCIO_FLOAT,	0,0},	//r_bh_m

	//風速
	{ NULL,BITFF,				CODE_PLCIO_WORD,	0,0 },	//wind_spd_01m
};

ST_HHGH29_PLC_W plc_io_wdef_hhgh29 = {
	//{pi16,mask,type,lp:ビットシフト,hp：バッファサイズ}
	//PLC制御
	{NULL,BITS_WORD,			CODE_PLCIO_WORD,	0,1},	//pc_healthy;
	{NULL,BITS_WORD,			CODE_PLCIO_WORD,	0,1},	//pc_ctrl_mode;
	//運転室操作台
	//B160
	{NULL,BIT11,				CODE_PLCIO_BIT,		0,0},	//mercury_lamp_sw1;	水銀灯スイッチ1
	{NULL,BIT12,				CODE_PLCIO_BIT,		0,0 },	//mercury_lamp_sw2;	水銀灯スイッチ2
	{NULL,BIT13,				CODE_PLCIO_BIT,		0,0 },	//mercury_lamp_sw3;	水銀灯スイッチ3
	{NULL,BIT15,				CODE_PLCIO_BIT,		0,0 },	//siren_sw;			モータサイレンスイッチ

	//B220
	{NULL,BIT0,					CODE_PLCIO_BIT,		0,0},	//syukan_on;
	{NULL,BIT1,					CODE_PLCIO_BIT_NC,	0,0},	//syukan_off;
	{NULL,BIT2 | BIT3 | BIT7,	CODE_PLCIO_BITS,	0,0},	//mh_spd_cs;
	{NULL,BIT4 | BIT5 | BIT6,	CODE_PLCIO_BITS,	0,0},	//bh_mode_cs;
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
	{NULL,BITFF,				CODE_PLCIO_DWORD,	0,0},	//absocoder_gt

	//SIM　INV 出力
	{NULL,BITFF,				CODE_PLCIO_WORD,	0,0},//vfb_mh;
	{NULL,BITFF,				CODE_PLCIO_WORD,	0,0},//vfb_bh;
	{NULL,BITFF,				CODE_PLCIO_WORD,	0,0},//vfb_sl;
	{NULL,BITFF,				CODE_PLCIO_WORD,	0,0},//vfb_gt;
	{NULL,BITFF,				CODE_PLCIO_WORD,	0,0},//trqref_mh
	{NULL,BITFF,				CODE_PLCIO_WORD,	0,0},//trqref_bh

	//モーメントリミッタ
	{NULL,BITFF,				CODE_PLCIO_WORD,	0,0},//h_mh_mm	//モーメントリミッタ荷重AI
	{NULL,BITFF,				CODE_PLCIO_WORD,	0,0},//r_bh_m	//モーメントリミッタ旋回半径AI
	//風速AI
	{NULL,BITFF,				CODE_PLCIO_WORD,	0,0},//wind_spd_ai 

	//OTEヘッダコマンド
	{NULL,BITFF,				CODE_PLCIO_WORD,	0,0},// 

	//映像遅延検出用デバイス
	{NULL,BIT0,					CODE_PLCIO_BIT,		0,0},	//ST_PLC_IO_DEF v_delay_device;
};

