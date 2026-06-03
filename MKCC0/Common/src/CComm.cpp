#include "CComm.h"
#include "NET_DEF.h"

ST_COMM_SET	CComm::addr_list;


CComm::CComm() {
	setup();
};
CComm:: ~CComm() {};

int CComm::setup(){

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
		int machine_id = CARNE_ID_HHGH29;
		ST_ADDR_SET st_src =
		{
			{//PC
				default_addr,	//ID_COMM_MAIN_PC_PLC
				default_addr,	//ID_COMM_CRANE_OTE_IF
				default_addr,	//ID_COMM_PC_SLBRK
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
				default_addr,	//ID_COMM_PC_SLBRK
				default_addr,
				default_addr,
				default_addr
			},	
		};
		set_addr_set(CODE_COMM_LOCATION_CRANE, machine_id, &st_src);
	}

	//CARNE_ID_HHGQ18
	{
		int machine_id = CARNE_ID_HHGQ18;
		ST_ADDR_SET st_src =
		{
			{//PC
				{OTE_IF_CRANE_IP_HHGH29,CODE_COMM_TYPE_LAN,CODE_COMM_PROTOCOL_UDP,OTE_IF_UNI_PORT_PC,0},	//ID_COMM_MAIN_PC_PLC
				default_addr,	//ID_COMM_CRANE_OTE_IF
				default_addr,	//ID_COMM_PC_SLBRK
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
				default_addr,	//ID_COMM_PC_SLBRK
				default_addr,
				default_addr,
				default_addr
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
				default_addr,	//ID_COMM_MAIN_PC_PLC
				default_addr,	//ID_COMM_CRANE_OTE_IF
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
		set_addr_set(CODE_COMM_LOCATION_CRANE, machine_id, &st_src);
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
		set_addr_set(CODE_COMM_LOCATION_CRANE, machine_id, &st_src);
	}
	
	return 0;
}

