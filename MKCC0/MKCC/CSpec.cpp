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

//主巻
ST_AXIS_ITEMS	CSpec::base_mh;
ST_AXIS_ITEMS	base_mh0 = {
	ID_HOIST,//INT32 axis_id
	
	{//double notch_spd_f[N_NOTCH_MODE][N_NOTCH_MAX];		//ノッチ指令速度
		{0.0,0.023,0.070,0.140,0.233,0.233},
		{0.0,0.060,0.182,0.364,0.605,0.605},
		{0.0,0.0,0.0,0.0,0.0,0.0}
	},
	
	{//double notch_spd_r[N_NOTCH_MODE][N_NOTCH_MAX];		//ノッチ指令速度
		{0.0,-0.023,-0.070,-0.140,-0.233,-0.233},
		{0.0,-0.06,-0.18,-0.36,-0.60,-0.60},
		{0.0,0.0,0.0,0.0,0.0,0.0}
	},
	{2000,5000,10000,25000,0x7FFF,0x7FFF},		//INT16 notch_pad_f[N_NOTCH_MAX];	//GamePadのノッチ配分
	{-2000,-5000,-10000,-25000,0x8000,0x8000},	//INT16 notch_pad_r[N_NOTCH_MAX];	//GamePadのノッチ配分
	
	0.233,//double v_rated;定格速度（m/s, rad/s：100％)

	/*
	//INT32 notch_spd_nrm_f[N_NOTCH_MODE][N_NOTCH_MAX];	//ノッチ指令速度正規化100%=1000
	{{0,100,300,600,10000,10000},{0,258,773,1545,2575,2575},{0,0,0,0,0,0}},
	//INT32 notch_spd_nrm_r[N_NOTCH_MODE][N_NOTCH_MAX];//ノッチ指令速度正規化100%=10000
	{{0,-100,-300,-600,-10000,-10000},{0,-258,-773,-1545,-2575,-2575},{0,0,0,0,0,0}},

	//double acc;加速度
	{0.1533,0.1491,0.0},
	//double dec;減速度
	{0.-1533,-0.1491,0.0},
	//double t_acc;加時間
	{1.5,2.5,0.0},
	//double t_dec;減時間
	{1.5,2.5,0.0},

	//double motor_rps;モータ定格回転数（rps　100%）
	0.0,
	//double motor_rpm;モータ定格回転数（rpm　100%）
	0.0,
	//double gear_ratio;減速比(ドラム回転 / モータ回転）
	0.0,
	//double drum_rps;定格ドラム回転速度（100％rps)
	0.0,
	//double drum_rpps;定格ドラム回転加速度（r/s~2)
	0.0,
	//double pos_limit_f[N_POS_LIMIT_TYPE];軸動作極限
	{0.0,0.0,0.0,0.0},
	//double pos_limit_r[N_POS_LIMIT_TYPE];軸動作極限
	{0.0,0.0,0.0,0.0},
	//double n_wire;		// ワイヤ掛数
	0.0,
	//double n_boom_wire;	// ワイヤ掛数ブーム部
	0.0,
	//double n_ex_wind;	// あだ巻数
	0.0,
	//double n_tread;		// ドラム溝数
	0.0,
	//double d_drum;		// １層ドラム径
	0.0,
	//double d_wire;		// ワイヤ径
	0.0,
	//double d_drum_layer;// ドラム径層加算値
	0.0,
	//double l_wire;		// 基準ワイヤ長(旋回はTTB円周
	0.0,
	//INT32 v100plc
	1000
	*/
};
ST_AUTO_SPEC	CSpec::auto_mh;
ST_REMOTE_SPEC	CSpec::rmt_mh;

