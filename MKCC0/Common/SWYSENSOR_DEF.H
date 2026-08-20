#pragma once

#include <time.h>
#include <opencv2/opencv.hpp>
#include "COMMON_DEF.h"
//=========================================================================== =
// define定義
#pragma region CONSTANT_DEFINITION
//----------------------------------------------------------------------------
#define ID_SWAY_CAMERA_SETTING_TYPE0        0           // カメラ初期パラメータ設定パターン

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
#define	EXPOSURE_CONTROL_AUTO               2           // シャッタコントロール:自動
#define	EXPOSURE_CONTROL_MANUAL             3           // シャッタコントロール:手動
#define	EXPOSURE_CONTROL_INIT               4             // シャッタコントロール:初期化


#define	EXPOSURE_CONTROL_ROI_MODE           0           // シャッタコントロール:ROI範囲
#define	EXPOSURE_CONTROL_ROI_KEEP           0x800       // シャッタコントロール:ROI保持
#define	EXPOSURE_CONTROL_RESET_STEP         0x100       // シャッタコントロール:初期化STEP 初期値セット
#define	EXPOSURE_CONTROL_RESET_STEP_FIN     0x11F       // シャッタコントロール:初期化STEP 完了待機

#define	EXPOSURE_CONTROL_RESET_WAIT_STEP    0x01        // シャッタコントロール:初期化STEP 完了待機

#define	EXPOSURE_CONTROL_STEP_COUNT        10          // シャッタコントロール:初期化STEP待機カウント

// ターゲット距離設定モード
#define	SWAY_CAL_BASE_SET_BY_DEFAULT         0           // デフォルト値をセット
#define	SWAY_CAL_BASE_SET_BY_MANUAL          1           // 手動セット（SCADAウィンドウから固定値セット
#define	SWAY_CAL_BASE_SET_BY_HOST            2           // 制御PCからのデータセット

//----------------------------------------------------------------------------
#define PI              3.14159     // π
#define CONV_DEG_RAD    (PI/180.0)  // deg→rad
#define CONV_MSEC_SEC   0.001       // msec→sec


//---------------------------------------------------------------------------------
#define SWAY_SENSOR_ROI_MIN_W       150 //ROI最小幅
#define SWAY_SENSOR_ROI_MIN_H       150 //ROI最小高さ

#define SWAY_SENSOR_RANGE_OVER_COUNT  10 //ROI最小高さ

//振れ用
#define SWAY_SENSOR_CUTOFF_HZ       2.0 //1次遅れフィルタカットオフ周波数

#pragma endregion CONSTANT_DEFINITION


