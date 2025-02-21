#pragma once
#include <windows.h>

//#define _DVELOPMENT_MODE                //開発環境を有効にする

///# 物理定数、変換係数
#pragma region PHISIC
#define GA				9.80665     //重力加速度

#define PI360           6.2832      //2π
#define PI330           5.7596   
#define PI315           5.4978
#define PI270           4.7124
#define PI180           3.1416      //π
#define PI165           2.8798
#define PI150           2.6180
#define PI135           2.3562
#define PI120           2.0944
#define PI90            1.5708     //π/2
#define PI60            1.0472
#define PI45            0.7854
#define PI30            0.5236
#define PI15            0.2618
#define PI10            0.17453
#define PI5             0.0873
#define PI0             0.00000

#define RAD1DEG         0.017453    //rad/deg
#define DEG1RAD         57.29578    //deg/rad

#define RAD1DEG_SQ      3282.6      //(rad/deg)^2
#define DEG1RAD_SQ      0.00030462  //(deg/rad)^2

#define PERCENT_NORM	1.0         //正規化％ 100% = 1.0

#pragma endregion 物理定数＿係数

///# コード
#pragma region STATUS
#define ID_UP           0   //上側
#define ID_DOWN         1   //下側
#define ID_FWD          0   //前進
#define ID_REV          1   //後進
#define ID_RIGHT        0   //右側
#define ID_LEFT         1   //左側
#define ID_ACC          0   //加速
#define ID_DEC          1   //減速

#define ID_STOP         -1  //停止
#define ID_SELECT       2   //選択

#define ID_X           0   // X方向
#define ID_Y           1   // Y方向
#define ID_Z           2   // Z方向

#define ID_T           0   // 接線方向
#define ID_R		   1   // 半径方向

#define NOTCH_0	        0
#define NOTCH_1	        1
#define NOTCH_2	        2
#define NOTCH_3	        3
#define NOTCH_4	        4
#define NOTCH_5	        5

#pragma endregion ステータスコード

#pragma region TYPE
#define UNIT_CODE_M             0   //単位のコード m
#define UNIT_CODE_RAD           1   //単位のコード rad
#define UNIT_CODE_MM            2   //単位のコード mm
#define UNIT_CODE_DEG           3   //単位のコード deg

#define CRANE_TYPE_JC0          0   //ジブクレーン 主巻のみ
#define CRANE_TYPE_JC1          1   //ジブクレーン 補巻あり
#define CRANE_TYPE_GC           2   //ゴライアスクレーン

//PC TYPE CODE
#define ID_PC_TYPE_DEFAULT      0
#define ID_PC_TYPE_MAIN_CONTROL 1
#define ID_PC_TYPE_SENSOR       2
#define ID_PC_TYPE_OTE          3
#define ID_PC_TYPE_PLC          4

//OTE TYPE CODE
#define OTE_TYPE_MONITOR        0   //モニター専用（停止操作のみ有効）
#define OTE_TYPE_MOBILE         1   //モバイル端末
#define OTE_TYPE_REMOTE_ROOM    2   //遠隔操作室
#define OTE_TYPE_REMOTE_SITE    3   //遠隔サイト

#pragma endregion 種別

#pragma region ID

#define CRANE_ID_DEVELOP    0       //開発
#define CRANE_ID_H6R602     6020    //みらい
#define CARNE_ID_HHGH29     29      //今造西多度津70t　102号  
#define CARNE_ID_HHGQ18     18      //今造西多度津300t 10号
#define CARNE_ID_HHFR22     22      //今造丸亀1200t 1号

#pragma endregion 識別ID

#pragma region RESPONCE
#define L_ON            0x01    // ON
#define L_OFF           0x00    // OFF
#define STAT_OK         L_ON   // ON
#define STAT_NG         L_OFF  // OFF

#define CODE_ACCEPTED      1        //受付完了
#define CODE_REJECTED      -1       //受付不可
#define CODE_NO_REQUEST    0        //要求無し
#define CODE_EXIST         1       //有り
#define CODE_NA            0       //特になしee
#define CODE_OK            1       //有り
#define CODE_NG            -1      //NG
#pragma endregion 応答

