#include "CSwayShared.h"
#include "CCamera.h"

//////////////////////////////////////////////////////////////////////////////
// CSwayShared

//グローバル
APP_CONFIG g_app_cnfg; // アプリケーション設定
APP_INFO   g_app_info; // アプリケーション情報

PAPP_CONFIG gp_app_cnfg; // アプリケーション設定
PAPP_INFO   gp_app_info; // アプリケーション情報

// ***アプリケーション設定アクセスポインタ
PCONFIG_COMMON    gp_cnfg_common;        // 共通設定
PCONFIG_CAMERA    gp_cnfg_camera;        // カメラ設定
PCONFIG_MOUNTING  gp_cnfg_mounting;      // 取付寸法設定
PCONFIG_IMGPROC   gp_cnfg_imgprc;        // 画像処理条件設定

// ***アプリケーション情報アクセスポインタ  
PINFO_IMGBUF_DATA gp_app_imgbuf[static_cast<uint32_t>(ENUM_IMAGE::E_MAX)];   // 画像情報
PINFO_CLIENT_DATA gp_app_client;        // クライアント情報
PINFO_ADJUST_DATA gp_app_adjust;        // 調整情報
PINFO_IMGPRC_DATA gp_app_imgprc;        // 画像処理情報
PINFO_SYSTEM_DATA gp_app_system;        // システム情報

CTeliCamLib* pCamera;//GEカメラオブジェクトへのグローバルポインタ
IMAGE_DATA g_img_src_work;// 作業用画像データ(BGR 24bit)

//////////////////////////////////////////////////////////////////////////////
// Public method

/// @brief
/// @param
/// @return
/// @note
CSwayShared::CSwayShared(BOOL init)
{
    gp_cnfg_common         = &g_app_cnfg.common;        // 共通設定
    gp_cnfg_camera         = &g_app_cnfg.camera;        // カメラ設定
    gp_cnfg_mounting       = &g_app_cnfg.mounting;      // 取付寸法設定
    gp_cnfg_imgprc         = &g_app_cnfg.imageprocess;  // 画像処理条件設定
    
    for(int i=0;i<static_cast<uint32_t>(ENUM_IMAGE::E_MAX);i++){
        gp_app_imgbuf[i] = &g_app_info.imgbuf[i].data;   // 画像情報
	}
    gp_app_client          = &g_app_info.client.data;   // クライアント情報
    gp_app_adjust          = &g_app_info.adjust.data;   // 調整情報
    gp_app_imgprc          = &g_app_info.imgprc.data;   // 画像処理情報
    gp_app_system          = &g_app_info.system.data;   // システム情報
  
    //----------------------------------------------------------------------------
    // カメラデータ受信バッファ準備
    if (g_img_src_work.data_bgr == NULL) {
        g_img_src_work.data_bgr = new (uint8_t[IMAGE_SIZE * IMAGE_FORMAT_SIZE]);                     // 画像データバッファのポインタ(BGR 24bit)
    }
    ZeroMemory(g_img_src_work.data_bgr, (sizeof(uint8_t) * IMAGE_SIZE * IMAGE_FORMAT_SIZE));         // The all clear the data area

    //----------------------------------------------------------------------------
    // 共有データアクセス用クリティカルセクションの初期化
    for (int i = 0; i < static_cast<uint32_t>(ENUM_IMAGE::E_MAX); i++) {
        InitializeCriticalSection(&g_app_info.imgbuf[i].cs);   // 画像情報
    }
    InitializeCriticalSection(&g_app_info.client.cs);   // クライアント情報
    InitializeCriticalSection(&g_app_info.adjust.cs);   // 調整情報
    InitializeCriticalSection(&g_app_info.imgprc.cs);   // 画像処理情報
    InitializeCriticalSection(&g_app_info.system.cs);   // システム情報

    if (init) {
        //----------------------------------------------------------------------------
        // データ初期化

        // クライアント情報データ
        PINFO_CLIENT_DATA info_client_data = gp_app_client;             // クライアント情報データ
        for(int i=0;i<4;i++) info_client_data->device_id[i] = '0';  // 機器ID
        info_client_data->command = 0;                              // コマンド
        info_client_data->mode = 0;                                 // モード
        info_client_data->cycle = 0;                                // 最小受信周期[ms]
        for (uint32_t idx = 0; idx < static_cast<uint32_t>(ENUM_SWAY_SENSOR::E_MAX); idx++) {
            info_client_data->tgt_len[idx] = 0.0;                   // ターゲット距離[mm]
        }

        // 調整情報データ
        PINFO_ADJUST_DATA info_adjust_data = gp_app_adjust;   // 調整情報データ
        info_adjust_data->tg_distance_mode = TARGET_DIST_SET_BY_DEFAULT; // 吊点～ターゲット間距離固定
        info_adjust_data->target_distance = EXTN_TARGETDIST_MIN;        // 吊点～ターゲット間距離

        // 画像処理情報データ
        PINFO_IMGPRC_DATA info_imgprc_data = &g_app_info.imgprc.data;   // 画像処理情報データ
        for (uint32_t idx = 0; idx < static_cast<uint32_t>(ENUM_IMAGE_MASK::E_MAX); idx++) {
            info_imgprc_data->target_data[idx].valid = FALSE;           // 検出状態
            info_imgprc_data->target_data[idx].max_val = 0.0;           // 最大輝度
            for (uint32_t axis = 0; axis < static_cast<uint32_t>(ENUM_AXIS::E_MAX); axis++) {
                info_imgprc_data->target_data[idx].pos[axis] = 0.0;     // 検出位置[pixel]
            }
            info_imgprc_data->target_data[idx].size = 0;                // 検出サイズ
            info_imgprc_data->target_data[idx].roi.x = 0;               // ROI:x coordinate of the top-left corner
            info_imgprc_data->target_data[idx].roi.y = 0;               // ROI:y coordinate of the top-left corner
            info_imgprc_data->target_data[idx].roi.width = 0;           // ROI:width of the rectangle
            info_imgprc_data->target_data[idx].roi.height = 0;          // ROI:height of the rectangle
        }
        for (UINT axis = 0; axis < static_cast<uint32_t>(ENUM_AXIS::E_MAX); axis++) {
            info_imgprc_data->sway_data[axis].target_pos = 0.0;         // ターゲット位置[pixel]
            info_imgprc_data->sway_data[axis].target_tilt = 0.0;        // ターゲット傾き[pixel]
            info_imgprc_data->sway_data[axis].sway_angle = 0.0;         // 振れ角[pixel]
            info_imgprc_data->sway_data[axis].sway_speed = 0.0;         // 振れ速度[pixel/s]
            info_imgprc_data->sway_data[axis].sway_zero = 0.0;          // 振れ中心[pixel]
        }
        info_imgprc_data->target_size = 0.0;                                               // ターゲットサイズ(ターゲット検出データの平均)
        info_imgprc_data->status = static_cast<uint32_t>(ENUM_PROCCESS_STATUS::DEFAULT);   // 状態
        info_imgprc_data->img_fps = 0.0;                                                   // フレームレート[fps]
        info_imgprc_data->img_val = 0.0;                                                   // 明度
        info_imgprc_data->exps_mode = EXPOSURE_CONTROL_HOLD;                               // 自動露光コマンド(0:停止 1:Up -1:Down)
        info_imgprc_data->exps_ctrl_mode = EXPOSURE_CONTROL_RESET_STEP;                               // 自動露光コマンド(0:停止 1:Up -1:Down)
        info_imgprc_data->exps_time = 0.0;                                                 // 露光時間[us]

        // システム情報データ
        PINFO_SYSTEM_DATA info_system_data = &g_app_info.system.data;                      // システム情報データ
        info_system_data->status = static_cast<uint32_t>(ENUM_SYSTEM_STATUS::DEFAULT);     // ステータス
 
    }   // if (init)
}

/// @brief
/// @param
/// @return
/// @note
CSwayShared::~CSwayShared()
{
	delete pCamera;
    delete g_img_src_work.data_bgr;
}