//============================================================================
// enum定義
#pragma region ENUM_DEFINITION
// 画像データ状態
enum class ENUM_IMAGE_STATUS : uint32_t {
    DEFAULT = 0x00000000,   // デフォルト
    ENABLED = 0x00000001    // 画像有効
};
// 画像処理状態
enum class ENUM_PROCCESS_STATUS : uint32_t {
    DEFAULT             = 0x00000000, // デフォルト
    IMAGE_ENABLE        = 0x00000001, // 画像データ有効
    TARGET_ENABLE       = 0x00000002, // ターゲット検出有効
    ZERO_SET_RUN        = 0x00000010, // 0点設定中(カメラ)
    ZERO_SET_MCC_RUN    = 0x00000020, // 0点設定中(カメラ)(MCC)
    ZERO_SET_COMPLETED  = 0x00000100  // 0点設定完了(カメラ)
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
    SENSOR_3,       // 振れセンサー3
    E_MAX
};
enum class ENUM_SWAY_CAMERA : uint32_t {
    CAM1 = 0,   // センサカメラ1
    CAM2,       // センサカメラ2
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
    FILTER_1 = 0,   // フィルター1　ゴマ塩
    FILTER_2,       // フィルター2　穴埋め
    FILTER_3,       // フィルター3 ガウス
    FILTER_4,       // フィルター3 ガウス
    FILTER_5,       // フィルター3 ガウス
    E_MAX
};
// ノイズフィルター1
enum class ENUM_NOISE_FILTER1 : uint32_t {
    NONE = 0,   // なし
    OPENNING,   // オープニング処理  
    CLOSING,    // クロージング処理
    MEDIAN,     // 中央値フィルター
 	MORPHOLOGY, // モルフォロジー
    GAUSS,      // ガウシアン
    E_MAX
};
// ノイズフィルター2
enum class ENUM_NOISE_FILTER2 : uint32_t {
    NONE = 0,   // なし
    OPENNING,   // オープニング処理  
    CLOSING,    // クロージング処理
    MEDIAN,
    MORPHOLOGY, // モルフォロジー
    GAUSS,      // ガウシアン
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

#define CAM1_SPEC_PIXEL_H           2048
#define CAM1_SPEC_PIXEL_V           1536        // PIX
#define CAM1_SPEC_PIXEL_H_OFFSET    1024
#define CAM1_SPEC_PIXEL_V_OFFSET    768        // PIX
#define CAM1_SPEC_ANGLE_RAD_H   0.7485      //東芝テリーBG302LMCG+9mmレンズ　42.89°
#define CAM1_SPEC_ANGLE_RAD_V   0.5726      //東芝テリーBG302LMCG+9mmレンズ　32.81°
//#define CAM1_SPEC_ANGLE_RAD_H   1.5813    //東芝テリーBG302LMCG+9mmレンズ　90.6°
//#define CAM1_SPEC_ANGLE_RAD_V   1.2968    //東芝テリーBG302LMCG+9mmレンズ　74.3°

#define CAM1_SPEC_RESOLUTION_H  2800.0        // PIX/rad
#define CAM1_SPEC_RESOLUTION_V  2746.0        // PIX/rad

typedef struct StSwySensParam{
    double arr[(int)ENUM_SWAY_SENSOR::E_MAX][(int)ENUM_SWAY_CAMERA::E_MAX][(int)ENUM_AXIS::E_MAX][SWAY_CAMERA_N_PARAM] =
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

    double rad2pix[(int)ENUM_SWAY_SENSOR::E_MAX][(int)ENUM_SWAY_CAMERA::E_MAX][(int)ENUM_AXIS::E_MAX] =
    {
        {{CAM1_SPEC_RESOLUTION_H,CAM1_SPEC_RESOLUTION_V}, {CAM1_SPEC_RESOLUTION_H,CAM1_SPEC_RESOLUTION_V}},
        {{CAM1_SPEC_RESOLUTION_H,CAM1_SPEC_RESOLUTION_V}, {CAM1_SPEC_RESOLUTION_H,CAM1_SPEC_RESOLUTION_V}},
        {{CAM1_SPEC_RESOLUTION_H,CAM1_SPEC_RESOLUTION_V}, {CAM1_SPEC_RESOLUTION_H,CAM1_SPEC_RESOLUTION_V}}
     };
}ST_SWY_SENS_PRM, *LPST_SWY_SENS_PRM;

#pragma pack(push, 1)
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
    uint32_t     img_source_camera;                 // カメラ画像取込み(カメラ[1]/画像ファイル[0])
    std::wstring img_source_fname;                  // 取込み画像ファイル名(CMN_IMAGE_SOURCE_CAMERA=0のときの画像)
    std::wstring img_output_fname;                  // 画像保存ファイル名
    double full_pix[(int)ENUM_AXIS::E_MAX];         // カメラのピクセルサイズ
    double view_angle_rad[(int)ENUM_AXIS::E_MAX];   // カメラの画角rad
    double PIXperRAD[(int)ENUM_AXIS::E_MAX];        // 1radのPixel数

    CONFIG_COMMON_IMAGE_SCREEN_LAYOUT img_screen_layout;    // 画像表示レイアウト設定

