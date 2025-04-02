#pragma once

#include <string>
#include "COMMON_DEF.h"
#include "NET_DEF.h"
#include "SPEC_DEF.h"
#include "AUXEQ_DEF.h"
#include "OTE_DEF.h"
#include "PLC_DEF.h"
#include "CVector3.h"
#include <dinput.h>

using namespace std;

//  共有メモリステータス
#define	OK_SHMEM						0	// 共有メモリ 生成/破棄正常
#define	ERR_SHMEM_CREATE				-1	// 共有メモリ Create異常
#define	ERR_SHMEM_VIEW					-2	// 共有メモリ View異常
#define	ERR_SHMEM_NOT_AVAILABLE			-3	// 共有メモリ View異常
#define	ERR_SHMEM_MUTEX					-4	// 共有メモリ View異常

#define SMEM_DATA_SIZE_MAX				1000000	//共有メモリ割り当て最大サイズ　1Mbyte	

static char smem_dummy_buf[SMEM_DATA_SIZE_MAX];

/****************************************************************************/
/*共有メモリクラス定義														*/
class CSharedMem
{
public:
	CSharedMem();
	~CSharedMem();

	int smem_available;			//共有メモリ有効
	int data_size;				//データサイズ

	int create_smem(LPCTSTR szName, DWORD dwSize, LPCTSTR szMuName);
	int delete_smem();
	int clear_smem();

	wstring wstr_smem_name;

	HANDLE get_hmutex() { return hMutex; }
	LPVOID get_pMap() { return pMapTop; }

protected:
	HANDLE hMapFile;
	LPVOID pMapTop;
	DWORD  dwExist;

	HANDLE hMutex;
};
/****************************************************************************/

/****************************************************************************/
/*   補機信号定義構造体                                  　         　    */
/* 　AUXEQがセットする共有メモリ上の情報　      　　　　　　               */
/****************************************************************************/
#define SMEM_AUX_ENV_INF_NAME			L"AUX_ENV_INF"
#define SMEM_AUX_AGENT_INF_NAME			L"AUX_AGENT_INF"
#define SMEM_AUX_CS_INF_NAME			L"AUX_CS_INF"

#define MUTEX_AUX_ENV_INF_NAME			L"MU_AUX_ENV_INF"
#define MUTEX_AUX_AGENT_INF_NAME		L"MU_AUX_AGENT_INF"
#define MUTEX_AUX_CS_INF_NAME			L"MU_AUX_CS_INF"

typedef struct _ST_AUX_ENV_INF {

	ST_DEVICE_CODE device_code;
	DWORD helthy_cnt;

}ST_AUX_ENV_INF, * LPST_AUX_ENV_INF;

#define SENSOR_CAM_MAX				 2//カメラ数
#define SENSOR_TARGET_MAX            4//検出ターゲット最大数
#define TG_LAMP_NUM_MAX              3//ターゲット毎のランプ最大数
#define SWAY_FAULT_ITEM_MAX			 4//異常検出項目数

typedef struct _ST_AUX_AGENT_INF {
	DWORD proc_mode;
	DWORD helthy_cnt;

	char sensorID[4];
	WORD mode[SENSOR_TARGET_MAX];							//ターゲットサ検出モード
	WORD status[SENSOR_TARGET_MAX];							//ターゲットサ検出状態
	DWORD fault[SWAY_FAULT_ITEM_MAX];						//センサ異常状態
	double pix_size[SENSOR_TARGET_MAX][TG_LAMP_NUM_MAX];	//ターゲット検出PIXEL数（面積）
	double tilt[SENSOR_CAM_MAX][SID_N_AXIS];				//傾斜角
	double dtilt[SENSOR_CAM_MAX][SID_N_AXIS];				//傾斜角
	double th[SID_N_LOAD][MOTION_ID_MAX];					//吊荷振角			rad
	double th0[SID_N_LOAD][MOTION_ID_MAX];					//吊荷振角			rad
	double dth[SID_N_LOAD][MOTION_ID_MAX];					//吊荷振角速度		rad/s
	double dthw[SID_N_LOAD][MOTION_ID_MAX];					//吊荷振角速度/ω　	rad
	double ph[SID_N_LOAD][MOTION_ID_MAX];					//吊荷位相平面位相	rad
	double rad_amp2[SID_N_LOAD][MOTION_ID_MAX];				//吊荷振角振幅の2乗		rad2
	double cam_pix[SID_N_LOAD][MOTION_ID_MAX];				//振れセンサカメラPIX位置
}ST_AUX_AGENT_INF, * LPST_AUX_AGENT_INF;

