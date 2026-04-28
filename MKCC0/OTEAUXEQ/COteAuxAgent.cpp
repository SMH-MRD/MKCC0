
#include "COteAuxAgent.h"
#include "COteAuxPol.h"
#include "resource.h"
#include "CSHAREDMEM.H"
#include "SmemOte.H"

#include <thread>
#include <mutex>
#include <atomic>


#include <StApi_TL.h>   //Transport Layer機能(必須)
#include <StApi_IP.h>   //Image Processing機能(オプション)

#pragma comment (lib, "Gdiplus.lib")

//Namespace for using StApi.
using namespace StApi;
using namespace cv;
using namespace Gdiplus;


ST_AUXAG_MON1 COteAuxAgent::st_mon1;
ST_AUXAG_MON2 COteAuxAgent::st_mon2;

extern CSharedMem* pAuxEnvInfObj;
extern CSharedMem* pAuxAgentInfObj;
extern CSharedMem* pAuxCsInfObj;
extern CSharedMem* pAuxPolInfObj;
extern CSharedMem* pCsInfObj;

extern BC_TASK_ID st_task_id;
extern vector<CBasicControl*>	    VectCtrlObj;	    //スレッドオブジェクトのポインタ

static COteAuxAgent* pAgentObj;
static COteAuxPol* pPolObj;

//共有メモリ
static LPST_OTE_AUX_ENV_INF		pAuxEnvInf = NULL;
static LPST_OTE_AUX_CS_INF		pAuxCSInf = NULL;
static LPST_OTE_AUX_AGENT_INF	pAuxAgInf = NULL;
static LPST_OTE_AUX_POL_INF		pAuxPolInf = NULL;
static LPST_OTE_CS_INF			pCSInf = NULL;

std::thread g_capThread;// スレッド変数が消えないようにグローバル領域に保持
std::atomic<bool> g_keepRunning = false;
HANDLE g_hStopEvent = NULL;   // 停止指示用イベント

static wostringstream wos_cam;

std::unique_ptr<Bitmap>   COteAuxAgent::m_pOffscreenBitmap;
std::unique_ptr<Gdiplus::Graphics> COteAuxAgent::m_pOffscreenGraphics;

Graphics* COteAuxAgent::pgraphic_img;	//描画用グラフィックス


COteAuxAgent::COteAuxAgent() {
	// 1. GDI+ 初期化
	GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);

	m_pOffscreenBitmap = std::make_unique<Bitmap>(AUXAG_MON2_WND_W, AUXAG_MON2_WND_H, PixelFormat32bppARGB);

	// 2. そのバッファに描き込むための Graphics オブジェクトを作成
	m_pOffscreenGraphics = std::unique_ptr<Graphics>(Graphics::FromImage(m_pOffscreenBitmap.get()));

}
COteAuxAgent::~COteAuxAgent() {
	GdiplusShutdown(m_gdiplusToken);
}


HRESULT COteAuxAgent::initialize(LPVOID lpParam){

	//### 出力用共有メモリ取得
	out_size = sizeof(ST_OTE_AUX_AGENT_INF);
	set_outbuf(pAuxAgentInfObj->get_pMap());

	//### 入力用共有メモリ取得
	pAuxAgInf = (LPST_OTE_AUX_AGENT_INF)pAuxAgentInfObj->get_pMap();
	pAuxEnvInf = (LPST_OTE_AUX_ENV_INF)(pAuxEnvInfObj->get_pMap());
	pAuxCSInf = (LPST_OTE_AUX_CS_INF)pAuxCsInfObj->get_pMap();
	pAuxPolInf = (LPST_OTE_AUX_POL_INF)pAuxPolInfObj->get_pMap();

	pAgentObj = (COteAuxAgent*)VectCtrlObj[st_task_id.AGENT];
	// スレッド停止用のイベントを作成（マニュアルリセット型）
	g_hStopEvent = CreateEvent(NULL, FALSE, FALSE, NULL);//自動リセット,初期値非シグナル


	//### 初期化
	 // GDI+ の初期化
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken = 0;
	Status status = GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, nullptr);
	if (status != Ok) {
		MessageBox(nullptr, L"GDI+ の初期化に失敗しました。", L"Error", MB_ICONERROR);
		return -1;
	}

	//システム周波数読み込み
	QueryPerformanceFrequency(&pAuxAgInf->frequency);

	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_MON_CHECK2, L"USBcam");

	//モニタウィンドウテキスト	
	set_func_pb_txt();
	set_item_chk_txt();
	set_panel_tip_txt();
	return S_OK;
}