    TAG_CONFIG_COMMON()
        : img_source_camera(0)
        , img_source_fname(L"")
        , img_output_fname(L"")
    {}
} CONFIG_COMMON, * PCONFIG_COMMON;
// ROI設定
typedef struct TAG_CONFIG_CAMERA_ROI {
    uint32_t offset = 0;        // ROI(領域)の開始位置(X, Y)
    uint32_t size = 512;        // ROI(領域)のサイズ(X(32の倍数, 2592以下), Y(2の倍数, 2048以下))
    uint32_t margin_pix = 200;  // 
} CONFIG_CAMERA_ROI, * PCONFIG_CAMERA_ROI;
// ホワイトバランスゲイン設定
typedef struct TAG_CONFIG_CAMERA_BALANCE_WHITE {
    unsigned int wb_auto;       // ホワイトバランスゲイン自動調整モー設定(自動調整OFF[1]/連続自動調整[2]/一回のみ自動調整[3])
    double       wb_ratio_red;  // ホワイトバランスゲイン設定(Red Gain)
    double       wb_ratio_blue; // ホワイトバランスゲイン設定(Blue Gain)

    TAG_CONFIG_CAMERA_BALANCE_WHITE()
        : wb_auto(0)
        , wb_ratio_red(1.0)
        , wb_ratio_blue(3.4)
    {
    }
} CONFIG_CAMERA_BALANCE_WHITE, * PCONFIG_CAMERA_BALANCE_WHITE;
// カメラの基本設定
typedef struct TAG_CONFIG_CAMERA_BASIS {
    std::wstring                ipaddress = L"172.31.0.30";  // IPアドレス
    CONFIG_CAMERA_ROI           roi[(int)ENUM_AXIS::E_MAX];  // ROI(領域)
    uint32_t                    packetsize = 9000; // ドライバが受け取るパケットの最大サイズ[byte]
    double                      framerate = 25.0;  // フレームレート[fps]
    double                      blacklevel = 0.0; // 黒レベル設定
    double                      gamma = 1.0;      // ガンマ補正
    CONFIG_CAMERA_BALANCE_WHITE wb;         // ホワイトバランスゲイン設定
    double                      viewangle[(int)ENUM_AXIS::E_MAX]; // 視野角[deg]
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

} CONFIG_CAMERA_BRIGHTNESS_CONTROL, * PCONFIG_CAMERA_BRIGHTNESS_CONTROL;
// カメラの異常判定設定
typedef struct TAG_CONFIG_CAMERA_ERROR {
    double   framedrop; // フレームレート低下の判定値[fps]
    uint32_t errtime;   // 異常判定時間[msec]
} CONFIG_CAMERA_ERROR, * PCONFIG_CAMERA_ERROR;
// カメラの起動設定
typedef struct TAG_CONFIG_CAMERA_BOOT {
    uint32_t bootwaittimer;     // システム起動後のカメラ起動待ち時間[msec]
    uint32_t rebootcount;       // システム起動後のカメラ起動失敗時にカメラリを再起動する回数[count]
    uint32_t rebootingtimer;    // カメラ再起動中であることを通知する時間[msec]
} CONFIG_CAMERA_BOOT, * PCONFIG_CAMERA_BOOT;
// カメラ設定
typedef struct TAG_CONFIG_CAMERA {
    CONFIG_CAMERA_BASIS              basis;     // カメラの基本設定
    CONFIG_CAMERA_BRIGHTNESS_CONTROL gain;      // 輝度コントロール設定(ゲイン)
    CONFIG_CAMERA_BRIGHTNESS_CONTROL expstime;  // 輝度コントロール設定(露光時間)
    CONFIG_CAMERA_ERROR              error;     // カメラの異常判定設定
    CONFIG_CAMERA_BOOT               boot;      // カメラの起動設定
} CONFIG_CAMERA, * PCONFIG_CAMERA;

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
// 画像処理ノイズフィルター
typedef struct TAG_CONFIG_IMGPROC_FILTER {
    uint32_t type;  // フィルター種類
    uint32_t val;   // フィルター値

    TAG_CONFIG_IMGPROC_FILTER()
        : type(0)
        , val(0)
    {}
} CONFIG_IMGPROC_FILTER, * PCONFIG_IMGPROC_FILTER;
// 画像処理マスク設定
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
    CONFIG_IMGPROC_MASK   mask[(uint32_t)(ENUM_IMAGE_MASK::E_MAX)];      // マスク設定
    CONFIG_IMGPROC_FILTER filter[(uint32_t)(ENUM_NOISE_FILTER::E_MAX)];  // ノイズフィルター(0=ゴマ塩, 1=穴埋め)
    uint32_t              imgprc;                                        // ターゲット検出アルゴリズム(0=最大輪郭面積, 1=最大輪郭長)
    double                swayspdfilter;                                 // 振れ速度フィルター時定数
    uint32_t              swayzeroset_time;                              // 振れゼロ点設定(計測時間[ms])
 
    TAG_CONFIG_IMGPROC()
        : imgprc(0)
        , swayspdfilter(0.0)
        , swayzeroset_time(0)
         
    {}
} CONFIG_IMGPROC, * PCONFIG_IMGPROC;

//----------------------------------------------------------------------------
// アプリケーション情報の構造体
//----------------------------------------------------------------------------
// 調整情報
// 調整情報データ
#define  SWAY_SENSOR_MASK_MODE_IDLE            0x0000
#define  SWAY_SENSOR_MASK_MODE_1               0x0001
#define  SWAY_SENSOR_MASK_MODE_2               0x0002
#define  SWAY_SENSOR_MASK_MODE_12              0x0003

