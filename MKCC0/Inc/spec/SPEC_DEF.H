#pragma once
#include "common_def.h"

/*** 仕様定義構造体 ***/
#define DIRECTION_MAX				2
#define ACCDEC_MAX					2
#define N_AS_AXIS					5	//自動の制御軸数　走行、横行、旋回、スキュー
#define N_SWAY_LEVEL				3	//完了,トリガ,制限
#define N_POSITION_LEVEL			3	//完了,トリガ,制限
#define N_NOTCH_RATIO				4	//ノッチ倍率設定数

//軸動作定格
#define DRIVE_ITEM_NUM				8	// ドライブ関連パラメータ項目数
#define DRIVE_ITEM_RPS				0	// モータ定格回転数（100%）
#define DRIVE_ITEM_GEAR				1	// 減速比
#define DRIVE_ITEM_ACC_TIME			2	// 加速時間（100%)
#define DRIVE_ITEM_RATE_V			3	// 定格速度（100%)
#define DRIVE_ITEM_RATE_NV			4	// ドラム回転速度（100%)
#define DRIVE_ITEM_RATE_NA			5	// ドラム回転加速度（100%)
#define DRIVE_ITEM_NMAX_RATIO		6	// ドラム最大回転数比率（100%->1.0)
#define DRIVE_ITEM_RPM				7	// モータ定格回転数（100%）

//軸動作極限
#define POS_LIMIT_TYPE_MAX			4
#define END_LIMIT_POS				0
#define SPD_LIMIT_POS1				1
#define SPD_LIMIT_POS2				2
#define SPD_LIMIT_POS3				3

//制御パラメータ
#define PARAM_PHASE_CHK_RANGE_BH	0.05//振れ止め位相到達判定範囲
#define PARAM_PHASE_CHK_RANGE_SLW	0.05//振れ止め位相到達判定範囲

#define ID_LV_COMPLE				0	//完了判定INDEX
#define ID_LV_TRIGGER				1	//トリガ判定INDEX
#define ID_LV_LIMIT					2	//制限判定INDEX

#define ID_LV_DIR_CHECK_MARGIN		0	//移動方向判定マージン
#define ID_LV_COMP_POS_PASSED		1	//通過判定マージン
#define ID_LV_COMP_POS_RANGE		2	//位置決め完了判定マージン

#define NUM_OF_DELAY_PTN			5	//加減速時FB遅れ時間評価パターン数
#define ID_DELAY_0START				0	//停止からの加速
#define ID_DELAY_ACC_DEC			1	//加速中の減速切替時
#define ID_DELAY_DEC_ACC			2	//減速中の加速切替時
#define ID_DELAY_CNT_ACC			3	//定速からの加速時
#define ID_DELAY_CNT_DEC			4	//定速からの減速時	

#define SEMI_AUTO_TARGET_MAX		8	//半自動目標ホールド数

#define WIRE_ITEM_NUM				8	//ワイヤ関連パラメータ項目数
#define NW_ITEM_WIND				0	//ワイヤ掛数
#define NW_ITEM_WIND_BOOM			1	//ワイヤ掛数ブーム部
#define NW_ITEM_EX_TURN				2	//あだ巻数
#define NW_ITEM_GROOVE				3	//ドラム溝数
#define DRUM_ITEM_DIR				4	//１層ドラム径
#define DRUM_ITEM_WIRE_DIR			5	//ワイヤ径
#define DRUM_ITEM_DIR_ADD			6	//ドラム径層加算値
#define SIZE_ITEM_WIRE_LEN0			7	//基準ワイヤ長

