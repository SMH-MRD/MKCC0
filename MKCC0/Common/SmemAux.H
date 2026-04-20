#pragma once

#include "AUXEQ_DEF.h"
#include "Windows.h"
#include <opencv2/opencv.hpp>

using namespace std;
/****************************************************************************/
/*   補機信号定義構造体                                  　         　		*/
/* 　AUXEQがセットする共有メモリ上の情報　      　　　　　　				*/
/****************************************************************************/
#define SMEM_AUX_ENV_INF_NAME			L"AUX_ENV_INF"
#define SMEM_AUX_AGENT_INF_NAME			L"AUX_AGENT_INF"
#define SMEM_AUX_CS_INF_NAME			L"AUX_CS_INF"
#define SMEM_AUX_POL_INF_NAME			L"AUX_POL_INF"

#define MUTEX_AUX_ENV_INF_NAME			L"MU_AUX_ENV_INF"
#define MUTEX_AUX_AGENT_INF_NAME		L"MU_AUX_AGENT_INF"
#define MUTEX_AUX_CS_INF_NAME			L"MU_AUX_CS_INF"
#define MUTEX_AUX_POL_INF_NAME			L"MU_AUX_POL_INF"

#define SMEM_OTE_AUX_ENV_INF_NAME			L"OTE_AUX_ENV_INF"
#define SMEM_OTE_AUX_AGENT_INF_NAME		L"OTE_AUX_AGENT_INF"
#define SMEM_OTE_AUX_CS_INF_NAME		L"OTE_AUX_CS_INF"
#define SMEM_OTE_AUX_POL_INF_NAME		L"OTE_AUX_POL_INF"

#define MUTEX_OTE_AUX_ENV_INF_NAME		L"MU_OTE_AUX_ENV_INF"
#define MUTEX_OTE_AUX_AGENT_INF_NAME	L"MU_OTE_AUX_AGENT_INF"
#define MUTEX_OTE_AUX_CS_INF_NAME		L"MU_OTE_AUX_CS_INF"
#define MUTEX_OTE_AUX_POL_INF_NAME		L"MU_OTE_AUX_POL_INF"

#define AUX_SLBRK_COM_HW_BRK			BIT4	//HaerWare Brake
#define AUX_SLBRK_COM_RESET				BIT5	//Reset
#define AUX_SLBRK_COM_PARKING			BIT6	//parking brake
#define AUX_SLBRK_COM_CHK				BIT7	//Auto Select Mode

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
	INT16 brk_fb_org_pt;
	INT16 brk_fb_emg;
	INT16 brk_fb_autosel;
	INT16 brk_fb_err_map;
	INT16 brk_fb_err_code;
	INT16 brk_fb_rbsl_pos;
	INT16 healthy_err;
	INT16 brk_fb_karaburi;
	INT16 brk_fb_time_over;
	INT16 brk_fb_release;
	INT16 brk_fb_sys_err;
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


/****************************************************************************/
/*   補機信号定義構造体(OTE)                                  　         　	*/
/* 　OTEAUXEQがセットする共有メモリ上の情報　      　　　　　　				*/
/****************************************************************************/
typedef struct _ST_OTE_AUX_ENV_INF {

	ST_DEVICE_CODE device_code;
	INT32 video_delay_chk_func_active = L_ON;	//ビデオ遅延チェック機能有効フラグ
	INT32 video_delay_ms = 10000;				//ビデオ遅延計測値
	DWORD helthy_cnt;

}ST_OTE_AUX_ENV_INF, * LPST_OTE_AUX_ENV_INF;

typedef struct _USB_CAM
{
	int width;
	int height;
	size_t stride;							// 1行のバイト数
	void* pRawData;							//画像バッファポインタ
	int is_first_frame = true;				//初期化フラグ  
	std::mutex g_mtx;
	cv::Mat bgrMatFrame;					 // メインスレッドと共有する画像
	cv::Mat hsvMatFrame;					 // メインスレッドと共有する画像
	bool isRawMatUpdated = false;
}ST_USB_CAM, LPST_USB_CAM;

typedef struct _ST_OTE_AUX_AGENT_INF {	//MCprotocolの送受信バッファ
	ST_USB_CAM st_usb_cam;
}ST_OTE_AUX_AGENT_INF, * LPST_OTE_AUX_AGENT_INF;

typedef struct _ST_VIDEO_CHK_FB {
	INT32 video_delay_ms;
}ST_VIDEO_CHK_FB, * LPST_VIDEO_CHK_FB;
typedef struct _ST_OTE_AUX_CS_INF {		//OTEプロセスとのIF用共有メモリバッファ
	//AUXEQからOTEプロセスへ
	ST_VIDEO_CHK_FB st_video_chk;
}ST_OTE_AUX_CS_INF, * LPST_OTE_AUX_CS_INF;
typedef struct _ST_OTE_AUX_POL_INF {
	int lamp_status = L_OFF;
}ST_OTE_AUX_POL_INF, * LPST_OTE_AUX_POL_INF;
