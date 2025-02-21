#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <ws2tcpip.h>

#include <iostream>
#include <iomanip>
#include <sstream>

#include "NET_DEF.h"

using namespace std;

//**********************************************************************************************************/
//ソケット関連定義
/**********************************************************************************************************/
#define ID_SOCK_EVENT_SWAY_PC    	10700					//PC受信ソケットイベント

#define TCP_PROTOCOL	SOCK_STREAM
#define UDP_PROTOCOL	SOCK_DGRAM

#define ACCESS_TYPE_CLIENT	1
#define ACCESS_TYPE_SERVER	2

#define MAX_N_MULTICAST				4
#define DEFAULT_SOCK_EVENT		10600

#define CSOCK_N_STAT				16 //初期状態
#define CSOCK_STAT_NA				0 //初期状態
#define CSOCK_STAT_NON_VALID_IP		1 //有効なIP無し
#define CSOCK_STAT_ERR_WINSOCK		2 //WINSOCK設定でエラー
#define CSOCK_STAT_ERR_BIND			3 //バインドエラー
#define CSOCK_STAT_ACTIVE			8//ソケット準備完了
#define CSOCK_STAT_RCV_ERR			9//受信エラー
#define CSOCK_STAT_SND_ERR			10//送信エラー

struct ERRMSG_CSOCK {
	WCHAR err_msg_wch[CSOCK_N_STAT][128] = {
		L"初期状態",L"有効なIPがありません",L"WINSOCK ERROR",L"バインドエラー",L"",L"",L"",
		L"ソケット準備完了",L"受信エラー",L"送信エラー",L"",L"",L"",L"",L"",L""
	};
};

class CSockBase {
public:
	CSockBase();
	virtual ~CSockBase(void);
	
	INT32 eventID;																		//非同期イベントID
	INT32 protocol;																		//プロトコル
	INT32 access;																		//クライアント/サーバー種別
	INT32 n_multicast;																	//マルチキャストアドレス設定数
	SOCKET s;																			//ソケット

	SOCKADDR_IN addr_in_rcv;															//受信アドレス
	SOCKADDR_IN addr_in_dst;															//送信先アドレス
	SOCKADDR_IN addr_in_from;															//送信元アドレス
	SOCKADDR_IN addr_in_multi[MAX_N_MULTICAST];											//マルチキャストアドレス

	CRITICAL_SECTION csSck;																//受信排他用クリティカルセクション

	wostringstream err_msg;
	wstring wstr_sock_err;
	INT32 status = 0;
	
	//メンバ関数
	void set_sock_addr(SOCKADDR_IN* paddr, PCSTR ip, USHORT port) {//アドレスセット
		paddr->sin_family = AF_INET;paddr->sin_port = htons(port);inet_pton(AF_INET, ip, &(paddr->sin_addr.s_addr)); return;
	}
	virtual HRESULT GetSockMsg(int nError);
	virtual HRESULT SetSockErr(int nError);
	virtual HRESULT Initialize();		//WinSock初期化
	virtual HRESULT Close();			//Socket　Close

	//インターフェイス
															//WinSock初期化
	virtual HRESULT init_sock(HWND hwnd, SOCKADDR_IN addr_in)=0;								//受信ソケットの初期化
	virtual HRESULT init_sock(HWND hwnd, SOCKADDR_IN addr_in, SOCKADDR_IN addr_in_m)=0;			//受信ソケットの初期化(マルチキャスト）

	virtual HRESULT add_multi_to_sock(SOCKADDR_IN addr_in_m)=0;									//マルチキャストグループへの参加登録
	virtual HRESULT set_dst_addr(SOCKADDR_IN addr_in) = 0;//送信先アドレスセット

	virtual int snd_msg(const char* pbuf, int len, SOCKADDR_IN addr)=0;							//メッセージ送信
	virtual int rcv_msg(char* pbuf, int len)=0;													//メッセージ受信

};


class CSockUDP:public CSockBase
{
public:
	CSockUDP();
	CSockUDP(INT32 access, INT32 eventID);
	virtual ~CSockUDP(void);
											
	virtual HRESULT init_sock(HWND hwnd, SOCKADDR_IN addr_in);								//受信ソケットの初期化
	virtual HRESULT init_sock(HWND hwnd, SOCKADDR_IN addr_in, SOCKADDR_IN addr_in_m);		//受信ソケットの初期化(マルチキャスト）

	virtual int snd_msg(const char* pbuf, int len, SOCKADDR_IN addr);						//メッセージ送信
	virtual int rcv_msg(char* pbuf, int len);	//メッセージ受信

	virtual HRESULT add_multi_to_sock(SOCKADDR_IN addr_in_m); //マルチキャストグループへの参加登録
	virtual HRESULT set_dst_addr(SOCKADDR_IN addr_in) { addr_in_dst = addr_in; return S_OK; };		//送信先アドレスセット
};