/// @brief ini file読み込みパラメータ設定
/// @param file_name:ファイル名
/// @return 
/// @note 
BOOL CSwayShared::set_app_config_ini(wchar_t* file_name)
{
    wchar_t str[256] = { 0 };
    wchar_t note[256] = { 0 };
#if 0
    //----------------------------------------------------------------------------
    // 共通設定
#pragma region SET_CONFIG_COMMON
  //  PCONFIG_COMMON cnfg_common = gp_cnfg_common;    // 共通設定 ←オリジナルはset関数不使用だったので使用するよう修正削除
    CONFIG_COMMON data_common;
    PCONFIG_COMMON cnfg_common = &data_common;

    // カメラ画像取込み
    if (GetPrivateProfileString(INI_SCT_COMMON,             //file_nemeのファイルからstrに値読み込み　カメラ画像ソースのコード　1：カメラ　0:画像ファイル
        INI_KEY_COMMON_IMAGE_SOURCE_CAMERA,
        L"", str, sizeof(str) / sizeof(*str),
        file_name) <= 0) {
        return FALSE;
    }
    else {                                                  //読み出し正常で
        if ((INI_KEY_COMMON_IMAGE_SOURCE_CAMERA_ITEM_NUM + 1) != swscanf_s(str,//読み込みテキストからswscanf_s:文字列から書式付きデータをnoteへ読み込む　戻り値：読み込み要素数　codeとコメント文字列で2が正常
            L"%lu,%[^,]",//%lu:unsigned long,%[^,]：,以外の文字列
            &cnfg_common->img_source_camera,
            note, (unsigned int)(sizeof(note) / sizeof(*note)))) {
            return FALSE;
        }
    }

    // 取込み画像ファイル名
    if (GetPrivateProfileString(INI_SCT_COMMON,
        INI_KEY_COMMON_IMAGE_SOURCE_FILENAME,
        L"", str, sizeof(str) / sizeof(*str),
        file_name) <= 0) {
        return FALSE;
    }
    else {
        wchar_t fname[256] = { 0 };
        if ((INI_KEY_COMMON_IMAGE_SOURCE_FILENAME_ITEM_NUM + 1) != swscanf_s(str,
            L"%[^,],%[^,]",
            fname, (unsigned int)(sizeof(fname) / sizeof(*fname)),
            note, (unsigned int)(sizeof(note) / sizeof(*note)))) {
            return FALSE;
        }
        else {
            cnfg_common->img_source_fname = fname;
            //return=remove(begin,end,target):begine,end間の要素からtargetの要素を削除する returnには、削除後のend更新位置
            cnfg_common->img_source_fname.erase(remove(cnfg_common->img_source_fname.begin(), cnfg_common->img_source_fname.end(), ' '),
                cnfg_common->img_source_fname.end());
        }
    }

    // 画像保存ファイル名
    if (GetPrivateProfileString(INI_SCT_COMMON,
        INI_KEY_COMMON_IMAGE_OUTPUT_FILENAME,
        L"", str, sizeof(str) / sizeof(*str),
        file_name) <= 0) {
        return FALSE;
    }
    else {
        wchar_t fname[256] = { 0 };
        if ((INI_KEY_COMMON_IMAGE_OUTPUT_FILENAME_ITEM_NUM + 1) != swscanf_s(str,
            L"%[^,],%[^,]",
            fname, (unsigned int)(sizeof(fname) / sizeof(*fname)),
            note, (unsigned int)(sizeof(note) / sizeof(*note)))) {
            return FALSE;
        }
        else {
            cnfg_common->img_output_fname = fname;
            cnfg_common->img_output_fname.erase(remove(cnfg_common->img_output_fname.begin(), cnfg_common->img_output_fname.end(), ' '),
                cnfg_common->img_output_fname.end());
        }
    }

    // 画像表示レイアウト
    if (GetPrivateProfileString(INI_SCT_COMMON,
        INI_KEY_COMMON_IMAGE_SCREEN_LAYOUT,
        L"", str, sizeof(str) / sizeof(*str),
        file_name) <= 0) {
        return FALSE;
    }
    else {
        if ((INI_KEY_COMMON_IMAGE_SCREEN_LAYOUT_ITEM_NUM + 1) != swscanf_s(str,
            L"%lu,%lu,%lu,%lu,%[^,]",
            &cnfg_common->img_screen_layout.x0,
            &cnfg_common->img_screen_layout.y0,
            &cnfg_common->img_screen_layout.width,
            &cnfg_common->img_screen_layout.height,
            note, (unsigned int)(sizeof(note) / sizeof(*note)))) {
            return FALSE;
        }
    }

    set_app_config(*cnfg_common);

#pragma endregion SET_CONFIG_COMMON

    //----------------------------------------------------------------------------
    // システム設定
#pragma region SET_CONFIG_SYSTEM
    //PCONFIG_SYSTEM cnfg_system = &m_app_cnfg.system;    // システム設定

    CONFIG_SYSTEM data_system;
    PCONFIG_SYSTEM cnfg_system = &data_system;    // システム設定

    // メインスレッド実行周期
    if (GetPrivateProfileString(INI_SCT_SYSTEM,
        INI_KEY_SYSTEM_CYCLE_TIME,
        L"", str, sizeof(str) / sizeof(*str),
        file_name) <= 0) {
        return FALSE;
    }
    else {
        if ((INI_KEY_SYSTEM_CYCLE_TIME_ITEM_NUM + 1) != swscanf_s(str,
            L"%lu,%[^,]",
            &cnfg_system->cycletime_sys,
            note, (unsigned int)(sizeof(note) / sizeof(*note)))) {
            return FALSE;
        }
    }

    // 機器ID
    if (GetPrivateProfileString(INI_SCT_SYSTEM,
        INI_KEY_SYSTEM_DEVICE_ID,
        L"", str, sizeof(str) / sizeof(*str),
        file_name) <= 0) {
        return FALSE;
    }
    else {
        wchar_t mcc_id[256] = { 0 };
        wchar_t my_id[256] = { 0 };
        if ((INI_KEY_SYSTEM_DEVICE_ID_ITEM_NUM + 1) != swscanf_s(str,
            L"%[^,],%[^,],%[^,]",
            mcc_id, (unsigned int)(sizeof(mcc_id) / sizeof(*mcc_id)),
            my_id, (unsigned int)(sizeof(my_id) / sizeof(*my_id)),
            note, (unsigned int)(sizeof(note) / sizeof(*note)))) {
            return FALSE;
        }
        else {
            cnfg_system->mcc_id = mcc_id;
            cnfg_system->mcc_id.erase(remove(cnfg_system->mcc_id.begin(), cnfg_system->mcc_id.end(), ' '),
                cnfg_system->mcc_id.end());
            cnfg_system->my_id = my_id;
            cnfg_system->my_id.erase(remove(cnfg_system->my_id.begin(), cnfg_system->my_id.end(), ' '),
                cnfg_system->my_id.end());
        }
    }

    set_app_config(*cnfg_system);

#pragma endregion SET_CONFIG_SYSTEM

    //----------------------------------------------------------------------------
    // タスク設定
#pragma region SET_CONFIG_TASK

//    PCONFIG_TASK cnfg_task = &m_app_cnfg.task;  // タスク設定

    CONFIG_TASK data_task;
    PCONFIG_TASK cnfg_task = &data_task;  // タスク設定

    // Environment
    if (GetPrivateProfileString(INI_SCT_TASK,
        INI_KEY_TASK_ENVIRONMENT,
        L"", str, sizeof(str) / sizeof(*str),
        file_name) <= 0) {
        return FALSE;
    }
    else {
        wchar_t name[256] = { 0 };
        wchar_t sname[256] = { 0 };
        if ((INI_KEY_TASK_ITEM_NUM + 1) != swscanf_s(str,
            L"%lu,%[^,],%[^,],%[^,]",
            &cnfg_task->environment.cycletime,
            name, (unsigned int)(sizeof(name) / sizeof(*name)),
            sname, (unsigned int)(sizeof(sname) / sizeof(*sname)),
            note, (unsigned int)(sizeof(note) / sizeof(*note)))) {
            return FALSE;
        }
        else {
            cnfg_task->environment.name = name;
            cnfg_task->environment.name.erase(remove(cnfg_task->environment.name.begin(), cnfg_task->environment.name.end(), ' '),
                cnfg_task->environment.name.end());
            cnfg_task->environment.sname = sname;
            cnfg_task->environment.sname.erase(remove(cnfg_task->environment.sname.begin(), cnfg_task->environment.sname.end(), ' '),
                cnfg_task->environment.sname.end());
        }
    }

    // Policy
    if (GetPrivateProfileString(INI_SCT_TASK,
        INI_KEY_TASK_POLICY,
        L"", str, sizeof(str) / sizeof(*str),
        file_name) <= 0) {
        return FALSE;
    }
    else {
        wchar_t name[256] = { 0 };
        wchar_t sname[256] = { 0 };
        if ((INI_KEY_TASK_ITEM_NUM + 1) != swscanf_s(str,
            L"%lu,%[^,],%[^,],%[^,]",
            &cnfg_task->policy.cycletime,
            name, (unsigned int)(sizeof(name) / sizeof(*name)),
            sname, (unsigned int)(sizeof(sname) / sizeof(*sname)),
            note, (unsigned int)(sizeof(note) / sizeof(*note)))) {
            return FALSE;
        }
        else {
            cnfg_task->policy.name = name;
            cnfg_task->policy.name.erase(remove(cnfg_task->policy.name.begin(), cnfg_task->policy.name.end(), ' '),
                cnfg_task->policy.name.end());
            cnfg_task->policy.sname = sname;
            cnfg_task->policy.sname.erase(remove(cnfg_task->policy.sname.begin(), cnfg_task->policy.sname.end(), ' '),
                cnfg_task->policy.sname.end());
        }
    }

    // Agent
    if (GetPrivateProfileString(INI_SCT_TASK,
        INI_KEY_TASK_AGENT,
        L"", str, sizeof(str) / sizeof(*str),
        file_name) <= 0) {
        return FALSE;
    }
    else {
        wchar_t name[256] = { 0 };
        wchar_t sname[256] = { 0 };
        if ((INI_KEY_TASK_ITEM_NUM + 1) != swscanf_s(str,
            L"%lu,%[^,],%[^,],%[^,]",
            &cnfg_task->agent.cycletime,
            name, (unsigned int)(sizeof(name) / sizeof(*name)),
            sname, (unsigned int)(sizeof(sname) / sizeof(*sname)),
            note, (unsigned int)(sizeof(note) / sizeof(*note)))) {
            return FALSE;
        }
        else {
            cnfg_task->agent.name = name;
            cnfg_task->agent.name.erase(remove(cnfg_task->agent.name.begin(), cnfg_task->agent.name.end(), ' '),
                cnfg_task->agent.name.end());
            cnfg_task->agent.sname = sname;
            cnfg_task->agent.sname.erase(remove(cnfg_task->agent.sname.begin(), cnfg_task->agent.sname.end(), ' '),
                cnfg_task->agent.sname.end());
        }
    }

    // Scada
    if (GetPrivateProfileString(INI_SCT_TASK,
        INI_KEY_TASK_SCADA,
        L"-", str, sizeof(str) / sizeof(*str),
        file_name) <= 0) {
        return FALSE;
    }
    else {
        wchar_t name[256] = { 0 };
        wchar_t sname[256] = { 0 };
        if ((INI_KEY_TASK_ITEM_NUM + 1) != swscanf_s(str,
            L"%lu,%[^,],%[^,],%[^,]",
            &cnfg_task->scada.cycletime,
            name, (unsigned int)(sizeof(name) / sizeof(*name)),
            sname, (unsigned int)(sizeof(sname) / sizeof(*sname)),
            note, (unsigned int)(sizeof(note) / sizeof(*note)))) {
            return FALSE;
        }
        else {
            cnfg_task->scada.name = name;
            cnfg_task->scada.name.erase(remove(cnfg_task->scada.name.begin(), cnfg_task->scada.name.end(), ' '),
                cnfg_task->scada.name.end());
            cnfg_task->scada.sname = sname;
            cnfg_task->scada.sname.erase(remove(cnfg_task->scada.sname.begin(), cnfg_task->scada.sname.end(), ' '),
                cnfg_task->scada.sname.end());
        }
    }

    set_app_config(*cnfg_task);
#pragma endregion SET_CONFIG_TASK

    //----------------------------------------------------------------------------
    // 通信設定
#pragma region SET_CONFIG_ETHERNET
  //  PCONFIG_ETHERNET cnfg_ethernet = &m_app_cnfg.ethernet;  // 通信設定

    CONFIG_ETHERNET data_ethernet;
    PCONFIG_ETHERNET cnfg_ethernet = &data_ethernet;  // 通信設定

    // Local設定
    if (GetPrivateProfileString(INI_SCT_ETHERNET,
        INI_KEY_ETHERNET_LOCAL,
        L"", str, sizeof(str) / sizeof(*str),
        file_name) <= 0) {
        return FALSE;
    }
    else {
        wchar_t ipaddress[256] = { 0 };
        if ((INI_KEY_ETHERNET_ITEM_NUM + 1) != swscanf_s(str,
            L"%[^,],%lu,%[^,]",
            ipaddress, (unsigned int)(sizeof(ipaddress) / sizeof(*ipaddress)),
            &cnfg_ethernet->local.port,
            note, (unsigned int)(sizeof(note) / sizeof(*note)))) {
            return FALSE;
        }
        else {
            cnfg_ethernet->local.ipaddress = ipaddress;
            cnfg_ethernet->local.ipaddress.erase(remove(cnfg_ethernet->local.ipaddress.begin(), cnfg_ethernet->local.ipaddress.end(), ' '),
                cnfg_ethernet->local.ipaddress.end());
        }
    }

    // Remote設定
    if (GetPrivateProfileString(INI_SCT_ETHERNET,
        INI_KEY_ETHERNET_REMOTE,
        L"", str, sizeof(str) / sizeof(*str),
        file_name) <= 0) {
        return FALSE;
    }
    else {
        wchar_t ipaddress[256] = { 0 };
        if ((INI_KEY_ETHERNET_ITEM_NUM + 1) != swscanf_s(str,
            L"%[^,],%lu,%[^,]",
            ipaddress, (unsigned int)(sizeof(ipaddress) / sizeof(*ipaddress)),
            &cnfg_ethernet->remote.port,
            note, (unsigned int)(sizeof(note) / sizeof(*note)))) {
            return FALSE;
        }
        else {
            cnfg_ethernet->remote.ipaddress = ipaddress;
            cnfg_ethernet->remote.ipaddress.erase(remove(cnfg_ethernet->remote.ipaddress.begin(), cnfg_ethernet->remote.ipaddress.end(), ' '),
                cnfg_ethernet->remote.ipaddress.end());
        }
    }

    set_app_config(*cnfg_ethernet);

#pragma endregion SET_CONFIG_ETHERNET

    //----------------------------------------------------------------------------
    // カメラ設定
#pragma region SET_CONFIG_CAMERA
    // カメラの基本設定
    //PCONFIG_CAMERA cnfg_camera = gp_cnfg_camera;    // カメラ設定

    CONFIG_CAMERA data_camera;    // カメラ設定
    PCONFIG_CAMERA cnfg_camera = &data_camera;    // カメラ設定

    // IPアドレス
    if (GetPrivateProfileString(INI_SCT_CAMERA,
        INI_KEY_CAMERA_BASIS_IP_ADDRESS,
        L"", str, sizeof(str) / sizeof(*str),
        file_name) <= 0) {
        return FALSE;
    }
    else {
        wchar_t ipaddress[256] = { 0 };
        if ((INI_KEY_CAMERA_BASIS_IP_ADDRESS_ITEM_NUM + 1) != swscanf_s(str,
            L"%[^,],%[^,]",
            ipaddress, (unsigned int)(sizeof(ipaddress) / sizeof(*ipaddress)),
            note, (unsigned int)(sizeof(note) / sizeof(*note)))) {
            return FALSE;
        }
        else {
            cnfg_camera->basis.ipaddress = ipaddress;
            cnfg_camera->basis.ipaddress.erase(remove(cnfg_camera->basis.ipaddress.begin(), cnfg_camera->basis.ipaddress.end(), ' '),
                cnfg_camera->basis.ipaddress.end());
        }
    }
    // ROI(領域)
    if (GetPrivateProfileString(INI_SCT_CAMERA,
        INI_KEY_CAMERA_BASIS_ROI,
        L"-", str, sizeof(str) / sizeof(*str),
        file_name) <= 0) {
        return FALSE;
    }
    else {
        if ((INI_KEY_CAMERA_BASIS_ROI_ITEM_NUM + 1) != swscanf_s(str,
            L"%lu,%lu,%lu,%lu,%[^,]",
            &cnfg_camera->basis.roi[static_cast<uint32_t>(ENUM_AXIS::X)].offset,
            &cnfg_camera->basis.roi[static_cast<uint32_t>(ENUM_AXIS::Y)].offset,
            &cnfg_camera->basis.roi[static_cast<uint32_t>(ENUM_AXIS::X)].size,
            &cnfg_camera->basis.roi[static_cast<uint32_t>(ENUM_AXIS::Y)].size,
            note, (unsigned int)(sizeof(note) / sizeof(*note)))) {
            return FALSE;
        }
    }
    // ドライバが受け取るパケットの最大サイズ
    if (GetPrivateProfileString(INI_SCT_CAMERA,
        INI_KEY_CAMERA_BASIS_PACKET_SIZE,
        L"", str, sizeof(str) / sizeof(*str),
        file_name) <= 0) {
        return FALSE;
    }
    else {
        if ((INI_KEY_CAMERA_BASIS_PACKET_SIZE_ITEM_NUM + 1) != swscanf_s(str,
            L"%lu,%[^,]",
            &cnfg_camera->basis.packetsize,
            note, (unsigned int)(sizeof(note) / sizeof(*note)))) {
            return FALSE;
        }
    }
    // フレームレート
    if (GetPrivateProfileString(INI_SCT_CAMERA,
        INI_KEY_CAMERA_BASIS_FRAME_RATE,
        L"", str, sizeof(str) / sizeof(*str),
        file_name) <= 0) {
        return FALSE;
    }
    else {
        if ((INI_KEY_CAMERA_BASIS_FRAME_RATE_ITEM_NUM + 1) != swscanf_s(str,
            L"%lf,%[^,]",
            &cnfg_camera->basis.framerate,
            note, (unsigned int)(sizeof(note) / sizeof(*note)))) {
            return FALSE;
        }
    }
    // 黒レベル設定
    if (GetPrivateProfileString(INI_SCT_CAMERA,
        INI_KEY_CAMERA_BASIS_BLACKLEVEL,
        L"", str, sizeof(str) / sizeof(*str),
        file_name) <= 0) {
        return FALSE;
    }
    else {
        if ((INI_KEY_CAMERA_BASIS_BLACKLEVEL_ITEM_NUM + 1) != swscanf_s(str,
            L"%lf,%[^,]",
            &cnfg_camera->basis.blacklevel,
            note, (unsigned int)(sizeof(note) / sizeof(*note)))) {
            return FALSE;
        }
    }
    // ガンマ補正
    if (GetPrivateProfileString(INI_SCT_CAMERA,
        INI_KEY_CAMERA_BASIS_GAMMA,
        L"", str, sizeof(str) / sizeof(*str),
        file_name) <= 0) {
        return FALSE;
    }
    else {
        if ((INI_KEY_CAMERA_BASIS_GAMMA_ITEM_NUM + 1) != swscanf_s(str,
            L"%lf,%[^,]",
            &cnfg_camera->basis.gamma,
            note, (unsigned int)(sizeof(note) / sizeof(*note)))) {
            return FALSE;
        }
    }
    // ホワイトバランスゲイン設定
    if (GetPrivateProfileString(INI_SCT_CAMERA,
        INI_KEY_CAMERA_BASIS_WBALANCE_AUTO,
        L"-", str, sizeof(str) / sizeof(*str),
        file_name) <= 0) {
        return FALSE;
    }
    else {
        if ((INI_KEY_CAMERA_BASIS_WBALANCE_AUTO_ITEM_NUM + 1) != swscanf_s(str,
            L"%lu,%lf,%lf,%[^,]",
            &cnfg_camera->basis.wb.wb_auto,
            &cnfg_camera->basis.wb.wb_ratio_red,
            &cnfg_camera->basis.wb.wb_ratio_blue,
            note, (unsigned int)(sizeof(note) / sizeof(*note)))) {
            return FALSE;
        }
    }
    // 視野角
    if (GetPrivateProfileString(INI_SCT_CAMERA,
        INI_KEY_CAMERA_BASIS_VIEW_ANGLE,
        L"-", str, sizeof(str) / sizeof(*str),
        file_name) <= 0) {
        return FALSE;
    }
    else {
        if ((INI_KEY_CAMERA_BASIS_VIEW_ANGLE_ITEM_NUM + 1) != swscanf_s(str,
            L"%lf,%lf,%[^,]",
            &cnfg_camera->basis.viewangle[static_cast<uint32_t>(ENUM_AXIS::X)],
            &cnfg_camera->basis.viewangle[static_cast<uint32_t>(ENUM_AXIS::Y)],
            note, (unsigned int)(sizeof(note) / sizeof(*note)))) {
            return FALSE;
        }
    }

    // 輝度コントロール設定(ゲイン)
    // ゲイン設定
    if (GetPrivateProfileString(INI_SCT_CAMERA,
        INI_KEY_CAMERA_GAIN,
        L"-", str, sizeof(str) / sizeof(*str),
        file_name) <= 0) {
        return FALSE;
    }
    else {
        if ((INI_KEY_CAMERA_GAIN_ITEM_NUM + 1) != swscanf_s(str,
            L"%lf,%lf,%lf,%[^,]",
            &cnfg_camera->gain.val,
            &cnfg_camera->gain.val_min,
            &cnfg_camera->gain.val_max,
            note, (unsigned int)(sizeof(note) / sizeof(*note)))) {
            return FALSE;
        }
    }
    // 自動ゲイン設定
    if (GetPrivateProfileString(INI_SCT_CAMERA,
        INI_KEY_CAMERA_GAIN_AUTO,
        L"-", str, sizeof(str) / sizeof(*str),
        file_name) <= 0) {
        return FALSE;
    }
    else {
        if ((INI_KEY_CAMERA_GAIN_AUTO_ITEM_NUM + 1) != swscanf_s(str,
            L"%lu,%lf,%[^,]",
            &cnfg_camera->gain.auto_control,
            &cnfg_camera->gain.auto_rate,
            note, (unsigned int)(sizeof(note) / sizeof(*note)))) {
            return FALSE;
        }
    }
    // 自動ゲイン判定輝度(開始上限)
    if (GetPrivateProfileString(INI_SCT_CAMERA,
        INI_KEY_CAMERA_GAIN_AUTO_START,
        L"-", str, sizeof(str) / sizeof(*str),
        file_name) <= 0) {
        return FALSE;
    }
    else {
        if ((INI_KEY_CAMERA_GAIN_AUTO_START_ITEM_NUM + 1) != swscanf_s(str,
            L"%lf,%lf,%[^,]",
            &cnfg_camera->gain.auto_start_h,
            &cnfg_camera->gain.auto_start_l,
            note, (unsigned int)(sizeof(note) / sizeof(*note)))) {
            return FALSE;
        }
    }
    // 自動ゲイン判定輝度(停止上限)
    if (GetPrivateProfileString(INI_SCT_CAMERA,
        INI_KEY_CAMERA_GAIN_AUTO_STOP,
        L"-", str, sizeof(str) / sizeof(*str),
        file_name) <= 0) {
        return FALSE;
    }
    else {
        if ((INI_KEY_CAMERA_GAIN_AUTO_STOP_ITEM_NUM + 1) != swscanf_s(str,
            L"%lf,%lf,%[^,]",
            &cnfg_camera->gain.auto_stop_h,
            &cnfg_camera->gain.auto_stop_l,
            note, (unsigned int)(sizeof(note) / sizeof(*note)))) {
            return FALSE;
        }
    }

    // 輝度コントロール設定(露光時間)
    // 露光時間設定
    if (GetPrivateProfileString(INI_SCT_CAMERA,
        INI_KEY_CAMERA_EXPSTIME,
        L"-", str, sizeof(str) / sizeof(*str),
        file_name) <= 0) {
        return FALSE;
    }
    else {
        if ((INI_KEY_CAMERA_EXPSTIME_ITEM_NUM + 1) != swscanf_s(str,
            L"%lf,%lf,%lf,%[^,]",
            &cnfg_camera->expstime.val,
            &cnfg_camera->expstime.val_min,
            &cnfg_camera->expstime.val_max,
            note, (unsigned int)(sizeof(note) / sizeof(*note)))) {
            return FALSE;
        }
    }
    // 自動露光設定
    if (GetPrivateProfileString(INI_SCT_CAMERA,
        INI_KEY_CAMERA_EXPSTIME_AUTO,
        L"-", str, sizeof(str) / sizeof(*str),
        file_name) <= 0) {
        return FALSE;
    }
    else {
        if ((INI_KEY_CAMERA_EXPSTIME_AUTO_ITEM_NUM + 1) != swscanf_s(str,
            L"%lu,%lf,%[^,]",
            &cnfg_camera->expstime.auto_control,
            &cnfg_camera->expstime.auto_rate,
            note, (unsigned int)(sizeof(note) / sizeof(*note)))) {
            return FALSE;
        }
    }
    // 自動露光判定輝度(開始上限)
    if (GetPrivateProfileString(INI_SCT_CAMERA,
        INI_KEY_CAMERA_EXPSTIME_AUTO_START,
        L"-", str, sizeof(str) / sizeof(*str),
        file_name) <= 0) {
        return FALSE;
    }
    else {
        if ((INI_KEY_CAMERA_EXPSTIME_AUTO_START_ITEM_NUM + 1) != swscanf_s(str,
            L"%lf,%lf,%[^,]",
            &cnfg_camera->expstime.auto_start_h,
            &cnfg_camera->expstime.auto_start_l,
            note, (unsigned int)(sizeof(note) / sizeof(*note)))) {
            return FALSE;
        }
    }
    // 自動露光判定輝度(停止上限)
    if (GetPrivateProfileString(INI_SCT_CAMERA,
        INI_KEY_CAMERA_EXPSTIME_AUTO_STOP,
        L"-", str, sizeof(str) / sizeof(*str),
        file_name) <= 0) {
        return FALSE;
    }
    else {
        if ((INI_KEY_CAMERA_EXPSTIME_AUTO_STOP_ITEM_NUM + 1) != swscanf_s(str,
            L"%lf,%lf,%[^,]",
            &cnfg_camera->expstime.auto_stop_h,
            &cnfg_camera->expstime.auto_stop_l,
            note, (unsigned int)(sizeof(note) / sizeof(*note)))) {
            return FALSE;
        }
    }

    // カメラの異常判定設定
    if (GetPrivateProfileString(INI_SCT_CAMERA,
        INI_KEY_CAMERA_ERROR,
        L"", str, sizeof(str) / sizeof(*str),
        file_name) <= 0) {
        return FALSE;
    }
    else {
        if ((INI_KEY_CAMERA_ERROR_ITEM_NUM + 1) != swscanf_s(str,
            L"%lf,%lu,%[^,]",
            &cnfg_camera->error.framedrop,
            &cnfg_camera->error.errtime,
            note, (unsigned int)(sizeof(note) / sizeof(*note)))) {
            return FALSE;
        }
    }

    // カメラの起動設定
    if (GetPrivateProfileString(INI_SCT_CAMERA,
        INI_KEY_CAMERA_BOOT,
        L"-", str, sizeof(str) / sizeof(*str),
        file_name) <= 0) {
        return FALSE;
    }
    else {
        if ((INI_KEY_CAMERA_BOOT_ITEM_NUM + 1) != swscanf_s(str,
            L"%lu,%lu,%lu,%[^,]",
            &cnfg_camera->boot.bootwaittimer,
            &cnfg_camera->boot.rebootcount,
            &cnfg_camera->boot.rebootingtimer,
            note, (unsigned int)(sizeof(note) / sizeof(*note)))) {
            return FALSE;
        }
    }

    set_app_config(*cnfg_camera);
#pragma endregion SET_CONFIG_CAMERA

    //----------------------------------------------------------------------------
    // 傾斜計設定
#pragma region SET_CONFIG_TILTMETER
    //PCONFIG_TILTMETER cnfg_tiltmeter = &m_app_cnfg.tiltmeter;   // 傾斜計設定
    CONFIG_TILTMETER data_tiltmeter;   // 傾斜計設定
    PCONFIG_TILTMETER cnfg_tiltmeter = &data_tiltmeter;   // 傾斜計設定

    // デバイス設定
    if (GetPrivateProfileString(INI_SCT_TILTMETER,
        INI_KEY_TILTMETER_CANOPEN,
        L"", str, sizeof(str) / sizeof(*str),
        file_name) <= 0) {
        return FALSE;
    }
    else {
        wchar_t setup_fname[256] = { 0 };
        if ((INI_KEY_TILTMETER_CANOPEN_ITEM_NUM + 1) != swscanf_s(str,
            L"%[^,],%hhu,%hhx,%[^,]",
            setup_fname, (unsigned int)(sizeof(setup_fname) / sizeof(*setup_fname)),
            &cnfg_tiltmeter->canopen.port,
            &cnfg_tiltmeter->canopen.node_id,
            note, (unsigned int)(sizeof(note) / sizeof(*note)))) {
            return FALSE;
        }
        else {
            cnfg_tiltmeter->canopen.setup_fname = setup_fname;
            cnfg_tiltmeter->canopen.setup_fname.erase(remove(cnfg_tiltmeter->canopen.setup_fname.begin(), cnfg_tiltmeter->canopen.setup_fname.end(), ' '),
                cnfg_tiltmeter->canopen.setup_fname.end());
        }
    }
    // 傾斜計フィルター
    if (GetPrivateProfileString(INI_SCT_TILTMETER,
        INI_KEY_TILTMETER_FILTER,
        L"", str, sizeof(str) / sizeof(*str),
        file_name) <= 0) {
        return FALSE;
    }
    else {
        wchar_t setup_fname[256] = { 0 };
        if ((INI_KEY_TILTMETER_FILTER_ITEM_NUM + 1) != swscanf_s(str,
            L"%hu,%hu,%[^,]",
            &cnfg_tiltmeter->dr_filter,
            &cnfg_tiltmeter->ma_filter,
            note, (unsigned int)(sizeof(note) / sizeof(*note)))) {
            return FALSE;
        }
    }
    // 傾斜角速度フィルター時定数
    if (GetPrivateProfileString(INI_SCT_TILTMETER,
        INI_KEY_TILTMETER_SPEED_TC,
        L"", str, sizeof(str) / sizeof(*str),
        file_name) <= 0) {
        return FALSE;
    }
    else {
        if ((INI_KEY_TILTMETER_SPEED_TC_ITEM_NUM + 1) != swscanf_s(str,
            L"%lf,%[^,]",
            &cnfg_tiltmeter->tiltspdfilter,
            note, (unsigned int)(sizeof(note) / sizeof(*note)))) {
            return FALSE;
        }
    }

    set_app_config(*cnfg_tiltmeter);

#pragma endregion SET_CONFIG_TILTMETER

    //----------------------------------------------------------------------------
    // 取付寸法設定
#pragma region SET_CONFIG_MOUNTING
   // PCONFIG_MOUNTING cnfg_mounting = gp_cnfg_mounting;  // 取付寸法設定

    CONFIG_MOUNTING data_mounting;  // 取付寸法設定
    PCONFIG_MOUNTING cnfg_mounting = &data_mounting;  // 取付寸法設定

    // カメラ回転軸距離L0[mm]
    if (GetPrivateProfileString(INI_SCT_MOUNTING,
        INI_KEY_MOUNTING_OFFSET_L0,
        L"-", str, sizeof(str) / sizeof(*str),
        file_name) <= 0) {
        return FALSE;
    }
    else {
        if ((INI_KEY_MOUNTING_OFFSET_ITEM_NUM + 1) != swscanf_s(str,
            L"%lf,%lf,%[^,]",
            &cnfg_mounting->offset[static_cast<uint32_t>(ENUM_AXIS::X)].l0,
            &cnfg_mounting->offset[static_cast<uint32_t>(ENUM_AXIS::Y)].l0,
            note, (unsigned int)(sizeof(note) / sizeof(*note)))) {
            return FALSE;
        }
    }
    // カメラ取付角度θ0(*10^6)[rad]
    if (GetPrivateProfileString(INI_SCT_MOUNTING,
        INI_KEY_MOUNTING_OFFSET_T0,
        L"-", str, sizeof(str) / sizeof(*str),
        file_name) <= 0) {
        return FALSE;
    }
    else {
        if ((INI_KEY_MOUNTING_OFFSET_ITEM_NUM + 1) != swscanf_s(str,
            L"%lf,%lf,%[^,]",
            &cnfg_mounting->offset[static_cast<uint32_t>(ENUM_AXIS::X)].t0,
            &cnfg_mounting->offset[static_cast<uint32_t>(ENUM_AXIS::Y)].t0,
            note, (unsigned int)(sizeof(note) / sizeof(*note)))) {
            return FALSE;
        }
    }
    // カメラ設置角度φ0(*10^6)[rad]
    if (GetPrivateProfileString(INI_SCT_MOUNTING,
        INI_KEY_MOUNTING_OFFSET_P0,
        L"-", str, sizeof(str) / sizeof(*str),
        file_name) <= 0) {
        return FALSE;
    }
    else {
        if ((INI_KEY_MOUNTING_OFFSET_ITEM_NUM + 1) != swscanf_s(str,
            L"%lf,%lf,%[^,]",
            &cnfg_mounting->offset[static_cast<uint32_t>(ENUM_AXIS::X)].p0,
            &cnfg_mounting->offset[static_cast<uint32_t>(ENUM_AXIS::Y)].p0,
            note, (unsigned int)(sizeof(note) / sizeof(*note)))) {
            return FALSE;
        }
    }

    set_app_config(*cnfg_mounting);
#pragma endregion SET_CONFIG_MOUNTING

    //----------------------------------------------------------------------------
    // 画像処理条件設定
#pragma region SET_CONFIG_IMGPROC
    //PCONFIG_IMGPROC cnfg_imgprc = gp_cnfg_imgprc;

    CONFIG_IMGPROC data_imgprc;
    PCONFIG_IMGPROC cnfg_imgprc = &data_imgprc;

    // 画像ROI設定
    if (GetPrivateProfileString(INI_SCT_IMGPROC,
        INI_KEY_IMGPROC_ROI,
        L"-", str, sizeof(str) / sizeof(*str),
        file_name) <= 0) {
        return FALSE;
    }
    else {
        if ((INI_KEY_IMGPROC_ROI_ITEM_NUM + 1) != swscanf_s(str,
            L"%lu,%lf,%[^,]",
            &cnfg_imgprc->roi.valid,
            &cnfg_imgprc->roi.scale,
            note, (unsigned int)(sizeof(note) / sizeof(*note)))) {
            return FALSE;
        }
    }
    // マスク画像選択
    if (GetPrivateProfileString(INI_SCT_IMGPROC,
        INI_KEY_IMGPROC_MASK,
        L"", str, sizeof(str) / sizeof(*str),
        file_name) <= 0) {
        return FALSE;
    }
    else {
        uint32_t mask = 0;
        if ((INI_KEY_IMGPROC_MASK_ITEM_NUM + 1) != swscanf_s(str,
            L"%lu,%[^,]",
            &mask,
            note, (unsigned int)(sizeof(note) / sizeof(*note)))) {
            return FALSE;
        }
        switch (mask) {
        case static_cast<uint32_t>(ENUM_IMAGE_MASK_SEL::IMAGE_1):
            cnfg_imgprc->mask[static_cast<uint32_t>(ENUM_IMAGE_MASK::MASK_1)].valid = TRUE;
            cnfg_imgprc->mask[static_cast<uint32_t>(ENUM_IMAGE_MASK::MASK_2)].valid = FALSE;
            break;
        case static_cast<uint32_t>(ENUM_IMAGE_MASK_SEL::IMAGE_2):
            cnfg_imgprc->mask[static_cast<uint32_t>(ENUM_IMAGE_MASK::MASK_1)].valid = FALSE;
            cnfg_imgprc->mask[static_cast<uint32_t>(ENUM_IMAGE_MASK::MASK_2)].valid = TRUE;
            break;
        default:
            cnfg_imgprc->mask[static_cast<uint32_t>(ENUM_IMAGE_MASK::MASK_1)].valid = TRUE;
            cnfg_imgprc->mask[static_cast<uint32_t>(ENUM_IMAGE_MASK::MASK_2)].valid = TRUE;
            break;
        }
    }
    // 画像1マスクH
    if (GetPrivateProfileString(INI_SCT_IMGPROC,
        INI_KEY_IMGPROC_MASK1_H,
        L"-", str, sizeof(str) / sizeof(*str),
        file_name) <= 0) {
        return FALSE;
    }
    else {
        if ((INI_KEY_IMGPROC_MASK1_ITEM_NUM + 1) != swscanf_s(str,
            L"%lu,%lu,%[^,]",
            &cnfg_imgprc->mask[static_cast<uint32_t>(ENUM_IMAGE_MASK::MASK_1)].hsv_l[static_cast<uint32_t>(ENUM_HSV_MODEL::H)],
            &cnfg_imgprc->mask[static_cast<uint32_t>(ENUM_IMAGE_MASK::MASK_1)].hsv_u[static_cast<uint32_t>(ENUM_HSV_MODEL::H)],
            note, (unsigned int)(sizeof(note) / sizeof(*note)))) {
            return FALSE;
        }
    }
    // 画像1マスクS
    if (GetPrivateProfileString(INI_SCT_IMGPROC,
        INI_KEY_IMGPROC_MASK1_S,
        L"-", str, sizeof(str) / sizeof(*str),
        file_name) <= 0) {
        return FALSE;
    }
    else {
        if ((INI_KEY_IMGPROC_MASK1_ITEM_NUM + 1) != swscanf_s(str,
            L"%lu,%lu,%[^,]",
            &cnfg_imgprc->mask[static_cast<uint32_t>(ENUM_IMAGE_MASK::MASK_1)].hsv_l[static_cast<uint32_t>(ENUM_HSV_MODEL::S)],
            &cnfg_imgprc->mask[static_cast<uint32_t>(ENUM_IMAGE_MASK::MASK_1)].hsv_u[static_cast<uint32_t>(ENUM_HSV_MODEL::S)],
            note, (unsigned int)(sizeof(note) / sizeof(*note)))) {
            return FALSE;
        }
    }
    // 画像1マスクV
    if (GetPrivateProfileString(INI_SCT_IMGPROC,
        INI_KEY_IMGPROC_MASK1_V,
        L"-", str, sizeof(str) / sizeof(*str),
        file_name) <= 0) {
        return FALSE;
    }
    else {
        if ((INI_KEY_IMGPROC_MASK1_ITEM_NUM + 1) != swscanf_s(str,
            L"%lu,%lu,%[^,]",
            &cnfg_imgprc->mask[static_cast<uint32_t>(ENUM_IMAGE_MASK::MASK_1)].hsv_l[static_cast<uint32_t>(ENUM_HSV_MODEL::V)],
            &cnfg_imgprc->mask[static_cast<uint32_t>(ENUM_IMAGE_MASK::MASK_1)].hsv_u[static_cast<uint32_t>(ENUM_HSV_MODEL::V)],
            note, (unsigned int)(sizeof(note) / sizeof(*note)))) {
            return FALSE;
        }
    }
    // 画像2マスクH
    if (GetPrivateProfileString(INI_SCT_IMGPROC,
        INI_KEY_IMGPROC_MASK2_H,
        L"-", str, sizeof(str) / sizeof(*str),
        file_name) <= 0) {
        return FALSE;
    }
    else {
        if ((INI_KEY_IMGPROC_MASK2_ITEM_NUM + 1) != swscanf_s(str,
            L"%lu,%lu,%[^,]",
            &cnfg_imgprc->mask[static_cast<uint32_t>(ENUM_IMAGE_MASK::MASK_2)].hsv_l[static_cast<uint32_t>(ENUM_HSV_MODEL::H)],
            &cnfg_imgprc->mask[static_cast<uint32_t>(ENUM_IMAGE_MASK::MASK_2)].hsv_u[static_cast<uint32_t>(ENUM_HSV_MODEL::H)],
            note, (unsigned int)(sizeof(note) / sizeof(*note)))) {
            return FALSE;
        }
    }
    // 画像2マスクS
    if (GetPrivateProfileString(INI_SCT_IMGPROC,
        INI_KEY_IMGPROC_MASK2_S,
        L"-", str, sizeof(str) / sizeof(*str),
        file_name) <= 0) {
        return FALSE;
    }
    else {
        if ((INI_KEY_IMGPROC_MASK2_ITEM_NUM + 1) != swscanf_s(str,
            L"%lu,%lu,%[^,]",
            &cnfg_imgprc->mask[static_cast<uint32_t>(ENUM_IMAGE_MASK::MASK_2)].hsv_l[static_cast<uint32_t>(ENUM_HSV_MODEL::S)],
            &cnfg_imgprc->mask[static_cast<uint32_t>(ENUM_IMAGE_MASK::MASK_2)].hsv_u[static_cast<uint32_t>(ENUM_HSV_MODEL::S)],
            note, (unsigned int)(sizeof(note) / sizeof(*note)))) {
            return FALSE;
        }
    }
    // 画像2マスクV
    if (GetPrivateProfileString(INI_SCT_IMGPROC,
        INI_KEY_IMGPROC_MASK2_V,
        L"-", str, sizeof(str) / sizeof(*str),
        file_name) <= 0) {
        return FALSE;
    }
    else {
        if ((INI_KEY_IMGPROC_MASK2_ITEM_NUM + 1) != swscanf_s(str,
            L"%lu,%lu,%[^,]",
            &cnfg_imgprc->mask[static_cast<uint32_t>(ENUM_IMAGE_MASK::MASK_2)].hsv_l[static_cast<uint32_t>(ENUM_HSV_MODEL::V)],
            &cnfg_imgprc->mask[static_cast<uint32_t>(ENUM_IMAGE_MASK::MASK_2)].hsv_u[static_cast<uint32_t>(ENUM_HSV_MODEL::V)],
            note, (unsigned int)(sizeof(note) / sizeof(*note)))) {
            return FALSE;
        }
    }
    // ゴマ塩ノイズフィルター
    if (GetPrivateProfileString(INI_SCT_IMGPROC,
        INI_KEY_IMGPROC_FILTER1,
        L"-", str, sizeof(str) / sizeof(*str),
        file_name) <= 0) {
        return FALSE;
    }
    else {
        if ((INI_KEY_IMGPROC_FILTER1_ITEM_NUM + 1) != swscanf_s(str,
            L"%lu,%lu,%[^,]",
            &cnfg_imgprc->filter[static_cast<uint32_t>(ENUM_NOISE_FILTER::FILTER_1)].type,
            &cnfg_imgprc->filter[static_cast<uint32_t>(ENUM_NOISE_FILTER::FILTER_1)].val,
            note, (unsigned int)(sizeof(note) / sizeof(*note)))) {
            return FALSE;
        }
    }
    // 穴埋めノイズフィルター
    if (GetPrivateProfileString(INI_SCT_IMGPROC,
        INI_KEY_IMGPROC_FILTER2,
        L"-", str, sizeof(str) / sizeof(*str),
        file_name) <= 0) {
        return FALSE;
    }
    else {
        if ((INI_KEY_IMGPROC_FILTER2_ITEM_NUM + 1) != swscanf_s(str,
            L"%lu,%lu,%[^,]",
            &cnfg_imgprc->filter[static_cast<uint32_t>(ENUM_NOISE_FILTER::FILTER_2)].type,
            &cnfg_imgprc->filter[static_cast<uint32_t>(ENUM_NOISE_FILTER::FILTER_2)].val,
            note, (unsigned int)(sizeof(note) / sizeof(*note)))) {
            return FALSE;
        }
    }
    // ターゲット検出アルゴリズム
    if (GetPrivateProfileString(INI_SCT_IMGPROC,
        INI_KEY_IMGPROC_PROC,
        L"", str, sizeof(str) / sizeof(*str),
        file_name) <= 0) {
        return FALSE;
    }
    else {
        if ((INI_KEY_IMGPROC_PROC_ITEM_NUM + 1) != swscanf_s(str,
            L"%lu,%[^,]",
            &cnfg_imgprc->imgprc,
            note, (unsigned int)(sizeof(note) / sizeof(*note)))) {
            return FALSE;
        }
    }
    // 振れ速度フィルター時定数
    if (GetPrivateProfileString(INI_SCT_IMGPROC,
        INI_KEY_IMGPROC_SWAY_SPEED_TC,
        L"", str, sizeof(str) / sizeof(*str),
        file_name) <= 0) {
        return FALSE;
    }
    else {
        if ((INI_KEY_IMGPROC_SWAY_SPEED_TC_ITEM_NUM + 1) != swscanf_s(str,
            L"%lf,%[^,]",
            &cnfg_imgprc->swayspdfilter,
            note, (unsigned int)(sizeof(note) / sizeof(*note)))) {
            return FALSE;
        }
    }
    // 振れゼロ点設定
    if (GetPrivateProfileString(INI_SCT_IMGPROC,
        INI_KEY_IMGPROC_SWAY_ZERO_SET,
        L"", str, sizeof(str) / sizeof(*str),
        file_name) <= 0) {
        return FALSE;
    }
    else {
        if ((INI_KEY_IMGPROC_SWAY_ZERO_SET_ITEM_NUM + 1) != swscanf_s(str,
            L"%lu,%[^,]",
            &cnfg_imgprc->swayzeroset_time,
            note, (unsigned int)(sizeof(note) / sizeof(*note)))) {
            return FALSE;
        }
    }

    set_app_config(*cnfg_imgprc);
#pragma endregion SET_CONFIG_IMGPROC
#endif
    return TRUE;
}