HRESULT COteAuxAgent::routine_work(void* pObj){
	input();
	parse();
	output();

	if (inf.total_act % 10 == 0) {
		wstring status_str;
		status_str =	(pAuxAgInf->v_delay_chk_status & OTEAUXAG_CODE_V_DELAY_STANDBY) ? L"STBY" :
						(pAuxAgInf->v_delay_chk_status & OTEAUXAG_CODE_V_DELAY_TRIG_ON_CHK) ? L"ON CHK" :
						(pAuxAgInf->v_delay_chk_status & OTEAUXAG_CODE_V_DELAY_TRIG_OFF_CHK) ? L"OFF CHK" :
						(pAuxAgInf->v_delay_chk_status & OTEAUXAG_CODE_V_DELAY_CHK_TMOV) ? L"TMOV" :
						(pAuxAgInf->v_delay_chk_status & OTEAUXAG_CODE_V_DELAY_CHK_FIN) ? L"FIN" : L"UNK";

		wos.str(L""); wos << inf.status << L":" << std::setfill(L'0') << std::setw(4) << inf.act_time <<L" CamCount:"<<st_mon2.thrad_counter
			<<L" DelayChkStatus:"<< status_str.c_str() << L" Delay:" << pAuxAgInf->v_delay_sec << L" TMOV:" << pAuxAgInf->v_delay_tmov_cnt << L" FIN:" << pAuxAgInf->v_delay_chk_fin_cnt;
		msg2host(wos.str());
	}

	return S_OK;
}

