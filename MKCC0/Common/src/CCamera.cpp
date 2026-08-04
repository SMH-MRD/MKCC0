#include "CCamera.h"
#include "CHelper.h"
#include <mmsystem.h>
#include <process.h>
#include <iostream>
#include <sstream>

using namespace std;
static wostringstream wos_msg;

TELI_CAM_CONFIG CTeliCamLib::cnfg;       // カメラの設定
TELI_CAM_STATUS CTeliCamLib::stat;


//////////////////////////////////////////////////////////////////////////////
// CTeliCamLib
#pragma comment(lib, "..\\Lib\\x64\\TeliCamApi64.lib")
#pragma comment(lib, "..\\Lib\\x64\\TeliCamUtl64.lib")

//////////////////////////////////////////////////////////////////////////////
// Public method

/// @brief Constructor
/// @param
/// @return
/// @note
CTeliCamLib::CTeliCamLib(void)
{
    m_errmsg = L""; // エラーメッセージ
    camcount = 0; // 検出したカメラの数

    // カメラのステータス
    stat.camidx = -1;                           // カメラのインデックス
    stat.camhndl = NULL;                         // オープンしたカメラのカメラハンドル
    stat.strmhndl = NULL;                         // オープンしたストリームインターフェースのストリームハンドル
    stat.apistat = Teli::CAM_API_STS_SUCCESS;    // TeliCamAPIのステータスコード
    stat.errstat = Teli::CAM_API_STS_SUCCESS;    // 画像ストリーム受信時のエラーステータスコード
    stat.pyldsize = 0;                            // 1つのストリームリクエストで受信するペイロードのサイズ(画像サイズ)
    stat.camname = L"Not found camera";          // カメラの名前
    stat.camwidth = 0;                            // 映像の幅
    stat.camheight = 0;                            // 映像の高さ
    stat.framecount = 0;                            // FPSを計算するためのフレームカウンター
    stat.frameidx = 0;                            // カメラから出力されるビデオデータのフレーム番号
    stat.frameidx_valid = FALSE;                        // "frameidx"に有効な値があることを示すフラグ
    stat.fpstimer = timeGetTime();                // フレーム更新タイマー
    stat.fps = 0.0;                          // 実際のFPS
    stat.framechk_valid = FALSE;                        // フレームレート損失チェック
    stat.expstime_min = 0.0;                          // 露光時間(最小)
    stat.expstime_max = 0.0;                          // 露光時間(最大)
    stat.expstime = 0.0;                          // 露光時間
    stat.gainmin = 0.0;                          // ゲイン(最小)
    stat.gainmax = 0.0;                          // ゲイン(最大)
    stat.gain = 0.0;                          // ゲイン
 }

/// @brief Constructor
/// @param caminfo カメラの情報  
/// @return
/// @note
CTeliCamLib::CTeliCamLib(TELICAM_LIB_INFO caminfo)
{
    m_errmsg = L""; // エラーメッセージ
    camcount = 0;  // 検出したカメラの数
  
    // カメラの設定
    PTELI_CAM_CONFIG pCamCnfg = &caminfo.details.cnfg;                // カメラの設定
    cnfg.valid          = pCamCnfg->valid;              // カメラの有効または無効[0:無効 1:有効]
    cnfg.ipaddress      = pCamCnfg->ipaddress;          // カメラのIPアドレス
    cnfg.packetsize     = pCamCnfg->packetsize;         // ドライバが受け取るパケットの最大サイズ(通常は0を指定)[byte]
    cnfg.framerate_drop = pCamCnfg->framerate_drop;     // フレームレート低下の判定値[fps]
   
    // カメラのステータス
    stat.camidx = -1;                               // カメラのインデックス
    stat.camhndl = NULL;                            // オープンしたカメラのカメラハンドル
    stat.strmhndl = NULL;                            // オープンしたストリームインターフェースのストリームハンドル
    stat.apistat = Teli::CAM_API_STS_SUCCESS;    // TeliCamAPIのステータスコード
    stat.errstat = Teli::CAM_API_STS_SUCCESS;    // 画像ストリーム受信時のエラーステータスコード
    stat.pyldsize = 0;                            // 1つのストリームリクエストで受信するペイロードのサイズ(画像サイズ)
    stat.camname = L"Not found camera";          // カメラの名前
    stat.camwidth = 0;                            // 映像の幅
    stat.camheight = 0;                            // 映像の高さ
    stat.framecount = 0;                            // FPSを計算するためのフレームカウンター
    stat.frameidx = 0;                            // カメラから出力されるビデオデータのフレーム番号
    stat.frameidx_valid = FALSE;                        // "frameidx"に有効な値があることを示すフラグ
    stat.fpstimer = timeGetTime();                // フレーム更新タイマー
    stat.fps = 0.0;                          // 実際のFPS
    stat.framechk_valid = FALSE;                        // フレームレート損失チェック 
    stat.expstime_min = 0.0;                          // 露光時間(最小)
    stat.expstime_max = 0.0;                          // 露光時間(最大)
    stat.expstime = 0.0;                          // 露光時間
    stat.gainmin = 0.0;                          // ゲイン(最小)
    stat.gainmax = 0.0;                          // ゲイン(最大)
    stat.gain = 0.0;                          // ゲイン
 }

/// @brief Destructor
/// @param
/// @return
/// @note
CTeliCamLib::~CTeliCamLib(void)
{
    //----------------------------------------------------------------------------
    // TeliCamAPIの終了処理
    close();
}

/// @brief TeliCamAPIの初期化処理
/// @param 
/// @return 結果(0:成功 0以外:失敗)
/// @note
int32_t CTeliCamLib::initialize(void)
{
    Teli::CAM_API_STATUS apistat = Teli::CAM_API_STS_SUCCESS;   // TeliCamAPIのステータスコード

    m_errmsg = L""; // エラーメッセージ
    //----------------------------------------------------------------------------
    // TeliCamAPIの初期化処理
    if ((apistat = Teli::Sys_Initialize()) != Teli::CAM_API_STS_SUCCESS) {
          if (cnfg.valid) {
            stat.apistat = apistat;
        }
        wos_msg.str(L""); wos_msg << L"[CTeliCamLib::initialize]<Error>Teli::Sys_Initialize(" << apistat <<")";
        m_errmsg = wos_msg.str();
        return -1;
    }   // if ((apistat = Teli::Sys_Initialize()) != CAM_API_STS_SUCCESS)
    return 0;
}

/// @brief TeliCamAPIの終了処理
/// @param 
/// @return Normal(0) or Abnormal(Not 0)
/// @note
void CTeliCamLib::close(void)
{
    Teli::CAM_API_STATUS apistat = Teli::CAM_API_STS_SUCCESS;    // TeliCamAPIのステータスコード
    //----------------------------------------------------------------------------
    // End of thread
    //----------------------------------------------------------------------------
    // 画像ストリームの転送停止
    stop_stream();
    _RPTWN(_CRT_WARN, L"%s%s\n", L">>>[CTeliCamLib::close]", get_error_message());

    //----------------------------------------------------------------------------
    // Stream close
    close_stream();
    _RPTWN(_CRT_WARN, L"%s%s\n", L">>>[CTeliCamLib::close]", get_error_message());

    //----------------------------------------------------------------------------
    // Close camera
    close_camera();
    _RPTWN(_CRT_WARN, L"%s%s\n", L">>>[CTeliCamLib::close]", get_error_message());
    
    //----------------------------------------------------------------------------
    // Termination process of TeliCamAPI
    if ((apistat = Teli::Sys_Terminate()) != Teli::CAM_API_STS_SUCCESS) {
        _RPTWN(_CRT_WARN, L"%s(%08X)\n", L">>>[CTeliCamLib::close]<Error>Teli::Sys_Terminate", apistat);
    }
    return;
}

