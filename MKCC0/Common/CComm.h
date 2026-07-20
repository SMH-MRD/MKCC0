#pragma once
#include "COMMON_DEF.h"


//@ DEFAULT-----------------------------------------------------------------------------------
#define DEFAULT_MY_IP1					"127.0.0.1"
#define DEFAULT_MY_IP2					"127.0.0.2"
#define DEFAULT_MY_IP3					"127.0.0.3"
#define DEFAULT_MY_PORT					10050


#define N_COMM_SET_CRANE			200
#define N_COMM_SET_OTE				128
#define N_COMM_SET_IP_WCHAR			16
#define N_COMM_SET_PC_LAN_PORT		8
#define N_COMM_SET_WAN_PORT			2
#define N_COMM_SET_PLC_LAN_PORT		2
#define N_COMM_SET_AUX_LAN_PORT		8

#define CODE_COMM_LOCATION_CRANE	0	//クレーン
#define CODE_COMM_LOCATION_OTE		1	//操作端末

#define ID_COMM_MAIN_PC_PLC			0	//メインのPCとPLC間通信
#define ID_COMM_CRANE_OTE_IF		1	//クレーンPCと遠隔操作PC（OTE）間通信
#define ID_COMM_PC_SLBRK			2	//メインのPCと旋回ブレーキPLC間通信
#define ID_COMM_PC_WIFI				2	//OTE WIFIポート
#define ID_COMM_SWAY_SENSOR			3	//振れセンサ
#define ID_COMM_LANIO				4	//振れセンサ
#define ID_COMM_PC_OPTION			7	//デバッグ用

#define ID_COMM_WAN0				0	//デフォルトのWAN
#define ID_COMM_WAN1				1	//保守用WAN

#define ID_COMM_LIST_SLBRK			0	//旋回ブレーキ遠隔操作装置

//ポートタイプ
#define CODE_COMM_TYPE_LAN			0	//
#define CODE_COMM_TYPE_WIFI			1
#define CODE_COMM_TYPE_WAN			2

//プロトコル
#define CODE_COMM_PROTOCOL_UDP		0	//UDP
#define CODE_COMM_PROTOCOL_TCP		1	//TCP
#define CODE_COMM_PROTOCOL_MC		2	//MCプロトコル
#define CODE_COMM_PROTOCOL_MCAST	3	//マルチキャスト

//@ PLC IF-----------------------------------------------------------------------------------
#define PLC_IF_TYPE_CC				0
#define PLC_IF_TYPE_OTE				1
#define PLC_IF_TYPE_SLBRK			2
#define PLC_IF_TYPE_OTE_DEBUG		3

#define ID_SOCK_EVENT_OTE_UNI			10603				//PC受信ソケットイベント　OTE UNICASTメッセージ
#define ID_SOCK_EVENT_PC_MUL			10604				//PC受信ソケットイベント　PC MULTICASTメッセージ
#define ID_SOCK_EVENT_OTE_MUL			10605				//PC受信ソケットイベント　OTE MULTICASTメッセージ


//通信定義構造体定義
typedef struct _ST_ADDR_ITEM
{
	char ip[N_COMM_SET_IP_WCHAR];	//受信IP
	USHORT type;					//タイプ（LAN,WiFi,WAN）
	USHORT protocol;				//プロトコル
	USHORT port;					//受信ポート番号,局番，Node，ch等
	USHORT option;					//
}ST_ADDR_ITEM, * LPST_ADDR_ITEM;

typedef struct _ST_ADDR_SET
{
	ST_ADDR_ITEM pc[N_COMM_SET_PC_LAN_PORT];
	ST_ADDR_ITEM wan[N_COMM_SET_WAN_PORT];
	ST_ADDR_ITEM plc[N_COMM_SET_PLC_LAN_PORT];
	ST_ADDR_ITEM aux[N_COMM_SET_AUX_LAN_PORT];
}ST_ADDR_SET, * LPST_ADDR_SET;

typedef struct _ST_COMM_SET
{
	ST_ADDR_SET crn[N_COMM_SET_CRANE];	//クレーンIPアドレスリスト
	ST_ADDR_SET ote[N_COMM_SET_OTE];	//OTE側IPアドレスリスト

	ST_ADDR_ITEM mcast_crn_crn;		//クレーン間マルチキャストアドレス
	ST_ADDR_ITEM mcast_crn_ote;		//クレーンからOTEへのマルチキャストアドレス
	ST_ADDR_ITEM mcast_ote_crn;		//OTEからクレーンへのマルチキャストアドレス
	ST_ADDR_ITEM mcast_ote_ote;		//OTE間マルチキャストアドレス

}ST_COMM_SET, * LPST_COMM_SET;


class CComm {
private:
	static HRESULT set_addr_set(int loc, int machine_id, LPST_ADDR_SET psrc) {
		if((loc == CODE_COMM_LOCATION_CRANE)&&(machine_id>= 0)&& (machine_id < N_COMM_SET_CRANE)) {
			addr_list.crn[machine_id] = *psrc;
			return S_OK;
		}
		if ((loc == CODE_COMM_LOCATION_OTE) && (machine_id >= 0) && (machine_id < N_COMM_SET_OTE)) {
			addr_list.ote[machine_id] = *psrc;
		}
		return S_FALSE;
	}

public:
	CComm();
	virtual ~CComm();

	static ST_COMM_SET	addr_list;
	
	static int setup();

};