/// @brief 共通設定書込み
/// @param
/// @return 
/// @note 
BOOL CSwayShared::set_app_config(CONFIG_COMMON data)
{
    PCONFIG_COMMON cnfg_data = gp_cnfg_common;                          // 共通設定

    cnfg_data->img_source_camera = data.img_source_camera;              // カメラ画像取込み(カメラ[1]/画像ファイル[0])
    cnfg_data->img_source_fname = data.img_source_fname;                // 取込み画像ファイル名CMN_IMAGE_SOURCE_CAMERA=0のときの画像)
    cnfg_data->img_output_fname = data.img_output_fname;                // 画像保存ファイル名

    cnfg_data->img_screen_layout.x0 = data.img_screen_layout.x0;        // 原点座標X
    cnfg_data->img_screen_layout.y0 = data.img_screen_layout.y0;        // 原点座標Y
    cnfg_data->img_screen_layout.width = data.img_screen_layout.width;     // 横幅サイズ
    cnfg_data->img_screen_layout.height = data.img_screen_layout.height;    // 高さサイズ

    return TRUE;
}

/// @brief 共通設定読込み
/// @param
/// @return 
/// @note 
BOOL CSwayShared::get_app_config(PCONFIG_COMMON data)
{
    if (data == NULL) {
        return FALSE;
    }

    PCONFIG_COMMON cnfg_data = gp_cnfg_common;  // 共通設定

    data->img_source_camera = cnfg_data->img_source_camera; // カメラ画像取込み(カメラ[1]/画像ファイル[0])
    data->img_source_fname = cnfg_data->img_source_fname;  // 取込み画像ファイル名CMN_IMAGE_SOURCE_CAMERA=0のときの画像)
    data->img_output_fname = cnfg_data->img_output_fname;  // 画像保存ファイル名

    data->img_screen_layout.x0 = cnfg_data->img_screen_layout.x0;       // 原点座標X
    data->img_screen_layout.y0 = cnfg_data->img_screen_layout.y0;       // 原点座標Y
    data->img_screen_layout.width = cnfg_data->img_screen_layout.width;    // 横幅サイズ
    data->img_screen_layout.height = cnfg_data->img_screen_layout.height;   // 高さサイズ

    return TRUE;
}

