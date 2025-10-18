#pragma once

#include <string>
#include "COMMON_DEF.h"
#include "SPEC_DEF.h"
#include "OTE_DEF.h"
#include "PLC_DEF.h"
#include "CVector3.h"
#include "CCrane.h"
#include "CFaults.h"

using namespace std;

/****************************************************************************/
/*	クレーンコントローラ（CC）構造体定義									*/	
/*	CCがセットする共有メモリ上の情報　      　　　　　　					*/			
/****************************************************************************/
#define SMEM_CRANE_STAT_CC_NAME			L"CRANE_STAT_CC"
#define SMEM_PLC_IO_NAME				L"PLC_IO"
#define SMEM_JOB_IO_NAME				L"JOB_IO"
#define SMEM_POL_INF_CC_NAME			L"POL_INF_CC"
#define SMEM_AGENT_INF_CC_NAME			L"AGENT_INF_CC"
#define SMEM_CS_INF_CC_NAME				L"CS_INF_CC"
#define SMEM_SIM_INF_CC_NAME			L"SIM_INF_CC"
#define SMEM_OTE_INF_NAME				L"OTE_INF"

#define MUTEX_CRANE_STAT_CC_NAME		L"MU_CRANE_STAT_CC"
#define MUTEX_PLC_IO_NAME				L"MU_PLC_IO"
#define MUTEX_JOB_IO_NAME				L"MU_JOB_IO"
#define MUTEX_POL_INF_CC_NAME			L"MU_POL_INF_CC"
#define MUTEX_AGENT_INF_CC_NAME			L"MU_AGENT_INF_CC"
#define MUTEX_CS_INF_CC_NAME			L"MU_CS_INF_CC"
#define MUTEX_SIM_INF_CC_NAME			L"MU_SIM_INF_CC"
#define MUTEX_OTE_INF_NAME				L"MU_OTE_INF"

//#	Environment
//## クレーン状態定義構造体
#pragma region CRANE STATUS

#define N_PC_FAULT_WORDS		16			//制御PC検出フォルトbitセットWORD数
#define N_PLC_FAULT_WORDS		32			//PLC検出フォルトbitセットWORD数
#define N_SWAY_DIR				4
#define N_DRUM_LAYER			10	//ドラ巻取り層数最大値

#define MODE_ENV_APP_PRODUCT	0	//実機モード
#define MODE_ENV_APP_EMURATOR	1	//エミュレータモード
#define MODE_ENV_APP_SIMURATION	2	//シミュレーションモード

#define N_FAULTS_HISTORY_BUF	200	//故障履歴バッファ数
typedef struct _ST_FAULT_RECORD {
	SYSTEMTIME systime;				//検出中故障リスト
	INT16 code;						//検出故障コード
	INT16 status;					//TriggerON,TriggerOFF,TriggerNON状態
}ST_FAULT_RECORD, * LPST_FAULT_RECORD;

typedef struct _ST_FAULTS_LIST {
	INT16 faults_detected_map[FAULT_TYPE::ALL][N_PLC_FAULT_BUF];	//検出中故障リスト
	INT16 codes_plc[N_OTE_PC_SET_PLC_FLT];							//発生中PLC故障コード
	INT16 codes_pc[N_OTE_PC_SET_PC_FLT];							//発生中PC故障コード
	ST_FAULT_RECORD history[N_FAULTS_HISTORY_BUF];					//故障履歴
	INT16 history_count=0;											//故障履歴数
	INT16 iw_history=0;												//書き込みポインタ
}ST_FAULTS_LIST, * LPST_FAULTS_LIST;

