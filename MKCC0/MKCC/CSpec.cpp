#include "CSpec.h"
#include "COMMON_DEF.h"

ST_STRUCTURE	CSpec::st_struct;//構造
ST_STRUCTURE	st_struct0 = {
	700000.0,	//定格荷重Kg
	84.0,		//ジブ長さ
	70.0,		//休止時揚程
	45.0,		//休止時半径
	90.0,		//引込ドラム引込/主巻部回転数の和
	72.50,		//引込引込ドラム休止時回転数
	17.50,		//引込主巻ドラム休止時回転数
	138.1,		//主巻ドラム休止時回転数
	35.0,		//ポスト上部高さ(ジブピン-AフレームTOP）
	38.5,		//ポスト下部高さ(ジブピン高さ)
};
ST_STRUCTURE	st_struct1 = {
	700000.0,	//定格荷重Kg
	84.0,		//ジブ長さ
	70.0,		//休止時揚程
	45.0,		//休止時半径
	90.0,		//引込ドラム引込/主巻部回転数の和
	72.50,		//引込引込ドラム休止時回転数
	17.50,		//引込主巻ドラム休止時回転数
	138.1,		//主巻ドラム休止時回転数
	35.0,		//ポスト上部高さ(ジブピン-AフレームTOP）
	38.5,		//ポスト下部高さ(ジブピン高さ)
};

//主巻
ST_AXIS_ITEMS	CSpec::base_mh;
ST_AXIS_ITEMS	base_mh0 = {
	ID_HOIST,//INT32 Axis_id
	
	{//double Notch_spd_f[N_NOTCH_MODE][N_NOTCH_MAX];		//ノッチ指令速度
		{0.0,0.023,0.070,0.140,0.233,0.233},
		{0.0,0.060,0.182,0.364,0.605,0.605},
		{0.0,0.0,0.0,0.0,0.0,0.0}
	},
	
	{//double Notch_spd_r[N_NOTCH_MODE][N_NOTCH_MAX];		//ノッチ指令速度
		{0.0,-0.023,-0.070,-0.140,-0.233,-0.233},
		{0.0,-0.06,-0.18,-0.36,-0.60,-0.60},
		{0.0,0.0,0.0,0.0,0.0,0.0}
	},
	{2000,5000,10000,25000,0x7FFF,0x7FFF},		//INT16 Notch_pad_f[N_NOTCH_MAX];	//GamePadのノッチ配分
	{-2000,-5000,-10000,-25000,0x8000,0x8000},	//INT16 Notch_pad_r[N_NOTCH_MAX];	//GamePadのノッチ配分
	
	0.233,			//double V_rated;定格速度（m/s, rad/s：100％)
	750.0,			// Rpm_rated;	定格回転数（rpm：100％)
	220.0,			// Kw_rated;	定格出力（kW：100％)
	31.129,			// Gear_ratio;	減速比(モータ回転 /ドラム回転 ）
	0.624,			// Ddrm0;		ドラム径0
	0.720,			// Ddrm1;		ドラム径1
	4.0,			// Nwire0;		ロープ掛数
	2.0,			// Nwire1;		ロープ掛数
	1.5,			// Ta0;			加速時間0
	2.5,			// Ta1;			加速時間1
	1.5,			// Td0;			減速時間0
	2.5,			// Td1;			減速時間1
	34,				// Ndmizo0;		ドラム溝数0
	21,				// Ndmizo1;		ドラム溝数1
	2048,			// CntPgR;		PG1回転カウント数×逓倍
	100000000,		// CntPgSet0;	PGプリセット0カウント値
	0.0,			// CntPgSet1;	PGプリセット1カウント値
	64.0,			// CntAbsR;		アブソコーダ1回転カウント数×入力軸ギア比
	50000.0,		// CntAbsSet0;	PGプリセット0カウント値
	0.0	,			// CntAbsSet1;	PGプリセット1カウント値
	0.04157,		// dDdrm;		ドラム1層追加径
	2801.127,		// Trq_rated;	定格トルク（モータ軸）
	135.1407,		// Pwr_base;	定格必要出力
	175.39915,		// Trq_base;	定格必要トルク
	26.748792,		// Ia;			慣性モーメント
	387.55,			// Lfull;		フルスパン（m）
};
ST_AXIS_ITEMS	base_mh1 = {
	ID_HOIST,//INT32 Axis_id

	{//double Notch_spd_f[N_NOTCH_MODE][N_NOTCH_MAX];		//ノッチ指令速度
		{0.0,0.023,0.070,0.140,0.233,0.233},
		{0.0,0.060,0.182,0.364,0.605,0.605},
		{0.0,0.0,0.0,0.0,0.0,0.0}
	},

	{//double Notch_spd_r[N_NOTCH_MODE][N_NOTCH_MAX];		//ノッチ指令速度
		{0.0,-0.023,-0.070,-0.140,-0.233,-0.233},
		{0.0,-0.06,-0.18,-0.36,-0.60,-0.60},
		{0.0,0.0,0.0,0.0,0.0,0.0}
	},
	{2000,5000,10000,25000,0x7FFF,0x7FFF},		//INT16 Notch_pad_f[N_NOTCH_MAX];	//GamePadのノッチ配分
	{-2000,-5000,-10000,-25000,0x8000,0x8000},	//INT16 Notch_pad_r[N_NOTCH_MAX];	//GamePadのノッチ配分

	0.233,			//double V_rated;定格速度（m/s, rad/s：100％)
	750.0,			// Rpm_rated;	定格回転数（rpm：100％)
	220.0,			// Kw_rated;	定格出力（kW：100％)
	31.129,			// Gear_ratio;	減速比(モータ回転 /ドラム回転 ）
	0.624,			// Ddrm0;		ドラム径0
	0.720,			// Ddrm1;		ドラム径1
	4.0,			// Nwire0;		ロープ掛数
	2.0,			// Nwire1;		ロープ掛数
	1.5,			// Ta0;			加速時間0
	2.5,			// Ta1;			加速時間1
	1.5,			// Td0;			減速時間0
	2.5,			// Td1;			減速時間1
	34,				// Ndmizo0;		ドラム溝数0
	21,				// Ndmizo1;		ドラム溝数1
	2048,			// CntPgR;		PG1回転カウント数×逓倍
	100000000,		// CntPgSet0;	PGプリセット0カウント値
	0.0,			// CntPgSet1;	PGプリセット1カウント値
	64.0,			// CntAbsR;		アブソコーダ1回転カウント数×入力軸ギア比
	50000.0,		// CntAbsSet0;	PGプリセット0カウント値
	0.0	,			// CntAbsSet1;	PGプリセット1カウント値
	0.04157,		// dDdrm;		ドラム1層追加径
	2801.127,		// Trq_rated;	定格トルク（モータ軸）
	135.1407,		// Pwr_base;	定格必要出力
	175.39915,		// Trq_base;	定格必要トルク
	26.748792,		// Ia;			慣性モーメント
	387.55,			// Lfull;		フルスパン（m）
};
ST_AUTO_SPEC	CSpec::auto_mh;
ST_REMOTE_SPEC	CSpec::rmt_mh;

