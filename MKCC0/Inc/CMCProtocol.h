#pragma once
#include <ws2tcpip.h>

#include <windows.h>
#include <commctrl.h>
#include <time.h>
#include <string>
#include "NET_DEF.H"

#include <iostream>
#include <iomanip>
#include <sstream>

#define ID_SOCK_MC_CLIENT   PORT_MC_CLIENT
#define ID_SOCK_MC_SERVER   PORT_MC_SERVER

#define ID_SOCK_MC_CC_AGENT		PORT_MC_CLIENT
#define ID_SOCK_MC_OTE_AGENT    PORT_MC_CLIENT
#define ID_SOCK_MC_OTE_CS		PORT_MC_CLIENT

#define CC_MC_ADDR_W_READ	10300
#define CC_MC_SIZE_W_READ	100
#define CC_MC_ADDR_W_WRITE	10200
#define CC_MC_SIZE_W_WRITE	100

#define OTE_MC_ADDR_W_READ	10300
#define OTE_MC_SIZE_W_READ	100
#define OTE_MC_ADDR_W_WRITE	10200
#define OTE_MC_SIZE_W_WRITE	100

#define CODE_3E_FORMAT		0x50
#define CODE_4E_FORMAT		0x54
#define CODE_3E_NULL		0x00
#define CODE_NW				0x00
#define CODE_PC				0xFF

#define CODE_UIO			0x3FF
#define CODE_UIO_L			0xFF
#define CODE_UIO_H			0x03
#define CODE_UNODE			0x00

#define CODE_MON_TIMER		0x0001
#define CODE_DEVICE_D		0xA8

#define CODE_COMMAND_READ	0x0401
#define CODE_SUBCOM_READ	0x00	//WORD単位読み出し

#define CODE_COMMAND_WRITE	0x1401
#define CODE_SUBCOM_WRITE	0x00	//WORD単位書き込み

#define MAX_MC_DATA			1024

#define MC_RES_WRITE		2		//応答受信タイプ　書き込み
#define MC_RES_READ			3		//応答受信タイプ　書き込み
#define MC_RES_ERR			0		//応答受信タイプ　エラー

//要求送信電文フォーマット
typedef struct _stXEMsgReq {
	UINT16	subcode;				//サブヘッダ
	UINT16  serial;					//シリアル
	UINT16	blank;					//空き
	UINT8	nNW;					//ネットワーク番号
	UINT8	nPC;					//PC番号
	UINT16	nUIO;					//要求ユニットIO番号
	UINT8	nUcode;					//要求ユニット局番
	UINT16	len;					//データ長
	UINT16	timer;					//監視タイマ
	UINT16	com;					//コマンド
	UINT16	scom;					//サブコマンド
	UINT16	d_no;					//デバイス番号(3byte表現の2byteのみ利用する事にする）
	UINT8	d_no0;					//デバイス番号(3byte表現の2byteのみ利用する事にする）
	UINT8	d_code;					//デバイスコード
	UINT16	n_device;				//読み/書きデバイス点数
	UINT16	req_data[MAX_MC_DATA];	//書き込みデータ
}ST_XE_REQ, * LPST_XE_REQ;
//受信電文フォーマット
typedef struct _stXEMsgRes {
	UINT16	subcode;				//サブヘッダ
	UINT16  serial;					//シリアル
	UINT16	blank;					//空き
	UINT8	nNW;					//ネットワーク番号
	UINT8	nPC;					//PC番号
	UINT16	nUIO;					//要求ユニットIO番号
	UINT8	nUcode;					//要求ユニット局番
	UINT16	len;					//データ長
	UINT16	endcode;				//終了コード
	UINT16	res_data[MAX_MC_DATA];	//読み込みデータ
}ST_XE_RES, * LPST_XE_RES;

class CMCProtocol
{
public:
	CMCProtocol(INT32 _eventID);
	~CMCProtocol();	
	
	//通信電文送受信内容構築用バッファ 
	UINT8 read_req_snd_buf[MAX_MC_DATA * 2];	//読込み要求送信用バッファ
	UINT8 write_req_snd_buf[MAX_MC_DATA * 2];	//書込み要求送信用バッファ

	UINT8 rcv_buf[MAX_MC_DATA * 2];
	UINT8 snd_buf[MAX_MC_DATA * 2];

	std::wostringstream msg_wos;

	//通信メッセージバッファ
	ST_XE_REQ mc_req_msg_r, mc_req_msg_w;
	ST_XE_RES mc_res_msg_r, mc_res_msg_w, mc_res_msg_err;
	
	//読み書きデバイスアドレス,バッファサイズセット/ゲット
	UINT16 D_no_r, D_no_w, n_D_read, n_D_write;
	void set_access_D_w(UINT16 no, UINT16 num) { D_no_w = no; n_D_write = num; return; };
	void set_access_D_r(UINT16 no, UINT16 num) { D_no_r = no; n_D_read = num; return; };

	//初期化/終了
	HRESULT Initialize(HWND hwnd,int com_id);
	HRESULT close();

	HRESULT set_sndbuf_read_D_3E();				//3E　Dデバイス読み込み要求送信フォーマットセット
	HRESULT send_read_req_D_3E();				//3Eフォーマット Wデバイス読み出し要求送信

	HRESULT set_sndbuf_write_D_3E();			//3E　Dデバイス書き込み要求送信フォーマットセット
	HRESULT send_write_req_D_3E(void* p_data);	//3Eフォーマット Wデバイス書き込み要求送信
		
	UINT rcv_msg_3E();
	
	//デバッグ,シミュレータ用
	UINT16 snd_responce(ST_XE_REQ st_com, UINT16* pdata);
	HRESULT parse_snd_buf(UINT8* p8,LPST_XE_REQ pbuf);

private:
	INT32 eventID;

};