typedef struct _ST_CRANE_STAT {
	INT32		i_layer[MOTION_ID_MAX];			//ドラム現在層数
	double		n_layer[MOTION_ID_MAX];			//ドラム現在層の回転数
	double		l_drum[MOTION_ID_MAX];			//ドラムロープ巻取り量
	ST_MOVE_SET	vdk[MOTION_ID_MAX];				//ドラム回転速度　100%が1000
	ST_MOVE_SET	nd[MOTION_ID_MAX];				//ドラム回転数動作
	ST_MOVE_SET	vm[MOTION_ID_MAX];				//軸動作　引込は角度
	ST_MOVE_SET	d;								//主巻ポスト‐起伏シーブ間状態（距離・速度・加速度）
	ST_MOVE_SET r;								//旋回半径
	ST_MOVE_SET th;								//起伏角度
	ST_MOVE_SET	lrm;							//主巻ロープ
	double		m;								//吊荷質量
	ST_FAULTS_LIST fault_list;					//故障検出リスト
	INT32		abs_preset_cnt[MOTION_ID_MAX];	//プリセットしたときのアブソコーダカウント値	
} ST_CRANE_STAT, * LPST_CRANE_STAT;

typedef struct _ST_CC_ENV_INF {

	ST_DEVICE_CODE device_code;

	//タスクステータス
	UINT32	app_mode = MODE_ENV_APP_PRODUCT;
	UINT32	aux_mode = FUNC_DEACTIVE;
	bool is_tasks_standby_ok;					//タスクの立ち上がり確認
	bool is_crane_status_ok = false;			//クレーンステータス初期化完了
	
	//Periodical Update	：定周期更新
	DWORD env_act_count = 0;					//ヘルシー信号
	WORD operation_mode;						//運転モード　機上,リモート

	//Parameters
	int debug_mode;
	double Cdrm[MOTION_ID_MAX][N_DRUM_LAYER];	//ドラム1層円周　[0]はドラム1層目の円周
	double Ldrm[MOTION_ID_MAX][N_DRUM_LAYER];	//ドラム層巻取量 [0]はドラム1層目の巻取量
	double Lspan[MOTION_ID_MAX];				//全ロープ長,TTB周長,走行可能距離

	ST_CRANE_STAT crane_stat;					//クレーン状態

	//吊荷状態
	Vector3 rc;									//クレーン主巻吊点のクレーン基準点とのx,y,z相対座標
	Vector3 rl;									//主巻吊荷のクレーン吊点とのx,y,z相対座標
	Vector3 rcam_m;								//主巻振れセンサ検出x,y,z座標 m
	Vector3 sway_ph_expected[MOTION_ID_MAX];	//振れ位相評価値
	Vector3 sway_amp_expected[MOTION_ID_MAX];	//振れ振幅評価値

	double T;									//振周期		s
	double w;									//振角周波数	/s
	double w2;									//振角周波数の2乗
	double g_ratio_x = 2.0;						//振れ周期補正係数　√gの比率で調整
	double g_ratio_y = 2.0;						//振れ周期補正係数　√gの比率で調整

	bool is_fwd_endstop[MOTION_ID_MAX];			//正転極限判定
	bool is_rev_endstop[MOTION_ID_MAX];			//逆転極限判定

	INT32 OTE_req_status;						//操作端末要求状態

	INT32 notch0;								//0ノッチ判定総合

}ST_CC_ENV_INF, * LPST_CC_ENV_INF;
#pragma endregion  クレーン状態定義構造体

//## PLC IO定義構造体　PLC通信の情報　
#pragma region PLC IO

#define PRM_CC_PLC_CHK_COUNT		20//PLCヘルシーチェック用 AGENT SCAN×カウントmsec
typedef struct _ST_CC_PLC_IO {
	INT16 pc_ctrl_fb;								//PC制御受付状態
	INT16 plc_enable;								//PLC有効で端末権利有でOTEリモートモード
	INT32 ctrl_source;								//主幹状態
	ST_PLC_AXIS_SET stat_mh;
	ST_PLC_AXIS_SET stat_bh;
	ST_PLC_AXIS_SET stat_sl;
	ST_PLC_AXIS_SET stat_gt;
	ST_PLC_AXIS_SET stat_ah;

	double	weight;									//主巻荷重FB
	double	weight_ah;								//補巻荷重FB
	double	th_bh;									//起伏角 acos(R/Lm)
	double	dth_bh;									//起伏角速度
	double	h_mh;									//揚程
	double  r;										//半径

	INT16 buf_io_read[CC_MC_SIZE_W_READ];			//PLCからの読み取り信号生値
	INT16 buf_io_write[CC_MC_SIZE_W_WRITE];			//PLCへの書き込み信号生値

	INT16 mh_spd_mode;
	INT16 ah_spd_mode;
	INT16 bh_ope_mode;
	INT16 spare;
	double v_ratio[MOTION_ID_MAX] = { 1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0 };

	INT16 plc_pnl_io_fb[N_OTE_PNL_CTRL];				//PLC IO コントロールパネルFB信号

}ST_CC_PLC_IO, * LPST_CC_PLC_IO;

