#pragma once

#include <winsock.h>
#include "PLC_DEF.h"
#include "CPlc.h"
#include "COMMON_DEF.h"


//操作端末のテキスト,ボタン、ランプ数
#define N_OTE_PNL_CTRL				128
#define N_OTE_PC_MSG_WCHAR			64
#define N_OTE_PC_SET_PLC_FLT		63//最初の要素にはセットされているタイプのコードが入る
#define N_OTE_PC_SET_PC_FLT			15//最初の要素にはセットされているタイプのコードが入る
#define N_OTE_PNL_NOTCH_DEVICE		5
#define N_OTE_PNL_NOTCH_PBL			N_NOTCH_TBL

enum OTE_PNL_CTRLS {//OTEパネルPB INDEX
	estop = 0,
	syukan_on,
	syukan_off,
	remote,
	auto_mode,
	fault_reset,
	bypass,
	game_pad,
	asel_mh,		//自動選択主巻
	asel_bh,		//自動選択引込
	asel_sl,		//自動選択旋回
	asel_gt,		//自動選択走行
	asel_ah,		//自動選択補巻
	ote_type,
	notch_mh,
	notch_bh,
	notch_sl,
	notch_gt,
	notch_ah,
	notch_aux,
	mh_spd_mode,	//主巻モード
	bh_r_mode,		//引込モード
	alm_stop,
	motor_siren,
	hd_lamp1,
	hd_lamp2,
	hd_lamp3,
	hv_trolley,
	hv_gantry,
	hv_aux,
	irisA,
	irisB,
	camA_adjust,
	camB_adjust,
	camAselect,
	camBselect,
	buzzer,			//ブザー
	main_power,		//動力確立ランプ
	sl_auto_gr,		//自動給脂ランプ
	ope_ready,		//運転準備完了ランプ
	alm_lamp,		//警報ランプ
	fault_lamp,		//故障ランプ
	sl_brk,			//旋回ブレーキペダル
	pcpnl_active,	//PCパネル有効

	MAX
};

enum OTE_PNL_NOTCH {//OTEパネルノッチPB INDEX
	R5 = 0,
	R4, R3, R2, R1, N0, F1, F2, F3, F4, F5,
	NOTCH_MAX
};


//**************************************************************************************
//通信電文フォーマット
//**************************************************************************************
#define CODE_ITE_RES_ACK			1
#define CODE_ITE_RES_NAK			1

#define OTE_NON_OPEMODE_ACTIVE		0

#define OTE_STAT_MODE_MON			0	//端末モニタモード
#define OTE_STAT_MODE_OPE			2	//端末運転モード

#define OTE_STAT_TYPE_UNKOWN		0	//タイプ不定
#define OTE_STAT_TYPE_SITE			1	//機側
#define OTE_STAT_TYPE_ROOM			2	//操作室
#define OTE_STAT_TYPE_REMOTE		3	//遠隔地

#define OTE_CODE_REQ_MON			0x00000000	//モニタ要求
#define OTE_CODE_REQ_OPE_ACTIVE		0x00000001	//操作有効化要求
#define OTE_CODE_REQ_ESTP			0x00000100	//緊急停止要求
#define OTE_CODE_REQ_STOP			0x00000200	//減速停止要求
#define OTE_MASK_REQ_FAULT_RESET	0x00000400	//停止要求リセット要求
#define OTE_MASK_REQ_STOP			0x00000300	//停止要求MASK

#define OTE_STOP_REQ_MODE_OFF		L_OFF
#define OTE_STOP_REQ_MODE_ESTOP		0x00000001
#define OTE_STOP_REQ_MODE_CTRL_OFF	0x00000002

/******* 共通メッセージヘッダ部 ***********/

/// <summary>
/// 
/// </summary>
typedef struct OteComHead {
	ST_DEVICE_CODE	myid;		//装置ID
	SOCKADDR_IN		addr;		//送信元受信アドレス
	INT32			code;		//要求内容コード OTE:操作有効化,停止要求　CC：NA
	INT32			status;		//状態　OTE:操作/モニタモード　CC：操作/モニタモード
	INT32			tgid;		//接続中機器ID	OTE:操作端末タイプ　CC：接続中端末ID
}ST_OTE_HEAD, * LPST_OTE_HEAD;