/// @brief PCに接続されているカメラの探索
/// @param 
/// @return 結果(0:成功 0以外:失敗)
/// @note
int32_t CTeliCamLib::update_camera_list(void)
{
    int32_t              ret = 0;
    Teli::CAM_API_STATUS apistat = Teli::CAM_API_STS_SUCCESS;   // TeliCamAPIのステータスコード

    m_errmsg = L""; // エラーメッセージ

    //----------------------------------------------------------------------------
    // カメラのステータスのクリア
    stat.camidx = -1;                             // カメラのインデックス
    stat.camhndl = NULL;                           // オープンしたカメラのカメラハンドル
    stat.strmhndl = NULL;                           // オープンしたストリームインターフェースのストリームハンドル
    stat.apistat = Teli::CAM_API_STS_SUCCESS;      // TeliCamAPIのステータスコード
    stat.errstat = Teli::CAM_API_STS_SUCCESS;      // 画像ストリーム受信時のエラーステータスコード
    stat.pyldsize = 0;                              // 1つのストリームリクエストで受信するペイロードのサイズ(画像サイズ)
    stat.camname = L"Not found camera";            // カメラの名前
    stat.camwidth = 0;                              // 映像の幅
    stat.camheight = 0;                              // 映像の高さ
    stat.framecount = 0;                              // FPSを計算するためのフレームカウンター
    stat.frameidx = 0;                              // カメラから出力されるビデオデータのフレーム番号
    stat.frameidx_valid = FALSE;                          // "frameidx"に有効な値があることを示すフラグ
    stat.fpstimer = timeGetTime();                  // フレーム更新タイマー
    stat.fps = 0.0;                            // 実際のFPS
    stat.expstime_min = 0.0;                            // 露光時間(最小)
    stat.expstime_max = 0.0;                            // 露光時間(最大)
    stat.expstime = 0.0;                            // 露光時間(最大)
    stat.gainmin = 0.0;                            // ゲイン(最小)
    stat.gainmax = 0.0;                            // ゲイン(最大)
    stat.gain = 0.0;                            // ゲイン
    //----------------------------------------------------------------------------
    // PCに接続されているカメラの探索
    if ((apistat = Teli::Sys_GetNumOfCameras(&camcount)) != Teli::CAM_API_STS_SUCCESS) {
       stat.apistat = apistat;
        ret = -1;

        wos_msg.str(L""); wos_msg << L"[CTeliCamLib::update_camera_list]<Error>Teli::Sys_Initialize(" << apistat<<")";
        m_errmsg = wos_msg.str();
     }
    else
    {
        if (camcount > 0) {
            Teli::CAM_INFO      teli_caminfo;   // カメラの情報
            Teli::PGEV_CAM_INFO gevcaminfo;     // カメラの情報(GEV)
            std::wstring        ipaddrs;        // Camera IP address
            for (uint32_t camidx = 0; camidx < camcount; camidx++) {
                //----------------------------------------------------------------------------
                // カメラの情報の取得
                apistat = Teli::Cam_GetInformation(NULL, camidx, &teli_caminfo);
                if (apistat != Teli::CAM_API_STS_SUCCESS) {
                    continue;
                }
                else if (teli_caminfo.eCamType != Teli::CAM_TYPE_GEV) {
                    continue;
                }
                else {
                    ;
                }
                gevcaminfo = &teli_caminfo.sGevCamInfo; // カメラの情報(GEV)

                //----------------------------------------------------------------------------
                // IPアドレスをキーに関連付ける
                wos_msg.str(L""); wos_msg << L"["<< gevcaminfo->aucIPAddress[0] <<"]." << L"[" << gevcaminfo->aucIPAddress[1] << "]." << L"[" << gevcaminfo->aucIPAddress[2] << "]." << L"[" << gevcaminfo->aucIPAddress[3] << "]";
                ipaddrs = wos_msg.str();
               //ipaddrs = std::format(L"{:d}.{:d}.{:d}.{:d}", gevcaminfo->aucIPAddress[0], gevcaminfo->aucIPAddress[1], gevcaminfo->aucIPAddress[2],gevcaminfo->aucIPAddress[3]);

                if (ipaddrs.compare(cnfg.ipaddress) == 0) {
                    stat.camidx = camidx;             // カメラのインデックス
                    std::string modelname(teli_caminfo.szModelName);    // カメラのモデル名

                    wos_msg.str(L""); wos_msg << L"{" << CStrHelper::conv_string(modelname) << "}." << L"{" << ipaddrs << "}" ;
                    stat.camname = wos_msg.str();
                    //m_caminfo.details.stat.camname = std::format(L"{:s}({:s})", CStrHelper::conv_string(modelname), ipaddrs);   // カメラの名前
                    break;
                }
            }   // for (uint32_t camcount = 0; camcount < m_caminfo.camcount; camcount++)
        }   // if (m_caminfo.camcount > 0)
    }   // if ((apistat = Teli::Sys_GetNumOfCameras(&m_caminfo.camcount)) != Teli::CAM_API_STS_SUCCESS) else
   
    return ret;
}


/// <summary>
/// カメラのオープン
/// </summary>
/// <param name="accessmode">
/// カメラのアクセスモード
/// </param>
/// <scenario>
/// 1.カメラをオープンし、アプリケーションがカメラを使用できるようにします。
/// 2.カメラの映像の幅の取得
/// </scenario>
/// <returns></returns>
int32_t CTeliCamLib::open_camera(Teli::CAM_ACCESS_MODE accessmode)
{
    int32_t ret = Teli::CAM_API_STS_SUCCESS;
    
    m_errmsg = L""; // エラーメッセージ

    PTELI_CAM_STATUS camstat = &stat;   // カメラのステータス
    uint32_t       camwidth;            // 映像の幅
    uint32_t       camheight;           // 映像の高さ
    if ((camstat->camidx < 0) || (camstat->camidx >= (int32_t)camcount)) {
        ret = -1;
        m_errmsg = L"[CTeliCamLib::open_camera]<Error>camstat->camidx";
    }
    else
    {
        //----------------------------------------------------------------------------
        // カメラをオープンし、アプリケーションがカメラを使用できるようにします。
        if ((camstat->apistat = Teli::Cam_Open(
            camstat->camidx,        // カメラのインデックス
            &camstat->camhndl,      // オープンしたカメラのカメラハンドル
            NULL,                   // カメラ取り外し通知用のイベント(シグナル)オブジェクトのハンドル
            true,                   // GenICamアクセスの有効／無効
            NULL,                   // PC内のカメラ記述情報(XMLデータ)
            accessmode              // カメラのアクセスモード
        )) != Teli::CAM_API_STS_SUCCESS) {            
            camstat->camhndl = NULL;
            ret = -2;
            wos_msg.str(L""); wos_msg << L"[CTeliCamLib::open_camera]<Error>Teli::Cam_Open(" << camstat->apistat << ")";
            m_errmsg = wos_msg.str();
          }
        else {
            // カメラの映像の幅の取得
            if ((camstat->apistat = Teli::GetCamWidth(camstat->camhndl, &camwidth)) != Teli::CAM_API_STS_SUCCESS) {
                ret = -3;
                wos_msg.str(L""); wos_msg << L"[CTeliCamLib::open_camera]<Error>Teli::GetCamWidth(" << camstat->apistat << ")";
                m_errmsg = wos_msg.str();
            }
            // カメラの映像の高さの取得
            else if ((camstat->apistat = Teli::GetCamHeight(camstat->camhndl, &camheight)) != Teli::CAM_API_STS_SUCCESS) {
                ret = -4;
                wos_msg.str(L""); wos_msg << L"[CTeliCamLib::open_camera]<Error>Teli::GetCamHeight(" << camstat->apistat << ")";
                m_errmsg = wos_msg.str();
             }
            else {
                ;
            }
        }
    }   // if ((camstat->camidx < 0) || (camstat->camidx >= m_caminfo.camcount)) else
    if (ret == 0) {
        stat.camwidth   = camwidth;  // 映像の幅
		stat.camheight  = camheight; // 映像の高さ
 
    }
    else {
        stat.camwidth   = 0;  // 映像の幅
        stat.camheight  = 0; // 映像の高さ
    }
    return ret;
}

/// @brief カメラのクローズ
/// @param 
/// @return 結果(0:成功 0以外:失敗)
/// @note
int32_t CTeliCamLib::close_camera(void)
{
    int32_t ret = 0;

    m_errmsg = L""; // エラーメッセージ

    PTELI_CAM_STATUS camstat = &stat;   // カメラのステータス
    if (camstat->camhndl == NULL) {
        ret = -1;
        m_errmsg = L"[CTeliCamLib::close_camera]<Error>camstat->camhndl";
    }
    else {
        //----------------------------------------------------------------------------
        // カメラのクローズ
        if ((camstat->apistat = Teli::Cam_Close(camstat->camhndl)) != Teli::CAM_API_STS_SUCCESS) {
            ret = -2;
            
            wos_msg.str(L""); wos_msg << L"[CTeliCamLib::close_camera]<Error>Teli::Cam_Close(" << camstat->apistat << ")";
            m_errmsg = wos_msg.str();
        }
    }   // if (camstat->camhndl == NULL) else
    camstat->camhndl = NULL;
 
    return ret;
}

