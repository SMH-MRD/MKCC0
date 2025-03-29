#pragma once
#include "PLC_DEF.H"
#include "SPEC_DEF.H"

namespace	H6R602 
{
	ST_SPEC			_spec;
	ST_PLC_IO_MAP 	_map;

}
namespace	HHGH29 
{
	ST_SPEC	_spec = {
		//ノッチ速度物理量
		//double notch_spd_f[MOTION_ID_MAX][NOTCH_MAX] = {	//# ノッチ指令速度（機上）1倍速時
		{	
		{ 0.0,	0.012,	0.035,	0.07,	0.117,	0.117 },			//[ID_HOIST]	m/s
		{ 0.0,	0.04,	0.125,	0.25,	0.416,	0.416 },			//[ID_GANTRY]	m/s
		{ 0.0,	0.0,	0.0,	0.0,	0.0,	0.0 },				//[ID_TROLLY]	m/s
		{ 0.0,	0.021,	0.063,	0.125,	0.208,	0.208 },			//[ID_BOOM_H]	m/s
		{ 0.0,	0.003,	0.011,	0.021,	0.035,	0.035 },			//[ID_SLEW]		rad/s;
		{  0.0,	0.02,	0.06,	0.12,	0.2,	0.2 },				//[ID_AHOIST]	m/s
		{ 0.0,	0.0,	0.0,	0.0,	0.0,	0.0 },				//[ID_H_ASSY];
		{ 0.0,	0.0,	0.0,	0.0,	0.0,	0.0 },				//[ID_MOTION1];
		},

		//double notch_spd_r[MOTION_ID_MAX][NOTCH_MAX] = {  //# ノッチ指令速度（機上）1倍速時
		{			
		{ 0.0,	-0.012,	-0.35,	-0.07,	-0.117,	-0.117 },			//[ID_HOIST]	m/s
		{ 0.0,	-0.04,	-0.125,	-0.25,	-0.416,	-0.416 },			//[ID_GANTRY]	m/s
		{ 0.0,	0.0,	0.0,	0.0,	0.0,	0.0 },				//[ID_TROLLY]	m/s
		{ 0.0,	-0.021,	-0.063,	-0.125,	-0.208,	-0.208 },			//[ID_BOOM_H]	m/s
		{  0.0,	-0.003,	-0.011,	-0.021,	-0.035,	-0.035  },			//[ID_SLEW]		rad/s;
		{ 0.0,	-0.02,	-0.06,	-0.12,	-0.2,	-0.2  },			//[ID_AHOIST]	m/s
		{ 0.0,	0.0,	0.0,	0.0,	0.0,	0.0 },				//[ID_H_ASSY];
		{ 0.0,	0.0,	0.0,	0.0,	0.0,	0.0 },				//[ID_MOTION1];
		},
		//ノッチ速度％
		//double notch_spd_nrm_f[MOTION_ID_MAX][NOTCH_MAX] = {		//# ノッチ指令速度（正規化）%
		{
		{ 0.0,	0.1,	0.3,	0.6,	1.0,	1.0 },				//[ID_HOIST]	%
		{ 0.0,	0.1,	0.3,	0.6,	1.0,	1.0 },				//[ID_GANTRY]	%
		{ 0.0,	0.0,	0.0,	0.0,	0.0,	0.0 },				//[ID_TROLLY]	%
		{ 0.0,	0.1,	0.3,	0.6,	1.0,	1.0 },				//[ID_BOOM_H]	%
		{ 0.0,	0.1,	0.3,	0.6,	1.0,	1.0 },				//[ID_SLEW]		%
		{ 0.0,	0.1,	0.3,	0.6,	1.0,	1.0 },				//[ID_AHOIST]	%
		{ 0.0,	0.0,	0.0,	0.0,	0.0,	0.0 },				//[ID_H_ASSY];
		{ 0.0,	0.0,	0.0,	0.0,	0.0,	0.0 },				//[ID_MOTION1];
		},
		//double notch_spd_nrm_r[MOTION_ID_MAX][NOTCH_MAX] = {			//# ノッチ指令速度（正規化）%
		{
		{ 0.0,	-0.1,	-0.3,	-0.6,	-1.0,	-1.0 },				//[ID_HOIST]	%
		{ 0.0,	-0.1,	-0.3,	-0.6,	-1.0,	-1.0},				//[ID_GANTRY]	%
		{ 0.0,	0.0,	0.0,	0.0,	0.0,	0.0 },				//[ID_TROLLY]	%
		{ 0.0,	-0.1,	-0.3,	-0.6,	-1.0,	-1.0  },			//[ID_BOOM_H]	%
		{ 0.0,	-0.1,	-0.3,	-0.6,	-1.0,	-1.0 },				//[ID_SLEW]		%
		{ 0.0,	-0.1,	-0.3,	-0.6,	-1.0,	-1.0  },			//[ID_AHOIST]	%
		{ 0.0,	0.0,	0.0,	0.0,	0.0,	0.0 },				//[ID_H_ASSY];
		{ 0.0,	0.0,	0.0,	0.0,	0.0,	0.0 },				//[ID_MOTION1];
		},
		//速度設定倍率パターン
		//double notch_spd_retio[MOTION_ID_MAX][N_NOTCH_RATIO] = {	//倍速パターン
		{
		{ 1.0,2.0,3.0,1.0},		//[ID_HOIST]	1倍　2倍　3倍
		{ 1.0,1.0,1.0,1.0},		//[ID_GANTRY]	
		{ 1.0,1.0,1.0,1.0},		//[ID_TROLLY]	
		{ 1.0,1.0,1.0,1.0},		//[ID_BOOM_H]	
		{ 1.0,0.75,1.0,1.0},	//[ID_SLEW]		1倍　0.75倍	
		{ 1.0,2.0,1.0,1.0},		//[ID_AHOIST]	1倍　2倍	
		{ 1.0,1.0,1.0,1.0},		//[ID_H_ASSY];
		{ 1.0,1.0,1.0,1.0}		//[ID_MOTION1];
		},
		//加減速度 m/s2,rad/s2
		//double accdec[MOTION_ID_MAX][DIRECTION_MAX][ACCDEC_MAX] = {	//# 各動作加減速度
		{
		{{ 0.078,	-0.078},	{-0.078,	0.078 }},				//[ID_HOIST]	m/s2
		{{ 0.0405,	-0.0405},	{-0.0405,	0.0405 }},				//[ID_GANTRY]	m/s2
		{{ 0.0,		-0.0},		{-0.0,		0.0 }},					//[ID_TROLLY]	m/s2
		{{ 0.1387,	-0.11387},	{-0.11387,	0.11387 }},				//[ID_BOOM_H]	m/s2
		{{ 0.00389,	-0.00389},	{-0.00389,	0.00389 }},				//[ID_SLEW]		rad/s2;
		{{ 0.133,	-0.133},	{-0.133,	0.133 }},				//[ID_AHOIST]	m/s2
		{{ 0.0,	0.0},			{0.0,	0.0 }},						//[ID_H_ASSY];
		{{ 0.0,	0.0},			{0.0,	0.0 }},						//[ID_MOTION1];
		},
		//軸動作定格
		//double prm_drv_rate[DRIVE_ITEM_NUM][MOTION_ID_MAX]{
		//	HST,		GNT,		BHMH,		BH,			SLW,		AH,			ASSY,	COM
		{		
				{12.5,		28.333,		0.0,		29.1667,	29.1667,	16.667,		0.0,	0.0},	// モータ定格回転数（rps　100%）
				{0.007823,	0.011234,	0.0,		0.004399,	0.002336,	0.008418,	0.0,	0.0},	// 減速比(ドラム回転/モータ回転）
				{1.5,		8.0,		0.0,		1.5,		9.0,		1.5,		0.0,	0.0},	// 加速時間（100%)
				{0.1170,	0.5,		0.0,		0.2080,		0.035,		0.2000,		0.0,	0.0},	// 定格速度（m/s, rad/s：100％)
				{0.09775,	0.3183,		0.0,		0.1283,		0.06815,	0.1403,		0.0,	0.0},	// 定格ドラム回転速度（100％rps)
				{0.065,		0.0398,		0.0,		0.8553,		0.007572,	0.0935,		0.0,	0.0},	// 定格ドラム回転加速度（rps2)
				{3.0,		1.0,		1.0,		1.0,		1.0,		2.0,		0.0,	0.0},	// ドラム最大回転数比率（100%->1.0)
				{750.0,		1700.0,		0.0,		1750.0,		1750.0,		1000.0,		0.0,	0.0}	// モータ定格回転数（rps　100%）
		},
		//軸動作極限
		//double pos_limit[MOTION_ID_MAX][DIRECTION_MAX][POS_LIMIT_TYPE_MAX] = {//# 極限寸法	
		{
		{{ 79.2,0.0,0.0,0.0},	{ -5.8,0.0,0.0,0.0}},				//[ID_HOIST]
		{{ 28.3,0.0,0.0,0.0},	{ 0.1,0.0,0.0,0.0}},				//[ID_GANTRY]
		{{ 0.0,0.0,0.0,0.0},	{ 0.0,0.0,0.0,0.0}},				//[ID_TROLLY]
		{{ 67.0,0.0,0.0,0.0},	{ 20.5,0.0,0.0,0.0}},				//[ID_BOOM_H]
		{{ 0.0,0.0,0.0,0.0},	{ 0.0,0.0,0.0,0.0}},				//[ID_SLEW]
		{{ 88.2,0.0,0.0,0.0},	{-5.65,0.0,0.0,0.0}},				//[ID_AHOIST]
		{{ 0.0,0.0,0.0,0.0},	{ 0.0,0.0,0.0,0.0}},				//[ID_H_ASSY]
		{{ 0.0,0.0,0.0,0.0},	{ 0.0,0.0,0.0,0.0}},				//[ID_MOTION1]
		},
		//ST_STRUCTURE 構造寸法
		{
		20.0,//double wheel_span = ;	// ホイルスパン
		15.0,//double leg_span = ;		// 脚間スパン
		47.7,//double Hp = ;			// ブーム水平時高さ
		64.009,//double Lb = ;			// ブーム長さ
		//
		63.000,//double Lm = ;			//ジブpin-主巻シーブ中心距離
		4.981,//double La_add = ;		//ジブ主補巻シーブ間距離
		2.371,//double rad_mh_ah = ;	//ジブ主補巻シーブ間角度
		66.566,//double La = ;			//ジブpin-補巻シーブ中心距離
		0.0537,//double rad_Lm_La = ;	//ジブ主補巻シーブ間角度
		1.676,//double Lmb = ;
		4.981,//double Lma = ;
		5.512,//double Lba = ;
		2.00301,//double Th_pba = ;
		0.90266,//double Alpa_db = ;
		0.02109,//double Alpa_b = ;	//ジブ主巻軸－起伏軸間角度
		0.05373,//double Alpa_a = ;	//ジブ補巻軸－主巻軸間角度
		2.20547,//double Phbm = ;
		25.174,//double Lp = ;
		1.6583,//double Php = ;
		2.9000,//double Lmf = ;
		2.2910,//double Laf = ;
		//
		-0.7113,//double Th_rest = ;	//ジブレスト時の起伏角
		41.425,//double D_min = ;	//最小半径時の起伏シーブ間距離
		83.905,//double D_rest = ;	//ジブレスト時の起伏シーブ間距離
		59.397,//double Hb_min = ;	//最小半径時の起伏高さ
		-41.786,//double Hb_rest = ;	//ジブレスト時起伏高さ
		3.652,//double Dttb = ;	//TTB径
		0.08434,//double Kttb = ;	//ピニオン径/TTB径
		67.974,//double Nbh_drum = ;	//引込ドラム回転数の和（引込＋主巻）
		11000.0,//double Load0_mh = ;	//主巻吊具荷重
		1900.0//double Load0_ah = ;	//補巻吊具荷重 
		},
		//制御パラメータ
		//# 振れ止め判定　振れ角レベル(rad)
		//double as_rad_level[NUM_OF_AS_AXIS][NUM_OF_SWAY_LEVEL] = 
		{
		{ 0.0,	0.0, 0.0},											//[ID_HOIST]
		{ 0.005, 0.01, 0.020 },									//[ID_GANTRY]
		{ 0.005, 0.01, 0.020 },									//[ID_TROLLY]
		{ 0.005, 0.01, 0.020 },									//[ID_BOOM_H]
		{ 0.005, 0.01, 0.020 }									//[ID_SLEW]	
		},
		//# 振れ止め判定　振れ角レベル(rad^2)
		//double as_rad2_level[NUM_OF_AS_AXIS][NUM_OF_SWAY_LEVEL] = 
		{
		{ 0.0,	0.0, 0.0},											//[ID_HOIST]
		{ 0.000025, 0.0001, 0.0004 },								//[ID_GANTRY]
		{ 0.000025, 0.0001, 0.0004 },								//[ID_TROLLY]
		{ 0.000025, 0.0001, 0.0004 },								//[ID_BOOM_H]
		{ 0.000025, 0.0001, 0.0004 },								//[ID_SLEW]
		},
		//# 振れ止め判定　振れ幅レベル(m)
		//double as_m_level[NUM_OF_AS_AXIS][NUM_OF_SWAY_LEVEL] = 
		{
		{ 0.0,	0.0, 0.0},											//[ID_HOIST]
		{ 0.1, 0.2, 2.00 },											//[ID_GANTRY]
		{ 0.1, 0.2, 2.00 },											//[ID_TROLLY]
		{ 0.1, 0.2, 2.00 },											//[ID_BOOM_H]
		{ 0.1, 0.2, 2.00 },											//[ID_SLEW]	
		},
		//# 振れ止め判定　振れ幅レベル(m^2)
		//double as_m2_level[NUM_OF_AS_AXIS][NUM_OF_SWAY_LEVEL] = 
		{
		{ 0.0,	0.0, 0.0},											//[ID_HOIST]
		{ 0.01, 0.04, 4.0 },										//[ID_GANTRY]
		{ 0.01, 0.04, 4.0 },										//[ID_TROLLY]
		{ 0.01, 0.04, 4.0 },										//[ID_BOOM_H]
		{ 0.01, 0.04, 4.0 },										//[ID_SLEW]
		},
		//# 位置決め制御判定　位置ずれレベル(m,rad)
		//double as_pos_level[NUM_OF_AS_AXIS][NUM_OF_POSITION_LEVEL] = 
		{
		{ 0.03, 0.06, 0.50 },										//m[ID_HOIST]
		{ 0.03, 0.06, 0.50 },										//m[ID_GANTRY]
		{ 0.03, 0.06, 0.50 },										//m[ID_TROLLY]
		{ 0.03, 0.06, 0.5 },										//m[ID_BOOM_H]
		{ 0.005, 0.01, 0.1 }										//rad[ID_SLEW]
		},
		//# 位置到達判定範囲(m,rad) 
		//double pos_check_limit[MOTION_ID_MAX][NUM_OF_POSITION_LEVEL] = 
		{
			{ 0.1, 0.2, 0.5 },											//m[ID_HOIST]
			{ 0.1, 0.2, 0.5 },											//m[ID_GANTRY]
			{ 0.1, 0.2, 0.5 },											//m[ID_TROLLY]
			{ 0.1, 0.2, 0.5 },											//m[ID_BOOM_H]
			{ 0.005, 0.17, 0.017},										//rad[ID_SLEW]
			{ 0.0, 0.0, 0.0 },											//
			{ 0.0, 0.0, 0.0 },											//
			{ 0.0, 0.0, 0.0 },											//
		},
		//# 遅れ時間,無駄時間定義
		//double delay_time[NUM_OF_AS_AXIS][NUM_OF_DELAY_PTN] = 
		{		// 加減速時のFB一時遅れ時定数
			{ 0.3,0.3,0.3,0.3,0.3},										//[ID_HOIST]
			{ 0.3,0.3,0.3,0.3,0.3 },									//[ID_GANTRY]
			{ 0.3,0.3,0.3,0.3,0.3 },									//[ID_TROLLY]
			{ 0.3,0.3,0.3,0.3,0.3 },									//[ID_BOOM_H]
			{ 0.3,0.3,0.3,0.3,0.3 },									//[ID_SLEW]
		},
		//# 半自動目標位置
		//double semi_target[SEMI_AUTO_TARGET_MAX][MOTION_ID_MAX]
		{//半自動デフォルト目標位置
			//	HST,	GNT,	TRY,	BH,		SLW,	OPR,	ASSY,	COM
				{63.4,	0.0,	0.0,	56.4,	0.0,	63.5,	0.0,	0.0},//配列0は最終設定値
				{63.4,	0.0,	0.0,	56.4,	0.0,	63.5,	0.0,	0.0},
				{20.0,	0.0,	0.0,	50.0,	1.5,	63.5,	0.0,	0.0},
				{40.0,	0.0,	0.0,	56.4,	-1.25,	63.5,	0.0,	0.0},
				{63.4,	0.0,	0.0,	56.4,	0.0,	63.5,	0.0,	0.0},
				{63.4,	0.0,	0.0,	56.4,	0.0,	63.5,	0.0,	0.0},
				{63.4,	0.0,	0.0,	56.4,	0.0,	63.5,	0.0,	0.00},
				{63.4,	0.0,	0.0,	56.4,	0.0,	63.5,	0.0,	0.0}
		},
		//# ワイヤ関連パラメータ
		//double prm_nw[WIRE_ITEM_NUM][MOTION_ID_MAX]
		{
			//	HST,	GNT,	BHMH,	BH,		SLW,	AH,		ASSY,	COM
				{4.0,	0.0,	4.0,	6.0,	0.0,	2.0,	0.0,	0.0},	// ワイヤ掛数
				{2.0,	0.0,	2.0,	6.0,	0.0,	4.0,	0.0,	0.0},	// ワイヤ掛数ブーム部
				{4.0,	0.0,	6.0,	4.0,	0.0,	5.0,	0.0,	0.0},	// あだ巻数
				{15.0,	1.0,	10.0,	12.0,	1.0,	15.0,	0.0,	0.0},	// ドラム溝数
				{1.200,	0.50,	1.200,	1.1925,	0.308,	0.750,	0.0,	0.0},	// １層ドラム径
				{0.05,	0.0,	0.05,	0.0425,	0.0,	0.028,	0.0,	0.0},	// ワイヤ径
				{0.0866,0.0,	0.0866,	0.0736,	0.0,	0.0485,	0.0,	0.0},	// ドラム径層加算値
				{563.598,1.571,	563.598,525.906,0.968,	402.636,0.0,	0.0}	// 基準ワイヤ長(旋回はTTB円周
		}
	};

#if 0

