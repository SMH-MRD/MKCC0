#pragma once

#include <winsock.h>
#include <time.h>
#include <windows.h>

#define N_CHAR_AUXEQ_ID             4
#define N_IPRM_AUXEQ_CLIENT         4
#define N_IDATA_AUXEQ_SERVER        4

typedef struct _ST_AUX_COM_MSG_HEAD {           //AUXEQメッセージヘッダ部
    char	id[N_CHAR_AUXEQ_ID];			    //機器個体情報
    UINT32  code;
    sockaddr_in sockaddr;                       //送信元IPアドレス
}ST_AUX_COM_MSG_HEAD, * LPST_AUX_COM_MSG_HEAD;

typedef struct _ST_AUX_COM_CLI_BODY {           //制御メイン送信メッセージボディ部
    INT32 command;
    INT32 mode;
    INT32 freq;                                 //最小受信周期 msec      
    INT32 d[N_IPRM_AUXEQ_CLIENT];               //要求パラメータ
}ST_AUX_COM_CLI_BODY, * LPST_AUX_COM_CLI_BODY;

typedef struct _ST_AUX_COM_SERV_BODY {          //補機システム送信メッセージボディ部
    INT32 command;
    INT32 mode;
    INT32 freq;                                 //最小受信周期 msec      
    INT32 d[N_IDATA_AUXEQ_SERVER];              //補機システム応答データ
}ST_AUX_COM_SERV_BODY, * LPST_AUX_COM_SERV_BODY;

typedef struct _ST_AUX_COM_CLI_MSG {            //制御メイン送信メッセージ
    ST_AUX_COM_MSG_HEAD head;
    ST_AUX_COM_CLI_BODY body;
}ST_AUX_COM_CLI_MSG, * LPST_AUX_COM_CLI_MSG;

typedef struct _ST_AUX_COM_SERV_MSG {          //補機システム送信メッセージ
    ST_AUX_COM_MSG_HEAD head;
    ST_AUX_COM_SERV_BODY body;
}ST_AUX_COM_SERV_MSG, * LPST_AUX_COM_SERV_MSG;


//振れセンサ
#define SID_TG1                      0//ターゲットID
#define SID_TG2                      1
#define SID_TG3                      2
#define SID_TG4                      3
#define SID_RED                      0
#define SID_GREEN                    1
#define SID_BLUE                     2
#define SID_COMMON_FLT               0
#define SID_AXIS_X					 0
#define SID_AXIS_Y					 1

#define SID_CAM_X              ID_SLEW
#define SID_CAM_Y              ID_BOOM_H
#define SID_CAM_XY             ID_TROLLY
#define SID_LOAD_MH            0
#define SID_LOAD_AH            1
#define SID_N_LOAD             2
#define SID_N_AXIS             4

#define CODE_SWAY_IF_MODE_XR_FP_RP	0  //カメラX方向が半径方向　前側＋　右側＋
#define CODE_SWAY_IF_MODE_YR_FP_RP	1  //カメラY方向が半径方向　前側＋　右側＋

#define N_SWAY_SENSOR			3		//振れセンサの数
#define SID_SENSOR1				0		//No.1振れセンサ
#define SID_SENSOR2				1		//No.2振れセンサ
#define SID_SIM					2		//シミュレータ
#define SWAY_SENSOR_N_AXIS		2
#define SID_PRM_AXIS_X			0	    //X軸
#define SID_PRM_AXIS_Y			1		//y軸

#define N_SWAY_SENSOR_CAMERA    2		//振れセンサ１セットのカメラの数
#define SID_CAMERA1				0		//No.1振れセンサ
#define SID_CAMERA2				1		//No.2振れセンサ

#define N_SWAY_SENSOR_TARGET    2		//振れセンサカメラ1台あたりのターゲット数
#define SID_TARGET1				0		//No.1振れセンサ
#define SID_TARGET2				1		//No.2振れセンサ

#define SID_COMMON_FLT               0

#define SID_AXIS_X					0
#define SID_AXIS_Y					1

#define SID_CAM_X              ID_SLEW
#define SID_CAM_Y              ID_BOOM_H
#define SID_CAM_XY             ID_TROLLY
#define SID_LOAD_MH            0
#define SID_LOAD_AH            1
#define SID_N_LOAD             2
#define SID_N_AXIS             4

#define CODE_SWAY_IF_MODE_XR_FP_RP	0  //カメラX方向が半径方向　前側＋　右側＋
#define CODE_SWAY_IF_MODE_YR_FP_RP	1  //カメラY方向が半径方向　前側＋　右側＋


#define SWAY_CAMERA_N_PARAM		10

#define	SID_D0  				0		//カメラハウジング軸-吊点距離（主巻）
#define	SID_row				    1		//カメラのハウジング-吊点角度（主巻）
#define SID_l0					2		//カメラ中心とハウジング回転軸間距離
#define	SID_ph0				    3		//カメラのハウジングへの取り付け角度
#define	SID_phc				    4		//カメラの取り付け角度
#define	SID_D02  				5		//カメラのハウジング-吊点距離（補巻）
#define	SID_row2				6		//カメラのハウジング-吊点角度（補巻）
#define SID_l02					7		//カメラ中心とハウジング回転軸間距離
#define	SID_ph02				8		//カメラのハウジングへの取り付け角度
#define	SID_phc2				9		//カメラの取り付け角度

#define CAM1_SPEC_PIXEL_H       2048
#define CAM1_SPEC_PIXEL_V       1536        // PIX
#define CAM1_SPEC_ANGLE_RAD_H   0.7315
#define CAM1_SPEC_ANGLE_RAD_V   0.5592

#define CAM1_SPEC_RESOLUTION_H  2800.0        // PIX/rad
#define CAM1_SPEC_RESOLUTION_V  2746.0        // PIX/rad

