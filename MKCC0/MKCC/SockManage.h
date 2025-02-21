#pragma once

#include "CSockLib.h"

typedef struct _ST_SOCK_BASE {
	static CSockUDP* pSockOteUniCastPc;		//OTE��PC���j�L���X�g��M�p�\�P�b�g
	static CSockUDP* pSockPcMultiCastPc;		//PC->PC�}���`�L���X�g��M�p�\�P�b�g
	static CSockUDP* pSockOteMultiCastPc;		//OTE��PC�}���`�L���X�g��M�p�\�P�b�g

	static SOCKADDR_IN addrin_ote_u_pc;			//OTE��PC PC���j�L���X�g��M�A�h���X�iPC�p)
	static SOCKADDR_IN addrin_pc_m_pc;			//PC->PC PC�}���`�L���X�g��M�A�h���X�iPC�p)
	static SOCKADDR_IN addrin_ote_m_pc;			//OTE��PC OTE�}���`�L���X�g��M�A�h���X�iPC�p)

	static SOCKADDR_IN addrin_pc_u_snd;		    //PC->OTE PC���j�`�L���X�g���M��A�h���X�iPC��M�p)
	static SOCKADDR_IN addrin_pc_m_pc_snd;		//PC->PC PC�}���`�L���X�g���M��A�h���X�iPC��M�p)
	static SOCKADDR_IN addrin_pc_m_ote_snd;		//PC->OTE PC�}���`�L���X�g���M��A�h���X�iOTE��M�p)

	static SOCKADDR_IN addrin_ote_u_from;		//OTE���j�L���X�g���M���A�h���X�iPC�p)
	static SOCKADDR_IN addrin_pc_m_from;		    //PC�}���`�L���X�g���M���A�h���X�iPC�p)
	static SOCKADDR_IN addrin_ote_m_from;		//OTE�}���`�L���X�g���M���A�h���X�iPC�p)
}ST_SOCK_BASE, * LPST_SOCK_BASE;

