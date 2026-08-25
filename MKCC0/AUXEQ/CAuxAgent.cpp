
#include "CAuxAgent.h"
#include "CAuxPol.h"
#include "resource.h"
#include "CSHAREDMEM.H"
#include "SmemAux.H"
#include "CMCProtocol.h"
#include "CPlc.h"
#include "CComm.h"
#include <mutex>
#include "CCamera.h"
#include "SWYSENSOR_DEF.H"

#include <thread>
#include <mutex>
#include <atomic>

#include "TeliCamApi.h" //SDKのフォルダからIncフォルダにコピー
#include "TeliCamUtl.h"//SDKのフォルダからIncフォルダにコピー

using namespace Teli;

#pragma comment (lib, "Gdiplus.lib")
//#pragma comment(lib, "..\\Lib\\x64\\TeliCamApi64.lib")
//#pragma comment(lib, "..\\Lib\\x64\\TeliCamUtl64.lib")

// 組み込み機能
extern int g_slbrk_enable;//旋回ブレーキ
extern int g_lanio_enable;//LANIO
extern int g_sway_sensor_enable;//振れセンサー
extern int g_gt_sensor_enable;//走行位置検出

//Swayセンサ関連
extern CTeliCamLib* pCamera;//GEカメラオブジェクトへのグローバルポインタ
// ***アプリケーション設定アクセスポインタ
extern PCONFIG_COMMON    gp_cnfg_common;        // 共通設定
extern PCONFIG_CAMERA    gp_cnfg_camera;        // カメラ設定
extern PCONFIG_MOUNTING  gp_cnfg_mounting;      // 取付寸法設定
extern PCONFIG_IMGPROC   gp_cnfg_imageprocess;  // 画像処理条件設定

// ***アプリケーション情報アクセスポインタ  
extern PINFO_CLIENT_DATA gp_app_client;        // クライアント情報
extern PINFO_ADJUST_DATA gp_app_adjust;        // 調整情報
extern PINFO_IMGPRC_DATA gp_app_imgprc;        // 画像処理情報
extern PINFO_SYSTEM_DATA gp_app_system;        // システム情報

extern IMAGE_DATA g_img_src_work;

ST_AUXAG_MON1 CAuxAgent::st_mon1;
ST_AUXAG_MON2 CAuxAgent::st_mon2;

extern BC_TASK_ID st_task_id;
extern vector<CBasicControl*>	    VectCtrlObj;	    //スレッドオブジェクトのポインタ

extern CSharedMem* pEnvInfObj;
extern CSharedMem* pAgentInfObj;
extern CSharedMem* pCsInfObj;
extern CSharedMem* pScadInfObj;

extern ST_DEVICE_CODE g_my_code;

//ソケット
static CMCProtocol* pMCSock;				//MCプロトコルオブジェクトポインタ

//共有メモリ
static LPST_AUX_ENV_INF		pEnv_Inf	= NULL;
static LPST_AUX_CS_INF		pCS_Inf		= NULL;
static LPST_AUX_AGENT_INF	pAgent_Inf	= NULL;
static LPST_AUX_POL_INF		pAuxPolInf	= NULL;
static LPST_AUX_SCAD_INF    pAuxScadInf = NULL;

static CAuxAgent* pAgentObj;
static CAuxPol* pPolObj;

//GE Camera
std::thread g_capThread;							// スレッド変数が消えないようにグローバル領域に保持
std::atomic<bool> g_keepRunning = false;
HANDLE g_hStopEvent = NULL;							// 停止指示用イベント
HANDLE g_hGECamStreamEvent = NULL;					// カメラからのストリーム受信通知

std::unique_ptr<Bitmap>   CAuxAgent::m_pOffscreenBitmap;
std::unique_ptr<Gdiplus::Graphics> CAuxAgent::m_pOffscreenGraphics;
Graphics* CAuxAgent::pgraphic_img;					//描画用グラフィックス

static wostringstream wos_cam;

static PINT16	pOteCtrl = NULL;					//OTE操作入力信号ポインタ
static LONG rcv_count_plc_r = 0, snd_count_plc_r = 0, rcv_errcount_plc_r = 0;
static LONG rcv_count_plc_w = 0, snd_count_plc_w = 0, rcv_errcount_plc_w = 0;
static LARGE_INTEGER start_count_w, end_count_w, start_count_r, end_count_r;  //システムカウント
static LARGE_INTEGER frequency;						//システム周波数
static LONGLONG res_delay_max_w, res_delay_max_r;	//PLC応答時間

CAuxAgent::CAuxAgent() {

	// 1. GDI+ 初期化
	GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);

	m_pOffscreenBitmap = std::make_unique<Bitmap>(AUXAG_MON1_WND_W, AUXAG_MON1_WND_H, PixelFormat32bppARGB);

	// 2. そのバッファに描き込むための Graphics オブジェクトを作成
	m_pOffscreenGraphics = std::unique_ptr<Graphics>(Graphics::FromImage(m_pOffscreenBitmap.get()));

}
CAuxAgent::~CAuxAgent() {
	GdiplusShutdown(m_gdiplusToken);
	g_keepRunning = false;//USBデバイス監視スレッド終了フラグセット
	if (pCamera != nullptr) delete pCamera;

	if (g_sway_sensor_enable) {
		Teli::Sys_CloseSignal(g_hGECamStreamEvent);
		CloseHandle(g_hStopEvent);
	}

	Sleep(1000);//スレッド終了待機
}

HRESULT CAuxAgent::initialize(LPVOID lpParam){

	HRESULT hr = S_OK;
	//システム周波数読み込み
	QueryPerformanceFrequency(&frequency);

	//### 出力用共有メモリ取得
	out_size = sizeof(ST_AUX_AGENT_INF);
	set_outbuf(pAgentInfObj->get_pMap());

	//### 入力用共有メモリ取得
	pAgent_Inf	= (LPST_AUX_AGENT_INF)pAgentInfObj->get_pMap();
	pEnv_Inf	= (LPST_AUX_ENV_INF)(pEnvInfObj->get_pMap());
	pCS_Inf		= (LPST_AUX_CS_INF)pCsInfObj->get_pMap();
	pAuxScadInf = (LPST_AUX_SCAD_INF)pScadInfObj->get_pMap();

	pAgentObj	= (CAuxAgent*)VectCtrlObj[st_task_id.AGENT];

	//### GE Camera
	if (g_sway_sensor_enable) {
		gp_app_imgprc->image_loss_count=0;
		//IFウィンドウ
		if (st_mon1.hwnd_mon == NULL) {
			WPARAM wp = MAKELONG(inf.index, WM_USER_WPH_OPEN_IF_WND);//HWORD:コマンドコード, LWORD:タスクインデックス
			LPARAM lp = BC_ID_MON1;
			SendMessage(inf.hwnd_opepane, WM_USER_TASK_REQ, wp, lp);
			Sleep(1000);
		}
		if (st_mon1.hwnd_mon == NULL) {
			wos << L"Err(MON1 NULL Handle!!):";
			msg2listview(wos.str()); wos.str(L"");
			return S_FALSE;
		}
		//### 初期化
		wos.str(L"");//初期化
		if (st_mon1.hwnd_mon == NULL) {
			wos << L"Initialize : MON1 NG"; msg2listview(wos.str());
			return S_FALSE;
		}
	}
		
	//### SLBRK IFウィンドウ
	if(g_slbrk_enable){
		if (st_mon2.hwnd_mon == NULL) {
			WPARAM wp = MAKELONG(inf.index, WM_USER_WPH_OPEN_IF_WND);//HWORD:コマンドコード, LWORD:タスクインデックス
			LPARAM lp = BC_ID_MON2;
			SendMessage(inf.hwnd_opepane, WM_USER_TASK_REQ, wp, lp);
			Sleep(1000);
		}
		if (st_mon2.hwnd_mon == NULL) {
			wos << L"Err(MON2 NULL Handle!!):";
			msg2listview(wos.str()); wos.str(L"");
			return S_FALSE;
		}
		//### 初期化
		wos.str(L"");//初期化
		if (st_mon2.hwnd_mon == NULL) {
			wos << L"Initialize : MON2 NG";
			return S_FALSE;
		}
		else {
			pMCSock = new CMCProtocol(ID_SOCK_MC_AUX_BRK);

			if (pMCSock->Initialize(st_mon2.hwnd_mon, PLC_IF_TYPE_SLBRK, g_my_code.machine_id) != S_OK) {
				wos << L"Initialize : MC Init NG"; msg2listview(wos.str()); wos.str(L"");
				wos << L"Err :" << pMCSock->msg_wos.str(); msg2listview(wos.str()); wos.str(L"");
				//			return S_FALSE;
			}
			else {
				wos << L"MCProtocol Init OK"; 
			}
		}
		msg2listview(wos.str());
	}

	//モニタウィンドウテキスト	
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_MON_CHECK1, L"GE_Cam");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_MON_CHECK2, L"SL BRK");
	
	inf.panel_func_id = IDC_TASK_FUNC_RADIO1;
	SendMessage(GetDlgItem(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO1), BM_SETCHECK, BST_CHECKED, 0L);
	for (int i = 1; i < 6; i++)
		SendMessage(GetDlgItem(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO1 + i), BM_SETCHECK, BST_UNCHECKED, 0L);
	//モード設定0
	inf.mode_id = BC_ID_MODE0;
	SendMessage(GetDlgItem(inf.hwnd_opepane, IDC_TASK_MODE_RADIO0), BM_SETCHECK, BST_CHECKED, 0L);

	set_func_pb_txt();
	set_item_chk_txt();
	set_panel_tip_txt();

	return S_OK;
}