typedef struct TAG_INFO_ADJUST_DATA {
    int mask_mode;                                  //マスクモード(適用するマスク画像の選択　MASK1,MASK2,MASK12）
    int32_t host_source_mode;                       // ターゲット間距離セットモード
    double target_distance;                         // 吊点～ターゲット間距離[mm]
    double coef_roi_margin[(int)ENUM_AXIS::E_MAX];  // ROI設定マージン計算用係数
  //  double T[(int)ENUM_AXIS::E_MAX];                // 振れ周期sec
  //  double w[(int)ENUM_AXIS::E_MAX];                // 振れ角周波数 /s
  //  double g[(int)ENUM_AXIS::E_MAX];                // 重力加速度
 
    TAG_INFO_ADJUST_DATA()
        : host_source_mode(SWAY_CAL_BASE_SET_BY_DEFAULT)
        , target_distance(EXTN_TARGETDIST_MIN)
     {}
} INFO_ADJUST_DATA, * PINFO_ADJUST_DATA;
// 調整情報
typedef struct _stExtnInfo {
    CRITICAL_SECTION cs;
    INFO_ADJUST_DATA data;  // 調整情報データ
} INFO_ADJUST, * PINFO_ADJUST;

// 元画像データ
typedef struct TAG_INFO_IMGBUF_DATA {
    cv::Mat cv_mat; // CpenCV
} INFO_IMGBUF_DATA, * PINFO_IMGBUF_DATA;
// 画像情報
typedef struct TAG_INFO_IMGBUF {
    CRITICAL_SECTION cs;
    INFO_IMGBUF_DATA data;  // 画像情報データ
} INFO_IMGBUF, * PINFO_IMGBUF;

typedef struct TAG_IMAGE_DATA {
    uint32_t status;        // 画像ステータス
    uint32_t width;         // 画像サイズ(水平画素) [pixel]
    uint32_t height;        // 画像サイズ(垂直画素) [pixel]
    double   fps;           // 画像フレームレート [fps]
    uint8_t* data_bgr;      // 画像データバッファのポインタ(BGR 24bit)
    cv::Mat  data_mat;      // 画像データ(OpenCV変換画像)

    TAG_IMAGE_DATA()
        : status(static_cast<uint32_t>(ENUM_IMAGE_STATUS::DEFAULT))
        , width(0)
        , height(0)
        , fps(0.0)
        , data_bgr(NULL)
    {
    }
} IMAGE_DATA, * PIMAGE_DATA;
// 画像処理情報
// ターゲット検出データ
#define  SWAY_SENSOR_RANGE_OVER_COUNT_LIMIT    40

