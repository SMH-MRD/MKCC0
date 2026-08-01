#include "CAuxEnv.h"
#include "resource.h"
#include "framework.h"
#include "AUXEQ_DEF.H"
#include "SmemAux.H"
#include "CComm.h"
#include "CCamera.h"
#include "SWYSENSOR_DEF.H"

extern ST_DEVICE_CODE g_my_code;

// Swayセンサ関連
extern CTeliCamLib* pCamera;//GEカメラオブジェクトへのグローバルポインタ

// ***アプリケーション設定アクセスポインタ
extern PCONFIG_COMMON    gp_cnfg_common;        // 共通設定
extern PCONFIG_CAMERA    gp_cnfg_camera;        // カメラ設定
extern PCONFIG_MOUNTING  gp_cnfg_mounting;      // 取付寸法設定
extern PCONFIG_IMGPROC   gp_cnfg_imgprc;		// 画像処理条件設定

// ***アプリケーション情報アクセスポインタ  
extern PINFO_CLIENT_DATA gp_app_client;        // クライアント情報
extern PINFO_ADJUST_DATA gp_app_adjust;        // 調整情報
extern PINFO_IMGPRC_DATA gp_app_imgprc;        // 画像処理情報
extern PINFO_SYSTEM_DATA gp_app_system;        // システム情報


ST_ENV_MON1 CAuxEnv::st_mon1;
ST_ENV_MON2 CAuxEnv::st_mon2;

//共有メモリ参照用定義
extern CSharedMem* pEnvInfObj;
extern CSharedMem* pAgentInfObj;
extern CSharedMem* pCsInfObj;
extern CSharedMem* pScadInfObj;

static LPST_AUX_ENV_INF		pEnvInf;
static LPST_AUX_CS_INF		pCsInf;
static LPST_AUX_AGENT_INF	pAgentInf;

/****************************************************************************/
/*   デフォルト関数											                    */
/****************************************************************************/
CAuxEnv::CAuxEnv() {
}

CAuxEnv::~CAuxEnv() {
}