HRESULT CAuxAgent::routine_work(void* pObj){
	if (inf.total_act % 20 == 0) {
		wos.str(L""); wos << inf.status << L":" << std::setfill(L'0') << std::setw(4) << inf.act_time;
		msg2host(wos.str());
	}

	input();
	parse();
	output();
	return S_OK;
}

int CAuxAgent::input() {
	return S_OK;
}

static INT16 slbrk_healthy_hold, slbrk_healthy_cnt;
int CAuxAgent::parse() {           //メイン処理
	//旋回ブレーキ処理
	if (g_slbrk_enable) {
		//ヘルシーチェック
		if (slbrk_healthy_hold == pAgent_Inf->slbrk_rbuf[0]) {
			if (!(slbrk_healthy_cnt & 0xF000)) slbrk_healthy_cnt++;
		}
		else slbrk_healthy_cnt = 0;
		slbrk_healthy_hold = pAgent_Inf->slbrk_rbuf[0];

		//ヘルシー異常検出
		if (slbrk_healthy_cnt >= 50) pCS_Inf->fb_slbrk.healthy_err = L_ON;
		else                         pCS_Inf->fb_slbrk.healthy_err = L_OFF;
	}
	

	//GE Camera
	if (g_sway_sensor_enable){
		if ((!g_keepRunning) && (pAgent_Inf->st_ge_cam.retry_count == 0)) {
			camera_capture_start();
		}
	}

	return STAT_OK;
}
int CAuxAgent::output() {          //出力処理
	//### MAINプロセスへ出力
	
	if (g_slbrk_enable) {
		LPST_PLC_RBUF_SBRK0 pfb = (LPST_PLC_RBUF_SBRK0)pAgent_Inf->slbrk_rbuf;

		pCS_Inf->fb_slbrk.d16 = pAgent_Inf->slbrk_rbuf[0];			//旋回ブレーキフィードバックD16
		pCS_Inf->fb_slbrk.d17 = pAgent_Inf->slbrk_rbuf[1];			//旋回ブレーキフィードバックD17
		pCS_Inf->fb_slbrk.d18 = pAgent_Inf->slbrk_rbuf[2];			//旋回ブレーキフィードバックD18
		pCS_Inf->fb_slbrk.d19 = pAgent_Inf->slbrk_rbuf[3];			//旋回ブレーキフィードバックD19
		pCS_Inf->fb_slbrk.d20 = pfb->fb_WF_D20;						//旋回ブレーキフィードバックD20

		pCS_Inf->aux_helthy_cnt++;

		pCS_Inf->fb_slbrk.brk_fb_level = pAgent_Inf->slbrk_rbuf[0] & 0x000F;	//旋回ブレーキフィードバックレベル
		pCS_Inf->fb_slbrk.brk_fb_hw_brk = pAgent_Inf->slbrk_rbuf[0] & 0x0010;	//旋回ブレーキフィードバックHWブレーキ
		pCS_Inf->fb_slbrk.brk_fb_autosel = pAgent_Inf->slbrk_rbuf[0] & 0x0080;	//旋回ブレーキフィードバックAutoMode
		pCS_Inf->fb_slbrk.brk_fb_emg = pAgent_Inf->slbrk_rbuf[0] & 0x0040;	//旋回ブレーキフィードバック非常停止
		pCS_Inf->fb_slbrk.brk_fb_time_over = pAgent_Inf->slbrk_rbuf[1] & 0x0010;	//旋回ブレーキフィードバックタイムオーバー
		pCS_Inf->fb_slbrk.brk_fb_release = pAgent_Inf->slbrk_rbuf[0] & 0x0100;	//旋回ブレーキフィードバック解除

		pCS_Inf->fb_slbrk.brk_fb_sys_err = pCS_Inf->fb_slbrk.d17 & 0x000F;	//旋回ブレーキフィードバックシステム異常
		pCS_Inf->fb_slbrk.brk_fb_karaburi = pCS_Inf->fb_slbrk.d20 & 0x0020;	//旋回ブレーキフィードバック空振
		pCS_Inf->fb_slbrk.brk_fb_org_pt = pCS_Inf->fb_slbrk.d20 & 0x0002;	//旋回ブレーキフィードバック原点復帰
		pCS_Inf->fb_slbrk.brk_fb_rbsl_pos = pCS_Inf->fb_slbrk.d19;				//旋回ブレーキフィードバック位置

		//### 旋回ブレーキシステムへ出力
		if (!st_mon2.slbrk_dbg_mode) {
			pAgent_Inf->slbrk_wbuf[0] = 0;
			pAgent_Inf->slbrk_wbuf[0] =
				pCS_Inf->com_slbrk.pc_com_brk_level |
				pCS_Inf->com_slbrk.pc_com_autosel |
				pCS_Inf->com_slbrk.pc_com_reset |
				pCS_Inf->com_slbrk.pc_com_hw_brk;
		}
	}
	
	//### GE Camera
	if (g_sway_sensor_enable) {
		//シャッターコントロル
		if (inf.total_act % 10) {//時間遅れがあるので間隔をあけて設定変更する
			float64_t set_exps_time_us;

			if ((gp_app_imgprc->exps_mode == EXPOSURE_CONTROL_AUTO) || 
				(gp_app_imgprc->exps_mode == EXPOSURE_CONTROL_INIT) ||
				(gp_app_imgprc->exps_mode == EXPOSURE_CONTROL_MANUAL)
				) {
				if ((pCamera->stat.apistat = Teli::SetCamExposureTime(pCamera->stat.camhndl, gp_app_imgprc->exps_time)) != Teli::CAM_API_STS_SUCCESS) {
					wos.str(L""); wos << L" Fail: SetExpsureTime  Code:" << pCamera->stat.apistat;
				}
			}
		}
	}

	return STAT_OK;
}
int CAuxAgent::close() {
	return 0;
}

// GEカメラ
void CAuxAgent::SaveParameters_GECamera() {
	return; 
}
void CAuxAgent::LoadParameters_GECamera() {
	return; 
}
/// <summary>
/// カメラ起動処理
/// </summary>
/// <scenario>
/// 1. もし既に動いていたり、古い残骸があれば片付ける camera_capture_stop()
/// 2. g_keepRunning = true
/// 3. GECameraStart()を実行
/// </scenario>
void CAuxAgent::camera_capture_start() {
	// 1. もし既に動いていたり、古い残骸があれば片付ける
	camera_capture_stop();
	// 2. フラグを立て直す
	g_keepRunning = true;
	// 3. GE Camera
	GECameraStart();
	return; 
}
/// <summary>
/// GECameraStop()を実行してSys_Terminate()でシステムを閉じる
/// </summary>
void CAuxAgent::camera_capture_stop() {
	if (g_keepRunning == true) {
		GECameraStop();
		Sys_Terminate();
		g_keepRunning = false;
	}
	return; 
}