#pragma endregion PLC IO定義構造体

//## Scada状態定義構造体
//#	Scada

//## Job定義構造体
//### 動作要素　加速、定速、減速等の一連の動作は、この要素の組み合わせで構成します。
#pragma region MTION STEP
//レシピ　Type
#define CTR_TYPE_WAIT_TIME					0x0000	//待機（時間経過待ち）
#define CTR_TYPE_WAIT_POS_HST				0x0101	//巻到達待ち
#define CTR_TYPE_WAIT_POS_GNT				0x0102	//走行到達待ち
#define CTR_TYPE_WAIT_POS_TRY				0x0104	//横行到達待ち
#define CTR_TYPE_WAIT_POS_BH				0x0108	//引込到達待ち
#define CTR_TYPE_WAIT_POS_SLW				0x0110	//旋回到達待ち
#define CTR_TYPE_WAIT_LAND					0x0120	//着床待ち
#define CTR_TYPE_WAIT_PH_1SHOT				0x0201	//位相待ち ONE SHOT
#define CTR_TYPE_WAIT_PH_2SHOT_FIRST		0x0202	//2INCH初回位相待ち
#define CTR_TYPE_WAIT_PH_2SHOT_SECOND		0x0204	//位相待ち TWO INCH　2回目
#define CTR_TYPE_VOUT_TIME					0x1000  //ステップ速度　時間完了
#define CTR_TYPE_VOUT_V						0x1001  //ステップ速度　速度到達完了
#define CTR_TYPE_VOUT_POS					0x1002  //ステップ速度　位置到達完了
#define CTR_TYPE_VOUT_PHASE     			0x1004  //ステップ速度　位相到達完了
#define CTR_TYPE_VOUT_LAND					0x1008  //ステップ速度　着床完了
#define CTR_TYPE_VOUT_TIME_1SHOT			0x1010  //ステップ速度　時間完了
#define CTR_TYPE_VOUT_TIME_2SHOT1			0x1020  //ステップ速度　時間完了
#define CTR_TYPE_VOUT_TIME_2SHOT2			0x1040  //ステップ速度　時間完了
#define CTR_TYPE_AOUT_TIME					0x2000  //加速速度　時間完了
#define CTR_TYPE_AOUT_V						0x2001  //加速速度　速度到達完了
#define CTR_TYPE_AOUT_POS					0x2002  //加速速度　位置到達完了
#define CTR_TYPE_AOUT_PHASE     			0x2004  //加速速度　位相到達完了
#define CTR_TYPE_AOUT_LAND					0x2008  //加速速度　着床完了
#define CTR_TYPE_FINE_POS					0x8001	//微小位置合わせ
#define CTR_TYPE_SMART_SLOW_DOWN			0x8010	//スマートスローダウン

#define TIME_LIMIT_CONFIRMATION				0.1		//パターン出力調整時間 秒
#define TIME_LIMIT_FINE_POS					10.0	//微小位置合わせ制限時間 秒
#define TIME_LIMIT_ERROR_DETECT				120		//異常検出時間
#define N_STEP_OPTION_MAX					8
#define PRM_1SHOT_PH_ADJUST					1.0		//1SHOT振れ止めの時間調整値　!!暫定　理屈未解析20241125