/// @brief カメラ設定書込み
/// @param
/// @return 
/// @note 
BOOL CSwayShared::set_app_config(CONFIG_CAMERA data)
{
    PCONFIG_CAMERA cnfg_data = gp_cnfg_camera;  // カメラ設定

    // カメラの基本設定
    cnfg_data->basis.ipaddress = data.basis.ipaddress;  // IPアドレス
    for (int idx = 0; idx < static_cast<uint32_t>(ENUM_AXIS::E_MAX); idx++) {
        cnfg_data->basis.roi[idx].offset = data.basis.roi[idx].offset;  // ROI(領域)の開始位置
        cnfg_data->basis.roi[idx].size = data.basis.roi[idx].size;    // ROI(領域)のサイズ
    }
    cnfg_data->basis.packetsize = data.basis.packetsize;          // ドライバが受け取るパケットの最大サイズ(通常は0を指定)[byte]
    cnfg_data->basis.framerate = data.basis.framerate;           // フレームレート[fps]
    cnfg_data->basis.blacklevel = data.basis.blacklevel;          // 黒レベル設定
    cnfg_data->basis.gamma = data.basis.gamma;               // ガンマ補正
    cnfg_data->basis.wb.wb_auto = data.basis.wb.wb_auto;          // ホワイトバランスゲイン自動調整モー設定(自動調整OFF[1]/連続自動調整[2]/一回のみ自動調整[3])
    cnfg_data->basis.wb.wb_ratio_red = data.basis.wb.wb_ratio_red;     // ホワイトバランスゲイン設定(Red Gain)
    cnfg_data->basis.wb.wb_ratio_blue = data.basis.wb.wb_ratio_blue;    // ホワイトバランスゲイン設定(Blue Gain)
    for (int idx = 0; idx < static_cast<uint32_t>(ENUM_AXIS::E_MAX); idx++) {
        cnfg_data->basis.viewangle[idx] = data.basis.viewangle[idx];    // 視野角[deg]
    }

    // 輝度コントロール設定(ゲイン)
    cnfg_data->gain.val = data.gain.val;           // 設定値
    cnfg_data->gain.val_min = data.gain.val_min;       // 最小値
    cnfg_data->gain.val_max = data.gain.val_max;       // 最大値
    cnfg_data->gain.auto_control = data.gain.auto_control;  // 自動コントロール(固定[0]/自動[1])
    cnfg_data->gain.auto_rate = data.gain.auto_rate;     // 自動コントロール変化量
    cnfg_data->gain.auto_start_h = data.gain.auto_start_h;  // 自動コントロール判定輝度(開始上限:この値より輝度が高い場合、暗くする)   [0 - 255]
    cnfg_data->gain.auto_start_l = data.gain.auto_start_l;  // 自動コントロール判定輝度(開始下限:この値より輝度が高い場合、明るくする) [0 - 255]
    cnfg_data->gain.auto_stop_h = data.gain.auto_stop_h;   // 自動コントロール判定輝度(停止上限:この値より輝度が低い場合、停止する)   [0 - 255]
    cnfg_data->gain.auto_stop_l = data.gain.auto_stop_l;   // 自動コントロール判定輝度(停止下限:この値より輝度が高い場合、停止する)   [0 - 255]

    // 輝度コントロール設定(露光時間)
    cnfg_data->expstime.val = data.expstime.val;           // 設定値
    cnfg_data->expstime.val_min = data.expstime.val_min;       // 最小値
    cnfg_data->expstime.val_max = data.expstime.val_max;       // 最大値
    cnfg_data->expstime.auto_control = data.expstime.auto_control;  // 自動コントロール(固定[0]/自動[1])
    cnfg_data->expstime.auto_rate = data.expstime.auto_rate;     // 自動コントロール変化量
    cnfg_data->expstime.auto_start_h = data.expstime.auto_start_h;  // 自動コントロール判定輝度(開始上限:この値より輝度が高い場合、暗くする)   [0 - 255]
    cnfg_data->expstime.auto_start_l = data.expstime.auto_start_l;  // 自動コントロール判定輝度(開始下限:この値より輝度が高い場合、明るくする) [0 - 255]
    cnfg_data->expstime.auto_stop_h = data.expstime.auto_stop_h;   // 自動コントロール判定輝度(停止上限:この値より輝度が低い場合、停止する)   [0 - 255]
    cnfg_data->expstime.auto_stop_l = data.expstime.auto_stop_l;   // 自動コントロール判定輝度(停止下限:この値より輝度が高い場合、停止する)   [0 - 255]

    // カメラの異常判定設定
    cnfg_data->error.framedrop = data.error.framedrop;  // フレームレート低下の判定値[fps]
    cnfg_data->error.errtime = data.error.errtime;    // 異常判定時間[msec]

    // カメラの起動設定
    cnfg_data->boot.bootwaittimer = data.boot.bootwaittimer;   // システム起動後のカメラ起動待ち時間[msec]
    cnfg_data->boot.rebootcount = data.boot.rebootcount;     // システム起動後のカメラ起動失敗時にカメラリを再起動する回数[count]
    cnfg_data->boot.rebootingtimer = data.boot.rebootingtimer;  // カメラ再起動中であることを通知する時間[msec]

    return TRUE;
}

