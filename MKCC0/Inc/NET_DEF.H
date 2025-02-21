#pragma once

/**********************************************************************************************************/
//アドレス定義
/**********************************************************************************************************/
//DEFAULT
#define DEFAULT_MY_IP1				"127.0.0.1"
#define DEFAULT_MY_IP2				"127.0.0.2"
#define DEFAULT_MY_IP3				"127.0.0.3"
#define DEFAULT_MY_PORT				10050

//PLC IF
#define IP_ADDR_MC_CLIENT			"192.168.100.100"		//PLC IF 制御PC　IPアドレス
#define IP_ADDR_MC_SERVER			"192.168.100.101"		//PLC IF PLC IPアドレス

#define PORT_MC_CLIENT				3020					//制御PCポート
#define PORT_MC_SERVER				3010					//CPU本体の時のPLCポート
//#define PORT_MC_SERVER			1025					//Ethernetユニットの時

//振れセンサ
#define IP_ADDR_SWAY_CLIENT	        "192.168.100.100"		//振れセンサ制御PC　
#define IP_ADDR_SWAY_SERVER			"192.168.100.20"		//振れセンサセンサPC
#define IP_PORT_SWAY_CLIENT			10080					//振れセンサ用クライアント（制御PC）　IP受信ポート
#define IP_PORT_SWAY_SERVER			10081					//振れセンサ用サーバ（センサPC）　    IP受信ポート

#define IP_ADDR_SWAYCAM_CLIENT		"192.168.150.10"		//振れセンサPCcカメラIF IPアドレス
#define IP_ADDR_SWAY_CAMERA		    "192.168.150.100"		//振れセンサカメラ      IPアドレス

//OTE
#define OTE_IF_UNI_IP_PC			"192.168.100.100" 		//PCのOTE UNICAST　IF用IP	（受信,送信）
#define OTE_IF_UNI_IP_PC_WAN		"100.87.239.249" 		//PCのOTE UNICAST　IF用IP	（送信 WAN回線）
#define OTE_IF_UNI_IP_PC_VPN		"192.168.1.9" 			//PCのOTE UNICAST　IF用IP	（受信 VPN回線）
#define OTE_IF_UNI_IP_OTE0			OTE_IF_UNI_IP_PC		//デフォルトOTEのUNICAST用IP（受信,送信）

#define OTE_IF_UNI_PORT_PC			10050					//ユニキャストPC受信ポート
#define OTE_IF_UNI_PORT_OTE			10051					//ユニキャスト端末受信ポート

#define OTE_IF_MULTI_IP_PC			"239.1.0.2"				//PCマルチキャスト用IP
#define OTE_IF_MULTI_IP_OTE			"239.1.0.1"				//OTEマルチキャスト用IP

#define OTE_IF_MULTI_PORT_PC2PC		20080					//PC発信マルチキャストOTE受信ポート
#define OTE_IF_MULTI_PORT_OTE2PC	20081					//OTE発信マルチキャストPC受信ポート;
#define OTE_IF_MULTI_PORT_PC2OTE	20082					//PC発信マルチキャストOTE受信ポート
#define OTE_IF_MULTI_PORT_OTE2OTE	20083					//OTE発信マルチキャストPC受信ポート;

#define ID_SOCK_EVENT_OTE_UNI	    10603					//PC受信ソケットイベント　OTE UNICASTメッセージ
#define ID_SOCK_EVENT_PC_MUL		10604					//PC受信ソケットイベント　PC MULTICASTメッセージ
#define ID_SOCK_EVENT_OTE_MUL		10605					//PC受信ソケットイベント　OTE MULTICASTメッセージ