typedef struct _ST_AUX_CS_INF {
	DWORD helthy_cnt;
	ST_AUX_COM_SERV_MSG st_msg_u_snd;
	ST_AUX_COM_CLI_MSG st_msg_u_rcv;
	SOCKADDR_IN addrin_snd;
	SOCKADDR_IN addrin_rcv;
	SOCKADDR_IN addrin_from;
}ST_AUX_CS_INF, * LPST_AUX_CS_INF;


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

typedef struct _ST_CC_ENV_INF {

	ST_DEVICE_CODE device_code;
	ST_SPEC spec;

	UINT32	aux_mode = FUNC_DEACTIVE;	
	
	DWORD helthy_cnt;
	ST_AUX_COM_SERV_MSG st_msg_u_rcv;
	ST_AUX_COM_CLI_MSG st_msg_u_snd;
	SOCKADDR_IN addrin_snd;
	SOCKADDR_IN addrin_rcv;
	SOCKADDR_IN addrin_from;

	//Event Update				:イベント条件で更新
	bool is_tasks_standby_ok;							//タスクの立ち上がり確認
	bool is_crane_status_ok = false;						//クレーンステータス初期化完了


	//Periodical Update			：定周期更新
	DWORD env_act_count = 0;								//ヘルシー信号
	WORD operation_mode;								//運転モード　機上,リモート
	int debug_mode;

	Vector3 rc;											//クレーン主巻吊点のクレーン基準点とのx,y,z相対座標
	Vector3 rl;											//主巻吊荷のクレーン吊点とのx,y,z相対座標
	Vector3 rcam_m;										//主巻振れセンサ検出x,y,z座標 m

	Vector3 rc_a;										//クレーン補巻吊点のクレーン基準点とのx,y,z相対座標
	Vector3 rl_a;										//補巻吊荷のクレーン吊点とのx,y,z相対座標
	Vector3 rcam_m_a;									//補巻振れセンサ検出x,y,z座標 m

	double mh_l;										//ロープ長
	double T;											//振周期		s
	double w;											//振角周波数	/s
	double w2;											//振角周波数の2乗

	double sway_ph_expected[MOTION_ID_MAX];				//振れ位相評価値
	double sway_amp_expected[MOTION_ID_MAX];			//振れ振幅評価値

	double ah_l;										//ロープ長
	double T_ah;										//振周期		s
	double w_ah;										//振角周波数	/s
	double w2_ah;										//振角周波数の2乗

	double R;											//旋回半径
	double g_ratio_x = 2.0;							//振れ周期補正係数　√gの比率で調整
	double g_ratio_y = 2.0;							//振れ周期補正係数　√gの比率で調整

	WORD faultPC[N_PC_FAULT_WORDS];						//PLC検出異常
	WORD faultPLC[N_PLC_FAULT_WORDS];					//制御PC検出異常

	bool is_fwd_endstop[MOTION_ID_MAX];					//正転極限判定
	bool is_rev_endstop[MOTION_ID_MAX];					//逆転極限判定

	INT32 OTE_req_status;								//操作端末要求状態

	INT32 notch0;										//0ノッチ判定総合

	double Cdr[MOTION_ID_MAX][N_DRUM_LAYER];	//ドラム1層円周
	double Ldr[MOTION_ID_MAX][N_DRUM_LAYER];	//ドラム層巻取量
	double Kdr[MOTION_ID_MAX][N_DRUM_LAYER];	//ドラム層円周倍率

	ST_MOVE_SET	axc[MOTION_ID_MAX];					//軸動作
	ST_MOVE_SET	d;									//主巻ポスト‐起伏シーブ間状態（距離・速度・加速度）
	ST_MOVE_SET	ph;									//φ
	ST_MOVE_SET	th;									//θ
	ST_MOVE_SET	hm0;								//主巻シーブ起伏高さ
	ST_MOVE_SET	ha0;								//補巻シーブ起伏高さ
	ST_MOVE_SET	nd[MOTION_ID_MAX];					//ドラム回転動作
	UINT32 i_layer[MOTION_ID_MAX];					//ドラム現在層数
	double n_layer[MOTION_ID_MAX];					//ドラム現在層巻取数
	double l_drum[MOTION_ID_MAX];					//ドラム巻取り量
	ST_MOVE_SET	lrm;								//主巻ロープ長
	ST_MOVE_SET	lra;								//補巻ロープ長

}ST_CC_ENV_INF, * LPST_CC_ENV_INF;
#pragma endregion  クレーン状態定義構造体