//起伏
ST_AXIS_ITEMS	CSpec::base_bh;
ST_AXIS_ITEMS	base_bh0 = {

	ID_BOOM_H,//INT32 Axis_id;
	{//double Notch_spd_f[N_NOTCH_MODE][N_NOTCH_MAX];		//ノッチ指令速度
		{0.0,0.33,0.1,0.2,0.33,0.33},
		{0.0,0.0,0.0,0.0,0.0,0.0},
		{0.0,0.0,0.0,0.0,0.0,0.0}
	},
	
	{//double Notch_spd_r[N_NOTCH_MODE][N_NOTCH_MAX];		//ノッチ指令速度
		{-0.0,-0.33,-0.1,-0.2,-0.33,-0.33},
		{0.0,0.0,0.0,0.0,0.0,0.0},
		{0.0,0.0,0.0,0.0,0.0,0.0}
	},
	{2000,5000,10000,25000,0x7FFF,0x7FFF},		//INT16 Notch_pad_f[N_NOTCH_MAX];	//GamePadのノッチ配分
	{-2000,-5000,-10000,-25000,0x8000,0x8000},	//INT16 Notch_pad_r[N_NOTCH_MAX];	//GamePadのノッチ配分
	0.33,				//double V_rated;定格速度（m/s, rad/s：100％)
	1750.0,				// Rpm_rated;	定格回転数（rpm：100％)
	90.0,				// Kw_rated;	定格出力（kW：100％)
	78.12,				// Gear_ratio;	減速比(モータ回転 /ドラム回転 ）
	0.720,				// Ddrm0;		ドラム径0
	0.0,				// Ddrm1;		ドラム径1
	6.0,				// Nwire0;		ロープ掛数
	1.0,				// Nwire1;		ロープ掛数
	1.5,				// Ta0;			加速時間0
	0.0,				// Ta1;			加速時間1
	1.5,				// Td0;			減速時間0
	0.0,				// Td1;			減速時間1
	21.0,				// Ndmizo0;		ドラム溝数0
	0.0,				// Ndmizo1;		ドラム溝数1
	2048,				// CntPgR;		PG1回転カウント数×逓倍
	86673,				// CntPgSet0;	PGプリセット0カウント値
	0.0,				// CntPgSet1;	PGプリセット1カウント値
	0.0,				// CntAbsR;		アブソコーダ1回転カウント数×入力軸ギア比
	0.0,				// CntAbsSet0;	アブソコーダプリセット0カウント値
	0.0,				// CntAbsSet1;	アブソコーダプリセット1カウント値
	0.04157,			// dDdrm;		ドラム1層追加直径
	491.1067,			// Trq_rated;	定格トルク（モータ軸）
	0.0,				// Pwr_base;	定格必要出力
	0.0,				// Trq_base;	定格必要トルク
	4.885128,			// Ia;			慣性モーメント
	348.50,				// Lfull;		フルスパン（m）

};
ST_AXIS_ITEMS	base_bh1 = {

	ID_BOOM_H,//INT32 Axis_id;
	{//double Notch_spd_f[N_NOTCH_MODE][N_NOTCH_MAX];		//ノッチ指令速度
		{0.0,0.33,0.1,0.2,0.33,0.33},
		{0.0,0.0,0.0,0.0,0.0,0.0},
		{0.0,0.0,0.0,0.0,0.0,0.0}
	},

	{//double Notch_spd_r[N_NOTCH_MODE][N_NOTCH_MAX];		//ノッチ指令速度
		{-0.0,-0.33,-0.1,-0.2,-0.33,-0.33},
		{0.0,0.0,0.0,0.0,0.0,0.0},
		{0.0,0.0,0.0,0.0,0.0,0.0}
	},
	{2000,5000,10000,25000,0x7FFF,0x7FFF},		//INT16 Notch_pad_f[N_NOTCH_MAX];	//GamePadのノッチ配分
	{-2000,-5000,-10000,-25000,0x8000,0x8000},	//INT16 Notch_pad_r[N_NOTCH_MAX];	//GamePadのノッチ配分
	0.33,				//double V_rated;定格速度（m/s, rad/s：100％)
	1750.0,				// Rpm_rated;	定格回転数（rpm：100％)
	90.0,				// Kw_rated;	定格出力（kW：100％)
	78.12,				// Gear_ratio;	減速比(モータ回転 /ドラム回転 ）
	0.720,				// Ddrm0;		ドラム径0
	0.0,				// Ddrm1;		ドラム径1
	6.0,				// Nwire0;		ロープ掛数
	1.0,				// Nwire1;		ロープ掛数
	1.5,				// Ta0;			加速時間0
	0.0,				// Ta1;			加速時間1
	1.5,				// Td0;			減速時間0
	0.0,				// Td1;			減速時間1
	21.0,				// Ndmizo0;		ドラム溝数0
	0.0,				// Ndmizo1;		ドラム溝数1
	2048,				// CntPgR;		PG1回転カウント数×逓倍
	86673,				// CntPgSet0;	PGプリセット0カウント値
	0.0,				// CntPgSet1;	PGプリセット1カウント値
	0.0,				// CntAbsR;		アブソコーダ1回転カウント数×入力軸ギア比
	0.0,				// CntAbsSet0;	アブソコーダプリセット0カウント値
	0.0,				// CntAbsSet1;	アブソコーダプリセット1カウント値
	0.04157,			// dDdrm;		ドラム1層追加直径
	491.1067,			// Trq_rated;	定格トルク（モータ軸）
	0.0,				// Pwr_base;	定格必要出力
	0.0,				// Trq_base;	定格必要トルク
	4.885128,			// Ia;			慣性モーメント
	348.50,				// Lfull;		フルスパン（m）
};
ST_AUTO_SPEC	CSpec::auto_bh;
ST_REMOTE_SPEC	CSpec::rmt_bh;

