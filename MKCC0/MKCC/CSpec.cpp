#include "CSpec.h"
#include "COMMON_DEF.h"

ST_STRUCTURE	CSpec::st_struct;//構造
ST_STRUCTURE	st_struct0 = {
	 /*wheel_span	*/ 20.0,	
	 /*leg_span		*/ 15.0,		
	 /*Hp			*/ 47.7,		
	 /*Lb			*/ 64.009,	
	 /*Lm			*/ 63.000,	
	 /*La_add		*/ 4.981,	
	 /*rad_mh_ah	*/ 2.371,	
	 /*La			*/ 66.566,	
	 /*rad_Lm_La	*/ 0.0537,	
	 /*Lmb			*/ 1.676,
	 /*Lma			*/ 4.981,
	 /*Lba			*/ 5.512,
	 /*Th_pba		*/ 2.00301,
	 /*Alpa_db		*/ 0.90266,
	 /*Alpa_b		*/ 0.02109,	
	 /*Alpa_a		*/ 0.05373,	
	 /*Phbm			*/ 2.20547,
	 /*Lp			*/ 25.174,
	 /*Php			*/ 1.6583,
	 /*Lmf			*/ 2.9000,
	 /*Laf			*/ 2.2910,
	 /*Th_rest		*/ -0.7113,	
	 /*D_min		*/ 41.425,	
	 /*D_rest		*/ 83.905,	
	 /*Hb_min		*/ 59.397,	
	 /*Hb_rest		*/ -41.786,	
	 /*Dttb			*/ 3.652,	
	 /*Kttb			*/ 0.08434,	
	 /*Nbh_drum		*/ 67.974,	
	 /*Load0_mh		*/ 11000.0,	
	 /*Load0_ah		*/ 1900.0	
};

//主巻
ST_AXIS_ITEMS	CSpec::base_mh;
ST_AXIS_ITEMS	base_mh0 = {
	//INT32 axis_id;
	ID_HOIST,
	//double notch_spd_f[N_NOTCH_MODE][N_NOTCH_MAX];		//ノッチ指令速度
	{{0.0,0.023,0.070,0.140,0.233,0.233},{0.0,0.060,0.182,0.364,0.605,0.605},{0.0,0.0,0.0,0.0,0.0,0.0}},
	//double notch_spd_r[N_NOTCH_MODE][N_NOTCH_MAX];		//ノッチ指令速度
	{{0.0,-0.023,-0.070,-0.140,-0.233,-0.233},{0.0,-0.060,-0.182,-0.364,-0.605,-0.605},{0.0,0.0,0.0,0.0,0.0,0.0}},
	//INT32 notch_spd_nrm_f[N_NOTCH_MODE][N_NOTCH_MAX];	//ノッチ指令速度正規化100%=10000
	{{0,987,3004,6009,10000,10000},{0,2567,7811,15622,26000,26000},{0,0,0,0,0,0}},
	//INT32 notch_spd_nrm_r[N_NOTCH_MODE][N_NOTCH_MAX];//ノッチ指令速度正規化100%=10000
	{{0,-987,-3004,-6009,-10000,-10000},{0,-2567,-7811,-15622,-26000,-26000},{0,0,0,0,0,0}},
	//INT16 notch_pad_f[N_NOTCH_MAX];						//GamePadのノッチ配分
	{5000,5000,20000,40000,60000,60000},
	//INT16 notch_pad_r[N_NOTCH_MAX];						//GamePadのノッチ配分
	{-5000,-5000. - 20000,-40000,-60000,-60000},
	//double acc;加速度
	{0.1533,0.1491,0.0},
	//double dec;減速度
	{0.-1533,-0.1491,0.0},
	//double t_acc;加時間
	{1.5,2.5,0.0},
	//double t_dec;減時間
	{1.5,2.5,0.0},
	//double v_rated;定格速度（m/s, rad/s：100％)
	0.233,
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
};
ST_AUTO_SPEC	CSpec::auto_mh;
ST_REMOTE_SPEC	CSpec::rmt_mh;

//起伏
ST_AXIS_ITEMS	CSpec::base_bh;
ST_AXIS_ITEMS	base_bh0 = {
	//INT32 axis_id;
	ID_BOOM_H,
	//double notch_spd_f[N_NOTCH_MODE][N_NOTCH_MAX];		//ノッチ指令速度
	{{0.0,0.33,0.1,0.2,0.33,0.33},{0.0,0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0,0.0}},
	//double notch_spd_r[N_NOTCH_MODE][N_NOTCH_MAX];		//ノッチ指令速度
	{{-0.0,-0.33,-0.1,-0.2,-0.33,-0.33},{0.0,0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0,0.0}},
	//INT32 notch_spd_nrm_f[N_NOTCH_MODE][N_NOTCH_MAX];	//ノッチ指令速度正規化100%=10000
	{{0,1000,3030,6061,10000,10000},{0,0,0,0,0,0},{0,0,0,0,0,0}},
	//INT32 notch_spd_nrm_r[N_NOTCH_MODE][N_NOTCH_MAX];//ノッチ指令速度正規化100%=10000
	{{0,-1000,-3030,-6061,-10000,-10000},{0,0,0,0,0,0},{0,0,0,0,0,0}},
	//INT16 notch_pad_f[N_NOTCH_MAX];						//GamePadのノッチ配分
	{5000,5000,20000,40000,60000,60000},
	//INT16 notch_pad_r[N_NOTCH_MAX];						//GamePadのノッチ配分
	{-5000,-5000. - 20000,-40000,-60000,-60000},
	//double acc;加速度
	{0.22,0.0,0.0},
	//double dec;減速度
	{-0.22,0.0,0.0},
	//double t_acc;加時間
	{1.5,0.0,0.0},
	//double t_dec;減時間
	{1.5,0.0,0.0},
	//double v_rated;定格速度（m/s, rad/s：100％)
	0.33,
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
};
ST_AUTO_SPEC	CSpec::auto_bh;
ST_REMOTE_SPEC	CSpec::rmt_bh;

