#pragma once
#include "common_def.h"

/*** 仕様定義構造体 ***/
//制御パラメータ
#define ID_LV_DIR_CHECK_MARGIN		0	//移動方向判定マージン
#define ID_LV_COMP_POS_PASSED		1	//通過判定マージン
#define ID_LV_COMP_POS_RANGE		2	//位置決め完了判定マージン

//クレーン構造定義構造体
typedef struct _ST_STRUCTURE
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
}ST_STRUCTURE, * LPST_STRUCTURE;

#define N_NOTCH_MAX					6	//ノッチ最大数　0ノッチを含む

#define N_NOTCH_MODE				3
#define NOTCH_MODE_BASE				0
#define NOTCH_MODE_1				1
#define NOTCH_MODE_2				2

#define N_POS_LIMIT_TYPE			4	//軸動作極限
#define END_LIMIT_POS				0
#define SPD_LIMIT_POS1				1
#define SPD_LIMIT_POS2				2
#define SPD_LIMIT_POS3				3
typedef struct _ST_AXIS_ITEMS {
	double notch_spd_f[N_NOTCH_MODE][N_NOTCH_MAX];		//ノッチ指令速度
	double notch_spd_r[N_NOTCH_MODE][N_NOTCH_MAX];		//ノッチ指令速度
	double notch_spd_nrm_f[N_NOTCH_MODE][N_NOTCH_MAX];	//ノッチ指令速度正規化100%=1.0
	double notch_spd_nrm_r[N_NOTCH_MODE][N_NOTCH_MAX]; 	//ノッチ指令速度正規化100%=1.0

	INT16 notch_pad_f[N_NOTCH_MAX];						//GamePadのノッチ配分
	INT16 notch_pad_r[N_NOTCH_MAX];						//GamePadのノッチ配分

	double acc;			//加速度
	double dec;			//減速度
	double v_rated;		// 定格速度（m/s, rad/s：100％)
	double t_acc;		// 加速時間（100%)

	double motor_rps;	// モータ定格回転数（rps　100%）
	double motor_rpm;	// モータ定格回転数（rpm　100%）
	double gear_ratio;	// 減速比(ドラム回転 / モータ回転）
	double drum_rps;	// 定格ドラム回転速度（100％rps)
	double drum_rpps;	// 定格ドラム回転加速度（r/s~2)

	double pos_limit_f[N_POS_LIMIT_TYPE] ;//軸動作極限

	double n_wire;		// ワイヤ掛数
	double n_boom_wire;	// ワイヤ掛数ブーム部
	double n_ex_wind;	// あだ巻数
	double n_tread;		// ドラム溝数
	double d_drum;		// １層ドラム径
	double d_wire;		// ワイヤ径
	double d_drum_layer;// ドラム径層加算値
	double l_wire;		// 基準ワイヤ長(旋回はTTB円周
} ST_AXIS_ITEMS, * LPST_AXIS_ITEMS;

#define N_SWAY_LEVEL				3	//完了,トリガ,制限
#define N_POS_LEVEL					3	//完了,トリガ,制限
#define ID_SWAY_LV_COMP				0	//完了,トリガ,制限
#define ID_SWAY_LV_TRIG				1	//完了,トリガ,制限
#define ID_SWAY_LV_LIMIT			2	//完了,トリガ,制限

#define N_DELAY_PTN					5	//加減速時FB遅れ時間評価パターン数
#define ID_DLY_0START				0	//停止からの加速
#define ID_DLY_ACC_DEC				1	//加速中の減速切替時
#define ID_DLY_DEC_ACC				2	//減速中の加速切替時
#define ID_DLY_CNT_ACC				3	//定速からの加速時
#define ID_DLY_CNT_DEC				4	//定速からの減速時	

#define N_MEM_TARGET				8	//半自動目標ホールド数

typedef struct _ST_AUTO_SPEC
{
	double as_rad_level[N_SWAY_LEVEL];	//振れ止め判定　振れ角レベル(rad)
	double as_rad2_level[N_SWAY_LEVEL];	//振れ止め判定　振れ角レベル(rad^2) 
	double as_m_level[N_SWAY_LEVEL];	//振れ止め判定　振れ幅レベル(m)
	double as_m2_level[N_SWAY_LEVEL];	//振れ止め判定　振れ幅レベル(m^2)

	double as_pos_level[N_POS_LEVEL];	//位置決め制御判定　位置ずれレベル(m,rad)
	double pos_check_limit[N_POS_LEVEL];//位置到達判定範囲(m,rad) 

	double sway_ph_chk_margin;			//振れ位相到達判定範囲
	double delay_time[N_DELAY_PTN];		//遅れ時間,無駄時間定義
	double default_target[N_MEM_TARGET];//デフォルトの登録目標位置

}ST_AUTO_SPEC, * LPST_AUTO_SPEC;

typedef struct _ST_REMOTE_SPEC
{
	double notch_spd_f[N_NOTCH_MODE][N_NOTCH_MAX];		//# ノッチ指令速度
	double notch_spd_r[N_NOTCH_MODE][N_NOTCH_MAX];		//# ノッチ指令速度
	double notch_spd_nrm_f[N_NOTCH_MODE][N_NOTCH_MAX];	//# ノッチ指令速度正規化100%=1.0
	double notch_spd_nrm_r[N_NOTCH_MODE][N_NOTCH_MAX]; 	//# ノッチ指令速度正規化100%=1.0
}ST_REMOTE_SPEC, * LPST_REMOTE_SPEC;

class CSpec {
private:
	int crane_id;
public:
	CSpec(int _crane_id) {
		crane_id = _crane_id;
		setup(crane_id);
	};
	virtual ~CSpec() {};

	int get_crane() { return crane_id; }
	//共通
	static ST_STRUCTURE	st_struct;//構造

	//主巻
	static ST_AXIS_ITEMS	base_mh;
	static ST_AUTO_SPEC		auto_mh;
	static ST_REMOTE_SPEC	rmt_mh;

	//起伏
	static ST_AXIS_ITEMS	base_bh;
	static ST_AUTO_SPEC		auto_bh;
	static ST_REMOTE_SPEC	rmt_bh;

	//旋回
	static ST_AXIS_ITEMS	base_sl;
	static ST_AUTO_SPEC		auto_sl;
	static ST_REMOTE_SPEC	rmt_sl;

	//走行
	static ST_AXIS_ITEMS	base_gt;
	static ST_AUTO_SPEC		auto_gt;
	static ST_REMOTE_SPEC	rmt_gt;

	//補巻
	static ST_AXIS_ITEMS	base_ah;
	static ST_AUTO_SPEC		auto_ah;
	static ST_REMOTE_SPEC	rmt_ah;
	
	int setup(int cran_id);

};