typedef struct StSwySensParam{
    double arr[N_SWAY_SENSOR][N_SWAY_SENSOR_CAMERA][SWAY_SENSOR_N_AXIS][SWAY_CAMERA_N_PARAM] =
    {	//振れセンサ　パラメータ
        {//No.1 センサ
        {{2.683,1.107149,0.07,0.0,0.0,1.697,0.7854,0.07,0.0,0.0},{2.4,0.104,0.071,-0.143,0.0,3.9,1.8675,0.071,-0.143,0.0}},	//カメラ1 x,y方向 D0,row,l0,ph0,phc,D02,row2,l02,ph02,phc2
        {{2.683,1.107149,0.07,0.0,0.0,1.697,0.7854,0.07,0.0,0.0},{2.4,0.104,0.071,-0.143,0.0,3.9,1.8675,0.071,-0.143,0.0}},	//カメラ2 x,y方向 D0,row,l0,ph0,phc,D02,row2,l02,ph02,phc
        },
        {//No.2 センサ
        {{2.683,1.107149,0.07,0.0,0.1,1.697,0.7854,0.07,0.0,0.1},{2.4,0.104,0.071,-0.143,0.1,3.9,1.8675,0.071,-0.143,0.1}},	//カメラ1 x,y方向 D0,row,l0,ph0,phc,D02,row2,l02,ph02,phc
        {{2.683,1.107149,0.07,0.0,0.1,1.697,0.7854,0.07,0.0,0.1},{2.4,0.104,0.071,-0.143,0.1,3.9,1.8675,0.071,-0.143,0.1}},	//カメラ2 x,y方向 D0,row,l0,ph0,phc,D02,row2,l02,ph02,phc
        },
        {//No.3 センサ
        {{2.683,1.107149,0.07,0.0,0.1,1.697,0.7854,0.07,0.0,0.1},{2.4,0.104,0.071,-0.143,0.1,3.9,1.8675,0.071,-0.143,0.1}},	//カメラ1 x,y方向 D0,row,l0,ph0,phc,D02,row2,l02,ph02,phc
        {{2.683,1.107149,0.07,0.0,0.1,1.697,0.7854,0.07,0.0,0.1},{2.4,0.104,0.071,-0.143,0.1,3.9,1.8675,0.071,-0.143,0.1}},	//カメラ2 x,y方向 D0,row,l0,ph0,phc,D02,row2,l02,ph02,phc
        }
    };

    double rad2pix[N_SWAY_SENSOR][N_SWAY_SENSOR_CAMERA][SWAY_SENSOR_N_AXIS] =
    {
        {{CAM1_SPEC_RESOLUTION_H,CAM1_SPEC_RESOLUTION_V}, {CAM1_SPEC_RESOLUTION_H,CAM1_SPEC_RESOLUTION_V}},
        {{CAM1_SPEC_RESOLUTION_H,CAM1_SPEC_RESOLUTION_V}, {CAM1_SPEC_RESOLUTION_H,CAM1_SPEC_RESOLUTION_V}},
        {{CAM1_SPEC_RESOLUTION_H,CAM1_SPEC_RESOLUTION_V}, {CAM1_SPEC_RESOLUTION_H,CAM1_SPEC_RESOLUTION_V}}
     };
}ST_SWY_SENS_PRM, *LPST_SWY_SENS_PRM;

#define SWAY_SENSOR_N_CAM       2
#define SWAY_SENSOR_CAM1        0
#define SWAY_SENSOR_CAM2        1

#define SWAY_SENSOR_N_TARGET    2
#define SWAY_SENSOR_TG1         0
#define SWAY_SENSOR_TG2         1

#define SWAY_SENSOR__MSG_SEND_COM         10600

#define SWAY_SENSOR_N_CHAR_ID   4
#define SWAY_SENSOR_N_CHAR_INFO 32

typedef struct SwayComServerHead { //振れセンサ受信メッセージヘッダ部
    char	id[SWAY_SENSOR_N_CHAR_ID];			//PC ID
    SYSTEMTIME time;		//タイムスタンプ
}ST_SWAY_SERVER_HEAD, * LPST_SWAY_SERVER_HEAD;

typedef struct SwayCamSpec {
    INT32	pix_x;			//カメラ画素数x軸
    INT32	pix_y;			//カメラ画素数y軸
    INT32	pixlrad_x;	    //カメラ分解能　PIX/rad
    INT32	pixlrad_y;	    //カメラ分解能　PIX/rad
    INT32	l0_x;			//カメラ取付パラメータ㎜
    INT32	l0_y;			//カメラ取付パラメータ㎜
    INT32	ph0_x;			//カメラ取付パラメータx1000000rad
    INT32	ph0_y;			//カメラ取付パラメータx1000000rad
    INT32	phc_x;			//カメラ取付パラメータx1000000rad
    INT32	phc_y;			//カメラ取付パラメータx1000000rad
}ST_SWAY_CAM_SPEC, * LPST_SWAY_CAM_SPEC;

#define CODE_SWAY_SENSOR_IMAGE_OK           0x00000001
#define CODE_SWAY_SENSOR_SWAY_DETECTED      0x00000002
#define CODE_SWAY_SENSOR_TG1_DETECTED       0x00000004
#define CODE_SWAY_SENSOR_TG2_DETECTED       0x00000008
#define CODE_SWAY_SENSOR_SWAY_RANGEOVER     0x00000010
#define CODE_SWAY_SENSOR_SWAY_CAM_ERROR     0x00000080

#define CODE_SWAY_SENSOR_TILT_DETECTED      0x00010000
#define CODE_SWAY_SENSOR_TILT_ERROR         0x00100000

typedef struct SwaySensStatus {
    //機器状態情報
    INT32	mode;		    //モード
    INT32	error;		    //エラーステータス
    INT32	status;		    //検出ステータス
    //傾斜計情報  
    double	tilt_x;         //傾斜X
    double	tilt_y;         //傾斜Y
    double	tilt_dx;        //傾斜角速度X
    double	tilt_dy;        //傾斜角速度Y
} ST_SWAY_SENS_STAT, * LPST_SWAY_SENS_STAT;

typedef struct TargetStatus {
    //振れ検出情報  
    double	th_x;		    //振角xPIX
    double	th_y;		    //振角yPIX
    double	dth_x;		    //振角速度x　PIX/s
    double	dth_y;			//振角速度y　PIX/s
    double	th_x0;			//振角0点xPIX
    double	th_y0;			//振角0点yPIX
    double	dpx_tgs;		//ターゲット間距離X方向
    double	dpy_tgs;		//ターゲット間距離Y方向
    double	tg_size;		//ターゲットサイズ
} ST_TARGET_STAT, * LPST_TARGET_STAT;

typedef struct SwayComServerBody {                          //振れセンサ送信メッセージボディ部
    ST_SWAY_CAM_SPEC cam_spec;
    ST_SWAY_SENS_STAT cam_stat;
    ST_TARGET_STAT tg_stat[SWAY_SENSOR_N_TARGET];           //カメラ２ｘターゲット２
    char info[SWAY_SENSOR_N_CHAR_INFO];
}ST_SWAY_SERVER_BODY, * LPST_SWAY_SERVER_BODY;

typedef struct SwayComServerMsg {                           //振れセンサ送信メッセージ
    ST_SWAY_SERVER_HEAD head;
    ST_SWAY_SERVER_BODY body[SWAY_SENSOR_N_CAM];
}ST_SWAY_SERVER_MSG, * LPST_SWAY_SERVER_MSG;

typedef struct SwayComClientHead {                          //制御PC送信メッセージヘッダ部
    char	id[SWAY_SENSOR_N_CHAR_ID];			    //機器個体情報
    sockaddr_in sockaddr;       //送信元IPアドレス
}ST_SWAY_CLIENT_HEAD, * LPST_SWAY_CLIENT_HEAD;

typedef struct SwayComClientBody {                          //制御PC送信メッセージボディ部
    INT32 command;
    INT32 mode;
    INT32 freq;                         //最小受信周期 msec      
    INT32 d[SWAY_SENSOR_N_CAM];         //カメラ-ターゲット間距離
}ST_SWAY_CLIENT_BODY, * LPST_SWAY_CLIENT_BODY;