/// @brief カメラ設定読込み
/// @param
/// @return 
/// @note 
BOOL CSwayShared::get_app_config(PCONFIG_CAMERA data)
{
    if (data == NULL) {
        return FALSE;
    }

    PCONFIG_CAMERA cnfg_data = gp_cnfg_camera;  // カメラ設定

    // カメラの基本設定
    data->basis.ipaddress = cnfg_data->basis.ipaddress;                 // IPアドレス
    for (int idx = 0; idx < static_cast<uint32_t>(ENUM_AXIS::E_MAX); idx++) {
        data->basis.roi[idx].offset = cnfg_data->basis.roi[idx].offset; // ROI(領域)の開始位置
        data->basis.roi[idx].size = cnfg_data->basis.roi[idx].size;   // ROI(領域)のサイズ
    }
    data->basis.packetsize = cnfg_data->basis.packetsize;         // ドライバが受け取るパケットの最大サイズ(通常は0を指定)[byte]
    data->basis.framerate = cnfg_data->basis.framerate;          // フレームレート[fps]
    data->basis.blacklevel = cnfg_data->basis.blacklevel;         // 黒レベル設定
    data->basis.gamma = cnfg_data->basis.gamma;              // ガンマ補正
    data->basis.wb.wb_auto = cnfg_data->basis.wb.wb_auto;         // ホワイトバランスゲイン自動調整モー設定(自動調整OFF[1]/連続自動調整[2]/一回のみ自動調整[3])
    data->basis.wb.wb_ratio_red = cnfg_data->basis.wb.wb_ratio_red;    // ホワイトバランスゲイン設定(Red Gain)
    data->basis.wb.wb_ratio_blue = cnfg_data->basis.wb.wb_ratio_blue;   // ホワイトバランスゲイン設定(Blue Gain)
    for (int idx = 0; idx < static_cast<uint32_t>(ENUM_AXIS::E_MAX); idx++) {
        data->basis.viewangle[idx] = cnfg_data->basis.viewangle[idx];   // 視野角[deg]
    }

    // 輝度コントロール設定(ゲイン)
    data->gain.val = cnfg_data->gain.val;          // 設定値
    data->gain.val_min = cnfg_data->gain.val_min;      // 最小値
    data->gain.val_max = cnfg_data->gain.val_max;      // 最大値
    data->gain.auto_control = cnfg_data->gain.auto_control; // 自動コントロール(固定[0]/自動[1])
    data->gain.auto_rate = cnfg_data->gain.auto_rate;    // 自動コントロール変化量
    data->gain.auto_start_h = cnfg_data->gain.auto_start_h; // 自動コントロール判定輝度(開始上限:この値より輝度が高い場合、暗くする)   [0 - 255]
    data->gain.auto_start_l = cnfg_data->gain.auto_start_l; // 自動コントロール判定輝度(開始下限:この値より輝度が高い場合、明るくする) [0 - 255]
    data->gain.auto_stop_h = cnfg_data->gain.auto_stop_h;  // 自動コントロール判定輝度(停止上限:この値より輝度が低い場合、停止する)   [0 - 255]
    data->gain.auto_stop_l = cnfg_data->gain.auto_stop_l;  // 自動コントロール判定輝度(停止下限:この値より輝度が高い場合、停止する)   [0 - 255]

    // 輝度コントロール設定(露光時間)
    data->expstime.val = cnfg_data->expstime.val;          // 設定値
    data->expstime.val_min = cnfg_data->expstime.val_min;      // 最小値
    data->expstime.val_max = cnfg_data->expstime.val_max;      // 最大値
    data->expstime.auto_control = cnfg_data->expstime.auto_control; // 自動コントロール(固定[0]/自動[1])
    data->expstime.auto_rate = cnfg_data->expstime.auto_rate;    // 自動コントロール変化量
    data->expstime.auto_start_h = cnfg_data->expstime.auto_start_h; // 自動コントロール判定輝度(開始上限:この値より輝度が高い場合、暗くする)   [0 - 255]
    data->expstime.auto_start_l = cnfg_data->expstime.auto_start_l; // 自動コントロール判定輝度(開始下限:この値より輝度が高い場合、明るくする) [0 - 255]
    data->expstime.auto_stop_h = cnfg_data->expstime.auto_stop_h;  // 自動コントロール判定輝度(停止上限:この値より輝度が低い場合、停止する)   [0 - 255]
    data->expstime.auto_stop_l = cnfg_data->expstime.auto_stop_l;  // 自動コントロール判定輝度(停止下限:この値より輝度が高い場合、停止する)   [0 - 255]

    // カメラの異常判定設定
    data->error.framedrop = cnfg_data->error.framedrop; // フレームレート低下の判定値[fps]
    data->error.errtime = cnfg_data->error.errtime;   // 異常判定時間[msec]

    // カメラの起動設定
    data->boot.bootwaittimer = cnfg_data->boot.bootwaittimer;  // システム起動後のカメラ起動待ち時間[msec]
    data->boot.rebootcount = cnfg_data->boot.rebootcount;    // システム起動後のカメラ起動失敗時にカメラリを再起動する回数[count]
    data->boot.rebootingtimer = cnfg_data->boot.rebootingtimer; // カメラ再起動中であることを通知する時間[msec]

    return TRUE;
}