/// <summary>
/// 画像取得用のストリームインターフェースのオープンして
/// コールバック関数をApiに登録
/// (画像取得　Strm_SetCallbackImageAcquired()　　cb_image_acquired)
/// (エラー更新　Strm_SetCallbackImageError())    cb_error_image   )
/// /// </summary>
/// <param name=""></param>
/// <returns></returns>
int32_t CTeliCamLib::open_stream(void)
{
    int32_t ret = 0;

    m_errmsg = L""; // エラーメッセージ

    PTELI_CAM_STATUS camstat = &stat;   // カメラのステータス
    if (camstat->camhndl == NULL) {
        ret = -1;

        wos_msg.str(L""); wos_msg << L"[CTeliCamLib::open_stream]<Error>" << camstat->camhndl;
        m_errmsg = wos_msg.str();
        }
    else {
        PTELI_CAM_CONFIG camcnfg = &cnfg;   // カメラの設定
        //----------------------------------------------------------------------------
        // 画像取得用のストリームインターフェースのオープン
        // TeliCamAPI内部に画像一時保管用のストリームリクエストリングバッファを作成
        camstat->apistat = Teli::Strm_OpenSimple(
            camstat->camhndl,                                     // カメラのカメラハンドル
            &camstat->strmhndl,                                   // オープンしたストリームインターフェースのストリームハンドル
            &camstat->pyldsize,                                   // 1つのストリームリクエストで受信するペイロードのサイズ(画像サイズ)
            NULL,                                                 // ストリームを受信し、ストリームリクエストリングバッファが更新されたことを通知するイベント（シグナル）オブジェクトのハンドル
            DEFAULT_API_BUFFER_CNT,                               // TeliCamAPI内部に作成するストリームリクエストリングバッファの数
            camcnfg->packetsize                                   // ドライバが受け取るパケットの最大サイズ　0のセットが推奨されている
        );
      
        if ((camstat->apistat) != Teli::CAM_API_STS_SUCCESS) { // ドライバが受け取るパケットの最大サイズ
              ret = -2;

            wos_msg.str(L""); wos_msg << L"[CTeliCamLib::open_stream]<Error>Teli::Strm_OpenSimple(" << camstat->apistat << ")";
            m_errmsg = wos_msg.str();
        }
        //----------------------------------------------------------------------------
        // コールバック関数のTeliCamAPIに登録
        // TeliCamAPI内部のストリームリクエストリングバッファの内容を正常受信した画像データで更新した時にこのコールバック関数が呼び出される
        else if (
            (camstat->apistat = Teli::Strm_SetCallbackImageAcquired(
                        camstat->strmhndl,              // ストリームインターフェースのストリームハンドル
                        this,                           // コールバック関数を実行するときに引数として渡すオブジェクトへのポインタ
				                                        // CTeliCamLibのオブジェクトのポインタを渡すことで、コールバック関数内でCTeliCamLibのメンバ変数にアクセスできる
                        &CTeliCamLib::cb_image_acquired)// コールバック関数
            ) != Teli::CAM_API_STS_SUCCESS
                 ) 
        {    // コールバック関数
            wos_msg.str(L""); wos_msg << L"[CTeliCamLib::open_stream]<Error>Teli::Strm_SetCallbackImageAcquired(" << camstat->apistat << ")";
            m_errmsg = wos_msg.str();ret = -3;
         }
        //----------------------------------------------------------------------------
        // コールバック関数をTeliCamAPIに登録
        // ストリームを正常に受信できず、TeliCamAPI内部のストリームリクエストリングバッファの内容がエラー更新された時にコールバック関数を呼び出される
        else if ((camstat->apistat = 
            Teli::Strm_SetCallbackImageError(
                camstat->strmhndl,              // ストリームインターフェースのストリームハンドル
                this,                           // コールバック関数を実行するときに引数として渡すオブジェクトへのポインタ
                &CTeliCamLib::cb_error_image    // コールバック関数
            )
         ) != Teli::CAM_API_STS_SUCCESS) 
        {  
             wos_msg.str(L""); wos_msg << L"[CTeliCamLib::open_stream]<Error>Teli::Strm_SetCallbackImageError(" << camstat->apistat << ")";
            m_errmsg = wos_msg.str();ret = -4;
        }
        else {
            ;
        }
    }   // if (camstat->camhndl == NULL) else
 
    return ret;
}

/// @brief 画像取得用のストリームインターフェースのクローズ
/// @param 
/// @return 結果(0:成功 0以外:失敗)
/// @note
int32_t CTeliCamLib::close_stream(void)
{
    int32_t ret = 0;

     m_errmsg = L""; // エラーメッセージ

    PTELI_CAM_STATUS camstat = &stat;   // カメラのステータス
    if (camstat->strmhndl == NULL) {
        ret = -1;
        m_errmsg = L"[CTeliCamLib::close_stream]<Error>camstat->strmhndl";
    }
    else {
        //----------------------------------------------------------------------------
        // 画像取得用のストリームインターフェースをクローズ
        if ((camstat->apistat = Teli::Strm_Close(camstat->strmhndl)) != Teli::CAM_API_STS_SUCCESS) {
            ret = -2;
            
            wos_msg.str(L""); wos_msg << L"[CTeliCamLib::close_stream]<Error>Teli::Strm_Close(" << camstat->apistat << ")";
            m_errmsg = wos_msg.str();
            //m_errmsg = std::format(L"[CTeliCamLib::close_stream]<Error>Teli::Strm_Close({:#08x})", camstat->apistat);
        }
    }   // if (camstat->strmhndl == NULL) else
    camstat->strmhndl = NULL;

    return ret;
}

/// @brief 画像ストリームの転送開始
/// @param 
/// @return 結果(0:成功 0以外:失敗)
/// @note
int32_t CTeliCamLib::start_stream(void)
{
    int32_t ret = 0;

    m_errmsg = L""; // エラーメッセージ

    PTELI_CAM_STATUS camstat = &stat;   // カメラのステータス
    if (camstat->strmhndl == NULL) {
        ret = -1;
        m_errmsg = L"[CTeliCamLib::start_stream]<Error>camstat->strmhndl";
    }
    else {
        //----------------------------------------------------------------------------
        // 画像ストリームの転送開始をカメラに要求
        if ((camstat->apistat = Teli::Strm_Start(camstat->strmhndl)) != Teli::CAM_API_STS_SUCCESS) {
            wos_msg.str(L""); wos_msg << L"[CTeliCamLib::start_stream]<Error>Teli::Strm_Start(" << camstat->apistat << ")";
            m_errmsg = wos_msg.str();ret = -2;
        }
		//情報の初期化
        camstat->framecount = 0;                // FPSを計算するためのフレームカウンター
        camstat->frameidx = 0;                  // カメラから出力されるビデオデータのフレーム番号
        camstat->frameidx_valid = FALSE;        // "frameidx"に有効な値があることを示すフラグ
        camstat->fpstimer = timeGetTime();      // フレーム更新タイマー
        camstat->fps = 0.0;              // 実際のFPS
    }   // if (camstat->StrmHandl == NULL) else
 
    return ret;
}

/// @brief 画像ストリームの転送停止
/// @param 
/// @return 結果(0:成功 0以外:失敗)
/// @note
int32_t CTeliCamLib::stop_stream(void)
{
    int32_t ret = 0;

    m_errmsg = L""; // エラーメッセージ

    PTELI_CAM_STATUS camstat = &stat;   // カメラのステータス
    if (camstat->strmhndl == NULL) {
        ret = -1;
        m_errmsg = L"[CTeliCamLib::stop_stream]<Error>camstat->strmhndl";
    }
    else {
        //----------------------------------------------------------------------------
        // 画像ストリームの転送停止をカメラに要求
        if ((camstat->apistat = Teli::Strm_Stop(camstat->strmhndl)) != Teli::CAM_API_STS_SUCCESS) {
            ret = -2;

            wos_msg.str(L""); wos_msg << L"[CTeliCamLib::stop_stream]<Error>Teli::Strm_Stop(" << camstat->apistat << ")";
            m_errmsg = wos_msg.str();
        }
    }   // if (camstat->hstrm == NULL) else
	// 情報の初期化
    camstat->framecount = 0;            // FPSを計算するためのフレームカウンター
    camstat->frameidx = 0;              // カメラから出力されるビデオデータのフレーム番号
    camstat->frameidx_valid = FALSE;    // "frameidx"に有効な値があることを示すフラグ
    camstat->fps = 0.0;                 // 実際のFPS
 
    return ret;
}