//旋回
ST_AXIS_ITEMS	CSpec::base_sl;
ST_AXIS_ITEMS	base_sl0 = {
	ID_SLEW,//INT32 Axis_id;
	{//double Notch_spd_f[N_NOTCH_MODE][N_NOTCH_MAX];		//ノッチ指令速度
		{0.0,0.00175,0.0175,0.035,0.035,0.035},
		{0.0,0.0013,0.013,0.026,0.026,0.026},
		{0.0,0.0,0.0,0.0,0.0,0.0}
	},
	{//double Notch_spd_r[N_NOTCH_MODE][N_NOTCH_MAX];		//ノッチ指令速度
		{0.0,-0.00175,-0.0175,-0.035,-0.035,-0.035},
		{0.0,-0.0013,-0.013,-0.026,-0.026,-0.026},
		{0.0,0.0,0.0,0.0,0.0,0.0}
	},
	{2000,5000,10000,25000,0x7FFF,0x7FFF},		//INT16 Notch_pad_f[N_NOTCH_MAX];						//GamePadのノッチ配分
	{-2000,-5000,-10000,-25000,0x8000,0x8000},	//INT16 Notch_pad_r[N_NOTCH_MAX];						//GamePadのノッチ配分

	0.0349,				//1/3rpm double V_rated;定格速度（m/s, rad/s：100％)
	1800.0,				// Rpm_rated;	定格回転数（rpm：100％)
	33.0,				// Kw_rated;	定格出力（kW：100％)
	479.4,				// Gear_ratio;	減速比(モータ回転 /ドラム回転 ）
	0.33,				// Ddrm0;		ドラム径0
	0.0,				// Ddrm1;		ドラム径1
	1.0,				// Nwire0;		ロープ掛数
	1.0,				// Nwire1;		ロープ掛数
	12.0,				// Ta0;			加速時間0
	20.0,				// Ta1;			加速時間1
	12.0,				// Td0;			減速時間0
	20.0,				// Td1;			減速時間1
	1.0,				// Ndmizo0;		ドラム溝数0
	1.0,				// Ndmizo1;		ドラム溝数1
	2048,				// CntPgR;		PG1回転カウント数×逓倍
	15000000,			// CntPgSet0;	PGプリセット0カウント値
	20432689,			// CntPgSet1;	PGプリセット1カウント値
	0.0,				// CntAbsR;		アブソコーダ1回転カウント数×入力軸ギア比
	0.0,				// CntAbsSet0;	アブソコーダPGプリセット0カウント値
	0.0,				// CntAbsSet1;	アブソコーダPGプリセット1カウント値
	0.0,				// dDdrm;		ドラム1層追加径
	175.0704,			// Trq_rated;	定格トルク（モータ軸）
	0.0,				// Pwr_base;	定格必要出力
	0.0,				// Trq_base;	定格必要トルク
	30.39636,			// Ia;			慣性モーメント
	11.473,				// Lfull;		フルスパン（m）
};
ST_AXIS_ITEMS	base_sl1 = {
	ID_SLEW,//INT32 Axis_id;
	{//double Notch_spd_f[N_NOTCH_MODE][N_NOTCH_MAX];		//ノッチ指令速度
		{0.0,0.00175,0.0175,0.035,0.035,0.035},
		{0.0,0.0013,0.013,0.026,0.026,0.026},
		{0.0,0.0,0.0,0.0,0.0,0.0}
	},
	{//double Notch_spd_r[N_NOTCH_MODE][N_NOTCH_MAX];		//ノッチ指令速度
		{0.0,-0.00175,-0.0175,-0.035,-0.035,-0.035},
		{0.0,-0.0013,-0.013,-0.026,-0.026,-0.026},
		{0.0,0.0,0.0,0.0,0.0,0.0}
	},
	{2000,5000,10000,25000,0x7FFF,0x7FFF},		//INT16 Notch_pad_f[N_NOTCH_MAX];						//GamePadのノッチ配分
	{-2000,-5000,-10000,-25000,0x8000,0x8000},	//INT16 Notch_pad_r[N_NOTCH_MAX];						//GamePadのノッチ配分

	0.0349,				//1/3rpm double V_rated;定格速度（m/s, rad/s：100％)
	1800.0,				// Rpm_rated;	定格回転数（rpm：100％)
	33.0,				// Kw_rated;	定格出力（kW：100％)
	479.4,				// Gear_ratio;	減速比(モータ回転 /ドラム回転 ）
	0.33,				// Ddrm0;		ドラム径0
	0.0,				// Ddrm1;		ドラム径1
	1.0,				// Nwire0;		ロープ掛数
	1.0,				// Nwire1;		ロープ掛数
	12.0,				// Ta0;			加速時間0
	20.0,				// Ta1;			加速時間1
	12.0,				// Td0;			減速時間0
	20.0,				// Td1;			減速時間1
	1.0,				// Ndmizo0;		ドラム溝数0
	1.0,				// Ndmizo1;		ドラム溝数1
	2048,				// CntPgR;		PG1回転カウント数×逓倍
	15000000,			// CntPgSet0;	PGプリセット0カウント値
	20432689,			// CntPgSet1;	PGプリセット1カウント値
	0.0,				// CntAbsR;		アブソコーダ1回転カウント数×入力軸ギア比
	0.0,				// CntAbsSet0;	アブソコーダPGプリセット0カウント値
	0.0,				// CntAbsSet1;	アブソコーダPGプリセット1カウント値
	0.0,				// dDdrm;		ドラム1層追加径
	175.0704,			// Trq_rated;	定格トルク（モータ軸）
	0.0,				// Pwr_base;	定格必要出力
	0.0,				// Trq_base;	定格必要トルク
	30.39636,			// Ia;			慣性モーメント
	11.473,				// Lfull;		フルスパン（m）
};
ST_AUTO_SPEC	CSpec::auto_sl;
ST_REMOTE_SPEC	CSpec::rmt_sl;