typedef struct SwayComClientMsg {                           //制御PC送信メッセージ
    ST_SWAY_CLIENT_HEAD head;
    ST_SWAY_CLIENT_BODY body;
}ST_SWAY_CLIENT_MSG, * LPST_SWAY_CLIENT_MSG;

//送信データコード
#define SW_CLIENT_COM_ONCE_DATA        0x00000000
#define SW_CLIENT_COM_CONST_DATA       0x00000001
#define SW_CLIENT_COM_STOP_DATA        0x00000008
#define SW_CLIENT_COM_CAMERA1_0SET     0x00000010
#define SW_CLIENT_COM_CAMERA2_0SET     0x00000020
#define SW_CLIENT_COM_TILT1_0SET       0x00000040
#define SW_CLIENT_COM_TILT2_0SET       0x00000080
#define SW_CLIENT_COM_CAMERAR1_RESET   0x00000100
#define SW_CLIENT_COM_CAMERAR2_RESET   0x00000200
#define SW_CLIENT_COM_TILT1_RESET      0x00000400
#define SW_CLIENT_COM_TILT2_RESET      0x00000800
#define SW_CLIENT_COM_SAVE_IMG         0x00001000
#define SW_CLIENT_COM_PC_RESET         0x10000000

#define SW_SENSOR_DEFAULT_SCAN         100                 //100msec

#define SW_SENSOR_MODE_NORMAL          0x000000001          //通常運用モード
#define SW_SENSOR_MODE_CHECK1          0x000000002          //試験チェックモード 


//受信データコード
//エラー状態
#define SW_SENSOR_ERR_CODE_CAMERA      0x00000001   // 機器異常(カメラ)
#define SW_SENSOR_ERR_CODE_TILT        0x00000002   // 機器異常(傾斜計)
#define SW_SENSOR_ERR_CODE_DETECT_TG1  0x00000010   // 検出異常(ターゲット1)
#define SW_SENSOR_ERR_CODE_DETECT_TG2  0x00000020   // 検出異常(ターゲット2)

//検出状態
#define SW_SENSOR_STAT_TG1_DETECTED    0x00000001   // 振れ検出中(ターゲット1)
#define SW_SENSOR_STAT_TG2_DETECTED    0x00000002   // 振れ検出中(ターゲット2)
#define SW_SENSOR_STAT_INIT_SENSOR     0x00000010   // 準備中(カメラ)
#define SW_SENSOR_STAT_INIT_TILT       0x00000020   // 準備中(傾斜計)
#define SW_SENSOR_STAT_0SET_SENSOR     0x00000010   // 0点設定中(カメラ)
#define SW_SENSOR_STAT_0SET_TILT       0x00000020   // 0点設定中(傾斜計)

//メッセージテキスト
//エラー状態
#define SW_TXT_ERR_CODE_CAMERA      L"カメラ異常"
#define SW_TXT_ERR_CODE_TILT        L"傾斜計異常"
#define SW_TXT_ERR_CODE_DETECT_TG1  L"ターゲット１検出異常"
#define SW_TXT_ERR_CODE_DETECT_TG2  L"ターゲット２検出異常"

//検出状態
#define SW_TXT_STAT_TG1_DETECTED    L"ターゲット１検出中"
#define SW_TXT_STAT_TG2_DETECTED    L"ターゲット２検出中"
#define SW_TXT_STAT_INIT_SENSOR     L"センサ準備中"
#define SW_TXT_STAT_INIT_TILT       L"傾斜計準備中"
#define SW_TXT_STAT_0SET_SENSOR     L"センサ０セット中"
#define SW_TXT_STAT_0SET_TILT       L"傾斜計０セット中"

//=========================================================================== =
// define定義
#pragma region CONSTANT_DEFINITION
//----------------------------------------------------------------------------
// .iniファイル関連設定
//#define FOLDER_OF_INIFILE                               L"Data\\"                       // .iniファイルフォルダ
#define FOLDER_OF_INIFILE                               L"ini\\"                       // .iniファイルフォルダ
#define EXT_OF_INIFILE                                  L"ini"                          // .iniファイル拡張子

#define INI_SCT_COMMON                                  L"COMMON"                       // 共通設定
#define INI_KEY_COMMON_IMAGE_SOURCE_CAMERA              L"CMN_IMAGE_SOURCE_CAMERA"      // カメラ画像取込み
#define INI_KEY_COMMON_IMAGE_SOURCE_CAMERA_ITEM_NUM     1                               // 
#define INI_KEY_COMMON_IMAGE_SOURCE_FILENAME            L"CMN_IMAGE_SOURCE_FILENAME"    // 取込み画像ファイル名
#define INI_KEY_COMMON_IMAGE_SOURCE_FILENAME_ITEM_NUM   1                               // 
#define INI_KEY_COMMON_IMAGE_OUTPUT_FILENAME            L"CMN_IMAGE_OUTPUT_FILENAME"    // 画像保存ファイル名
#define INI_KEY_COMMON_IMAGE_OUTPUT_FILENAME_ITEM_NUM   1                               // 
#define INI_KEY_COMMON_IMAGE_SCREEN_LAYOUT              L"CMN_IMAGE_SCREEN_LAYOUT"      // 画像表示レイアウト
#define INI_KEY_COMMON_IMAGE_SCREEN_LAYOUT_ITEM_NUM     4                               // 

#define INI_SCT_SYSTEM                                  L"SYSTEM"                       // システム設定
#define INI_KEY_SYSTEM_CYCLE_TIME                       L"SYS_CYCLE_TIME"               // メインスレッド実行周期
#define INI_KEY_SYSTEM_CYCLE_TIME_ITEM_NUM              1                               // 
#define INI_KEY_SYSTEM_DEVICE_ID                        L"SYS_DEVICE_ID"                // 機器ID(半角英数字4文字)
#define INI_KEY_SYSTEM_DEVICE_ID_ITEM_NUM               2                               // 

#define INI_SCT_TASK                                    L"TASK"                         // タスク設定
#define INI_KEY_TASK_ENVIRONMENT                        L"TSK_ENVIRONMENT"              // Environment
#define INI_KEY_TASK_POLICY                             L"TSK_POLICY"                   // Policy
#define INI_KEY_TASK_AGENT                              L"TSK_AGENT"                    // Agent
#define INI_KEY_TASK_SCADA                              L"TSK_SCADA"                    // Scada
#define INI_KEY_TASK_ITEM_NUM                           3                               // 

#define INI_SCT_ETHERNET                                L"ETHERNET"                     // 通信設定
#define INI_KEY_ETHERNET_LOCAL                          L"NET_LOCAL"                    // Local設定
#define INI_KEY_ETHERNET_REMOTE                         L"NET_REMOTE"                   // Remote設定
#define INI_KEY_ETHERNET_ITEM_NUM                       2                               // 