/// @brief カメラのビデオストリームのピクセル形式の設定
/// @param [in] pixelformat - ピクセルフォーマット(CAM_PIXEL_FORMAT)
/// @return 結果(0:成功 0以外:失敗)
/// @note
int32_t CTeliCamLib::set_pixelformat(Teli::CAM_PIXEL_FORMAT pixelformat)
{
    int32_t ret = 0;

    m_errmsg = L""; // エラーメッセージ

    PTELI_CAM_STATUS camstat = &stat;   // カメラのステータス
    if (camstat->camhndl == NULL) {
        ret = -1;
        m_errmsg = L"[CTeliCamLib::set_pixelformat]<Error>camstat->camhndl";
    }
    else {
        //----------------------------------------------------------------------------
        // カメラの映像ストリームのピクセルフォーマットを設定
        if ((camstat->apistat = Teli::SetCamPixelFormat(camstat->camhndl,
            pixelformat)) != Teli::CAM_API_STS_SUCCESS) {
            ret = -2;
            wos_msg.str(L""); wos_msg << L"[CTeliCamLib::set_pixelformat]<Error>Teli::SetCamPixelFormat(" << camstat->apistat << ")";
            m_errmsg = wos_msg.str();
        }
    }   // if (camstat->camhndl == NULL) else
     return ret;
}

/// @brief カメラのROI(領域)の設定
/// @param [in] offset_x - 映像の水平方向開始位置
/// @param [in] offset_y - 映像の垂直方向開始位置
/// @param [in] width - 映像の幅
/// @param [in] height - 映像の高さ
/// @return 結果(0:成功 0以外:失敗)
/// @note
int32_t CTeliCamLib::set_camroi(uint32_t offset_x, uint32_t offset_y, uint32_t width, uint32_t height)
{
    int32_t ret = 0;

    m_errmsg = L""; // エラーメッセージ

    PTELI_CAM_STATUS camstat = &stat; // カメラのステータス
    uint32_t       camwidth = 0;                       // 映像の幅
    uint32_t       camheight = 0;                       // 映像の高さ
    if (camstat->camhndl == NULL) {
        ret = -1;
        m_errmsg = L"[CTeliCamLib::set_camroi]<Error>camstat->camhndl";
    }
    else {
        //----------------------------------------------------------------------------
        // カメラのROI(領域)を設定
        if ((camstat->apistat = Teli::SetCamRoi(camstat->camhndl,
            width, height,
            offset_x, offset_y)) != Teli::CAM_API_STS_SUCCESS) {
            ret = -2;
   
            wos_msg.str(L""); wos_msg << L"[CTeliCamLib::set_pixelformat]<Error>Teli::SetCamPixelFormat(" << camstat->apistat << ")";
            m_errmsg = wos_msg.str();
         }
        else {
            //----------------------------------------------------------------------------
            // カメラの映像の幅を取得
            if ((camstat->apistat = Teli::GetCamWidth(camstat->camhndl,
                &camwidth)) != Teli::CAM_API_STS_SUCCESS) {
                ret = -3;
  
                wos_msg.str(L""); wos_msg << L"[CTeliCamLib::set_camroi]GetCamWidth:{" << camstat->apistat << "}";
                m_errmsg = wos_msg.str();
            }
            //----------------------------------------------------------------------------
            // カメラの映像の高さを取得
            else if ((camstat->apistat = Teli::GetCamHeight(camstat->camhndl,
                &camheight)) != Teli::CAM_API_STS_SUCCESS) {
                ret = -4;

                wos_msg.str(L""); wos_msg << L"[CTeliCamLib::set_camroi]<Error>Teli::GetCamHeight(" << camstat->apistat << ")";
                m_errmsg = wos_msg.str();
            }
            else {
                ;
            }
        }
    }   // if (camstat->camhndl == NULL) else
    if (ret == 0) {
        camstat->camwidth = camwidth;  // 映像の幅
        camstat->camheight = camheight; // 映像の高さ
    }
    else {
        camstat->camwidth = 0; // 映像の幅
        camstat->camheight = 0; // 映像の高さ
    }
 
    return ret;
}

/// @brief カメラのフレームレートの設定
/// @param [in] framerate - フレームレート
/// @param [in] ctrltype - フレームレート設定(CAM_ACQ_FRAME_RATE_CTRL_TYPE)
/// @return 結果(0:成功 0以外:失敗)
/// @note
int32_t CTeliCamLib::set_framerate(float64_t framerate, Teli::CAM_ACQ_FRAME_RATE_CTRL_TYPE ctrltype)
{
    int32_t ret = 0;

    m_errmsg = L""; // エラーメッセージ

    PTELI_CAM_STATUS camstat = &stat;   // カメラのステータス
    if (camstat->camhndl == NULL) {
        ret = -1;
        m_errmsg = L"[CTeliCamLib::set_framerate]<Error>camstat->camhndl";
    }
    else {
        //----------------------------------------------------------------------------
        // カメラのフレームレートを設定
        if ((camstat->apistat = Teli::SetCamAcquisitionFrameRateControl(camstat->camhndl,
            ctrltype)) != Teli::CAM_API_STS_SUCCESS) {
            ret = -2;
 
            wos_msg.str(L""); wos_msg << L"[CTeliCamLib::set_framerate]<Error>Teli::SetCamAcquisitionFrameRateControl(" << camstat->apistat << ")";
            m_errmsg = wos_msg.str();
         }
        else if ((camstat->apistat = Teli::SetCamAcquisitionFrameRate(camstat->camhndl,
            framerate)) != Teli::CAM_API_STS_SUCCESS) {
            ret = -3;

            wos_msg.str(L""); wos_msg << L"[CTeliCamLib::set_framerate]<Error>Teli::SetCamAcquisitionFrameRate(" << camstat->apistat << ")";
            m_errmsg = wos_msg.str();
         }
        else {
            ;
        }
    }   // if (camstat->camhndl == NULL) else
    return ret;
}

/// @brief カメラのトリガー動作モードの設定
/// @param [in] triggermode - トリガ動作モード(false:トリガ動作モードOFF true:トリガ動作モードON)
/// @return 結果(0:成功 0以外:失敗)
/// @note
int32_t CTeliCamLib::set_triggermode(bool8_t triggermode)
{
    int32_t ret = 0;

    m_errmsg = L""; // エラーメッセージ
    PTELI_CAM_STATUS camstat = &stat;   // カメラのステータス
    if (camstat->camhndl == NULL) {
        wos_msg.str(L""); wos_msg << L"[CTeliCamLib::set_triggermode]<Error>" << camstat->camhndl;
        m_errmsg = wos_msg.str(); ret = -1;
    }
    else {
        //----------------------------------------------------------------------------
        // カメラのトリガ動作モード(TriggerMode)を設定
        if ((camstat->apistat = Teli::SetCamTriggerMode(camstat->camhndl,triggermode)) != Teli::CAM_API_STS_SUCCESS) {
            wos_msg.str(L""); wos_msg << L"[CTeliCamLib::set_triggermode]<Error>Teli::SetCamTriggerMode(" << camstat->apistat << ")";
            m_errmsg = wos_msg.str(); ret = -2;
        }
    }   // if (camstat->camhndl == NULL) else	
 
    return ret;
}

/// @brief カメラの黒レベルの設定
/// @param [in] blacklevel - 黒レベル
/// @return 結果(0:成功 0以外:失敗)
/// @note
int32_t CTeliCamLib::set_blacklevel(float64_t blacklevel)
{
    int32_t ret = 0;

    m_errmsg = L""; // エラーメッセージ

    PTELI_CAM_STATUS camstat = &stat;   // カメラのステータス
    if (camstat->camhndl == NULL) {
        ret = -1;
        m_errmsg = L"[CTeliCamLib::set_blacklevel]<Error>camstat->camhndl";
    }
    else {
        //----------------------------------------------------------------------------
        // カメラの黒レベルの最小値と最大値を取得
        float64_t valmin, valmax;
        if ((camstat->apistat = Teli::GetCamBlackLevelMinMax(camstat->camhndl,
            &valmin,
            &valmax)) != Teli::CAM_API_STS_SUCCESS) {
            ret = -2;

            wos_msg.str(L""); wos_msg << L"[CTeliCamLib::set_blacklevel]<Error>Teli::GetCamBlackLevelMinMax(" << camstat->apistat << ")";
            m_errmsg = wos_msg.str();
         }
        //----------------------------------------------------------------------------
        // カメラの黒レベルを設定
        else {
            if (blacklevel < valmin) {
                blacklevel = valmin;
            }
            else if (blacklevel > valmax) {
                blacklevel = valmax;
            }
            else {
                ;
            }
            if ((camstat->apistat = Teli::SetCamBlackLevel(camstat->camhndl,
                blacklevel)) != Teli::CAM_API_STS_SUCCESS) {
                ret = -3;
              
                wos_msg.str(L""); wos_msg << L"[CTeliCamLib::set_blacklevel]<Error>Teli::SetCamBlackLevel(" << camstat->apistat << ")";
                m_errmsg = wos_msg.str();
            }
        }
    }   // if (camstat->camhndl == NULL) else
    return ret;
}