//起伏
ST_AXIS_ITEMS	CSpec::base_bh;
ST_AXIS_ITEMS	base_bh0 = {

	ID_BOOM_H,//INT32 axis_id;
	{//double notch_spd_f[N_NOTCH_MODE][N_NOTCH_MAX];		//ノッチ指令速度
		{0.0,0.33,0.1,0.2,0.33,0.33},
		{0.0,0.0,0.0,0.0,0.0,0.0},
		{0.0,0.0,0.0,0.0,0.0,0.0}
	},
	
	{//double notch_spd_r[N_NOTCH_MODE][N_NOTCH_MAX];		//ノッチ指令速度
		{-0.0,-0.33,-0.1,-0.2,-0.33,-0.33},
		{0.0,0.0,0.0,0.0,0.0,0.0},
		{0.0,0.0,0.0,0.0,0.0,0.0}
	},
	{2000,5000,10000,25000,0x7FFF,0x7FFF},		//INT16 notch_pad_f[N_NOTCH_MAX];	//GamePadのノッチ配分
	{-2000,-5000,-10000,-25000,0x8000,0x8000},	//INT16 notch_pad_r[N_NOTCH_MAX];	//GamePadのノッチ配分
	0.33,//double v_rated;定格速度（m/s, rad/s：100％)

	/*
	//INT32 notch_spd_nrm_f[N_NOTCH_MODE][N_NOTCH_MAX];	//ノッチ指令速度正規化100%=10000
	{{0,1000,3030,6061,10000,10000},{0,0,0,0,0,0},{0,0,0,0,0,0}},
	//INT32 notch_spd_nrm_r[N_NOTCH_MODE][N_NOTCH_MAX];//ノッチ指令速度正規化100%=10000
	{{0,-1000,-3030,-6061,-10000,-10000},{0,0,0,0,0,0},{0,0,0,0,0,0}},

	//double acc;加速度
	{0.22,0.0,0.0},
	//double dec;減速度
	{-0.22,0.0,0.0},
	//double t_acc;加時間
	{1.5,0.0,0.0},
	//double t_dec;減時間
	{1.5,0.0,0.0},

	//double motor_rps;モータ定格回転数（rps　100%）
	0.0,
	//double motor_rpm;モータ定格回転数（rpm　100%）
	0.0,
	//double gear_ratio;減速比(ドラム回転 / モータ回転）
	0.0,
	//double drum_rps;定格ドラム回転速度（100％rps)
	0.0,
	//double drum_rpps;定格ドラム回転加速度（r/s~2)
	0.0,
	//double pos_limit_f[N_POS_LIMIT_TYPE];軸動作極限
	{0.0,0.0,0.0,0.0},
	//double pos_limit_r[N_POS_LIMIT_TYPE];軸動作極限
	{0.0,0.0,0.0,0.0},
	//double n_wire;		// ワイヤ掛数
	0.0,
	//double n_boom_wire;	// ワイヤ掛数ブーム部
	0.0,
	//double n_ex_wind;	// あだ巻数
	0.0,
	//double n_tread;		// ドラム溝数
	0.0,
	//double d_drum;		// １層ドラム径
	0.0,
	//double d_wire;		// ワイヤ径
	0.0,
	//double d_drum_layer;// ドラム径層加算値
	0.0,
	//double l_wire;		// 基準ワイヤ長(旋回はTTB円周
	0.0,
	//INT32 v100plc
	1000
	*/
};
ST_AUTO_SPEC	CSpec::auto_bh;
ST_REMOTE_SPEC	CSpec::rmt_bh;

//旋回
ST_AXIS_ITEMS	CSpec::base_sl;
ST_AXIS_ITEMS	base_sl0 = {
	ID_SLEW,//INT32 axis_id;
	{//double notch_spd_f[N_NOTCH_MODE][N_NOTCH_MAX];		//ノッチ指令速度
		{0.0,0.00175,0.0175,0.035,0.035,0.035},
		{0.0,0.0013,0.013,0.026,0.026,0.026},
		{0.0,0.0,0.0,0.0,0.0,0.0}
	},
	{//double notch_spd_r[N_NOTCH_MODE][N_NOTCH_MAX];		//ノッチ指令速度
		{0.0,-0.00175,-0.0175,-0.035,-0.035,-0.035},
		{0.0,-0.0013,-0.013,-0.026,-0.026,-0.026},
		{0.0,0.0,0.0,0.0,0.0,0.0}
	},
	{2000,5000,10000,25000,0x7FFF,0x7FFF},		//INT16 notch_pad_f[N_NOTCH_MAX];						//GamePadのノッチ配分
	{-2000,-5000,-10000,-25000,0x8000,0x8000},	//INT16 notch_pad_r[N_NOTCH_MAX];						//GamePadのノッチ配分

	0.0349,	//1/3rpm double v_rated;定格速度（m/s, rad/s：100％)
	/*
	//INT32 notch_spd_nrm_f[N_NOTCH_MODE][N_NOTCH_MAX];	//ノッチ指令速度正規化100%=10000
	{{0,500,5000,10000,10000,10000},{0,371,3714,7429,7429,7429},{0,0,0,0,0,0}},
	//INT32 notch_spd_nrm_r[N_NOTCH_MODE][N_NOTCH_MAX];//ノッチ指令速度正規化100%=10000
	{{0,-500,-5000,-10000,-10000,-10000},{0,-371,-3714,-7429,-7429,-7429},{0,0,0,0,0,0}},

	//double acc;加速度
	{0.02971,0.01750,0.0},
	//double dec;減速度
	{-0.02971,-0.01750,0.0},
	//double t_acc;加時間
	{12.0,20.0,0.0},
	//double t_dec;減時間
	{12.0,20.0,0.0},

	//double motor_rps;モータ定格回転数（rps　100%）
	0.0,
	//double motor_rpm;モータ定格回転数（rpm　100%）
	0.0,
	//double gear_ratio;減速比(ドラム回転 / モータ回転）
	0.0,
	//double drum_rps;定格ドラム回転速度（100％rps)
	0.0,
	//double drum_rpps;定格ドラム回転加速度（r/s~2)
	0.0,
	//double pos_limit_f[N_POS_LIMIT_TYPE];軸動作極限
	{0.0,0.0,0.0,0.0},
	//double pos_limit_r[N_POS_LIMIT_TYPE];軸動作極限
	{0.0,0.0,0.0,0.0},
	//double n_wire;		// ワイヤ掛数
	0.0,
	//double n_boom_wire;	// ワイヤ掛数ブーム部
	0.0,
	//double n_ex_wind;	// あだ巻数
	0.0,
	//double n_tread;		// ドラム溝数
	0.0,
	//double d_drum;		// １層ドラム径
	0.0,
	//double d_wire;		// ワイヤ径
	0.0,
	//double d_drum_layer;// ドラム径層加算値
	0.0,
	//double l_wire;		// 基準ワイヤ長(旋回はTTB円周
	0.0,
	//INT32 v100plc
	1000
	*/
};
ST_AUTO_SPEC	CSpec::auto_sl;
ST_REMOTE_SPEC	CSpec::rmt_sl;

