#pragma once

#include "CBasicControl.h"
#include <opencv2/opencv.hpp>
#include "SENSOR_DEF.H"

// 画像情報
// 画像情報データ
typedef struct TAG_INFO_IMGBUF_DATA {
    cv::Mat cv_mat; // CpenCV
} INFO_IMGBUF_DATA, * PINFO_IMGBUF_DATA;
// 画像情報
typedef struct TAG_INFO_IMGBUF {
    CRITICAL_SECTION cs;
    INFO_IMGBUF_DATA data;  // 画像情報データ
} INFO_IMGBUF, * PINFO_IMGBUF;

// 画像処理情報
// ターゲット検出データ

#define  SWAY_SENSOR__RANGE_OVER_COUNT_LIMIT    40

typedef struct TAG_TARGET_DATA {
    BOOL     valid;                                         // 検出状態
    int      range_over_count;                              //レンジオーバーカウント
    double   max_val;                                       // 最大輝度
    double   pos[static_cast<uint32_t>(ENUM_AXIS::E_MAX)];  // 検出位置[pixel]
    int32_t  size;                                          // 検出サイズ
    cv::Rect roi;                                           // ROI
    cv::Size2i size_real;                                   // ターゲット実サイズmm
    cv::Size2i size_expected;                              // カメラとの距離による想定ターゲットサイズ
    cv::Size2i size_detected;                              // 検出ターゲットサイズ
    cv::Size2i size_roi_spd_margin;                        // scan後roiの移動
    double dist_camera;                                     // カメラ－ターゲット間距離

    BOOL    chk_flg;                                        //デバッグ用

    TAG_TARGET_DATA()
        : valid(FALSE)
        , range_over_count(0)
        , max_val(0.0)
        , pos{ 0.0, 0.0 }
        , size(0)
        , roi(0, 0, 100, 100)
        , size_real(80, 80)
        , size_expected(10, 10)
        , size_detected(10, 10)
        , size_roi_spd_margin(50, 50)
        , dist_camera(1.0)
        , chk_flg(FALSE)
    {
    }
} TARGET_DATA, * PTARGET_DATA;
// 振れ検出データ(ターゲット検出データの中心)
typedef struct TAG_SWAY_DATA {
    double target_pos;  // ターゲット位置[pixel]
    double target_tilt; // ターゲット傾き[pixel]
    double sway_angle;  // 振れ角[pixel]
    double sway_speed;  // 振れ速度[pixel/s]
    double sway_zero;   // 振れ中心[pixel]

    TAG_SWAY_DATA()
        : target_pos(0.0)
        , target_tilt(0.0)
        , sway_angle(0.0)
        , sway_speed(0.0)
        , sway_zero(0.0)
    {
    }
} SWAY_DATA, * PSWAY_DATA;
// 画像処理情報データ
typedef struct TAG_INFO_IMGPRC_DATA {
    std::vector<std::vector<cv::Point>> contours[static_cast<uint32_t>(ENUM_IMAGE_MASK::E_MAX)];    // 輪郭座標
    TARGET_DATA target_data[static_cast<uint32_t>(ENUM_IMAGE_MASK::E_MAX)];                         // ターゲット検出データ
    SWAY_DATA   sway_data[static_cast<uint32_t>(ENUM_AXIS::E_MAX)];                                 // 振れ検出データ(ターゲット検出データの中心)
    double      target_size;                                                                        // ターゲットサイズ(ターゲット検出データの平均)
    uint32_t    status;                                                                             // 状態
    double      img_fps;                                                                            // フレームレート[fps]
    double      img_val;                                                                            // 明度
    int32_t     exps_mode;                                                                          // 自動露光コマンド(0:停止 1:Up -1:Down)
    int32_t     exps_ctrl_mode;                                                                     // 露光設定モード(0:RESET 1:ROI)
    double      exps_time;                                                                          // 露光時間[us]
    int32_t     exps_step_count;
    double      exps_chk_brightness;                                                                //露光時間調整用輝度
    cv::Scalar mean_hsv;    //HSVフォーマットでの平均値

    TAG_INFO_IMGPRC_DATA()
        : target_size(0.0)
        , status(static_cast<uint32_t>(ENUM_PROCCESS_STATUS::DEFAULT))
        , img_fps(0.0)
        , img_val(0.0)
        , exps_mode(0)
        , exps_ctrl_mode(0)
        , exps_time(0.0)
    {
    }
} INFO_IMGPRC_DATA, * PINFO_IMGPRC_DATA;
// 画像処理情報
typedef struct TAG_INFO_IMGPRC {
    CRITICAL_SECTION cs;
    INFO_IMGPRC_DATA data;  // 画像処理情報データ
} INFO_IMGPRC, * PINFO_IMGPRC;

// システム情報
// システム情報データ
typedef struct TAG_INFO_SYSTEM_DATA {
    uint32_t status;    // ステータス

    TAG_INFO_SYSTEM_DATA()
        : status(static_cast<uint32_t>(ENUM_SYSTEM_STATUS::DEFAULT))
    {
    }
} INFO_SYSTEM_DATA, * PINFO_SYSTEM_DATA;
// システム情報
typedef struct TAG_INFO_SYSTEM {
    CRITICAL_SECTION cs;
    INFO_SYSTEM_DATA data;  // システム情報データ
} INFO_SYSTEM, * PINFO_SYSTEM;

// ***アプリケーション情報
typedef struct TAG_APP_INFO {
    INFO_IMGBUF imgbuf[static_cast<uint32_t>(ENUM_IMAGE::E_MAX)];   // 画像情報
    INFO_TILTMT tiltmt;                                             // 傾斜計情報
    INFO_CLIENT client;                                             // クライアント情報
    INFO_ADJUST adjust;                                             // 調整情報
    INFO_IMGPRC imgprc;                                             // 画像処理情報
    INFO_SYSTEM system;                                             // システム情報
} APP_INFO, * PAPP_INFO;

class CSway :   public CBasicControl
{
};