	ST_PLC_IO_MAP 	_map = {
	//CABPLC_BOUT_MAP     mp_cab_Bout;
	//struct CABPLC_BOUT_MAP 
	{

	},
	//CABPLC_BIN_MAP      mp_cab_Bin;
	//struct CABPLC_BIN_MAP 
	{
		//B220-B26F
		////POINT ctrl_on			= 
		{ ID_OPEPLC_B220,0x0001 },
		////POINT ctrl_off			= 
		{ ID_OPEPLC_B220,0x0002 },
		////POINT mh_spd_low		= 
		{ ID_OPEPLC_B220,0x0004 },
		////POINT mh_spd_middle		= 
		{ ID_OPEPLC_B220,0x0008 },
		////POINT bh_normal			= 
		{ ID_OPEPLC_B220,0x0010 },
		////POINT bh_down			= 
		{ ID_OPEPLC_B220,0x0020 },
		////POINT bh_rest			= 
		{ ID_OPEPLC_B220,0x0040 },
		////POINT mh_high_spd		= 
		{ ID_OPEPLC_B220,0x0080 },
		////POINT ah_multi			= 
		{ ID_OPEPLC_B220,0x0100 },
		////POINT ah_single			= 
		{ ID_OPEPLC_B220,0x0200 },
		////POINT notch_ah			= 
		{ ID_OPEPLC_B220,10 },     //ノッチのYはビットシフト量
		////POINT notch_mh			= 
		{ ID_OPEPLC_B230,0 },       //ノッチのYはビットシフト量
		////POINT load_swayless		= 
		{ ID_OPEPLC_B230,0x0040 },
		////POINT gt_spd_30			= 
		{ ID_OPEPLC_B230,0x0080 },
		////POINT gt_spd_35			= 
		{ ID_OPEPLC_B230,0x0100 },
		////POINT notch_gt			= 
		{ ID_OPEPLC_B230,9 },      //ノッチのYはビットシフト量
		////POINT cab_estp			= 
		{ ID_OPEPLC_B230,0x8000 },
		////POINT whip_5t_ng		= 
		{ ID_OPEPLC_B240,0x0001 },
		////POINT mlim_acc_low		= 
		{ ID_OPEPLC_B240,0x0002 },
		////POINT mlim_3times		= 
		{ ID_OPEPLC_B240,0x0004 },
		////POINT mlim_load_90		= 
		{ ID_OPEPLC_B240,0x0008 },
		////POINT mlim_load_100		= 
		{ ID_OPEPLC_B240,0x0010 },
		////POINT mlim_high_load	= 
		{ ID_OPEPLC_B240,0x0020 },
		////POINT mlim_normal		= 
		{ ID_OPEPLC_B240,0x0040 },
		////POINT wind_spd16		= 
		{ ID_OPEPLC_B240,0x0080 },
		////POINT bz_stop			= 
		{ ID_OPEPLC_B240,0x0100 },
		////POINT fault_reset		= 
		{ ID_OPEPLC_B240,0x0200 },
		////POINT il_bypass			= 
		{ ID_OPEPLC_B240,0x0400 },
		////POINT mhbrk_opn_normal	= 
		{ ID_OPEPLC_B240,0x0800 },
		////POINT mhbrk_opn_abnormal= 
		{ ID_OPEPLC_B240,0x1000 },
		////POINT mh_emr_open		= 
		{ ID_OPEPLC_B240,0x2000 },
		////POINT ah_emr_lower		= 
		{ ID_OPEPLC_B240,0x4000 },
		////POINT trace				= 
		{ ID_OPEPLC_B240,0x8000 },
		////POINT notch_bh			= 
		{ ID_OPEPLC_B250,0 },//ノッチのYはビットシフト量
		////POINT ah_sel_low_spd	= 
		{ ID_OPEPLC_B250,0x0040 },
		////POINT ah_sel_high_spd	= 
		{ ID_OPEPLC_B250,0x0080 },
		////POINT foot_brk			= 
		{ ID_OPEPLC_B250,0x0100 },
		////POINT notch_sl			= 
		{ ID_OPEPLC_B250,9 },//ノッチのYはビットシフト量
		////POINT siren				= 
		{ ID_OPEPLC_B260,0x0001 },
		////POINT camera_ghange		= 
		{ ID_OPEPLC_B260,0x0002 },
		////POINT slew_fix			= 
		{ ID_OPEPLC_B260,0x0004 },
		////POINT camera_sel_east	= 
		{ ID_OPEPLC_B260,0x0008 },
		////POINT camera_sel_conj	= 
		{ ID_OPEPLC_B260,0x0010 },
		////POINT camera_sel_west	= 
		{ ID_OPEPLC_B260,0x0020 },
		////POINT high_pow_ON		= 
		{ ID_OPEPLC_B260,0x0040 },
		////POINT high_pow_OFF		= 
		{ ID_OPEPLC_B260,0x0080 },
		////POINT slew_notch5		= 
		{ ID_OPEPLC_B260,0x0100 },
		////POINT jib_approch_wide	= 
		{ ID_OPEPLC_B260,0x0200 },
		////POINT jib_approch_narrow= 
		{ ID_OPEPLC_B260,0x0400 },
		////POINT spare0			= 
		{ ID_OPEPLC_B260,0x0800 },
		////POINT jib_approch_stop	= 
		{ ID_OPEPLC_B260,0x1000 },
		////POINT gt_warm_shock_on	= 
		{ ID_OPEPLC_B260,0x2000 },
		////POINT gt_warm_shock_off = 
		{ ID_OPEPLC_B260,0x4000 },
		////POINT ah_high_pos_load	= 
		{ ID_OPEPLC_B260,0x8000 }
	},
	//ERMPLC_M900_MAP     mp_M900,
	//struct ERMPLC_M900_MAP 
	{
		//SIM用追加信号
		////POINT ctrl_source_cut	= 
		{ ID_OPEPLC_M900,0x0001 },
		////POINT mh1_source_com	= 
		{ ID_OPEPLC_M900,0x0010 },
		////POINT mh2_source_com	= 
		{ ID_OPEPLC_M900,0x0020 },
		////POINT ah_source_com		= 
		{ ID_OPEPLC_M900,0x0040 },
		////POINT gt_source_com		= 
		{ ID_OPEPLC_M900,0x0080 },
		////POINT sl_source_com		= 
		{ ID_OPEPLC_M900,0x0100 },
		////POINT bh_source_com		= 
		{ ID_OPEPLC_M900,0x0200 },
		////POINT ctrl_source_comple= 
		{ ID_OPEPLC_M900,0x0400 },
		////POINT ctrl_source_pls	= 
		{ ID_OPEPLC_M900,0x0800 },
		////POINT common_source_com = 
		{ ID_OPEPLC_M900,0x1000 },
		////POINT each_source_com	= 
		{ ID_OPEPLC_M900,0x2000 }
	},
	//ERMPLC_BOUT_MAP     mp_erm_Bout,
	//struct ERMPLC_BOUT_MAP 
	{
		//B0-B1F
		////POINT cycle_bit						= 
		{ 0,0x0001 },
		////POINT cycle_bit_remote				= 
		{ 0,0x0002 },
		////POINT fault_bz						= 
		{ 0,0x0080 },
		////POINT concurrent_limit_area			= 
		{ 0,0x0100 },
		////POINT il_bypass						= 
		{ 0,0x0200 },
		////POINT inv_fault_time_on				= 
		{ 0,0x0800 },
		////POINT fault_item_detect				= 
		{ 1,0x0001 },
		////POINT mh_edl_start					= 
		{ 1,0x0020 },
		////POINT ah_edl_start					= 
		{ 1,0x0040 },
		//B0-B1F
		////POINT got_useful_func				= 
		{ 2,0x0001 },
		////POINT hook_pos_in_hlimit			= 
		{ 2,0x0002 },
		////POINT hook_pos_in_high_pos			= 
		{ 2,0x0004 },
		////POINT hook_pos_in_under_hlimit		= 
		{ 2,0x0008 },
		////POINT hook_pos_bh_down_over			= 
		{ 2,0x0010 },
		////POINT hook_pos_bh_down_under		= 
		{ 2,0x0020 },
		////POINT hook_pos_bh_down_normal		= 
		{ 2,0x0040 },
		////POINT mh_high1_con_up_slow_area		= 
		{ 2,0x0080 },
		////POINT mh_high1_con_up_stop_area		= 
		{ 2,0x0100 },
		////POINT mh_normal_up_slow_area		= 
		{ 2,0x0200 },
		////POINT mh_normal_con_up_slow1_area	= 
		{ 2,0x0400 },
		////POINT mh_normal_con_up_slow2_area	= 
		{ 2,0x0800 },
		////POINT mh_normal_con_up_stop_area	= 
		{ 2,0x1000 },
		////POINT ctrl_source_ok_bz				= 
		{ 4,0x0001 },
		////POINT ctrl_source_mc_ok				= 
		{ 4,0x0002 },//主幹ON
		////POINT fault_pl						= 
		{ 4,0x0004 },
		////POINT ctrl_standby					= 
		{ 4,0x0008 },//主幹条件成立
		////POINT load_disp_ok					= 
		{ 4,0x0010 },
		////POINT high_pos_operation_mode		= 
		{ 4,0x0100 },
		////POINT bh_rest_mode					= 
		{ 4,0x0200 },
		////POINT bh_fixed_pl					= 
		{ 4,0x0400 },
		////POINT ah_active_jc					= 
		{ 4,0x1000 },
		////POINT bh_fixed_pl2					= 
		{ 4,0x2000 },
		////POINT motive_power_ok				= 
		{ 4,0x4000 }
	},
	//ERMPLC_YOUT_MAP     mp_emr_Yout,
	//struct ERMPLC_YOUT_MAP 
	{
		//D0-F0
		//POINT il_0notch					= 
		{ 0,0x0001 },
		//POINT mh_brk_mc					= 
		{ 0,0x0002 },
		//POINT ah_brk_mc					= 
		{ 0,0x0004 },
		//POINT bh_brk_mc					= 
		{ 0,0x0008 },
		//POINT gt_brk_mc					= 
		{ 0,0x0010 },
		//POINT multi_trans_reset			= 
		{ 0,0x0020 },
		//POINT inv_reel_preset				= 
		{ 0,0x0800 },
		//POINT inv_reel_power_on			= 
		{ 0,0x1000 },
		//POINT inv_reel_gt_start			= 
		{ 0,0x2000 },
		//POINT inv_reel_fault_reset		= 
		{ 0,0x4000 },
		//POINT mh_fan_mc					= 
		{ 2,0x0001 },
		//POINT bh_fan_mc					= 
		{ 2,0x0002 },
		//POINT db_convertor_fan_mc			= 
		{ 2,0x0004 },
		//POINT ah_fan_mc					= 
		{ 2,0x0008 },
		//POINT gt_alarm					= 
		{ 2,0x0010 },
		//POINT initial_charge				= 
		{ 2,0x0020 }
	},
	//ERMPLC_UXIN_MAP     mp_emr_UXin,
	//struct ERMPLC_UXIN_MAP 
	{
		//X20
		//POINT hcnt_mh_ah_ready	= 
		{ ID_ERMPLC_X20,0x0001 },
		//X30
		//POINT hcnt_bh_sl_ready	= 
		{ ID_ERMPLC_X30,0x0001 },
		//X40
		//POINT abs_mh_ready		= 
		{ ID_ERMPLC_X40,0x0002 },
		//X120
		//POINT abs_ah_ready		= 
		{ ID_ERMPLC_X120,0x0002 }
	},

	//ERMPLC_XIN_MAP      mp_emr_Xin,
	//struct ERMPLC_XIN_MAP 
	{
		//X60-C0
		//POINT mh_high_area_emr_up_lim		= 
		{ ID_ERMPLC_X60,0x0001 },
		//POINT spare0						= 
		{ ID_ERMPLC_X60,0x0002 },
		//POINT spare1						= 
		{ ID_ERMPLC_X60,0x0004 },
		//POINT mh_normal_area_emr_up_lim		= 
		{ ID_ERMPLC_X60,0x0008 },
		//POINT bh_down_area_emr_up_lim		= 
		{ ID_ERMPLC_X60,0x0010 },
		//POINT mh_emr_lower_lim				= 
		{ ID_ERMPLC_X60,0x0020 },
		//POINT bh_emr_in_lim					= 
		{ ID_ERMPLC_X60,0x0040 },
		//POINT bh_normal_in_lim				= 
		{ ID_ERMPLC_X60,0x0080 },
		//POINT slw_spd_change				= 
		{ ID_ERMPLC_X60,0x0100 },
		//POINT spare2						= 
		{ ID_ERMPLC_X60,0x0200 },
		//POINT spare3						= 
		{ ID_ERMPLC_X60,0x0400 },
		//POINT high_pos_emr_out_lim			= 
		{ ID_ERMPLC_X60,0x0800 },
		//POINT bh_emr_out_lim				= 
		{ ID_ERMPLC_X60,0x1000 },
		//POINT bh_down_area_out_lim			= 
		{ ID_ERMPLC_X60,0x2000 },
		//POINT bh_rest_slow					= 
		{ ID_ERMPLC_X60,0x4000 },
		//POINT bh_rest_lim					= 
		{ ID_ERMPLC_X60,0x8000 },
		//X70
				
		//POINT muliti_transmit_ok			= 
		{ ID_ERMPLC_X70,0x0001 },
		//POINT spare4						= 
		{ ID_ERMPLC_X70,0x0002 },
		//POINT gt_anticol_stop				= 
		{ ID_ERMPLC_X70,0x0004 },
		//POINT gt_anticol_alarm				= 
		{ ID_ERMPLC_X70,0x0008 },
		//POINT gt_anticol_fault				= 
		{ ID_ERMPLC_X70,0x0010 },
		//POINT gt_east_lim_slow				= 
		{ ID_ERMPLC_X70,0x0020 },
		//POINT gt_east_lim_stop				= 
		{ ID_ERMPLC_X70,0x0040 },
		//POINT gt_west_lim_slow				= 
		{ ID_ERMPLC_X70,0x0080 },
		//POINT gt_west_lim_stop				= 
		{ ID_ERMPLC_X70,0x0100 },
		//POINT gt_fix_open					= 
		{ ID_ERMPLC_X70,0x0200 },
		//POINT gt_motor_thermal_trip			= 
		{ ID_ERMPLC_X70,0x0400 },
		//POINT spare5						= 
		{ ID_ERMPLC_X70,0x0800 },
		//POINT mh_motor_fan_mc				= 
		{ ID_ERMPLC_X70,0x1000 },
		//POINT ah_motor_fan_mc				= 
		{ ID_ERMPLC_X70,0x2000 },
		//POINT bh_motor_fan_mc				= 
		{ ID_ERMPLC_X70,0x4000 },
		//POINT DB_conv_fan_mc				= 
		{ ID_ERMPLC_X70,0x8000 },
		//X80
				
		//POINT DB_over_heat					= 
		{ ID_ERMPLC_X80,0x0001 },
		//POINT mh_pos_preset					= 
		{ ID_ERMPLC_X80,0x0002 },
		//POINT mh_data_preset				= 
		{ ID_ERMPLC_X80,0x0004 },
		//POINT slw_panel_high_temp			= 
		{ ID_ERMPLC_X80,0x0008 },
		//POINT leg_emr_stop					= 
		{ ID_ERMPLC_X80,0x0010 },
		//POINT slew_pos_0					= 
		{ ID_ERMPLC_X80,0x0020 },
		//POINT slew_pos_180					= 
		{ ID_ERMPLC_X80,0x0040 },
		//POINT slew_grease_active			= 
		{ ID_ERMPLC_X80,0x0080 },
		//POINT slew_grease_fault				= 
		{ ID_ERMPLC_X80,0x0100 },
		//POINT ah_camlim_high_area_emr_up	= 
		{ ID_ERMPLC_X80,0x0200 },
		//POINT ah_camlim_normal_area1_emr_up = 
		{ ID_ERMPLC_X80,0x0400 },
		//POINT ah_camlim_normal_area2_emr_up = 
		{ ID_ERMPLC_X80,0x0800 },
		//POINT ah_camlim_bh_down_area_emr_up = 
		{ ID_ERMPLC_X80,0x1000 },
		//POINT ah_camlim_emr_low				= 
		{ ID_ERMPLC_X80,0x2000 },
		//POINT spare6						= 
		{ ID_ERMPLC_X80,0x4000 },
		//POINT spare7						= 
		{ ID_ERMPLC_X80,0x8000 },
		//X90
			
		//POINT conv_1_trip					= 
		{ ID_ERMPLC_X90,0x0001 },
		//POINT inv_ctrl_trip					= 
		{ ID_ERMPLC_X90,0x0002 },
		//POINT brk_control_source_trip		= 
		{ ID_ERMPLC_X90,0x0004 },
		//POINT DB_fan_trip					= 
		{ ID_ERMPLC_X90,0x0008 },
		//POINT motor_fan_trip				= 
		{ ID_ERMPLC_X90,0x0010 },
		//POINT spare8						= 
		{ ID_ERMPLC_X90,0x0020 },
		//POINT slw_grease_trip				= 
		{ ID_ERMPLC_X90,0x0040 },
		//POINT spare9						= 
		{ ID_ERMPLC_X90,0x0080 },
		//POINT aero_panel_fault				= 
		{ ID_ERMPLC_X90,0x0100 },
		//POINT remote_trace					= 
		{ ID_ERMPLC_X90,0x0200 },
		//POINT inv_reel_battery_low			= 
		{ ID_ERMPLC_X90,0x0400 },
		//POINT inv_reel_gt_ok				= 
		{ ID_ERMPLC_X90,0x0800 },
		//POINT inv_reel_standby				= 
		{ ID_ERMPLC_X90,0x1000 },
		//POINT inv_reel_inv_normal			= 
		{ ID_ERMPLC_X90,0x2000 },
		//POINT spare10						= 
		{ ID_ERMPLC_X90,0x4000 },
		//POINT inv_reel_escape_enable		= 
		{ ID_ERMPLC_X90,0x8000 },
		//XA0
			
		//POINT motive_power_ok				= 
		{ ID_ERMPLC_XA0,0x0001 },
		//POINT mc0							= 
		{ ID_ERMPLC_XA0,0x0002 },
		//POINT conv2_mc						= 
		{ ID_ERMPLC_XA0,0x0004 },
		//POINT ctrl_brk_mc					= 
		{ ID_ERMPLC_XA0,0x0008 },
		//POINT mh_brk_mc						= 
		{ ID_ERMPLC_XA0,0x0010 },
		//POINT bh_brk_mc						= 
		{ ID_ERMPLC_XA0,0x0020 },
		//POINT gt_brk_mc						= 
		{ ID_ERMPLC_XA0,0x0040 },
		//POINT ah_brk_mc						= 
		{ ID_ERMPLC_XA0,0x0080 },
		//POINT dbu_charge_mc					= 
		{ ID_ERMPLC_XA0,0x0100 },
		//POINT spare11						= 
		{ ID_ERMPLC_XA0,0x0200 },
		//POINT jib_approch_alarm_enable		= 
		{ ID_ERMPLC_XA0,0x0400 },
		//POINT jib_approch_alarm_disable		= 
		{ ID_ERMPLC_XA0,0x0800 },
		//POINT conv1_over_heat				= 
		{ ID_ERMPLC_XA0,0x1000 },
		//POINT conv2_fuse_down				= 
		{ ID_ERMPLC_XA0,0x2000 },
		//POINT conv2_over_heat				= 
		{ ID_ERMPLC_XA0,0x4000 },
		//POINT conv1_fuse_down				= 
		{ ID_ERMPLC_XA0,0x8000 },
		//XB0
			
		//POINT thermal_trip_ah_brk			= 
		{ ID_ERMPLC_XB0,0x0001 },
		//POINT thermal_trip_bh_motor			= 
		{ ID_ERMPLC_XB0,0x0002 },
		//POINT thermal_trip_mh_fan			= 
		{ ID_ERMPLC_XB0,0x0004 },
		//POINT thermal_trip_bh_fan			= 
		{ ID_ERMPLC_XB0,0x0008 },
		//POINT thermal_trip_DB_fan			= 
		{ ID_ERMPLC_XB0,0x0010 },
		//POINT thermal_trip_mh_brk			= 
		{ ID_ERMPLC_XB0,0x0020 },
		//POINT thermal_trip_bh_brk			= 
		{ ID_ERMPLC_XB0,0x0040 },
		//POINT thermal_trip_ah_fan			= 
		{ ID_ERMPLC_XB0,0x0080 },
		//POINT braking_unit1_fault			= 
		{ ID_ERMPLC_XB0,0x0100 },
		//POINT braking_unit2_fault			= 
		{ ID_ERMPLC_XB0,0x0200 },
		//POINT braking_unit3_fault			= 
		{ ID_ERMPLC_XB0,0x0400 },
		//POINT braking_unit4_fault			= 
		{ ID_ERMPLC_XB0,0x0800 },
		//POINT braking_unit5_fault			= 
		{ ID_ERMPLC_XB0,0x1000 },
		//POINT braking_unit6_fault			= 
		{ ID_ERMPLC_XB0,0x2000 },
		//POINT braking_unit7_fault			= 
		{ ID_ERMPLC_XB0,0x4000 },
		//POINT braking_unit8_fault			= 
		{ ID_ERMPLC_XB0,0x8000 },
		
		//POINT mh_preset						= 
		{ ID_ERMPLC_XC0,0x0001 },
		//POINT ah_preset						= 
		{ ID_ERMPLC_XC0,0x0002 },
		//POINT spare12						= 
		{ ID_ERMPLC_XC0,0x0004 },
		//POINT spare13						= 
		{ ID_ERMPLC_XC0,0x0008 },
		//POINT spare14						= 
		{ ID_ERMPLC_XC0,0x0010 },
		//POINT spare15						= 
		{ ID_ERMPLC_XC0,0x0020 },
		//POINT spare16						= 
		{ ID_ERMPLC_XC0,0x0040 },
		//POINT spare17						= 
		{ ID_ERMPLC_XC0,0x0080 },
		//POINT spare18						= 
		{ ID_ERMPLC_XC0,0x0100 },
		//POINT spare19						= 
		{ ID_ERMPLC_XC0,0x0200 },
		//POINT spare20						= 
		{ ID_ERMPLC_XC0,0x0400 },
		//POINT spare21						= 
		{ ID_ERMPLC_XC0,0x0800 },
		//POINT spare22						= 
		{ ID_ERMPLC_XC0,0x1000 },
		//POINT spare23						= 
		{ ID_ERMPLC_XC0,0x2000 },
		//POINT spare24						= 
		{ ID_ERMPLC_XC0,0x4000 },
		//POINT spare25						= 
		{ ID_ERMPLC_XC0,0x8000 }
	},
	//INV_IF_X_MAP        mp_inv_X,
	//struct INV_IF_X_MAP ]
	{
		//D0-F0
		//POINT fwd			= 
		{ 0,0x0001 },
		//POINT rev			= 
		{ 0,0x0002 },
		//POINT fan_stop		= 
		{ 0,0x0004 },
		//POINT charge_ok		= 
		{ 0,0x0008 },
		//POINT fault			= 
		{ 0,0x0800 },
		//POINT on_monitor	= 
		{ 0,0x1000 },
		//POINT spd_set_ok	= 
		{ 0,0x2000 },
		//POINT com_complete	= 
		{ 0,0x8000 }
	},
	//INV_IF_Y_MAP 	    mp_inv_Y,
	//struct INV_IF_Y_MAP 
	{
		//D0-F0
		//POINT com_fwd		= 
		{ 0,0x0001 },
		//POINT com_rev		= 
		{ 0,0x0002 },
		//POINT change_acc	= 
		{ 0,0x0040 },
		//POINT flt_reset		= 
		{ 0,0x0100 },
		//POINT free_run		= 
		{ 0,0x0200 }
	}
	};
#endif
}
namespace	HHGQ18 
{
	ST_SPEC _spec;
	ST_PLC_IO_MAP 	_map;
}
namespace	HHFR22 
{
	ST_SPEC _spec;
	ST_PLC_IO_MAP 	_map;
}