/******* 共通メッセージ定義 **************/
#pragma region  構造体用define
#define N_CRANE_PC_MAX					32

#pragma endregion

//ランプ表示コマンド構造体（PC→OTE）
#define OTE_LAMP_FLICK_COUNT			0x0008
#define OTE_STATIC_UPDATE_COUNT			0x0004
/******* PCマルチキャストBody構造体 ***********/
typedef struct PcMBody {
	INT32 pos[ID_AXIS::Max];
}ST_PC_M_BODY, * LPST_PC_M_BODY;

/******* OTEマルチキャストBody構造体 ***********/
#define N_CRANE_PC_MAX      32
typedef struct OteMBody {
	UCHAR       pc_enable[N_CRANE_PC_MAX];		//接続可能端末フラグ
	INT32	    n_remote_wait;					//接続待ち遠隔操作卓台数
	INT32	    onbord_seqno;					//機側接続シーケンス番号
	INT32	    remote_seqno;					//遠隔卓接続シーケンス番号
	INT32	    my_seqno;						//自身の接続シーケンス番号
}ST_OTE_M_BODY, * LPST_OTE_M_BODY;

/******* PCユニキャストBody構造体 ***********/
#define N_OTE_LOAD_TYPE		2
#define ID_OTE_LOAD_MHOIST	0
#define ID_OTE_LOAD_AHOIST	1
typedef struct _ST_OTE_LOAD_STAT {
	ST_XYZ pos;		//吊点からの相対位置
	double l;		//ロープ長
	double m;		//質量
}ST_OTE_LOAD_STAT, * LPST_OTE_LOAD_STAT;
typedef struct _ST_OTE_PC_MSG {
	INT16 counter;						//メッセージを更新する毎にカウントアップ
	INT16 id;							//固定メッセージのID
}ST_OTE_PC_MSG, * LPST_OTE_PC_MSG;
typedef struct _ST_OTE_PC_FLTS_SET {
	INT16 set_type;							//セットされている故障の内容
	INT16 set_plc_count;					//セットされている故障コード数
	INT16 codes_plc[N_OTE_PC_SET_PLC_FLT];		//発生中故障コード
	INT16 set_pc_count;						//セットされている故障コード数
	INT16 codes_pc[N_OTE_PC_SET_PC_FLT];	//発生中故障コード
}ST_OTE_PC_FLTS_SET, * LPST_OTE_PC_FLTS_SET;


//デフォルト
/******* PCユニキャスト通信BODY構造体 ***********/
typedef struct PcUBody {//デフォルト
	UN_LAMP_COM			lamp[N_OTE_PNL_CTRL];									//OTEパネルコントローラ表示指示データ
	UN_LAMP_COM			notch_lamp[N_OTE_PNL_NOTCH_DEVICE][N_OTE_PNL_NOTCH_PBL];//ノッチランプ
	INT16				buf_io_read[CC_MC_SIZE_W_READ];							//PLC READデータ
	ST_PLC_AXIS_SET		st_axis_set[ID_AXIS::Max];								//動作状態モニタ値
	ST_OTE_LOAD_STAT	st_load_stat[N_OTE_LOAD_TYPE];							//吊荷状態
	ST_OTE_PC_MSG		pc_msg;													//メッセージを更新する毎にカウントアップ
	ST_OTE_PC_FLTS_SET  faults_set;												//発生中故障コード
	float 				bh_angle;												//起伏角度
	float				wind_spd;
	INT16				sl_brk_fb[8];											//遠隔旋回ブレーキFB
}ST_PC_U_BODY, * LPST_PC_U_BODY;

/******* OTEユニキャスト通信BODY構造体 ***********/
typedef struct OteBodyAxis {
	INT16 pb_notch[N_NOTCH_TBL];
	INT16 notch_ref;
	INT32 auto_sel;
	double auto_tg_pos;
}ST_OTE_BODY_AXIS, * LPST_OTE_BODY_AXIS;
typedef struct _ST_GPAD_IN {
	INT16 syukan_on;
	INT16 syukan_off;
	INT16 estop;
	INT16 remote;
	INT16 f_reset;
	INT16 bypass;
	INT16 kidou_r;
	INT16 kidou_l;
	INT16 pan_l;
	INT16 pan_r;
	INT16 tilt_u;
	INT16 tilt_d;
	INT16 zoom_f;
	INT16 zoom_n;

	INT16 trig_l;
	INT16 trig_r;

	INT16 pad_mh;
	INT16 pad_bh;
	INT16 pad_sl;
	INT16 pad_gt;
	INT16 pad_ah;
	INT16 spare[3];

}ST_GPAD_IN, * LPST_GPAD_IN;