HRESULT CAuxEnv::initialize(LPVOID lpParam) {
	HRESULT hr = S_OK;
	//### 出力用共有メモリ取得
	out_size = sizeof(ST_AUX_ENV_INF);
	if (OK_SHMEM != pEnvInfObj->create_smem(SMEM_AUX_ENV_INF_NAME, sizeof(ST_AUX_ENV_INF), MUTEX_AUX_ENV_INF_NAME)) {
		return(FALSE);
	}
	set_outbuf(pEnvInfObj->get_pMap());

	//### 入力用共有メモリ取得
	if (OK_SHMEM != pCsInfObj->create_smem(SMEM_AUX_CS_INF_NAME, sizeof(ST_AUX_CS_INF), MUTEX_AUX_CS_INF_NAME)) {
		return(FALSE);
	}
	if (OK_SHMEM != pAgentInfObj->create_smem(SMEM_AUX_AGENT_INF_NAME, sizeof(ST_AUX_AGENT_INF), MUTEX_AUX_AGENT_INF_NAME)) {
		return(FALSE);
	}
	if (OK_SHMEM != pScadInfObj->create_smem(SMEM_AUX_SCAD_INF_NAME, sizeof(ST_AUX_SCAD_INF), MUTEX_AUX_SCAD_INF_NAME)) {
		return(FALSE);
	}

	pEnvInf = (LPST_AUX_ENV_INF)(pEnvInfObj->get_pMap());
	pAgentInf = (LPST_AUX_AGENT_INF)(pAgentInfObj->get_pMap());
	pCsInf = (LPST_AUX_CS_INF)pCsInfObj->get_pMap();

	if ((pEnvInf == NULL) || (pAgentInf == NULL) || (pCsInf == NULL))
		hr = S_FALSE;

	if (hr == S_FALSE) {
		wos.str(L""); wos << L"Initialize : SMEM NG"; msg2listview(wos.str());
		return hr;
	};

	//### 有効機能の設定
	pEnvInf->device_code = g_my_code;
	//旋回ブレーキ
	int enable = (g_my_code.option >> 28) & 0x0F;
	pEnvInf->slbrk_enable = enable;
	//LANIO
	enable = (g_my_code.option >> 24) & 0x0F;
	pEnvInf->lanio_enable = enable;
	//振れセンサー
	enable = (g_my_code.option >> 20) & 0x0F;
	pEnvInf->sway_sensor_enable = enable;

	if (pEnvInf->sway_sensor_enable)
		init_camera_parameters(ID_SWAY_CAMERA_SETTING_TYPE0);

#if 0
	//### IFウィンドウOPEN
	WPARAM wp = MAKELONG(inf.index, WM_USER_WPH_OPEN_IF_WND);//HWORD:コマンドコード, LWORD:タスクインデックス
	LPARAM lp = BC_ID_MON2;
	SendMessage(inf.hwnd_opepane, WM_USER_TASK_REQ, wp, lp);

	Sleep(1000);
	if (st_mon2.hwnd_mon == NULL) {
		wos << L"Err(MON2 NULL Handle!!):";
		msg2listview(wos.str()); wos.str(L"");
		return S_FALSE;
	}
#endif
	//###  オペレーションパネル設定
	//Function mode RADIO1
	inf.panel_func_id = IDC_TASK_FUNC_RADIO1;
	SendMessage(GetDlgItem(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO1), BM_SETCHECK, BST_CHECKED, 0L);
	for (int i = 1; i < 6; i++)
		SendMessage(GetDlgItem(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO1 + i), BM_SETCHECK, BST_UNCHECKED, 0L);
	//モード設定0
	inf.mode_id = BC_ID_MODE0;
	SendMessage(GetDlgItem(inf.hwnd_opepane, IDC_TASK_MODE_RADIO0), BM_SETCHECK, BST_CHECKED, 0L);
	//モニタウィンドウテキスト	
	set_item_chk_txt();
	set_panel_tip_txt();
	//モニタ2 CB状態セット	
	if (st_mon2.hwnd_mon != NULL)
		SendMessage(GetDlgItem(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK1), BM_SETCHECK, BST_CHECKED, 0L);
	else
		SendMessage(GetDlgItem(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK1), BM_SETCHECK, BST_UNCHECKED, 0L);

	return hr;
}

HRESULT CAuxEnv::routine_work(void* pObj) {
	input();
	parse();
	output();
	return S_OK;
}


int CAuxEnv::input() {


	return S_OK;
}

int CAuxEnv::parse() {
	return S_OK;
}

int CAuxEnv::output() {          //出力処理

	return S_OK;
}

int CAuxEnv::close() {
	return 0;
}