//クレーン構造定義構造体
typedef struct _ST_STRUCTURE_
{
	double wheel_span = 20.0;	// ホイルスパン
	double leg_span = 15.0;		// 脚間スパン
	double Hp = 47.7;			// ブーム水平時高さ
	double Lb = 64.009;			// ブーム長さ

	double Lm = 63.000;			//ジブpin-主巻シーブ中心距離
	double La_add = 4.981;		//ジブ主補巻シーブ間距離
	double rad_mh_ah = 2.371;	//ジブ主補巻シーブ間角度
	double La = 66.566;			//ジブpin-補巻シーブ中心距離
	double rad_Lm_La = 0.0537;	//ジブ主補巻シーブ間角度
	double Lmb = 1.676;
	double Lma = 4.981;
	double Lba = 5.512;
	double Th_pba = 2.00301;
	double Alpa_db = 0.90266;
	double Alpa_b = 0.02109;	//ジブ主巻軸－起伏軸間角度
	double Alpa_a = 0.05373;	//ジブ補巻軸－主巻軸間角度
	double Phbm = 2.20547;
	double Lp = 25.174;
	double Php = 1.6583;
	double Lmf = 2.9000;
	double Laf = 2.2910;

	double Th_rest = -0.7113;	//ジブレスト時の起伏角
	double D_min = 41.425;	//最小半径時の起伏シーブ間距離
	double D_rest = 83.905;	//ジブレスト時の起伏シーブ間距離
	double Hb_min = 59.397;	//最小半径時の起伏高さ
	double Hb_rest = -41.786;	//ジブレスト時起伏高さ
	double Dttb = 3.652;	//TTB径
	double Kttb = 0.08434;	//ピニオン径/TTB径
	double Nbh_drum = 67.974;	//引込ドラム回転数の和（引込＋主巻）
	double Load0_mh = 11000.0;	//主巻吊具荷重
	double Load0_ah = 1900.0;	//補巻吊具荷重
}ST_STRUCTURE_, *LPST_STRUCTURE_;