/// @brief 取付寸法設定書込み
/// @param
/// @return 
/// @note 
BOOL CSwayShared::set_app_config(CONFIG_MOUNTING data)
{
    PCONFIG_MOUNTING cnfg_data = gp_cnfg_mounting;  // 取付寸法設定

    for (int idx = 0; idx < static_cast<uint32_t>(ENUM_AXIS::E_MAX); idx++) {
        cnfg_data->offset[idx].l0 = data.offset[idx].l0;    // カメラ回転軸距離L0[mm]
        cnfg_data->offset[idx].t0 = data.offset[idx].t0;    // カメラ取付角度θ0(*10^6)[rad]
        cnfg_data->offset[idx].p0 = data.offset[idx].p0;    // カメラ設置角度φ0(*10^6)[rad]
    }

    return TRUE;
}

/// @brief 取付寸法設定読込み
/// @param
/// @return 
/// @note 
BOOL CSwayShared::get_app_config(PCONFIG_MOUNTING data)
{
    if (data == NULL) {
        return FALSE;
    }

    PCONFIG_MOUNTING cnfg_data = gp_cnfg_mounting;  // 取付寸法設定

    for (int idx = 0; idx < static_cast<uint32_t>(ENUM_AXIS::E_MAX); idx++) {
        data->offset[idx].l0 = cnfg_data->offset[idx].l0;   // カメラ回転軸距離L0[mm]
        data->offset[idx].t0 = cnfg_data->offset[idx].t0;   // カメラ取付角度θ0(*10^6)[rad]
        data->offset[idx].p0 = cnfg_data->offset[idx].p0;   // カメラ設置角度φ0(*10^6)[rad]
    }

    return TRUE;
}

