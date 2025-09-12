#pragma once

#include <string>
#include "COMMON_DEF.h"

//#include "SPEC_DEF.h"

#include "OTE_DEF.h"
#include "PLC_DEF.h"
//#include "CVector3.h"
//#include <dinput.h>
#include "CCrane.h"
#include <dinput.h>

using namespace std;

/****************************************************************************/
/*   操作端末側信号定義構造体                                    　			*/
/* 　OTEプロセス用共有メモリ上の情報　　　　　　　          　				*/
/****************************************************************************/
#pragma region SMEM NAME
#define SMEM_OTE_ENV_NAME			L"OTE_ENV_IF"
#define SMEM_OTE_CS_INF_NAME		L"OTE_CS_IF"
#define SMEM_OTE_UI_NAME			L"OTE_UI"
#define SMEM_OTE_CC_IF_NAME			L"OTE_CC_IF"
#define SMEM_OTE_PANEL_NAME			L"OTE_PANEL"

#define MUTEX_OTE_ENV_NAME			L"MU_OTE_ENV_IF"
#define MUTEX_OTE_CS_INF_NAME		L"MU_OTE_CS_IF"
#define MUTEX_OTE_UI_NAME			L"MU_OTE_UI"
#define MUTEX_OTE_CC_IF_NAME		L"MU_OTE_CC_IF"
#pragma endregion
#pragma region GRAPHIC
#define N_PANEL_COLOR_PEN		16
#define N_PANEL_COLOR_BRUSH		16
#define N_PANEL_FONT			16
#define N_STRING_FORMAT			8

#define ID_PANEL_COLOR_BLACK	0
#define ID_PANEL_COLOR_DGRAY	1
#define ID_PANEL_COLOR_WHITE	2
#define ID_PANEL_COLOR_RED		3
#define ID_PANEL_COLOR_GREEN	4
#define ID_PANEL_COLOR_BLUE		5
#define ID_PANEL_COLOR_YELLOW	6
#define ID_PANEL_COLOR_ORANGE	7
#define ID_PANEL_COLOR_MAZENDA	8
#define ID_PANEL_COLOR_BROWN	9
#define ID_PANEL_COLOR_LGRAY	10

#define ID_PANEL_FONT_6			0
#define ID_PANEL_FONT_8			1
#define ID_PANEL_FONT_10		2
#define ID_PANEL_FONT_12		3
#define ID_PANEL_FONT_14		4
#define ID_PANEL_FONT_20		5
#define ID_PANEL_FONT_24		6
#define ID_PANEL_FONT_32		7
#define ID_PANEL_FONT_48		8

#define ID_STR_FORMAT_CENTER			0
#define ID_STR_FORMAT_LEFT_CENTER		1
#define ID_STR_FORMAT_LEFT_TOP			2
#define ID_STR_FORMAT_LEFT_BOTTOM		3
#define ID_STR_FORMAT_RIGHT_CENTER		4
#define ID_STR_FORMAT_RIGHT_TOP			5
#define ID_STR_FORMAT_RIGHT_BOTTOM		6

typedef struct _ST_DRAWING_BASE {
	Pen* ppen[N_PANEL_COLOR_PEN];
	SolidBrush* pbrush[N_PANEL_COLOR_BRUSH];
	FontFamily* pfamily[N_PANEL_FONT];
	Font*		pfont[N_PANEL_FONT];
	StringFormat* pstrformat[N_STRING_FORMAT];
}ST_DRAWING_BASE, *LPST_DRAWING_BASE;
#pragma endregion

#pragma region OTE_INF ENV
typedef struct _ST_OTE_ENV_INF {
	ST_DEVICE_CODE device_code;

	UINT32			target_crane;			//接続対象クレーン
	UINT32			ote_mode;				//操作端末モード
	UINT32			ote_id;					//端末ID
	UINT32			grip_mode;				//グリップスイッチモード
	UINT32			gpad_mode = L_OFF;		//ゲームパッドモード
	DIJOYSTATE		pad_data;

	INT32			selected_crane;

}ST_OTE_ENV_INF, * LPST_OTE_ENV_INF;
#pragma endregion 