/// @brief カメラのガンマ補正値の設定
/// @param [in] gamma - ガンマ補正値
/// @return 結果(0:成功 0以外:失敗)
/// @note
int32_t CTeliCamLib::set_gamma(float64_t gamma)
{
    int32_t ret = 0;

    m_errmsg = L""; // エラーメッセージ

    PTELI_CAM_STATUS camstat = &stat;   // カメラのステータス
    if (camstat->camhndl == NULL) {
        ret = -1;
        m_errmsg = L"[CTeliCamLib::set_gamma]<Error>camstat->camhndl";
    }
    else {
        //----------------------------------------------------------------------------
        // カメラのガンマ補正値の最小値と最大値を取得
        float64_t valmin, valmax;
        if ((camstat->apistat = Teli::GetCamGammaMinMax(camstat->camhndl,
            &valmin,
            &valmax)) != Teli::CAM_API_STS_SUCCESS) {
            ret = -2;
 
            wos_msg.str(L""); wos_msg << L"[CTeliCamLib::set_gamma]<Error>Teli::GetCamGammaMinMax(" << camstat->apistat << ")";
            m_errmsg = wos_msg.str();
            //m_errmsg = std::format(L"[CTeliCamLib::set_gamma]<Error>Teli::GetCamGammaMinMax({:#08x})", camstat->apistat);
        }
        //----------------------------------------------------------------------------
        // カメラのガンマ補正値を設定
        else {
            if (gamma < valmin) {
                gamma = valmin;
            }
            else if (gamma > valmax) {
                gamma = valmax;
            }
            else {
                ;
            }
            if ((camstat->apistat = Teli::SetCamGamma(camstat->camhndl,
                gamma)) != Teli::CAM_API_STS_SUCCESS) {
                ret = -3;

                wos_msg.str(L""); wos_msg << L"[CTeliCamLib::set_gamma]<Error>Teli::SetCamGamma(" << camstat->apistat << ")";
                m_errmsg = wos_msg.str();
            }
        }
    }   // if (camstat->camhndl == NULL) else

    return ret;
}

/// @brief カメラのホワイトバランスゲイン自動調整モードの設定
/// @param [in] autotype - ホワイトバランスゲイン自動調整モード(CAM_BALANCE_WHITE_AUTO_TYPE)
/// @return 結果(0:成功 0以外:失敗)
/// @note
int32_t CTeliCamLib::set_wbalance_auto(Teli::CAM_BALANCE_WHITE_AUTO_TYPE autotype)
{
    int32_t ret = 0;

    m_errmsg = L""; // エラーメッセージ

    PTELI_CAM_STATUS camstat = &stat;   // カメラのステータス
    if (camstat->camhndl == NULL) {
        ret = -1;
        m_errmsg = L"[CTeliCamLib::set_wbalance_auto]<Error>camstat->camhndl";
    }
    else {
        //----------------------------------------------------------------------------
        // カメラのホワイトバランスゲイン自動調整モードを設定
        if ((camstat->apistat = Teli::SetCamBalanceWhiteAuto(camstat->camhndl,
            autotype)) != Teli::CAM_API_STS_SUCCESS) {
            ret = -2;
            wos_msg.str(L""); wos_msg << L"[CTeliCamLib::set_wbalance_auto]<Error>Teli::SetCamBalanceWhiteAuto(" << camstat->apistat << ")";
            m_errmsg = wos_msg.str();
            //m_errmsg = std::format(L"[CTeliCamLib::set_wbalance_auto]<Error>Teli::SetCamBalanceWhiteAuto({:#08x})", camstat->apistat);
        }
    }   // if (camstat->camhndl == NULL) else	

    return ret;
}

/// @brief カメラのホワイトバランスゲイン(倍率)の設定
/// @param [in] wbratio - ホワイトバランスゲイン(倍率)
/// @param [in] selectortype - ホワイトバランスゲイン設定の対象となる要素(CAM_BALANCE_RATIO_SELECTOR_TYPE)
/// @return 結果(0:成功 0以外:失敗)
/// @note
int32_t CTeliCamLib::set_wbalance_ratio(float64_t wbratio, Teli::CAM_BALANCE_RATIO_SELECTOR_TYPE selectortype)
{
    int32_t ret = 0;

     m_errmsg = L""; // エラーメッセージ

    PTELI_CAM_STATUS camstat = &stat;   // カメラのステータス
    if (camstat->camhndl == NULL) {
        ret = -1;
        m_errmsg = L"[CTeliCamLib::set_wbalance_ratio]<Error>camstat->camhndl";
    }
    else {
        //----------------------------------------------------------------------------
        // カメラのホワイトバランスゲイン自動調整モードがManualに設定されているときの、ゲインの最小値と最大値を取得
        float64_t valmin, valmax;
        if ((camstat->apistat = Teli::GetCamBalanceRatioMinMax(camstat->camhndl,
            selectortype,
            &valmin,
            &valmax)) != Teli::CAM_API_STS_SUCCESS) {
            ret = -2;
            wos_msg.str(L""); wos_msg << L"[CTeliCamLib::set_wbalance_ratio]<Error>Teli::GetCamBalanceRatioMinMax(" << camstat->apistat << ")";
            m_errmsg = wos_msg.str();
        }
        //----------------------------------------------------------------------------
        // カメラのホワイトバランスゲイン(倍率)を設定
        else {
            if (wbratio < valmin) {
                wbratio = valmin;
            }
            else if (wbratio > valmax) {
                wbratio = valmax;
            }
            else {
                ;
            }
            if ((camstat->apistat = Teli::SetCamBalanceRatio(camstat->camhndl,
                selectortype,
                wbratio)) != Teli::CAM_API_STS_SUCCESS) {
                ret = -3;
                wos_msg.str(L""); wos_msg << L"[CTeliCamLib::set_wbalance_ratio]<Error>Teli::SetCamBalanceRatio(" << camstat->apistat << ")";
                m_errmsg = wos_msg.str();
             }
        }
    }   // if (camstat->camhndl == NULL) else

    return ret;
}

/// @brief カメラのAGC(Automatic gain control)動作モードの設定
/// @param [in] autotype - AGC動作モード(CAM_GAIN_AUTO_TYPE)
/// @return 結果(0:成功 0以外:失敗)
/// @note
int32_t CTeliCamLib::set_gain_auto(Teli::CAM_GAIN_AUTO_TYPE autotype)
{
    int32_t ret = 0;

    m_errmsg = L""; // エラーメッセージ

    PTELI_CAM_STATUS camstat = &stat;   // カメラのステータス
    if (camstat->camhndl == NULL) {
        ret = -1;
        m_errmsg = L"[CTeliCamLib::set_gain_auto]<Error>camstat->camhndl";
    }
    else {
        //----------------------------------------------------------------------------
        // カメラのAGC(Automatic gain control)動作モードを設定
        if ((camstat->apistat = Teli::SetCamGainAuto(camstat->camhndl,
            autotype)) != Teli::CAM_API_STS_SUCCESS) {
            ret = -2;
            wos_msg.str(L""); wos_msg << L"[CTeliCamLib::set_gain_auto]<Error>Teli::SetCamGainAuto(" << camstat->apistat << ")";
            m_errmsg = wos_msg.str();
         }
        //----------------------------------------------------------------------------
        // カメラのAGC動作モードがManualに設定されているときの、ゲインの最小値と最大値を取得
        else if ((camstat->apistat = Teli::GetCamGainMinMax(camstat->camhndl,
            &camstat->gainmin,
            &camstat->gainmax)) != Teli::CAM_API_STS_SUCCESS) {
            ret = -3;
 
            wos_msg.str(L""); wos_msg << L"[CTeliCamLib::set_gain_auto]<Error>Teli::GetCamGainMinMax(" << camstat->apistat << ")";
            m_errmsg = wos_msg.str();
        }
        else {
            ;
        }
    }   // if (camstat->CamHNdl == NULL) else
    return ret;
}