typedef struct _ST_SPEC {
//ノッチ速度物理量
	double notch_spd_f[MOTION_ID_MAX][N_NOTCH_MAX] = {		//# ノッチ指令速度（機上）1倍速時
	{ 0.0,	0.012,	0.035,	0.07,	0.117,	0.117 },			//[ID_HOIST]	m/s
	{ 0.0,	0.04,	0.125,	0.25,	0.416,	0.416 },			//[ID_GANTRY]	m/s
	{ 0.0,	0.0,	0.0,	0.0,	0.0,	0.0 },				//[ID_TROLLY]	m/s
	{ 0.0,	0.021,	0.063,	0.125,	0.208,	0.208 },			//[ID_BOOM_H]	m/s
	{ 0.0,	0.003,	0.011,	0.021,	0.035,	0.035 },			//[ID_SLEW]		rad/s;
	{  0.0,	0.02,	0.06,	0.12,	0.2,	0.2 },				//[ID_AHOIST]	m/s
	{ 0.0,	0.0,	0.0,	0.0,	0.0,	0.0 },				//[ID_H_ASSY];
	{ 0.0,	0.0,	0.0,	0.0,	0.0,	0.0 },				//[ID_MOTION1];
	};
	double notch_spd_r[MOTION_ID_MAX][N_NOTCH_MAX] = {			//# ノッチ指令速度（機上）1倍速時
	{ 0.0,	-0.012,	-0.35,	-0.07,	-0.117,	-0.117 },			//[ID_HOIST]	m/s
	{ 0.0,	-0.04,	-0.125,	-0.25,	-0.416,	-0.416 },			//[ID_GANTRY]	m/s
	{ 0.0,	0.0,	0.0,	0.0,	0.0,	0.0 },				//[ID_TROLLY]	m/s
	{ 0.0,	-0.021,	-0.063,	-0.125,	-0.208,	-0.208 },			//[ID_BOOM_H]	m/s
	{  0.0,	-0.003,	-0.011,	-0.021,	-0.035,	-0.035  },			//[ID_SLEW]		rad/s;
	{ 0.0,	-0.02,	-0.06,	-0.12,	-0.2,	-0.2  },			//[ID_AHOIST]	m/s
	{ 0.0,	0.0,	0.0,	0.0,	0.0,	0.0 },				//[ID_H_ASSY];
	{ 0.0,	0.0,	0.0,	0.0,	0.0,	0.0 },				//[ID_MOTION1];
	};
//ノッチ速度％
	double notch_spd_nrm_f[MOTION_ID_MAX][N_NOTCH_MAX] = {		//# ノッチ指令速度（正規化）%
	{ 0.0,	0.1,	0.3,	0.6,	1.0,	1.0 },				//[ID_HOIST]	%
	{ 0.0,	0.1,	0.3,	0.6,	1.0,	1.0 },				//[ID_GANTRY]	%
	{ 0.0,	0.0,	0.0,	0.0,	0.0,	0.0 },				//[ID_TROLLY]	%
	{ 0.0,	0.1,	0.3,	0.6,	1.0,	1.0 },				//[ID_BOOM_H]	%
	{ 0.0,	0.1,	0.3,	0.6,	1.0,	1.0 },				//[ID_SLEW]		%
	{ 0.0,	0.1,	0.3,	0.6,	1.0,	1.0 },				//[ID_AHOIST]	%
	{ 0.0,	0.0,	0.0,	0.0,	0.0,	0.0 },				//[ID_H_ASSY];
	{ 0.0,	0.0,	0.0,	0.0,	0.0,	0.0 },				//[ID_MOTION1];
	};
	double notch_spd_nrm_r[MOTION_ID_MAX][N_NOTCH_MAX] = {			//# ノッチ指令速度（正規化）%
	{ 0.0,	-0.1,	-0.3,	-0.6,	-1.0,	-1.0 },				//[ID_HOIST]	%
	{ 0.0,	-0.1,	-0.3,	-0.6,	-1.0,	-1.0},				//[ID_GANTRY]	%
	{ 0.0,	0.0,	0.0,	0.0,	0.0,	0.0 },				//[ID_TROLLY]	%
	{ 0.0,	-0.1,	-0.3,	-0.6,	-1.0,	-1.0  },			//[ID_BOOM_H]	%
	{ 0.0,	-0.1,	-0.3,	-0.6,	-1.0,	-1.0 },				//[ID_SLEW]		%
	{ 0.0,	-0.1,	-0.3,	-0.6,	-1.0,	-1.0  },			//[ID_AHOIST]	%
	{ 0.0,	0.0,	0.0,	0.0,	0.0,	0.0 },				//[ID_H_ASSY];
	{ 0.0,	0.0,	0.0,	0.0,	0.0,	0.0 },				//[ID_MOTION1];
	};
//速度設定倍率パターン
	double notch_spd_retio[MOTION_ID_MAX][N_NOTCH_RATIO] = {	//倍速パターン
	{ 1.0,2.0,3.0,1.0},		//[ID_HOIST]	1倍　2倍　3倍
	{ 1.0,1.0,1.0,1.0},		//[ID_GANTRY]	
	{ 1.0,1.0,1.0,1.0},		//[ID_TROLLY]	
	{ 1.0,1.0,1.0,1.0},		//[ID_BOOM_H]	
	{ 1.0,0.75,1.0,1.0},	//[ID_SLEW]		1倍　0.75倍	
	{ 1.0,2.0,1.0,1.0},		//[ID_AHOIST]	1倍　2倍	
	{ 1.0,1.0,1.0,1.0},		//[ID_H_ASSY];
	{ 1.0,1.0,1.0,1.0}		//[ID_MOTION1];
	};
//加減速度 m/s2,rad/s2
	double accdec[MOTION_ID_MAX][DIRECTION_MAX][ACCDEC_MAX] = {	//# 各動作加減速度
	{{ 0.078,	-0.078},	{-0.078,	0.078 }},				//[ID_HOIST]	m/s2
	{{ 0.0405,	-0.0405},	{-0.0405,	0.0405 }},				//[ID_GANTRY]	m/s2
	{{ 0.0,		-0.0},		{-0.0,		0.0 }},					//[ID_TROLLY]	m/s2
	{{ 0.1387,	-0.11387},	{-0.11387,	0.11387 }},				//[ID_BOOM_H]	m/s2
	{{ 0.00389,	-0.00389},	{-0.00389,	0.00389 }},				//[ID_SLEW]		rad/s2;
	{{ 0.133,	-0.133},	{-0.133,	0.133 }},				//[ID_AHOIST]	m/s2
	{{ 0.0,	0.0},			{0.0,	0.0 }},						//[ID_H_ASSY];
	{{ 0.0,	0.0},			{0.0,	0.0 }},						//[ID_MOTION1];
	};
//軸動作定格
	double prm_drv_rate[DRIVE_ITEM_NUM][MOTION_ID_MAX]{
		//	HST,		GNT,		BHMH,		BH,			SLW,		AH,			ASSY,	COM
			{12.5,		28.333,		0.0,		29.1667,	29.1667,	16.667,		0.0,	0.0},	// モータ定格回転数（rps　100%）
			{0.007823,	0.011234,	0.0,		0.004399,	0.002336,	0.008418,	0.0,	0.0},	// 減速比(ドラム回転/モータ回転）
			{1.5,		8.0,		0.0,		1.5,		9.0,		1.5,		0.0,	0.0},	// 加速時間（100%)
			{0.1170,	0.5,		0.0,		0.2080,		0.035,		0.2000,		0.0,	0.0},	// 定格速度（m/s, rad/s：100％)
			{0.09775,	0.3183,		0.0,		0.1283,		0.06815,	0.1403,		0.0,	0.0},	// 定格ドラム回転速度（100％rps)
			{0.065,		0.0398,		0.0,		0.8553,		0.007572,	0.0935,		0.0,	0.0},	// 定格ドラム回転加速度（rps2)
			{3.0,		1.0,		1.0,		1.0,		1.0,		2.0,		0.0,	0.0},	// ドラム最大回転数比率（100%->1.0)
			{750.0,		1700.0,		0.0,		1750.0,		1750.0,		1000.0,		0.0,	0.0}	// モータ定格回転数（rps　100%）
	};
//軸動作極限
	double pos_limit[MOTION_ID_MAX][DIRECTION_MAX][POS_LIMIT_TYPE_MAX] = {//# 極限寸法	
	{{ 79.2,0.0,0.0,0.0},	{ -5.8,0.0,0.0,0.0}},				//[ID_HOIST]
	{{ 28.3,0.0,0.0,0.0},	{ 0.1,0.0,0.0,0.0}},				//[ID_GANTRY]
	{{ 0.0,0.0,0.0,0.0},	{ 0.0,0.0,0.0,0.0}},				//[ID_TROLLY]
	{{ 67.0,0.0,0.0,0.0},	{ 20.5,0.0,0.0,0.0}},				//[ID_BOOM_H]
	{{ 0.0,0.0,0.0,0.0},	{ 0.0,0.0,0.0,0.0}},				//[ID_SLEW]
	{{ 88.2,0.0,0.0,0.0},	{-5.65,0.0,0.0,0.0}},				//[ID_AHOIST]
	{{ 0.0,0.0,0.0,0.0},	{ 0.0,0.0,0.0,0.0}},				//[ID_H_ASSY]
	{{ 0.0,0.0,0.0,0.0},	{ 0.0,0.0,0.0,0.0}},				//[ID_MOTION1]
	};
//構造寸法
	ST_STRUCTURE_ st_structure;
//制御パラメータ
//# 振れ止め判定　振れ角レベル(rad)
	double as_rad_level[N_AS_AXIS][N_SWAY_LEVEL] = {	
	{ 0.0,	0.0, 0.0},											//[ID_HOIST]
	{ 0.005, 0.01, 0.020 },									//[ID_GANTRY]
	{ 0.005, 0.01, 0.020 },									//[ID_TROLLY]
	{ 0.005, 0.01, 0.020 },									//[ID_BOOM_H]
	{ 0.005, 0.01, 0.020 },									//[ID_SLEW]	
	};
//# 振れ止め判定　振れ角レベル(rad^2)
	double as_rad2_level[N_AS_AXIS][N_SWAY_LEVEL] = {	
	{ 0.0,	0.0, 0.0},											//[ID_HOIST]
	{ 0.000025, 0.0001, 0.0004 },								//[ID_GANTRY]
	{ 0.000025, 0.0001, 0.0004 },								//[ID_TROLLY]
	{ 0.000025, 0.0001, 0.0004 },								//[ID_BOOM_H]
	{ 0.000025, 0.0001, 0.0004 },								//[ID_SLEW]
	};
//# 振れ止め判定　振れ幅レベル(m)
	double as_m_level[N_AS_AXIS][N_SWAY_LEVEL] = {	
	{ 0.0,	0.0, 0.0},											//[ID_HOIST]
	{ 0.1, 0.2, 2.00 },											//[ID_GANTRY]
	{ 0.1, 0.2, 2.00 },											//[ID_TROLLY]
	{ 0.1, 0.2, 2.00 },											//[ID_BOOM_H]
	{ 0.1, 0.2, 2.00 },											//[ID_SLEW]	
	};
//# 振れ止め判定　振れ幅レベル(m^2)
	double as_m2_level[N_AS_AXIS][N_SWAY_LEVEL] = {
	{ 0.0,	0.0, 0.0},											//[ID_HOIST]
	{ 0.01, 0.04, 4.0 },										//[ID_GANTRY]
	{ 0.01, 0.04, 4.0 },										//[ID_TROLLY]
	{ 0.01, 0.04, 4.0 },										//[ID_BOOM_H]
	{ 0.01, 0.04, 4.0 },										//[ID_SLEW]
	};
//# 位置決め制御判定　位置ずれレベル(m,rad)
	double as_pos_level[N_AS_AXIS][N_POSITION_LEVEL] = { 
	{ 0.03, 0.06, 0.50 },										//m[ID_HOIST]
	{ 0.03, 0.06, 0.50 },										//m[ID_GANTRY]
	{ 0.03, 0.06, 0.50 },										//m[ID_TROLLY]
	{ 0.03, 0.06, 0.5 },										//m[ID_BOOM_H]
	{ 0.005, 0.01, 0.1 }										//rad[ID_SLEW]
	};
//# 位置到達判定範囲(m,rad) 
double pos_check_limit[MOTION_ID_MAX][N_POSITION_LEVEL] = {	
	{ 0.1, 0.2, 0.5 },											//m[ID_HOIST]
	{ 0.1, 0.2, 0.5 },											//m[ID_GANTRY]
	{ 0.1, 0.2, 0.5 },											//m[ID_TROLLY]
	{ 0.1, 0.2, 0.5 },											//m[ID_BOOM_H]
	{ 0.005, 0.17, 0.017},										//rad[ID_SLEW]
	{ 0.0, 0.0, 0.0 },											//
	{ 0.0, 0.0, 0.0 },											//
	{ 0.0, 0.0, 0.0 },											//
	};
//# 遅れ時間,無駄時間定義
double delay_time[N_AS_AXIS][NUM_OF_DELAY_PTN] = {		// 加減速時のFB一時遅れ時定数
	{ 0.3,0.3,0.3,0.3,0.3},										//[ID_HOIST]
	{ 0.3,0.3,0.3,0.3,0.3 },									//[ID_GANTRY]
	{ 0.3,0.3,0.3,0.3,0.3 },									//[ID_TROLLY]
	{ 0.3,0.3,0.3,0.3,0.3 },									//[ID_BOOM_H]
	{ 0.3,0.3,0.3,0.3,0.3 },									//[ID_SLEW]
	};
//# 半自動目標位置(デフォルト)
double semi_target[SEMI_AUTO_TARGET_MAX][MOTION_ID_MAX]{//半自動デフォルト目標位置
	//	HST,	GNT,	TRY,	BH,		SLW,	OPR,	ASSY,	COM
		{63.4,	0.0,	0.0,	56.4,	0.0,	63.5,	0.0,	0.0},//配列0は最終設定値
		{63.4,	0.0,	0.0,	56.4,	0.0,	63.5,	0.0,	0.0},
		{20.0,	0.0,	0.0,	50.0,	1.5,	63.5,	0.0,	0.0},
		{40.0,	0.0,	0.0,	56.4,	-1.25,	63.5,	0.0,	0.0},
		{63.4,	0.0,	0.0,	56.4,	0.0,	63.5,	0.0,	0.0},
		{63.4,	0.0,	0.0,	56.4,	0.0,	63.5,	0.0,	0.0},
		{63.4,	0.0,	0.0,	56.4,	0.0,	63.5,	0.0,	0.00},
		{63.4,	0.0,	0.0,	56.4,	0.0,	63.5,	0.0,	0.0}
	};
//# ワイヤ関連パラメータ
double prm_nw[WIRE_ITEM_NUM][MOTION_ID_MAX]{
		//	HST,	GNT,	BHMH,	BH,		SLW,	AH,		ASSY,	COM
			{4.0,	0.0,	4.0,	6.0,	0.0,	2.0,	0.0,	0.0},	// ワイヤ掛数
			{2.0,	0.0,	2.0,	6.0,	0.0,	4.0,	0.0,	0.0},	// ワイヤ掛数ブーム部
			{4.0,	0.0,	6.0,	4.0,	0.0,	5.0,	0.0,	0.0},	// あだ巻数
			{15.0,	1.0,	10.0,	12.0,	1.0,	15.0,	0.0,	0.0},	// ドラム溝数
			{1.200,	0.50,	1.200,	1.1925,	0.308,	0.750,	0.0,	0.0},	// １層ドラム径
			{0.05,	0.0,	0.05,	0.0425,	0.0,	0.028,	0.0,	0.0},	// ワイヤ径
			{0.0866,0.0,	0.0866,	0.0736,	0.0,	0.0485,	0.0,	0.0},	// ドラム径層加算値
			{563.598,1.571,	563.598,525.906,0.968,	402.636,0.0,	0.0}	// 基準ワイヤ長(旋回はTTB円周
	};

}ST_SPEC, * LPST_SPEC;