//## PLC IO定義構造体　PLC通信の情報　
#pragma region PLC IO
typedef struct _ST_CC_PLC_IO {
	INT32 mode;
	INT32 healthy_cnt;
	double v_fb[MOTION_ID_MAX];						//速度FB
	double v_fb_rpm[MOTION_ID_MAX];					//速度FB rpm
	INT16 v_fb_notch[MOTION_ID_MAX];				//速度FB ノッチ表現
	INT16 v_com_notch[MOTION_ID_MAX];				//速度指令 ノッチ表現
	double v_com_pc[MOTION_ID_MAX];					//PLCへの速度指令生値
	double nv_ref[MOTION_ID_MAX];					//PLCのINVへの速度指令出力
	double nv_tg[MOTION_ID_MAX];					//PLC目標速度
	double trq_fb_01per[MOTION_ID_MAX];				//トルクFB
	double pos[MOTION_ID_MAX];						//位置FB
	double weight;									//主巻荷重FB
	double weight_ah;								//補巻荷重FB
	double th_bh;									//起伏角 acos(R/Lm)
	double dth_bh;									//起伏角速度
	double lmh;										//主巻ロープ長
	double lah;										//補巻ロープ長

	INT16 brk[MOTION_ID_MAX];						//ブレーキ状態FB 0閉
	INT16 notch_auto[MOTION_ID_MAX];				//AGENT指令のノッチ位置
	INT32 endlim[MOTION_ID_MAX];					//極限センサ状態

	INT32 ctrl_source;								//主幹

	INT16 buf_io_read[CC_MC_SIZE_W_READ];			//PLCからの読み取り信号生値
	INT16 buf_io_write[CC_MC_SIZE_W_WRITE];			//PLCへの書き込み信号生値

	INT32 mh_spd_mode;
	INT32 ah_spd_mode;
	INT32 bh_ope_mode;
	double v_ratio[MOTION_ID_MAX] = { 1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0 };
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
	int axis_id;
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
#pragma endregion 動作内容定義構造体/****************************************************************************/
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

	WORD fault_map[FAULT_MAP_W_SIZE];

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

typedef struct _ST_CC_AGENT_INF {
	ST_COMMAND_SET st_as_comset;					//振れ止め用コマンドセット
	ST_POS_TARGETS auto_pos_target;					//自動目標位置
	double dist_for_target[MOTION_ID_MAX];			//目標までの距離
	int auto_on_going;								//実行中の自動種別
	int antisway_on_going;							//実行中振れ止め
	int pc_ctrl_mode;								//PCからの指令で動作させる軸の指定
	int auto_active[MOTION_ID_MAX];					//自動実行中フラグ(軸毎)
	int axis_status[MOTION_ID_MAX];					//各軸の状態（fb0,異常等）

	INT16 notch_ref[MOTION_ID_MAX] = { 0,0,0,0,0,0,0,0 };	//ノッチ指令値
	double v_ref[MOTION_ID_MAX];					//速度指令出力値

	ST_COMMAND_SET st_active_com_inf;				//実行中コマンドセット情報

	int as_count[MOTION_ID_MAX];					//振れ止めレシピ作成呼び出し回数
	int command_count;								//コマンドレシピ作成呼び出し回数

	LPST_COMMAND_SET    pCom_hot;					//実行中コマンドセット
	LPST_JOB_SET		pJob_hot;

}ST_CC_AGENT_INF, * LPST_CC_AGENT_INF;

#pragma endregion AGENT情報定義構造体

//#	ClientService
//## CS状態定義構造体
#pragma region CSInfo_CC
typedef struct _ST_CC_CS_INF {
	//自動,遠隔設定（モード）
	INT32		auto_mode;												//自動モード
	INT32		antisway_mode;											//振れ止めモード
	INT32		auto_status[MOTION_ID_MAX];
	INT32		ote_remote_status;										//操作端末の状態　0bit：端末信号有効

	ST_POS_TARGETS semi_auto_selected_target;							//半自動選択目標位置(振れ止め含む）

	double ote_camera_height_m;											//操作端末VIEWのカメラ設置高さ

	//UI関連
	ST_OTE_LAMP_COM ote_pb_lamp[N_OTE_PNL_PB];							//端末ランプ表示指令
	ST_OTE_LAMP_COM ote_notch_lamp[N_OTE_PNL_NOTCH];					//端末ランプ表示指令

	INT16	notch_pos[2][MOTION_ID_MAX];								//ノッチ指定値

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
typedef struct _ST_SIM_LOAD {//負荷
	double m;	//質量
	double wx;	//幅
	double dy;	//奥行
	double hz;	//高さ
}ST_SIM_LOAD, * LPST_SIM_LOAD;

#define SIM_N_SENSOR			128							//センサ数
typedef struct _ST_CC_SIM_INF {
	DWORD mode;
	DWORD helthy_cnt;
	INT16 plc_w[CC_MC_SIZE_W_WRITE];
	ST_AUX_ENV_INF sway_io;
	Vector3 L, vL;											//ﾛｰﾌﾟﾍﾞｸﾄﾙ(主巻振れ）
	Vector3 L2, vL2;										//ﾛｰﾌﾟﾍﾞｸﾄﾙ(補巻振れ）
	double a_fb[MOTION_ID_MAX];								//軸座標加速度fb
	double v_fb[MOTION_ID_MAX];								//軸座標速度fb
	double pos[MOTION_ID_MAX];								//軸座標位置fb
	double mtrq[MOTION_ID_MAX];								//モータトルクfb
	ST_SIM_LOAD load[MOTION_ID_MAX];						//軸負荷（巻は荷重）
	double rad_cam_x, rad_cam_y, w_cam_x, w_cam_y;			//カメラ座標振れ角,振れ角速度
	double kbh;												//引込半径に依存する速度、加速度補正係数
	ST_SWAY_SERVER_MSG rcv_msg;
	ST_SWAY_CLIENT_MSG snd_msg;
	INT32 sonsor[SIM_N_SENSOR];								//センサ状態

	ST_MOVE_SET	nd[MOTION_ID_MAX];							//ドラム回転動作
	ST_MOVE_SET	d;									//ポスト‐起伏シーブ間状態（距離・速度・加速度）
	ST_MOVE_SET	db;									//ジブポスト‐起伏シーブ間状態（距離・速度・加速度）
	ST_MOVE_SET	ph;									//φ
	ST_MOVE_SET	phb;								//φ
	ST_MOVE_SET	th;									//θ

	UINT32 i_layer[MOTION_ID_MAX];					//ドラム現在層数
	double n_layer[MOTION_ID_MAX];					//ドラム現在層巻取数
	double l_drum[MOTION_ID_MAX];					//ドラム巻取り量

	ST_MOVE_SET	lrm;								//主巻ロープ長
	ST_MOVE_SET	lra;								//補巻ロープ長

	double T;										//主巻振れ周期
	double Tah;										//補巻振れ周期
	double w;										//主巻振れ角周波数
	double wah;										//補巻振れ角周波数

	double tht_swx_mh, tht_swy_mh, tht_swx_ah, tht_swy_ah;

}ST_CC_SIM_INF, * LPST_CC_SIM_INF;
#pragma endregion シミュレーション信号定義構造体

//   操作端末信号定義構造体                                  　         　	
#define OTE_STOP_REQ_MODE_OFF			L_OFF
#define OTE_STOP_REQ_MODE_ESTOP			0x00000001
#define OTE_STOP_REQ_MODE_CTRL_OFF		0x00000002

//## 操作端末状態定義構造体 
typedef struct _ST_CC_OTE_INF {

	ST_OTE_U_MSG    st_msg_ote_u_rcv;
	ST_PC_M_MSG     st_msg_pc_m_rcv;
	ST_OTE_M_MSG    st_msg_ote_m_rcv;

	ST_PC_U_MSG     st_msg_pc_u_snd;
	ST_PC_M_MSG     st_msg_pc_m_snd;
	
	UINT32			id_ope_active = OTE_NON_OPEMODE_ACTIVE;	//ノッチ操作が有効な接続端末ID
	SOCKADDR_IN		addr_in_to_oteu;
	SOCKADDR_IN		addr_in_from_oteu;
	SOCKADDR_IN		addr_in_from_pcm;	
	SOCKADDR_IN		addr_in_from_otem;	
			
	UINT32			ope_ote_silent_cnt;	//操作有効端末からの受信無しでカウントアップ
	UINT32			mon_silent_cnt;  	//隣接PCからのマルチキャスト受信カウント無しでカウントアップ	
	UINT32			stop_req_mode;		//停止OTE要求モード	非常停止,主幹切

}ST_CC_OTE_INF, * LPST_CC_OTE_INF;

/****************************************************************************/
/*   操作端末側信号定義構造体                                    　			*/
/* 　OTEプロセス用共有メモリ上の情報　　　　　　　          　				*/
/****************************************************************************/
//OTEの状態
#define SMEM_OTE_ENV_NAME			L"OTE_ENV_IF"
#define SMEM_OTE_CS_INF_NAME		L"OTE_CS_IF"
#define SMEM_OTE_UI_NAME			L"OTE_UI"
#define SMEM_OTE_CC_IF_NAME			L"OTE_CC_IF"

#define MUTEX_OTE_ENV_NAME			L"MU_OTE_ENV_IF"
#define MUTEX_OTE_CS_INF_NAME		L"MU_OTE_CS_IF"
#define MUTEX_OTE_UI_NAME			L"MU_OTE_UI"
#define MUTEX_OTE_CC_IF_NAME		L"MU_OTE_CC_IF"

#pragma region OTE_INF
typedef struct _ST_OTE_ENV {
	ST_DEVICE_CODE device_code;

	UINT32			target_crane;			//接続対象クレーン
	UINT32			ote_mode;				//操作端末モード
	UINT32			ote_id;					//端末ID
	UINT32			grip_mode;				//グリップスイッチモード
	UINT32			gpad_mode = L_OFF;		//ゲームパッドモード
	DIJOYSTATE		pad_data;
}ST_OTE_ENV, * LPST_OTE_ENV;
#pragma endregion 

//## OTE_CS_INF
#pragma region OTE_CS_INF
typedef struct _ST_OTE_CS_INF {
	INT16 buf_io_read[CC_MC_SIZE_W_READ];	//PLCからの読み取り信号生値
	INT16 buf_io_write[CC_MC_SIZE_W_WRITE];	//PLCへの書き込み信号生値
}ST_OTE_CS_INF, * LPST_OTE_CS_INF;
#pragma endregion 操作卓PLCとの通信他

//## OTE_CC_IF
#pragma region OTE_CC_INF
typedef struct _ST_OTE_CC_IF {

	ST_PC_U_MSG     st_msg_pc_u_rcv;
	ST_PC_M_MSG     st_msg_pc_m_rcv;
	ST_OTE_M_MSG    st_msg_ote_m_rcv;

	ST_OTE_U_MSG    st_msg_ote_u_snd;
	ST_OTE_M_MSG    st_msg_ote_m_snd;

	UINT32			id_ope_active = OTE_NON_OPEMODE_ACTIVE;	//ノッチ操作が有効な接続端末ID
	SOCKADDR_IN		addr_in_to_oteu;
	SOCKADDR_IN		addr_in_from_oteu;
	SOCKADDR_IN		addr_in_from_pcm;
	SOCKADDR_IN		addr_in_from_otem;

	UINT32			ope_ote_silent_cnt;	//操作有効端末からの受信無しでカウントアップ
	UINT32			mon_silent_cnt;  	//隣接PCからのマルチキャスト受信カウント無しでカウントアップ	
	UINT32			stop_req_mode;		//停止OTE要求モード	非常停止,主幹切

}ST_OTE_CC_IF, * LPST_OTE_CC_IF;
#pragma endregion OTEのCCとの通信構造体

//## OTE_UI
#pragma region OTE_UI
typedef struct _ST_OTE_UI {
	DWORD notch_pos_gmpad[MOTION_ID_MAX];
	UINT16 pb_stat[N_OTE_PNL_PB];						//PB状態 OFFディレイカウント
	UINT16 notch_pb_stat[N_OTE_PNL_PB];					//PB状態 OFFディレイカウント
}ST_OTE_UI, * LPST_OTE_UI;
#pragma endregion 操作端末UI構造体