#define INI_SCT_CAMERA                                  L"CAMERA"                       // カメラ設定
#define INI_KEY_CAMERA_BASIS_IP_ADDRESS                 L"CAM_BASIS_IP_ADDRESS"         // カメラのIPアドレス
#define INI_KEY_CAMERA_BASIS_IP_ADDRESS_ITEM_NUM        1                               // 
#define INI_KEY_CAMERA_BASIS_ROI                        L"CAM_BASIS_ROI"                // ROI(領域)
#define INI_KEY_CAMERA_BASIS_ROI_ITEM_NUM               4                               // 
#define INI_KEY_CAMERA_BASIS_PACKET_SIZE                L"CAM_BASIS_PACKET_SIZE"        // ドライバが受け取るパケットの最大サイズ
#define INI_KEY_CAMERA_BASIS_PACKET_SIZE_ITEM_NUM       1                               // 
#define INI_KEY_CAMERA_BASIS_FRAME_RATE                 L"CAM_BASIS_FRAME_RATE"         // フレームレート
#define INI_KEY_CAMERA_BASIS_FRAME_RATE_ITEM_NUM        1                               // 
#define INI_KEY_CAMERA_BASIS_BLACKLEVEL                 L"CAM_BASIS_BLACKLEVEL"         // 黒レベル設定
#define INI_KEY_CAMERA_BASIS_BLACKLEVEL_ITEM_NUM        1                               // 
#define INI_KEY_CAMERA_BASIS_GAMMA                      L"CAM_BASIS_GAMMA"              // ガンマ補正
#define INI_KEY_CAMERA_BASIS_GAMMA_ITEM_NUM             1                               // 
#define INI_KEY_CAMERA_BASIS_WBALANCE_AUTO              L"CAM_BASIS_WBALANCE_AUTO"      // ホワイトバランスゲイン設定
#define INI_KEY_CAMERA_BASIS_WBALANCE_AUTO_ITEM_NUM     3                               // 
#define INI_KEY_CAMERA_BASIS_VIEW_ANGLE                 L"CAM_BASIS_VIEW_ANGLE"         // カメラ視野角
#define INI_KEY_CAMERA_BASIS_VIEW_ANGLE_ITEM_NUM        2                               // 
#define INI_KEY_CAMERA_GAIN                             L"CAM_GAIN"                     // ゲイン設定
#define INI_KEY_CAMERA_GAIN_ITEM_NUM                    3                               // 
#define INI_KEY_CAMERA_GAIN_AUTO                        L"CAM_GAIN_AUTO"                // 自動ゲイン設定
#define INI_KEY_CAMERA_GAIN_AUTO_ITEM_NUM               2                               // 
#define INI_KEY_CAMERA_GAIN_AUTO_START                  L"CAM_GAIN_AUTO_START"          // 自動ゲイン判定輝度(開始)
#define INI_KEY_CAMERA_GAIN_AUTO_START_ITEM_NUM         2                               // 
#define INI_KEY_CAMERA_GAIN_AUTO_STOP                   L"CAM_GAIN_AUTO_STOP"           // 自動ゲイン判定輝度(停止)
#define INI_KEY_CAMERA_GAIN_AUTO_STOP_ITEM_NUM          2                               // 
#define INI_KEY_CAMERA_EXPSTIME                         L"CAM_EXPSTIME"                 // 露光時間設定
#define INI_KEY_CAMERA_EXPSTIME_ITEM_NUM                3                               // 
#define INI_KEY_CAMERA_EXPSTIME_AUTO                    L"CAM_EXPSTIME_AUTO"            // 自動露光設定
#define INI_KEY_CAMERA_EXPSTIME_AUTO_ITEM_NUM           2                               // 
#define INI_KEY_CAMERA_EXPSTIME_AUTO_START              L"CAM_EXPSTIME_AUTO_START"      // 自動露光判定輝度(開始)
#define INI_KEY_CAMERA_EXPSTIME_AUTO_START_ITEM_NUM     2                               // 
#define INI_KEY_CAMERA_EXPSTIME_AUTO_STOP               L"CAM_EXPSTIME_AUTO_STOP"       // 自動露光判定輝度(停止)
#define INI_KEY_CAMERA_EXPSTIME_AUTO_STOP_ITEM_NUM      2                               // 
#define INI_KEY_CAMERA_ERROR                            L"CAM_ERROR"                    // カメラの異常判定設定
#define INI_KEY_CAMERA_ERROR_ITEM_NUM                   2                               // 
#define INI_KEY_CAMERA_BOOT                             L"CAM_BOOT"                     // カメラの起動設定
#define INI_KEY_CAMERA_BOOT_ITEM_NUM                    3                               // 

#define INI_SCT_TILTMETER                               L"TILTMETER"                    // 傾斜計設定
#define INI_KEY_TILTMETER_CANOPEN                       L"TLT_CANOPEN"                  // CANope設定
#define INI_KEY_TILTMETER_CANOPEN_ITEM_NUM              3                               // 
#define INI_KEY_TILTMETER_FILTER                        L"TLT_FILTER"                   // 傾斜計フィルター
#define INI_KEY_TILTMETER_FILTER_ITEM_NUM               2                               // 
#define INI_KEY_TILTMETER_SPEED_TC                      L"TLT_SPEED_TC"                 // 傾斜角速度フィルター時定数
#define INI_KEY_TILTMETER_SPEED_TC_ITEM_NUM             1                               // 

#define INI_SCT_MOUNTING                                L"MOUNTING"                     // 取付寸法設定
#define INI_KEY_MOUNTING_OFFSET_L0                      L"MNT_OFFSET_L0"                // カメラ回転軸距離L0[mm]
#define INI_KEY_MOUNTING_OFFSET_T0                      L"MNT_OFFSET_T0"                // カメラ取付角度θ0(*10^6)[rad]
#define INI_KEY_MOUNTING_OFFSET_P0                      L"MNT_OFFSET_P0"                // カメラ設置角度φ0(*10^6)[rad]
#define INI_KEY_MOUNTING_OFFSET_ITEM_NUM                2                               // 

