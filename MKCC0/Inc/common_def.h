#pragma once
#include <windows.h>

//#define _DVELOPMENT_MODE                //開発環境を有効にする

///# 物理定数、係数
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

///# コード
#define L_ON            0x01  // ON
#define L_OFF           0x00  // OFF
#define STAT_OK         0x01  // ON
#define STAT_NG         0x00  // OFF

#define UNIT_CODE_M     0//単位のコード m
#define UNIT_CODE_RAD   1//単位のコード rad
#define UNIT_CODE_MM    2//単位のコード mm
#define UNIT_CODE_DEG   3//単位のコード deg


/*** 応答 ***/
#define CODE_ACCEPTED      1        //受付完了
#define CODE_REJECTED      -1       //受付不可
#define CODE_NO_REQUEST    0        //要求無し
#define CODE_EXIST         1       //有り
#define CODE_NA            0       //特になしee
#define CODE_OK            1       //有り
#define CODE_NG            -1      //NG


///# マクロ
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

///# 配列参照用　動作インデックス
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

///# シミュレーションモード
#define MODE_PRODUCTION         0x0000//実機
#define MODE_CRANE_SIM          0x0001//手動部分のセンサFBをシミュレータの出力値にする
#define MODE_SENSOR_SIM		    0x0002//振れセンサ等自動化,遠隔用センサ信の応答をクレーン物理シミュレータの出力から生成する

///# ビット定義
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



typedef struct DeviceCode {
    char    order[2];   //製番
    char    system[2];  //機械、システムコード    :クレーン番号等
    char    type[2];    //デバイス種別　          :制御PC,端末等
    INT16   no;         //シリアル番号
}ST_DEVICE_CODE, * LPST_DEVICE_CODE;

