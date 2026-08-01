#pragma once
#include <windows.h>
#include <string>
#include <opencv2/opencv.hpp>
#include "SWYSENSOR_DEF.h"
//SWAYSENSOR_DEF.hにクラス定義移行
class CSwayShared
{
public:
    CSwayShared(BOOL init = FALSE);
    ~CSwayShared();

    // メンバー変数

    // メンバー関数
    static BOOL set_app_config_ini(wchar_t* file_name);             // ini file読み込みパラメータ設定
    static BOOL set_app_config(CONFIG_COMMON data);                 // 共通設定書込み
    static BOOL get_app_config(PCONFIG_COMMON data);                // 共通設定読込み
    static BOOL set_app_config(CONFIG_CAMERA data);                 // カメラ設定書込み
    static BOOL get_app_config(PCONFIG_CAMERA data);                // カメラ設定読込み
    static BOOL set_app_config(CONFIG_MOUNTING data);               // 取付寸法設定書込み
    static BOOL get_app_config(PCONFIG_MOUNTING data);              // 取付寸法設定読込み
    static BOOL set_app_config(CONFIG_IMGPROC data);                // 画像処理条件設定書込み
    static BOOL get_app_config(PCONFIG_IMGPROC data);               // 画像処理条件設定読込み

    static BOOL set_app_info_data(uint8_t id, cv::Mat cv_mat);      // 画像データ書込み
    static BOOL get_app_info_data(uint8_t id, cv::Mat* cv_mat);     // 画像データ読込み
    static BOOL set_app_info_data(INFO_CLIENT_DATA data);           // クライアント情報データ書込み
    static BOOL get_app_info_data(PINFO_CLIENT_DATA data);          // クライアント情報データ読込み
    static BOOL set_app_info_data(INFO_ADJUST_DATA data);           // 調整情報データ書込み
    static BOOL get_app_info_data(PINFO_ADJUST_DATA data);          // 調整情報データ読込み
    static BOOL set_app_info_data(INFO_IMGPRC_DATA data);           // 画像処理情報データ書込み
    static BOOL get_app_info_data(PINFO_IMGPRC_DATA data);          // 画像処理情報データ読込み
    static BOOL set_app_info_data(INFO_SYSTEM_DATA data);           // システム情報データ書込み
    static BOOL get_app_info_data(PINFO_SYSTEM_DATA data);          // システム情報データ読込み

protected:

private:

};