/// @brief 画像処理条件設定書込み
/// @param
/// @return 
/// @note 
BOOL CSwayShared::set_app_config(CONFIG_IMGPROC data)
{
    PCONFIG_IMGPROC cnfg_data = gp_cnfg_imgprc;   // 画像処理条件設定

    // ROI設定
    cnfg_data->roi.valid = data.roi.valid;  // ROI有効設定(無効[FALSE]/有効[TRUE])
    cnfg_data->roi.scale = data.roi.scale;  // ROIスケール(検出したターゲットに対する倍率)

    // マスク設定
    for (int idx = 0; idx < static_cast<uint32_t>(ENUM_IMAGE_MASK::E_MAX); idx++) {
        cnfg_data->mask[idx].valid = data.mask[idx].valid;    // マスク選択(無効[FALSE]/有効[TRUE])
        for (int hsv = 0; hsv < static_cast<uint32_t>(ENUM_HSV_MODEL::E_MAX); hsv++) {
            cnfg_data->mask[idx].hsv_l[hsv] = data.mask[idx].hsv_l[hsv];    // HSVマスク判定値(下限)
            cnfg_data->mask[idx].hsv_u[hsv] = data.mask[idx].hsv_u[hsv];    // HSVマスク判定値(上限)
        }
    }

    // フィルター
    for (uint32_t filter = 0; filter < static_cast<uint32_t>(ENUM_NOISE_FILTER::E_MAX); filter++) {
        cnfg_data->filter[filter].type = data.filter[filter].type;  // フィルター種類
        cnfg_data->filter[filter].val = data.filter[filter].val;   // フィルター値
    }

    cnfg_data->imgprc = data.imgprc;         // ターゲット検出アルゴリズム(0=最大輪郭面積, 1=最大輪郭長)
    cnfg_data->swayspdfilter = data.swayspdfilter;  // 振れ速度フィルター時定数

    cnfg_data->swayzeroset_time = data.swayzeroset_time;    // 振れゼロ点設定(計測時間[ms])

    return TRUE;
}

/// @brief 画像処理条件設定読込み
/// @param
/// @return 
/// @note 
BOOL CSwayShared::get_app_config(PCONFIG_IMGPROC data)
{
    if (data == NULL) {
        return FALSE;
    }

    PCONFIG_IMGPROC cnfg_data = gp_cnfg_imgprc;   // 画像処理条件設定

    // ROI設定
    data->roi.valid = cnfg_data->roi.valid; // ROI有効設定(無効[FALSE]/有効[TRUE])
    data->roi.scale = cnfg_data->roi.scale; // ROIスケール(検出したターゲットに対する倍率)

    // マスク設定
    for (int idx = 0; idx < static_cast<uint32_t>(ENUM_IMAGE_MASK::E_MAX); idx++) {
        data->mask[idx].valid = cnfg_data->mask[idx].valid;           // マスク選択(無効[FALSE]/有効[TRUE])
        for (int hsv = 0; hsv < static_cast<uint32_t>(ENUM_HSV_MODEL::E_MAX); hsv++) {
            data->mask[idx].hsv_l[hsv] = cnfg_data->mask[idx].hsv_l[hsv];   // HSVマスク判定値(下限)
            data->mask[idx].hsv_u[hsv] = cnfg_data->mask[idx].hsv_u[hsv];   // HSVマスク判定値(上限)
        }
    }

    // ゴマ塩ノイズフィルター
    for (uint32_t filter = 0; filter < static_cast<uint32_t>(ENUM_NOISE_FILTER::E_MAX); filter++) {
        data->filter[filter].type = cnfg_data->filter[filter].type; // フィルター種類
        data->filter[filter].val = cnfg_data->filter[filter].val;  // フィルター値
    }

    data->imgprc = cnfg_data->imgprc;        // ターゲット検出アルゴリズム(0=最大輪郭面積, 1=最大輪郭長)
    data->swayspdfilter = cnfg_data->swayspdfilter; // 振れ速度フィルター時定数

    data->swayzeroset_time = cnfg_data->swayzeroset_time;   // 振れゼロ点設定(計測時間[ms])

    return TRUE;
}

/// @brief 画像データ書込み
/// @param
/// @return 
/// @note
BOOL CSwayShared::set_app_info_data(uint8_t id, cv::Mat cv_mat)
{
    if (id >= static_cast<uint32_t>(ENUM_IMAGE::E_MAX)) {
        return FALSE;
    }

    PINFO_IMGBUF_DATA info_data = gp_app_imgbuf[id];  // 画像情報データ

    EnterCriticalSection(&g_app_info.imgbuf[id].cs);

    cv_mat.copyTo(info_data->cv_mat);   // CpenCV

    LeaveCriticalSection(&g_app_info.imgbuf[id].cs);

    return TRUE;
}

/// @brief 画像データ読込み
/// @param
/// @return 
/// @note
BOOL CSwayShared::get_app_info_data(uint8_t id, cv::Mat* cv_mat)
{
    if (id >= static_cast<uint32_t>(ENUM_IMAGE::E_MAX)) {
        return FALSE;
    }
    if (cv_mat == NULL) {
        return FALSE;
    }

    PINFO_IMGBUF_DATA info_data = gp_app_imgbuf[id];  // 画像情報データ

    EnterCriticalSection(&g_app_info.imgbuf[id].cs);

    info_data->cv_mat.copyTo(*cv_mat);  // CpenCV

    LeaveCriticalSection(&g_app_info.imgbuf[id].cs);

    return TRUE;
}
/// @brief クライアント情報データ書込み
/// @param
/// @return 
/// @note
BOOL CSwayShared::set_app_info_data(INFO_CLIENT_DATA data)
{
    PINFO_CLIENT_DATA info_data = &g_app_info.client.data;  // クライアント情報データ

    EnterCriticalSection(&g_app_info.client.cs);

    for (int i = 0; i < 4; i++) info_data->device_id[i] = data.device_id[i];  // 機器ID
    info_data->command = data.command;    // コマンド
    info_data->mode = data.mode;       // モード
    info_data->cycle = data.cycle;      // 最小受信周期[ms]
    for (uint32_t idx = 0; idx < static_cast<uint32_t>(ENUM_SWAY_SENSOR::E_MAX); idx++) {
        info_data->tgt_len[idx] = data.tgt_len[idx];    // ターゲット距離[mm]
    }

    LeaveCriticalSection(&g_app_info.client.cs);

    return TRUE;
}