//オプションインデックス定義
//double
#define ID_STEP_OPT_ONESHOT_PH					0		//起動位相のdouble配列インデックス　SINGLE
#define ID_STEP_OPT_PHASE_FWD					0		//起動位相の配列インデックス　DOUBLE　正方向用
#define ID_STEP_OPT_PHASE_REV					1		//起動位相の配列インデックス　DOUBLE　逆方向用
#define ID_STEP_OPT_PHASE_CHK_RANGE				2		//起動位相の配列インデックス　DOUBLE　逆方向用
//int
#define ID_STEP_OPT_DIR							0		//動作方向指定のint配列インデックス
#define ID_STEP_OPT_VFB_DELAY_COUNT				1		//速度FB遅れ時間カウント値

typedef struct stMotionStep {	//運動要素
	//recipe
	int type;								//制御種別
	ST_MOVE_SET mtg;						//動作目標
	double _t;								//予定継続時間
	int time_count;							//予定継続時間のカウンタ変換値
	double opt_d[N_STEP_OPTION_MAX];		//オプションdouble
	int opt_i[N_STEP_OPTION_MAX];			//オプションint
	//status
	int act_count;
	int status;
}ST_MOTION_STEP, * LPST_MOTION_STEP;

#pragma endregion 動作要素定義構造体
//### 動作シーケンス 単軸の目標状態に移行する動作パターンを動作要素の組み合わせで実現します
#pragma region MTION SEQUENCE
#define M_STEP_MAX	32
#define SEQ_RECIPE_OPTION_N							8
#define SEQ_RECIPE_OPT_I_DIR_1SHOT					0
#define SEQ_RECIPE_OPT_I_DIR_2INCH					1
#define SEQ_RECIPE_OPT_I_VTOP_COUNT_2INCH			2
#define SEQ_RECIPE_OPT_I_PH_WAIT_COUNT_2INCH		3

typedef struct _ST_MOTION_SEQ {	
	//CS set
	int Axis_id;
	int motion_type;
	int option_i;								//オプション条件
	int direction;								//動作方向(-1,0,+1)
	int n_step;									//動作構成要素数
	int time_limit;								//タイムオーバー判定値
	ST_MOTION_STEP steps[M_STEP_MAX];			//動作定義要素配列

	//Agent set
	int i_hot_step;								//実行中要素配列index -1で完了
	int sequence_act_count;						//動作実行時間カウントカウント数
	int seq_status;								//
	int opt_agent[SEQ_RECIPE_OPTION_N];
	//int fin_code;								//完了コード
}ST_MOTION_SEQ, * LPST_MOTION_SEQ;
#pragma endregion 軸動作シーケンス定義構造体
//### 動作内容 目的動作を軸動作シーケンスの組み合わせで実現します
//		! COMMAND:1つのJOBを、複数のコマンドで構成	PICK GRAND PARK
#pragma region MTION COMMAND
/*** コマンド種類 ***/
#define COM_TYPE_MASK			0x0F00      
#define COM_TYPE_PICK			0x0100
#define COM_TYPE_GRND			0x0200
#define COM_TYPE_PARK			0x0400
#define COM_TYPE_FROM_TO		0x0800
#define COM_TYPE_NON 			0x0000

/*** 状態種類 ***/
#define STAT_MASK               0x00FF      //ステータス部マスク
#define STAT_NA					0x0000      //適用不可
#define STAT_MANUAL				0x0000      //適用不可
#define STAT_STANDBY			0x0001      //スタンバイ
#define STAT_TRIGED             0x0002      //起動済実行待ち
#define STAT_ACTIVE             0x0004      //実行中報告
#define STAT_SUSPENDED          0x0008      //一時停止報告

#define STAT_ABOTED             0x0120      //キャンセル
#define STAT_END				0x0020      //正常完了
#define STAT_ABNORMAL_END       0x8020      //異常完了
#define STAT_SEQ_ERR_END		0x2020      //シーケンス異常完了

#define STAT_REQ_WAIT           0x0080      //要求待ち
#define STAT_POSITIVE			0x1000		//OK
#define STAT_SUCCEED			0x1000		//成功
#define STAT_ACK				0x1001		//正常受付
#define STAT_NAK				0x8000		//NG
#define STAT_TIME_OVER			0x8010		//タイムオーバー

#define STAT_LOGICAL_ERROR		0x8004      //整合性異常
#define STAT_CODE_ERROR			0x8008      //適合コード無し

