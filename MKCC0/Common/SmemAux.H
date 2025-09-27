#pragma once

#include "AUXEQ_DEF.h"

using namespace std;
/****************************************************************************/
/*   補機信号定義構造体                                  　         　		*/
/* 　AUXEQがセットする共有メモリ上の情報　      　　　　　　				*/
/****************************************************************************/
#define SMEM_AUX_ENV_INF_NAME			L"AUX_ENV_INF"
#define SMEM_AUX_AGENT_INF_NAME			L"AUX_AGENT_INF"
#define SMEM_AUX_CS_INF_NAME			L"AUX_CS_INF"

#define MUTEX_AUX_ENV_INF_NAME			L"MU_AUX_ENV_INF"
#define MUTEX_AUX_AGENT_INF_NAME		L"MU_AUX_AGENT_INF"
#define MUTEX_AUX_CS_INF_NAME			L"MU_AUX_CS_INF"

typedef struct _ST_AUX_ENV_INF {

	ST_DEVICE_CODE device_code;
	DWORD helthy_cnt;

}ST_AUX_ENV_INF, * LPST_AUX_ENV_INF;

typedef struct _ST_SLBRK_COM {
	INT16 pc_com_mode;
	INT16 pc_com_brk_level;
	INT16 pc_com_hw_brk;
	INT16 pc_com_reset;
	INT16 pc_com_emg;
	INT16 pc_com_autosel;
}ST_SLBRK_COM, * LPST_SLBRK_COM;

typedef struct _ST_SLBRK_FB {
	INT16 brk_fb_level;
	INT16 brk_fb_hw_brk;
	INT16 brk_fb_reset;
	INT16 brk_fb_emg;
	INT16 brk_fb_autosel;
	INT16 brk_fb_healthy;
	INT16 brk_fb_err_map;
	INT16 brk_fb_err_code;
	INT16 brk_fb_rbsl_pos;
	INT16 d16;
	INT16 d17;
	INT16 d18;
	INT16 d19;
	INT16 d20;
	INT16 d21;
	INT16 d22;
}ST_SLBRK_FB, * LPST_SLBRK_FB;

typedef struct _ST_AUX_AGENT_INF {	//MCprotocolの送受信バッファ
	INT16 slbrk_wbuf[16];
	INT16 slbrk_rbuf[16];
	
}ST_AUX_AGENT_INF, * LPST_AUX_AGENT_INF;

typedef struct _ST_AUX_CS_INF {		//MAINプロセスとのIF用共有メモリバッファ
	//MAINプロセスからAUXEQへ
	DWORD main_helthy_cnt;
	ST_SLBRK_COM com_slbrk;

	//AUXEQからMAINプロセスへ
	DWORD aux_helthy_cnt;
	ST_SLBRK_FB fb_slbrk;
}ST_AUX_CS_INF, * LPST_AUX_CS_INF;

