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
#define OTE_ENV_APP_PRODUCT				0	//実機モード
#define OTE_ENV_APP_DEBUG_TYPE1			3	//PC1台デバッグモード
#define OTE_ENV_APP_DEBUG_TYPE2			4	//PC1台+操作卓PLCデバッグモード

#define OTE_ENV_MODE_OTE_PORT_LOCAL		0	//端末種別：機上有線端末(PORT0）
#define OTE_ENV_MODE_OTE_PORT_WIFI		1	//端末種別：機側無線（Wifi）
#define OTE_ENV_MODE_OTE_PORT_WAN		2	//端末種別：遠隔操作室端末（WAN回線）
#define OTE_ENV_MODE_OTE_PORT_ERR		-1	//ソケットアドレス未定

typedef struct _ST_OTE_ENV_INF {
	ST_DEVICE_CODE		device_code;
	ST_APP_COMMON_PARAM app_common_param;

	UINT32			target_crane;			//接続対象クレーン
	UINT32			ote_type;				//操作端末タイプ
	UINT32			ote_id;					//端末ID
	UINT32			ote_com_mode;			//接続対象クレーン
	UINT32			grip_mode;				//グリップスイッチモード
	UINT32			gpad_mode = L_OFF;		//ゲームパッドモード
	DIJOYSTATE		pad_data;

	INT32			selected_crane;


}ST_OTE_ENV_INF, * LPST_OTE_ENV_INF;
#pragma endregion 

//## OTE_CS_INF @CS
#pragma region OTE_CS_INF

#define OTE_OPE_SOURCE_CODE_OPEPNL		0x00000001	//操作卓有効
#define OTE_OPE_SOURCE_CODE_GPAD		0x00000002	//ゲームパッド有効
#define OTE_OPE_SOURCE_CODE_PCPNL		0x00000004	//PCパネル有効
#define OTE_OPE_SOURCE_CODE_HANDY		0x00000008	//携帯パネル有効

#define OTE_OPE_GOT_COM_CONNECT_CRANE	0x00000001	//GOTクレーン接続コマンド
#define OTE_OPE_GOT_COM_RELEASE_CRANE	0x00000002	//GOTクレーン解放コマンド

#define OTE_PC_FLT_DETECT_MAX			15

#define OTE_CS_CODE_OPEPNL_PC			0	//制御PC端末
#define OTE_CS_CODE_OPEPNL_LOCAL		1	//機側端末
#define OTE_CS_CODE_OPEPNL_ROOM			2	//操作室端末

typedef struct _ST_OTE_CS_INF {
	INT32 ote_type	 = OTE_CS_CODE_OPEPNL_PC;		//PLCとの通信状態
	INT32 ote_option = 0;							//予備

	INT32 ope_plc_stat = L_OFF;						//PLCとの通信状態
	INT32 cc_ope_active = L_OFF;
	INT32 ope_source_mode=0;						//操作器具モード
	INT32 pc_pnl_active;
	INT32 plc_com_stat_r = L_OFF;						//PLCとの通信状態
	INT32 plc_com_stat_s = L_OFF;						//PLCとの通信状態

	ST_OTE_U_BODY st_body;							//OTE⇒PC送信メッセージボディ部
	ST_GPAD_IN gpad_in;	
	INT16 GOT_command;								//GOTのコマンド
	INT16 ote_error;								//OTEのエラーコード
	INT32 GOT_crane_select;							//GOTのクレーン選択状態
	INT16 buf_opepnl_read[OTE_MC_SIZE_W_READ+2];		//PLCからの読み取り信号生値
	INT16 buf_opepnl_write[OTE_MC_SIZE_W_WRITE];	//PLCへの書き込み信号生値
	INT16 pnl_ctrl[N_OTE_PNL_CTRL];					//操作卓入力仕分け
	INT16 rpc_flt_codes[OTE_PC_FLT_DETECT_MAX];		//操作卓故障検出バッファ
	INT16 rpc_flt_count;							//操作卓故障検出バッファカウント

}ST_OTE_CS_INF, * LPST_OTE_CS_INF;
#pragma endregion 操作卓PLCとの通信他

//## OTE_CC_IF @AGENT
#pragma region OTE_CC_INF

#define CODE_OTE_PC_COM_STAT_SOCK_NG	-1	//ソケット初期化エラー
#define CODE_OTE_PC_COM_STAT_SOCK_ERR	0	//ソケットエラー応答
#define CODE_OTE_PC_COM_STAT_SOCK_OK	1	//ソケット初期化OK　通信無し
#define CODE_OTE_PC_COM_STAT_LINK_OK	2	//通信中

#define PRM_OTE_PC_COM_TMOV_CNT			20	//通信チェックカウント

#define OTE_AGENT_MODE_OTE_PORT_LOCAL		0	//有線LAN接続モード(PORT0）
#define OTE_AGENT_MODE_OTE_PORT_WIFI		1	//端末種別：遠隔操作室端末（有線LAN）
#define OTE_AGENT_MODE_OTE_PORT_WAN			2	//有線LAN接続モード(PORT1）
#define OTE_AGENT_MODE_OTE_PORT_ERR			-1	//ソケットアドレス未定


typedef struct _ST_OTE_CC_IF {
	INT32			cc_com_stat_r;		//制御PCとの通信状態（受信）
	INT32			cc_com_stat_s;		//制御PCとの通信状態（送信）
	INT32			cc_active_ote_id;	//クレーン側操作有効端末id
	INT32			cc_comm_chk_cnt;	//通信状態チェックカウンタ

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
	UINT32	ote_command;		//停止OTE要求モード	非常停止,主幹切
	UINT32	ote_mode;				//操作端末接続モード

}ST_OTE_CC_IF, * LPST_OTE_CC_IF;
#pragma endregion OTEのCCとの通信構造体

//## OTE_UI @SCADA or VIEW Proc
#pragma region OTE_UI


typedef struct _ST_OTE_UI {
	INT16 pc_pnl_active;					
	INT16 pnl_ctrl[N_OTE_PNL_CTRL];			
	INT16 notch_pb_mh;						//選択中のノッチ
	INT16 notch_pb_bh;
	INT16 notch_pb_sl;
	INT16 notch_pb_gt;
	INT16 notch_pb_ah;
	INT16 flt_req_code;						//フォルト要求コード
	HWND  hWnd_crane_ope_panel = NULL;		//操作パネルウィンドウハンドル  
}ST_OTE_UI, * LPST_OTE_UI;
#pragma endregion 操作端末UI構造体