void CAuxEnv::init_camera_parameters(int type) {
	//##### CONFIG_COMMON g_config_common ########
	gp_cnfg_common->img_source_camera = 1;							// カメラ画像取込み(カメラ[1] / 画像ファイル[0])
	gp_cnfg_common->img_source_fname = L"C:\\Work\\Image.bmp";		//取込み画像ファイル名(CMN_IMAGE_SOURCE_CAMERA = 0のときの画像)
	gp_cnfg_common->img_output_fname = L"C:\\Work\\ImageSave.bmp";	// 画像保存ファイル名
	gp_cnfg_common->img_screen_layout.x0 = 25;						//画像表示レイアウト(原点座標X, 原点座標Y, 横幅サイズ, 高さサイズ)
	gp_cnfg_common->img_screen_layout.y0 = 120;
	gp_cnfg_common->img_screen_layout.width = 680;
	gp_cnfg_common->img_screen_layout.height = 480;
	
	gp_cnfg_common->full_pix[(int)ENUM_AXIS::X] = CAM1_SPEC_PIXEL_H;
	gp_cnfg_common->full_pix[(int)ENUM_AXIS::Y] = CAM1_SPEC_PIXEL_V;
	gp_cnfg_common->view_angle_rad[(int)ENUM_AXIS::X] = CAM1_SPEC_ANGLE_RAD_H;
	gp_cnfg_common->view_angle_rad[(int)ENUM_AXIS::Y] = CAM1_SPEC_ANGLE_RAD_V;
	gp_cnfg_common->pix1rad[(int)ENUM_AXIS::X] = CAM1_SPEC_PIXEL_H/CAM1_SPEC_ANGLE_RAD_H;
	gp_cnfg_common->pix1rad[(int)ENUM_AXIS::Y] = CAM1_SPEC_PIXEL_V/CAM1_SPEC_ANGLE_RAD_V;


	//##### CONFIG_CAMERA g_config_common ########
	gp_cnfg_camera->basis.ipaddress = L"172.31.0.30";
	// カメラのROI(領域)の設定
	gp_cnfg_camera->basis.roi[(int)ENUM_AXIS::X].offset = gp_cnfg_camera->basis.roi[(int)ENUM_AXIS::Y].offset = 0;
	gp_cnfg_camera->basis.roi[(int)ENUM_AXIS::X].size = 2048; 
	gp_cnfg_camera->basis.roi[(int)ENUM_AXIS::Y].size = 1536;
	gp_cnfg_camera->basis.packetsize= pCamera->cnfg.packetsize = 0; 
	gp_cnfg_camera->basis.framerate			= 25.0;			//カメラのフレームレートの設定 フル画像では3fps　512x368で最大50fps位が上限
	gp_cnfg_camera->basis.blacklevel		= 0.0;			// カメラの黒レベルの設定
	gp_cnfg_camera->basis.gamma				= 1.0;			// カメラのガンマ補正値の設定
	gp_cnfg_camera->basis.wb.wb_auto		= 1;			// カメラのホワイトバランスゲイン(倍率)の設定
	gp_cnfg_camera->basis.wb.wb_ratio_red	= 1.0;	// カメラのホワイトバランスゲイン(倍率)の設定
	gp_cnfg_camera->basis.wb.wb_ratio_blue	= 3.4;	// カメラのホワイトバランスゲイン(倍率)の設定
	gp_cnfg_camera->basis.viewangle[0]		= 41.9127;	//視野角[deg](X, Y)
	gp_cnfg_camera->basis.viewangle[1]		= 32.0383;	//視野角[deg](X, Y)

	// 輝度コントロール設定(ゲイン)
	gp_cnfg_camera->gain.val				= 0.0;					//ゲイン設定[dB](初期値, 最小値, 最大値)
	gp_cnfg_camera->gain.val_min			= 0.0;				//ゲイン設定[dB](初期値, 最小値, 最大値)
	gp_cnfg_camera->gain.val_max			= 36.0;			//ゲイン設定[dB](初期値, 最小値, 最大値)
	gp_cnfg_camera->gain.auto_control		= 0;			//自動ゲイン設定(固定[0], 自動ゲイン変化量[dB](未使用))
	gp_cnfg_camera->gain.auto_rate			= 0;				//自動ゲイン設定(固定[0], 自動ゲイン変化量[dB](未使用))
	gp_cnfg_camera->gain.auto_start_h		= 0.0;		//自動ゲイン判定輝度(開始上限(この値より輝度が高い場合、暗くする),開始下限(この値より輝度が低い場合、明るくする))[0 - 255]
	gp_cnfg_camera->gain.auto_start_l		= 0.0;		//自動ゲイン判定輝度(開始上限(この値より輝度が高い場合、暗くする),開始下限(この値より輝度が高い場合、明るくする))[0 - 255]
	gp_cnfg_camera->gain.auto_stop_h		= 0.0;			//自動ゲイン判定輝度(停止上限(この値より輝度が低い場合、停止する),停止下限(この値より輝度が高い場合、停止する))  [0 - 255]
	gp_cnfg_camera->gain.auto_stop_l		= 0.0;			//自動ゲイン判定輝度(停止上限(この値より輝度が低い場合、停止する),停止下限(この値より輝度が高い場合、停止する))  [0 - 255]
	
	// 輝度コントロール設定(露光時間)
	gp_cnfg_camera->expstime.val			= 5000.0;			//ゲイン設定[dB](初期値, 最小値, 最大値)
	gp_cnfg_camera->expstime.val_min		= 20.0;		//ゲイン設定[dB](初期値, 最小値, 最大値)
	gp_cnfg_camera->expstime.val_max		= 33000.0;		//ゲイン設定[dB](初期値, 最小値, 最大値)
	gp_cnfg_camera->expstime.auto_control	= 1;		//自動ゲイン設定(固定[0], 自動ゲイン変化量[dB](未使用))
	gp_cnfg_camera->expstime.auto_rate		= 5.0;		//自動ゲイン設定(固定[0], 自動ゲイン変化量[dB](未使用))
	gp_cnfg_camera->expstime.auto_start_h	= 250.0;	//自動露光判定輝度  (開始上限(この値より輝度が高い場合、暗くする)　　[0 - 255]
	gp_cnfg_camera->expstime.auto_start_l	= 230.0;	//自動露光判定輝度  (開始下限(この値より輝度が低い場合、明るくする)　[0 - 255]
	gp_cnfg_camera->expstime.auto_stop_h	= 245.0;	//自動露光判定輝度  (停止上限(この値より輝度が低い場合、停止する)　　[0 - 255]
	gp_cnfg_camera->expstime.auto_stop_l	= 235.0;	//自動露光判定輝度  (停止下限(この値より輝度が高い場合、停止する))  [0 - 255]
	
	//カメラの異常判定設定
	gp_cnfg_camera->error.framedrop			= 15.0;			//カメラの異常判定設定(フレームレート低下の判定値[fps], 異常判定時間[msec])
	gp_cnfg_camera->error.errtime			= 100;			//カメラの異常判定設定(フレームレート低下の判定値[fps], 異常判定時間[msec])
		
	// カメラのゲインの設定(APIへの設定はスレッドで実行される)
	gp_cnfg_camera->boot.bootwaittimer		= 15000;		//カメラの起動設定(システム起動後のカメラ起動待ち時間[msec])
	gp_cnfg_camera->boot.rebootcount		= 1;			//システム起動後のカメラ起動失敗時のカメラ再起動回数[count]
	gp_cnfg_camera->boot.rebootingtimer		= 40000;		//カメラ再起動中を通知する時間[msec])

	//##### CONFIG_IMGPROC g_config_imgproc ########
	gp_cnfg_imgprc->roi.valid = 1;		//画像ROI設定               (ROI有効設定(なし[0]/あり[1]),ROI倍率)
	gp_cnfg_imgprc->roi.scale = 1,5;	//画像ROI設定               (ROI有効設定(なし[0]/あり[1]),ROI倍率)
	
	gp_cnfg_imgprc->mask[0].valid = 0;						//マスク画像選択(両方[0] / 画像1のみ[1] / 画像2のみ[2])
	//画像マスクH[0-179] (下限,上限)  画像マスクS[0-255](下限,上限) 画像1マスクV[0-255](下限,上限)
	gp_cnfg_imgprc->mask[0].hsv_l[0] = 0;	gp_cnfg_imgprc->mask[0].hsv_l[1] = 50;	gp_cnfg_imgprc->mask[0].hsv_l[2] = 150;
	gp_cnfg_imgprc->mask[0].hsv_u[0] = 30;	gp_cnfg_imgprc->mask[0].hsv_u[1] = 255; gp_cnfg_imgprc->mask[0].hsv_u[2] = 255;
	gp_cnfg_imgprc->mask[1].hsv_l[0] = 50;	gp_cnfg_imgprc->mask[1].hsv_l[1] = 50;	gp_cnfg_imgprc->mask[1].hsv_l[2] = 150;
	gp_cnfg_imgprc->mask[1].hsv_u[0] = 90;	gp_cnfg_imgprc->mask[1].hsv_u[1] = 255; gp_cnfg_imgprc->mask[1].hsv_u[2] = 255;

	gp_cnfg_imgprc->filter[0].val	= 2;//ゴマ塩ノイズフィルター(フィルター設定(なし[0] / 中央値フィルター[1] / オープニング処理[2]), フィルター値(中央値フィルター[1, 3, 5, ...] / オープニング処理[1, 2, ...]))
	gp_cnfg_imgprc->filter[0].type	= 2;
	gp_cnfg_imgprc->filter[1].val	= 0;//穴埋めノイズフィルター(フィルター設定(なし[0] / クロージング処理[1]), フィルター値[1, 2, ...])
	gp_cnfg_imgprc->filter[1].type	= 9;
	gp_cnfg_imgprc->imgprc			= 2;				//ターゲット検出アルゴリズム(最大輪郭面積[0], 最大輪郭長[1])
	gp_cnfg_imgprc->swayspdfilter	= 0.1;		//振れ速度フィルター時定数
	gp_cnfg_imgprc->swayzeroset_time = 20000;	//振れゼロ点設定(計測時間[ms])

	// カメラのゲインの設定(APIへの設定はスレッドで実行される)
	pCamera->cnfg.packetsize = 0;

	return;
}