typedef struct StPosTargets {
	double pos[MOTION_ID_MAX];
	bool is_held[MOTION_ID_MAX];				//目標位置ホールド中フラグ
}ST_POS_TARGETS, * LPST_POS_TARGETS;

typedef struct StCommandCode {
	int i_list;
	int i_job;
	int type;
}ST_COM_CODE, * LPST_COM_CODE;

#define COM_RECIPE_OPTION_N							8
#define COM_RECIPE_OPT_D_PH_1SHOT					0
#define COM_RECIPE_OPT_D_PH_2INCH_FWD				0
#define COM_RECIPE_OPT_D_PH_2INCH_REV				1
#define COM_RECIPE_OPT_D_VTOP_1SHOT					2
#define COM_RECIPE_OPT_D_VTOP_2INCH					3

#define COM_RECIPE_OPT_I_DIR_1SHOT					0
#define COM_RECIPE_OPT_I_DIR_2INCH					1
#define COM_RECIPE_OPT_I_VTOP_COUNT_2INCH			2
#define COM_RECIPE_OPT_I_PH_WAIT_COUNT_2INCH		3

typedef struct _ST_COMMAND_SET {
	//POLICY SET
	ST_COM_CODE com_code;
	int n_seq;									//コマンドで動作する軸の数
	int seq_mode[MOTION_ID_MAX];				//コマンド動作する軸のID配列（MOTION　RECIPEの対象配列）L_ON or L_OFF
	ST_MOTION_SEQ seq[MOTION_ID_MAX];
	ST_POS_TARGETS target;							//目標位置	

	//POLICY or AGENT SET
	int option_i[COM_RECIPE_OPTION_N];
	double option_d[COM_RECIPE_OPTION_N];

	//AGENT SET
	int com_status;

	SYSTEMTIME time_start;
	SYSTEMTIME time_end;

}ST_COMMAND_SET, * LPST_COMMAND_SET;
#pragma endregion 動作内容定義構造体

/****************************************************************************/
//### 運転内容 動作内容の組み合わせで運転内容を実現します
//		! JOB:From-Toの搬送作業
#pragma region JOB
#define JOB_COMMAND_MAX				10			//　JOBを構成するコマンド最大数
#define MODE_PC_CTRL		0x00000001
#define MODE_ANTISWAY		0x00010000
#define MODE_RMOTE_PANEL	0x00000100

#define ID_JOBIO_COMTYPE_PAUSE				0
#define ID_JOBIO_COMTYPE_PICK				1
#define ID_JOBIO_COMTYPE_GRND				2
#define ID_JOBIO_COMTYPE_PARK				3
#define ID_JOBIO_COMTYPE_ANTISWAY			4

#define CODE_JOBIO_COMTYPE_ANTISWAY			-1

typedef struct _ST_JOB_SET {
	//CS 構成 JOBセット
	int list_id;								//登録されているJOB list配列のインデックスid
	int job_id;										//登録されているJOB list内job配列のインデックスid
	int type;									//JOB種別（JOB,半自動,OPERATION））
	int code;									//JOB識別コード
	int n_com;
	ST_POS_TARGETS targets[JOB_COMMAND_MAX];		//目標位置
	int com_type[JOB_COMMAND_MAX];

	//POLICY 構成コマンド
	ST_COMMAND_SET com[JOB_COMMAND_MAX];	//コマンドのレシピ
	int i_hot_com;
	int status;									//JOBの状態
	SYSTEMTIME time_start;
	SYSTEMTIME time_end;
}ST_JOB_SET, * LPST_JOB_SET;

typedef struct _ST_JOB_ORDER {
	int type;									//JOB種別（JOB,半自動,OPERATION））
	int job_code;
}ST_JOB_ORDER, * LPST_JOB_ORDER;

//JOB LIST
#define N_JOB_LIST						3				//JOB LIST登録数
#define ID_JOBTYPE_JOB					0				//JOB Type index番号
#define ID_JOBTYPE_SEMI					1				//SEMIAUTO Type index番号
#define ID_JOBTYPE_ANTISWAY				2				//FB ANTISWAY Type index番号
#define JOB_REGIST_MAX					10			//　JOB登録最大数