//走行
ST_AXIS_ITEMS	CSpec::base_gt;
ST_AXIS_ITEMS	base_gt0 = {
	ID_GANTRY,//INT32 axis_id;
	{//double notch_spd_f[N_NOTCH_MODE][N_NOTCH_MAX];		//ノッチ指令速度
		{0.0,0.058,0.175,0.350,0.580,0.580},
		{0.0,0.0,0.0,0.0,0.0,0.0},
		{0.0,0.0,0.0,0.0,0.0,0.0}
	},
	{//double notch_spd_r[N_NOTCH_MODE][N_NOTCH_MAX];		//ノッチ指令速度
		{0.0,-0.058,-0.175,-0.350,-0.580,-0.580},
		{0.0,0.0,0.0,0.0,0.0,0.0},
		{0.0,0.0,0.0,0.0,0.0,0.0}
	},
	
	{2000,5000,10000,25000,0x7FFF,0x7FFF},		//INT16 notch_pad_f[N_NOTCH_MAX];	//GamePadのノッチ配分
	{-2000,-5000,-10000,-25000,0x8000,0x8000},	//INT16 notch_pad_r[N_NOTCH_MAX];	//GamePadのノッチ配分
	
	0.58,//double v_rated;定格速度（m/s, rad/s：100％)

	/*
	//INT32 notch_spd_nrm_f[N_NOTCH_MODE][N_NOTCH_MAX];	//ノッチ指令速度正規化100%=10000
	{{0,1000,3017,6034,10000,10000},{0,0,0,0,0,0},{0,0,0,0,0,0}},
	//INT32 notch_spd_nrm_r[N_NOTCH_MODE][N_NOTCH_MAX];//ノッチ指令速度正規化100%=10000
	{{0,-1000,-3017,-6034,-10000,-10000},{0,0,0,0,0,0},{0,0,0,0,0,0}},

	//double acc;加速度
	{0.0644,0.0644,0.0},
	//double dec;減速度
	{-0.0644,-0.116,0.0},
	//double t_acc;加時間
	{9.0,9.0,0.0},
	//double t_dec;減時間
	{9.0,5.0,0.0},

	//double motor_rps;モータ定格回転数（rps　100%）
	0.0,
	//double motor_rpm;モータ定格回転数（rpm　100%）
	0.0,
	//double gear_ratio;減速比(ドラム回転 / モータ回転）
	0.0,
	//double drum_rps;定格ドラム回転速度（100％rps)
	0.0,
	//double drum_rpps;定格ドラム回転加速度（r/s~2)
	0.0,
	//double pos_limit_f[N_POS_LIMIT_TYPE];軸動作極限
	{0.0,0.0,0.0,0.0},
	//double pos_limit_r[N_POS_LIMIT_TYPE];軸動作極限
	{0.0,0.0,0.0,0.0},
	//double n_wire;		// ワイヤ掛数
	0.0,
	//double n_boom_wire;	// ワイヤ掛数ブーム部
	0.0,
	//double n_ex_wind;	// あだ巻数
	0.0,
	//double n_tread;		// ドラム溝数
	0.0,
	//double d_drum;		// １層ドラム径
	0.0,
	//double d_wire;		// ワイヤ径
	0.0,
	//double d_drum_layer;// ドラム径層加算値
	0.0,
	//double l_wire;		// 基準ワイヤ長(旋回はTTB円周
	0.0,
	//INT32 v100plc
	1000
	*/
};
ST_AUTO_SPEC	CSpec::auto_gt;
ST_REMOTE_SPEC	CSpec::rmt_gt;