#if 0
/*** PLC IFバッファ構造体定義 ***/
//デフォルト（みらい）の定義
typedef struct _ST_PLC_WRITE {//制御PC→PLC
	INT16   helthy;               //PCヘルシー出力信号
	INT16   ctrl_mode;            //自動制御フラグ
	UINT16  cab_ai[8];            //運転室PLC AI信号【将来用】
	INT16   cab_bi[6];            //運転室PLC→電気室PLC b出力
	INT16   spare0[8];            //運転室PLC→電気室PLC b出力
	UINT32  hcounter[4];          //高速カウンタユニット 
	UINT32  absocoder[3];         //アブソコーダ 
	INT16   spare1[2];            //予備
	INT16   pc_fault[2];          //PC検出異常マップ
	INT16   spare2[16];           //予備
	INT16   erm_unitx[4];         //電気室ユニットX 020-03F：カウンタユニット　040-04F　120-12F：ABSユニット
	INT16   erm_x[8];             //電気室X 060-0CF
	INT16   inv_cc_x[6];          //インバータFB書き込み値　ｘデバイス
	INT16   inv_cc_Wr1[6];        //インバータFB書き込み値　rpm
	INT16   inv_cc_Wr2[6];        //インバータFB書き込み値　トルク0.1%
	INT16   spare3[40];
}ST_PLC_WRITE, * LPST_PLC_WRITE;
typedef struct _ST_PLC_READ {
	INT16   helthy;               //PLCヘルシーカウンタ
	INT16   plc_ctrl;             // PLC運転モード
	UINT16  cab_ai[6];            //運転室PLC→電気室PLC W出力
	INT16   cab_bi[5];            //運転室PLC→電気室PLC b出力
	INT16   erm_900;
	INT16   erm_bo[6];            //電気室PLC b出力
	INT32   pos[5];               //各軸位置信号
	INT16   spd_tg[6];            //各軸指令速度目標
	INT16   plc_fault[18];        //各軸速度信号
	INT16   erm_y[4];             //電気室PLC Y出力
	INT16   erm_x[8];             //電気室PLC X入力
	INT16   inv_cc_y[6];          //インバータPLC DO指令
	INT16   inv_cc_Ww1[6];        //インバータPLC 速度指令　rpm
	INT16   inv_cc_x[6];          //インバータFB書き込み値　ｘデバイス
	INT16   inv_cc_Wr1[6];        //インバータFB書き込み値　rpm
	INT16   inv_cc_Wr2[6];        //インバータFB書き込み値　トルク0.1%
	INT16   spare1[4];            //予備
}ST_PLC_READ, * LPST_PLC_READ;