#define INI_SCT_IMGPROC                                 L"IMGPROC"                      // 画像処理条件設定
#define INI_KEY_IMGPROC_ROI                             L"IMG_ROI"                      // 画像ROI設定
#define INI_KEY_IMGPROC_ROI_ITEM_NUM                    2                               // 
#define INI_KEY_IMGPROC_MASK                            L"IMG_MASK"                     // マスク画像選択
#define INI_KEY_IMGPROC_MASK_ITEM_NUM                   1                               // 
#define INI_KEY_IMGPROC_MASK1_H                         L"IMG_MASK1_H"                  // 画像1マスクH
#define INI_KEY_IMGPROC_MASK1_S                         L"IMG_MASK1_S"                  // 画像1マスクS
#define INI_KEY_IMGPROC_MASK1_V                         L"IMG_MASK1_V"                  // 画像1マスクV
#define INI_KEY_IMGPROC_MASK1_ITEM_NUM                  2                               // 
#define INI_KEY_IMGPROC_MASK2_H                         L"IMG_MASK2_H"                  // 画像2マスクH
#define INI_KEY_IMGPROC_MASK2_S                         L"IMG_MASK2_S"                  // 画像2マスクS
#define INI_KEY_IMGPROC_MASK2_V                         L"IMG_MASK2_V"                  // 画像2マスクV
#define INI_KEY_IMGPROC_MASK2_ITEM_NUM                  2                               // 
#define INI_KEY_IMGPROC_FILTER1                         L"IMG_FILTER1"                  // フィルター1(ゴマ塩ノイズ)
#define INI_KEY_IMGPROC_FILTER1_ITEM_NUM                2                               // 
#define INI_KEY_IMGPROC_FILTER2                         L"IMG_FILTER2"                  // フィルター2(穴埋めノイズ)
#define INI_KEY_IMGPROC_FILTER2_ITEM_NUM                2                               // 
#define INI_KEY_IMGPROC_PROC                            L"IMG_PROC"                     // ターゲット検出アルゴリズム
#define INI_KEY_IMGPROC_PROC_ITEM_NUM                   1                               // 
#define INI_KEY_IMGPROC_SWAY_SPEED_TC                   L"IMG_SWAY_SPEED_TC"            // 振れ速度フィルター時定数
#define INI_KEY_IMGPROC_SWAY_SPEED_TC_ITEM_NUM          1                               // 
#define INI_KEY_IMGPROC_SWAY_ZERO_SET                   L"IMG_SWAY_ZERO_SET"            // 振れゼロ点設定
#define INI_KEY_IMGPROC_SWAY_ZERO_SET_ITEM_NUM          1                               // 

//----------------------------------------------------------------------------
// 
// H, S, V
#define IMAGE_HSV_H_MIN                     0           // H最小値
#define IMAGE_HSV_H_MAX                     179         // H最大値
#define IMAGE_HSV_S_MIN                     0           // S最小値
#define IMAGE_HSV_S_MAX                     255         // S最大値
#define IMAGE_HSV_V_MIN                     0           // V最小値
#define IMAGE_HSV_V_MAX                     255         // V最大値

// 調整
#define EXTN_TARGETDIST_MIN                 1.0       // ターゲット距離(最小)
#define EXTN_TARGETDIST_MAX                 100.0     // ターゲット距離(最大)

// シャッタコントロールモード
#define	EXPOSURE_CONTROL_HOLD               0           // シャッタコントロール:Hold
#define	EXPOSURE_CONTROL_LIGHT              1           // シャッタコントロール:明るく
#define	EXPOSURE_CONTROL_DARKEN             -1          // シャッタコントロール:暗く

#define	EXPOSURE_CONTROL_ROI_MODE           0           // シャッタコントロール:ROI範囲
#define	EXPOSURE_CONTROL_ROI_KEEP           0x800       // シャッタコントロール:ROI保持
#define	EXPOSURE_CONTROL_RESET_STEP         0x100       // シャッタコントロール:初期化STEP 初期値セット
#define	EXPOSURE_CONTROL_RESET_STEP_FIN     0x11F       // シャッタコントロール:初期化STEP 完了待機

#define	EXPOSURE_CONTROL_RESET_WAIT_STEP    0x01        // シャッタコントロール:初期化STEP 完了待機

#define	EXPOSURE_CONTROL_STEP_COUNT        10          // シャッタコントロール:初期化STEP待機カウント


//----------------------------------------------------------------------------
// 
#define PI              3.14159     // π
#define CONV_DEG_RAD    (PI/180.0)  // deg→rad
#define CONV_MSEC_SEC   0.001       // msec→sec


//---------------------------------------------------------------------------------
#define SWAY_SENSOR_ROI_MIN_W       150 //ROI最小幅
#define SWAY_SENSOR_ROI_MIN_H       150 //ROI最小高さ

#define SWAY_SENSOR_RANGE_OVER_COUNT  10 //ROI最小高さ

//振れ用
#define SWAY_SENSOR_SAMPLE_TIME     0.05 //50msec
#define SWAY_SENSOR_LPF_T           0.1  //振れセンサLPFカットオフ周期   
#define SWAY_SENSOR_LPF_K1          0.6666  //T/(T+scan)
#define SWAY_SENSOR_LPF_K2          0.3333  //scan/(T+scan)

//傾斜計用
#define SWAY_SENSOR_LPF_T2           0.5  //振れセンサLPFカットオフ周期   
#define SWAY_SENSOR_LPF_K3          0.1818  //T/(T+scan)
#define SWAY_SENSOR_LPF_K4          0.0909  //scan/(T+scan)

#pragma endregion CONSTANT_DEFINITION

//============================================================================
// enum定義
#pragma region ENUM_DEFINITION
// 傾斜計状態
enum class ENUM_TILT_STATUS : uint32_t {
    DEFAULT = 0x00000000,  // デフォルト
    CANOPEN_STARTED = 0x00000001,  // CANopen起動
    ERR_CANOPEN_LIB = 0x00000010,  // CANopen Libエラー
    ERR_CANOPEN_READ = 0x00000020   // データ読み込みエラー
};

// 画像処理状態
enum class ENUM_PROCCESS_STATUS : uint32_t {
    DEFAULT             = 0x00000000,   // デフォルト
    IMAGE_ENABLE        = 0x00000001,   // 画像データ有効
    TARGET_ENABLE       = 0x00000002,   // ターゲット検出有効
    ZERO_SET_RUN        = 0x00000010,   // 0点設定中(カメラ)
    ZERO_SET_MCC_RUN    = 0x00000020,   // 0点設定中(カメラ)(MCC)
    ZERO_SET_COMPLETED  = 0x00000100    // 0点設定完了(カメラ)
};

// システム状態
enum class ENUM_SYSTEM_STATUS : uint32_t {
    DEFAULT = 0x00000000,    // デフォルト
    CAMERA_RESET_REQ = 0x00000010,    // カメラ再接続要求
    CAMERA_RESET_REQ_MCC = 0x00000020,    // カメラ再接続要求(MCC)
    CAMERA_RESET_RUN = 0x00000040,    // カメラ再接続中
    CAMERA_RESET_COMPLETED = 0x00000080,    // カメラ再接続完了
    TILT_PRESET_REQ = 0x00000100,    // 傾斜計プリセット要求
    TILT_PRESET_REQ_MCC = 0x00000200,    // 傾斜計プリセット要求(MCC)
    TILT_PRESET_RUN = 0x00000400,    // 傾斜計プリセット実行中
    TILT_PRESET_COMPLETED = 0x00000800     // 傾斜計プリセット完了
};

// カメラ画像取込み
enum class ENUM_GRAB_IMAGE : uint32_t {
    GRAB_FILE = 0,  // 画像取込み(ファイル)
    GRAB_CAMERA,    // 画像取込み(カメラ)
    E_MAX
};