/****************************************************************************/
/*   モニタウィンドウ									                    */
/****************************************************************************/

LRESULT CALLBACK CAuxEnv::Mon1Proc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
	switch (msg)
	{
	case WM_CREATE: {
		InitCommonControls();//コモンコントロール初期化
		HINSTANCE hInst = (HINSTANCE)GetModuleHandle(0);
		//ウィンドウにコントロール追加
		st_mon1.hctrl[ENV_ID_MON1_STATIC_1] = CreateWindowW(TEXT("STATIC"), st_mon1.text[ENV_ID_MON1_STATIC_1], WS_CHILD | WS_VISIBLE | SS_LEFT,
			st_mon1.pt[ENV_ID_MON1_STATIC_1].x, st_mon1.pt[ENV_ID_MON1_STATIC_1].y,
			st_mon1.sz[ENV_ID_MON1_STATIC_1].cx, st_mon1.sz[ENV_ID_MON1_STATIC_1].cy,
			hWnd, (HMENU)(ENV_ID_MON1_CTRL_BASE + ENV_ID_MON1_STATIC_1), hInst, NULL);

		//表示更新用タイマー
		SetTimer(hWnd, ENV_ID_MON1_TIMER, st_mon1.timer_ms, NULL);

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

	}break;

	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
	}break;
	case WM_DESTROY: {
		st_mon1.hwnd_mon = NULL;
		KillTimer(hWnd, ENV_ID_MON1_TIMER);
	}break;
	default:
		return DefWindowProc(hWnd, msg, wp, lp);
	}
	return S_OK;
};