typedef struct _ST_JOB_LIST {
	int id;
	int type;									//JOB種別（JOB,半自動）
	int n_job;								   //Job数
	ST_JOB_SET	 job[JOB_REGIST_MAX];			//登録job
	ST_JOB_ORDER order[JOB_REGIST_MAX];			//要求job

	int i_job_hot;								//次完了待ちJob(実行中or待機中）	  id
	int status[JOB_REGIST_MAX];					//JOB LISTの状態
}ST_JOB_LIST, * LPST_JOB_LIST;

typedef struct _ST_JOB_IO {
	ST_JOB_LIST	job_list[N_JOB_LIST];
}ST_JOB_IO, * LPST_JOB_IO;

#pragma endregion 作業内容（JOB)定義構造体 

//## Policy定義構造体
#pragma region POLICY
#define FAULT_MAP_W_SIZE	64	//フォルトマップサイズ
typedef struct _ST_CC_POL_INF {

	WORD pc_fault_map[N_PC_FAULT_BUF];

}ST_CC_POL_INF, * LPST_CC_POL_INF;
#pragma endregion Policy情報定義構造体

//## Agent	情報定義構造体
#pragma region AGENT
/*** ジョブ,コマンドステータス ***/
//auto_ongoing用
#define AUTO_TYPE_JOB_MASK				0x7000      //JOB種別部マスク
#define AUTO_TYPE_SEL_MASK				0xf000      //自動種別部マスク
#define AUTO_TYPE_MANUAL				0x0000
#define AUTO_TYPE_JOB					0x1000      //種別JOB
#define AUTO_TYPE_SEMIAUTO				0x2000      //種別半自動
#define AUTO_TYPE_OPERATION				0x4000      //種別クレーン操作
#define AUTO_TYPE_FB_ANTI_SWAY			0x8000
//antisway_on_going用
#define ANTISWAY_ALL_MANUAL				0x00000000      //振れ止め無し
#define ANTISWAY_BH_ACTIVE				0x00000001      //引込振止実行中
#define ANTISWAY_BH_PAUSED				0x00000002      //引込振止一時停止
#define ANTISWAY_BH_COMPLETE			0x00000008      //引込振止完了
#define ANTISWAY_SLEW_ACTIVE			0x00000010      //旋回振止実行
#define ANTISWAY_SLEW_PAUSED			0x00000020      //旋回振止一時停止
#define ANTISWAY_SLEW_COMPLETE			0x00000080      //旋回振止完了 
//axis_status用
#define AG_AXIS_STAT_FB0				0x000000001	 //停止中
#define AG_AXIS_STAT_PC_ENABLE			0x800000000	 //PC指令可
#define AG_AXIS_STAT_AUTO_ENABLE		0x100000000	 //自動可
#define AG_AXIS_STAT_ANTISWAY_ENABLE	0x200000000	 //振れ止め可

typedef struct _ST_CC_AXIS_CTRL {
	double dist_for_target;
	int auto_active;
	int status;
	int notch_ref;
	double v_ref;
}ST_CC_AXIS_CTRL, * LPST_CC_AXIS_CTRL;

typedef struct _ST_CC_AGENT_INF {
	//プロセス外から参照可能
	INT16 pc_ctrl_mode;								//PCからの指令で動作させる軸の指定
	INT16 pc_ctrl_mode2plc = 0;							//PLCへのPC制御フラグ出力信号
	ST_CC_AXIS_CTRL st_mh_ctrl;
	ST_CC_AXIS_CTRL st_bh_ctrl;
	ST_CC_AXIS_CTRL st_sl_ctrl;
	ST_CC_AXIS_CTRL st_gt_ctrl;
	ST_CC_AXIS_CTRL st_ah_ctrl;

	//プロセス内参照用
	int command_count;								//コマンドレシピ作成呼び出し回数
	int auto_on_going;								//実行中の自動種別
	int antisway_on_going;							//実行中振れ止め
	LPST_COMMAND_SET	st_active_com_inf;			//実行中コマンドセット情報
	LPST_POS_TARGETS	auto_pos_target;			//自動目標位置
	LPST_COMMAND_SET    pCom_hot;					//実行中コマンドセット
	LPST_JOB_SET		pJob_hot;

}ST_CC_AGENT_INF, * LPST_CC_AGENT_INF;