#define CODE_OTE_FLT_SEL_HEAVY1		BIT1	//重故障1	
#define CODE_OTE_FLT_SEL_HEAVY2		BIT2	//重故障2
#define CODE_OTE_FLT_SEL_HEAVY3		BIT3	//重故障3
#define CODE_OTE_FLT_SEL_LIGHT		BIT4	//軽故障
#define CODE_OTE_FLT_SEL_IL			BIT5	//INTER_LOCK
#define CODE_OTE_FLT_INF_TRIG		BIT8	//PCからトリガ通知
#define CODE_OTE_FLT_REQ_REFRESH	BIT8	//OTEからリスト更新要求（トリガクリア,表示選択変更時）

#define N_OTE_OPEPNL_XIN			10		//重故障1	

typedef struct OteUBody {
	INT16		ope_mode;						//操作モード（自動/手動/ビギナー）
	INT16		remote;							//
	INT16		auto_mode;						//
	INT16		ctrl_device_mode;				//操作デバイスモード（操作卓/JOYPAD/パネル）
	INT16		opt_device_status;				//オプションデバイスモード
	INT16		game_pad_mode;					//ゲームパッド操作モード
	INT16		faults_disp_req;				//故障表示要求
	INT16		notch_mode;						//ノッチモード
	INT16		pnl_ctrl[N_OTE_PNL_CTRL];		//操作卓PB入力
	INT16		ote_type;						//操作卓タイプ
	INT16       spar;							//バウンダリ合わせスペア
	UINT32		target_seq_no;					//自動目標位置のシーケンス番号（トリガ検出用）
	INT16		ote_err[4];						//OTE検出異常
	ST_OTE_BODY_AXIS axis[ID_AXIS::Max];
}ST_OTE_U_BODY, * LPST_OTE_U_BODY;

//########################################################
//			ボディ部UNION定義　
// BODY部の構造体はクレーンタイプに応じて定義して追加　
//########################################################
#define OTE_PRM_SIZE_OF_BODY_I16			600//1200 byte
union UN_PC_M_BODY {
	INT16			val[OTE_PRM_SIZE_OF_BODY_I16];
	ST_PC_M_BODY	st;
};
union UN_OTE_M_BODY {
	INT16			val[OTE_PRM_SIZE_OF_BODY_I16];
	ST_OTE_M_BODY	st;
};
union UN_PC_U_BODY {
	INT16			val[OTE_PRM_SIZE_OF_BODY_I16];
	ST_PC_U_BODY	st;
};
union UN_OTE_U_BODY {
	INT16			val[OTE_PRM_SIZE_OF_BODY_I16];
	ST_OTE_U_BODY	st;
};

//#################################################
//			メッセージ構造体定義　
//#################################################
/******* PCマルチキャストメッセージ ***********/
typedef struct PcMMsg {
	ST_OTE_HEAD		head;
	UN_PC_M_BODY	body;
}ST_PC_M_MSG, * LPST_PC_M_MSG;
/******* OTEマルチキャストメッセージ構造体 ***********/
typedef struct OteMMsg {
	ST_OTE_HEAD     head;
	UN_OTE_M_BODY   body;
}ST_OTE_M_MSG, * LPST_OTE_M_MSG;
/******* PCユニキャストメッセージ構造体 ***********/
typedef struct PcUMsg {
	ST_OTE_HEAD     head;
	UN_PC_U_BODY    body;
}ST_PC_U_MSG, * LPST_PC_U_MSG;
/******* OTEユニキャスト通信メッセージ構造体 ***********/
typedef struct OteUMsg {
	ST_OTE_HEAD     head;
	UN_OTE_U_BODY   body;
}ST_OTE_U_MSG, * LPST_OTE_U_MSG;