static wostringstream mon2wos;
LRESULT CALLBACK CAuxEnv::Mon2Proc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
	switch (msg)
	{
	case WM_CREATE: {
		InitCommonControls();//コモンコントロール初期化
		HINSTANCE hInst = (HINSTANCE)GetModuleHandle(0);
		//ウィンドウにコントロール追加
		//STATIC,LABEL
		for (int i = ENV_ID_MON2_STATIC_MSG; i <= ENV_ID_MON2_STATIC_MSG; i++) {
			st_mon2.hctrl[i] = CreateWindowW(TEXT("STATIC"), st_mon2.text[i], WS_CHILD | WS_VISIBLE | SS_LEFT,
				st_mon2.pt[i].x, st_mon2.pt[i].y, st_mon2.sz[i].cx, st_mon2.sz[i].cy,
				hWnd, (HMENU)(ENV_ID_MON2_CTRL_BASE + i), hInst, NULL);
		}

		SetTimer(hWnd, ENV_ID_MON2_TIMER, ENV_PRM_MON2_TIMER_MS, NULL);

	}break;
	case WM_COMMAND: {
		int wmId = LOWORD(wp);

		int _Id = wmId - ENV_ID_MON2_CTRL_BASE;
		// 選択されたメニューの解析:
		switch (_Id)
		{

		default:
			return DefWindowProc(hWnd, msg, wp, lp);
		}
	}break;
	case WM_TIMER: {

	}break;

	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
	}break;
	case WM_DESTROY: {
		KillTimer(hWnd, ENV_ID_MON2_TIMER);
		st_mon2.hwnd_mon = NULL;
	}break;
	default:
		return DefWindowProc(hWnd, msg, wp, lp);
	}
	return S_OK;
}