#pragma region MESSAGE
// タスクスレッドからの要求イベント
//wp:要求イベントのハンドル要求種別　lp:パラメータ
#define WM_USER_TASK_REQ            0x0405          // wp LOW:タスクID　HIGH:コマンドコード　lp パラメータD    
#define WM_USER_WPH_OPEN_IF_WND      0x0001         // wp　code IFウィンドウOPEN   lp ウィンドウのID
#define WM_USER_WPH_CLOSE_IF_WND     0x0002         // wp　code IFウィンドウCLOSE   lp ウィンドウのID

#pragma endregion ウィンドウメッセージ

///# マクロ
#pragma region MACRO_DEF

#ifndef MIN
#  define MIN(a,b)  ((a) > (b) ? (b) : (a))
#endif

#ifndef MAX
#  define MAX(a,b)  ((a) < (b) ? (b) : (a))
#endif

#ifndef iABS
#  define iABS(a)  (a < 0 ? -a : a)
#endif

#ifndef fABS
#  define fABS(a)  (a < 0.0 ? -a : a)
#endif

#ifndef dABS
#  define dABS(a)  (a < 0.0 ? -a : a)
#endif

#pragma endregion マクロ

///# 配列参照用　動作インデックス
#pragma region ARR_MOTION

#define MOTION_ID_MAX   8  //制御軸最大数

#define ID_HOIST        0   //巻 　       ID
#define ID_GANTRY       1   //走行        ID
#define ID_TROLLY       2   //横行        ID
#define ID_BHMH         2   //引込主巻    ID
#define ID_BOOM_H       3   //引込        ID
#define ID_SLEW         4   //旋回        ID
#define ID_AHOIST       5   //補巻　      ID
#define ID_OP_ROOM      6   //運転室移動　ID
#define ID_H_ASSY       7   //吊具等      ID

#define BIT_SEL_HST         0x00000001
#define BIT_SEL_GNT         0x00000002
#define BIT_SEL_TRY         0x00000004
#define BIT_SEL_BHMH        0x00000004
#define BIT_SEL_BH          0x00000008
#define BIT_SEL_SLW         0x00000010
#define BIT_SEL_AH          0x00000020  //JC
#define BIT_SEL_OPR         0x00000040
#define BIT_SEL_ASSY        0x00000080
#define BIT_SEL_ALL_0NOTCH  0x10000000
#define BIT_SEL_MOTION      BIT_SEL_HST|BIT_SEL_GNT|BIT_SEL_BH|BIT_SEL_SLW|BIT_SEL_AH
#define BIT_SEL_STATUS      0xFFFF0000

#pragma endregion 動作インデックス

///#ビット定義 
#pragma region BIT_DEF

#define BITS_WORD   0xFFFF //WORDデータ
#define BIT0        0x0001
#define BIT1        0x0002
#define BIT2        0x0004
#define BIT3        0x0008
#define BIT4        0x0010
#define BIT5        0x0020
#define BIT6        0x0040
#define BIT7        0x0080
#define BIT8        0x0100
#define BIT9        0x0200
#define BIT10       0x0400
#define BIT11       0x0800
#define BIT12       0x1000
#define BIT13       0x2000
#define BIT14       0x4000
#define BIT15       0x8000

#pragma endregion ビット定義

//タスク異常フラグ
#pragma region ERR_DEF

#define SMEM_NG_OTE_INF			0x0000000000010000
#define SMEM_NG_CRANE_STAT	    0x0000000000020000
#define SMEM_NG_CS_INF			0x0000000000040000
#define SMEM_NG_PLC_IO			0x0000000000080000
#define SMEM_NG_AGENT_INF		0x0000000000100000
#define SMEM_NG_SIM_INF		    0x0000000000200000
#define SMEM_NG_POL_INF			0x0000000000400000

#define SOCK_NG_UNICAST			0x0000000000000100
#define SOCK_NG_MULTICAST		0x0000000000000200
                                               

#pragma endregion タスクエラービット定義


typedef struct DeviceCode {
    UINT16  crane_id;   //クレーンID
    UINT16  pc_type;    //PC TYPE
    UINT16  pc_serial;  //PCシリアル番号
    UINT16  option;
 }ST_DEVICE_CODE, * LPST_DEVICE_CODE;

typedef struct _ST_MOVE_SET {
    double p;	//位置
    double v;	//速度
    double a;	//加速度
}ST_MOVE_SET, * LPST_MOVE_SET;