typedef struct TAG_TARGET_DATA {
    BOOL     valid;                                         // 検出状態
    int      range_over_count;                              // レンジオーバーカウント
    double   max_val;                                       // 最大輝度
    double   pos[static_cast<uint32_t>(ENUM_AXIS::E_MAX)];  // 検出位置[pixel]
    int32_t  size;                                          // 検出サイズ
    cv::Rect roi;                                           // ROI
    cv::Size2i size_real;                                   // ターゲット実サイズmm
    cv::Size2i size_expected;                               // カメラとの距離による想定ターゲットサイズ
    cv::Size2i size_detected;                               // 検出ターゲットサイズ
    cv::Size2i size_roi_spd_margin;                         // scan後roiの移動
        
    BOOL    chk_flg;                                        //デバッグ用

    TAG_TARGET_DATA()
        : valid(FALSE)
        , range_over_count(0)
        , max_val(0.0)
        , pos{ 0.0, 0.0 }
        , size(0)
        , roi(0, 0, 100, 100)
        , size_real(80,80)
        , size_expected(10, 10)
        , size_detected(10, 10)
        , size_roi_spd_margin(50,50)
        ,chk_flg(FALSE)
    {}
} TARGET_DATA, * PTARGET_DATA;
// 振れ検出データ(ターゲット検出データの中心)
typedef struct TAG_SWAY_DATA {
    double alpha;       // 1次遅れフィルタ係数
    double tau;         // 時定数
    double T;           // 振れ周期
    double w;           // 角周波数
    double w2;          // 角周波数の2乗
    double g;           // 重力加速度
    double p0;          // 振れ中心[pixel]
    double p;           // ターゲット位置[pixel]
    double til;         // ターゲット傾き[pixel]
    double v;           // 振れ速度[pixel/s]
    double a;           // 振れ速度[pixel/s]
    double amp_p2p;     // 振れ振幅評価値　Peak2Peak計測
    double amp_cal;     // 振れ振幅評価値　計算　sqrt((speed^2 + acc^2)/(w^2+w^4))=sqrt((speed/w)^2 +(acc/w^2)^2)
    double amp_disp;    // 振れ振幅評価値　表示用　CAL orP2P
    double ph_cal = 0.0;      // 振れ位相　計算　atan(-(acc/w)/speed)
    double ph_time = 0.0;     // 振れ位相　振れ速度+-の変化でプリセット後、wtで評価
    double ph_disp;     // 振れ位相　表示用　CAL or　time
    double vw;          // v/w
    double aw2;         // a/w/w

    std::vector<double> p_history;

    TAG_SWAY_DATA()
        : p(0.0)
        , til(0.0)
        , v(0.0)
        , a(0.0)
        , p0(0.0)
    {}
} SWAY_DATA, * PSWAY_DATA;
// 画像処理情報データ
typedef struct TAG_INFO_IMGPRC_DATA {
    std::vector<std::vector<cv::Point>> contours[static_cast<uint32_t>(ENUM_IMAGE_MASK::E_MAX)];    // 輪郭座標
    TARGET_DATA target_data[static_cast<uint32_t>(ENUM_IMAGE_MASK::E_MAX)];                         // ターゲット検出データ
    SWAY_DATA   sway_data[static_cast<uint32_t>(ENUM_AXIS::E_MAX)];                                 // 振れ検出データ(ターゲット検出データの中心)
    double      target_size;                                                                        // ターゲットサイズ(ターゲット検出データの平均)
    uint32_t    status;  
    uint32_t    image_loss_count;                                                                   
    double      img_fps;                                                                            // フレームレート[fps]
   // double      img_val;                                                                            // 明度
    int32_t     exps_mode;                                                                          // 自動露光コマンド(0:停止 1:Up -1:Down)
    int32_t     exps_ctrl_mode;                                                                     // 露光設定モード(0:RESET 1:ROI)
    double      exps_time;                                                                          // 露光時間[us]
    int32_t     exps_step_count;
    double      exps_chk_brightness;                                                                //露光時間調整用輝度
    double      fc;                                                                                 //1次遅れフィルタカットオフ周波数


    cv::Scalar mean_hsv;    //HSVフォーマットでの平均値

    TAG_INFO_IMGPRC_DATA()
        : target_size(0.0)
        , status(static_cast<uint32_t>(ENUM_PROCCESS_STATUS::DEFAULT))
        , img_fps(0.0)
        , exps_mode(0)
        , exps_ctrl_mode(0)
        , exps_time(0.0)
    {}
} INFO_IMGPRC_DATA, * PINFO_IMGPRC_DATA;
// 画像処理情報
typedef struct TAG_INFO_IMGPRC {
    CRITICAL_SECTION cs;
    INFO_IMGPRC_DATA data;  // 画像処理情報データ
} INFO_IMGPRC, * PINFO_IMGPRC;
// システム情報
// システム情報データ
typedef struct TAG_INFO_SYSTEM_DATA {
    uint32_t status;            // ステータス
    uint32_t host_com_status;   // 制御PCとの通信状態ステータス
    double sample_cycle;
   
    TAG_INFO_SYSTEM_DATA()
        : status(static_cast<uint32_t>(ENUM_SYSTEM_STATUS::DEFAULT))
    {}
} INFO_SYSTEM_DATA, * PINFO_SYSTEM_DATA;
// システム情報
typedef struct TAG_INFO_SYSTEM {
    CRITICAL_SECTION cs;
    INFO_SYSTEM_DATA data;  // システム情報データ
} INFO_SYSTEM, * PINFO_SYSTEM;
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
    {
    }
} INFO_CLIENT_DATA, * PINFO_CLIENT_DATA;
typedef struct TAG_INFO_CLIENT {
    CRITICAL_SECTION cs;
    INFO_CLIENT_DATA data;  // クライアント情報データ
} INFO_CLIENT, * PINFO_CLIENT;

// ***アプリケーション情報
typedef struct TAG_APP_INFO {
    INFO_IMGBUF imgbuf[static_cast<uint32_t>(ENUM_IMAGE::E_MAX)];   // 画像情報
    INFO_CLIENT client;                                             // クライアント情報
    INFO_ADJUST adjust;                                             // 調整情報
    INFO_IMGPRC imgprc;                                             // 画像処理情報
    INFO_SYSTEM system;                                             // システム情報
} APP_INFO, * PAPP_INFO;

// ***アプリケーション設定
typedef struct TAG_APP_CONFIG {
    CONFIG_COMMON    common;        // 共通設定
    CONFIG_CAMERA    camera;        // カメラ設定
    CONFIG_MOUNTING  mounting;      // 取付寸法設定
    CONFIG_IMGPROC   imageprocess;  // 画像処理条件設定
} APP_CONFIG, * PAPP_CONFIG;