#pragma endregion AGENT情報定義構造体

//#	ClientService
//## CS状態定義構造体
#pragma region CSInfo_CC

#define CC_CS_CODE_OPEPNL_DEACTIVE	0
#define CC_CS_CODE_OPEPNL_ACTIVE	1

typedef struct _ST_CC_CS_CTRL {
	INT32		auto_mode;														//自動モード
	INT32		antisway_mode;													//振れ止めモード
	INT32		auto_status[MOTION_ID_MAX];
	INT32		ote_remote_status;												//
	INT32		ope_pnl_status = CC_CS_CODE_OPEPNL_DEACTIVE;					//操作端末の状態　0bit：端末信号有効
	ST_POS_TARGETS semi_auto_selected_target;									//半自動選択目標位置(振れ止め含む）
	INT16		ote_mh_spd_mode;												//主巻速度モード
	INT16		ote_bh_r_mode;													//引込モード
	INT32		remote_estop;
	ST_GPAD_IN gpad_in;	

}ST_CC_CS_CTRL, * LPST_CC_CS_CTRL;


typedef struct _ST_CC_CS_INF {
	//自動,遠隔設定（モード）
	ST_CC_CS_CTRL cs_ctrl;												
	//OTE関連
	double ote_camera_height_m;													//操作端末VIEWのカメラ設置高さ
	UN_LAMP_COM ote_pb_lamp[N_OTE_PNL_CTRL];									//端末ランプ表示指令
	UN_LAMP_COM ote_notch_lamp[N_OTE_PNL_NOTCH_DEVICE][N_OTE_PNL_NOTCH_PBL];	//端末ノッチランプ表示指令
	INT16	notch_pos[2][MOTION_ID_MAX];										//ノッチ指定値
	INT16	ote_mh_spd_mode;													//主巻速度モード
	INT16	ote_bh_r_mode;														//引込モード

	//JOB関連
	int command_type;													//PARK,PICK,GRND
	int tg_sel_trigger_z = L_OFF, tg_sel_trigger_xy = L_OFF;			//目標位置の設定入力（半自動PB、モニタタッチ）があったかどうかの判定値
	//int target_set_z = CS_SEMIAUTO_TG_SEL_FIXED, target_set_xy = CS_SEMIAUTO_TG_SEL_FIXED;		//Z座標目標位置確定
	LPST_JOB_SET p_active_job;
	int job_control_status;
	int grip_status;

}ST_CC_CS_INF, * LPST_CC_CS_INF;

#define N_JOB_TARGET_MAX	10
#define N_JOB_OPTION_MAX	10

typedef struct stClientRcvMSG {
	int req_code;
	int JOB_ID;
	double target[N_JOB_TARGET_MAX][MOTION_ID_MAX];
	int option[N_JOB_OPTION_MAX];
}ST_CLIENT_RCV_MSG, * LPST_CLIENT_RCV_MSG;

typedef struct stClientSndMSG {
	int fb_code;
	int JOB_ID;
	int option[N_JOB_OPTION_MAX];
}ST_CLIENT_SND_MSG, * LPST_CLIENT_SND_MSG;

#define N_CLIENT_MSG_HOLD_MAX	10

typedef struct stClientIO {
	int i_rcv_hot;
	int i_snd_hot;
	ST_CLIENT_RCV_MSG rmsg[N_CLIENT_MSG_HOLD_MAX];
	ST_CLIENT_SND_MSG smsg[N_CLIENT_MSG_HOLD_MAX];
}ST_CLIENT_IO, * LPST_CLIENT_IO;

#pragma endregion Client Service情報定義構造体

//## シミュレーション信号定義構造体 
#pragma region SIMULATOR

