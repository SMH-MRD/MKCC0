// CraneLib.cpp : スタティック ライブラリ用の関数を定義します。
//

#include "pch.h"
#include "framework.h"
#include "CCraneLib.h"

int CCraneBase::_crane_id;
ST_SPEC CCraneBase::_spec;
ST_CRANE_OBJ CCraneBase::objs;
CCraneBase* CCraneBase::pCrane;
PINT16 CCraneBase::_buf_if_r, CCraneBase::_buf_if_w;

INT16 CCraneBase::dummy_if_wbuf[CC_MC_SIZE_W_WRITE];
INT16 CCraneBase::dummy_if_rbuf[CC_MC_SIZE_W_READ];

CCraneBase::CCraneBase(int crane_id)
{
	_crane_id = crane_id;
	_buf_if_r = dummy_if_rbuf, _buf_if_w = dummy_if_wbuf;
	pCrane = this;
	setup_crane(crane_id);
};
CCraneBase::CCraneBase(int crane_id, PINT16 buf_if_r, PINT16 buf_if_w)
{
	_crane_id = crane_id;
	_buf_if_r = buf_if_r, _buf_if_w = buf_if_w;
	pCrane = this;
	setup_crane(crane_id);
};
CCraneBase::~CCraneBase(void) 
{
	delete& objs;
};

HRESULT CCraneBase::setup_crane(int crane_id) {

	PINT16 pw016, pr016, pw16, pr16;
	INT16 prmw16, prmr16;
	PINT32 pw032, pr032;
	INT32 prmw32, prmr32;
	
	switch (crane_id) {
	case CRANE_ID_H6R602: {//みらい
		_spec = {
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
	}return S_OK;
	case CARNE_ID_HHGH29: {//今造西多度津70tJC　102号
		_spec = {
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

		LPST_PLC_WRITE_HHGH29 pwbuf = (LPST_PLC_WRITE_HHGH29)_buf_if_w;
		LPST_PLC_READ_HHGH29  prbuf = (LPST_PLC_READ_HHGH29 )_buf_if_r;
		
		//* 運転室 bレジスタ　**********************************************
		pw016 = pwbuf->cab_bi;	pr016 = prbuf->cab_bi;
		//B220
		pw16 = pw016 + 0;	pr16 = pr016 + 0;
		{

			prmw16 = prmr16 = BIT0;		//PB主幹入
			objs.pb_ctrl_source_on = new CBitIO(pr16, pw16, &prmw16, &prmr16);

			prmw16 = prmr16 = BIT1;		//PB主幹切
			objs.pb_ctrl_source_off = new CBitIO(pr16, pw16, &prmw16, &prmr16);

			prmw16 = prmr16 = 0x000c;	//CS主巻速度
			objs.cs3_mh_spd			= new CModeIO(pr16, pw16, &prmw16, &prmr16);

			prmw16 = prmr16 = 0x0070;	//CS引込モード
			objs.cs4_bh_R			= new CModeIO(pr16, pw16, &prmw16, &prmr16);
		}
		//B230
		pw16 = pw016 + 1;	pr16 = pr016 + 1;
		{
			prmw16 = prmr16 = BIT15;	//運転室非常停止
			objs.pb_cab_estop = new CBitIO(pr16, NULL, &prmw16, &prmr16);
		}
		//B240
		pw16 = pw016 + 2;	pr16 = pr016 + 2;
		{
			prmw16 = prmr16 = BIT1;		//モーメントリミッタ旋回加速切替50t以下
			objs.sw_mlim_50t_lower = new CBitIO(pr16, NULL, &prmw16, &prmr16);

			prmw16 = prmr16 = BIT2;		//モーメントリミッタ3倍速許可荷重以下
			objs.sw_mlim_x3_enable = new CBitIO(pr16, NULL, &prmw16, &prmr16);

			prmw16 = prmr16 = BIT3;		//モーメントリミッタ予備警報
			objs.sw_mlim_90_warning	= new CBitIO(pr16, NULL, &prmw16, &prmr16);

			prmw16 = prmr16 = BIT4;		//モーメントリミッタ警報
			objs.sw_mlim_100_warning		= new CBitIO(pr16, NULL, &prmw16, &prmr16);

			prmw16 = prmr16 = BIT5;		//モーメントリミッタ高巻可
			objs.sw_mlim_hhoist_enable = new CBitIO(pr16, NULL, &prmw16, &prmr16);

			prmw16 = prmr16 = BIT6;		//モーメントリミッタエラー
			objs.sw_mlim_error			= new CBitIO(pr16, NULL, &prmw16, &prmr16);

			prmw16 = prmr16 = BIT7;	//風速16m/s以上
			objs.sw_wind_over16			= new CBitIO(pr16, NULL, &prmw16, &prmr16);

			prmw16 = prmr16 = BIT8;	//PB警報停止
			objs.pb_alarm_stop = new CBitIO(pr16, pw16, &prmw16, &prmr16);

			prmw16 = prmr16 = BIT9;	//PB故障リセット
			objs.pb_fault_reset = new CBitIO(pr16, pw16, &prmw16, &prmr16);

			prmw16 = prmr16 = BIT10;	//PBバイパス
			objs.pb_il_bypass = new CBitIO(pr16, pw16, &prmw16, &prmr16);
		}

		//B250
		pw16 = pw016 + 3;	pr16 = pr016 + 3;
		{
			prmw16 = prmr16 = BIT15;		//旋回油圧圧力SW
			objs.sw_slw_brk = new CBitIO(pr16, pw16, &prmw16, &prmr16);
		}

		//* 運転室 ノッチ　**********************************************
		{
			pw016 = pwbuf->cab_bi;	pr016 = prbuf->cab_bi;	//B220-B22F
			//主巻
			pw16 = pw016 + 1;	pr16 = pr016 + 1;			//B230-B23F
			prmw16 = prmr16 = 0x003f;
			objs.notch[ID_HOIST] = new CNotchIO(pr16, pw16, &prmw16, &prmr16);

			//走行
			prmw16 = prmr16 = 0x7e00;
			objs.notch[ID_GANTRY] = new CNotchIO(pr16, pw16, &prmw16, &prmr16);
			for (int i = 0; i < PLCIO_N_NOTCH_INDEX; i++) {
				objs.notch[ID_GANTRY]->o_ptn[i] <<= 10;
				objs.notch[ID_GANTRY]->i_ptn[i] <<= 10;
			}
			//引込
			pw16 = pw016 + 3;	pr16 = pr016 + 3;//B250-B25F
			prmw16 = prmr16 = 0x003f;
			objs.notch[ID_BOOM_H] = new CNotchIO(pr16, pw16, &prmw16, &prmr16);
			//旋回
			prmw16 = prmr16 = 0x7e00;
			objs.notch[ID_SLEW] = new CNotchIO(pr16, pw16, &prmw16, &prmr16);
			for (int i = 0; i < PLCIO_N_NOTCH_INDEX; i++) {
				objs.notch[ID_SLEW]->o_ptn[i] <<= 6;
				objs.notch[ID_SLEW]->i_ptn[i] <<= 6;
			}
		}

	}return S_OK;
	case CARNE_ID_HHGQ18: {//今造西多度津300tJC　10号

	}return S_OK;
	case CARNE_ID_HHFR22: {//

	}return S_OK;
	default:break;
	}
	return S_FALSE;
}
