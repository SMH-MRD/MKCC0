#pragma once

#include <windows.h>
#include <string>

#include "..\Inc\TeliCamApi.h" // For Teli Camera
#include "..\Inc\TeliCamUtl.h" // For Teli Camera

//============================================================================
// define定義
#pragma region CONSTANT_DEFINITION
//----------------------------------------------------------------------------
#define GEV_CAMERA_REMOVE_EVENT_WAITIME     2000    // シグナル状態にセットされるまで待機するタイムアウト時間[ms]
#define CHECK_FPS_INTERVAL_TIME             2000    // fpsをチェックする間隔[ms]

#define IMAGE_SIZE_WIDTH                    2048    // 最大画像サイズ (水平画素)
#define IMAGE_SIZE_HEIGHT                   1536    // 最大画像サイズ (垂直画素)
#define IMAGE_SIZE                          (IMAGE_SIZE_WIDTH * IMAGE_SIZE_HEIGHT)  // 最大画像サイズ
#define IMAGE_FORMAT_SIZE                   3       // 画像フォーマットサイズ(uint8_t * IMAGE_FORMAT_SIZE)[byte]
#pragma endregion CONSTANT_DEFINITION

//============================================================================
// enum定義
#pragma region ENUM_DEFINITION
#pragma endregion ENUM_DEFINITION

//============================================================================
// 構造体/共用体定義
#pragma region STRUCTURE_DEFINITION
// カメラの設定
typedef struct _TELI_CAM_CONFIG {
    BOOL         valid = FALSE;             // カメラの有効または無効[0:無効 1:有効]
    std::wstring ipaddress = L"0.0.0.0";         // カメラのIPアドレス
    uint32_t     packetsize = 0;        // ドライバが受け取るパケットの最大サイズ(通常は0を指定)[byte]
    float64_t    framerate_drop = 15.0;    // フレームレート低下の判定値[fps]
 } TELI_CAM_CONFIG, * PTELI_CAM_CONFIG;
// カメラのステータス
typedef struct _TELI_CAM_STATUS {
    int32_t               control_status;
    int32_t               retry_count;

    bool                  is_img_valid;     //
    bool                  is_fps_valid;     //
    int32_t               camidx;           // カメラのインデックス
    Teli::CAM_HANDLE      camhndl;          // オープンしたカメラのカメラハンドル
    Teli::CAM_STRM_HANDLE strmhndl;         // オープンしたストリームインターフェースのストリームハンドル
    Teli::CAM_API_STATUS  apistat;          // TeliCamAPIのステータスコード
    Teli::CAM_API_STATUS  errstat;          // 画像ストリーム受信時のエラーステータスコード
    uint32_t              pyldsize;         // 1つのストリームリクエストで受信するペイロードのサイズ(画像サイズ)
    std::wstring          camname;          // カメラの名前
    uint32_t              camwidth;         // 映像の幅
    uint32_t              camheight;        // 映像の高さ
    uint32_t              framecount;       // FPSを計算するためのフレームカウンター
    uint32_t              frameidx;         // カメラから出力されるビデオデータのフレーム番号
    BOOL                  frameidx_valid;   // "frameidx"に有効な値があることを示すフラグ
    uint32_t              fpstimer;         // フレーム更新タイマー
    float64_t             fps;              // 実際のFPS
    BOOL                  framechk_valid;   // フレームレート損失チェック
    float64_t             expstime_min;     // 露光時間(最小)
    float64_t             expstime_max;     // 露光時間(最大)
    float64_t             expstime;         // 露光時間(最大)
    float64_t             gainmin;          // ゲイン(最小)
    float64_t             gainmax;          // ゲイン(最大)
    float64_t             gain;             // ゲイン

    _TELI_CAM_STATUS()
        : camidx(0)
        , camhndl(NULL)
        , strmhndl(NULL)
        , apistat(Teli::CAM_API_STS_SUCCESS)
        , errstat(Teli::CAM_API_STS_SUCCESS)
        , pyldsize(0)
        , camname(L"Not found camera")
        , camwidth(0)
        , camheight(0)
        , framecount(0)
        , frameidx(0)
        , frameidx_valid(FALSE)
        , fpstimer(0)
        , fps(0.0)
        , framechk_valid(FALSE)
        , expstime_min(0.0)
        , expstime_max(0.0)
        , expstime(0.0)
        , gainmin(0.0)
        , gainmax(0.0)
        , gain(0.0)
    {
    }
} TELI_CAM_STATUS, * PTELI_CAM_STATUS;

// カメラの詳細情報
typedef struct _TELI_CAM_DETAILS {
    HANDLE EventHndlCamRemoval; // カメラ取り外し通知用のイベント(シグナル)オブジェクトのハンドル
    //
    HANDLE td_camera_remove_hndl;       // スレッドハンドル
    BOOL   td_camera_remove_stat;       // スレッドステータス(FALSE:Exit TRUE:Run)
    HANDLE td_gain_control_hndl;        // スレッドハンドル
    BOOL   td_gain_control_stat;        // スレッドステータス(FALSE:Exit TRUE:Run)
    HANDLE td_expstime_control_hndl;    // スレッドハンドル
    BOOL   td_expstime_control_stat;    // スレッドステータス(FALSE:Exit TRUE:Run)
    //
    TELI_CAM_CONFIG cnfg; // カメラの設定
    TELI_CAM_STATUS stat; // カメラのステータス
  
    _TELI_CAM_DETAILS()
        : EventHndlCamRemoval(NULL)
        , td_camera_remove_hndl(NULL)
        , td_camera_remove_stat(FALSE)
        , td_gain_control_hndl(NULL)
        , td_gain_control_stat(FALSE)
        , td_expstime_control_hndl(NULL)
        , td_expstime_control_stat(FALSE)
        , cnfg()
        , stat()
    {
    }
} TELI_CAM_DETAILS, * PTELI_CAM_DETAILS;
// カメラの情報
typedef struct TAG_TELICAM_LIB_INFO {
    uint32_t       camcount;    // 検出したカメラの数
    TELI_CAM_DETAILS details;     // カメラの詳細情報

    TAG_TELICAM_LIB_INFO()
        : camcount(0)
        , details()
    {
    }
} TELICAM_LIB_INFO, * PTELICAM_LIB_INFO;