/// @brief カメラのゲインの設定(APIへの設定はスレッドで実行される)
/// @param [in] gain - ゲイン
/// @return 結果(0:成功 0以外:失敗)
/// @note
int32_t CTeliCamLib::set_gain(float64_t gain)
{
    int32_t ret = 0;

    m_errmsg = L""; // エラーメッセージ

    PTELI_CAM_STATUS camstat = &stat;   // カメラのステータス
    if (camstat->camhndl == NULL) {
        m_errmsg = L"[CTeliCamLib::set_gain]<Error>camstat->camhndl";ret = -1;
    }
    else if (camstat->apistat != Teli::CAM_API_STS_SUCCESS) {   // TeliCamAPIのエラー
        wos_msg.str(L""); wos_msg << L"[CTeliCamLib::set_gain]<Error>camstat->apistat(" << camstat->apistat << ")";
        m_errmsg = wos_msg.str();ret = -2;
     }
    else if (camstat->errstat != Teli::CAM_API_STS_SUCCESS) {   // エラーコールバックで検出のストリーム受信時のエラー
        wos_msg.str(L""); wos_msg << L"[CTeliCamLib::set_gain]<Error>camstat->errstat(" << camstat->apistat << ")";
        m_errmsg = wos_msg.str();ret = -3;
    }
    else {
        // カメラのゲインを設定(APIへの設定はスレッドで実行される)
        if (gain < camstat->gainmin)  gain = camstat->gainmin;
        if (gain > camstat->gainmax)  gain = camstat->gainmax;
         camstat->gain = gain;
    } 
    return ret;
}

/// @brief カメラの露光時間の制御モードの設定
/// @param [in] ctrltype - 露光時間の制御モード(CAM_EXPOSURE_TIME_CONTROL_TYPE)
/// @return 結果(0:成功 0以外:失敗)
/// @note
int32_t CTeliCamLib::set_expstime_control(Teli::CAM_EXPOSURE_TIME_CONTROL_TYPE ctrltype)
{
    int32_t ret = 0;

    m_errmsg = L""; // エラーメッセージ

    PTELI_CAM_STATUS camstat = &stat;   // カメラのステータス
    if (camstat->camhndl == NULL) {
        ret = -1;
        m_errmsg = L"[CTeliCamLib::set_expstime_control]<Error>camstat->camhndl";
    }
    else {
        //----------------------------------------------------------------------------
        // カメラの露光時間の制御モードを設定
        if ((camstat->apistat = Teli::SetCamExposureTimeControl(camstat->camhndl,
            ctrltype)) != Teli::CAM_API_STS_SUCCESS) {
            ret = -2;
  
            wos_msg.str(L""); wos_msg << L"[CTeliCamLib::set_expstime_control]<Error>Teli::SetCamExposureTimeControl(" << camstat->apistat << ")";
            m_errmsg = wos_msg.str();
           // m_errmsg = std::format(L"[CTeliCamLib::set_expstime_control]<Error>Teli::SetCamExposureTimeControl({:#08x})", camstat->apistat);
        }
        //----------------------------------------------------------------------------
        // カメラの露光時間制御モードがManualに設定されているときの、露光時間の最小値と最大値を取得
        else if ((camstat->apistat = Teli::GetCamExposureTimeMinMax(camstat->camhndl,
            &camstat->expstime_min,
            &camstat->expstime_max)) != Teli::CAM_API_STS_SUCCESS) {
            ret = -3;

            wos_msg.str(L""); wos_msg << L"[CTeliCamLib::set_expstime_control]<Error>Teli::GetCamExposureTimeMinMax(" << camstat->apistat << ")";
            m_errmsg = wos_msg.str();
          //  m_errmsg = std::format(L"[CTeliCamLib::set_expstime_control]<Error>Teli::GetCamExposureTimeMinMax({:#08x})", camstat->apistat);
        }
        else {
            ;
        }
    }   // if (camstat->camhndl == NULL) else
    return ret;
}

/// @brief カメラの露光時間の設定(APIへの設定はスレッドで実行される)
/// @param [in] expstime - Exposure time
/// @return 結果(0:成功 0以外:失敗)
/// @note
int32_t CTeliCamLib::set_expstime(float64_t expstime)
{
    int32_t ret = 0;
    m_errmsg = L""; // エラーメッセージ
    PTELI_CAM_STATUS camstat = &stat;   // Camera status
    if (camstat->camhndl == NULL) {
        m_errmsg = L"[CTeliCamLib::set_expstime]<Error>camstat->camhndl";ret = -1;
    }
    else if (camstat->apistat != Teli::CAM_API_STS_SUCCESS) {   // TeliCamAPIのエラー
        wos_msg.str(L""); wos_msg << L"[CTeliCamLib::set_expstime]<Error>camstat->apistat(" << camstat->apistat << ")";
        m_errmsg = wos_msg.str();ret = -2;
    }
    else if (camstat->errstat != Teli::CAM_API_STS_SUCCESS) {   // 画像ストリーム受信時のエラー
        wos_msg.str(L""); wos_msg << L"[CTeliCamLib::set_expstime]<Error>camstat->errstat(" << camstat->apistat << ")";
        m_errmsg = wos_msg.str();ret = -3;
    }
    else {
        //----------------------------------------------------------------------------
        // カメラの露光時間を設定(APIへの設定はスレッドで実行される)
        if (expstime < camstat->expstime_min) expstime = camstat->expstime_min;
        if (expstime > camstat->expstime_max) expstime = camstat->expstime_max;
        camstat->expstime = expstime;
    }
    return ret;
}

/// @brief 画像情報を取得
/// @param [in] bufsize - Copy destination buffer size 
/// @param [out] image - Image pointer
/// @return 結果(0:成功 0以外:失敗)
/// @note
 
