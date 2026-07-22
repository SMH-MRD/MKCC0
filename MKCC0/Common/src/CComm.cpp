#include "CComm.h"

ST_COMM_SET	CComm::addr_list;

CComm::CComm() {
	setup();
};
CComm:: ~CComm() {};

int CComm::setup(){
	ST_ADDR_ITEM default_addr = {
		{ "192.168.100.100" },
		CODE_COMM_TYPE_LAN, CODE_COMM_PROTOCOL_UDP, 8001,0
	};


	// リスト初期化
	for (int i = 0; i < N_COMM_SET_CRANE; i++) {
		for (int j = 0; j < N_COMM_SET_PC_LAN_PORT; j++)	addr_list.crn[i].pc[j] = default_addr;
		for (int j = 0; j < N_COMM_SET_WAN_PORT; j++)		addr_list.crn[i].wan[j] = default_addr;
		for (int j = 0; j < N_COMM_SET_PLC_LAN_PORT; j++)	addr_list.crn[i].plc[j] = default_addr;
		for (int j = 0; j < N_COMM_SET_AUX_LAN_PORT; j++)	addr_list.crn[i].aux[j] = default_addr;
	}

//#############################################################
//################ クレーンアドレスセット　####################
//#############################################################
	
	//CARNE_ID_HHGH29
	{
		int machine_id = CRANE_ID_HHGH29;
		ST_ADDR_SET st_src =
		{
			{//PC
				{"192.168.101.100",CODE_COMM_TYPE_LAN,CODE_COMM_PROTOCOL_MC,3020,0},	//ID_COMM_MAIN_PC_PLC
				{"192.168.101.101",CODE_COMM_TYPE_LAN,CODE_COMM_PROTOCOL_UDP,8001,0},	//ID_COMM_CRANE_OTE_IF
				{"192.168.101.100",CODE_COMM_TYPE_LAN,CODE_COMM_PROTOCOL_MC,3021,0},	//ID_COMM_PC_SLBRK
				{"192.168.100.102",CODE_COMM_TYPE_LAN,CODE_COMM_PROTOCOL_UDP,10080,0},	//ID_COMM_SENSOR
				{"172.31.0.0",CODE_COMM_TYPE_LAN,CODE_COMM_PROTOCOL_UDP,10003,0},	//ID_COMM_LANIO
				default_addr,
				default_addr,
				default_addr
			},	
			{//WAN
				{"10.130.7.205",CODE_COMM_TYPE_WAN,CODE_COMM_PROTOCOL_UDP,8001,0},		//ID_COMM_WAN0
				{"10.130.5.74",CODE_COMM_TYPE_WAN,CODE_COMM_PROTOCOL_UDP,8001,0}		//ID_COMM_WAN1(保守用WAN)
			},							
			{//PLC
				{"192.168.101.110",CODE_COMM_TYPE_LAN,CODE_COMM_PROTOCOL_MC,3010,0},	//ID_COMM_MAIN_PC_PLC
				default_addr
			},							
			{//AUX
				default_addr,
				default_addr,
				{"192.168.101.120",CODE_COMM_TYPE_LAN,CODE_COMM_PROTOCOL_MC,3011,0},	//ID_COMM_PC_SLBRK
				{"192.168.0.121",CODE_COMM_TYPE_LAN,CODE_COMM_PROTOCOL_MC,10081,0},	//ID_COMM_SENSOR
				{"172.31.0.31",CODE_COMM_TYPE_LAN,CODE_COMM_PROTOCOL_UDP,10003,0},	//ID_COMM_LANIO
				default_addr,
				default_addr,
				default_addr,
			},	
		};
		set_addr_set(CODE_COMM_LOCATION_CRANE, machine_id, &st_src);
	}

	//CARNE_ID_HHGQ18
	{
		int machine_id = CRANE_ID_HHGQ18;
		ST_ADDR_SET st_src =
		{
			{//PC
				{"192.168.110.100",CODE_COMM_TYPE_LAN,CODE_COMM_PROTOCOL_MC,3020,0},	//ID_COMM_MAIN_PC_PLC
				{"192.168.110.101",CODE_COMM_TYPE_LAN,CODE_COMM_PROTOCOL_UDP,8001,0},	//ID_COMM_CRANE_OTE_IF
				{"192.168.110.100",CODE_COMM_TYPE_LAN,CODE_COMM_PROTOCOL_MC,3021,0},	//ID_COMM_PC_SLBRK
				{"192.168.100.102",CODE_COMM_TYPE_LAN,CODE_COMM_PROTOCOL_UDP,10080,0},	//ID_COMM_SENSOR
				{"172.31.0.0",CODE_COMM_TYPE_LAN,CODE_COMM_PROTOCOL_UDP,10003,0},	//ID_COMM_LANIO
				default_addr,
				default_addr,
				default_addr
			},
			{//WAN
				{"10.130.7.205",CODE_COMM_TYPE_WAN,CODE_COMM_PROTOCOL_UDP,8001,0},		//ID_COMM_WAN0
				default_addr
			},
			{//PLC
				{"192.168.110.110",CODE_COMM_TYPE_LAN,CODE_COMM_PROTOCOL_MC,3010,0},	//ID_COMM_MAIN_PC_PLC
				default_addr
			},
			{//AUX
				default_addr,
				default_addr,
				{"192.168.101.120",CODE_COMM_TYPE_LAN,CODE_COMM_PROTOCOL_MC,3011,0},	//ID_COMM_PC_SLBRK
				{"192.168.0.121",CODE_COMM_TYPE_LAN,CODE_COMM_PROTOCOL_MC,10081,0},	//ID_COMM_SENSOR
				{"172.31.0.31",CODE_COMM_TYPE_LAN,CODE_COMM_PROTOCOL_UDP,10003,0},	//ID_COMM_LANIO
				default_addr,
				default_addr,
				default_addr,
			},
		};
		set_addr_set(CODE_COMM_LOCATION_CRANE, machine_id, &st_src);
	}

	//CARNE_ID_H6R602
	{
		int machine_id = CRANE_ID_H6R602;
		ST_ADDR_SET st_src =
		{
			{//PC
				{"192.168.110.100",CODE_COMM_TYPE_LAN,CODE_COMM_PROTOCOL_MC,3020,0},	//ID_COMM_MAIN_PC_PLC
				{"192.168.110.101",CODE_COMM_TYPE_LAN,CODE_COMM_PROTOCOL_UDP,8001,0},	//ID_COMM_CRANE_OTE_IF
				{"192.168.110.100",CODE_COMM_TYPE_LAN,CODE_COMM_PROTOCOL_MC,3021,0},	//ID_COMM_PC_SLBRK
				{"192.168.100.102",CODE_COMM_TYPE_LAN,CODE_COMM_PROTOCOL_UDP,10080,0},	//ID_COMM_SENSOR
				{"172.31.0.0",CODE_COMM_TYPE_LAN,CODE_COMM_PROTOCOL_UDP,10003,0},	//ID_COMM_LANIO
				default_addr,
				default_addr,
				default_addr
			},
			{//WAN
				{"10.130.7.205",CODE_COMM_TYPE_WAN,CODE_COMM_PROTOCOL_UDP,8001,0},		//ID_COMM_WAN0
				default_addr
			},
			{//PLC
				{"192.168.100.110",CODE_COMM_TYPE_LAN,CODE_COMM_PROTOCOL_MC,3010,0},	//ID_COMM_MAIN_PC_PLC
				default_addr
			},
			{//AUX
				default_addr,
				default_addr,
				{"192.168.101.120",CODE_COMM_TYPE_LAN,CODE_COMM_PROTOCOL_MC,3011,0},	//ID_COMM_PC_SLBRK
				{"192.168.0.121",CODE_COMM_TYPE_LAN,CODE_COMM_PROTOCOL_MC,10081,0},	//ID_COMM_SENSOR
				{"172.31.0.31",CODE_COMM_TYPE_LAN,CODE_COMM_PROTOCOL_UDP,10003,0},	//ID_COMM_LANIO
				default_addr,
				default_addr,
				default_addr,
			},
		};
		set_addr_set(CODE_COMM_LOCATION_CRANE, machine_id, &st_src);
	}

	//#############################################################
	//################ OTEアドレスセット　     ####################
	//#############################################################
	
	//OTE_ID_HHGG3800
	{
		int machine_id = OTE_ID_HHGG3800;
		ST_ADDR_SET st_src =
		{
			{//PC
				{"192.168.101.150",CODE_COMM_TYPE_LAN,CODE_COMM_PROTOCOL_MC,3040,0},	//ID_COMM_MAIN_PC_PLC
				{"192.168.101.151",CODE_COMM_TYPE_LAN,CODE_COMM_PROTOCOL_UDP,8001,0},	//ID_COMM_CRANE_OTE_IF
				{"192.168.101.152",CODE_COMM_TYPE_LAN,CODE_COMM_PROTOCOL_UDP,8001,0},	//ID_COMM_WIFI
				default_addr,
				default_addr,
				default_addr,
				default_addr,
				{"192.168.101.100",CODE_COMM_TYPE_LAN,CODE_COMM_PROTOCOL_UDP,8001,0},	//ID_COMM_OPTION 制御PC内部OTE IF用ポート（デバッグ用）
			},
			{//WAN
				{"10.130.6.105",CODE_COMM_TYPE_WAN,CODE_COMM_PROTOCOL_UDP,8001,0},		//ID_COMM_WAN0
				{"10.130.5.74",CODE_COMM_TYPE_WAN,CODE_COMM_PROTOCOL_UDP,8001,0}		//ID_COMM_WAN1(保守用WAN)
			},
			{//PLC
				{"192.168.101.160",CODE_COMM_TYPE_LAN,CODE_COMM_PROTOCOL_MC,3030,0},	//ID_COMM_MAIN_PC_PLC
				default_addr
			},
			{//AUX
				default_addr,	
				default_addr,
				default_addr,
				default_addr
			},
		};
		set_addr_set(CODE_COMM_LOCATION_OTE, machine_id, &st_src);
	}

	//OTE_ID_HHGG3801
	{
		int machine_id = OTE_ID_HHGG3801;
		ST_ADDR_SET st_src =
		{
			{//PC
				default_addr,	//ID_COMM_MAIN_PC_PLC
				default_addr,	//ID_COMM_CRANE_OTE_IF
				default_addr,
				default_addr,
				default_addr,
				default_addr,
				default_addr,
				default_addr
			},
			{//WAN
				default_addr,
				default_addr
			},
			{//PLC
				default_addr,	//ID_COMM_MAIN_PC_PLC
				default_addr
			},
			{//AUX
				default_addr,
				default_addr,
				default_addr,
				default_addr
			},
		};
		set_addr_set(CODE_COMM_LOCATION_OTE, machine_id, &st_src);
	}

//#############################################################
//############# マルチキャストアドレスセット　#################
//#############################################################

	//クレーン間マルチキャストアドレス	
	{
		ST_ADDR_ITEM mcast = { "239.1.0.2", CODE_COMM_TYPE_LAN, CODE_COMM_PROTOCOL_MCAST, 20080,0 };
		addr_list.mcast_crn_crn = mcast;
	}

	{//クレーン2OTEマルチキャストアドレス

		ST_ADDR_ITEM mcast = { "239.1.0.2", CODE_COMM_TYPE_LAN, CODE_COMM_PROTOCOL_MCAST, 20082,0 };
		addr_list.mcast_crn_crn = mcast;
	}

	//OTE間マルチキャストアドレス	
	{
		ST_ADDR_ITEM mcast = { "239.1.0.1", CODE_COMM_TYPE_LAN, CODE_COMM_PROTOCOL_MCAST, 20083,0 };
		addr_list.mcast_ote_ote = mcast;
	}

	{//クレーン2OTEマルチキャストアドレス

		ST_ADDR_ITEM mcast = { "239.1.0.1", CODE_COMM_TYPE_LAN, CODE_COMM_PROTOCOL_MCAST, 20081,0 };
		addr_list.mcast_ote_crn = mcast;
	}
	
	return 0;
}