static wostringstream wosGE;
/// <summary>
/// GEカメラの初期化と起動
/// </summary>
/// <scenario>
/// 1. TeliCAM Apiの初期化
/// 2. カメラの数を取得(Sys_GetNumOfCamerasでApi内にカメラのリスト作成)
/// 3. カメラ情報の取得と表示
/// 　　Cam_GetInformationでカメラ情報をカメラオブジェクトのメンバ変数に取り込み表示
/// 　　メンバ変数への取り込みは最後のカメラのみ
/// 4. カメラオープン
/// 5. カメラのパラメータ設定書き込み(update_camera_parameter_base())
///		ピクセルフォーマット、ROI、フレームレート、、トリガーモード、黒レベル、ガンマ補正、ホワイトバランス、露光時間、ゲイン
/// 
/// 　　　※初期書き込み設定値は、CAuxEnvの初期化(init_camera_parameters())で設定済み
/// 
/// 6.  画像取得用のストリームインターフェースのオープン
/// 7.  カメラストリーム開始
/// /// 　　
/// </scenario>
/// <returns></returns>
HRESULT CAuxAgent::GECameraStart() {
	int32_t ret;
	U3V_CAM_INFO* psU3vCamInfo;
	GEV_CAM_INFO* psGevCamInfo;
	CAM_API_STATUS  uiStatus = CAM_API_STS_SUCCESS;

	// Initialize system.
	{
		uiStatus = Teli::Sys_Initialize();
		wosGE.str(L"");
		if (uiStatus != CAM_API_STS_SUCCESS) {
			if ((uiStatus & 0xF0000000) == 0x10000000) {// Warning
				wosGE << L"Warning:Sys_Initialize() >> Code :" << uiStatus;
			}
			else {
				wosGE << L"Failed:Sys_Initialize() >> Code :" << uiStatus;
			}
			pAgentObj->msg2listview(wosGE.str());
			return S_FALSE;
		}
		else {
			wosGE << L"TeliCamApi Initialize Success ";
			pAgentObj->msg2listview(wosGE.str());
		}
	}

	// Get number of cameras.
	{
		uiStatus = Teli::Sys_GetNumOfCameras(&(pCamera->camcount));
		wosGE.str(L"");
		if (uiStatus != CAM_API_STS_SUCCESS) {
			wosGE << L"Failed:GetNumOfCameras code:" << uiStatus;
			pAgentObj->msg2listview(wosGE.str());
			// Terminate system.
			Sys_Terminate();
			return S_FALSE;
		}
		else {
			wosGE << L"GetNumOfCameras = " << pCamera->camcount;
			pAgentObj->msg2listview(wosGE.str());
		}
	}
	// Get information of a camera.
	{
		wosGE.str(L"");
		for (uint32_t i = 0; i < pCamera->camcount; i++) {
			memset((void*)&(pCamera->m_caminfo), 0, sizeof(CAM_INFO));

			uiStatus = Cam_GetInformation((CAM_HANDLE)NULL, i, &(pCamera->m_caminfo));
			if (uiStatus != CAM_API_STS_SUCCESS) {
				// Terminate system.
				Sys_Terminate();
				return S_FALSE;
			}
			else {
				pCamera->stat.camidx = i;
			}

			wosGE << L"<Camera" << i << L" information>    ";
			if (pCamera->m_caminfo.eCamType == CAM_TYPE_U3V) {
				wosGE << L" Type : USB3 camera  CamIndex = " << pCamera->stat.camidx;
				pAgentObj->msg2listview(wosGE.str()); wosGE.str(L"");
				wosGE << L" Manufacturer :" << pCamera->m_caminfo.szManufacturer << L" Model name :" << pCamera->m_caminfo.szModelName << L" Serial number : " << pCamera->m_caminfo.szSerialNumber << L" User defined name :" << pCamera->m_caminfo.szUserDefinedName;
				pAgentObj->msg2listview(wosGE.str()); wosGE.str(L"");
				psU3vCamInfo = &pCamera->m_caminfo.sU3vCamInfo;
				wosGE << L" Adapter default MaxPacketSize :" << psU3vCamInfo->uiAdapterDfltMaxPacketSize; pAgentObj->msg2listview(wosGE.str()); wosGE.str(L"");
				pAgentObj->msg2listview(wosGE.str());
			}
			else if (pCamera->m_caminfo.eCamType == CAM_TYPE_GEV) {
				wosGE << L" Type : GigE camera  CamIndex = " << pCamera->stat.camidx;
				pAgentObj->msg2listview(wosGE.str()); wosGE.str(L"");
				wosGE << L" Manufacturer :" << pCamera->m_caminfo.szManufacturer << L" Model name :" << pCamera->m_caminfo.szModelName << L" Serial number : " << pCamera->m_caminfo.szSerialNumber << L" User defined name :" << pCamera->m_caminfo.szUserDefinedName;
				pAgentObj->msg2listview(wosGE.str()); wosGE.str(L"");

				psGevCamInfo = &pCamera->m_caminfo.sGevCamInfo;
				wosGE << L" Gev display name :" << psGevCamInfo->szDisplayName;
				pAgentObj->msg2listview(wosGE.str()); wosGE.str(L"");
				wosGE << L" Gev IP:" << psGevCamInfo->aucIPAddress[0] << L"." << psGevCamInfo->aucIPAddress[1] << L"." << psGevCamInfo->aucIPAddress[2] << L"." << psGevCamInfo->aucIPAddress[3]; pAgentObj->msg2listview(wosGE.str()); wosGE.str(L"");
				wosGE << L" adapter IP:" << psGevCamInfo->aucAdapterIPAddress[0] << L"." << psGevCamInfo->aucAdapterIPAddress[1] << L"." << psGevCamInfo->aucAdapterIPAddress[2] << L"." << psGevCamInfo->aucAdapterIPAddress[3];
				pAgentObj->msg2listview(wosGE.str()); wosGE.str(L"");
			}
			else {
				wosGE << L" Type : Unknown Camera";
				pAgentObj->msg2listview(wosGE.str());
				// Terminate system.
				Sys_Terminate();
				return S_FALSE;
			}
		}
	}
	//open camera
	{
		wosGE.str(L"");
		ret = pCamera->open_camera(Teli::CAM_ACCESS_MODE::CAM_ACCESS_MODE_CONTROL);
		if (ret != CAM_API_STS_SUCCESS) {
			wosGE << L" Fail: open_camera()  Code:" << ret;
			pAgentObj->msg2listview(wosGE.str()); wosGE.str(L"");
			return S_FALSE;
		}
		else {
			wosGE << L" open_camera SUCCESS Width:" << pCamera->stat.camwidth << L" Height:" << pCamera->stat.camheight;
			pAgentObj->msg2listview(wosGE.str()); wosGE.str(L"");
		}
	}

	//カメラの基本設定セット
	if (update_camera_parameter_base()) {
		Sys_Terminate(); return S_FALSE;
	}

	// 画像取得用のストリームインターフェースのオープン
	ret = pCamera->open_stream();
	if (ret != 0) {
		wosGE << L" Fail: open_stream  Code:" << ret;
		pAgentObj->msg2listview(wosGE.str()); wosGE.str(L"");
		return S_FALSE;
	}
	else {
		wosGE << L" open_stream SUCCESS";
		pAgentObj->msg2listview(wosGE.str()); wosGE.str(L"");
	}

	// 画像ストリームの転送開始
	ret = pCamera->start_stream();
	if (ret != 0) {
		wosGE << L" Fail: start_stream  Code:" << ret;
		pAgentObj->msg2listview(wosGE.str()); wosGE.str(L"");
		return S_FALSE;
	}
	else {
		wosGE << L"Camera stream started";
		pAgentObj->msg2listview(wosGE.str()); wosGE.str(L"");
	}
	return S_OK;
}

/// <summary>
/// GEカメラの停止
/// </summary>
/// <scenario>
/// 1.ストリームのハンドルクリア
/// 2.カメラクローズ
/// </scenario>
/// <returns></returns>
HRESULT CAuxAgent::GECameraStop() {
	// Close the stream interface.
	if (pCamera->stat.strmhndl != (CAM_STRM_HANDLE)NULL)
	{
		Teli::Strm_Close(pCamera->stat.strmhndl);
		pCamera->stat.strmhndl = (CAM_STRM_HANDLE)NULL;
	}
	// Close the camera.
	if (pCamera->stat.camhndl != (CAM_HANDLE)NULL)
	{
		Teli::Cam_Close(pCamera->stat.camhndl);
		pCamera->stat.camhndl = (CAM_HANDLE)NULL;
	}
		
	return S_OK;
}

