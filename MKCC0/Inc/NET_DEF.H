#pragma once

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

#define ID_SOCK_EVENT_SWAY_PC    	10700					//PC受信ソケットイベント