HWND CAuxEnv::open_monitor_wnd(HWND h_parent_wnd, int id) {

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
		wcex.lpszMenuName = TEXT("CS_MON1");
		wcex.lpszClassName = TEXT("CS_MON1");
		wcex.hIconSm = NULL;

		ATOM fb = RegisterClassExW(&wcex);

		st_mon1.hwnd_mon = CreateWindowW(TEXT("ENV_MON1"), TEXT("ENV_MON1"), WS_OVERLAPPEDWINDOW,
			ENV_MON1_WND_X, ENV_MON1_WND_Y, ENV_MON1_WND_W, ENV_MON1_WND_H,
			h_parent_wnd, nullptr, hInst, nullptr);
		show_monitor_wnd(id);

		wos.str(L"");
		if (st_mon1.hwnd_mon != NULL) wos << L"Succeed : MON1 open";
		else                          wos << L"!! Failed : MON1 open";
		msg2listview(wos.str());

		return st_mon1.hwnd_mon;
	}
	else if (id == BC_ID_MON2) {//通信用ウィンドウ
		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = Mon2Proc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = hInst;
		wcex.hIcon = NULL;
		wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wcex.lpszMenuName = TEXT("ENV_MON2");
		wcex.lpszClassName = TEXT("ENV_MON2");
		wcex.hIconSm = NULL;

		ATOM fb = RegisterClassExW(&wcex);

		st_mon2.hwnd_mon = CreateWindowW(TEXT("ENV_MON2"), TEXT("ENV_MON2"), WS_OVERLAPPEDWINDOW,
			ENV_MON2_WND_X, ENV_MON2_WND_Y, ENV_MON2_WND_W, ENV_MON2_WND_H,
			h_parent_wnd, nullptr, hInst, nullptr);

		wos.str(L"");
		if (st_mon2.hwnd_mon != NULL) wos << L"Succeed : MON2 open";
		else                          wos << L"!! Failed : MON2 open";
		msg2listview(wos.str());


		return st_mon2.hwnd_mon;
	}
	else
	{
		return NULL;
	};

	return NULL;
}
void CAuxEnv::close_monitor_wnd(int id) {
	wos.str(L"");
	if (id == BC_ID_MON1) {
		DestroyWindow(st_mon1.hwnd_mon);
		wos << L"MON1 closed";
	}
	else if (id == BC_ID_MON2) {
		DestroyWindow(st_mon2.hwnd_mon);
		wos << L"MON2 closed";
	}
	else;
	msg2listview(wos.str());
	return;
}
void CAuxEnv::show_monitor_wnd(int id) {
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
void CAuxEnv::hide_monitor_wnd(int id) {
	if ((id == BC_ID_MON1) && (st_mon1.hwnd_mon != NULL)) {
		ShowWindow(st_mon1.hwnd_mon, SW_HIDE);
		st_mon1.is_monitor_active = false;
	}
	else if ((id == BC_ID_MON2) && (st_mon2.hwnd_mon != NULL)) {
		ShowWindow(st_mon2.hwnd_mon, SW_HIDE);
		st_mon2.is_monitor_active = false;
	}
	else;
	return;
}

/****************************************************************************/
/*   タスク設定タブパネルウィンドウのコールバック関数                       */
/****************************************************************************/
LRESULT CALLBACK CAuxEnv::PanelProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp) {

	switch (msg) {
	case WM_USER_TASK_REQ: {
		if (HIWORD(wp) == WM_USER_WPH_OPEN_IF_WND) {
			if (lp == BC_ID_MON1) st_mon1.hwnd_mon = open_monitor_wnd(hDlg, (int)lp);
			if (lp == BC_ID_MON2) st_mon2.hwnd_mon = open_monitor_wnd(hDlg, (int)lp);
		}
		else if (wp == WM_USER_WPH_CLOSE_IF_WND) close_monitor_wnd(lp);
		else;
	}break;
	case WM_COMMAND:
		switch (LOWORD(wp)) {
		case IDC_TASK_FUNC_RADIO1:
		case IDC_TASK_FUNC_RADIO2:
		case IDC_TASK_FUNC_RADIO3:
		case IDC_TASK_FUNC_RADIO4:
		case IDC_TASK_FUNC_RADIO5:
		case IDC_TASK_FUNC_RADIO6:
		{
			inf.panel_func_id = LOWORD(wp);
			set_panel_tip_txt();
			set_item_chk_txt();
			set_PNLparam_value(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
		}break;

		case IDC_TASK_ITEM_CHECK1: {
			switch (inf.panel_func_id) {
			case IDC_TASK_FUNC_RADIO4:
				set_item_chk_txt();
				break;
			default:break;
			}

		}break;
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
				open_monitor_wnd(inf.hwnd_parent, BC_ID_MON1);
			}
			else {
				close_monitor_wnd(BC_ID_MON1);
			}
		}break;

		case IDC_TASK_MON_CHECK2: {

			if (IsDlgButtonChecked(hDlg, IDC_TASK_MON_CHECK2) == BST_CHECKED) {

				show_monitor_wnd(BC_ID_MON2);
			}
			else {
				//				close_monitor_wnd(BC_ID_MON2);
				hide_monitor_wnd(BC_ID_MON2);
			}
		}break;
		}
	}
	return 0;
};