#pragma endregion STRUCTURE_DEFINITION

//////////////////////////////////////////////////////////////////////////////
// CTeliCamLib

class CTeliCamLib
{
public:
    CTeliCamLib(void);
    CTeliCamLib(TELICAM_LIB_INFO caminfo);
    virtual ~CTeliCamLib(void);

    // メンバー関数
    int32_t initialize(void);                               // TeliCamAPIの初期化処理
    void    close(void);                                    // TeliCamAPIの終了処理
    int32_t update_camera_list(void);                       // PCに接続されているカメラの探索
    int32_t	open_camera(Teli::CAM_ACCESS_MODE accessmode);  // カメラのオープン
    int32_t	close_camera(void);                             // カメラのクローズ
    int32_t open_stream(void);                              // 画像取得用のストリームインターフェースのオープン
    int32_t close_stream(void);                             // 画像取得用のストリームインターフェースのクローズ
    int32_t start_stream(void);                             // 画像ストリームの転送開始
    int32_t stop_stream(void);                              // 画像ストリームの転送停止

    int32_t set_pixelformat(Teli::CAM_PIXEL_FORMAT pixelformat);                                        // カメラのビデオストリームのピクセル形式の設定
    int32_t set_camroi(uint32_t offset_x, uint32_t offset_y, uint32_t width, uint32_t height);          // カメラのROI(領域)の設定
    int32_t set_framerate(float64_t framerate, Teli::CAM_ACQ_FRAME_RATE_CTRL_TYPE ctrltype);            // カメラのフレームレートの設定
    int32_t set_triggermode(bool8_t triggermode);                                                       // カメラのトリガー動作モードの設定
    int32_t set_blacklevel(float64_t blacklevel);                                                       // カメラの黒レベルの設定
    int32_t set_gamma(float64_t gamma);                                                                 // カメラのガンマ補正値の設定
    int32_t set_wbalance_auto(Teli::CAM_BALANCE_WHITE_AUTO_TYPE autotype);                              // カメラのホワイトバランスゲイン自動調整モードの設定
    int32_t set_wbalance_ratio(float64_t wbratio, Teli::CAM_BALANCE_RATIO_SELECTOR_TYPE selectortype);  // カメラのホワイトバランスゲイン(倍率)の設定
    int32_t set_gain_auto(Teli::CAM_GAIN_AUTO_TYPE autotype);                                           // カメラのAGC(Automatic gain control)動作モードの設定
    int32_t set_gain(float64_t gain);                                                                   // カメラのゲインの設定(APIへの設定はスレッドで実行される)
    int32_t set_expstime_control(Teli::CAM_EXPOSURE_TIME_CONTROL_TYPE ctrltype);                        // カメラの露光時間の制御モードの設定
    int32_t set_expstime(float64_t expstime);                                                           // カメラの露光時間の設定(APIへの設定はスレッドで実行される)

    int32_t get_image(uint32_t bufsize, uint8_t* image);        // 画像情報を取得
    int32_t get_image(void* image);                             // 画像情報を取得
    int32_t get_image_size(uint32_t* width, uint32_t* height);  // 画像サイズの取得
    int32_t get_image_fps(float64_t* fps);                      // フレームレートの取得

    Teli::CAM_INFO get_stat_caminfo(void);  // カメラ情報の取得
    std::wstring   get_camera_name(void);   // カメラ名の取得
    std::wstring   get_error_message(void); // エラーメッセージの取得
   
    bool is_stat_img_valid();
    // カメラの情報
    uint32_t       camcount;    // 検出したカメラの数
    Teli::CAM_INFO m_caminfo;
   
 static   TELI_CAM_CONFIG cnfg;       // カメラの設定
 static   TELI_CAM_STATUS stat;
    
protected:

private:
   // メンバー変数
   // CMyCriticalSection m_cscam;     // Critical section (exclusion processing)

    std::wstring       m_errmsg;    // エラーメッセージ

    // メンバー関数
    static int32_t get_caminfo_camindex(uint32_t camidx);  // カメラをオープンしたときのカメラのインデックスの取得
  
    static void CALLBACK cb_image_acquired(Teli::CAM_HANDLE hCam,
        Teli::CAM_STRM_HANDLE hStrm,
        Teli::PCAM_IMAGE_INFO psImageInfo,
        uint32_t uiBufferIndex,
        void* pvContext);    // 受信した画像を処理するためのコールバック関数
    static void CALLBACK cb_error_image(Teli::CAM_HANDLE hCam,
        Teli::CAM_STRM_HANDLE hStrm,
        Teli::CAM_API_STATUS uiErrorStatus,
        uint32_t uiBufferIndex,
        void* pvContext);       // 受信エラーを処理するためのコールバック関数

    static unsigned WINAPI td_camera_remove(LPVOID lpParam);    // カメラ取外し検出スレッド関数(未使用)
    static unsigned WINAPI td_gain_control(LPVOID lpParam);     // ゲイン設定スレッド関数
    static unsigned WINAPI td_expstime_control(LPVOID lpParam); // 露光時間設定スレッド関数
};

