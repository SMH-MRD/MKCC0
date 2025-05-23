#pragma once

#include <winsock.h>
#include "PLC_DEF.h"
#include "COMMON_DEF.h"


//操作端末のテキスト,ボタン、ランプ数
#define N_OTE_PNL_CTRL				64
#define N_OTE_PC_MSG_WCHAR			64
#define N_OTE_PNL_NOTCH_DEVICE		5
#define N_OTE_PNL_NOTCH_PBL			N_NOTCH_TBL

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

#if 0
#define OTE_N_LOAD						2	//吊荷数
#define OTE_N_PC_MSG_WCHAR				128	//PCからのメッセージテキスト数
#define OTE_ID_LOAD_MH					0
#define OTE_ID_LOAD_AH					1

//吊荷状態　INDEX（OTEアニメーション表示用）
#define ID_PUC_BODY_LD_MHX				0
#define ID_PUC_BODY_LD_MHY				1
#define ID_PUC_BODY_LD_MHZ				2
#define ID_PUC_BODY_LD_MHTH				3
#define ID_PUC_BODY_LD_AHX				0
#define ID_PUC_BODY_LD_AHY				1
#define ID_PUC_BODY_LD_AHZ				2
#define ID_PUC_BODY_LD_AHTH				3

//半自動目標設定INDEX
#define ID_PUC_BODY_TG_MAIN				0
#define ID_PUC_BODY_TG_S1				1
#define ID_PUC_BODY_TG_S2				2
#define ID_PUC_BODY_TG_S3				3
#define ID_PUC_BODY_TG_L1				4
#define ID_PUC_BODY_TG_L2				5
#define ID_PUC_BODY_TG_L3				6

//ノッチ状態バッファ参照用インデックス
#define ID_OTE_NOTCH_POS_HOLD			0 //現在値保持
#define ID_OTE_NOTCH_POS_TRIG			1 //トリガ状態
#define ID_OTE_NOTCH_POS_CNT			2 //状態保持カウント値

//グリップスイッチ
#define OTE_GRIP_ESTP					0x03
#define OTE_GRIP_ENABLE					0x08
#define OTE_GRIP_ACTIVE					0X14
#define OTE_GRIP_DBG_ENABLE				0x80

#endif

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
typedef struct _ST_OTE_MOTION_STAT {
	double pos_fb;
	double pos_tg;
	double v_fb;
	double nv_ref;
	INT32 brake;
	INT32 auto_sel;
}ST_OTE_MOTION_STAT,*LPST_OTE_MOTION_STAT;
#define N_OTE_LOAD_TYPE		2
#define ID_OTE_LOAD_MHOIST	0
#define ID_OTE_LOAD_AHOIST	1
typedef struct _ST_OTE_LOAD_STAT {
	ST_XYZ pos;		//吊点からの相対位置
	double l;		//ロープ長
}ST_OTE_LOAD_STAT, * LPST_OTE_LOAD_STAT;
typedef struct _ST_OTE_PC_MSG {
	INT16 counter;						//メッセージを更新する毎にカウントアップ
	INT16 id;							//固定メッセージのID
	WCHAR wch[N_OTE_PC_MSG_WCHAR];
}ST_OTE_PC_MSG, * LPST_OTE_PC_MSG;

enum OTE_PNL_CTRLS{
	estop=0,
	syukan_on,
	syukan_off,
	remote,
	auto_mode,
	fault_reset,
	bypass,
	game_pad,
	asel_mh,
	asel_bh,
	asel_sl,
	asel_gt,
	asel_ah,
	ote_type,
	notch_mh,
	notch_bh,
	notch_sl,
	notch_gt
};

enum OTE_PNL_NOTCH {
	R5 = 0,
	R4, R3, R2, R1, N0, F1, F2, F3, F4, F5,
	NOTCH_MAX
};

//デフォルト
/******* PCユニキャスト通信BODY構造体 ***********/
typedef struct PcUBody {//デフォルト
	UN_LAMP_COM			lamp[N_OTE_PNL_CTRL];									//OTEパネルコントローラ表示指示データ
	UN_LAMP_COM			notch_lamp[N_OTE_PNL_NOTCH_DEVICE][N_OTE_PNL_NOTCH_PBL];//ノッチランプ
	INT16				buf_io_read[CC_MC_SIZE_W_READ];							//PLC READデータ
	ST_OTE_MOTION_STAT	st_motion_stat[ID_AXIS::Max];							//動作状態モニタ値
	ST_OTE_LOAD_STAT	st_load_stat[N_OTE_LOAD_TYPE];							//吊荷状態
	ST_OTE_PC_MSG		pc_msg;													//メッセージを更新する毎にカウントアップ
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
}ST_GPAD_IN, * LPST_GPAD_IN;

typedef struct OteUBody {
	INT16		ope_mode;														//操作モード（自動/手動/ビギナー）
	INT16		remote;															//
	INT16		auto_mode;														//
	INT16		ctrl_device_mode;												//操作デバイスモード（操作卓/JOYPAD/パネル）
	INT16		opt_device_status;												//オプションデバイスモード
	INT16		game_pad_mode;													//ゲームパッド操作モード
	INT16		ote_type;														//操作卓種別
	INT16		ctrl_ope[N_OTE_PNL_CTRL];										//操作卓PB入力
	ST_OTE_BODY_AXIS axis[ID_AXIS::Max];
	UINT32		target_seq_no;													//自動目標位置のシーケンス番号（トリガ検出用）
	ST_GPAD_IN	gpad_in;														//ゲームパッド入力
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




