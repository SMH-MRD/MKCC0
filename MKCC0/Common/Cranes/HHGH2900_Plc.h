// ################################################################################
// ##############  PLC IO設定ファイル　　      　　　##############################
// ##############  西多度津70tJC 102号（HHGH2900）   ##############################
// ################################################################################
//西多度津70tJC102号の定義
//PLCからのIO読み書き条件定義グローバル
ST_JC_PLC_IO_R plc_io_rdef_hhgh29 = {
	//{pi16,mask,type,lp,hp}
	//PLC制御
	{NULL,BITS_WORD,			CODE_PLCIO_WORD,	0,1,0},	//plc_healthy;
	{NULL,BITS_WORD,			CODE_PLCIO_WORD,	0,1,1},	//plc_ctrl_mode;
	//運転室操作台
	//B220
	{NULL,BIT0,					CODE_PLCIO_BIT,		0,0,6},	//syukan_on;
	{NULL,BIT1,					CODE_PLCIO_BIT_NC,	0,0,6},	//syukan_off;
	{NULL,BIT2 | BIT3 | BIT7,	CODE_PLCIO_BITS,	0,0,6},	//mh_spd_cs;
	{NULL,BIT4 | BIT5 | BIT6,	CODE_PLCIO_BITS,	0,0,6},	//bh_mode_cs;
	{NULL,BIT8,					CODE_PLCIO_BIT,		0,0,6},	//ah_use_sel;	
	{NULL,0xFC00,				CODE_PLCIO_BITS,	10,0,6},// ah_notch;

	//B230
	{NULL,0x003F,				CODE_PLCIO_BITS,	0,0,7},	//mh_notch;
	{NULL,BIT7 | BIT8,			CODE_PLCIO_BITS,	7,0,7},	//gt_spd_sel;	
	{NULL,0x7E00,				CODE_PLCIO_BITS,	9,0,7},	//gt_notch;	
	{NULL,BIT15,				CODE_PLCIO_BIT_NC,	0,0,7},	//estop;

	//B240
	{NULL,BIT0,					CODE_PLCIO_BIT,		0,0,8},	//ah_under_limit;補巻制限荷重以下
	{NULL,BIT1,					CODE_PLCIO_BIT,		0,0,8},	//mlim_warn_1;	ﾓｰﾒﾝﾄﾘﾐｯﾀ旋回加速切替荷重以下//ah_under_limit;
	{NULL,BIT2,					CODE_PLCIO_BIT,		0,0,8},	//mlim_warn_2;	ﾓｰﾒﾝﾄﾘﾐｯﾀ3倍速許可荷重以下//ah_under_limit;
	{NULL,BIT3,					CODE_PLCIO_BIT,		0,0,8},	//mlim_90;		ﾓｰﾒﾝﾄﾘﾐｯﾀ90%荷重//ah_under_limit;
	{NULL,BIT4,					CODE_PLCIO_BIT,		0,0,8},	//mlim_100;		ﾓｰﾒﾝﾄﾘﾐｯﾀ100%荷重//ah_under_limit;
	{NULL,BIT5,					CODE_PLCIO_BIT,		0,0,8},	//mlim_under_hmh;ﾓｰﾒﾝﾄﾘﾐｯﾀ高巻荷重以下//ah_under_limit;
	{NULL,BIT6,					CODE_PLCIO_BIT,		0,0,8},	//mlim_normal;	ﾓｰﾒﾝﾄﾘﾐｯﾀ正常//ah_under_limit;
	{NULL,BIT7,					CODE_PLCIO_BIT,		0,0,8},	//wind_over16;
	{NULL,BIT8,					CODE_PLCIO_BIT,		0,0,8},	//alarm_stp_pb;
	{NULL,BIT9,					CODE_PLCIO_BIT,		0,0,8},	//fault_reset_pb;
	{NULL,BIT10,				CODE_PLCIO_BIT,		0,0,8},	//bypass_pb;
	{NULL,BIT11,				CODE_PLCIO_BIT,		0,0,8},	//mhbk_normal_ss;
	{NULL,BIT12,				CODE_PLCIO_BIT,		0,0,8},	//mhbk_emr_ss;
	{NULL,BIT13,				CODE_PLCIO_BIT,		0,0,8},	//mhbk_opn_pb;
	{NULL,BIT14,				CODE_PLCIO_BIT,		0,0,8},	//ahbk_emr_low_ss;


	//B250
	{NULL,0x003F,				CODE_PLCIO_BITS,	0,0,9},	//bh_notch;
	{NULL, BIT6 | BIT7,			CODE_PLCIO_BITS,	0,0,9},	//ah_spd_cs;
	{NULL,BIT8,					CODE_PLCIO_BIT,		0,0,9},	//sl_brake;
	{NULL,0x7E00,				CODE_PLCIO_BITS,	9,0,9},	//sl_notch;
	{NULL,BIT15,				CODE_PLCIO_BIT,		0,0,9},	//sl_hydr_press_sw;	旋回油圧圧力スイッチ
	//B2B0
	{NULL,BITFF,				CODE_PLCIO_WORD,	0,0,38},	// fault_bz;		故障ブザー	
	{NULL,BIT4,					CODE_PLCIO_BIT,		0,0,14},	//auto_kyusi		旋回自動給脂装置ランプ
	//B160
	{NULL,BIT0,					CODE_PLCIO_BIT,		0,0,15},	//syukan_comp_bz;	//主幹投入完了  Bz
	{NULL,BIT1,					CODE_PLCIO_BIT,		0,0,15},	//syukan_mc_comp;	//主幹MC投入完了
	{NULL,BIT2,					CODE_PLCIO_BIT,		0,0,15},	//fault_pl;			//故障ランプ
	{NULL,BIT3,					CODE_PLCIO_BIT,		0,0,15},	//syukairo_comp;    //主回路準備完了
	{NULL,BIT8,					CODE_PLCIO_BIT,		0,0,15},	//takamaki_mode;	//高巻モード
	{NULL,BIT9,					CODE_PLCIO_BIT,		0,0,15},	//bh_rest_mode;		//引込レストモード
	{NULL,BIT11,				CODE_PLCIO_BIT,		0,0,15},	//mercury_lamp_sw1;	水銀灯スイッチ1sl_fix_pl;L
	{NULL,BIT12,				CODE_PLCIO_BIT,		0,0,15},	//mercury_lamp_sw2;	水銀灯スイッチ2ah_ari_jc
	{NULL,BIT13,				CODE_PLCIO_BIT,		0,0,15},	//mercury_lamp_sw3;	水銀灯スイッチ3sl_fix_pl2;
	{NULL,BIT14,				CODE_PLCIO_BIT,		0,0,15},	//douryoku_ok;		//動力電源確立;
	{NULL,BIT15,				CODE_PLCIO_BIT,		0,0,15},	//siren_sw;			モータサイレンスイッチ

	//X090
	{NULL,BIT11,				CODE_PLCIO_BIT,		0,0,65},	//rmote_mode_sw;		//リモートモードスイッチ

	//X0C0
	{NULL,BIT9,					CODE_PLCIO_BIT,		0,0,66},	//brk_mc3_fb;		//ブレーキ主幹アンサーバック
	{NULL,BIT10,				CODE_PLCIO_BIT,		0,0,66},	//mh_brk1_fb;		//主巻ブレーキアンサーバック
	{NULL,BIT12,				CODE_PLCIO_BIT,		0,0,66},	//bh_brk_fb;		//引込ブレーキアンサーバック
	{NULL,BIT13,				CODE_PLCIO_BIT,		0,0,66},	//gt_brk_fb;		//走行ブレーキアンサーバック
	{NULL,BIT15,				CODE_PLCIO_BIT,		0,0,66},	//ah_brk_fb;		//ホイップブレーキアンサーバック

	//インバータへの指令出力内容
	{NULL,BIT0,					CODE_PLCIO_BIT,		0,0,57},//inv_fwd_mh
	{NULL,BIT1,					CODE_PLCIO_BIT,		0,0,57},//inv_rev_mh
	{NULL,BIT0,					CODE_PLCIO_BIT,		0,0,58},//inv_fwd_bh
	{NULL,BIT1,					CODE_PLCIO_BIT,		0,0,58},//inv_rev_bh
	{NULL,BIT8,					CODE_PLCIO_BIT,		0,0,58},//inv_fwd_sl
	{NULL,BIT9,					CODE_PLCIO_BIT,		0,0,58},//inv_rev_sl
	{NULL,BIT0,					CODE_PLCIO_BIT,		0,0,59},//inv_fwd_gt
	{NULL,BIT1,					CODE_PLCIO_BIT,		0,0,59},//inv_rev_gt
	{NULL,BIT0,					CODE_PLCIO_BIT,		0,0,59},//inv_fwd_ah
	{NULL,BIT1,					CODE_PLCIO_BIT,		0,0,59},//inv_rev_ah

	//インバータ速度指令
	{NULL,BITFF,				CODE_PLCIO_WORD,	0,0,70},//inv_vref_mh
	{NULL,BITFF,				CODE_PLCIO_WORD,	0,0,71},//inv_vref_bh
	{NULL,BITFF,				CODE_PLCIO_WORD,	0,0,72},//inv_vref_sl
	{NULL,BITFF,				CODE_PLCIO_WORD,	0,0,73},//inv_vref_gt
	{NULL,BITFF,				CODE_PLCIO_WORD,	0,0,73},//inv_vref_ah

	//インバータ速度FB
	{NULL,BITFF,				CODE_PLCIO_WORD,	0,0,74},//inv_vfb_mh
	{NULL,BITFF,				CODE_PLCIO_WORD,	0,0,75},//inv_vfb_bh
	{NULL,BITFF,				CODE_PLCIO_WORD,	0,0,76},//inv_vfb_sl
	{NULL,BITFF,				CODE_PLCIO_WORD,	0,0,77},//inv_vfb_gt
	{NULL,BITFF,				CODE_PLCIO_WORD,	0,0,77},//inv_vfb_ah

	//目標速度％
	{NULL,BITFF,				CODE_PLCIO_WORD,	0,0,33},//target_v_m
	{NULL,BITFF,				CODE_PLCIO_WORD,	0,0,34},//target_v_b
	{NULL,BITFF,				CODE_PLCIO_WORD,	0,0,35},//target_v_s
	{NULL,BITFF,				CODE_PLCIO_WORD,	0,0,36},//target_v_g
	{NULL,BITFF,				CODE_PLCIO_WORD,	0,0,37},//target_v_ah

	//インバータトルク指令
	{NULL,BITFF,				CODE_PLCIO_WORD,	0,0,78},//inv_trqref_mh
	{NULL,BITFF,				CODE_PLCIO_WORD,	0,0,79},//inv_trqref_bh
	{NULL,BITFF,				CODE_PLCIO_WORD,	0,0,80},//inv_trqref_ah

	//SIM　高速カウンタ　アブソコーダ
	{NULL,BITFF,				CODE_PLCIO_DWORD,	0,0,82},	//hcounter_mh
	{NULL,BITFF,				CODE_PLCIO_DWORD,	0,0,84},	//hcounter_ah
	{NULL,BITFF,				CODE_PLCIO_DWORD,	0,0,86},	//hcounter_bh
	{NULL,BITFF,				CODE_PLCIO_DWORD,	0,0,88},	//hcounter_sl

	{NULL,BITFF,				CODE_PLCIO_DWORD,	0,0,90},	//absocoder_mh
	{NULL,BITFF,				CODE_PLCIO_DWORD,	0,0,92},	//absocoder_ah
	{NULL,BITFF,				CODE_PLCIO_DWORD,	0,0,94},	//absocoder_gt
	//荷重
	{NULL,BITFF,				CODE_PLCIO_WORD,	0,0,4},//荷重
	{NULL,BITFF,				CODE_PLCIO_WORD,	0,0,2},	//m_ah 荷重
	//揚程　旋回半径
	{NULL,BITFF,				CODE_PLCIO_DWORD,	0,0,25},	//h_mh_mm
	{NULL,BITFF,				CODE_PLCIO_FLOAT,	0,0,27},	//r_bh_m
	{NULL,BITFF,				CODE_PLCIO_DWORD,	0,0,29},	//h_ah_mm

	//風速
	{ NULL,BITFF,				CODE_PLCIO_WORD,	0,0,5},	//wind_spd_01m
	//風向
	{ NULL,BITFF,				CODE_PLCIO_WORD,	0,0,31 },	//wind_dir_deg
};
ST_JC_PLC_IO_W plc_io_wdef_hhgh29 = {
	//{pi16,mask,type,lp:ビットシフト,hp：バッファサイズ}
	//PLC制御
	{NULL,BITS_WORD,			CODE_PLCIO_WORD,	0,1,0},	//pc_healthy;
	{NULL,BITS_WORD,			CODE_PLCIO_WORD,	0,1,1},	//pc_ctrl_mode;
	//運転室操作台
	//B160
	{NULL,BIT11,				CODE_PLCIO_BIT,		0,0,15},	//mercury_lamp_sw1;	水銀灯スイッチ1
	{NULL,BIT12,				CODE_PLCIO_BIT,		0,0,15},	//mercury_lamp_sw2;	水銀灯スイッチ2
	{NULL,BIT13,				CODE_PLCIO_BIT,		0,0,15},	//mercury_lamp_sw3;	水銀灯スイッチ3
	{NULL,BIT15,				CODE_PLCIO_BIT,		0,0,15},	//siren_sw;			モータサイレンスイッチ

	//B220
	{NULL,BIT0,					CODE_PLCIO_BIT,		0,0,6},	//syukan_on;
	{NULL,BIT1,					CODE_PLCIO_BIT_NC,	0,0,6},	//syukan_off;
	{NULL,BIT2 | BIT3 | BIT7,	CODE_PLCIO_BITS,	0,0,6},	//mh_spd_cs;
	{NULL,BIT4 | BIT5 | BIT6,	CODE_PLCIO_BITS,	0,0,6},	//bh_mode_cs;
	{NULL,BIT8,					CODE_PLCIO_BIT,		0,0,6},	//ah_use_sel;
	{NULL,0xFC00,				CODE_PLCIO_BITS,	10,0,6},	// ah_notch;

	//B230
	{NULL,0x003F,				CODE_PLCIO_BITS,	0,0,7},	//mh_notch;
	{NULL,BIT7 | BIT8,			CODE_PLCIO_BITS,	7,0,7},	//gt_spd_sel;	
	{NULL,0x7E00,				CODE_PLCIO_BITS,	9,0,7},	//gt_notch;	
	{NULL,BIT15,				CODE_PLCIO_BIT_NC,	0,0,7},	//estop;

	//B240
	{NULL,BIT0,					CODE_PLCIO_BIT,		0,0,8},	//ah_under_limit;補巻制限荷重以下
	{NULL,BIT1,					CODE_PLCIO_BIT,		0,0,8},	//mlim_warn_1;	ﾓｰﾒﾝﾄﾘﾐｯﾀ旋回加速切替荷重以下//ah_under_limit;
	{NULL,BIT2,					CODE_PLCIO_BIT,		0,0,8},	//mlim_warn_2;	ﾓｰﾒﾝﾄﾘﾐｯﾀ3倍速許可荷重以下//ah_under_limit;
	{NULL,BIT3,					CODE_PLCIO_BIT,		0,0,8},	//mlim_90;		ﾓｰﾒﾝﾄﾘﾐｯﾀ90%荷重//ah_under_limit;
	{NULL,BIT4,					CODE_PLCIO_BIT,		0,0,8},	//mlim_100;		ﾓｰﾒﾝﾄﾘﾐｯﾀ100%荷重//ah_under_limit;
	{NULL,BIT5,					CODE_PLCIO_BIT,		0,0,8},	//mlim_under_hmh;ﾓｰﾒﾝﾄﾘﾐｯﾀ高巻荷重以下//ah_under_limit;
	{NULL,BIT6,					CODE_PLCIO_BIT,		0,0,8},	//mlim_normal;	ﾓｰﾒﾝﾄﾘﾐｯﾀ正常//ah_under_limit;
	{NULL,BIT7,					CODE_PLCIO_BIT,		0,0,8},	//wind_over16;
	{NULL,BIT8,					CODE_PLCIO_BIT,		0,0,8},	//alarm_stp_pb;
	{NULL,BIT9,					CODE_PLCIO_BIT,		0,0,8},	//fault_reset_pb;
	{NULL,BIT10,				CODE_PLCIO_BIT,		0,0,8},	//bypass_pb;
	{NULL,BIT11,				CODE_PLCIO_BIT,		0,0,8},	//mhbk_normal_ss;
	{NULL,BIT12,				CODE_PLCIO_BIT,		0,0,8},	//mhbk_emr_ss;
	{NULL,BIT13,				CODE_PLCIO_BIT,		0,0,8},	//mhbk_opn_pb;
	{NULL,BIT14,				CODE_PLCIO_BIT,		0,0,8},	//ahbk_emr_low_ss;

	//B250
	{NULL,0x003F,				CODE_PLCIO_BITS,	0,0,9},	//bh_notch;
	{NULL, BIT6 | BIT7,			CODE_PLCIO_BITS,	0,0,9},	//ah_spd_cs;
	{NULL,BIT8,					CODE_PLCIO_BIT,		0,0,9},	//sl_brake;
	{NULL,0x7E00,				CODE_PLCIO_BITS,	9,0,9},	//sl_notch;
	{NULL,BIT15,				CODE_PLCIO_BIT,		0,0,9},	//sl_hydr_press_sw;	旋回油圧圧力スイ

	//SIM　高速カウンタ　アブソコーダ
	{NULL,BITFF,				CODE_PLCIO_DWORD,	0,0,30},	//hcounter_mh
	{NULL,BITFF,				CODE_PLCIO_DWORD,	0,0,32},	//hcounter_ah
	{NULL,BITFF,				CODE_PLCIO_DWORD,	0,0,34},	//hcounter_bh
	{NULL,BITFF,				CODE_PLCIO_DWORD,	0,0,36},	//hcounter_sl
	
	{NULL,BITFF,				CODE_PLCIO_DWORD,	0,0,38},	//absocoder_mh
	{NULL,BITFF,				CODE_PLCIO_DWORD,	0,0,40},	//absocoder_ah
	{NULL,BITFF,				CODE_PLCIO_DWORD,	0,0,42},	//absocoder_gt

	//SIM　INV 出力
	{NULL,BITFF,				CODE_PLCIO_WORD,	0,0,70},//vfb_mh;
	{NULL,BITFF,				CODE_PLCIO_WORD,	0,0,71},//vfb_bh;
	{NULL,BITFF,				CODE_PLCIO_WORD,	0,0,72},//vfb_sl;
	{NULL,BITFF,				CODE_PLCIO_WORD,	0,0,73},//vfb_gt;
	{NULL,BITFF,				CODE_PLCIO_WORD,	0,0,69},//vfb_ah;

	{NULL,BITFF,				CODE_PLCIO_WORD,	0,0,74},//trqref_mh
	{NULL,BITFF,				CODE_PLCIO_WORD,	0,0,75},//trqref_bh
	{NULL,BITFF,				CODE_PLCIO_WORD,	0,0,76},//trqref_ah

	//モーメントリミッタ
	{NULL,BITFF,				CODE_PLCIO_WORD,	0,0,4},//mlim_weight_ai	モーメントリミッタ荷重AI
	{NULL,BITFF,				CODE_PLCIO_WORD,	0,0,2},//mlim_weight_ah_ai	モーメントリミッタ荷重AI
	{NULL,BITFF,				CODE_PLCIO_WORD,	0,0,3},//mlim_r_ai	モーメントリミッタ旋回半径AI

	//風速AI
	{NULL,BITFF,				CODE_PLCIO_WORD,	0,0,5},//wind_spd_ai 
	//風向
	{NULL,BITFF,				CODE_PLCIO_WORD,	0,0,31},// wind_dir_ai 

	//OTEヘッダコマンド
	{NULL,BITFF,				CODE_PLCIO_WORD,	0,0,16},// 

	//映像遅延検出用デバイス
	{NULL,BIT0,					CODE_PLCIO_BIT,		0,0,10},	//ST_PLC_IO_DEF v_delay_device;
};

#define PLC_IF_RINDEX_RMTSW_HHGH29		65		//遠隔スイッチ情報の読み出し位置
#define PLC_IF_RMASK_RMTSW_HHGH29		0x0800	//遠隔スイッチ情報のマスク