// 振れセンサー
enum class ENUM_SWAY_SENSOR : uint32_t {
    SENSOR_1 = 0,   // 振れセンサー1
    SENSOR_2,       // 振れセンサー2
    E_MAX
};

// ターゲット検出
enum class ENUM_TARGET : uint32_t {
    TARGET_1 = 0,   // ターゲット検出1
    TARGET_2,       // ターゲット検出2
    E_MAX
};

// HSV色空間
enum class ENUM_HSV_MODEL : uint32_t {
    H = 0,  // 色相(Hue)
    S,      // 彩度(Saturation)
    V,      // 明度(Value)
    E_MAX
};

// カメラ画像配列 INDEX
enum class ENUM_IMAGE : uint32_t {
    SOURCE = 0, // 元画像
    MASK_1,     // マスク画像1
    MASK_2,     // マスク画像2
    PROCESS,    // 加工画像
    E_MAX
};

// 画像処理配列 INDEX
enum class ENUM_IMAGE_MASK : uint32_t {
    MASK_1 = 0, // マスク画像1
    MASK_2,     // マスク画像2
    E_MAX
};

// マスク画像選択
enum class ENUM_IMAGE_MASK_SEL : uint32_t {
    ALL = 0,    // マスク画像選択(両方)
    IMAGE_1,    // マスク画像選択(画像1のみ)
    IMAGE_2,    // マスク画像選択(画像2のみ)
    E_MAX
};

// ノイズフィルター
enum class ENUM_NOISE_FILTER : uint32_t {
    FILTER_1 = 0,   // フィルター1
    FILTER_2,       // フィルター2
    E_MAX
};

// ノイズフィルター1
enum class ENUM_NOISE_FILTER1 : uint32_t {
    NONE = 0,   // なし
    MEDIAN,     // 中央値フィルター
    OPENNING,   // オープニング処理
    E_MAX
};

// ノイズフィルター2
enum class ENUM_NOISE_FILTER2 : uint32_t {
    NONE = 0,   // なし
    CLOSING,    // クロージング処理
    MEDIAN,
    E_MAX
};

// 重心位置算出アルゴリズム
enum class ENUM_COG : uint32_t {
    MAX_CONTOUR_AREA = 0,   // 最大輪郭面積
    MAX_CONTOUR_LENGTH,     // 最大輪郭長
    BOUND_RECT,             // 直立バウンディングボックス
    E_MAX
};

// 軸
enum class ENUM_AXIS : uint32_t {
    X = 0,  // X軸
    Y,      // Y軸
    E_MAX
};
#pragma endregion ENUM_DEFINITION

//============================================================================
// 構造体/共用体定義
#pragma region STRUCTURE_DEFINITION
//----------------------------------------------------------------------------
// アプリケーション設定の構造体
// 共通設定
// 画像表示レイアウト設定
typedef struct TAG_CONFIG_COMMON_IMAGE_SCREEN_LAYOUT {
    uint32_t x0;        // 原点座標X
    uint32_t y0;        // 原点座標Y
    uint32_t width;     // 横幅サイズ
    uint32_t height;    // 高さサイズ

    TAG_CONFIG_COMMON_IMAGE_SCREEN_LAYOUT()
        : x0(0)
        , y0(0)
        , width(0)
        , height(0)
    {}
} CONFIG_COMMON_IMAGE_SCREEN_LAYOUT, * PCONFIG_COMMON_IMAGE_SCREEN_LAYOUT;
// 共通設定
typedef struct TAG_CONFIG_COMMON {
    uint32_t     img_source_camera; // カメラ画像取込み(カメラ[1]/画像ファイル[0])
    std::wstring img_source_fname;  // 取込み画像ファイル名(CMN_IMAGE_SOURCE_CAMERA=0のときの画像)
    std::wstring img_output_fname;  // 画像保存ファイル名

    CONFIG_COMMON_IMAGE_SCREEN_LAYOUT img_screen_layout;    // 画像表示レイアウト設定

    TAG_CONFIG_COMMON()
        : img_source_camera(0)
        , img_source_fname(L"")
        , img_output_fname(L"")
    {}
} CONFIG_COMMON, * PCONFIG_COMMON;

// システム設定
typedef struct TAG_CONFIG_SYSTEM {
    uint32_t     cycletime_sys; // メインスレッド実行周期[msec]
    std::wstring mcc_id;        // 機器ID(制御PC)[半角英数字4文字]
    std::wstring my_id;         // 機器ID(振れセンサーPC)[半角英数字4文字]

    TAG_CONFIG_SYSTEM()
        : cycletime_sys(0)
        , mcc_id(L"")
        , my_id(L"")
    {}
} CONFIG_SYSTEM, * PCONFIG_SYSTEM;

// タスク設定
// タスク設定(データ)
typedef struct TAG_CONFIG_TASK_DATA {
    uint32_t     cycletime; // メインスレッド実行周期[msec]
    std::wstring name;      // タスク名
    std::wstring sname;     // タスク略称

    TAG_CONFIG_TASK_DATA()
        : cycletime(0)
        , name(L"")
        , sname(L"")
    {}
} CONFIG_TASK_DATA, * PCONFIG_TASK_DATA;
// タスク設定
typedef struct TAG_CONFIG_TASK {
    CONFIG_TASK_DATA environment;   // Environment
    CONFIG_TASK_DATA policy;        // Policy
    CONFIG_TASK_DATA agent;         // Agent
    CONFIG_TASK_DATA scada;         // Scada
} CONFIG_TASK, * PCONFIG_TASK;

