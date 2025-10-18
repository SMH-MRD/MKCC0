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
	double Mr		= 700000.0;	//定格荷重Kg
	double Lb		= 84.0;		//ジブ長さ
	double Zbase	= 70.0;		//休止時揚程
	double Rbase	= 45.0;		//休止時半径
	double Nttl_bh	= 90.0;		//引込ドラム引込/主巻部回転数の和
	double Nbhb_base= 72.50;	//引込引込ドラム休止時回転数
	double Nbhm_base= 17.50;	//引込主巻ドラム休止時回転数
	double Nmh_base = 138.1;	//主巻ドラム休止時回転数
	double Ha		= 35.0;		//ポスト上部高さ(ジブピン-AフレームTOP）
	double Hp		= 38.5;		//ポスト下部高さ(ジブピン高さ)
	double d0		= 52.809;	//起伏上限時d	
	double Whook	= 10000.0;	//フック重量KG	

}ST_STRUCTURE, * LPST_STRUCTURE;

#define N_NOTCH_MAX					6	//ノッチ最大数　0ノッチを含む

#define N_NOTCH_MODE				3
#define NOTCH_MODE_BASE				0
#define NOTCH_MODE_1				1
#define NOTCH_MODE_2				2

#define N_ACC_MODE					3
#define ACC_MODE_BASE				0
#define ACC_MODE_1					1
#define ACC_MODE_2					2

#define N_POS_LIMIT_TYPE			4	//軸動作極限
#define END_LIMIT_POS				0
#define SPD_LIMIT_POS1				1
#define SPD_LIMIT_POS2				2
#define SPD_LIMIT_POS3				3
typedef struct _ST_AXIS_ITEMS {
	INT32 Axis_id;
	double	Notch_spd_f[N_NOTCH_MODE][N_NOTCH_MAX];		//ノッチ指令速度
	double	Notch_spd_r[N_NOTCH_MODE][N_NOTCH_MAX];		//ノッチ指令速度
	INT16	Notch_pad_f[N_NOTCH_MAX];					//GamePadのノッチ配分
	INT16	Notch_pad_r[N_NOTCH_MAX];					//GamePadのノッチ配分
	
	double V_rated;						// 定格速度（m/s, rad/s：100％)
	double Rpm_rated;					// 定格回転数（rpm：100％)
	double Kw_rated;					// 定格出力（kW：100％)
	double Gear_ratio;					// 減速比(モータ回転 /ドラム回転 ）
	double Ddrm0;						// ドラム径0
	double Ddrm1;						// ドラム径1
	double Nwire0;						// ロープ掛数
	double Nwire1;						// ロープ掛数
	double Ta0;							// 加速時間0
	double Ta1;							// 加速時間1
	double Td0;							// 減速時間0
	double Td1;							// 減速時間1
	double Ndmizo0;						// ドラム溝数0
	double Ndmizo1;						// ドラム溝数1
	double CntPgR;						// PG1回転カウント数×逓倍
	double CntPgDrumR;					// ドラム1回転PGカウント数
	double CntPgSet0;					// PGプリセット0カウント値
	double NdrmPgSet0;					// PGプリセット0ドラム回転数
	double CntAbsR;						// アブソコーダ1回転カウント数×入力軸ギア比
	double CntAbsSet0;					// アブソコーダプリセット0カウント値
	double NdrmAbsSet0;					// アブソコーダプリセット0ドラム回転数
	double Kp;							// 位置変換係数

	double dDdrm;						// ドラム1層追加半径
	double Trq_rated;					// 定格トルク（モータ軸）
	double Pwr_base;					// 定格必要出力
	double Trq_base;					// 定格必要トルク
	double Ia;							// 慣性モーメント
	double Lfull;						// フルスパン（m）
	double PosPreset;					// プリセット時の設定位置（m）


} ST_AXIS_ITEMS, * LPST_AXIS_ITEMS;

#define N_SWAY_LEVEL				3					//完了,トリガ,制限
#define N_POS_LEVEL					3					//完了,トリガ,制限
#define ID_SWAY_LV_COMP				0					//完了,トリガ,制限
#define ID_SWAY_LV_TRIG				1					//完了,トリガ,制限
#define ID_SWAY_LV_LIMIT			2					//完了,トリガ,制限

#define N_DELAY_PTN					5					//加減速時FB遅れ時間評価パターン数
#define ID_DLY_0START				0					//停止からの加速
#define ID_DLY_ACC_DEC				1					//加速中の減速切替時
#define ID_DLY_DEC_ACC				2					//減速中の加速切替時
#define ID_DLY_CNT_ACC				3					//定速からの加速時
#define ID_DLY_CNT_DEC				4					//定速からの減速時	

#define N_MEM_TARGET				8					//半自動目標ホールド数

typedef struct _ST_AUTO_SPEC
{
	double as_rad_level[N_SWAY_LEVEL];					//振れ止め判定　振れ角レベル(rad)
	double as_rad2_level[N_SWAY_LEVEL];					//振れ止め判定　振れ角レベル(rad^2) 
	double as_m_level[N_SWAY_LEVEL];					//振れ止め判定　振れ幅レベル(m)
	double as_m2_level[N_SWAY_LEVEL];					//振れ止め判定　振れ幅レベル(m^2)

	double as_pos_level[N_POS_LEVEL];					//位置決め制御判定　位置ずれレベル(m,rad)
	double pos_check_limit[N_POS_LEVEL];				//位置到達判定範囲(m,rad) 

	double sway_ph_chk_margin;							//振れ位相到達判定範囲
	double delay_time[N_DELAY_PTN];						//遅れ時間,無駄時間定義
	double default_target[N_MEM_TARGET];				//デフォルトの登録目標位置

}ST_AUTO_SPEC, * LPST_AUTO_SPEC;

typedef struct _ST_REMOTE_SPEC
{
	double Notch_spd_f[N_NOTCH_MODE][N_NOTCH_MAX];		//# ノッチ指令速度
	double Notch_spd_r[N_NOTCH_MODE][N_NOTCH_MAX];		//# ノッチ指令速度
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