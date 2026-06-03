#pragma once
#include "COMMON_DEF.h"

#define N_COMM_SET_CRANE			200
#define N_COMM_SET_OTE				64
#define N_COMM_SET_IP_WCHAR			16
#define N_COMM_SET_PC_LAN_PORT		4
#define N_COMM_SET_WAN_PORT			2
#define N_COMM_SET_PLC_LAN_PORT		2
#define N_COMM_SET_AUX_LAN_PORT		4

#define N_COMM_SET_OTE_ID_OFFSET	100	//OTEのIDは100から始まる

#define CODE_COMM_LOCATION_CRANE	0	//クレーン
#define CODE_COMM_LOCATION_OTE		1	//操作端末

#define ID_COMM_MAIN_PC_PLC			0	//メインのPCとPLC間通信
#define ID_COMM_CRANE_OTE_IF		1	//クレーンPCと遠隔操作PC（OTE）間通信
#define ID_COMM_PC_SLBRK			2	//メインのPCと旋回ブレーキPLC間通信

#define ID_COMM_WAN0				0	//デフォルトのWAN
#define ID_COMM_WAN1				1	//WAN1

#define ID_COMM_LIST_SLBRK			0	//旋回ブレーキ遠隔操作装置

//ポートタイプ
#define CODE_COMM_TYPE_LAN			0	//
#define CODE_COMM_TYPE_WIFI			1
#define CODE_COMM_TYPE_WAN			2

//プロトコル
#define CODE_COMM_PROTOCOL_UDP		0	//UDP
#define CODE_COMM_PROTOCOL_TCP		1	//TCP
#define CODE_COMM_PROTOCOL_MC		2	//MCプロトコル

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
}ST_COMM_SET, * LPST_COMM_SET;


class CComm {
private:
	HRESULT set_addr_set(int loc, int machine_id, LPST_ADDR_SET psrc) {
		if((loc == CODE_COMM_LOCATION_CRANE)&&(machine_id>= 0)&& (machine_id < N_COMM_SET_CRANE)) {
			addr_list.crn[machine_id] = *psrc;
			return S_OK;
		}
		if ((loc == CODE_COMM_LOCATION_OTE) && (machine_id >= N_COMM_SET_OTE_ID_OFFSET) && (machine_id < (N_COMM_SET_OTE_ID_OFFSET + N_COMM_SET_OTE))) {
			addr_list.ote[machine_id - N_COMM_SET_OTE_ID_OFFSET] = *psrc;
		}
		return S_FALSE;
	}

public:
	CComm();
	virtual ~CComm();

	static ST_COMM_SET	addr_list;
	
	ST_ADDR_ITEM default_addr = {
		// ip[N_COMM_SET_IP_WCHAR] の初期化
		{ "192.168.100.100" },
		// port[N_COMM_SET_PORT] の初期化（例: 0で初期化、必要に応じてポート番号を指定）
		CODE_COMM_TYPE_LAN, CODE_COMM_PROTOCOL_UDP, 8001,0
	};

	int setup();

};