int COteAuxAgent::input() {
		
	return S_OK;
}
int COteAuxAgent::parse() {           //メイン処理

	//# 映像遅延計測処理
	if (pAuxEnvInf->video_delay_chk_func_active == L_ON) {
		double delay_sec = 0.0;
		//## スレッドを起動してカメラキャプチャを開始
		if ((!g_keepRunning) && (pAuxAgInf->st_usb_cam.retry_count == 0)) {
			camera_capture_start();
		}

		//## 処理ステップ判定処理
		//遅延計測処理
		if (pAuxAgInf->v_delay_chk_status & OTEAUXAG_CODE_V_DELAY_STANDBY) {
			//計測トリガ処理
			if (pPolObj->GetCraneDeviceStatus(&(pAuxPolInf->st_img_proc)) == L_ON) {
				pAuxAgInf->v_delay_chk_status |= OTEAUXAG_CODE_V_DELAY_TRIG_OFF_CHK;
				pAuxAgInf->v_delay_chk_status &= ~OTEAUXAG_CODE_V_DELAY_TRIG_OFF_CHK;
				pAuxAgInf->req_command_to_crane = OTEAUXAG_COM_CRANE_DEVICE_DEACTIVE;	//クレーン装置にOFFコマンド送信
				QueryPerformanceCounter(&pAuxAgInf->start_count);//計測開始カウンタ値取得
			}
			else if (pPolObj->GetCraneDeviceStatus(&(pAuxPolInf->st_img_proc)) == L_OFF) {
				pAuxAgInf->v_delay_chk_status |= OTEAUXAG_CODE_V_DELAY_TRIG_ON_CHK;
				pAuxAgInf->v_delay_chk_status &= ~OTEAUXAG_CODE_V_DELAY_TRIG_OFF_CHK;
				pAuxAgInf->req_command_to_crane = OTEAUXAG_COM_CRANE_DEVICE_ACTIVE;	//クレーン装置にONコマンド送信
				QueryPerformanceCounter(&pAuxAgInf->start_count);//計測開始カウンタ値取得
			}
			else;
		}
		else if (pAuxAgInf->v_delay_chk_status & OTEAUXAG_CODE_V_DELAY_TRIG_ON_CHK) {
			QueryPerformanceCounter(&pAuxAgInf->end_count);//計測完了カウンタ値取得
			delay_sec = (double)(pAuxAgInf->end_count.QuadPart - pAuxAgInf->start_count.QuadPart) / (double)pAuxAgInf->frequency.QuadPart;

			if (pPolObj->GetCraneDeviceStatus(&(pAuxPolInf->st_img_proc)) == L_ON) {
				pAuxAgInf->v_delay_chk_status = OTEAUXAG_CODE_V_DELAY_CHK_FIN;
				pAuxAgInf->v_delay_chk_fin_cnt++;
			}
			else {
				//タイムオーバー判定LV1 (フラグセット）
				if (delay_sec > AUXAG_PRM_V_DELAY_TMOV_LV1)
					pAuxAgInf->v_delay_chk_status |= OTEAUXAG_CODE_V_DELAY_CHK_TMOV;
				//タイムオーバー判定LV2 測定打ち切り判定
				if (delay_sec > AUXAG_PRM_V_DELAY_TMOV_LV2) {
					pAuxAgInf->v_delay_chk_status = OTEAUXAG_CODE_V_DELAY_CHK_TMOV;
					pAuxAgInf->v_delay_sec = delay_sec;
				}

			}
		}
		else if (pAuxAgInf->v_delay_chk_status & OTEAUXAG_CODE_V_DELAY_TRIG_OFF_CHK) {
			QueryPerformanceCounter(&pAuxAgInf->end_count);//計測完了カウンタ値取得
			delay_sec = (double)(pAuxAgInf->end_count.QuadPart - pAuxAgInf->start_count.QuadPart) / (double)pAuxAgInf->frequency.QuadPart;
			if (pPolObj->GetCraneDeviceStatus(&(pAuxPolInf->st_img_proc)) == L_OFF) {
				pAuxAgInf->v_delay_chk_status = OTEAUXAG_CODE_V_DELAY_CHK_FIN;
				pAuxAgInf->v_delay_chk_fin_cnt++;
			}
			else {
				//タイムオーバー判定LV1(フラグセット）
				if (delay_sec > AUXAG_PRM_V_DELAY_TMOV_LV1)
					pAuxAgInf->v_delay_chk_status |= OTEAUXAG_CODE_V_DELAY_CHK_TMOV;
				//タイムオーバー判定LV2 測定打ち切り判定
				if (delay_sec > AUXAG_PRM_V_DELAY_TMOV_LV2) {
					pAuxAgInf->v_delay_chk_status = OTEAUXAG_CODE_V_DELAY_CHK_TMOV;
					pAuxAgInf->v_delay_sec = delay_sec;
				}
			}
		}
		else if(pAuxAgInf->v_delay_chk_status == OTEAUXAG_CODE_V_DELAY_CHK_FIN){
			QueryPerformanceCounter(&pAuxAgInf->end_count);//計測完了カウンタ値取得
			pAuxAgInf->v_delay_sec = (double)(pAuxAgInf->end_count.QuadPart - pAuxAgInf->start_count.QuadPart) / (double)pAuxAgInf->frequency.QuadPart;
			pAuxAgInf->v_delay_chk_status |= OTEAUXAG_CODE_V_DELAY_STANDBY;
		}
		else if (pAuxAgInf->v_delay_chk_status == OTEAUXAG_CODE_V_DELAY_CHK_TMOV) {
			pAuxAgInf->v_delay_tmov_cnt++;
			pAuxAgInf->v_delay_chk_status |= OTEAUXAG_CODE_V_DELAY_STANDBY;
		}
		else;

		//デバイス強制ON要求処理
		if (pAuxEnvInf->video_delay_chk_device_forth_on) {
			pAuxAgInf->v_delay_chk_status |= OTEAUXAG_CODE_V_DELAY_TRIG_ON_CHK;
			pAuxAgInf->req_command_to_crane = OTEAUXAG_COM_CRANE_DEVICE_ACTIVE;	//クレーン装置にONコマンド送信
		}

		//自動パラメータ設定要求処理
		if ((pAuxEnvInf->video_delay_chk_auto_prm_set_req) && !(pAuxAgInf->v_delay_chk_status & OTEAUXAG_CODE_V_DELAY_AUTO_PRM_START)) {
			pAuxAgInf->v_delay_chk_status |= OTEAUXAG_CODE_V_DELAY_AUTO_PRM_START;
			//自動パラメータ設定処理開始
		}
		else if ((!pAuxEnvInf->video_delay_chk_auto_prm_set_req) && (pAuxAgInf->v_delay_chk_status & (OTEAUXAG_CODE_V_DELAY_AUTO_PRM_FIN | OTEAUXAG_CODE_V_DELAY_AUTO_PRM_FAIL))) {
			pAuxAgInf->v_delay_chk_status &= ~OTEAUXAG_CODE_V_DELAY_AUTO_PRM_START;
			pAuxAgInf->v_delay_chk_status &= ~(OTEAUXAG_CODE_V_DELAY_AUTO_PRM_FIN | OTEAUXAG_CODE_V_DELAY_AUTO_PRM_FAIL);
		}
		else if (pAuxAgInf->v_delay_chk_status & OTEAUXAG_CODE_V_DELAY_AUTO_PRM_START) {

		}
		else;
			//自動パラメータ設定処理終了

	}
	else {
		if (g_keepRunning) {
			SetEvent(g_hStopEvent);//スレッド停止指示
			Sleep(1000);
			if (g_capThread.joinable()) {
				g_capThread.join();
			}
		}
		pAuxAgInf->v_delay_chk_status = OTEAUXAG_CODE_V_DELAY_STANDBY;
		pAuxAgInf->v_delay_sec = 0.0;
	}

	
	//### 映像遅延計測処理
	if (pAuxAgInf->st_usb_cam.retry_count > 0)pAuxAgInf->st_usb_cam.retry_count--;


	return STAT_OK;
}
int COteAuxAgent::output() {          //出力処理
	
	return STAT_OK;
}
int COteAuxAgent::close() {
   if (g_capThread.joinable()) {
	   g_capThread.join();
   }
	return 0;
}