/// @brief クライアント情報データ読込み
/// @param
/// @return 
/// @note
BOOL CSwayShared::get_app_info_data(PINFO_CLIENT_DATA data)
{
    if (data == NULL) {
        return FALSE;
    }

    PINFO_CLIENT_DATA info_data = &g_app_info.client.data;  // クライアント情報データ

    EnterCriticalSection(&g_app_info.client.cs);

    for (int i = 0; i < 4; i++) data->device_id[i] = info_data->device_id[i];  // 機器ID
    data->command = info_data->command;   // コマンド
    data->mode = info_data->mode;      // モード
    data->cycle = info_data->cycle;     // 最小受信周期[ms]
    for (uint32_t idx = 0; idx < static_cast<uint32_t>(ENUM_SWAY_SENSOR::E_MAX); idx++) {
        data->tgt_len[idx] = info_data->tgt_len[idx];   // ターゲット距離[mm]
    }

    LeaveCriticalSection(&g_app_info.client.cs);

    return TRUE;
}

/// @brief 調整情報データ書込み
/// @param
/// @return 
/// @note
BOOL CSwayShared::set_app_info_data(INFO_ADJUST_DATA data)
{
    PINFO_ADJUST_DATA info_data = gp_app_adjust;  // 調整情報データ

    EnterCriticalSection(&g_app_info.adjust.cs);

    info_data->tg_distance_mode = data.tg_distance_mode;  // 吊点～ターゲット間距離固定
    info_data->target_distance = data.target_distance;        // 吊点～ターゲット間距離

    LeaveCriticalSection(&g_app_info.adjust.cs);

    return TRUE;
}

/// @brief 調整情報データ読込み
/// @param
/// @return 
/// @note
BOOL CSwayShared::get_app_info_data(PINFO_ADJUST_DATA data)
{
    if (data == NULL) {
        return FALSE;
    }

    PINFO_ADJUST_DATA info_data = gp_app_adjust;  // 調整情報データ

    EnterCriticalSection(&g_app_info.adjust.cs);

    data->tg_distance_mode = info_data->tg_distance_mode; // 吊点～ターゲット間距離固定
    data->target_distance = info_data->target_distance;       // 吊点～ターゲット間距離

    LeaveCriticalSection(&g_app_info.adjust.cs);

    return TRUE;
}

/// @brief 画像処理情報データ書込み
/// @param
/// @return 
/// @note
BOOL CSwayShared::set_app_info_data(INFO_IMGPRC_DATA data)
{
    PINFO_IMGPRC_DATA info_data = &g_app_info.imgprc.data;  // 画像処理情報データ

    EnterCriticalSection(&g_app_info.imgprc.cs);

    for (uint32_t idx = 0; idx < static_cast<uint32_t>(ENUM_IMAGE_MASK::E_MAX); idx++) {
        info_data->contours[idx] = data.contours[idx];  // 輪郭座標
    }

    for (uint32_t idx = 0; idx < static_cast<uint32_t>(ENUM_IMAGE_MASK::E_MAX); idx++) {
        info_data->target_data[idx].valid                               = data.target_data[idx].valid;              // 検出状態
        info_data->target_data[idx].max_val                             = data.target_data[idx].max_val;            // 最大輝度
        for (int axis = 0; axis < static_cast<uint32_t>(ENUM_AXIS::E_MAX); axis++) {
            info_data->target_data[idx].pos[axis]                       = data.target_data[idx].pos[axis];          // 検出位置[pixel]
        }
        info_data->target_data[idx].size                                = data.target_data[idx].size;               // 検出サイズ
        info_data->target_data[idx].roi.x                               = data.target_data[idx].roi.x;              // ROI:x coordinate of the top-left corner
        info_data->target_data[idx].roi.y                               = data.target_data[idx].roi.y;              // ROI:y coordinate of the top-left corner
        info_data->target_data[idx].roi.width                           = data.target_data[idx].roi.width;          // ROI:width of the rectangle
        info_data->target_data[idx].roi.height                          = data.target_data[idx].roi.height;         // ROI:height of the rectangle

        info_data->target_data[idx].size_detected                       = data.target_data[idx].size_detected;      //
        info_data->target_data[idx].size_expected                       = data.target_data[idx].size_expected;      //
        info_data->target_data[idx].size_roi_spd_margin                 = data.target_data[idx].size_roi_spd_margin;//
    }

    for (int axis = 0; axis < static_cast<uint32_t>(ENUM_AXIS::E_MAX); axis++) {
        info_data->sway_data[axis].target_pos                           = data.sway_data[axis].target_pos;          // ターゲット位置[pixel]
        info_data->sway_data[axis].target_tilt                          = data.sway_data[axis].target_tilt;         // ターゲット傾き[pixel]
        info_data->sway_data[axis].sway_angle                           = data.sway_data[axis].sway_angle;          // 振れ角[pixel]
        info_data->sway_data[axis].sway_speed                           = data.sway_data[axis].sway_speed;          // 振れ速度[pixel/s]
        info_data->sway_data[axis].sway_zero                            = data.sway_data[axis].sway_zero;           // 振れ中心[pixel]
    }

    info_data->target_size                                              = data.target_size;                         // ターゲットサイズ(ターゲット検出データの平均)
    info_data->status                                                   = data.status;                              // 状態
    info_data->img_fps                                                  = data.img_fps;                             // フレームレート[fps]
    info_data->img_val                                                  = data.img_val;                             // 明度
    info_data->exps_mode                                                = data.exps_mode;                           // シャッタコントロールモード(0:停止 1:Up -1:Down)
    info_data->exps_time                                                = data.exps_time;                           // 露光時間[us]

    info_data->mean_hsv                                                 = data.mean_hsv;

    LeaveCriticalSection(&g_app_info.imgprc.cs);

    return TRUE;
}

/// @brief 画像処理情報データ読込み
/// @param
/// @return 
/// @note
BOOL CSwayShared::get_app_info_data(PINFO_IMGPRC_DATA data)
{
    if (data == NULL) {
        return FALSE;
    }

    PINFO_IMGPRC_DATA info_data = &g_app_info.imgprc.data;  // 画像処理情報データ

    EnterCriticalSection(&g_app_info.imgprc.cs);

    for (int idx = 0; idx < static_cast<uint32_t>(ENUM_IMAGE_MASK::E_MAX); idx++) {
        data->contours[idx] = info_data->contours[idx]; // 輪郭座標
    }

    for (int idx = 0; idx < static_cast<uint32_t>(ENUM_IMAGE_MASK::E_MAX); idx++) {
        data->target_data[idx].valid = info_data->target_data[idx].valid;     // 検出状態
        data->target_data[idx].max_val = info_data->target_data[idx].max_val;   // 最大輝度
        for (int axis = 0; axis < static_cast<uint32_t>(ENUM_AXIS::E_MAX); axis++) {
            data->target_data[idx].pos[axis] = info_data->target_data[idx].pos[axis];   // 検出位置X[pixel]
        }
        data->target_data[idx].size = info_data->target_data[idx].size;       // 検出サイズ
        data->target_data[idx].roi.x = info_data->target_data[idx].roi.x;      // ROI:x coordinate of the top-left corner
        data->target_data[idx].roi.y = info_data->target_data[idx].roi.y;      // ROI:y coordinate of the top-left corner
        data->target_data[idx].roi.width = info_data->target_data[idx].roi.width;  // ROI:width of the rectangle
        data->target_data[idx].roi.height = info_data->target_data[idx].roi.height; // ROI:height of the rectangle

        data->target_data[idx].size_detected = info_data->target_data[idx].size_detected; // 
        data->target_data[idx].size_expected = info_data->target_data[idx].size_expected; // 
        data->target_data[idx].size_roi_spd_margin = info_data->target_data[idx].size_roi_spd_margin; // 
    }

    for (int axis = 0; axis < static_cast<uint32_t>(ENUM_AXIS::E_MAX); axis++) {
        data->sway_data[axis].target_pos = info_data->sway_data[axis].target_pos;  // ターゲット位置[pixel]
        data->sway_data[axis].target_tilt = info_data->sway_data[axis].target_tilt; // ターゲット傾き[pixel]
        data->sway_data[axis].sway_angle = info_data->sway_data[axis].sway_angle;  // 振れ角[pixel]
        data->sway_data[axis].sway_speed = info_data->sway_data[axis].sway_speed;  // 振れ速度[pixel/s]
        data->sway_data[axis].sway_zero = info_data->sway_data[axis].sway_zero;   // 振れ中心[pixel]
    }

    data->target_size   = info_data->target_size; // ターゲットサイズ(ターゲット検出データの平均)
    data->status        = info_data->status;      // 状態
    data->img_fps       = info_data->img_fps;     // フレームレート[fps]
    data->img_val       = info_data->img_val;     // 明度
    data->exps_mode     = info_data->exps_mode;   // シャッタコントロールモード(0:停止 1:Up -1:Down)
    data->exps_time     = info_data->exps_time;   // 露光時間[us]

    data->mean_hsv      = info_data->mean_hsv;

    LeaveCriticalSection(&g_app_info.imgprc.cs);

    return TRUE;
}

/// @brief システム情報データ書込み
/// @param
/// @return 
/// @note
BOOL CSwayShared::set_app_info_data(INFO_SYSTEM_DATA data)
{
    PINFO_SYSTEM_DATA info_data = &g_app_info.system.data;  // システム情報データ

    EnterCriticalSection(&g_app_info.system.cs);

    info_data->status = data.status;    // ステータス

    LeaveCriticalSection(&g_app_info.system.cs);

    return TRUE;
}

/// @brief システム情報データ読込み
/// @param
/// @return 
/// @note
BOOL CSwayShared::get_app_info_data(PINFO_SYSTEM_DATA data)
{
    if (data == NULL) {
        return FALSE;
    }

    PINFO_SYSTEM_DATA info_data = &g_app_info.system.data;  // システム情報データ

    EnterCriticalSection(&g_app_info.system.cs);

    data->status = info_data->status;   // ステータス

    LeaveCriticalSection(&g_app_info.system.cs);

    return TRUE;
}