/// <summary>
/// カメラのパラメータを設定する（書き込む）
/// </summary>
/// <scenario>
/// 1. ピクセルフォーマットの設定
/// 2. ROIの設定
/// 3. フレームレートの設定
/// 4. トリガーモード(false)の設定
/// 5. 黒レベルの設定
/// 6. ガンマ補正の設定
/// 7. ホワイトバランスの設定
/// 8. 輝度コントロールの設定
/// 　8-1. 露光時間制御モードの設定
/// 　8-2. 露光時間の設定(エラーチェックのみ）
///   8-3. カメラのAGC(Automatic gain control)動作モード(AUTO OFF)の設定
///	  8-4. ゲインの設定
/// </scenario>	
/// <returns></returns>
int CAuxAgent::update_camera_parameter_base() {
	int ret = 0;
	// カメラのビデオストリームのピクセル形式の設定(BayerBG8) 
	{
		wosGE.str(L"");
		if (pCamera->set_pixelformat(Teli::_CAM_PIXEL_FORMAT::PXL_FMT_BayerBG8) != 0) {
			pAgentObj->msg2listview(wosGE.str()); wosGE.str(L"");
			wosGE << L" Fail: set_pixelformat";
			ret = 1;
		}
		else wosGE << L"pixel:BG8 >> ";
	}
	// カメラのROI(領域)の設定
	{
		if (pCamera->set_camroi(gp_cnfg_camera->basis.roi[(int)ENUM_AXIS::X].offset,
			gp_cnfg_camera->basis.roi[(int)ENUM_AXIS::Y].offset,
			gp_cnfg_camera->basis.roi[(int)ENUM_AXIS::X].size,
			gp_cnfg_camera->basis.roi[(int)ENUM_AXIS::Y].size) != 0) {

			wosGE << L" Fail: set_ROI";
			pAgentObj->msg2listview(wosGE.str()); wosGE.str(L"");
			ret = 2;
		}
		else {
			if (!ret) {
				wosGE << L"ROI:X " << gp_cnfg_camera->basis.roi[(int)ENUM_AXIS::X].offset << L" Y " << gp_cnfg_camera->basis.roi[(int)ENUM_AXIS::Y].offset
					<< L" W " << gp_cnfg_camera->basis.roi[(int)ENUM_AXIS::X].size << L" H " << gp_cnfg_camera->basis.roi[(int)ENUM_AXIS::Y].size ;
			}
		}
	}
	// カメラのフレームレートの設定
	{
		if (pCamera->set_framerate(static_cast<float64_t>(gp_cnfg_camera->basis.framerate),
			Teli::CAM_ACQ_FRAME_RATE_CTRL_TYPE::CAM_ACQ_FRAME_RATE_CTRL_MANUAL) != 0) {
			pAgentObj->msg2listview(wosGE.str()); wosGE.str(L"");
			wosGE << L" Fail: set_framerate";
			ret = 3;
		}
		else {
			if (!ret) wosGE << L">>framerate: " << gp_cnfg_camera->basis.framerate;
		}
	}
	// カメラのトリガー動作モードの設定
	{
		if (pCamera->set_triggermode(false) != 0) {
			pAgentObj->msg2listview(wosGE.str()); wosGE.str(L"");
			wosGE << L" Fail: set_trig mode";
			ret = 4;
		}
		else {
			if (!ret) wosGE << L">>trig:false ";
		}
	}
	
	pAgentObj->msg2listview(wosGE.str()); wosGE.str(L"");

	// カメラの黒レベルの設定
	{
		if (pCamera->set_blacklevel(static_cast<float64_t>(gp_cnfg_camera->basis.blacklevel)) != 0) {
			pAgentObj->msg2listview(wosGE.str()); wosGE.str(L"");
			wosGE << L" Fail: black level";
			ret = 5;
		}
		else {
			if (!ret) wosGE << L"bk level: " << gp_cnfg_camera->basis.blacklevel;
		}
	}
	// カメラのガンマ補正値の設定
	{
		if (pCamera->set_gamma(static_cast<float64_t>(gp_cnfg_camera->basis.gamma)) != 0) {
			pAgentObj->msg2listview(wosGE.str()); wosGE.str(L"");
			wosGE << L" Fail: gamma";
			ret = 6;
		}
		else {
			if (!ret) wosGE << L">>gamma: " << gp_cnfg_camera->basis.gamma;
		}
	}
	// カメラのホワイトバランスゲイン自動調整モードの設定
	{
		if ((static_cast<Teli::CAM_BALANCE_WHITE_AUTO_TYPE>(gp_cnfg_camera->basis.wb.wb_auto) == Teli::CAM_BALANCE_WHITE_AUTO_TYPE::CAM_BALANCE_WHITE_AUTO_CONTINUOUS) ||
			(static_cast<Teli::CAM_BALANCE_WHITE_AUTO_TYPE>(gp_cnfg_camera->basis.wb.wb_auto) == Teli::CAM_BALANCE_WHITE_AUTO_TYPE::CAM_BALANCE_WHITE_AUTO_ONCE)) {
			if (pCamera->set_wbalance_auto(static_cast<Teli::CAM_BALANCE_WHITE_AUTO_TYPE>(gp_cnfg_camera->basis.wb.wb_auto)) != 0) {
				pAgentObj->msg2listview(wosGE.str()); wosGE.str(L"");
				wosGE << L" Fail: wb auto";
				ret = 7;
			}
		}
		else {
			if (pCamera->set_wbalance_auto(Teli::CAM_BALANCE_WHITE_AUTO_TYPE::CAM_BALANCE_WHITE_AUTO_OFF) != 0) {
				pAgentObj->msg2listview(wosGE.str()); wosGE.str(L"");
				wosGE << L" Fail: wb auto off";
				ret = 15;
			}
			else {
				//----------------------------------------------------------------------------
				// カメラのホワイトバランスゲイン(倍率)の設定
				if (pCamera->set_wbalance_ratio(static_cast<float64_t>(gp_cnfg_camera->basis.wb.wb_ratio_red),
					Teli::CAM_BALANCE_RATIO_SELECTOR_TYPE::CAM_BALANCE_RATIO_SELECTOR_RED) != 0) {
					pAgentObj->msg2listview(wosGE.str()); wosGE.str(L"");
					wosGE << L" Fail: wb set red";
					ret = 16;
				}
				else if (pCamera->set_wbalance_ratio(static_cast<float64_t>(gp_cnfg_camera->basis.wb.wb_ratio_blue),
					Teli::CAM_BALANCE_RATIO_SELECTOR_TYPE::CAM_BALANCE_RATIO_SELECTOR_BLUE) != 0) {
					pAgentObj->msg2listview(wosGE.str()); wosGE.str(L"");
					wosGE << L" Fail: wb set blue";
					ret = 17;
				}
				else {
					if (!ret)wosGE << L">>wb R: " << gp_cnfg_camera->basis.wb.wb_ratio_red << L" wb B:" << gp_cnfg_camera->basis.wb.wb_ratio_blue;
				}
			}
		}
	}
	// 輝度コントロール設定(露光時間)
	// カメラの露光時間の制御モードの設定
	{
		if (pCamera->set_expstime_control(Teli::CAM_EXPOSURE_TIME_CONTROL_TYPE::CAM_EXPOSURE_TIME_CONTROL_MANUAL) != 0) {
			pAgentObj->msg2listview(wosGE.str()); wosGE.str(L"");
			wosGE << L" Fail: expstime ctrl";
			ret = 18;
		}
	}
	// カメラの露光時間の設定(APIへの設定はスレッドで実行される)
	{
		if (pCamera->set_expstime(static_cast<float64_t>(gp_cnfg_camera->expstime.val)) != 0) {
			wosGE << L" Fail: expstime set";
			pAgentObj->msg2listview(wosGE.str());
			ret = 8;
		}
		else {
			if (!ret)wosGE << L"  expstime ctrl" << gp_cnfg_camera->expstime.val;
		}
	}
	// 輝度コントロール設定(ゲイン)
	// カメラのAGC(Automatic gain control)動作モードの設定
	{
		if (pCamera->set_gain_auto(Teli::CAM_GAIN_AUTO_TYPE::CAM_GAIN_AUTO_OFF) != 0) {
			pAgentObj->msg2listview(wosGE.str()); wosGE.str(L"");
			wosGE << L" Fail: gain auto";
			ret = 9;
		}
		else {
			if (!ret) wosGE << L"  gain auto off";
		}
	}
	// PacketDelayの設定
	{
		if (pCamera->set_packet_delay(static_cast<float64_t>(gp_cnfg_camera->basis.packet_delay)) != 0) {
			pAgentObj->msg2listview(wosGE.str()); wosGE.str(L"");
			wosGE << L" Fail: Packet delay set";
			ret = 11;
		}
		else {
			if (!ret) wosGE << L"  packet delay set" << gp_cnfg_camera->basis.packet_delay;
		}
		if (ret) wosGE << L"!! Fail Parameter Update Code:" << ret;
	}

	// カメラのゲインの設定(APIへの設定はスレッドで実行される)
	{
		if (pCamera->set_gain(static_cast<float64_t>(gp_cnfg_camera->gain.val)) != 0) {
			pAgentObj->msg2listview(wosGE.str()); wosGE.str(L"");
			wosGE << L" Fail: gain set";
			ret = 10;
		}
		else {
			if (!ret) wosGE << L"  gain set" << gp_cnfg_camera->gain.val;
		}
		if (ret) wosGE << L"!! Fail Parameter Update Code:" << ret;
		else {
			pAgentObj->msg2listview(wosGE.str()); wosGE.str(L"");
			wosGE << L"!! All Parameters Updated Normally";
		}
	}

	pAgentObj->msg2listview(wosGE.str()); wosGE.str(L"");

	return ret;
}
void CAuxAgent::OnPaintMon1(HWND hWnd, HDC hdc) {
	return; 
}