static wostringstream monwos;
LRESULT CALLBACK COteAuxAgent::Mon1Proc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
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

LRESULT CALLBACK COteAuxAgent::Mon2Proc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
	switch (msg)
	{
	case WM_CREATE: {
		InitCommonControls();//コモンコントロール初期化

		//	setup_graphics(hWnd);
		HINSTANCE hInst = (HINSTANCE)GetModuleHandle(0);
		//ウィンドウにコントロール追加

		//タイマー起動
		UINT rtn = SetTimer(hWnd, AUXAG_ID_MON2_TIMER, AUXAG_PRM_MON2_TIMER_MS, NULL);

	    // スレッド停止用のイベントを作成（マニュアルリセット型）
	//	g_hStopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
		// スレッドを起動してカメラキャプチャを開始
	//	camera_capture_start();

		setup_graphics(hWnd);

		break;

	}
	case WM_TIMER: {
		//InvalidateRect(hWnd, NULL, FALSE);
		if (st_mon2.is_monitor_active)
			OnPaintMon2(st_mon2.hwnd_mon, st_mon2.hdc);
	}break;
	case WM_COMMAND: {
		int wmId = LOWORD(wp);
		// 選択されたメニューの解析:
		switch (wmId - AUXAG_ID_MON2_CTRL_BASE)
		{
										 
		default:
			return DefWindowProc(hWnd, msg, wp, lp);
		}
	}break;
	case WM_PAINT: {

		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;
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

void COteAuxAgent::OnPaintMon2(HWND hWnd, HDC hdc) {

	std::lock_guard<std::mutex> lock(pAuxAgInf->st_usb_cam.g_mtx);
	if (!pAuxAgInf->st_usb_cam.bgrMatFrame.empty()) {
		// --- 手順1: Bitmapバッファへ順次描画 ---
		Gdiplus::Bitmap bitmap(pAuxAgInf->st_usb_cam.bgrMatFrame.cols, pAuxAgInf->st_usb_cam.bgrMatFrame.rows,
			(int)pAuxAgInf->st_usb_cam.bgrMatFrame.step, PixelFormat24bppRGB, pAuxAgInf->st_usb_cam.bgrMatFrame.data);

		int img_w = bitmap.GetWidth(), img_h = bitmap.GetHeight();
		m_pOffscreenGraphics->DrawImage(&bitmap, 0, 0, img_w, img_h);

		// --- 手順2: 完成したバッファを画面(HDC)へ一気に転送する ---
		pgraphic_img->DrawImage(m_pOffscreenBitmap.get(), 0, 0);
	}

	return;
}

HWND COteAuxAgent::open_monitor_wnd(HWND h_parent_wnd, int id) {
	
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
		show_monitor_wnd(id);
		if(st_mon1.hwnd_mon!=NULL)st_mon1.is_monitor_active = true;
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

		st_mon2.hwnd_mon = CreateWindowW(TEXT("AUXAG_MON2"), TEXT("USB CAMERA"), WS_OVERLAPPEDWINDOW,
			AUXAG_MON2_WND_X, AUXAG_MON2_WND_Y, AUXAG_MON2_WND_W, AUXAG_MON2_WND_H,
			h_parent_wnd, nullptr, hInst, nullptr);
		show_monitor_wnd(id);
		if (st_mon2.hwnd_mon != NULL)st_mon2.is_monitor_active = true;
		return st_mon2.hwnd_mon;
	}
	else
	{
		return NULL;
	};

	return NULL;
}
void COteAuxAgent::close_monitor_wnd(int id) {
	if (id == BC_ID_MON1)
		DestroyWindow(st_mon1.hwnd_mon);
	else if (id == BC_ID_MON2)
		DestroyWindow(st_mon2.hwnd_mon);
	else;
	return;
}
void COteAuxAgent::show_monitor_wnd(int id) { 
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
void COteAuxAgent::hide_monitor_wnd(int id) { 
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
LRESULT CALLBACK COteAuxAgent::PanelProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp) {

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
			if (inf.panel_func_id == IDC_TASK_FUNC_RADIO1) {
			}
			
			if (IsDlgButtonChecked(hDlg, LOWORD(wp)) == BST_CHECKED)
				inf.panel_act_chk[inf.panel_func_id - IDC_TASK_FUNC_RADIO1][LOWORD(wp) - IDC_TASK_ITEM_CHECK1] = true;
			else
				inf.panel_act_chk[inf.panel_func_id - IDC_TASK_FUNC_RADIO1][LOWORD(wp) - IDC_TASK_ITEM_CHECK1] = false;
		}break;

		case IDSET:
		{
			camera_capture_start();
		}break;
		case IDRESET:
		{
			InvalidateRect(st_mon2.hwnd_mon, NULL, TRUE);
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
				st_mon1.is_monitor_active = false;
			}
		}break;

		case IDC_TASK_MON_CHECK2: {
			if (IsDlgButtonChecked(hDlg, IDC_TASK_MON_CHECK2) == BST_CHECKED) {
				open_monitor_wnd(inf.hwnd_parent, BC_ID_MON2);
			}
			else {
				close_monitor_wnd(BC_ID_MON2);
				st_mon2.is_monitor_active = false;
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
void COteAuxAgent::msg2listview(wstring wstr) {

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

void COteAuxAgent::set_PNLparam_value(float p1, float p2, float p3, float p4, float p5, float p6) {
	wstring wstr;
	wstr += std::to_wstring(p1); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT1), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p2); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT2), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p3); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT3), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p4); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT4), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p5); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT5), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p6); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT6), wstr.c_str());
}