//西多度津70tJC102号の定義
typedef struct _ST_PLC_WRITE_HHGH29 {//制御PC→PLC
	INT16   helthy;               //D10200:PCヘルシー出力信号
	INT16   ctrl_mode;            //D10201:自動制御フラグ
	UINT16  cab_ai[8];            //D10202:運転室PLC AI信号【モーメントリミッタ】
	INT16   cab_bi[4];            //D10210:運転室PLC→電気室PLC b出力
	UINT32  hcounter[4];          //D10214:高速カウンタユニット 
	UINT32  absocoder[3];         //D10222アブソコーダ 
	INT16   pc_fault[2];          //PC検出異常マップ
	INT16   erm_unitx[6];         //電気室ユニットX 020-03F：カウンタユニット　040-04F：ABSユニット
	INT16   erm_x[7];             //電気室X 080-0EF
	INT16   inv_cc_x[6];          //インバータFB書き込み値　ｘデバイス
	INT16   inv_cc_Wr1[6];        //インバータFB書き込み値　rpm
	INT16   inv_cc_Wr2[6];        //インバータFB書き込み値　トルク0.1%
	INT16   spare4[30];
}ST_PLC_WRITE_HHGH29, * LPST_PLC_WRITE_HHGH29;
typedef struct _ST_PLC_READ_HHGH29 {
	INT16   helthy;             // D10400:PLCヘルシーカウンタ
	INT16   plc_ctrl;           // D10401: PLC運転モード
	UINT16  cab_ai[4];          // D10402:運転室PLC→電気室PLC W出力
	INT16   cab_bi[4];          // D10406:運転室PLC→電気室PLC b出力
	INT16   cab_bxi[4];         // D10410:運転室PLC→電気室PLC b出力
	INT16   erm_900;			// D10414:
	INT16   erm_bo[8];          // D10415:電気室PLC b出力
	INT32   pos[4];             // D10423:各軸位置信号
	INT16   spd_tg[6];          // D10431:各軸指令速度目標
	INT16   plc_fault[17];      // D10437:故障信号
	INT16   erm_y[5];           // D10454:電気室PLC Y出力
	INT16   erm_x[7];           // D10459:電気室PLC X入力
	INT16   inv_cc_y[6];        //インバータPLC DO指令
	INT16   inv_cc_x[6];        //インバータFB書き込み値　ｘデバイス
	INT16   inv_cc_Ww1[6];      //インバータPLC 速度指令　rpm
	INT16   inv_cc_Wr1[6];      //インバータFB書き込み値　rpm
	INT16   inv_cc_Wr2[6];      //インバータFB書き込み値　トルク0.1%
	INT16   spare1[3];          //予備
}ST_PLC_READ_HHGH29, * LPST_PLC_READ_HHGH29;
#endif