//走行
ST_AXIS_ITEMS	CSpec::base_gt;
ST_AXIS_ITEMS	base_gt0 = {
	ID_GANTRY,//INT32 Axis_id;
	{//double Notch_spd_f[N_NOTCH_MODE][N_NOTCH_MAX];		//ノッチ指令速度
		{0.0,0.058,0.175,0.350,0.580,0.580},
		{0.0,0.0,0.0,0.0,0.0,0.0},
		{0.0,0.0,0.0,0.0,0.0,0.0}
	},
	{//double Notch_spd_r[N_NOTCH_MODE][N_NOTCH_MAX];		//ノッチ指令速度
		{0.0,-0.058,-0.175,-0.350,-0.580,-0.580},
		{0.0,0.0,0.0,0.0,0.0,0.0},
		{0.0,0.0,0.0,0.0,0.0,0.0}
	},
	
	{2000,5000,10000,25000,0x7FFF,0x7FFF},		//INT16 Notch_pad_f[N_NOTCH_MAX];	//GamePadのノッチ配分
	{-2000,-5000,-10000,-25000,0x8000,0x8000},	//INT16 Notch_pad_r[N_NOTCH_MAX];	//GamePadのノッチ配分
	
	0.58,			//double V_rated;定格速度（m/s, rad/s：100％)
	1700.0,			// Rpm_rated;	定格回転数（rpm：100％)
	44.0,			// Kw_rated;	定格出力（kW：100％)
	88.0,			// Gear_ratio;	減速比(モータ回転 /ドラム回転 ）
	0.5,			// Ddrm0;		ドラム径0
	0.0,			// Ddrm1;		ドラム径1
	1.0,			// Nwire0;		ロープ掛数
	1.0,			// Nwire1;		ロープ掛数
	9.0,			// Ta0;			加速時間0
	9.0,			// Ta1;			加速時間1
	9.0,			// Td0;			減速時間0
	5.0,			// Td1;			減速時間1
	1.0,			// Ndmizo0;		ドラム溝数0
	0.0,			// Ndmizo1;		ドラム溝数1
	0.0,			// CntPgR;		PG1回転カウント数×逓倍
	0.0,			// CntPgSet0;	PGプリセット0カウント値
	0.0,			// CntPgSet1;	PGプリセット1カウント値
	0.0,			// CntAbsR;		アブソコーダ1回転カウント数×入力軸ギア比
	0.0,			// CntAbsSet0;	PGプリセット0カウント値
	0.0,			// CntAbsSet1;	PGプリセット1カウント値
	0.0,			// dDdrm;		ドラム1層追加径
	0.0,			// Trq_rated;	定格トルク（モータ軸）
	0.0,			// Pwr_base;	定格必要出力
	0.0,			// Trq_base;	定格必要トルク
	6.24755,		// Ia;			慣性モーメント
	1000.0,			// Lfull;		フルスパン（m）

};
ST_AXIS_ITEMS	base_gt1 = {
	ID_GANTRY,//INT32 Axis_id;
	{//double Notch_spd_f[N_NOTCH_MODE][N_NOTCH_MAX];		//ノッチ指令速度
		{0.0,0.058,0.175,0.350,0.580,0.580},
		{0.0,0.0,0.0,0.0,0.0,0.0},
		{0.0,0.0,0.0,0.0,0.0,0.0}
	},
	{//double Notch_spd_r[N_NOTCH_MODE][N_NOTCH_MAX];		//ノッチ指令速度
		{0.0,-0.058,-0.175,-0.350,-0.580,-0.580},
		{0.0,0.0,0.0,0.0,0.0,0.0},
		{0.0,0.0,0.0,0.0,0.0,0.0}
	},

	{2000,5000,10000,25000,0x7FFF,0x7FFF},		//INT16 Notch_pad_f[N_NOTCH_MAX];	//GamePadのノッチ配分
	{-2000,-5000,-10000,-25000,0x8000,0x8000},	//INT16 Notch_pad_r[N_NOTCH_MAX];	//GamePadのノッチ配分

	0.58,			//double V_rated;定格速度（m/s, rad/s：100％)
	1700.0,			// Rpm_rated;	定格回転数（rpm：100％)
	44.0,			// Kw_rated;	定格出力（kW：100％)
	88.0,			// Gear_ratio;	減速比(モータ回転 /ドラム回転 ）
	0.5,			// Ddrm0;		ドラム径0
	0.0,			// Ddrm1;		ドラム径1
	1.0,			// Nwire0;		ロープ掛数
	1.0,			// Nwire1;		ロープ掛数
	9.0,			// Ta0;			加速時間0
	9.0,			// Ta1;			加速時間1
	9.0,			// Td0;			減速時間0
	5.0,			// Td1;			減速時間1
	1.0,			// Ndmizo0;		ドラム溝数0
	0.0,			// Ndmizo1;		ドラム溝数1
	0.0,			// CntPgR;		PG1回転カウント数×逓倍
	0.0,			// CntPgSet0;	PGプリセット0カウント値
	0.0,			// CntPgSet1;	PGプリセット1カウント値
	0.0,			// CntAbsR;		アブソコーダ1回転カウント数×入力軸ギア比
	0.0,			// CntAbsSet0;	PGプリセット0カウント値
	0.0,			// CntAbsSet1;	PGプリセット1カウント値
	0.0,			// dDdrm;		ドラム1層追加径
	0.0,			// Trq_rated;	定格トルク（モータ軸）
	0.0,			// Pwr_base;	定格必要出力
	0.0,			// Trq_base;	定格必要トルク
	6.24755,		// Ia;			慣性モーメント
	1000.0,			// Lfull;		フルスパン（m）
};
ST_AUTO_SPEC	CSpec::auto_gt;
ST_REMOTE_SPEC	CSpec::rmt_gt;

