#pragma once

#include "CSockLib.h"

typedef struct _ST_SOCK_BASE {
	static CSockUDP* pSockOteUniCastPc;		//OTE→PCユニキャスト受信用ソケット
	static CSockUDP* pSockPcMultiCastPc;		//PC->PCマルチキャスト受信用ソケット
	static CSockUDP* pSockOteMultiCastPc;		//OTE→PCマルチキャスト受信用ソケット

	static SOCKADDR_IN addrin_ote_u_pc;			//OTE→PC PCユニキャスト受信アドレス（PC用)
	static SOCKADDR_IN addrin_pc_m_pc;			//PC->PC PCマルチキャスト受信アドレス（PC用)
	static SOCKADDR_IN addrin_ote_m_pc;			//OTE→PC OTEマルチキャスト受信アドレス（PC用)

	static SOCKADDR_IN addrin_pc_u_snd;		    //PC->OTE PCユニチキャスト送信先アドレス（PC受信用)
	static SOCKADDR_IN addrin_pc_m_pc_snd;		//PC->PC PCマルチキャスト送信先アドレス（PC受信用)
	static SOCKADDR_IN addrin_pc_m_ote_snd;		//PC->OTE PCマルチキャスト送信先アドレス（OTE受信用)

	static SOCKADDR_IN addrin_ote_u_from;		//OTEユニキャスト送信元アドレス（PC用)
	static SOCKADDR_IN addrin_pc_m_from;		    //PCマルチキャスト送信元アドレス（PC用)
	static SOCKADDR_IN addrin_ote_m_from;		//OTEマルチキャスト送信元アドレス（PC用)
}ST_SOCK_BASE, * LPST_SOCK_BASE;