///###	タブパネルのListViewにメッセージを出力
void CAuxEnv::msg2listview(wstring wstr) {

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
void CAuxEnv::set_PNLparam_value(float p1, float p2, float p3, float p4, float p5, float p6) {
	wstring wstr;
	wstr += std::to_wstring(p1); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT1), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p2); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT2), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p3); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT3), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p4); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT4), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p5); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT5), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p6); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT6), wstr.c_str());
}
//タブパネルのEdit Box説明テキストを設定
void CAuxEnv::set_panel_tip_txt() {
	wstring wstr_type; wstring wstr;
	switch (inf.panel_func_id) {
	case IDC_TASK_FUNC_RADIO4: {
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
	case IDC_TASK_FUNC_RADIO1:
	case IDC_TASK_FUNC_RADIO2:
	case IDC_TASK_FUNC_RADIO3:
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
void CAuxEnv::set_func_pb_txt() {
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO1, L"OTE IF");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO2, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO3, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO4, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO5, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO6, L"-");
	return;
}
//タブパネルのItem chkテキストを設定
void CAuxEnv::set_item_chk_txt() {
	wstring wstr_type; wstring wstr;
	switch (inf.panel_func_id) {
	case IDC_TASK_FUNC_RADIO1: {
		SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK1, L"WND");
		SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK2, L"-");
		SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK3, L"-");
		SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK4, L"-");
		SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK5, L"-");
		SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK6, L"-");
	}break;

	case IDC_TASK_FUNC_RADIO2:
	case IDC_TASK_FUNC_RADIO3:
	case IDC_TASK_FUNC_RADIO4:
	case IDC_TASK_FUNC_RADIO5:
	case IDC_TASK_FUNC_RADIO6:
	default:
		SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK1, L"-");
		SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK2, L"-");
		SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK3, L"-");
		SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK4, L"-");
		SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK5, L"-");
		SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK6, L"-");
		break;
	}
	return;
}