static wostringstream monwos;
LRESULT CALLBACK CAuxAgent::Mon1Proc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
	switch (msg)
	{
	case WM_CREATE: {
		InitCommonControls();//コモンコントロール初期化
		HINSTANCE hInst = (HINSTANCE)GetModuleHandle(0);
		//ウィンドウにコントロール追加
		st_mon1.hctrl[AUXAG_ID_MON1_STATIC_INF] = CreateWindowW(TEXT("STATIC"), st_mon1.text[AUXAG_ID_MON1_STATIC_INF], WS_CHILD | WS_VISIBLE | SS_LEFT,
			st_mon1.pt[AUXAG_ID_MON1_STATIC_INF].x, st_mon1.pt[AUXAG_ID_MON1_STATIC_INF].y, 
			st_mon1.sz[AUXAG_ID_MON1_STATIC_INF].cx, st_mon1.sz[AUXAG_ID_MON1_STATIC_INF].cy, 
			hWnd, (HMENU)(AUXAG_ID_MON1_CTRL_BASE + AUXAG_ID_MON1_STATIC_INF), hInst, NULL);

		//表示更新用タイマー
		SetTimer(hWnd, AUXAG_ID_MON1_TIMER, st_mon1.timer_ms, NULL);

		break;
	}
	case WM_COMMAND: {
		int wmId = LOWORD(wp);
		// 選択されたメニューの解析:
		switch (wmId)
		{
		case 1:break;
		default:
			return DefWindowProc(hWnd, msg, wp, lp);
		}
	}break;
	case WM_TIMER: {
		monwos.str(L""); monwos << L"GE Frame Index:" << pCamera->stat.frameidx << L"   GE FRAME LOST:" << pCamera->stat.frame_loss_count;
		SetWindowText(st_mon1.hwnd_mon, monwos.str().c_str());

		monwos.str(L"");
		SetWindowText(st_mon1.hctrl[AUXAG_ID_MON1_STATIC_INF], monwos.str().c_str());
	}break;

	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
	}break;
	case WM_DESTROY: {
		st_mon1.hwnd_mon= NULL;	
		st_mon1.is_monitor_active = false;
		KillTimer(hWnd, AUXAG_ID_MON1_TIMER);
	}break;
	default:
		return DefWindowProc(hWnd, msg, wp, lp);
	}
	return S_OK;
};