//旋回
ST_AXIS_ITEMS	CSpec::base_sl;
ST_AXIS_ITEMS	base_sl0 = {
	//INT32 axis_id;
	ID_SLEW,
	//double notch_spd_f[N_NOTCH_MODE][N_NOTCH_MAX];		//ノッチ指令速度
	{{0.0,0.00175,0.0175,0.035,0.035,0.035},{0.0,0.0013,0.013,0.026,0.026,0.026},{0.0,0.0,0.0,0.0,0.0,0.0}},
	//double notch_spd_r[N_NOTCH_MODE][N_NOTCH_MAX];		//ノッチ指令速度
	{{0.0,-0.00175,-0.0175,-0.035,-0.035,-0.035},{0.0,-0.0013,-0.013,-0.026,-0.026,-0.026},{0.0,0.0,0.0,0.0,0.0,0.0}},
	//INT32 notch_spd_nrm_f[N_NOTCH_MODE][N_NOTCH_MAX];	//ノッチ指令速度正規化100%=10000
	{{0,500,5000,10000,10000,10000},{0,371,3714,7429,7429,7429},{0,0,0,0,0,0}},
	//INT32 notch_spd_nrm_r[N_NOTCH_MODE][N_NOTCH_MAX];//ノッチ指令速度正規化100%=10000
	{{0,-500,-5000,-10000,-10000,-10000},{0,-371,-3714,-7429,-7429,-7429},{0,0,0,0,0,0}},
	//INT16 notch_pad_f[N_NOTCH_MAX];						//GamePadのノッチ配分
	{5000,5000,20000,40000,60000,60000},
	//INT16 notch_pad_r[N_NOTCH_MAX];						//GamePadのノッチ配分
	{-5000,-5000. - 20000,-40000,-60000,-60000},
	//double acc;加速度
	{0.02971,0.01750,0.0},
	//double dec;減速度
	{-0.02971,-0.01750,0.0},
	//double t_acc;加時間
	{12.0,20.0,0.0},
	//double t_dec;減時間
	{12.0,20.0,0.0},
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
};
ST_AUTO_SPEC	CSpec::auto_sl;
ST_REMOTE_SPEC	CSpec::rmt_sl;

//走行
ST_AXIS_ITEMS	CSpec::base_gt;
ST_AXIS_ITEMS	base_gt0 = {
	//INT32 axis_id;
	ID_GANTRY,
	//double notch_spd_f[N_NOTCH_MODE][N_NOTCH_MAX];		//ノッチ指令速度
	{{0.0,0.058,0.175,0.350,0.580,0.580},{0.0,0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0,0.0}},
	//double notch_spd_r[N_NOTCH_MODE][N_NOTCH_MAX];		//ノッチ指令速度
	{{0.0,-0.058,-0.175,-0.350,-0.580,-0.580},{0.0,0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0,0.0}},
	//INT32 notch_spd_nrm_f[N_NOTCH_MODE][N_NOTCH_MAX];	//ノッチ指令速度正規化100%=10000
	{{0,1000,3017,6034,10000,10000},{0,0,0,0,0,0},{0,0,0,0,0,0}},
	//INT32 notch_spd_nrm_r[N_NOTCH_MODE][N_NOTCH_MAX];//ノッチ指令速度正規化100%=10000
	{{0,-1000,-3017,-6034,-10000,-10000},{0,0,0,0,0,0},{0,0,0,0,0,0}},
	//INT16 notch_pad_f[N_NOTCH_MAX];						//GamePadのノッチ配分
	{5000,5000,20000,40000,60000,60000},
	//INT16 notch_pad_r[N_NOTCH_MAX];						//GamePadのノッチ配分
	{-5000,-5000. - 20000,-40000,-60000,-60000},
	//double acc;加速度
	{0.0644,0.0644,0.0},
	//double dec;減速度
	{-0.0644,-0.116,0.0},
	//double t_acc;加時間
	{9.0,9.0,0.0},
	//double t_dec;減時間
	{9.0,5.0,0.0},
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
};
ST_AUTO_SPEC	CSpec::auto_gt;
ST_REMOTE_SPEC	CSpec::rmt_gt;

//補巻
ST_AXIS_ITEMS	CSpec::base_ah;
ST_AXIS_ITEMS	base_ah0 = {
	//INT32 axis_id;
	ID_AHOIST,
	//double notch_spd_f[N_NOTCH_MODE][N_NOTCH_MAX];		//ノッチ指令速度
	{{0.0,0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0,0.0}},
	//double notch_spd_r[N_NOTCH_MODE][N_NOTCH_MAX];		//ノッチ指令速度
	{{0.0,0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0,0.0}},
	//INT32 notch_spd_nrm_f[N_NOTCH_MODE][N_NOTCH_MAX];	//ノッチ指令速度正規化100%=10000
	{{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0}},
	//INT32 notch_spd_nrm_r[N_NOTCH_MODE][N_NOTCH_MAX];//ノッチ指令速度正規化100%=10000
	{{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0}},
	//INT16 notch_pad_f[N_NOTCH_MAX];						//GamePadのノッチ配分
	{5000,5000,20000,40000,60000,60000},
	//INT16 notch_pad_r[N_NOTCH_MAX];						//GamePadのノッチ配分
	{-5000,-5000. - 20000,-40000,-60000,-60000},
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