//補巻
ST_AXIS_ITEMS	CSpec::base_ah;
ST_AXIS_ITEMS	base_ah0 = {
	ID_AHOIST,//INT32 axis_id;
	/*
	//double notch_spd_f[N_NOTCH_MODE][N_NOTCH_MAX];		//ノッチ指令速度
	{{0.0,0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0,0.0}},
	//double notch_spd_r[N_NOTCH_MODE][N_NOTCH_MAX];		//ノッチ指令速度
	{{0.0,0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0,0.0}},
	//INT32 notch_spd_nrm_f[N_NOTCH_MODE][N_NOTCH_MAX];	//ノッチ指令速度正規化100%=10000
	{{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0}},
	//INT32 notch_spd_nrm_r[N_NOTCH_MODE][N_NOTCH_MAX];//ノッチ指令速度正規化100%=10000
	{{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0}},
	//INT16 notch_pad_f[N_NOTCH_MAX];						//GamePadのノッチ配分
	{2000,5000,10000,25000,0x7FFF,0x7FFF},
	//INT16 notch_pad_r[N_NOTCH_MAX];						//GamePadのノッチ配分
	{-2000,-5000,-10000,-25000,0x8000,0x8000},
	//double acc;加速度
	{0.0,0.0,0.0},
	//double dec;減速度
	{0.0,0.0,0.0},
	//double t_acc;加時間
	{0.0,0.0,0.0},
	//double t_dec;減時間
	{0.0,0.0,0.0},
	//double v_rated;定格速度（m/s, rad/s：100％)
	0.0,
	//double motor_rps;モータ定格回転数（rps　100%）
	0.0,
	//double motor_rpm;モータ定格回転数（rpm　100%）
	0.0,
	//double gear_ratio;減速比(ドラム回転 / モータ回転）
	0.0,
	//double drum_rps;定格ドラム回転速度（100％rps)
	0.0,
	//double drum_rpps;定格ドラム回転加速度（r/s~2)
	0.0,
	//double pos_limit_f[N_POS_LIMIT_TYPE];軸動作極限
	{0.0,0.0,0.0,0.0},
	//double pos_limit_r[N_POS_LIMIT_TYPE];軸動作極限
	{0.0,0.0,0.0,0.0},
	//double n_wire;		// ワイヤ掛数
	0.0,
	//double n_boom_wire;	// ワイヤ掛数ブーム部
	0.0,
	//double n_ex_wind;	// あだ巻数
	0.0,
	//double n_tread;		// ドラム溝数
	0.0,
	//double d_drum;		// １層ドラム径
	0.0,
	//double d_wire;		// ワイヤ径
	0.0,
	//double d_drum_layer;// ドラム径層加算値
	0.0,
	//double l_wire;		// 基準ワイヤ長(旋回はTTB円周
	0.0
	*/
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
		memcpy_s(&st_struct, sizeof(ST_STRUCTURE), &st_struct0, sizeof(ST_STRUCTURE));
		memcpy_s(&base_mh, sizeof(ST_AXIS_ITEMS), &base_mh0, sizeof(ST_AXIS_ITEMS));
		memcpy_s(&base_bh, sizeof(ST_AXIS_ITEMS), &base_bh0, sizeof(ST_AXIS_ITEMS));
		memcpy_s(&base_sl, sizeof(ST_AXIS_ITEMS), &base_sl0, sizeof(ST_AXIS_ITEMS));
		memcpy_s(&base_gt, sizeof(ST_AXIS_ITEMS), &base_gt0, sizeof(ST_AXIS_ITEMS));
		memcpy_s(&base_ah, sizeof(ST_AXIS_ITEMS), &base_ah0, sizeof(ST_AXIS_ITEMS));
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