static bool is_write_req_turn = false;//書き込み要求送信の順番でtrue
LRESULT CALLBACK CAuxAgent::Mon2Proc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
	switch (msg)
	{
	case WM_CREATE: {
		InitCommonControls();//コモンコントロール初期化
		HINSTANCE hInst = (HINSTANCE)GetModuleHandle(0);
		//ウィンドウにコントロール追加
		//STATIC,LABEL
		for (int i = AUXAG_ID_MON2_STATIC_MSG; i <= AUXAG_ID_MON2_STATIC_MAIN_INF; i++) {
			st_mon2.hctrl[i] = CreateWindowW(TEXT("STATIC"), st_mon2.text[i], WS_CHILD | WS_VISIBLE | SS_LEFT,
				st_mon2.pt[i].x, st_mon2.pt[i].y, st_mon2.sz[i].cx, st_mon2.sz[i].cy,
				hWnd, (HMENU)(AUXAG_ID_MON2_CTRL_BASE + i), hInst, NULL);
		}
		//CB
		for (int i = AUXAG_ID_MON2_CB_COM_LEVEL_BIT0; i <= AUXAG_ID_MON2_CB_COM_LEVEL_FULL; i++) {
			st_mon2.hctrl[i] = CreateWindowW(TEXT("BUTTON"), st_mon2.text[i], WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
				st_mon2.pt[i].x, st_mon2.pt[i].y, st_mon2.sz[i].cx, st_mon2.sz[i].cy,
				hWnd, (HMENU)(AUXAG_ID_MON2_CTRL_BASE + i), hInst, NULL);
		}

		//タイマー起動
		UINT rtn = SetTimer(hWnd, AUXAG_ID_MON2_TIMER, AUXAG_PRM_MON2_TIMER_MS, NULL);
		break;

	}
	case WM_TIMER: {
		if (pMCSock == NULL)break;
		if (is_write_req_turn) {//書き込み要求送信
			st_mon2.wo_req_w.str(L"");
			//3Eフォーマット Dデバイス書き込み要求送信
			if (pMCSock->send_write_req_D_3E(pAgent_Inf->slbrk_wbuf) != S_OK) {
				st_mon2.wo_req_w << L"ERROR : send_read_req_D_3E()\n";
			}
			else snd_count_plc_w++;

			//電文内容表示出力
			if ((st_mon2.msg_disp_mode != AUXAG_MON2_MSG_DISP_OFF) && st_mon2.is_monitor_active) {
				//ヘッダ部分
				st_mon2.wo_req_w << L"Sw>>"
					<< L"#sub:" << std::hex << pMCSock->mc_req_msg_w.subcode
					<< L"#serial:" << pMCSock->mc_req_msg_w.serial
					<< L"#NW:" << pMCSock->mc_req_msg_w.nNW
					<< L"#PC:" << pMCSock->mc_req_msg_w.nPC
					<< L"#UIO:" << pMCSock->mc_req_msg_w.nUIO
					<< L"#Ucd:" << pMCSock->mc_req_msg_w.nUcode
					<< L"#len:" << pMCSock->mc_req_msg_w.len
					<< L"#tm:" << pMCSock->mc_req_msg_w.timer
					<< L"#com:" << pMCSock->mc_req_msg_w.com
					<< L"#scom:" << pMCSock->mc_req_msg_w.scom << L"\n"
					<< L"#d_no:" << pMCSock->mc_req_msg_w.d_no
					<< L"#d_code:" << pMCSock->mc_req_msg_w.d_code
					<< L"#n_dev:" << pMCSock->mc_req_msg_w.n_device << L"\n";
	
					st_mon2.wo_req_w << L"D" << dec << L" |";
					if (st_mon2.msg_disp_mode == AUXAG_MON2_MSG_DISP_HEX)	st_mon2.wo_req_w << hex;
					st_mon2.wo_req_w << std::setw(4) << std::setfill(L'0') << pAgent_Inf->slbrk_wbuf[0] << L"|";
					st_mon2.wo_req_w << L"\n";

				SetWindowText(st_mon2.hctrl[AUXAG_ID_MON2_STATIC_REQ_W], st_mon2.wo_req_w.str().c_str());
			}

			QueryPerformanceCounter(&start_count_w);  // 書き込み要求送信時カウント値取り込み
			is_write_req_turn = false;
		}
		else {
			st_mon2.wo_req_r.str(L"");
			//読み出し要求送信
			if (pMCSock->send_read_req_D_3E() != S_OK) {
				st_mon2.wo_req_r << L"ERROR : send_read_req_D_3E()";
			}
			else snd_count_plc_r++;

			if ((st_mon2.msg_disp_mode != AUXAG_MON2_MSG_DISP_OFF) && st_mon2.is_monitor_active) {
				st_mon2.wo_req_r << L"Sr>>"
					<< L"#sub:" << std::hex << pMCSock->mc_req_msg_r.subcode
					<< L"#serial:" << pMCSock->mc_req_msg_r.serial
					<< L"#NW:" << pMCSock->mc_req_msg_r.nNW
					<< L"#PC:" << pMCSock->mc_req_msg_r.nPC
					<< L"#UIO:" << pMCSock->mc_req_msg_r.nUIO
					<< L"#Ucd:" << pMCSock->mc_req_msg_r.nUcode
					<< L"#len:" << pMCSock->mc_req_msg_r.len
					<< L"#tm:" << pMCSock->mc_req_msg_r.timer
					<< L"#com:" << pMCSock->mc_req_msg_r.com
					<< L"#scom:" << pMCSock->mc_req_msg_r.scom << L"\n"
					<< L"#d_no:" << pMCSock->mc_req_msg_r.d_no
					<< L"#d_code:" << pMCSock->mc_req_msg_r.d_code
					<< L"#n_dev:" << pMCSock->mc_req_msg_r.n_device;
				SetWindowText(st_mon2.hctrl[AUXAG_ID_MON2_STATIC_REQ_R], st_mon2.wo_req_r.str().c_str());
			}

			QueryPerformanceCounter(&start_count_r);  // 書き込み要求送信時カウント値取り込み
			is_write_req_turn = true;
		}
		//共通表示 (送受信カウント,遅延時間,IP情報）
		if (st_mon2.is_monitor_active) {
			monwos.str(L""); monwos << L"RCV:R " << rcv_count_plc_r
				<< L"  W " << rcv_count_plc_w
				<< L"    SND:R " << snd_count_plc_r
				<< L"  W " << snd_count_plc_w
				<< L"    ERR:R " << rcv_errcount_plc_r
				<< L"  W " << rcv_errcount_plc_w
				<< L"    遅延μs:R " << res_delay_max_r
				<< L"  W " << res_delay_max_w;

			SetWindowText(hWnd, monwos.str().c_str());

			monwos.str(L""); 
			if(st_mon2.msg_disp_mode == AUXAG_MON2_MSG_DISP_HEX)monwos << hex << std::setw(4) << std::setfill(L'0');

			monwos << L" BRK D96:" << pAgent_Inf->slbrk_wbuf[0] << L"  D16:" << pAgent_Inf->slbrk_rbuf[0] << L" 17:" << pAgent_Inf->slbrk_rbuf[1] << L" 18:" << pAgent_Inf->slbrk_rbuf[2] << L" 19:" << pAgent_Inf->slbrk_rbuf[3];
			SetWindowText(st_mon2.hctrl[AUXAG_ID_MON2_STATIC_MSG], monwos.str().c_str());

			SOCKADDR_IN	addr;
			if (pMCSock != NULL) {
				addr = pMCSock->get_addrin_rcv(); monwos.str(L"");
				monwos << L"UNI>>IP R:" << addr.sin_addr.S_un.S_un_b.s_b1 << L"." << addr.sin_addr.S_un.S_un_b.s_b2 << L"." << addr.sin_addr.S_un.S_un_b.s_b3 << L"." << addr.sin_addr.S_un.S_un_b.s_b4 << L":"
					<< htons(addr.sin_port) << L" ";
				addr = pMCSock->get_addrin_snd();
				monwos << L" S:" << addr.sin_addr.S_un.S_un_b.s_b1 << L"." << addr.sin_addr.S_un.S_un_b.s_b2 << L"." << addr.sin_addr.S_un.S_un_b.s_b3 << L"." << addr.sin_addr.S_un.S_un_b.s_b4 << L":"
					<< htons(addr.sin_port) << L" ";
				addr = pMCSock->get_addrin_from();
				monwos << L" F:" << addr.sin_addr.S_un.S_un_b.s_b1 << L"." << addr.sin_addr.S_un.S_un_b.s_b2 << L"." << addr.sin_addr.S_un.S_un_b.s_b3 << L"." << addr.sin_addr.S_un.S_un_b.s_b4 << L":"
					<< htons(addr.sin_port) << L" ";
				SetWindowText(st_mon2.hctrl[AUXAG_ID_MON2_STATIC_INF], monwos.str().c_str());
			}
			
			monwos.str(L"");
			monwos << L" MAIN CS >> LEVEL:" << pCS_Inf->com_slbrk.pc_com_brk_level << L"  HW:" << pCS_Inf->com_slbrk.pc_com_hw_brk << L" RST:" << pCS_Inf ->com_slbrk.pc_com_reset << L" EMG:" << pCS_Inf->com_slbrk.pc_com_emg << L" AUTO:" << pCS_Inf->com_slbrk.pc_com_autosel << L" \n";

			monwos <<L"SLBR FB >> LV:"<< pCS_Inf->fb_slbrk.brk_fb_level	<<L" HW:"<< pCS_Inf->fb_slbrk.brk_fb_hw_brk<<L" EMG:"<< pCS_Inf->fb_slbrk.brk_fb_emg<<L" AUTO:"<< pCS_Inf->fb_slbrk.brk_fb_autosel<<L" ERR MAP:"<< pCS_Inf->fb_slbrk.brk_fb_err_map << L" ERR CODE:" << pCS_Inf->fb_slbrk.brk_fb_err_code << L" ERR HTHY:" << pCS_Inf->fb_slbrk.healthy_err << L" \n";
			monwos <<L" POS:"<< pCS_Inf->fb_slbrk.brk_fb_rbsl_pos<<L" 空振:"<< pCS_Inf->fb_slbrk.brk_fb_karaburi<<L" ORG PT:"<< pCS_Inf->fb_slbrk.brk_fb_org_pt<<L" TMOV:"<< pCS_Inf->fb_slbrk.brk_fb_time_over<<L" RELEASE:"<< pCS_Inf->fb_slbrk.brk_fb_release<<L" SYS ERR:"<< pCS_Inf->fb_slbrk.brk_fb_sys_err;
			SetWindowText(st_mon2.hctrl[AUXAG_ID_MON2_STATIC_MAIN_INF], monwos.str().c_str());
		}
	}break;
	case WM_COMMAND: {
		int wmId = LOWORD(wp);
		// 選択されたメニューの解析:
		switch (wmId - AUXAG_ID_MON2_CTRL_BASE)
		{
		case AUXAG_ID_MON2_CB_COM_LEVEL_BIT0: {
			if (BST_UNCHECKED == SendMessage(st_mon2.hctrl[AUXAG_ID_MON2_CB_COM_LEVEL_BIT0], BM_GETCHECK, 0, 0)) {
				pAgent_Inf->slbrk_wbuf[0] &= 0xFFFE;
			}
			else {
				pAgent_Inf->slbrk_wbuf[0] |= 0x0001;
			}
		}break;
		case AUXAG_ID_MON2_CB_COM_LEVEL_BIT1: {
			if (BST_UNCHECKED == SendMessage(st_mon2.hctrl[AUXAG_ID_MON2_CB_COM_LEVEL_BIT1], BM_GETCHECK, 0, 0)) {
				pAgent_Inf->slbrk_wbuf[0] &= 0xFFFD;
			}
			else {
				pAgent_Inf->slbrk_wbuf[0] |= 0x0002;
			}
		}break;
		case AUXAG_ID_MON2_CB_COM_LEVEL_BIT2: {
			if (BST_UNCHECKED == SendMessage(st_mon2.hctrl[AUXAG_ID_MON2_CB_COM_LEVEL_BIT2], BM_GETCHECK, 0, 0)) {
				pAgent_Inf->slbrk_wbuf[0] &= 0xFFFB;
			}
			else {
				pAgent_Inf->slbrk_wbuf[0] |= 0x0004;
			}
		}break;
		case AUXAG_ID_MON2_CB_COM_LEVEL_BIT3: {
			if (BST_UNCHECKED == SendMessage(st_mon2.hctrl[AUXAG_ID_MON2_CB_COM_LEVEL_BIT3], BM_GETCHECK, 0, 0)) {
				pAgent_Inf->slbrk_wbuf[0] &= 0xFFF7;
			}
			else {
				pAgent_Inf->slbrk_wbuf[0] |= 0x0008;
			}
		}break;
		case AUXAG_ID_MON2_CB_COM_LEVEL_FULL: {
			if (BST_UNCHECKED == SendMessage(st_mon2.hctrl[AUXAG_ID_MON2_CB_COM_LEVEL_FULL], BM_GETCHECK, 0, 0)) {
				pAgent_Inf->slbrk_wbuf[0] &= 0xFFF0;
				SendMessage(st_mon2.hctrl[AUXAG_ID_MON2_CB_COM_LEVEL_BIT0], BM_SETCHECK, BST_UNCHECKED, 0);
				SendMessage(st_mon2.hctrl[AUXAG_ID_MON2_CB_COM_LEVEL_BIT1], BM_SETCHECK, BST_UNCHECKED, 0);
				SendMessage(st_mon2.hctrl[AUXAG_ID_MON2_CB_COM_LEVEL_BIT2], BM_SETCHECK, BST_UNCHECKED, 0);
				SendMessage(st_mon2.hctrl[AUXAG_ID_MON2_CB_COM_LEVEL_BIT3], BM_SETCHECK, BST_UNCHECKED, 0);
			}
			else {
				pAgent_Inf->slbrk_wbuf[0] |= 0x000F;
				SendMessage(st_mon2.hctrl[AUXAG_ID_MON2_CB_COM_LEVEL_BIT0], BM_SETCHECK, BST_CHECKED, 0);
				SendMessage(st_mon2.hctrl[AUXAG_ID_MON2_CB_COM_LEVEL_BIT1], BM_SETCHECK, BST_CHECKED, 0);
				SendMessage(st_mon2.hctrl[AUXAG_ID_MON2_CB_COM_LEVEL_BIT2], BM_SETCHECK, BST_CHECKED, 0);
				SendMessage(st_mon2.hctrl[AUXAG_ID_MON2_CB_COM_LEVEL_BIT3], BM_SETCHECK, BST_CHECKED, 0);
			}

		}break;
		case AUXAG_ID_MON2_CB_COM_HW_BRK: {
			if (BST_UNCHECKED == SendMessage(st_mon2.hctrl[AUXAG_ID_MON2_CB_COM_HW_BRK], BM_GETCHECK, 0, 0)) {
				pAgent_Inf->slbrk_wbuf[0] &= 0xFFEF;
			}
			else {
				pAgent_Inf->slbrk_wbuf[0] |= 0x0010;
			}
		}break;
		case AUXAG_ID_MON2_CB_COM_RST: {
			if (BST_UNCHECKED == SendMessage(st_mon2.hctrl[AUXAG_ID_MON2_CB_COM_RST], BM_GETCHECK, 0, 0)) {
				pAgent_Inf->slbrk_wbuf[0] &= 0xFFDF;
			}
			else {
				pAgent_Inf->slbrk_wbuf[0] |= 0x0020;
			}
		}break;
		case AUXAG_ID_MON2_CB_COM_EMG: {
			if (BST_UNCHECKED == SendMessage(st_mon2.hctrl[AUXAG_ID_MON2_CB_COM_EMG], BM_GETCHECK, 0, 0)) {
				pAgent_Inf->slbrk_wbuf[0] &= 0xFFBF;
			}
			else {
				pAgent_Inf->slbrk_wbuf[0] |= 0x0040;
			}
		}break;
		case AUXAG_ID_MON2_CB_COM_AUTOSEL: {
			if (BST_UNCHECKED == SendMessage(st_mon2.hctrl[AUXAG_ID_MON2_CB_COM_AUTOSEL], BM_GETCHECK, 0, 0)) {
				pAgent_Inf->slbrk_wbuf[0] &= 0xFF7F;
			}
			else {
				pAgent_Inf->slbrk_wbuf[0] |= 0x0080;
			}
		}break;
		case AUXAG_ID_MON2_CB_MODE_SLBRK_DBG: {
			if (BST_UNCHECKED == SendMessage(st_mon2.hctrl[AUXAG_ID_MON2_CB_MODE_SLBRK_DBG], BM_GETCHECK, 0, 0)) {
				st_mon2.slbrk_dbg_mode = L_OFF;
			}
			else {
				st_mon2.slbrk_dbg_mode = L_ON;
			}
		}break;
										 
		default:
			return DefWindowProc(hWnd, msg, wp, lp);
		}
	}break;
	case ID_SOCK_MC_AUX_BRK://MCソケット受信イベント
	{
		if (pMCSock == NULL)break;
		int nEvent = WSAGETSELECTEVENT(lp);
		st_mon2.wo_res_r.str(L"");
		st_mon2.wo_res_w.str(L"");
		switch (nEvent) {
		case FD_READ: {
			UINT nRtn = pMCSock->rcv_msg_3E(pAgent_Inf->slbrk_rbuf);//読み出し応答の時はデータ部のみ指定バッファにコピー
			if (nRtn == MC_RES_READ) {//読み出し応答
				rcv_count_plc_r++;

				//モニタWindow表示中ならば、モニタ表示出力処理
				if ((st_mon2.msg_disp_mode != AUXAG_MON2_MSG_DISP_OFF) && st_mon2.is_monitor_active) {
					st_mon2.wo_res_r << L"Rr>>"
						<< L"#sub:" << std::hex << pMCSock->mc_res_msg_r.subcode
						<< L"#serial:" << pMCSock->mc_res_msg_r.serial
						<< L"#NW:" << pMCSock->mc_res_msg_r.nNW
						<< L"#PC:" << pMCSock->mc_res_msg_r.nPC
						<< L"#UIO:" << pMCSock->mc_res_msg_r.nUIO
						<< L"#Ucd:" << pMCSock->mc_res_msg_r.nUcode
						<< L"#len:" << pMCSock->mc_res_msg_r.len
						<< L"#end:" << pMCSock->mc_res_msg_r.endcode << L"\n";
					if (st_mon2.msg_disp_mode == AUXAG_MON2_MSG_DISP_HEX) st_mon2.wo_res_r << hex << std::setw(4)<< std::setfill(L'0') ;
					else st_mon2.wo_res_r <<dec << std::setw(6)<< std::setfill(L' ');
	
					st_mon2.wo_res_r << L"D" << L" |";
							st_mon2.wo_res_r << hex << std::setw(4) << pAgent_Inf->slbrk_rbuf[0] << L"|" << pAgent_Inf->slbrk_rbuf[1] << L"|" << pAgent_Inf->slbrk_rbuf[2] << L"|";
					st_mon2.wo_res_r << L"\n";

					SetWindowText(st_mon2.hctrl[AUXAG_ID_MON2_STATIC_RES_R], st_mon2.wo_res_r.str().c_str());
				}

				/**************** 読み込み応答時間計測(400回の最大値）*************************************/
				QueryPerformanceCounter(&end_count_r);    // 現在のカウント数
				LONGLONG lspan = (end_count_r.QuadPart - start_count_r.QuadPart) * 1000000L / frequency.QuadPart;// 時間の間隔[usec]
				if (res_delay_max_r < lspan) res_delay_max_r = lspan;
				if (rcv_count_plc_r % 400 == 0) res_delay_max_r = 0;
				/******************************************************************************************/
			}
			else if (nRtn == MC_RES_WRITE) {
				rcv_count_plc_w++;
				if ((st_mon2.msg_disp_mode != AUXAG_MON2_MSG_DISP_OFF) && st_mon2.is_monitor_active) {
					st_mon2.wo_res_w << L"Rw>>"
						<< L"#sub:" << std::hex << pMCSock->mc_res_msg_w.subcode
						<< L"#serial:" << pMCSock->mc_res_msg_w.serial
						<< L"#NW:" << pMCSock->mc_res_msg_w.nNW
						<< L"#PC:" << pMCSock->mc_res_msg_w.nPC
						<< L"#UIO:" << pMCSock->mc_res_msg_w.nUIO
						<< L"#Ucd:" << pMCSock->mc_res_msg_w.nUcode
						<< L"#len:" << pMCSock->mc_res_msg_w.len
						<< L"#end:" << pMCSock->mc_res_msg_w.endcode;
					SetWindowText(st_mon2.hctrl[AUXAG_ID_MON2_STATIC_RES_W], st_mon2.wo_res_w.str().c_str());
				}

				QueryPerformanceCounter(&end_count_w);    // 現在のカウント数
				LONGLONG lspan = (end_count_w.QuadPart - start_count_w.QuadPart) * 1000000L / frequency.QuadPart;// 時間の間隔[usec]
				if (res_delay_max_w < lspan) res_delay_max_w = lspan;
				if (rcv_count_plc_w % 400 == 0) {
					res_delay_max_w = 0;
				}
			}
			else {
				int err_code = WSAGetLastError();
				if (is_write_req_turn) {
					st_mon2.wo_res_r << L"PLC READ RES_ERR  CODE:err_code" << err_code;
					rcv_errcount_plc_r++;
					SetWindowText(st_mon2.hctrl[AUXAG_ID_MON2_STATIC_RES_R], st_mon2.wo_res_r.str().c_str());
				}
				else {
					st_mon2.wo_res_w << L"PLC WRITE RES_ERR  CODE:err_code" << err_code;
					rcv_errcount_plc_w++;
					SetWindowText(st_mon2.hctrl[AUXAG_ID_MON2_STATIC_RES_W], st_mon2.wo_res_w.str().c_str());
				}
			}
		}break;
		default: break;
		}
	}break;
	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
	}break;
	case WM_DESTROY: {
		st_mon2.hwnd_mon = NULL;
		st_mon2.is_monitor_active = false;
		KillTimer(hWnd, AUXAG_ID_MON2_TIMER);
	}break;
	default:
		return DefWindowProc(hWnd, msg, wp, lp);
	}
	return S_OK;
}