//補巻
ST_AXIS_ITEMS	CSpec::base_ah;
ST_AXIS_ITEMS	base_ah0 = {
	ID_AHOIST,//INT32 Axis_id;
};
ST_AXIS_ITEMS	base_ah1 = {
	ID_AHOIST,//INT32 Axis_id;
};
ST_AUTO_SPEC	CSpec::auto_ah;
ST_REMOTE_SPEC	CSpec::rmt_ah;

int CSpec::setup(int crane_id) {
	switch (crane_id) {
	case CRANE_ID_H6R602:{
		memcpy_s(&st_struct, sizeof(ST_STRUCTURE), &st_struct0, sizeof(ST_STRUCTURE));
		memcpy_s(&base_mh, sizeof(ST_AXIS_ITEMS), &base_mh0, sizeof(ST_AXIS_ITEMS));
		memcpy_s(&base_bh, sizeof(ST_AXIS_ITEMS), &base_bh0, sizeof(ST_AXIS_ITEMS));
		memcpy_s(&base_sl, sizeof(ST_AXIS_ITEMS), &base_sl0, sizeof(ST_AXIS_ITEMS));
		memcpy_s(&base_gt, sizeof(ST_AXIS_ITEMS), &base_gt0, sizeof(ST_AXIS_ITEMS));
		memcpy_s(&base_ah, sizeof(ST_AXIS_ITEMS), &base_ah0, sizeof(ST_AXIS_ITEMS));
	}break;
	case CARNE_ID_HHGH29:{
		memcpy_s(&st_struct, sizeof(ST_STRUCTURE), &st_struct1, sizeof(ST_STRUCTURE));
		memcpy_s(&base_mh, sizeof(ST_AXIS_ITEMS), &base_mh1, sizeof(ST_AXIS_ITEMS));
		memcpy_s(&base_bh, sizeof(ST_AXIS_ITEMS), &base_bh1, sizeof(ST_AXIS_ITEMS));
		memcpy_s(&base_sl, sizeof(ST_AXIS_ITEMS), &base_sl1, sizeof(ST_AXIS_ITEMS));
		memcpy_s(&base_gt, sizeof(ST_AXIS_ITEMS), &base_gt1, sizeof(ST_AXIS_ITEMS));
		memcpy_s(&base_ah, sizeof(ST_AXIS_ITEMS), &base_ah1, sizeof(ST_AXIS_ITEMS));
	}break;
	case CARNE_ID_HHGQ18:{
		memcpy_s(&st_struct, sizeof(ST_STRUCTURE), &st_struct0, sizeof(ST_STRUCTURE));
		memcpy_s(&base_mh, sizeof(ST_AXIS_ITEMS), &base_mh0, sizeof(ST_AXIS_ITEMS));
		memcpy_s(&base_bh, sizeof(ST_AXIS_ITEMS), &base_bh0, sizeof(ST_AXIS_ITEMS));
		memcpy_s(&base_sl, sizeof(ST_AXIS_ITEMS), &base_sl0, sizeof(ST_AXIS_ITEMS));
		memcpy_s(&base_gt, sizeof(ST_AXIS_ITEMS), &base_gt0, sizeof(ST_AXIS_ITEMS));
		memcpy_s(&base_ah, sizeof(ST_AXIS_ITEMS), &base_ah0, sizeof(ST_AXIS_ITEMS));
	}break;
	case CARNE_ID_HHFR22:{
		memcpy_s(&st_struct, sizeof(ST_STRUCTURE), &st_struct0, sizeof(ST_STRUCTURE));
		memcpy_s(&base_mh, sizeof(ST_AXIS_ITEMS), &base_mh0, sizeof(ST_AXIS_ITEMS));
		memcpy_s(&base_bh, sizeof(ST_AXIS_ITEMS), &base_bh0, sizeof(ST_AXIS_ITEMS));
		memcpy_s(&base_sl, sizeof(ST_AXIS_ITEMS), &base_sl0, sizeof(ST_AXIS_ITEMS));
		memcpy_s(&base_gt, sizeof(ST_AXIS_ITEMS), &base_gt0, sizeof(ST_AXIS_ITEMS));
		memcpy_s(&base_ah, sizeof(ST_AXIS_ITEMS), &base_ah0, sizeof(ST_AXIS_ITEMS));
	}break;
	default: {
		memcpy_s(&st_struct, sizeof(ST_STRUCTURE), &st_struct0, sizeof(ST_STRUCTURE));
		memcpy_s(&base_mh, sizeof(ST_AXIS_ITEMS), &base_mh0, sizeof(ST_AXIS_ITEMS));
		memcpy_s(&base_bh, sizeof(ST_AXIS_ITEMS), &base_bh0, sizeof(ST_AXIS_ITEMS));
		memcpy_s(&base_sl, sizeof(ST_AXIS_ITEMS), &base_sl0, sizeof(ST_AXIS_ITEMS));
		memcpy_s(&base_gt, sizeof(ST_AXIS_ITEMS), &base_gt0, sizeof(ST_AXIS_ITEMS));
		memcpy_s(&base_ah, sizeof(ST_AXIS_ITEMS), &base_ah0, sizeof(ST_AXIS_ITEMS));
	}break;
	}
	return 0;
}