//タブパネルのStaticテキストを設定
void COteAuxAgent::set_panel_tip_txt() {
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
void COteAuxAgent::set_func_pb_txt() {
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO1, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO2, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO3, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO4, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO5, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO6, L"-");
	return;
}
//タブパネルのItem chkテキストを設定
void COteAuxAgent::set_item_chk_txt() {
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK1, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK2, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK3, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK4, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK5, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK6, L"-");
	return;
}

void COteAuxAgent::UsbCameraThreadAG() {

	CIStSystemPtr				pSystem = nullptr;
	CIStDevicePtr				pDevice = nullptr;
	CIStDataStreamPtr			pDataStream = nullptr;
	IStImageBufferReleasable* pDestImage = nullptr;

	// 20ms周期のタイマー作成
	HANDLE hTimer = CreateWaitableTimer(NULL, FALSE, NULL);
	LARGE_INTEGER liDueTime;
	liDueTime.QuadPart = -1000000LL; // 初回：100ms後 (100ナノ秒単位、負の値は相対)
	SetWaitableTimer(hTimer, &liDueTime, 20, NULL, NULL, 0); // 以降20ms間隔

	HANDLE hEvents[2] = { g_hStopEvent, hTimer };


	//USBカメラキャプチャ開始

		//SDKの初期化オブジェクトを作成。これにより、SDKの初期化と終了処理が自動的に行われる。
	CStApiAutoInit stApiInit;
	try {
		pSystem = StApi::CreateIStSystem();
		pDevice = pSystem->CreateFirstIStDevice();
		pDataStream = pDevice->CreateIStDataStream();

		pDestImage = StApi::CreateIStImageBuffer();

		pDataStream->SetStreamBufferCount(2); // デフォルト（8〜16など）から最小の2〜3に変更　遅延を減らすため

		pDataStream->StartAcquisition();
		pDevice->AcquisitionStart();
		pAuxAgInf->st_usb_cam.status = OTEAUXAG_CAMERA_STAT_NORMAL;
		pAuxAgInf->st_usb_cam.retry_count = 0;
	}
	catch (const GenICam::GenericException& e) {
		wos_cam.str(L""); wos_cam << "USB CAM Init Exception: " << e.GetDescription() << endl;
		pAgentObj->msg2listview(wos_cam.str().c_str());
		pAuxAgInf->st_usb_cam.status = OTEAUXAG_CAMERA_STAT_INIT_ERR;
		pAuxAgInf->st_usb_cam.retry_count = OTEAUXAG_CAMERA_PRM_RETRY_CNT;
		g_keepRunning = false;
		wos_cam.str(L""); wos_cam << "Exit Thread Loop: " << endl;
		pAgentObj->msg2listview(wos_cam.str().c_str());

		return;
	}

	g_keepRunning = true; // ループ制御フラグを立てる

	while (g_keepRunning) {
		try {
			// WaitForMultipleObjects で停止イベントまたはタイマーを待機
			DWORD dwWait = WaitForMultipleObjects(2, hEvents, FALSE, INFINITE);

			if (dwWait == WAIT_OBJECT_0) {		//終了イベント
				// StopEventがセットされたらループ脱出
				pAuxAgInf->st_usb_cam.status = OTEAUXAG_CAMERA_STAT_SUSPEND;
				pAuxAgInf->st_usb_cam.retry_count = 0;
				break;
			}

			if (dwWait == WAIT_OBJECT_0 + 1) {	//タイマーイベント
				auto pBuffer = pDataStream->RetrieveBuffer(50);

				if (pBuffer != nullptr) {
					CIStStreamBufferPtr pIStStreamBuffer(pBuffer);
					auto pIStImage = pBuffer->GetIStImage();

					if (pIStImage != nullptr) {
						// 2. 画像情報の取得
						pAuxAgInf->st_usb_cam.width	= (int)pIStImage->GetImageWidth();
						pAuxAgInf->st_usb_cam.height	= (int)pIStImage->GetImageHeight();
						pAuxAgInf->st_usb_cam.stride	= pIStImage->GetImageLinePitch(); // 1行のバイト数
						pAuxAgInf->st_usb_cam.pRawData = pIStImage->GetImageBuffer();

						{
							std::lock_guard<std::mutex> lock(pAuxAgInf->st_usb_cam.g_mtx);
							// 3. OpenCVのMatに生のBayerデータを読み込み ※ 8bit (CV_8UC1) であることを前提としています
							cv::Mat rawMat(pAuxAgInf->st_usb_cam.height, pAuxAgInf->st_usb_cam.width, CV_8UC1, pAuxAgInf->st_usb_cam.pRawData, pAuxAgInf->st_usb_cam.stride);
							
							// 4. OpenCVでBGRへ変換
							// 注意: ここではBayerGR2RGBを使用していますが。bgrMatFrameの中身は、BGR順のデータになる様です。表示や処理の際は、OpenCVの慣習に従ってBGRとして扱ってください。
							cv::cvtColor(rawMat, pAuxAgInf->st_usb_cam.bgrMatFrame, cv::COLOR_BayerGR2RGB);
							
							// 5. OpenCVでHSV Matを作成
							cv::cvtColor(pAuxAgInf->st_usb_cam.bgrMatFrame, pAuxAgInf->st_usb_cam.hsvMatFrame, cv::COLOR_BGR2HSV);
							pAuxAgInf->st_usb_cam.isRawMatUpdated = true; // フレームが更新されたことを示すフラグを立てる
						}
					}
					st_mon2.thrad_counter++;
				}
			}
		}
		catch (const GenICam::GenericException& e) {
			wos_cam.str(L""); wos_cam << "USB CAM Capture Exception: " << e.GetDescription() << endl;
			pAgentObj->msg2listview(wos_cam.str().c_str());
			pAuxAgInf->st_usb_cam.isRawMatUpdated = false;
		}
	}
	pDevice->AcquisitionStop();
	pDataStream->StopAcquisition();
	
	wos_cam.str(L""); wos_cam << "Exit Thread Loop: "<< endl;
	pAgentObj->msg2listview(wos_cam.str().c_str());

	CloseHandle(hTimer);
	g_keepRunning = false;

	return;
}
void COteAuxAgent::camera_capture_start() {
	// 1. もし既に動いていたり、古い残骸があれば片付ける
	camera_capture_stop();

	// 2. フラグを立て直す
	g_keepRunning = true;

	// 3. 新しいスレッドを生成して代入（ムーブ代入）
	// これにより、同じ g_workerThread 変数で新しい処理が始まる
	g_capThread = std::thread(UsbCameraThreadAG);
	pAgentObj->wos.str(L""); pAgentObj->wos << "New thread launched." << std::endl;
	pAgentObj->msg2listview(pAgentObj->wos.str().c_str());
}
void COteAuxAgent::camera_capture_stop() {
	if (g_capThread.joinable()) {
		g_keepRunning = false; // フラグを倒してスレッドに終了を促す
		g_capThread.join(); // スレッドが完全に終わるのを待って片付ける
		pAgentObj->wos.str(L""); pAgentObj->wos << "Thread joined and cleaned up." << std::endl;
		pAgentObj->msg2listview(pAgentObj->wos.str().c_str());
	}
}
HRESULT COteAuxAgent::setup_graphics(HWND hwnd) {
	clear_graphics();
	RECT rect; GetClientRect(hwnd, &rect);

	st_mon2.hdc = GetDC(hwnd);
	if (st_mon2.hdc == NULL) return E_FAIL;

	//グラフィックオブジェクト作成
	pgraphic_img = new Gdiplus::Graphics(st_mon2.hdc);
	if (pgraphic_img == NULL) return E_FAIL;
	pgraphic_img->SetSmoothingMode(SmoothingModeAntiAlias);
	pgraphic_img->SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
	pgraphic_img->SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);

	return S_OK;
}
void COteAuxAgent::clear_graphics() {
	if (pgraphic_img != NULL)		delete pgraphic_img;
	DeleteDC(st_mon2.hdc);
	return;
}