HWND CAuxAgent::open_monitor_wnd(HWND h_parent_wnd, int id) {
	
	InitCommonControls();//コモンコントロール初期化
	HINSTANCE hInst = GetModuleHandle(0);

	WNDCLASSEXW wcex;
	ATOM fb = RegisterClassExW(&wcex);

	if (id == BC_ID_MON1) {
		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = Mon1Proc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = hInst;
		wcex.hIcon = NULL;
		wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wcex.lpszMenuName = TEXT("AUXAG_MON1");
		wcex.lpszClassName = TEXT("AUXAG_MON1");
		wcex.hIconSm = NULL;

		ATOM fb = RegisterClassExW(&wcex);

		st_mon1.hwnd_mon = CreateWindowW(TEXT("AUXAG_MON1"), TEXT("AUXAG_MON1"), WS_OVERLAPPEDWINDOW,
			AUXAG_MON1_WND_X, AUXAG_MON1_WND_Y, AUXAG_MON1_WND_W, AUXAG_MON1_WND_H,
			h_parent_wnd, nullptr, hInst, nullptr);
	//	show_monitor_wnd(id);
		return st_mon1.hwnd_mon;
	}
	else if (id == BC_ID_MON2) {
		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = Mon2Proc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = hInst;
		wcex.hIcon = NULL;
		wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wcex.lpszMenuName = TEXT("AUXAG_MON2");
		wcex.lpszClassName = TEXT("AUXAG_MON2");
		wcex.hIconSm = NULL;

		ATOM fb = RegisterClassExW(&wcex);

		st_mon2.hwnd_mon = CreateWindowW(TEXT("AUXAG_MON2"), TEXT("SLBRK IF"), WS_OVERLAPPEDWINDOW,
			AUXAG_MON2_WND_X, AUXAG_MON2_WND_Y, AUXAG_MON2_WND_W, AUXAG_MON2_WND_H,
			h_parent_wnd, nullptr, hInst, nullptr);
		st_mon2.msg_disp_mode = AUXAG_MON2_MSG_DISP_OFF;
//		show_monitor_wnd(id);
		return st_mon2.hwnd_mon;
	}
	else
	{
		return NULL;
	};

	return NULL;
}
void CAuxAgent::close_monitor_wnd(int id) {
	if (id == BC_ID_MON1)
		DestroyWindow(st_mon1.hwnd_mon);
	else if (id == BC_ID_MON2)
		DestroyWindow(st_mon2.hwnd_mon);
	else;
	return;
}
void CAuxAgent::show_monitor_wnd(int id) { 
	if ((id == BC_ID_MON1) && (st_mon1.hwnd_mon != NULL)) {
		ShowWindow(st_mon1.hwnd_mon, SW_SHOW);
		UpdateWindow(st_mon1.hwnd_mon);
		st_mon1.is_monitor_active = true;
	}
	else if ((id == BC_ID_MON2) && (st_mon2.hwnd_mon != NULL)) {
		ShowWindow(st_mon2.hwnd_mon, SW_SHOW);
		UpdateWindow(st_mon2.hwnd_mon);
		st_mon2.is_monitor_active = true;
	}
	else;
	return;
}
void CAuxAgent::hide_monitor_wnd(int id) { 
	if (id == BC_ID_MON1)
		ShowWindow(st_mon1.hwnd_mon, SW_HIDE);
	else if (id == BC_ID_MON2)
		ShowWindow(st_mon2.hwnd_mon, SW_HIDE);
	else;	
	return;
}
											