/// <summary>
/// 画像取り込みバッファにサイズを指定して画像をコピーする
/// #　現状使っていない　コピーサイズをしていしないタイプを使用
/// </summary>
/// <scenario>
/// 1.エラーチェック
/// 2.最新のバッファインデクスを取得
/// 3.バッファをロックして画像情報を取得
/// </scenario>
/// <param name="bufsize"></param>
/// <param name="image"></param>
/// <returns></returns>
int32_t CTeliCamLib::get_image(uint32_t bufsize, uint8_t* image)
{
    int32_t ret = 0;
     m_errmsg = L""; // エラーメッセージ

    if (image == NULL) {
        ret = -1;
        m_errmsg = L"[CTeliCamLib::get_image]<Error>image";
    }
    else if ((stat.camhndl == NULL) || (stat.strmhndl == NULL)) { // // Handle of camera assigned by TeliCamAPI on opening camera and opening stream channel
        m_errmsg = L"[CTeliCamLib::get_image]<Error>stat.camhndl/strmhndl";ret = -2;
    }
    //コールバック関数でエラーが無いことをチェックしてCAM_API_STS_SUCCESSをセットしている
    else if (stat.apistat != Teli::CAM_API_STS_SUCCESS) {   // TeliCamAPIのエラー
        wos_msg.str(L""); wos_msg << L"[CTeliCamLib::get_image]<Error>stat.apistat error(" << stat.apistat << ")";
        m_errmsg = wos_msg.str();ret = -3;
    }
    else if (stat.errstat != Teli::CAM_API_STS_SUCCESS) {   // 画像ストリーム受信時のエラー
        ret = -4;
        wos_msg.str(L""); wos_msg << L"[CTeliCamLib::get_image]<Error>stat.errstat(" << stat.apistat << ")";
        m_errmsg = wos_msg.str();
       // m_errmsg = std::format(L"[CTeliCamLib::get_image]<Error>stat.errstat({:#08x})", stat.errstat);
    }
    else {
        if (!stat.frameidx_valid) {
            m_errmsg = L"[CTeliCamLib::get_image]<Error>stat.frameidx_valid";ret = -5;
        }
        else {  // Image received
            uint32_t rbufidx;   // Buffer index of the acquired stream request ring buffer
            //----------------------------------------------------------------------------
            // Gets the buffer index of the stream request ring buffer inside TeliCam API that stores the latest stream request (image).
            if ((stat.apistat = Teli::Strm_GetCurrentBufferIndex(stat.strmhndl, &rbufidx)) != Teli::CAM_API_STS_SUCCESS) {
                wos_msg.str(L""); wos_msg << L"[CTeliCamLib::get_image]<Error>Teli::Strm_GetCurrentBufferIndex(" << stat.apistat << ")";
                m_errmsg = wos_msg.str(); ret = -6;
             }
            else {
                Teli::CAM_IMAGE_INFO imginfo;   // Accompanying information of the image stored in the stream request ring buffer to be locked
                //----------------------------------------------------------------------------
                // Get image pointer in Ring Buffer.
                // Image data is locked in Strm_LockApiRingBufferPointer() method.
                if ((stat.apistat = Teli::Strm_LockBuffer(stat.strmhndl, rbufidx, &imginfo)) != Teli::CAM_API_STS_SUCCESS) {
                    wos_msg.str(L""); wos_msg << L"[CTeliCamLib::get_image]<Error>Teli::Strm_LockBuffer(" << stat.apistat << ")";
                    m_errmsg = wos_msg.str();ret = -7;
                  }
                else if (imginfo.uiStatus != Teli::CAM_API_STS_SUCCESS) {
                    stat.apistat = imginfo.uiStatus;    // TeliCamAPI status
                    wos_msg.str(L""); wos_msg << L"[CTeliCamLib::get_image]<Error>Teli::Strm_LockBuffer(" << stat.apistat << ")";
                    m_errmsg = wos_msg.str();ret = -8;
                  }
                else if (stat.pyldsize > bufsize) {
                     m_errmsg = L"[CTeliCamLib::get_image]<Error>bufsize";ret = -9;
                }
                else {
                    CopyMemory(image, imginfo.pvBuf, stat.pyldsize);
                    
					if (stat.framechk_valid) {// フレームレート損失チェックタイミングフラグがセットされているとき
                        if (stat.fps <= cnfg.framerate_drop) {
                            m_errmsg = L"[CTeliCamLib::get_image]<Error>Frame rate drop"; ret = -10;
                        }
                        stat.framechk_valid = FALSE; // フレームレート損失チェックタイミングフラグクリア 
                    }
                 }
                //----------------------------------------------------------------------------
                // Unlock current image
                Teli::CAM_API_STATUS apistat = Teli::Strm_UnlockBuffer(stat.strmhndl, rbufidx);
                if (apistat != Teli::CAM_API_STS_SUCCESS) {
                    if (stat.apistat == Teli::CAM_API_STS_SUCCESS) {    //SUCCESS->NotSuccessトリガチェック
                        stat.apistat = apistat;                         // NotSuccessのステータスコードに更新
                        wos_msg.str(L""); wos_msg << L"[CTeliCamLib::get_image]<Error>Teli::Strm_UnlockBuffer(" << stat.apistat << ")";
                        m_errmsg = wos_msg.str();ret = -11;
                    }
                }
            }   // if ((stat.apistat = Teli::Strm_GetCurrentBufferIndex(stat.strmhndl, &rbufidx)) != Teli::CAM_API_STS_SUCCESS) else
        }   // if (!stat.frameidx_valid) else
    }   // if (image == NULL) ... else

    //----------------------------------------------------------------------------
    // Check interval (fps)
	uint32_t fpstimer = timeGetTime() - stat.fpstimer;// 経過時間
    if (fpstimer > CHECK_FPS_INTERVAL_TIME) {
        stat.fps = ((float64_t)stat.framecount / (float64_t)fpstimer) * 1000.0;
        stat.framecount = 0;
        stat.fpstimer = timeGetTime();
		stat.framechk_valid = TRUE;         // フレームレート損失チェックタイミングフラグをセット
    }
    return ret;
}

/// <summary>
/// 処理用の画像を取得する(引数の
/// </summary>
/// <scenario>
/// １．エラーチェック
/// ２．最新のバッファインデクスを取得
/// ３．バッファをロックして画像情報を取得
/// ４．元データByrBG8をBGRに変換して引数のimageにコピー
/// ５．フレームレート損失チェック
/// ５．バッファをアンロック
/// </scenario>
/// <param name="image"></param>
/// <returns></returns>
int32_t CTeliCamLib::get_image(void* image)
{
    int32_t ret = 0;
     m_errmsg = L""; // エラーメッセージ
	 stat.is_img_valid = is_stat_img_valid(); // 画像受信状態のチェック
      
	if (image == NULL) {// 引数のimageがNULL
        ret = -1;  
    }
    else if (!stat.is_img_valid) {
        ret = -2; 
    }
    else {
        uint32_t rbufidx;   // Buffer index of the acquired stream request ring buffer
        //----------------------------------------------------------------------------
        // Gets the buffer index of the stream request ring buffer inside TeliCam API that stores the latest stream request (image).
        if ((stat.apistat = Teli::Strm_GetCurrentBufferIndex(stat.strmhndl, &rbufidx)) != Teli::CAM_API_STS_SUCCESS) {
            ret = -6;
        }
        else {
            Teli::CAM_IMAGE_INFO imginfo;   // Strm_LockBufferで獲得する画像の情報を格納する為の変数
            //----------------------------------------------------------------------------
            //TeliCamAPI 内部のストリームリクエスト リングバッファの指定先をロックし、画像情報を取得します。
            if ((stat.apistat = Teli::Strm_LockBuffer(stat.strmhndl, rbufidx, &imginfo)) != Teli::CAM_API_STS_SUCCESS)  ret = -7;
            else if (imginfo.uiStatus != Teli::CAM_API_STS_SUCCESS) {
                stat.apistat = imginfo.uiStatus;    ret = -8;    // TeliCamAPI status
            }
            else if ((stat.apistat = Teli::ConvByrBG8ToBGR(image, imginfo.pvBuf, imginfo.uiSizeX, imginfo.uiSizeY)) != Teli::CAM_API_STS_SUCCESS) {
                ret = -9;
            }
            else {//正常受信
				if (stat.framechk_valid) { // フレームレート損失チェックタイミングフラグがセットされているとき
                    if (stat.fps <= cnfg.framerate_drop)    stat.is_fps_valid = false;
                    else                                    stat.is_fps_valid = true;
                    stat.framechk_valid = FALSE; //フレームレート損失チェックタイミングフラグクリア
                }
            }

            // Unlock current image
            Teli::CAM_API_STATUS apistat = Teli::Strm_UnlockBuffer(stat.strmhndl, rbufidx);
            if (apistat != Teli::CAM_API_STS_SUCCESS) {
                if (stat.apistat == Teli::CAM_API_STS_SUCCESS) {    //SUCCESS->NotSuccessトリガチェック
                    stat.apistat = apistat;                         // TeliCamAPIのステータスコード
                }
            }
        }   // if ((stat.apistat = Teli::Strm_GetCurrentBufferIndex(stat.strmhndl, &rbufidx)) != Teli::CAM_API_STS_SUCCESS) else
    }   // if (image == NULL) ... else

    //----------------------------------------------------------------------------
    // Check interval (fps)
    uint32_t       fpstimer = timeGetTime() - stat.fpstimer;
    const uint32_t interval = CHECK_FPS_INTERVAL_TIME;  // fpsをチェックする間隔[ms]
    if (fpstimer > interval) {
        //framecountは、フレームを受信するたびにコールバック関数でインクリメントされる
        stat.fps = ((float64_t)stat.framecount / (float64_t)fpstimer) * 1000.0;
        stat.framecount = 0;
        stat.fpstimer = timeGetTime();
        stat.framechk_valid = TRUE; // フレームレート損失チェック 
    }
    return ret;
}

/// @brief 画像サイズの取得
/// @param [out] width - Image width
/// @param [out] height - Image height
/// @return 結果(0:成功 0以外:失敗)
/// @note
int32_t CTeliCamLib::get_image_size(uint32_t* width, uint32_t* height)
{
    int32_t ret = 0;

    m_errmsg = L""; // エラーメッセージ

    if ((width == NULL) || (height == NULL)) {
         m_errmsg = L"[CTeliCamLib::get_image_size]<Error>width/height";ret = -1;
    }
    else if (stat.camhndl == NULL) {    // Handle of camera assigned by TeliCamAPI on opening camera
        *width = 0; *height = 0;
        m_errmsg = L"[CTeliCamLib::get_image_size]<Error>camhndl";ret = -2;
    }
    else if (stat.apistat != Teli::CAM_API_STS_SUCCESS) {   // TeliCamAPIステータスエラー
        *width = 0; *height = 0;
        wos_msg.str(L""); wos_msg << L"[CTeliCamLib::get_image_size]<Error>apistat(" << stat.apistat << ")";
        m_errmsg = wos_msg.str();ret = -3;
    }
    else {
        *width  = stat.camwidth;    // 映像の幅
        *height = stat.camheight;   // 映像の高さ
    }
    return ret;
}

