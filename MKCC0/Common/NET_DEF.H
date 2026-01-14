#pragma once

#define _DEBUG_PC0

/**********************************************************************************************************/
//アドレス定義
/**********************************************************************************************************/

//@ DEFAULT-----------------------------------------------------------------------------------
#define DEFAULT_MY_IP1					"127.0.0.1"
#define DEFAULT_MY_IP2					"127.0.0.2"
#define DEFAULT_MY_IP3					"127.0.0.3"
#define DEFAULT_MY_PORT					10050

//@ CRANE-------------------------------------------------------------------------------------
#define OTE_IF_CRANE_IP_PC0				"192.168.100.101"	//PC単体デバッグ用
#define OTE_IF_CRANE_IP_H6R602			"192.168.100.101"	//みらい（デフォルト）
#define OTE_IF_CRANE_IP_HHGH29			"192.168.101.101"	//今造多度津JC70t(102号)
#define OTE_IF_CRANE_IP_HHGQ18			"192.168.102.101"	//今造多度津JC300t(10号)
#define OTE_IF_CRANE_IP_HHFR22			"192.168.103.101"	//今造丸亀GC1300t(1号)

#define OTE_IF_CRANE_IP_HHGH29_WAN		"10.130.7.205"	//今造多度津JC70t(102号) WAN回線

//@ OTE---------------------------------------------------------------------------------------
#define OTE_IF_UNI_IP_PC				"192.168.101.101" 	//PCのOTE UNICAST　IF用IP	（受信,送信）
#define OTE_IF_UNI_IP_PC_HHGG8200		"192.168.101.101" 	//PCのOTE UNICAST　IF用IP	（受信,送信）
#define OTE_IF_UNI_IP_PC_OTE0			"192.168.101.102"	//デフォルトOTEのUNICAST用IP（受信,送信）
#define OTE_IF_UNI_IP_PC_OTE1			"192.168.101.103"	//デフォルトOTEのUNICAST用IP2（受信,送信）

#define OTE_IF_UNI_IP_OTE_HHGG3801		"192.168.101.151"	//デフォルトOTEのUNICAST用IP（受信,送信）
#define OTE_IF_UNI_IP_OTE_HHGG3801_W	"192.168.101.152"	//デフォルトOTEのSUB IP（Wifi 受信,送信）

#define OTE_IF_UNI_IP_PC_WAN			"100.87.239.249" 	//PCのOTE UNICAST　IF用IP	（送信 WAN回線）
#define OTE_IF_UNI_IP_PC_WAN_HHGG3801	"10.130.6.105" 		//PCのOTE UNICAST　IF用IP	（送信 WAN回線 今造操作卓No.1）
#define OTE_IF_UNI_IP_PC_WAN_MENTE01	"10.130.6.106" 		//PCのOTE UNICAST　IF用IP	（送信 WAN回線　保守端末No1）


//#define OTE_IF_UNI_PORT_PC			10050				//ユニキャストPC受信ポート
//#define OTE_IF_UNI_PORT_OTE			10051				//ユニキャスト端末受信ポート

#define OTE_IF_UNI_PORT_PC				8001				//ユニキャストPC受信ポート
#define OTE_IF_UNI_PORT_OTE				8001				//ユニキャスト端末受信ポート

#define OTE_IF_MULTI_IP_PC				"239.1.0.2"			//PCマルチキャスト用IP
#define OTE_IF_MULTI_IP_OTE				"239.1.0.1"			//OTEマルチキャスト用IP

#define OTE_IF_MULTI_PORT_PC2PC			20080				//PC発信マルチキャストOTE受信ポート
#define OTE_IF_MULTI_PORT_OTE2PC		20081				//OTE発信マルチキャストPC受信ポート;
#define OTE_IF_MULTI_PORT_PC2OTE		20082				//PC発信マルチキャストOTE受信ポート
#define OTE_IF_MULTI_PORT_OTE2OTE		20083				//OTE発信マルチキャストPC受信ポート;

#define ID_SOCK_EVENT_OTE_UNI			10603				//PC受信ソケットイベント　OTE UNICASTメッセージ
#define ID_SOCK_EVENT_PC_MUL			10604				//PC受信ソケットイベント　PC MULTICASTメッセージ
#define ID_SOCK_EVENT_OTE_MUL			10605				//PC受信ソケットイベント　OTE MULTICASTメッセージ


