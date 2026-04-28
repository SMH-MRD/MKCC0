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

#define SMEM_OTE_AUX_ENV_INF_NAME		L"OTE_AUX_ENV_INF"
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
	INT32 video_delay_chk_func_active = L_ON;		//ビデオ遅延チェック機能有効フラグ
	INT32 video_delay_chk_device_forth_on = L_OFF;	//ビデオ遅延チェック用デバイス強制ONフラグ
	INT32 video_delay_chk_auto_prm_set_req = L_OFF;	//ビデオ遅延チェック用自動パラメータ設定要求フラグ
	INT32 video_delay_chk_prm_save_req = L_OFF;	//ビデオ遅延チェック用パラメータ保存要求フラグ
	INT32 video_delay_ms = 10000;				//ビデオ遅延計測値
	DWORD helthy_cnt;

}ST_OTE_AUX_ENV_INF, * LPST_OTE_AUX_ENV_INF;


#define OTEAUXAG_CAMERA_STAT_NORMAL			0//正常
#define OTEAUXAG_CAMERA_STAT_INIT_ERR		1//初期化エラー停止中
#define OTEAUXAG_CAMERA_STAT_SUSPEND		2//停止待機中

#define OTEAUXAG_CAMERA_PRM_RETRY_CNT		200//初期化エラー（カメラ未接続）からのリトライ待ちカウント

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
	int status = OTEAUXAG_CAMERA_STAT_NORMAL;	//状態フラグ
	int retry_count = 0;
}ST_USB_CAM, LPST_USB_CAM;

#define OTEAUXAG_COM_CRANE_DEVICE_ACTIVE	0x0011	//クレーン機器状態 ON要求コード
#define OTEAUXAG_COM_CRANE_DEVICE_DEACTIVE	0x0010	//クレーン機器状態 OFF要求コード

#define OTEAUXAG_CODE_V_DELAY_DEACTIVE			0x0000	//映像遅延検出無効
#define OTEAUXAG_CODE_V_DELAY_STANDBY			0x0001	//映像遅延検出待機中
#define OTEAUXAG_CODE_V_DELAY_AUTO_PRM			0x0002	//映像遅延検出パラメータ設定中
#define OTEAUXAG_CODE_V_DELAY_SAVE_PRM			0x0004	//映像遅延検出パラメータ保存中
#define OTEAUXAG_CODE_V_DELAY_TRIG_ON_CHK		0x0010	//映像遅延検出ONチェック起動
#define OTEAUXAG_CODE_V_DELAY_TRIG_OFF_CHK		0x0020	//映像遅延検出OFFチェック起動
#define OTEAUXAG_CODE_V_DELAY_CHK_FIN			0x0100	//映像遅延検出チェック正常完了フラグ
#define OTEAUXAG_CODE_V_DELAY_CHK_TMOV			0x0200	//映像遅延検出チェックタイムオーバーフラグ
#define OTEAUXAG_CODE_V_DELAY_AUTO_PRM_START	0x1000	//映像遅延検出パラメータ自動設定ターゲット検出中
#define OTEAUXAG_CODE_V_DELAY_AUTO_PRM_FIN		0x4000	//映像遅延検出コマンド実行完了
#define OTEAUXAG_CODE_V_DELAY_AUTO_PRM_FAIL		0x8000	//映像遅延検出コマンド実行エラー

typedef struct _ST_OTE_AUX_AGENT_INF {				//MCprotocolの送受信バッファ
	ST_USB_CAM st_usb_cam;
	INT32 req_command_to_crane	= OTEAUXAG_COM_CRANE_DEVICE_DEACTIVE;
	INT32 v_delay_chk_status	= OTEAUXAG_CODE_V_DELAY_DEACTIVE;
	INT32 v_delay_tmov_cnt = 0;
	INT32 v_delay_chk_fin_cnt = 0;
	double v_delay_sec = 10.0;						//映像遅延計測値
	INT32 v_delay_chk_period_ms = 1000;				//映像遅延計測の周期
	LARGE_INTEGER start_count, end_count;			//システムカウント
	LARGE_INTEGER frequency;						//システム周波数
}ST_OTE_AUX_AGENT_INF, * LPST_OTE_AUX_AGENT_INF;

typedef struct _ST_VIDEO_CHK_FB {
	INT32 video_delay_ms;
}ST_VIDEO_CHK_FB, * LPST_VIDEO_CHK_FB;

typedef struct _ST_OTE_AUX_CS_INF {					//OTEプロセスとのIF用共有メモリバッファ
	//AUXEQからOTEプロセスへ
	ST_VIDEO_CHK_FB st_video_chk;
}ST_OTE_AUX_CS_INF, * LPST_OTE_AUX_CS_INF;


#define OTEAUXPOL_CODE_MAGIC						0x53544341 // "STCA" (Sentech Camera App)
#define OTEAUXPOL_CODE_V_DELAY_PRM_SAVE_START		0x2000	//映像遅延検出パラメータ自動設定ターゲットパラメータセット中
#define OTEAUXPOL_CODE_V_DELAY_PRM_SAVE_FIN			0x4000	//映像遅延検出コマンド実行完了
#define OTEAUXPOL_CODE_V_DELAY_PRM_SAVE_FAIL		0x8000	//映像遅延検出コマンド実行エラー

typedef struct _ST_AUXPOL_IMG_PROC {
	uint32_t magic = OTEAUXPOL_CODE_MAGIC;			//構造体の識別用マジックナンバー
	INT32 v_delay_pam_save_status = L_OFF;
	// HSV マスク設定値
	int hl = 0, hh = 10, hCenter = 0;
	int sl = 0, sh = 255, sCenter = 0;
	int vl = 0, vh = 255, vCenter;
	int h_margin = 20, s_margin = 20, v_margin = 20;
	int status = 0;
	int target_chk_base_count = 0;
	int roi_cnt_diff = 0;
	INT32 is_target_detected = L_OFF;
	int whiteCountInWorkRoi = 0;    //画像処理対象範囲ROI内の白画素数
	int whiteCountInWorkRoi_Last = 0;
	int whiteCountInCriterionRoi = 0;//基準(検出対象）ROI内の白画素数
	cv::Mat mask;
	cv::Mat hsvMat_mask;            //白黒１chのmask画像を表示用に3ｃｈに変換したもの
} ST_AUXPOL_IMG_PROC, * LPST_AUXPOL_IMG_PROC;

typedef struct _ST_OTE_AUX_POL_INF {
	ST_AUXPOL_IMG_PROC st_img_proc;

}ST_OTE_AUX_POL_INF, * LPST_OTE_AUX_POL_INF;