// #######################################################
// ###### ***通信データ
// ######################################################

#define SW_CLIENT_COM_CAMERA1_0SET     0x00000010

/// <summary>
/// 振れセンサIF
/// </summary>
typedef struct _SwayClientHead {
    ST_DEVICE_CODE	myid;		    //装置ID
//    SOCKADDR_IN		addr;	    //送信元受信アドレス
    INT16			mask_mode;	        //0:IDLE 1:MASK1のみ　2：MASK2のみ　3：MASK12共使用
    INT16           filter_type1;       //画像フィルタ処理　ENUM_NOISE_FILTER1の定義で指定
    INT16           filter_type1_val;   //画像フィルタ処理　設定値
    INT16           filter_type2;       //画像フィルタ処理　ENUM_NOISE_FILTER2の定義で指定
    INT16           filter_type2_val;   //画像フィルタ処理　設定値
    INT16			command;	//コマンドコード(初期化要求等）
    INT32			status;		//状態
    INT32 			seqno;		//シーケンス番号(ヘルシーチェック用）	
}ST_SWAY_CLIENT_HEAD, * LPST_SWAY_CLIENT_HEAD;

typedef struct SwayClientBody {
    double          tg_distance;
    double          T[(int)ENUM_AXIS::E_MAX]; //振れ周期
    double          w[(int)ENUM_AXIS::E_MAX]; //振れ角周波数
}ST_SWAY_CLIENT_BODY, * LPST_SWAY_CLIENT_BODY;

typedef struct SwayClientMsg {
    ST_SWAY_CLIENT_HEAD head;
    ST_SWAY_CLIENT_BODY body;
}ST_SWAY_CLIENT_MSG, * LPST_SWAY_CLIENT_MSG;

typedef struct _SwayServerHead {
    ST_DEVICE_CODE	myid;		//装置ID
    //    SOCKADDR_IN		addr;		//送信元受信アドレス
    INT16			mask_mode;	                        //0:IDLE 1:MASK1のみ　2：MASK2のみ　3：MASK12共使用 FB
    INT16           filter_type1;                       //画像フィルタ処理FB　ENUM_NOISE_FILTER1の定義
    INT16           filter_type1_val;                   //画像フィルタ処理FB　設定値
    INT16           filter_type2;                       //画像フィルタ処理FB　ENUM_NOISE_FILTER2の定義
    INT16           filter_type2_val;                   //画像フィルタ処理FB　設定値
    INT16			command;	                        //コマンド応答コード(受付コマンドコード）
    INT32			status;		                        //状態
    double          pix1rad[(int)ENUM_AXIS::E_MAX];     //1radianあたりのPIXEL数
    INT32 			seqno;		                        //シーケンス番号(ヘルシーチェック用）	
}ST_SWAY_SERVER_HEAD, * LPST_SWAY_SERVER_HEAD;

typedef struct _ST_TARGET_DETECT {
    INT32 valid;                          //ターゲット有効
    INT32 value;                          //輝度
    double pix[(int)(ENUM_AXIS::E_MAX)];   //PIXCEL位置
}ST_TARGET_DETECT, * LPST_TARGET_DETECT;

typedef struct _ST_SWAY_DETECT {
    double  p0;             //振れ0点PIXEL値
    double  p;              //振れ0点からの振角　PIX
    double  v;              //振角速度　PIX/s
    double  amp_p2p;        //Peak to Peakで計測した振れ振幅(角速度の符号切り替わりで判定)
    double  amp_cal;        //振れ角速度から評価した振れ振幅(半周期間の振れ角速度最大値で判定）
    double  ph_cal;         //振れ位相評価値(振れ角、振れ角速度の符号変化から振れ周期時間カウントで求める
    double  ph_time;        //振れ位相評価値(振れ角、振れ角速度の符号変化から振れ周期時間カウントで求める
    double  T;              //計測振れ周期s
    double  vw;             // v/w SCATチャート用
}ST_SWAY_DETECT, * LPST_SWAY_DETECT;

typedef struct SwayServerBody {
    ST_TARGET_DETECT   tg_data[(int)ENUM_IMAGE::E_MAX];
    ST_SWAY_DETECT     sway_data[(int)ENUM_AXIS::E_MAX];

}ST_SWAY_SERVER_BODY, * LPST_SWAY_SERVER_BODY;

typedef struct SwayServerMsg {
    ST_SWAY_SERVER_HEAD head;
    ST_SWAY_SERVER_BODY body;
}ST_SWAY_SERVER_MSG, * LPST_SWAY_SERVER_MSG;