//## OTE_CS_INF @CS
#pragma region OTE_CS_INF

#define OTE_OPE_SOURCE_CODE_OPEPNL	0x00000001	//操作卓有効
#define OTE_OPE_SOURCE_CODE_GPAD	0x00000002	//ゲームパッド有効
#define OTE_OPE_SOURCE_CODE_PCPNL	0x00000004	//PCパネル有効
#define OTE_OPE_SOURCE_CODE_HANDY	0x00000008	//携帯パネル有効

typedef struct _ST_OTE_CS_INF {
	INT32 ope_plc_stat = L_OFF;						//PLCとの通信状態
	INT32 cc_ope_active = L_OFF;
	INT32 ope_source_mode=0;						//操作器具モード
	ST_OTE_U_BODY st_body;							//OTE⇒PC送信メッセージボディ部
	ST_GPAD_IN gpad_in;								//ゲームパッド入力
	INT16 buf_opepnl_read[OTE_MC_SIZE_W_READ];		//PLCからの読み取り信号生値
	INT16 buf_opepnl_write[OTE_MC_SIZE_W_WRITE];	//PLCへの書き込み信号生値
	INT16 pnl_ctrl[N_OTE_PNL_CTRL];					//操作卓入力仕分け
}ST_OTE_CS_INF, * LPST_OTE_CS_INF;
#pragma endregion 操作卓PLCとの通信他

//## OTE_CC_IF @AGENT
#pragma region OTE_CC_INF

#define CODE_OTE_PC_COM_STAT_SOCK_NG	-1	//ソケット初期化エラー
#define CODE_OTE_PC_COM_STAT_SOCK_ERR	0	//ソケットエラー応答
#define CODE_OTE_PC_COM_STAT_SOCK_OK	1	//ソケット初期化OK　通信無し
#define CODE_OTE_PC_COM_STAT_LINK_OK	2	//通信中

typedef struct _ST_OTE_CC_IF {
	INT32			cc_com_stat_r;		//制御PCとの通信状態（受信）
	INT32			cc_com_stat_s;		//制御PCとの通信状態（送信）
	INT32			cc_active_ote_id;	//クレーン側操作有効端末id

	ST_PC_U_MSG     st_msg_pc_u_rcv;
	ST_PC_M_MSG     st_msg_pc_m_rcv;
	ST_OTE_M_MSG    st_msg_ote_m_rcv;

	ST_OTE_U_MSG    st_msg_ote_u_snd;
	ST_OTE_M_MSG    st_msg_ote_m_snd;

	INT32	id_ote_ope_active = OTE_NON_OPEMODE_ACTIVE;			//ノッチ操作が有効な接続端末ID
	INT32	id_conected_crane = CRANE_ID_NULL;					//接続中クレーンID
	INT32	com_status_cc_rcv = CODE_OTE_PC_COM_STAT_SOCK_NG;	//受信ソケット状態
	INT32	com_status_cc_snd = CODE_OTE_PC_COM_STAT_SOCK_NG;	//受信ソケット状態

	UINT32	ope_ote_silent_cnt;	//操作有効端末からの受信無しでカウントアップ
	UINT32	mon_silent_cnt;  	//隣接PCからのマルチキャスト受信カウント無しでカウントアップ	
	UINT32	stop_req_mode;		//停止OTE要求モード	非常停止,主幹切

}ST_OTE_CC_IF, * LPST_OTE_CC_IF;
#pragma endregion OTEのCCとの通信構造体

//## OTE_UI @SCADA or VIEW Proc
#pragma region OTE_UI


typedef struct _ST_OTE_UI {
	INT16 pc_pnl_active;					//描画モード
	INT16 pnl_ctrl[N_OTE_PNL_CTRL];			
	INT16 notch_pb_mh;						//選択中のノッチ
	INT16 notch_pb_bh;
	INT16 notch_pb_sl;
	INT16 notch_pb_gt;
	INT16 notch_pb_ah;
	INT16 flt_req_code;		//フォルト要求コード
}ST_OTE_UI, * LPST_OTE_UI;
#pragma endregion 操作端末UI構造体