typedef struct _ST_CC_SIM_INF {
	DWORD mode;
	DWORD helthy_cnt;

	INT16 buf_io_read[CC_MC_SIZE_W_READ];			//PLC IFバッファSIMULATION結果(Simulation Mode)
	INT16 buf_io_write[CC_MC_SIZE_W_WRITE];			//PLC IFバッファSIMULATION結果(Emulator, Simulation Mode)

//	ST_AUX_ENV_INF aux_io;							//センサ,補機SIMULATION結果(Emulator, Simulation Mode)
//	ST_SWAY_SERVER_MSG rcv_msg;						//振れセンサセンサ受信バッファ計算値

	INT16 mlim_weight_AI;					//モーメントリミッタフック質量 AI入力計算値 
	INT16 mlim_r_AI;					//モーメントリミッタ半径AI入力計算値
	
	INT32 hcount_mh;					//主巻高速カウンタ計算値
	INT32 hcount_bh;					//引込高速カウンタ計算値
	INT32 hcount_sl;					//旋回高速カウンタ計算値
	INT32 absocoder_mh;					//主巻アブソコーダ計算値
	INT32 absocoder_gt;					//走行アブソコーダ計算値
	
	INT16 vfb_mh;						//主巻速度FB計算値
	INT16 vfb_bh;						//引込速度FB計算値
	INT16 vfb_sl;						//旋回速度FB計算値
	INT16 vfb_gt;						//走行速度FB計算値

	INT16 trq_ref_mh;					//主巻トルク指令計算値
	INT16 trq_ref_bh;					//引込トルク指令計算値

}ST_CC_SIM_INF, * LPST_CC_SIM_INF;
#pragma endregion シミュレーション信号定義構造体

//   操作端末情報定義構造体
typedef struct _ST_CC_OTE_CTRL {
	INT16		active_ote_mode;						//操作端末モード タブレット,,操作卓,リモート
	INT16		gpad_mode;								//ゲームパッドモード
	INT16		auto_mode;								//オートモード
	INT16		auto_sel;								//オートSEL
	INT32		id_ope_active = OTE_NON_OPEMODE_ACTIVE;	//ノッチ操作が有効な接続端末ID
	INT16		stop_req_mode;							//OTE停止要求モード	非常停止,主幹切
	INT16		active_ote_type;						//操作有効OTEのタイプ
	SOCKADDR_IN	addr_in_active_ote;
}ST_CC_OTE_CTRL, * LPST_CC_OTE_CTRL;
// OTEからの要求コード                              　         　	


#define CODE_CC_CS_OTE_COM_ERR_SOCK  0x0001	//通信エラー
#define CODE_CC_CS_OTE_COM_ERR_RCV   0x0002	//メッセージエラー
#define CODE_CC_CS_OTE_COM_ERR_SND   0x0004	//シーケンスエラー

typedef struct _ST_CC_OTE_INF {
//CS作業バッファ
	ST_OTE_U_MSG    st_msg_ote_u_rcv;
	ST_OTE_U_MSG    st_msg_ote_u_rcv_mon;
	ST_PC_M_MSG     st_msg_pc_m_rcv;
	ST_OTE_M_MSG    st_msg_ote_m_rcv;

	ST_PC_U_MSG     st_msg_pc_u_snd;
	ST_PC_M_MSG     st_msg_pc_m_snd;

	INT16		err_ote_comm;
	INT16		err_stat1;
	
	SOCKADDR_IN		addr_in_to_oteu;
	SOCKADDR_IN		addr_in_from_oteu;
	SOCKADDR_IN		addr_in_from_pcm;	
	SOCKADDR_IN		addr_in_from_otem;	
	
	UINT32	ope_ote_silent_cnt;		//操作有効端末からの受信無しでカウントアップ
	UINT32	mon_silent_cnt;  		//隣接PCからのマルチキャスト受信カウント無しでカウントアップ

	//CS共有情報
	ST_CC_OTE_CTRL st_ote_ctrl;		//操作端末制御情報
	ST_PC_U_BODY   st_body;			//送信バッファ作業用

}ST_CC_OTE_INF, * LPST_CC_OTE_INF;