// 通信設定
// 通信設定(データ)
typedef struct TAG_CONFIG_ETHERNET_DATA {
    std::wstring ipaddress; // IPアドレス
    unsigned int port;      // ポート番号

    TAG_CONFIG_ETHERNET_DATA()
        : ipaddress(L"")
        , port(0)
    {}
} CONFIG_ETHERNET_DATA, * PCONFIG_ETHERNET_DATA;
// 通信設定
typedef struct TAG_CONFIG_SYS_ETHERNET {
    TAG_CONFIG_ETHERNET_DATA local;     // Local設定
    TAG_CONFIG_ETHERNET_DATA remote;    // Remote設定
} CONFIG_ETHERNET, * PCONFIG_ETHERNET;
// カメラ設定
// ROI設定
typedef struct TAG_CONFIG_CAMERA_ROI {
    uint32_t offset;    // ROI(領域)の開始位置(X, Y)
    uint32_t size;      // ROI(領域)のサイズ(X(32の倍数, 2592以下), Y(2の倍数, 2048以下))

    TAG_CONFIG_CAMERA_ROI()
        : offset(0)
        , size(0)
    {}
} CONFIG_CAMERA_ROI, * PCONFIG_CAMERA_ROI;
// ホワイトバランスゲイン設定
typedef struct TAG_CONFIG_CAMERA_BALANCE_WHITE {
    unsigned int wb_auto;       // ホワイトバランスゲイン自動調整モー設定(自動調整OFF[1]/連続自動調整[2]/一回のみ自動調整[3])
    double       wb_ratio_red;  // ホワイトバランスゲイン設定(Red Gain)
    double       wb_ratio_blue; // ホワイトバランスゲイン設定(Blue Gain)

    TAG_CONFIG_CAMERA_BALANCE_WHITE()
        : wb_auto(0)
        , wb_ratio_red(0.0)
        , wb_ratio_blue(0.0)
    {}
} CONFIG_CAMERA_BALANCE_WHITE, * PCONFIG_CAMERA_BALANCE_WHITE;
// カメラの基本設定
typedef struct TAG_CONFIG_CAMERA_BASIS {
    std::wstring                ipaddress;  // IPアドレス
    CONFIG_CAMERA_ROI           roi[static_cast<uint32_t>(ENUM_AXIS::E_MAX)];   // ROI(領域)
    uint32_t                    packetsize; // ドライバが受け取るパケットの最大サイズ[byte]
    double                      framerate;  // フレームレート[fps]
    double                      blacklevel; // 黒レベル設定
    double                      gamma;      // ガンマ補正
    CONFIG_CAMERA_BALANCE_WHITE wb;         // ホワイトバランスゲイン設定
    double                      viewangle[static_cast<uint32_t>(ENUM_AXIS::E_MAX)]; // 視野角[deg]

    TAG_CONFIG_CAMERA_BASIS()
        : ipaddress(L"")
        , packetsize(0)
        , framerate(0.0)
        , blacklevel(0.0)
        , gamma(0.0)
        , viewangle{ 0.0, 0.0 }
    {}
} CONFIG_CAMERA_BASIS, * PCONFIG_CAMERA_BASIS;
// 輝度コントロール設定
typedef struct TAG_CONFIG_CAMERA_BRIGHTNESS_CONTROL {
    double val;     // 設定値
    double val_min; // 最小値
    double val_max; // 最大値

    BOOL   auto_control;    // 自動コントロール(自動[TRUE]/固定[FALSE])
    double auto_rate;       // 自動コントロール変化量
    double auto_start_h;    // 自動コントロール判定輝度(開始上限:この値より輝度が高い場合、暗くする)   [0 - 255]
    double auto_start_l;    // 自動コントロール判定輝度(開始下限:この値より輝度が高い場合、明るくする) [0 - 255]
    double auto_stop_h;     // 自動コントロール判定輝度(停止上限:この値より輝度が低い場合、停止する)   [0 - 255]
    double auto_stop_l;     // 自動コントロール判定輝度(停止下限:この値より輝度が高い場合、停止する)   [0 - 255]

    TAG_CONFIG_CAMERA_BRIGHTNESS_CONTROL()
        : val(0.0)
        , val_min(0.0)
        , val_max(0.0)
        , auto_control(FALSE)
        , auto_rate(0.0)
        , auto_start_h(0.0)
        , auto_start_l(0.0)
        , auto_stop_h(0.0)
        , auto_stop_l(0.0)
    {}
} CONFIG_CAMERA_BRIGHTNESS_CONTROL, * PCONFIG_CAMERA_BRIGHTNESS_CONTROL;
// カメラの異常判定設定
typedef struct TAG_CONFIG_CAMERA_ERROR {
    double   framedrop; // フレームレート低下の判定値[fps]
    uint32_t errtime;   // 異常判定時間[msec]

    TAG_CONFIG_CAMERA_ERROR()
        : framedrop(0.0)
        , errtime(0)
    {}
} CONFIG_CAMERA_ERROR, * PCONFIG_CAMERA_ERROR;
// カメラの起動設定
typedef struct TAG_CONFIG_CAMERA_BOOT {
    uint32_t bootwaittimer;     // システム起動後のカメラ起動待ち時間[msec]
    uint32_t rebootcount;       // システム起動後のカメラ起動失敗時にカメラリを再起動する回数[count]
    uint32_t rebootingtimer;    // カメラ再起動中であることを通知する時間[msec]

    TAG_CONFIG_CAMERA_BOOT()
        : bootwaittimer(0)
        , rebootcount(0)
        , rebootingtimer(0)
    {}
} CONFIG_CAMERA_BOOT, * PCONFIG_CAMERA_BOOT;
// カメラ設定
typedef struct TAG_CONFIG_CAMERA {
    CONFIG_CAMERA_BASIS              basis;     // カメラの基本設定
    CONFIG_CAMERA_BRIGHTNESS_CONTROL gain;      // 輝度コントロール設定(ゲイン)
    CONFIG_CAMERA_BRIGHTNESS_CONTROL expstime;  // 輝度コントロール設定(露光時間)
    CONFIG_CAMERA_ERROR              error;     // カメラの異常判定設定
    CONFIG_CAMERA_BOOT               boot;      // カメラの起動設定
} CONFIG_CAMERA, * PCONFIG_CAMERA;

// 傾斜計設定
// CANopen設定
typedef struct TAG_CONFIG_CANOPEN {
    std::wstring setup_fname;   // CANopen set-up .ini file name
    uint8_t      port;          // CANopen port (port1(=0) or port2(=1))
    uint8_t      node_id;       // CANopen node-ID

    TAG_CONFIG_CANOPEN()
        : setup_fname(L"")
        , port(0)
        , node_id(0x00)
    {}
} CONFIG_CANOPEN, * PCONFIG_CANOPEN;
// 傾斜計設定
typedef struct TAG_CONFIG_TILTMETER {
    CONFIG_CANOPEN canopen;         // CANopen設定
    uint16_t       dr_filter;       // 傾斜計フィルター(再帰形ディジタルフィルター)
    uint16_t       ma_filter;       // 傾斜計フィルター(移動平均フィルター)
    double         tiltspdfilter;   // 傾斜角速度フィルター時定数

    TAG_CONFIG_TILTMETER()
        : dr_filter(0)
        , ma_filter(0)
        , tiltspdfilter(0.0)
    {}
} CONFIG_TILTMETER, * PCONFIG_TILTMETER;

// 構造定義設定
// 取付オフセット設定
typedef struct TAG_CONFIG_MOUNTING_OFFSET {
    double l0;  // カメラ回転軸距離L0[mm]
    double t0;  // カメラ取付角度θ0(*10^6)[rad]
    double p0;  // カメラ設置角度φ0(*10^6)[rad]

    TAG_CONFIG_MOUNTING_OFFSET()
        : l0(0.0)
        , t0(0.0)
        , p0(0.0)
    {}
} CONFIG_MOUNTING_OFFSET, * PCONFIG_MOUNTING_OFFSET;
// 構造定義設定
typedef struct TAG_CONFIG_MOUNTING {
    CONFIG_MOUNTING_OFFSET offset[static_cast<uint32_t>(ENUM_AXIS::E_MAX)]; // 取付オフセット設定
} CONFIG_MOUNTING, * PCONFIG_MOUNTING;