//@ PLC IF-----------------------------------------------------------------------------------
#define PLC_IF_TYPE_CC				0
#define PLC_IF_TYPE_OTE				1
#define PLC_IF_TYPE_SLBRK			2
#define PLC_IF_TYPE_OTE_DEBUG		3
#define IP_ADDR_MC_CLIENT			"192.168.101.100"		//PLC IF 制御PC　IPアドレス
#define IP_ADDR_MC_SERVER			"192.168.101.110"		//PLC IF PLC IPアドレス
#define PORT_MC_CLIENT				3020					//制御PCポート
#define PORT_MC_SERVER				3010					//CPU本体の時のPLCポート

//#define IP_ADDR_MC_CLIENT_OTE		OTE_IF_SUB_IP_OTE0		//PLC IF 遠隔操作卓PC　IPアドレス
#define IP_ADDR_MC_CLIENT_OTE		"192.168.101.150"		//PLC IF 遠隔操作卓PC　IPアドレス
#define IP_ADDR_MC_SERVER_OTE		"192.168.101.160"		//PLC IF 遠隔操作卓PLC IPアドレス
#define PORT_MC_CLIENT_OTE			3040					//遠隔PCポート
#define PORT_MC_SERVER_OTE			3030					//OTEの時のPLCポート

#define IP_ADDR_MC_CLIENT_SLBRK		IP_ADDR_MC_CLIENT		//CC_IF_CRANE_SUB_IP		//旋回BRK IF 遠隔PC	IPアドレス
#define IP_ADDR_MC_SERVER_SLBRK		"192.168.101.120"		//旋回BRK IF PLC	IPアドレス
#define PORT_MC_CLIENT_SLBRK		3021					//遠隔PCポート
#define PORT_MC_SERVER_SLBRK		3011					//旋回BRKのPLCポート

#define IP_ADDR_MC_CLIENT_DEF		"127.0.0.1"
#define IP_ADDR_MC_SERVER_DEF		"127.254.254.254"

#define ID_SOCK_EVENT_CC_ENV_UNI	10503					//CC ENV受信ソケットイベント

//@ 補機-------------------------------------------------------------------------------

#define IP_ADDR_AUX_CLIENT	        OTE_IF_UNI_IP_PC		//補機通信クライアント側　
#define IP_ADDR_AUX_SERVER			OTE_IF_UNI_IP_PC		//補機通信サーバ側
#define IP_PORT_AUX_CS_CLIENT		10070					//補機通信クライアント（制御PC）IP受信ポート
#define IP_PORT_AUX_CS_SERVER		10071					//補機通信サーバ（センサPC）　   IP受信ポート

#define IP_ADDR_SWAYCAM_CLIENT		"192.168.150.10"		//振れセンサPCcカメラIF IPアドレス
#define IP_ADDR_SWAY_CAMERA		    "192.168.150.100"		//振れセンサカメラ      IPアドレス

#define ID_SOCK_EVENT_AUXCS_UNI	    10703					//補機プロセスCS受信ソケットイベント

namespace DEFAULT
{
#define IP_ADDR_CAMERA1				"192.168.100.201"
#define IP_PORT_CAMERA1				1201
#define IP_ADDR_CAMERA2				"192.168.100.202"
#define IP_PORT_CAMERA2				1202
#define IP_ADDR_CAMERA3				"192.168.100.203"
#define IP_PORT_CAMERA3				1203
#define IP_ADDR_CAMERA4				"192.168.100.204"
#define IP_PORT_CAMERA4				1204
#define IP_ADDR_CAMERA5				"192.168.100.205"
#define IP_PORT_CAMERA5				1205
#define IP_ADDR_CAMERA6				"192.168.100.206"
#define IP_PORT_CAMERA6				1206
#define IP_ADDR_CAMERA7				"192.168.100.207"
#define IP_PORT_CAMERA7				1207
#define IP_ADDR_CAMERA8				"192.168.100.208"
#define IP_PORT_CAMERA8				1208
}
namespace HHGH29
{
#define IP_ADDR_CAMERA1				"192.168.100.201"
#define IP_PORT_CAMERA1				1201
#define IP_ADDR_CAMERA2				"192.168.100.202"
#define IP_PORT_CAMERA2				1202
#define IP_ADDR_CAMERA3				"192.168.100.203"
#define IP_PORT_CAMERA3				1203
#define IP_ADDR_CAMERA4				"192.168.100.204"
#define IP_PORT_CAMERA4				1204
#define IP_ADDR_CAMERA5				"192.168.100.205"
#define IP_PORT_CAMERA5				1205
#define IP_ADDR_CAMERA6				"192.168.100.206"
#define IP_PORT_CAMERA6				1206
#define IP_ADDR_CAMERA7				"192.168.100.207"
#define IP_PORT_CAMERA7				1207
#define IP_ADDR_CAMERA8				"192.168.100.208"
#define IP_PORT_CAMERA8				1208
}