/// @brief フレームレートの取得
/// @param [out] fps - Actual frame rate
/// @return 結果(0:成功 0以外:失敗)
/// @note
int32_t CTeliCamLib::get_image_fps(float64_t* fps)
{
    int32_t ret = 0;
    m_errmsg = L""; // エラーメッセージ
    if (fps == NULL) {
         m_errmsg = L"[CTeliCamLib::get_image_fps]<Error>fps";ret = -1;
    }
    else if ((stat.camhndl == NULL) || // Handle of camera assigned by TeliCamAPI on opening camera
        (stat.strmhndl == NULL)) { // Handle of stream channel assigned by TeliCamAPI on opening stream channel
        *fps = 0.0;
         m_errmsg = L"[CTeliCamLib::get_image_fps]<Error>stat.camhndl/strmhndl"; ret = -2;
    }
    else if (stat.apistat != Teli::CAM_API_STS_SUCCESS) {   // TeliCamAPI status
        *fps = 0.0;ret = -3;
        wos_msg.str(L""); wos_msg << L"[CTeliCamLib::get_image_fps]<Error>stat.apistat(" << stat.apistat << ")";
        m_errmsg = wos_msg.str();
     }
    else {
        *fps = stat.fps;
    }
    return ret;
}

/// @brief カメラ情報の取得
/// @param 
/// @return カメラの情報
/// @note
Teli::CAM_INFO CTeliCamLib::get_stat_caminfo(void)
{
    return m_caminfo;
}

/// @brief カメラ名の取得
/// @param 
/// @return カメラの名前
/// @note
std::wstring CTeliCamLib::get_camera_name(void)
{
    return stat.camname;
}

/// @brief エラーメッセージの取得
/// @param 
/// @return エラーメッセージ
/// @note
std::wstring CTeliCamLib::get_error_message(void)
{
    return m_errmsg;
}

//////////////////////////////////////////////////////////////////////////////
// Private method

/// @brief カメラをオープンしたときのカメラのインデックスの取得
/// @param [in] camidx - Camera index when the camera is opened
/// @return Camera index
/// @note
int32_t CTeliCamLib::get_caminfo_camindex(uint32_t camidx)
{
    int32_t val = -1;

    if (stat.camidx == camidx) {
        val = camidx;
    }

    return val;
}

bool CTeliCamLib::is_stat_img_valid() {
    if ((stat.camhndl == NULL) || (stat.strmhndl == NULL)) { // Handle of camera assigned by TeliCamAPI on opening camera or opening stream channel
        return false;
    }
    if ((stat.apistat != Teli::CAM_API_STS_SUCCESS)||
        (stat.errstat != Teli::CAM_API_STS_SUCCESS)||
        !stat.frameidx_valid
       ) {   // TeliCamAPI status
        return false;
    }
	return true;
}

/// <summary>
/// 受信した画像を処理するためのコールバック関数
/// </summary>
/// <scenario>
/// 1.カメラハンドルからカメラのインデックスを取得する
/// 2.カメラのインデックスが正しい場合、フレーム番号とフレームカウントを更新する
/// </scenario>
/// <param name="hCam">         Apiがセットする受信画像のカメラのハンドル</param>
/// <param name="hStrm">        Apiがセットする受信画像のストリームのハンドル</param>
/// <param name="psImageInfo">  Apiがセットする受信画像(画像データのポインタ）とその付随情報</param>
/// <param name="uiBufferIndex">Apiがセットするt] psImageInfo のデータが保存されているバッファのストリームリクエストリングバッファ内INDEX</param>
/// <param name="pvContext">    コールバック関数を実行するときに引数として渡すオブジェクトへのポインタ　コールバックセット時のパラメータ</param>
void CTeliCamLib::cb_image_acquired(Teli::CAM_HANDLE hCam, Teli::CAM_STRM_HANDLE hStrm, Teli::PCAM_IMAGE_INFO psImageInfo, uint32_t uiBufferIndex, void* pvContext)
{
	// コールバック関数セット時のパラメータ(pvContext：CTeliCamLibオブジェクトのポインタ)をアクセスポインタに変換
      
    Teli::CAM_API_STATUS apistat = Teli::CAM_API_STS_SUCCESS;  // TeliCamAPIのステータスコード
    uint32_t             camidx;                               // Camera index when the camera is opened

     // Get camera index from camera handle
    if ((apistat = Teli::GetCamIndexFromCamHandle(hCam, &camidx)) != Teli::CAM_API_STS_SUCCESS) {
        if (get_caminfo_camindex(camidx) >= 0) {//取得したカメラのIDがオブジェクトで保持していたIDと一致
            stat.apistat = apistat;   // TeliCamAPIのステータスコード
        }
        return;
    }
    else {
        stat.apistat = Teli::CAM_API_STS_SUCCESS;      // TeliCamAPIのステータスコード
    }

    //
	if (get_caminfo_camindex(camidx) >= 0) {//取得したカメラのIDがオブジェクトで保持していたIDと一致していたらフレーム番号とフレームカウントを更新する
 
        stat.frameidx_valid = TRUE;
        stat.framecount++;
        stat.frameidx = (uint32_t)(psImageInfo->ullBlockId); // カメラから出力されるビデオデータのフレーム番号
    }
    else {
        stat.frameidx_valid = FALSE;
        stat.framecount = 0;
        stat.frameidx = 0; // カメラから出力されるビデオデータのフレーム番号
    }
}

/// @brief 受信エラーを処理するためのコールバック関数
/// @param [in] hCam - Handler of camera
/// @param [in] hStrm - Handler of stream
/// @param [in] uiErrorStatus - AError status
/// @param [in] uiBufferIndex - Index of buffer
/// @param [in] pvContext - Additional caller-specified context
/// @return 
/// @note
void CTeliCamLib::cb_error_image(Teli::CAM_HANDLE hCam, Teli::CAM_STRM_HANDLE hStrm, Teli::CAM_API_STATUS uiErrorStatus, uint32_t uiBufferIndex, void* pvContext)
{
    Teli::CAM_API_STATUS apistat = Teli::CAM_API_STS_SUCCESS;    // TeliCamAPIのステータスコード
    uint32_t             camidx;                                    // Camera index when the camera is opened

    //----------------------------------------------------------------------------
    // Get camera index from camera handle
    if ((apistat = Teli::GetCamIndexFromCamHandle(hCam, &camidx)) != Teli::CAM_API_STS_SUCCESS) {
        if (get_caminfo_camindex(camidx) >= 0) {
            stat.apistat = apistat;           // TeliCamAPIのステータスコード
            stat.framecount = 0;              // FPSを計算するためのフレームカウンター
            stat.frameidx = 0;                // カメラから出力されるビデオデータのフレーム番号
            stat.frameidx_valid = FALSE;      // "frameidx"に有効な値があることを示すフラグ
            stat.fpstimer = timeGetTime();    // Frame update timer
            stat.fps = 0.0;                   // 実際のFPS
        }
    }
    else {
        if (get_caminfo_camindex(camidx) >= 0) {
            switch (uiErrorStatus) {
            case Teli::CAM_API_STS_RESPONSE_TIMEOUT:
            case Teli::CAM_API_STS_BUFFER_FULL:
            case Teli::CAM_API_STS_TOO_MANY_PACKET_MISSING:
                //stat.errstat        = uiErrorStatus;    // 画像ストリーム受信時のエラーステータスコード
                //stat.framecount     = 0;                // FPSを計算するためのフレームカウンター
                //stat.frameidx       = 0;                // カメラから出力されるビデオデータのフレーム番号
                //stat.frameidx_valid = FALSE;            // "frameidx"に有効な値があることを示すフラグ
                //stat.fpstimer       = timeGetTime();    // Frame update timer
                //stat.fps            = 0.0;              // 実際のFPS
                 break;
            default:
                 break;
            }
        }
        else {
            stat.errstat = apistat;
        }
    }

   
}