////送信データコード
//#define SW_CLIENT_COM_ONCE_DATA        0x00000000
//#define SW_CLIENT_COM_CONST_DATA       0x00000001
//#define SW_CLIENT_COM_STOP_DATA        0x00000008

//#define SW_CLIENT_COM_CAMERA2_0SET     0x00000020
//#define SW_CLIENT_COM_TILT1_0SET       0x00000040
//#define SW_CLIENT_COM_TILT2_0SET       0x00000080
//#define SW_CLIENT_COM_CAMERAR1_RESET   0x00000100
//#define SW_CLIENT_COM_CAMERAR2_RESET   0x00000200
//#define SW_CLIENT_COM_TILT1_RESET      0x00000400
//#define SW_CLIENT_COM_TILT2_RESET      0x00000800
//#define SW_CLIENT_COM_SAVE_IMG         0x00001000
//#define SW_CLIENT_COM_PC_RESET         0x10000000
//
//#define SW_SENSOR_DEFAULT_SCAN         100                 //100msec
//
//#define SW_SENSOR_MODE_NORMAL          0x000000001          //通常運用モード
//#define SW_SENSOR_MODE_CHECK1          0x000000002          //試験チェックモード 
//
//
////受信データコード
////エラー状態
//#define SW_SENSOR_ERR_CODE_CAMERA      0x00000001   // 機器異常(カメラ)
//#define SW_SENSOR_ERR_CODE_TILT        0x00000002   // 機器異常(傾斜計)
//#define SW_SENSOR_ERR_CODE_DETECT_TG1  0x00000010   // 検出異常(ターゲット1)
//#define SW_SENSOR_ERR_CODE_DETECT_TG2  0x00000020   // 検出異常(ターゲット2)
//
////検出状態
//#define SW_SENSOR_STAT_TG1_DETECTED    0x00000001   // 振れ検出中(ターゲット1)
//#define SW_SENSOR_STAT_TG2_DETECTED    0x00000002   // 振れ検出中(ターゲット2)
//#define SW_SENSOR_STAT_INIT_SENSOR     0x00000010   // 準備中(カメラ)
//#define SW_SENSOR_STAT_INIT_TILT       0x00000020   // 準備中(傾斜計)
//#define SW_SENSOR_STAT_0SET_SENSOR     0x00000010   // 0点設定中(カメラ)
//#define SW_SENSOR_STAT_0SET_TILT       0x00000020   // 0点設定中(傾斜計)
//
////メッセージテキスト
////エラー状態
//#define SW_TXT_ERR_CODE_CAMERA      L"カメラ異常"
//#define SW_TXT_ERR_CODE_TILT        L"傾斜計異常"
//#define SW_TXT_ERR_CODE_DETECT_TG1  L"ターゲット１検出異常"
//#define SW_TXT_ERR_CODE_DETECT_TG2  L"ターゲット２検出異常"
//
////検出状態
//#define SW_TXT_STAT_TG1_DETECTED    L"ターゲット１検出中"
//#define SW_TXT_STAT_TG2_DETECTED    L"ターゲット２検出中"
//#define SW_TXT_STAT_INIT_SENSOR     L"センサ準備中"
//#define SW_TXT_STAT_INIT_TILT       L"傾斜計準備中"
//#define SW_TXT_STAT_0SET_SENSOR     L"センサ0セット中"
//#define SW_TXT_STAT_0SET_TILT       L"傾斜計0セット中"
//
//#define SWAY_SENSOR_N_CAM       2
//#define SWAY_SENSOR_CAM1        0
//#define SWAY_SENSOR_CAM2        1
//
//#define SWAY_SENSOR_N_TARGET    2
//#define SWAY_SENSOR_TG1         0
//#define SWAY_SENSOR_TG2         1
//
//#define SWAY_SENSOR__MSG_SEND_COM         10600
//
//#define SWAY_SENSOR_N_CHAR_ID   4
//#define SWAY_SENSOR_N_CHAR_INFO 32
//
//typedef struct SwayCamSpec {
//    INT32	pix_x;			//カメラ画素数x軸
//    INT32	pix_y;			//カメラ画素数y軸
//    INT32	pixlrad_x;	    //カメラ分解能　PIX/rad
//    INT32	pixlrad_y;	    //カメラ分解能　PIX/rad
//    INT32	l0_x;			//カメラ取付パラメータ㎜
//    INT32	l0_y;			//カメラ取付パラメータ㎜
//    INT32	ph0_x;			//カメラ取付パラメータx1000000rad
//    INT32	ph0_y;			//カメラ取付パラメータx1000000rad
//    INT32	phc_x;			//カメラ取付パラメータx1000000rad
//    INT32	phc_y;			//カメラ取付パラメータx1000000rad
//}ST_SWAY_CAM_SPEC, * LPST_SWAY_CAM_SPEC;
//
//#define CODE_SWAY_SENSOR_IMAGE_OK           0x00000001
//#define CODE_SWAY_SENSOR_SWAY_DETECTED      0x00000002
//#define CODE_SWAY_SENSOR_TG1_DETECTED       0x00000004
//#define CODE_SWAY_SENSOR_TG2_DETECTED       0x00000008
//#define CODE_SWAY_SENSOR_SWAY_RANGEOVER     0x00000010
//#define CODE_SWAY_SENSOR_SWAY_CAM_ERROR     0x00000080
//
//#define CODE_SWAY_SENSOR_TILT_DETECTED      0x00010000
//#define CODE_SWAY_SENSOR_TILT_ERROR         0x00100000
//
//typedef struct SwaySensStatus {
//    //機器状態情報
//    INT32	mode;		    //モード
//    INT32	error;		    //エラーステータス
//    INT32	status;		    //検出ステータス
//    //傾斜計情報  
//    double	tilt_x;         //傾斜X
//    double	tilt_y;         //傾斜Y
//    double	tilt_dx;        //傾斜角速度X
//    double	tilt_dy;        //傾斜角速度Y
//} ST_SWAY_SENS_STAT, * LPST_SWAY_SENS_STAT;
//
//typedef struct TargetStatus {
//    //振れ検出情報  
//    double	th_x;		    //振角xPIX
//    double	th_y;		    //振角yPIX
//    double	dth_x;		    //振角速度x　PIX/s
//    double	dth_y;			//振角速度y　PIX/s
//    double	th_x0;			//振角0点xPIX
//    double	th_y0;			//振角0点yPIX
//    double	dpx_tgs;		//ターゲット間距離X方向
//    double	dpy_tgs;		//ターゲット間距離Y方向
//    double	tg_size;		//ターゲットサイズ
//} ST_TARGET_STAT, * LPST_TARGET_STAT;
//
//typedef struct SwayComServerHead { //振れセンサ受信メッセージヘッダ部
//    char	id[SWAY_SENSOR_N_CHAR_ID];			    //PC ID
//    SYSTEMTIME time;		                        //タイムスタンプ
//}ST_SWAY_SERVER_HEAD, * LPST_SWAY_SERVER_HEAD;
//typedef struct SwayComServerBody {                  //振れセンサ送信メッセージボディ部
//    ST_SWAY_CAM_SPEC cam_spec;
//    ST_SWAY_SENS_STAT cam_stat;
//    ST_TARGET_STAT tg_stat[SWAY_SENSOR_N_TARGET];   //カメラ２ｘターゲット２
//    char info[SWAY_SENSOR_N_CHAR_INFO];
//}ST_SWAY_SERVER_BODY, * LPST_SWAY_SERVER_BODY;
//typedef struct SwayComServerMsg {                   //振れセンサ送信メッセージ
//    ST_SWAY_SERVER_HEAD head;
//    ST_SWAY_SERVER_BODY body[SWAY_SENSOR_N_CAM];
//}ST_SWAY_SERVER_MSG, * LPST_SWAY_SERVER_MSG;
//
//typedef struct SwayComClientHead {                  //制御PC送信メッセージヘッダ部
//    char	id[SWAY_SENSOR_N_CHAR_ID];			    //機器個体情報
// //   sockaddr_in sockaddr;                         //送信元IPアドレス
//}ST_SWAY_CLIENT_HEAD, * LPST_SWAY_CLIENT_HEAD;
//typedef struct SwayComClientBody {                  //制御PC送信メッセージボディ部
//    INT32 command;
//    INT32 mode;
//    INT32 freq;                                     //最小受信周期 msec      
//    INT32 d[SWAY_SENSOR_N_CAM];                     //カメラ-ターゲット間距離
//}ST_SWAY_CLIENT_BODY, * LPST_SWAY_CLIENT_BODY;
//typedef struct SwayComClientMsg {                   //制御PC送信メッセージ
//    ST_SWAY_CLIENT_HEAD head;
//    ST_SWAY_CLIENT_BODY body;
//}ST_SWAY_CLIENT_MSG, * LPST_SWAY_CLIENT_MSG;

#pragma pack(pop)