/****************************************************************************/
/*   タスク設定タブパネルウィンドウのコールバック関数                       */
/****************************************************************************/
LRESULT CALLBACK CAuxAgent::PanelProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp) {

	switch (msg) {
	case WM_COMMAND: {
		switch (LOWORD(wp)) {
		case IDC_TASK_FUNC_RADIO1:
		case IDC_TASK_FUNC_RADIO2:
		case IDC_TASK_FUNC_RADIO3:
		case IDC_TASK_FUNC_RADIO4:
		case IDC_TASK_FUNC_RADIO5:
		case IDC_TASK_FUNC_RADIO6:
		{
			inf.panel_func_id = LOWORD(wp); set_panel_tip_txt();
			set_PNLparam_value(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
			for (int i = 0; i < BC_N_ACT_ITEM; i++) {
				if (inf.panel_act_chk[wp - IDC_TASK_FUNC_RADIO1][i])
					SendMessage(GetDlgItem(hDlg, IDC_TASK_ITEM_CHECK1 + i), BM_SETCHECK, BST_CHECKED, 0L);
				else
					SendMessage(GetDlgItem(hDlg, IDC_TASK_ITEM_CHECK1 + i), BM_SETCHECK, BST_UNCHECKED, 0L);
			}
		}break;

		case IDC_TASK_ITEM_CHECK1:
		case IDC_TASK_ITEM_CHECK2:
		case IDC_TASK_ITEM_CHECK3:
		case IDC_TASK_ITEM_CHECK4:
		case IDC_TASK_ITEM_CHECK5:
		case IDC_TASK_ITEM_CHECK6:
		{
			if (IsDlgButtonChecked(hDlg, LOWORD(wp)) == BST_CHECKED)
				inf.panel_act_chk[inf.panel_func_id - IDC_TASK_FUNC_RADIO1][LOWORD(wp) - IDC_TASK_ITEM_CHECK1] = true;
			else
				inf.panel_act_chk[inf.panel_func_id - IDC_TASK_FUNC_RADIO1][LOWORD(wp) - IDC_TASK_ITEM_CHECK1] = false;
		}break;

		case IDSET:
		{
			wstring wstr, wstr_tmp;

			wstr += L"Param 1(d):";
			int n = GetDlgItemText(hDlg, IDC_TASK_EDIT1, (LPTSTR)wstr_tmp.c_str(), 128);
			msg2listview(wstr);

		}break;
		case IDRESET:
		{
			set_PNLparam_value(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);

		}break;

		case IDC_TASK_MODE_RADIO0:
		{
			inf.mode_id = BC_ID_MODE0;
		}break;
		case IDC_TASK_MODE_RADIO1:
		{
			inf.mode_id = BC_ID_MODE1;
		}break;
		case IDC_TASK_MODE_RADIO2:
		{
			inf.mode_id = BC_ID_MODE2;
		}break;

		case IDC_TASK_MON_CHECK1:
		{
			if (IsDlgButtonChecked(hDlg, IDC_TASK_MON_CHECK1) == BST_CHECKED) {
			//	open_monitor_wnd(inf.hwnd_parent, BC_ID_MON1);
				show_monitor_wnd(BC_ID_MON1);
			}
			else {
			//	close_monitor_wnd(BC_ID_MON1);
				hide_monitor_wnd(BC_ID_MON1);
			}
		}break;

		case IDC_TASK_MON_CHECK2: {

			if (IsDlgButtonChecked(hDlg, IDC_TASK_MON_CHECK2) == BST_CHECKED) {
				show_monitor_wnd(BC_ID_MON2);
				st_mon2.msg_disp_mode = AUXAG_MON2_MSG_DISP_DEC;
			}
			else {
				hide_monitor_wnd(BC_ID_MON2);
				st_mon2.msg_disp_mode = AUXAG_MON2_MSG_DISP_OFF;
			}
		}break;
		}
	}break;

	case WM_USER_TASK_REQ: {
		if (HIWORD(wp) == WM_USER_WPH_OPEN_IF_WND) {
			wos.str(L"");
			if (lp == BC_ID_MON1) st_mon1.hwnd_mon = open_monitor_wnd(hDlg, (int)lp);
			if (lp == BC_ID_MON2) st_mon2.hwnd_mon = open_monitor_wnd(hDlg, (int)lp);
		}
		else if (wp == WM_USER_WPH_CLOSE_IF_WND) 	close_monitor_wnd(lp);
		else;
	}break;

	default:break;
	}
	return 0;
};

///###	タブパネルのListViewにメッセージを出力
void CAuxAgent::msg2listview(wstring wstr) {

	const wchar_t* pwc; pwc = wstr.c_str();

	inf.hwnd_msglist = GetDlgItem(inf.hwnd_opepane, IDC_LIST1);
	LVITEM item;

	item.mask = LVIF_TEXT;
	item.pszText = (wchar_t*)pwc;								// テキスト
	item.iItem = inf.panel_msglist_count % BC_LISTVIEW_ROW_MAX;	// 番号
	item.iSubItem = 1;											// サブアイテムの番号
	ListView_SetItem(inf.hwnd_msglist, &item);

	SYSTEMTIME st; TCHAR tbuf[32];
	::GetLocalTime(&st);
	wsprintf(tbuf, L"%02d:%02d:%02d.%01d", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds / 100);

	item.pszText = tbuf;   // テキスト
	item.iSubItem = 0;											// サブアイテムの番号
	ListView_SetItem(inf.hwnd_msglist, &item);

	//InvalidateRect(inf.hWnd_msgList, NULL, TRUE);
	inf.panel_msglist_count++;
	return;
}

void CAuxAgent::set_PNLparam_value(float p1, float p2, float p3, float p4, float p5, float p6) {
	wstring wstr;
	wstr += std::to_wstring(p1); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT1), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p2); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT2), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p3); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT3), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p4); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT4), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p5); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT5), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p6); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT6), wstr.c_str());
}

//タブパネルのStaticテキストを設定
void CAuxAgent::set_panel_tip_txt() {
	wstring wstr_type; wstring wstr;
	switch (inf.panel_func_id) {
	case IDC_TASK_FUNC_RADIO1:
	case IDC_TASK_FUNC_RADIO2:
	case IDC_TASK_FUNC_RADIO3:
	case IDC_TASK_FUNC_RADIO4:
	case IDC_TASK_FUNC_RADIO5:
	case IDC_TASK_FUNC_RADIO6:
	default:
	{
		wstr = L"1:-";
		SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_STATIC_ITEM3), wstr.c_str());
		wstr = L"2:-";
		SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_STATIC_ITEM4), wstr.c_str());
		wstr = L"3:-";
		SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_STATIC_ITEM5), wstr.c_str());
		wstr = L"4:-";
		SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_STATIC_ITEM6), wstr.c_str());
		wstr = L"5:-";
		SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_STATIC_ITEM7), wstr.c_str());
		wstr = L"6:-";
		SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_STATIC_ITEM8), wstr.c_str());
	}break;
	}
	return;
}
//タブパネルのFunctionボタンのStaticテキストを設定
void CAuxAgent::set_func_pb_txt() {
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO1, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO2, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO3, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO4, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO5, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO6, L"-");
	return;
}
//タブパネルのItem chkテキストを設定
void CAuxAgent::set_item_chk_txt() {
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK1, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK2, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK3, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK4, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK5, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK6, L"-");
	return;
}