// 画像処理条件設定
// ROI設定
typedef struct TAG_CONFIG_IMGPROC_ROI {
    BOOL   valid;   // ROI有効設定(無効[FALSE]/有効[TRUE])
    double scale;   // ROIスケール(検出したターゲットに対する倍率)

    TAG_CONFIG_IMGPROC_ROI()
        : valid(FALSE)
        , scale(0.0)
    {}
} CONFIG_IMGPROC_ROI, * PCONFIG_IMGPROC_ROI;
// ノイズフィルター
typedef struct TAG_CONFIG_IMGPROC_FILTER {
    uint32_t type;  // フィルター種類
    uint32_t val;   // フィルター値

    TAG_CONFIG_IMGPROC_FILTER()
        : type(0)
        , val(0)
    {}
} CONFIG_IMGPROC_FILTER, * PCONFIG_IMGPROC_FILTER;
// マスク設定
typedef struct TAG_CONFIG_IMGPROC_MASK {
    BOOL     valid;                                                 // マスク選択(無効[FALSE]/有効[TRUE])
    uint32_t hsv_l[static_cast<uint32_t>(ENUM_HSV_MODEL::E_MAX)];   // HSVマスク判定値(下限)
    uint32_t hsv_u[static_cast<uint32_t>(ENUM_HSV_MODEL::E_MAX)];   // HSVマスク判定値(上限)

    TAG_CONFIG_IMGPROC_MASK()
        : valid(FALSE)
        , hsv_l{ 0,0,0 }
        , hsv_u{ 0,0,0 }
    {}
} CONFIG_IMGPROC_MASK, * PCONFIG_IMGPROC_MASK;
// 画像処理条件設定
typedef struct TAG_CONFIG_IMGPROC {
    CONFIG_IMGPROC_ROI    roi;                                                      // ROI設定
    CONFIG_IMGPROC_MASK   mask[static_cast<uint32_t>(ENUM_IMAGE_MASK::E_MAX)];      // マスク設定
    CONFIG_IMGPROC_FILTER filter[static_cast<uint32_t>(ENUM_NOISE_FILTER::E_MAX)];  // ノイズフィルター(0=ゴマ塩, 1=穴埋め)
    uint32_t              imgprc;                                                   // ターゲット検出アルゴリズム(0=最大輪郭面積, 1=最大輪郭長)
    double                swayspdfilter;                                            // 振れ速度フィルター時定数
    uint32_t              swayzeroset_time;                                         // 振れゼロ点設定(計測時間[ms])
    double                PIXperRAD;                                                // 角度分解能

    TAG_CONFIG_IMGPROC()
        : imgprc(0)
        , swayspdfilter(0.0)
        , swayzeroset_time(0)
        , PIXperRAD(2773.3)    
    {}
} CONFIG_IMGPROC, * PCONFIG_IMGPROC;

// ***アプリケーション設定
typedef struct TAG_APP_CONFIG {
    CONFIG_COMMON    common;        // 共通設定
    CONFIG_SYSTEM    system;        // システム設定
    CONFIG_TASK      task;          // タスク設定
    CONFIG_ETHERNET  ethernet;      // 通信設定
    CONFIG_CAMERA    camera;        // カメラ設定
    CONFIG_TILTMETER tiltmeter;     // 傾斜計設定
    CONFIG_MOUNTING  mounting;      // 取付寸法設定
    CONFIG_IMGPROC   imageprocess;  // 画像処理条件設定
} APP_CONFIG, * PAPP_CONFIG;

//----------------------------------------------------------------------------
// アプリケーション情報の構造体
//----------------------------------------------------------------------------

// 傾斜計情報
// 傾斜計データ
typedef struct TAG_TILTMT_DATA {
    double deg;         // 傾斜角[deg]
    double rad;         // 傾斜角[rad]
    double speed_deg;   // 傾斜角速度[deg/s]
    double speed_rad;   // 傾斜角速度[rad/s]

    TAG_TILTMT_DATA()
        : deg(0.0)
        , rad(0.0)
        , speed_deg(0.0)
        , speed_rad(0.0)
    {}
} TILT_DATA, * PTILT_DATA;
// 傾斜計情報データ
typedef struct TAG_INFO_TILTMT_DATA {
    int32_t   status;                                           // ステータス情報
    TILT_DATA tilt[static_cast<uint32_t>(ENUM_AXIS::E_MAX)];    // 傾斜計データ

    TAG_INFO_TILTMT_DATA()
        : status(static_cast<uint32_t>(ENUM_TILT_STATUS::DEFAULT))
    {}
} INFO_TILTMT_DATA, * PINFO_TILTMT_DATA;

// 傾斜計情報
typedef struct TAG_INFO_TILTMT {
    CRITICAL_SECTION cs;
    INFO_TILTMT_DATA data;  // 傾斜計情報データ
} INFO_TILTMT, * PINFO_TILTMT;

// クライアント情報
// クライアント情報データ
typedef struct TAG_INFO_CLIENT_DATA {
    char         device_id[4];                                              // 機器ID
    uint32_t     command;                                                   // コマンド
    uint32_t     mode;                                                      // モード
    uint32_t     cycle;                                                     // 最小受信周期[ms]
    double       tgt_len[static_cast<uint32_t>(ENUM_SWAY_SENSOR::E_MAX)];   // ターゲット距離[m]
    uint32_t     scan_agent;                                                // AGENTのスキャンタイム（送信タイミング計算用

    TAG_INFO_CLIENT_DATA()
        : device_id{ '0','0', '0', '0' }
        , command(0)
        , mode(0)
        , cycle(0)
        , tgt_len{ EXTN_TARGETDIST_MIN,EXTN_TARGETDIST_MIN }
        , scan_agent(50)
    {}
} INFO_CLIENT_DATA, * PINFO_CLIENT_DATA;
// クライアント情報
typedef struct TAG_INFO_CLIENT {
    CRITICAL_SECTION cs;
    INFO_CLIENT_DATA data;  // クライアント情報データ
} INFO_CLIENT, * PINFO_CLIENT;

// 調整情報
// 調整情報データ
typedef struct TAG_INFO_ADJUST_DATA {
    BOOL   target_distance_fixed;   // 吊点～ターゲット間距離固定
    double target_distance;         // 吊点～ターゲット間距離[mm]
    int32_t     T_ms;               // 振れ周期ms

    TAG_INFO_ADJUST_DATA()
        : target_distance_fixed(FALSE)
        , target_distance(EXTN_TARGETDIST_MIN)
        , T_ms(10000)
    {}
} INFO_ADJUST_DATA, * PINFO_ADJUST_DATA;
// 調整情報
typedef struct _stExtnInfo {
    CRITICAL_SECTION cs;
    INFO_ADJUST_DATA data;  // 調整情報データ
} INFO_ADJUST, * PINFO_ADJUST;

#pragma endregion STRUCTURE_DEFINITION

