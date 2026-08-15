// LibMKLog.cpp : スタティック ライブラリ用の関数を定義します。
//

#include "pch.h"
#include "framework.h"
#include "..\Inc\CMKLog.h"

#include <thread>
#include <fstream>
#include<sstream>

#include "..\Inc\mklog_itm_db.h"

// ライブラリ
#pragma comment( lib, "gdiplus.lib" )

#include <objbase.h>//gdiplus.hのコンパイルを通すために必要
#include <gdiplus.h>
using namespace Gdiplus;

//@GDI+
GdiplusStartupInput gdiplusStartupInput;
ULONG_PTR gdiplusToken;

std::wostringstream CMKLog::wos, CMKLog::filename;
ST_MKLOG_WORK_WND CMKLog::st_work_wnd;

UN_MKLOG_WND_ACTIVE CMKLog::is_log_wnd_active;

ST_LOG_FILE_BUF CMKLog::logbuf[MKLOG_N_ID_TYPE];
INT16 CMKLog::loghot[MKLOG_N_ID_TYPE][MKLOG_N_LOG_ITEM_MAX];//ログ項目表示値現在値
INT32 CMKLog::logidhot[MKLOG_N_ID_TYPE];

ST_TIMELOG_WINDOW CMKLog::st_timelog_wnd;
ST_SCATLOG_WINDOW CMKLog::st_scatlog_wnd;

ST_LOG_TIME_CHART_CTRL CMKLog::st_tm_chart_ctrl;
ST_LOG_SCAT_CHART_CTRL CMKLog::st_sc_chart_ctrl;


LPST_LOG_HEADER CMKLog::phead[MKLOG_N_ID_TYPE];
LPST_MKLOG_DB CMKLog::pdb;

CMKLog::CMKLog() {
	create_wnd_objects(MKLOG_ID_TYPE_ALL);
	for (int i = 0; i < MKLOG_N_ID_TYPE; i++) {
		logbuf[i].iw = logbuf[i].ir = logidhot[i] = 0;
	}
};	//staticクラスにするのでprivateにする
CMKLog::~CMKLog() {
	delete_wnd_objects(MKLOG_ID_TYPE_ALL);
};	//staticクラスにするのでprivateにする


LRESULT CALLBACK CMKLog::EventLogWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

	HINSTANCE hInst = GetModuleHandle(0);

	int type = MKLOG_ID_TYPE_EVENT;
	switch (message)
	{

	case WM_CREATE: {
		is_log_wnd_active.i16[MKLOG_ID_TYPE_EVENT] = L_ON;
		InitCommonControls();//コモンコントロール初期化
		//ウィンドウにコントロール追加
		}break;
	case WM_COMMAND: {
		int wmId = LOWORD(wParam);
		// 選択されたメニューの解析:
		switch (wmId)
		{
		case MKLOG_ID_WND_CTRL_EVENT + 0:

			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}break;
	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
	}break;
	case WM_DESTROY: {
		is_log_wnd_active.i16[MKLOG_ID_TYPE_EVENT] = L_OFF;
		//PostQuitMessage(0);
	}break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return S_OK;
}

/// <summary>
/// 
/// </summary>
/// <param name="hWnd"></param>
/// <param name="message"></param>
/// <param name="wParam"></param>
/// <param name="lParam"></param>
/// <returns></returns>
 
static int win_pad = 5;//Windowのパディング
static bool check_flg = true;
static bool be_refresh = false;

LRESULT CALLBACK CMKLog::TimeLogWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

	HINSTANCE hInst = GetModuleHandle(0);

	UINT32 type = MKLOG_ID_TYPE_TIME;
	switch (message)
	{
	case WM_CREATE: {
		is_log_wnd_active.i16[MKLOG_ID_TYPE_TIME] = L_ON;

		setup_timelog_wnd(hWnd);
		st_tm_chart_ctrl.icon_draw = L_OFF;
		create_wnd_objects(type);
		init_graphic(type);
		st_tm_chart_ctrl.req_bg_update = L_ON;

		st_tm_chart_ctrl.scan_time_ms = (INT)logbuf[type].header.d100[MKLOG_INDEX_SCAN_MS];
		
		st_tm_chart_ctrl.plot_count_limit = st_tm_chart_ctrl.ms_per_pix / st_tm_chart_ctrl.scan_time_ms;//グラフィックにプロットする時にX軸プロットが前回値ポイントからこの値以下であればプロットしない（設定スキャンタイム以下）
		st_tm_chart_ctrl.plot_xshift_pix = st_tm_chart_ctrl.scan_time_ms / st_tm_chart_ctrl.ms_per_pix;//スキャンタイムでシフトするX軸のPIXEL数
		if (st_tm_chart_ctrl.plot_xshift_pix == 0)st_tm_chart_ctrl.plot_xshift_pix = 1;
		
		//SCADAインスタンスのlogsourceの内容でCHART表示の項目をセット
		init_log_setting(type, L_OFF);
		//X軸のINF表示テキストセット
		//600 PIXEL位置の秒数テキスト表示内容セット
		double sec60pix = (double)(st_tm_chart_ctrl.ms_per_pix) * 600.0 / 1000.0; // s/pix * 600 600PIXEL位置の秒数
		wos.str(L""); wos << std::setprecision(3) << sec60pix << L"s";
		for (int i = 0; i < MKLOG_N_LOG_TITLE_WCH; i++) {
			if (i < wos.str().length()) {
				st_timelog_wnd.tx_obj[ID_MKLOG_ITEM_TMCHART_ST_F1X_MAX][i] = *(wos.str().c_str() + i);
				st_timelog_wnd.tx_obj[ID_MKLOG_ITEM_TMCHART_ST_F2X_MAX][i] = *(wos.str().c_str() + i);
			}
			else {
				st_timelog_wnd.tx_obj[ID_MKLOG_ITEM_TMCHART_ST_F1X_MAX][i] = L'\0';
				st_timelog_wnd.tx_obj[ID_MKLOG_ITEM_TMCHART_ST_F2X_MAX][i] = L'\0';
			}
		};
		//300 PIXEL位置の秒数テキスト表示内容セット
		wos.str(L""); wos << std::setprecision(3) << sec60pix / 2.0 << L"s"; // 300PIXEL位置の秒数
		for (int i = 0; i < MKLOG_N_LOG_TITLE_WCH; i++) {
			if (i < wos.str().length()) {
				st_timelog_wnd.tx_obj[ID_MKLOG_ITEM_TMCHART_ST_F1X_MIDDLE][i] = *(wos.str().c_str() + i);
				st_timelog_wnd.tx_obj[ID_MKLOG_ITEM_TMCHART_ST_F2X_MIDDLE][i] = *(wos.str().c_str() + i);
			}
			else {
				st_timelog_wnd.tx_obj[ID_MKLOG_ITEM_TMCHART_ST_F1X_MIDDLE][i] = L'\0';
				st_timelog_wnd.tx_obj[ID_MKLOG_ITEM_TMCHART_ST_F2X_MIDDLE][i] = L'\0';
			}
		};
		//時間軸目盛り間隔の秒数表示テキストセット
		wos.str(L""); wos << std::setprecision(3) << st_tm_chart_ctrl.ms_per_pix * st_tm_chart_ctrl.division_time_axis / 1000.0 << L"s/div";
		for (int i = 0; i < MKLOG_N_LOG_TITLE_WCH; i++) {
			if (i < wos.str().length()) {
				st_timelog_wnd.tx_obj[ID_MKLOG_ITEM_TMCHART_ST_F1X_MIN][i] = *(wos.str().c_str() + i);
				st_timelog_wnd.tx_obj[ID_MKLOG_ITEM_TMCHART_ST_F2X_MIN][i] = *(wos.str().c_str() + i);
			}
			else {
				st_timelog_wnd.tx_obj[ID_MKLOG_ITEM_TMCHART_ST_F1X_MIN][i] = L'\0';
				st_timelog_wnd.tx_obj[ID_MKLOG_ITEM_TMCHART_ST_F2X_MIN][i] = L'\0';
			}
		};

	}break;
	case WM_TIMER: {
		wos.str(L""); 
		wos << L"ログ収集 INDEX :" << logbuf[type].iw;
		SetWindowText(st_timelog_wnd.hwnd_obj[ID_MKLOG_ITEM_TMCHART_ST_INF], wos.str().c_str());

		set_record(type, logidhot[type]);//ログバッファのレコードに各項目の現在値を書き込み

		logidhot[type]++;

		//グラフィック更新用　ON PAINT　呼び出し　表示更新
		if (st_timelog_wnd.stat_obj[ID_MKLOG_ITEM_TMCHART_PB_CHART]) {//チャート起動中
			//経過時間更新
			st_tm_chart_ctrl.eraps_ms += (UINT)(logbuf[type].header.d100[MKLOG_INDEX_SCAN_MS]);
			
			if (++st_tm_chart_ctrl.plot_counter >= st_tm_chart_ctrl.plot_count_limit) {
				//チャートX軸書込位置更新	pix_time_plot_range：2倍エリア幅
				st_tm_chart_ctrl.pt_time_plot_pix += st_tm_chart_ctrl.plot_xshift_pix;//plot_xshift_pix⇒スキャン毎時間軸移動量
				st_tm_chart_ctrl.plot_counter = 0;

				if (st_tm_chart_ctrl.pt_time_plot_pix > st_tm_chart_ctrl.pix_time_plot_range) {
					st_tm_chart_ctrl.pt_time_plot_pix = 0;
					st_tm_chart_ctrl.is_disp_over_range = L_ON;
				}
				//チャートX軸消込位置更新	pix_disp_range:表示幅　pix_time_plot_range：2倍エリア幅
				//表示幅分先のプロットを消す
				st_tm_chart_ctrl.pt_time_erase_pix = st_tm_chart_ctrl.pt_time_plot_pix + st_tm_chart_ctrl.pix_disp_range;
				if (st_tm_chart_ctrl.pt_time_erase_pix > st_tm_chart_ctrl.pix_time_plot_range) st_tm_chart_ctrl.pt_time_erase_pix -= st_tm_chart_ctrl.pix_disp_range;
			}

			//CHART表示エリア更新
			draw_graphic(MKLOG_ID_TYPE_TIME, false);
			draw_info(MKLOG_ID_TYPE_TIME);

			if (st_tm_chart_ctrl.pt_time_plot_pix < st_tm_chart_ctrl.pix_disp_range) {//時間軸プロット位置が分割２画面の前半部分
				if (st_tm_chart_ctrl.is_disp_over_range) {//既に１周している時は、前半部の開始地点はプロット２画面目の後半部分　後半部はプロット１画面目の最初から
					//前半グラフ
					st_tm_chart_ctrl.area[ID_MKLOG_TM_CHART_AREA1_1].x = st_tm_chart_ctrl.area[ID_MKLOG_TM_CHART_AREA2_1].x = st_tm_chart_ctrl.pix_disp_range + st_tm_chart_ctrl.pt_time_plot_pix;
					st_tm_chart_ctrl.area[ID_MKLOG_TM_CHART_AREA1_1].w = st_tm_chart_ctrl.area[ID_MKLOG_TM_CHART_AREA2_1].w = st_tm_chart_ctrl.pix_disp_range - st_tm_chart_ctrl.pt_time_plot_pix;
					//後半グラフ
					st_tm_chart_ctrl.area[ID_MKLOG_TM_CHART_AREA1_2].x = st_tm_chart_ctrl.area[ID_MKLOG_TM_CHART_AREA2_2].x = 0;
					st_tm_chart_ctrl.area[ID_MKLOG_TM_CHART_AREA1_2].w = st_tm_chart_ctrl.area[ID_MKLOG_TM_CHART_AREA2_2].w = st_tm_chart_ctrl.pt_time_plot_pix;
				}
				else {//1周目では、プロット前半部分のみのコピー
					//前半グラフ
					st_tm_chart_ctrl.area[ID_MKLOG_TM_CHART_AREA1_1].x = st_tm_chart_ctrl.area[ID_MKLOG_TM_CHART_AREA2_1].x = 0;
					st_tm_chart_ctrl.area[ID_MKLOG_TM_CHART_AREA1_1].w = st_tm_chart_ctrl.area[ID_MKLOG_TM_CHART_AREA2_1].w = st_tm_chart_ctrl.pix_disp_range;
					//後半グラフ
					st_tm_chart_ctrl.area[ID_MKLOG_TM_CHART_AREA1_2].x = st_tm_chart_ctrl.area[ID_MKLOG_TM_CHART_AREA2_2].x = st_tm_chart_ctrl.pix_disp_range;
					st_tm_chart_ctrl.area[ID_MKLOG_TM_CHART_AREA1_2].w = st_tm_chart_ctrl.area[ID_MKLOG_TM_CHART_AREA2_2].w = 0;
				}
			}
			else {//時間軸プロット位置が分割２画面の後半部分の時は、前半部分のみのコピーでOK
				//前半グラフ
				st_tm_chart_ctrl.area[ID_MKLOG_TM_CHART_AREA1_1].x = st_tm_chart_ctrl.area[ID_MKLOG_TM_CHART_AREA2_1].x = st_tm_chart_ctrl.pt_time_plot_pix - st_tm_chart_ctrl.pix_disp_range;
				st_tm_chart_ctrl.area[ID_MKLOG_TM_CHART_AREA1_1].w = st_tm_chart_ctrl.area[ID_MKLOG_TM_CHART_AREA2_1].w = st_tm_chart_ctrl.pix_disp_range;
				//後半グラフ
				st_tm_chart_ctrl.area[ID_MKLOG_TM_CHART_AREA1_2].x = st_tm_chart_ctrl.area[ID_MKLOG_TM_CHART_AREA2_2].x = st_tm_chart_ctrl.pt_time_plot_pix;
				st_tm_chart_ctrl.area[ID_MKLOG_TM_CHART_AREA1_2].w = st_tm_chart_ctrl.area[ID_MKLOG_TM_CHART_AREA2_2].w = 0;
			}

			//WM_PAINT　画面更新
			InvalidateRect(hWnd, &st_timelog_wnd.rc[ID_MKLOG_ITEM_TMCHART_GRAPH1], FALSE);
			InvalidateRect(hWnd, &st_timelog_wnd.rc[ID_MKLOG_ITEM_TMCHART_GRAPH2], FALSE);
		}


	}break;
	case WM_COMMAND: {
		int wmId = LOWORD(wParam);
		// 選択されたメニューの解析:
		switch (wmId)
		{
		case MKLOG_ID_WND_CTRL_TIME + ID_MKLOG_ITEM_TMCHART_CB_ACT: {		//ログ記録開始/停止
			if (BST_CHECKED == SendMessage(st_timelog_wnd.hwnd_obj[ID_MKLOG_ITEM_TMCHART_CB_ACT], BM_GETCHECK, 0, 0)) {

				st_tm_chart_ctrl.scan_time_ms = (UINT)(logbuf[type].header.d100[MKLOG_INDEX_SCAN_MS]);
				SetTimer(hWnd, ID_LOG_TIMER_TIME, st_tm_chart_ctrl.scan_time_ms, NULL);

				st_work_wnd.is_timer_active[type] = L_ON;
				logidhot[type] = 0;
				SetWindowText(st_timelog_wnd.hwnd_obj[ID_MKLOG_ITEM_TMCHART_ST_INF], L"ログ起動");
				logbuf[type].iw = 0;
				for (int i = 0; i < MKLOG_N_LOG_RECORD; i++)logbuf[type].records[i].id = 0;
				set_logstatus(type, (get_logstatus(type) | MKLOG_CODE_LOG_RECORDING));
				st_tm_chart_ctrl.req_bg_update = L_ON;
			}
			else {
				KillTimer(hWnd, ID_LOG_TIMER_TIME);
				st_work_wnd.is_timer_active[type] = L_OFF;

				wos.str(L"");
				wos << L"ログ停止 :" << logbuf[type].iw;
				SetWindowText(st_timelog_wnd.hwnd_obj[ID_MKLOG_ITEM_TMCHART_ST_INF], wos.str().c_str());

				set_logstatus(type, (get_logstatus(type) & ~MKLOG_CODE_LOG_RECORDING));
			}
		}break;
		case MKLOG_ID_WND_CTRL_TIME + ID_MKLOG_ITEM_TMCHART_PB_CHART: {		//ログチャート表示開始/停止

			if (st_tm_chart_ctrl.icon_draw != L_ON) {
				draw_icon(GetDC(hWnd), MKLOG_ID_TYPE_TIME);
			}

			if (get_logstatus(type) & MKLOG_CODE_LOG_CHART_ACTIVE) {
				set_logstatus(type, (get_logstatus(type) & ~MKLOG_CODE_LOG_CHART_ACTIVE));
				
				st_timelog_wnd.stat_obj[ID_MKLOG_ITEM_TMCHART_PB_CHART] = BST_UNCHECKED;
				SetWindowText(st_timelog_wnd.hwnd_obj[ID_MKLOG_ITEM_TMCHART_ST_INF], L"チャートを停止します");
				st_tm_chart_ctrl.req_graph_update = L_OFF;

				KillTimer(hWnd, ID_LOG_TIMER_TIME);
				st_work_wnd.is_timer_active[type] = L_OFF;

				st_tm_chart_ctrl.is_disp_over_range = L_OFF;

			}
			else {
				init_log_setting(type, L_OFF);
				st_timelog_wnd.stat_obj[ID_MKLOG_ITEM_TMCHART_PB_CHART] = BST_CHECKED;
				set_logstatus(type, (get_logstatus(type) | MKLOG_CODE_LOG_CHART_ACTIVE));
				SetWindowText(st_timelog_wnd.hwnd_obj[ID_MKLOG_ITEM_TMCHART_ST_INF], L"チャートを表示します");
				st_tm_chart_ctrl.req_graph_update = L_ON;

				if (st_work_wnd.is_timer_active[type] == L_OFF) {
					st_tm_chart_ctrl.scan_time_ms = (UINT)(logbuf[type].header.d100[MKLOG_INDEX_SCAN_MS]);
					
					SetTimer(hWnd, ID_LOG_TIMER_TIME, st_tm_chart_ctrl.scan_time_ms, NULL);
					st_work_wnd.is_timer_active[type] = L_ON;

					st_tm_chart_ctrl.req_bg_update = L_ON;
				}

				st_timelog_wnd.stat_obj[ID_MKLOG_ITEM_TMCHART_PB_PAUSE] = L_OFF;

				SetWindowText(st_timelog_wnd.hwnd_obj[ID_MKLOG_ITEM_TMCHART_PB_PAUSE], L"PAUSE");

				//プロットするコードの取り込み
				wchar_t buffer[256];
				for (int i = ID_MKLOG_ITEM_TMCHART_ED_F1CODE1, k = 0; i <= ID_MKLOG_ITEM_TMCHART_ED_F2CODE8; i++, k++) {
					GetWindowText(st_timelog_wnd.hwnd_obj[i], buffer, 256);
					std::wistringstream iss(buffer);
					iss >> std::dec >> st_tm_chart_ctrl.plot_item_code[k];
				}

				for (int i = ID_MKLOG_HDC_MEM0; i <= ID_MKLOG_GR_MEM_INFO; i++) {
					PatBlt(st_timelog_wnd.hdc[i], 0, 0, st_timelog_wnd.size_obj[ID_MKLOG_ITEM_TMCHART_MEM_GRAPH1].cx, st_timelog_wnd.size_obj[ID_MKLOG_ITEM_TMCHART_MEM_GRAPH1].cy * 2, WHITENESS);
					PatBlt(st_timelog_wnd.hdc[i], 0, 0, st_timelog_wnd.size_obj[ID_MKLOG_ITEM_TMCHART_MEM_GRAPH2].cx, st_timelog_wnd.size_obj[ID_MKLOG_ITEM_TMCHART_MEM_GRAPH2].cy * 2, WHITENESS);

				}
				clear_record(type);
				st_tm_chart_ctrl.pt_time_plot_pix = st_tm_chart_ctrl.plot_counter = st_tm_chart_ctrl.eraps_ms = 0;
				draw_graphic(MKLOG_ID_TYPE_TIME, true);//描画ポイントリセット

			}
		}break;
		case MKLOG_ID_WND_CTRL_TIME + ID_MKLOG_ITEM_TMCHART_PB_REFRESH: {			//REFRESH

			st_tm_chart_ctrl.is_disp_over_range = L_OFF;

			for (int i = ID_MKLOG_HDC_MEM0; i <= ID_MKLOG_GR_MEM_INFO; i++) {
				PatBlt(st_timelog_wnd.hdc[i], 0, 0, st_timelog_wnd.size_obj[ID_MKLOG_ITEM_TMCHART_MEM_GRAPH1].cx, st_timelog_wnd.size_obj[ID_MKLOG_ITEM_TMCHART_MEM_GRAPH1].cy * 2, WHITENESS);
				PatBlt(st_timelog_wnd.hdc[i], 0, 0, st_timelog_wnd.size_obj[ID_MKLOG_ITEM_TMCHART_MEM_GRAPH2].cx, st_timelog_wnd.size_obj[ID_MKLOG_ITEM_TMCHART_MEM_GRAPH2].cy * 2, WHITENESS);
			}
			st_tm_chart_ctrl.pt_time_plot_pix = st_tm_chart_ctrl.pt_time_plot_pix_last = st_tm_chart_ctrl.plot_counter = st_tm_chart_ctrl.eraps_ms = 0;
			draw_graphic(MKLOG_ID_TYPE_TIME, true);//描画ポイントリセット
			draw_bg(MKLOG_ID_TYPE_TIME);


			//LOG SOURCE更新
			init_log_setting(type, L_ON);
			SetWindowText(st_timelog_wnd.hwnd_obj[ID_MKLOG_ITEM_TMCHART_ST_INF], L"チャート設定を更新しました");
			clear_record(type);


		}break;
		case MKLOG_ID_WND_CTRL_TIME + ID_MKLOG_ITEM_TMCHART_PB_PAUSE: {			//PAUSE/RESTART
	
			if (st_timelog_wnd.stat_obj[ID_MKLOG_ITEM_TMCHART_PB_PAUSE] == L_OFF) {

				st_timelog_wnd.stat_obj[ID_MKLOG_ITEM_TMCHART_PB_PAUSE] = L_ON;

				KillTimer(hWnd, ID_LOG_TIMER_TIME);
				st_work_wnd.is_timer_active[type] = L_OFF;

				SetWindowText(st_timelog_wnd.hwnd_obj[ID_MKLOG_ITEM_TMCHART_PB_PAUSE], L"RESTART");
			}
			else {
				if ((st_timelog_wnd.stat_obj[ID_MKLOG_ITEM_TMCHART_PB_PAUSE] == L_ON) && (st_timelog_wnd.stat_obj[ID_MKLOG_ITEM_TMCHART_PB_CHART])) {
					st_timelog_wnd.stat_obj[ID_MKLOG_ITEM_TMCHART_PB_PAUSE] = L_OFF;
	
					SetTimer(hWnd, ID_LOG_TIMER_TIME, (UINT)(logbuf[type].header.d100[MKLOG_INDEX_SCAN_MS]), NULL);
					st_work_wnd.is_timer_active[type] = L_ON;
					SetWindowText(st_timelog_wnd.hwnd_obj[ID_MKLOG_ITEM_TMCHART_PB_PAUSE], L"PAUSE");
				}
			}
		}break;
		case MKLOG_ID_WND_CTRL_TIME + ID_MKLOG_ITEM_TMCHART_PB_BMP: {			//RESTART

		}break;
		case MKLOG_ID_WND_CTRL_TIME + ID_MKLOG_ITEM_TMCHART_PB_FILE: {			//ログファイル書き出し
			if (get_logstatus(type) & MKLOG_CODE_LOG_FILE_OUT_ACTIVE) {
				set_logstatus(type, (get_logstatus(type) & ~MKLOG_CODE_LOG_FILE_OUT_ACTIVE));
				SetWindowText(st_timelog_wnd.hwnd_obj[ID_MKLOG_ITEM_TMCHART_ST_INF], L"ログを出力しました");
			}
			else {
				set_logstatus(type, (get_logstatus(type) | MKLOG_CODE_LOG_FILE_OUT_ACTIVE));
				unsigned thrdID;
				HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, CMKLog::logfileout, &type, 0, &thrdID);
			}
		}break;
		case MKLOG_ID_WND_CTRL_TIME + ID_MKLOG_ITEM_TMCHART_PB_LIST: {			//ログファイル書き出し
			if (st_tm_chart_ctrl.chart_item_txt_mode == ID_MKLOG_ITEM_TXT_NAME) {
				st_tm_chart_ctrl.chart_item_txt_mode = ID_MKLOG_ITEM_TXT_100;
				SetWindowText(st_timelog_wnd.hwnd_obj[ID_MKLOG_ITEM_TMCHART_PB_LIST], L"NAME");
			}
			else {
				st_tm_chart_ctrl.chart_item_txt_mode = ID_MKLOG_ITEM_TXT_NAME;
				SetWindowText(st_timelog_wnd.hwnd_obj[ID_MKLOG_ITEM_TMCHART_PB_LIST], L"100%");
			}
			init_log_setting(type, L_OFF);
		}break;
		case MKLOG_ID_WND_CTRL_TIME + ID_MKLOG_ITEM_TMCHART_PB_Xd2: 
		case MKLOG_ID_WND_CTRL_TIME + ID_MKLOG_ITEM_TMCHART_PB_Xx2:
		{
			if (wmId == MKLOG_ID_WND_CTRL_TIME + ID_MKLOG_ITEM_TMCHART_PB_Xx2) {
				st_tm_chart_ctrl.ms_per_pix /= 2;
				if (st_tm_chart_ctrl.ms_per_pix < 25)st_tm_chart_ctrl.ms_per_pix = 25;
				st_tm_chart_ctrl.plot_count_limit = st_tm_chart_ctrl.ms_per_pix / st_tm_chart_ctrl.scan_time_ms;
				st_tm_chart_ctrl.plot_xshift_pix = st_tm_chart_ctrl.scan_time_ms / st_tm_chart_ctrl.ms_per_pix;
				if (st_tm_chart_ctrl.plot_xshift_pix == 0)st_tm_chart_ctrl.plot_xshift_pix = 1;
			}
			else {
				st_tm_chart_ctrl.ms_per_pix *= 2;
				if (st_tm_chart_ctrl.ms_per_pix > 3600000)st_tm_chart_ctrl.ms_per_pix = 3600000;
				st_tm_chart_ctrl.plot_count_limit = st_tm_chart_ctrl.ms_per_pix / st_tm_chart_ctrl.scan_time_ms;
				st_tm_chart_ctrl.plot_xshift_pix = st_tm_chart_ctrl.scan_time_ms / st_tm_chart_ctrl.ms_per_pix;
				if (st_tm_chart_ctrl.plot_xshift_pix == 0)st_tm_chart_ctrl.plot_xshift_pix = 1;

			}
			double sec60pix = (double)(st_tm_chart_ctrl.ms_per_pix) *600.0/ 1000.0;
			wos.str(L""); wos << std::setprecision(3) << sec60pix << L"s"; 

			for (int i = 0; i < MKLOG_N_LOG_TITLE_WCH; i++) {
				if (i < wos.str().length()) {
					st_timelog_wnd.tx_obj[ID_MKLOG_ITEM_TMCHART_ST_F1X_MAX][i] = *(wos.str().c_str() + i);
					st_timelog_wnd.tx_obj[ID_MKLOG_ITEM_TMCHART_ST_F2X_MAX][i] = *(wos.str().c_str() + i);
				}
				else {
					st_timelog_wnd.tx_obj[ID_MKLOG_ITEM_TMCHART_ST_F1X_MAX][i] = L'\0';
					st_timelog_wnd.tx_obj[ID_MKLOG_ITEM_TMCHART_ST_F2X_MAX][i] = L'\0';
				}
			};

			wos.str(L""); wos << std::setprecision(3) << sec60pix/2.0 << L"s";
			for (int i = 0; i < MKLOG_N_LOG_TITLE_WCH; i++) {
				if (i < wos.str().length()) {
					st_timelog_wnd.tx_obj[ID_MKLOG_ITEM_TMCHART_ST_F1X_MIDDLE][i] = *(wos.str().c_str() + i);
					st_timelog_wnd.tx_obj[ID_MKLOG_ITEM_TMCHART_ST_F2X_MIDDLE][i] = *(wos.str().c_str() + i);
				}
				else {
					st_timelog_wnd.tx_obj[ID_MKLOG_ITEM_TMCHART_ST_F1X_MIDDLE][i] = L'\0';
					st_timelog_wnd.tx_obj[ID_MKLOG_ITEM_TMCHART_ST_F2X_MIDDLE][i] = L'\0';
				}
			};

	
			wos.str(L""); wos << std::setprecision(3) << st_tm_chart_ctrl.ms_per_pix * st_tm_chart_ctrl.division_time_axis / 1000.0 << L"s/div";

			for (int i = 0; i < MKLOG_N_LOG_TITLE_WCH; i++) {
				if (i < wos.str().length()) {
					st_timelog_wnd.tx_obj[ID_MKLOG_ITEM_TMCHART_ST_F1X_MIN][i] = *(wos.str().c_str() + i);
					st_timelog_wnd.tx_obj[ID_MKLOG_ITEM_TMCHART_ST_F2X_MIN][i] = *(wos.str().c_str() + i);
				}
				else {
					st_timelog_wnd.tx_obj[ID_MKLOG_ITEM_TMCHART_ST_F1X_MIN][i] = L'\0';
					st_timelog_wnd.tx_obj[ID_MKLOG_ITEM_TMCHART_ST_F2X_MIN][i] = L'\0';
				}
			};

		}break;

		case MKLOG_ID_WND_CTRL_TIME + ID_MKLOG_ITEM_TMCHART_PB_F1Yx1 :
		case MKLOG_ID_WND_CTRL_TIME + ID_MKLOG_ITEM_TMCHART_PB_F1Yx2:
		case MKLOG_ID_WND_CTRL_TIME + ID_MKLOG_ITEM_TMCHART_PB_F1Yx4:
		case MKLOG_ID_WND_CTRL_TIME + ID_MKLOG_ITEM_TMCHART_PB_F1YP10:
		case MKLOG_ID_WND_CTRL_TIME + ID_MKLOG_ITEM_TMCHART_PB_F1YN10:
		{
			switch (wmId) {
			case MKLOG_ID_WND_CTRL_TIME + ID_MKLOG_ITEM_TMCHART_PB_F1Yx1:
				st_tm_chart_ctrl.disp_mag_yc[ID_MKLOG_PLOT_GRAPH1_A] = 100; break;
			case MKLOG_ID_WND_CTRL_TIME + ID_MKLOG_ITEM_TMCHART_PB_F1Yx2:
				st_tm_chart_ctrl.disp_mag_yc[ID_MKLOG_PLOT_GRAPH1_A] = 200; break;
			case MKLOG_ID_WND_CTRL_TIME + ID_MKLOG_ITEM_TMCHART_PB_F1Yx4:
				st_tm_chart_ctrl.disp_mag_yc[ID_MKLOG_PLOT_GRAPH1_A] = 400; break;
			case MKLOG_ID_WND_CTRL_TIME + ID_MKLOG_ITEM_TMCHART_PB_F1YP10:
				st_tm_chart_ctrl.disp_offset_y[ID_MKLOG_PLOT_GRAPH1_A] += 10;
				if (st_tm_chart_ctrl.disp_offset_y[ID_MKLOG_PLOT_GRAPH1_A] > 100)st_tm_chart_ctrl.disp_offset_y[ID_MKLOG_PLOT_GRAPH1_A] = 100;
				break;
			case MKLOG_ID_WND_CTRL_TIME + ID_MKLOG_ITEM_TMCHART_PB_F1YN10:
				st_tm_chart_ctrl.disp_offset_y[ID_MKLOG_PLOT_GRAPH1_A] -= 10;
				if (st_tm_chart_ctrl.disp_offset_y[ID_MKLOG_PLOT_GRAPH1_A] < -100)st_tm_chart_ctrl.disp_offset_y[ID_MKLOG_PLOT_GRAPH1_A] = -100;
				break;
			}
			INT ymiddle_c = -st_tm_chart_ctrl.disp_offset_y[ID_MKLOG_PLOT_GRAPH1_A] * st_tm_chart_ctrl.disp_mag_yc[ID_MKLOG_PLOT_GRAPH1_A] / 100;
			INT ymax_c = st_tm_chart_ctrl.disp_mag_yc[ID_MKLOG_PLOT_GRAPH1_A] + ymiddle_c;
			INT ymin_c = ymiddle_c - (ymax_c - ymiddle_c);

			wos.str(L""); wos <<  ymax_c << L"%";
			for (int i = 0; i < MKLOG_N_LOG_TITLE_WCH; i++) {
				if (i < wos.str().length()) 
					st_timelog_wnd.tx_obj[ID_MKLOG_ITEM_TMCHART_ST_F1Y_MAX][i] = *(wos.str().c_str() + i);
				else 
					st_timelog_wnd.tx_obj[ID_MKLOG_ITEM_TMCHART_ST_F1Y_MAX][i] = L'\0';
			};

			wos.str(L""); wos << ymiddle_c << L"%";
			for (int i = 0; i < MKLOG_N_LOG_TITLE_WCH; i++) {
				if (i < wos.str().length())
					st_timelog_wnd.tx_obj[ID_MKLOG_ITEM_TMCHART_ST_F1Y_MIDDLE][i] = *(wos.str().c_str() + i);
				else
					st_timelog_wnd.tx_obj[ID_MKLOG_ITEM_TMCHART_ST_F1Y_MIDDLE][i] = L'\0';
			};

			wos.str(L""); wos << ymin_c << L"%";
			for (int i = 0; i < MKLOG_N_LOG_TITLE_WCH; i++) {
				if (i < wos.str().length())
					st_timelog_wnd.tx_obj[ID_MKLOG_ITEM_TMCHART_ST_F1Y_MIN][i] = *(wos.str().c_str() + i);
				else
					st_timelog_wnd.tx_obj[ID_MKLOG_ITEM_TMCHART_ST_F1Y_MIN][i] = L'\0';
			};

		}break;

		case MKLOG_ID_WND_CTRL_TIME + ID_MKLOG_ITEM_TMCHART_PB_F2Yx1:
		case MKLOG_ID_WND_CTRL_TIME + ID_MKLOG_ITEM_TMCHART_PB_F2Yx2:
		case MKLOG_ID_WND_CTRL_TIME + ID_MKLOG_ITEM_TMCHART_PB_F2Yx4:
		case MKLOG_ID_WND_CTRL_TIME + ID_MKLOG_ITEM_TMCHART_PB_F2YP10:
		case MKLOG_ID_WND_CTRL_TIME + ID_MKLOG_ITEM_TMCHART_PB_F2YN10:
		{
			switch (wmId) {
			case MKLOG_ID_WND_CTRL_TIME + ID_MKLOG_ITEM_TMCHART_PB_F2Yx1:
				st_tm_chart_ctrl.disp_mag_yc[ID_MKLOG_PLOT_GRAPH2_A] = 100; break;
			case MKLOG_ID_WND_CTRL_TIME + ID_MKLOG_ITEM_TMCHART_PB_F2Yx2:
				st_tm_chart_ctrl.disp_mag_yc[ID_MKLOG_PLOT_GRAPH2_A] = 200; break;
			case MKLOG_ID_WND_CTRL_TIME + ID_MKLOG_ITEM_TMCHART_PB_F2Yx4:
				st_tm_chart_ctrl.disp_mag_yc[ID_MKLOG_PLOT_GRAPH2_A] = 400; break;
			case MKLOG_ID_WND_CTRL_TIME + ID_MKLOG_ITEM_TMCHART_PB_F2YP10:
				st_tm_chart_ctrl.disp_offset_y[ID_MKLOG_PLOT_GRAPH2_A] += 10;
				if (st_tm_chart_ctrl.disp_offset_y[ID_MKLOG_PLOT_GRAPH2_A] > 100)st_tm_chart_ctrl.disp_offset_y[ID_MKLOG_PLOT_GRAPH2_A] = 100;
				break;
			case MKLOG_ID_WND_CTRL_TIME + ID_MKLOG_ITEM_TMCHART_PB_F2YN10:
				st_tm_chart_ctrl.disp_offset_y[ID_MKLOG_PLOT_GRAPH2_A] -= 10;
				if (st_tm_chart_ctrl.disp_offset_y[ID_MKLOG_PLOT_GRAPH2_A] < -100)st_tm_chart_ctrl.disp_offset_y[ID_MKLOG_PLOT_GRAPH2_A] = -100;
				break;
			}

			INT ymiddle_c = -st_tm_chart_ctrl.disp_offset_y[ID_MKLOG_PLOT_GRAPH2_A] * st_tm_chart_ctrl.disp_mag_yc[ID_MKLOG_PLOT_GRAPH2_A] / 100;
			INT ymax_c = st_tm_chart_ctrl.disp_mag_yc[ID_MKLOG_PLOT_GRAPH2_A] + ymiddle_c;
			INT ymin_c = ymiddle_c - (ymax_c - ymiddle_c);

			wos.str(L""); wos << ymax_c << L"%";
			for (int i = 0; i < MKLOG_N_LOG_TITLE_WCH; i++) {
				if (i < wos.str().length())
					st_timelog_wnd.tx_obj[ID_MKLOG_ITEM_TMCHART_ST_F2Y_MAX][i] = *(wos.str().c_str() + i);
				else
					st_timelog_wnd.tx_obj[ID_MKLOG_ITEM_TMCHART_ST_F2Y_MAX][i] = L'\0';
			};

			wos.str(L""); wos << ymiddle_c << L"%";
			for (int i = 0; i < MKLOG_N_LOG_TITLE_WCH; i++) {
				if (i < wos.str().length())
					st_timelog_wnd.tx_obj[ID_MKLOG_ITEM_TMCHART_ST_F2Y_MIDDLE][i] = *(wos.str().c_str() + i);
				else
					st_timelog_wnd.tx_obj[ID_MKLOG_ITEM_TMCHART_ST_F2Y_MIDDLE][i] = L'\0';
			};

			wos.str(L""); wos << ymin_c << L"%";
			for (int i = 0; i < MKLOG_N_LOG_TITLE_WCH; i++) {
				if (i < wos.str().length())
					st_timelog_wnd.tx_obj[ID_MKLOG_ITEM_TMCHART_ST_F2Y_MIN][i] = *(wos.str().c_str() + i);
				else
					st_timelog_wnd.tx_obj[ID_MKLOG_ITEM_TMCHART_ST_F2Y_MIN][i] = L'\0';
			};
		}break;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}break;
	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);

		if (st_tm_chart_ctrl.req_bg_update) {		//背景
			draw_bg(MKLOG_ID_TYPE_TIME);

			BitBlt(
				hdc, st_timelog_wnd.pt_obj[ID_MKLOG_ITEM_TMCHART_GRAPH1].x, st_timelog_wnd.pt_obj[ID_MKLOG_ITEM_TMCHART_GRAPH1].y, st_timelog_wnd.size_obj[ID_MKLOG_ITEM_TMCHART_GRAPH1].cx, st_timelog_wnd.size_obj[ID_MKLOG_ITEM_TMCHART_GRAPH1].cy,
				st_timelog_wnd.hdc[ID_MKLOG_HDC_BG], st_tm_chart_ctrl.area[ID_MKLOG_TM_GR_AREA1].x, st_tm_chart_ctrl.area[ID_MKLOG_TM_GR_AREA1].y,
				SRCCOPY
			);

			BitBlt(
				hdc, st_timelog_wnd.pt_obj[ID_MKLOG_ITEM_TMCHART_GRAPH2].x, st_timelog_wnd.pt_obj[ID_MKLOG_ITEM_TMCHART_GRAPH2].y, st_timelog_wnd.size_obj[ID_MKLOG_ITEM_TMCHART_GRAPH2].cx, st_timelog_wnd.size_obj[ID_MKLOG_ITEM_TMCHART_GRAPH2].cy,
				st_timelog_wnd.hdc[ID_MKLOG_HDC_BG], st_tm_chart_ctrl.area[ID_MKLOG_TM_GR_AREA2].x, st_tm_chart_ctrl.area[ID_MKLOG_TM_GR_AREA2].y,
				SRCCOPY
			);
		}

		if (st_tm_chart_ctrl.req_graph_update) {

			draw_bg(MKLOG_ID_TYPE_TIME);

			// MAIN DC に　INFO DC MEMの内容をコピー
			//CHART INFO
			draw_info(MKLOG_ID_TYPE_TIME);
	
			INT y1_offset = st_tm_chart_ctrl.disp_offset_y[ID_MKLOG_PLOT_GRAPH1_A];
			INT y2_offset = st_tm_chart_ctrl.disp_offset_y[ID_MKLOG_PLOT_GRAPH2_A];

		// INFO DC MEMにBG DC MEMの内容をコピー
			BitBlt(
				st_timelog_wnd.hdc[ID_MKLOG_HDC_MEM_INFO], st_tm_chart_ctrl.area[ID_MKLOG_TM_GR_AREA1].x + MKLOG_TM_DISP_X_MARGINE, st_tm_chart_ctrl.area[ID_MKLOG_TM_GR_AREA1].y, st_tm_chart_ctrl.area[ID_MKLOG_TM_GR_AREA1].w, st_timelog_wnd.size_obj[ID_MKLOG_ITEM_TMCHART_GRAPH1].cy,
				st_timelog_wnd.hdc[ID_MKLOG_HDC_BG], st_tm_chart_ctrl.area[ID_MKLOG_TM_GR_AREA1].x, st_tm_chart_ctrl.area[ID_MKLOG_TM_GR_AREA1].y + y1_offset,
				SRCCOPY
			);
			BitBlt(
				st_timelog_wnd.hdc[ID_MKLOG_HDC_MEM_INFO], st_tm_chart_ctrl.area[ID_MKLOG_TM_GR_AREA2].x + MKLOG_TM_DISP_X_MARGINE, st_tm_chart_ctrl.area[ID_MKLOG_TM_GR_AREA2].y , st_tm_chart_ctrl.area[ID_MKLOG_TM_GR_AREA2].w, st_timelog_wnd.size_obj[ID_MKLOG_ITEM_TMCHART_GRAPH1].cy,
				st_timelog_wnd.hdc[ID_MKLOG_HDC_BG], st_tm_chart_ctrl.area[ID_MKLOG_TM_GR_AREA2].x, st_tm_chart_ctrl.area[ID_MKLOG_TM_GR_AREA2].y + y2_offset,
				SRCCOPY
			);
			// INFO MEMにGRAPHIC DC MEMの内容をコピー

			INT y10 = st_tm_chart_ctrl.mem_offset_y[ID_MKLOG_PLOT_GRAPH1_A] - st_timelog_wnd.size_obj[ID_MKLOG_ITEM_TMCHART_GRAPH1].cy / 2;
			INT y20 = st_tm_chart_ctrl.mem_offset_y[ID_MKLOG_PLOT_GRAPH2_A] - st_timelog_wnd.size_obj[ID_MKLOG_ITEM_TMCHART_GRAPH2].cy / 2;

			INT x, y,y_onof, w, h, xd, yd, wd, hd;

			//CHART
			x = st_tm_chart_ctrl.area[ID_MKLOG_TM_CHART_AREA1_1].x; 
			y = y10 + y1_offset; 
			w = st_tm_chart_ctrl.area[ID_MKLOG_TM_CHART_AREA1_1].w; 
			h = st_timelog_wnd.size_obj[ID_MKLOG_ITEM_TMCHART_GRAPH1].cy;
			y_onof = st_tm_chart_ctrl.mem_offset_y[ID_MKLOG_PLOT_GRAPH1_D] - h;

			xd = st_timelog_wnd.pt_obj[ID_MKLOG_ITEM_TMCHART_GRAPH1].x; yd = y10; wd = w; hd = h;
			//ANALOG		
			TransparentBlt(st_timelog_wnd.hdc[ID_MKLOG_HDC_MEM_INFO], xd, yd, wd, hd, st_timelog_wnd.hdc[ID_MKLOG_HDC_MEM_GR], x, y, w, h,	RGB(255, 255, 255));
			//BIT
			TransparentBlt(st_timelog_wnd.hdc[ID_MKLOG_HDC_MEM_INFO], xd, yd, wd, hd, st_timelog_wnd.hdc[ID_MKLOG_HDC_MEM_GR], x, y_onof, w, h, RGB(255, 255, 255));

			x = st_tm_chart_ctrl.area[ID_MKLOG_TM_CHART_AREA1_2].x;																w = st_tm_chart_ctrl.area[ID_MKLOG_TM_CHART_AREA1_2].w;
			xd = st_timelog_wnd.pt_obj[ID_MKLOG_ITEM_TMCHART_GRAPH1].x + st_tm_chart_ctrl.area[ID_MKLOG_TM_CHART_AREA1_1].w;	wd = w;
			
			//ANALOG
			TransparentBlt(st_timelog_wnd.hdc[ID_MKLOG_HDC_MEM_INFO], xd, yd, wd, hd,st_timelog_wnd.hdc[ID_MKLOG_HDC_MEM_GR], x, y, w, h,RGB(255, 255, 255));
			//BIT
			TransparentBlt(st_timelog_wnd.hdc[ID_MKLOG_HDC_MEM_INFO], xd, yd, wd, hd, st_timelog_wnd.hdc[ID_MKLOG_HDC_MEM_GR], x, y_onof, w, h, RGB(255, 255, 255));

			x = st_tm_chart_ctrl.area[ID_MKLOG_TM_CHART_AREA2_1].x; 
			y = y20 + y2_offset; w = st_tm_chart_ctrl.area[ID_MKLOG_TM_CHART_AREA2_1].w; 
			h = st_timelog_wnd.size_obj[ID_MKLOG_ITEM_TMCHART_GRAPH2].cy;
			y_onof = st_tm_chart_ctrl.mem_offset_y[ID_MKLOG_PLOT_GRAPH2_D] - h;

			xd = st_timelog_wnd.pt_obj[ID_MKLOG_ITEM_TMCHART_GRAPH2].x; yd = y20; wd = w; hd = h;


			TransparentBlt(st_timelog_wnd.hdc[ID_MKLOG_HDC_MEM_INFO], xd, yd, wd, hd, st_timelog_wnd.hdc[ID_MKLOG_HDC_MEM_GR], x, y, w, h, RGB(255, 255, 255));
			//BIT
			TransparentBlt(st_timelog_wnd.hdc[ID_MKLOG_HDC_MEM_INFO], xd, yd, wd, hd, st_timelog_wnd.hdc[ID_MKLOG_HDC_MEM_GR], x, y_onof, w, h, RGB(255, 255, 255));

			x = st_tm_chart_ctrl.area[ID_MKLOG_TM_CHART_AREA2_2].x; w = st_tm_chart_ctrl.area[ID_MKLOG_TM_CHART_AREA2_2].w;
			xd = st_timelog_wnd.pt_obj[ID_MKLOG_ITEM_TMCHART_GRAPH2].x + st_tm_chart_ctrl.area[ID_MKLOG_TM_CHART_AREA2_1].w; wd = w;

			TransparentBlt(st_timelog_wnd.hdc[ID_MKLOG_HDC_MEM_INFO], xd, yd, wd, hd, st_timelog_wnd.hdc[ID_MKLOG_HDC_MEM_GR], x, y, w, h, RGB(255, 255, 255));
			//BIT
			TransparentBlt(st_timelog_wnd.hdc[ID_MKLOG_HDC_MEM_INFO], xd, yd, wd, hd, st_timelog_wnd.hdc[ID_MKLOG_HDC_MEM_GR], x, y_onof, w, h, RGB(255, 255, 255));

			xd = 0;
			yd = st_timelog_wnd.pt_obj[ID_MKLOG_ITEM_TMCHART_GRAPH1].y;
			wd = st_timelog_wnd.size_obj[ID_MKLOG_ITEM_TMCHART_GRAPH1].cx + MKLOG_TM_DISP_X_MARGINE;
			hd = st_timelog_wnd.size_obj[ID_MKLOG_ITEM_TMCHART_GRAPH1].cy + MKLOG_TM_DISP_H_MARGINE;

			x = st_tm_chart_ctrl.area[ID_MKLOG_TM_GR_AREA1].x;
			y = st_tm_chart_ctrl.area[ID_MKLOG_TM_GR_AREA1].y;
			w = wd; 
			h = hd;
			BitBlt(
				hdc, xd, yd, wd, hd, st_timelog_wnd.hdc[ID_MKLOG_HDC_MEM_INFO], x, y,
				SRCCOPY
			);

			xd = 0;
			yd = st_timelog_wnd.pt_obj[ID_MKLOG_ITEM_TMCHART_GRAPH2].y;
			wd = st_timelog_wnd.size_obj[ID_MKLOG_ITEM_TMCHART_GRAPH2].cx + MKLOG_TM_DISP_X_MARGINE;
			hd = st_timelog_wnd.size_obj[ID_MKLOG_ITEM_TMCHART_GRAPH2].cy + MKLOG_TM_DISP_H_MARGINE;

			x = st_tm_chart_ctrl.area[ID_MKLOG_TM_GR_AREA2].x;
			y = st_tm_chart_ctrl.area[ID_MKLOG_TM_GR_AREA2].y;
			w = wd; 
			h = hd;
			BitBlt(
				hdc, xd, yd, wd, hd, st_timelog_wnd.hdc[ID_MKLOG_HDC_MEM_INFO], x, y,
				SRCCOPY
			);
		}

		EndPaint(hWnd, &ps);
	}break;
	case WM_DESTROY: {
		is_log_wnd_active.i16[MKLOG_ID_TYPE_TIME] = L_OFF;
		set_logstatus(type, (get_logstatus(type) & ~MKLOG_CODE_LOG_REC_AND_CHART));
		delete_wnd_objects(type);
		//PostQuitMessage(0);
	}break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	
	return S_OK;
}
LRESULT CALLBACK CMKLog::ScatLogWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

	HINSTANCE hInst = GetModuleHandle(0);

	int type = MKLOG_ID_TYPE_SCAT;
	switch (message)
	{
	case WM_CREATE: {
		InitCommonControls();//コモンコントロール初期化

		is_log_wnd_active.i16[MKLOG_ID_TYPE_SCAT] = L_ON;
		//ウィンドウにコントロール追加
		setup_scatlog_wnd(hWnd);
		st_sc_chart_ctrl.icon_draw = L_OFF;
		create_wnd_objects(type);
		init_graphic(type);
		st_sc_chart_ctrl.req_bg_update = L_ON;

		init_log_setting(type, L_OFF);

	}break;
	case WM_TIMER: {
		wos.str(L"");
		wos << L"ログ収集 INDEX :" << logbuf[type].iw;
		SetWindowText(st_scatlog_wnd.hwnd_obj[ID_MKLOG_ITEM_SCCHART_ST_INF], wos.str().c_str());

		set_record(type, logidhot[type]);//ログバッファのレコードに各項目の現在値を書き込み

		//テスト描画
#if 0
		INT cycle_ms = 20000;
		double th = (double)(st_sc_chart_ctrl.eraps_ms % cycle_ms) * PI360 / (double)cycle_ms;
		double a = 10000.0 - (double)(st_sc_chart_ctrl.eraps_ms % 9000);
		double dx = a * cos(th);
		double dy = a * sin(th + PI45);
		st_sc_chart_ctrl.plot_item_code[0] = st_sc_chart_ctrl.plot_item_code[1] = st_sc_chart_ctrl.plot_item_code[12] = st_sc_chart_ctrl.plot_item_code[13] = L_ON;
		loghot[type][0 + MKLOG_INDEX_LOG_DATA0] = loghot[type][12 + MKLOG_INDEX_LOG_DATA0] = dx;
		loghot[type][1 + MKLOG_INDEX_LOG_DATA0] = loghot[type][13 + MKLOG_INDEX_LOG_DATA0] = dy;
#endif
		logidhot[type]++;


		//グラフィック更新用　ON PAINT　呼び出し　表示更新
		if (st_scatlog_wnd.stat_obj[ID_MKLOG_ITEM_SCCHART_PB_CHART]) {//チャート起動中
			//経過時間更新
			st_sc_chart_ctrl.eraps_ms += (UINT)(logbuf[type].header.d100[MKLOG_INDEX_SCAN_MS]);

	
			//CHART表示エリア更新
			draw_graphic(MKLOG_ID_TYPE_SCAT, false);
			draw_info(MKLOG_ID_TYPE_SCAT);
			//WM_PAINT　画面更新
			InvalidateRect(hWnd, &st_scatlog_wnd.rc[ID_MKLOG_ITEM_SCCHART_GRAPH1], FALSE);
			InvalidateRect(hWnd, &st_scatlog_wnd.rc[ID_MKLOG_ITEM_SCCHART_GRAPH2], FALSE);
		}
	}break;
	case WM_COMMAND: {
		int wmId = LOWORD(wParam);
		// 選択されたメニューの解析:
		switch (wmId)
		{

		case MKLOG_ID_WND_CTRL_SCAT + ID_MKLOG_ITEM_SCCHART_CB_ACT: {		//ログ記録開始/停止
			if (BST_CHECKED == SendMessage(st_scatlog_wnd.hwnd_obj[ID_MKLOG_ITEM_SCCHART_CB_ACT], BM_GETCHECK, 0, 0)) {

				st_tm_chart_ctrl.scan_time_ms = (UINT)(logbuf[type].header.d100[MKLOG_INDEX_SCAN_MS]);
				SetTimer(hWnd, ID_LOG_TIMER_SCAT, st_tm_chart_ctrl.scan_time_ms, NULL);

				st_work_wnd.is_timer_active[type] = L_ON;
				logidhot[type] = 0;
				SetWindowText(st_scatlog_wnd.hwnd_obj[ID_MKLOG_ITEM_SCCHART_ST_INF], L"ログ起動");
				logbuf[type].iw = 0;
				for (int i = 0; i < MKLOG_N_LOG_RECORD; i++)logbuf[type].records[i].id = 0;
				set_logstatus(type, (get_logstatus(type) | MKLOG_CODE_LOG_RECORDING));
			}
			else {
				KillTimer(hWnd, ID_LOG_TIMER_SCAT);
				st_work_wnd.is_timer_active[type] = L_OFF;

				wos.str(L"");
				wos << L"ログ停止 :" << logbuf[type].iw;
				SetWindowText(st_scatlog_wnd.hwnd_obj[ID_MKLOG_ITEM_SCCHART_ST_INF], wos.str().c_str());

				set_logstatus(type, (get_logstatus(type) & ~MKLOG_CODE_LOG_RECORDING));
			}
		}break;
		case MKLOG_ID_WND_CTRL_SCAT + ID_MKLOG_ITEM_SCCHART_PB_CHART: {		//ログチャート表示開始/停止

			if (st_sc_chart_ctrl.icon_draw != L_ON) {
				draw_icon(GetDC(hWnd), MKLOG_ID_TYPE_SCAT);
			}

			if (get_logstatus(type) & MKLOG_CODE_LOG_CHART_ACTIVE) {
				set_logstatus(type, (get_logstatus(type) & ~MKLOG_CODE_LOG_CHART_ACTIVE));
				st_timelog_wnd.stat_obj[ID_MKLOG_ITEM_SCCHART_PB_CHART] = BST_UNCHECKED;
				SetWindowText(st_scatlog_wnd.hwnd_obj[ID_MKLOG_ITEM_TMCHART_ST_INF], L"チャートを停止します");
				st_sc_chart_ctrl.req_graph_update = L_OFF;

				KillTimer(hWnd, ID_LOG_TIMER_SCAT);
				st_work_wnd.is_timer_active[type] = L_OFF;

			}
			else {
				init_log_setting(type, L_OFF);
				st_scatlog_wnd.stat_obj[ID_MKLOG_ITEM_SCCHART_PB_CHART] = BST_CHECKED;
				set_logstatus(type, (get_logstatus(type) | MKLOG_CODE_LOG_CHART_ACTIVE));
				SetWindowText(st_scatlog_wnd.hwnd_obj[ID_MKLOG_ITEM_SCCHART_ST_INF], L"チャートを表示します");
				st_sc_chart_ctrl.req_graph_update = L_ON;

				if (st_work_wnd.is_timer_active[type] == L_OFF) {
					st_tm_chart_ctrl.scan_time_ms = (UINT)(logbuf[type].header.d100[MKLOG_INDEX_SCAN_MS]);
					SetTimer(hWnd, ID_LOG_TIMER_SCAT, st_tm_chart_ctrl.scan_time_ms, NULL);
					st_work_wnd.is_timer_active[type] = L_ON;
				}

				//プロットするコードの取り込み
				wchar_t buffer[256];
				for (int i = ID_MKLOG_ITEM_SCCHART_ED_F1CODE1, k = 0; i <= ID_MKLOG_ITEM_SCCHART_ED_F2CODE8; i++, k++) {
					GetWindowText(st_scatlog_wnd.hwnd_obj[i], buffer, 256);
					std::wistringstream iss(buffer);
					iss >> std::dec >> st_sc_chart_ctrl.plot_item_code[k];
				}

				for (int i = ID_MKLOG_HDC_MEM0; i <= ID_MKLOG_GR_MEM_INFO; i++) {
					PatBlt(st_scatlog_wnd.hdc[i], st_sc_chart_ctrl.area[ID_MKLOG_SC_GR_AREA1].x + MKLOG_SC_DISP_X_MARGINE, st_sc_chart_ctrl.area[ID_MKLOG_SC_GR_AREA1].y, st_sc_chart_ctrl.area[ID_MKLOG_SC_GR_AREA1].w + MKLOG_SC_DISP_X_MARGINE, st_sc_chart_ctrl.area[ID_MKLOG_SC_GR_AREA1].h, WHITENESS);
					PatBlt(st_scatlog_wnd.hdc[i], st_sc_chart_ctrl.area[ID_MKLOG_SC_GR_AREA2].x + MKLOG_SC_DISP_X_MARGINE, st_sc_chart_ctrl.area[ID_MKLOG_SC_GR_AREA2].y, st_sc_chart_ctrl.area[ID_MKLOG_SC_GR_AREA2].w + MKLOG_SC_DISP_X_MARGINE, st_sc_chart_ctrl.area[ID_MKLOG_SC_GR_AREA2].h, WHITENESS);
				}
				clear_record(type);
				st_sc_chart_ctrl.eraps_ms = 0;
				draw_bg(type);//描画ポイントリセット
				draw_graphic(type, true);//描画ポイントリセット
			}
		}break;
		case MKLOG_ID_WND_CTRL_SCAT + ID_MKLOG_ITEM_SCCHART_PB_REFRESH: {			//REFRESH
			for (int i = ID_MKLOG_HDC_MEM0; i <= ID_MKLOG_GR_MEM_INFO; i++) {
				PatBlt(st_scatlog_wnd.hdc[i], st_sc_chart_ctrl.area[ID_MKLOG_SC_GR_AREA1].x + MKLOG_SC_DISP_X_MARGINE, st_sc_chart_ctrl.area[ID_MKLOG_SC_GR_AREA1].y, st_sc_chart_ctrl.area[ID_MKLOG_SC_GR_AREA1].w + MKLOG_SC_DISP_X_MARGINE, st_sc_chart_ctrl.area[ID_MKLOG_SC_GR_AREA1].h, WHITENESS);
				PatBlt(st_scatlog_wnd.hdc[i], st_sc_chart_ctrl.area[ID_MKLOG_SC_GR_AREA2].x + MKLOG_SC_DISP_X_MARGINE, st_sc_chart_ctrl.area[ID_MKLOG_SC_GR_AREA2].y, st_sc_chart_ctrl.area[ID_MKLOG_SC_GR_AREA2].w + MKLOG_SC_DISP_X_MARGINE, st_sc_chart_ctrl.area[ID_MKLOG_SC_GR_AREA2].h, WHITENESS);
			}
			st_tm_chart_ctrl.eraps_ms = 0;
			draw_bg(type);

			//LOG SOURCE更新
			init_log_setting(type, L_ON);
			SetWindowText(st_scatlog_wnd.hwnd_obj[ID_MKLOG_ITEM_SCCHART_ST_INF], L"チャート設定を更新しました");
			clear_record(type);
		}break;
		case MKLOG_ID_WND_CTRL_SCAT + ID_MKLOG_ITEM_SCCHART_PB_PAUSE: {			//PAUSE/RESTART
			if (st_scatlog_wnd.stat_obj[ID_MKLOG_ITEM_SCCHART_PB_PAUSE] == L_OFF) {
				st_scatlog_wnd.stat_obj[ID_MKLOG_ITEM_SCCHART_PB_PAUSE] = L_ON;
				KillTimer(hWnd, ID_LOG_TIMER_SCAT);
				st_work_wnd.is_timer_active[type] = L_OFF;
				SetWindowText(st_scatlog_wnd.hwnd_obj[ID_MKLOG_ITEM_SCCHART_PB_PAUSE], L"RESTART");
			}
			else {

				if ((st_scatlog_wnd.stat_obj[ID_MKLOG_ITEM_SCCHART_PB_PAUSE] == L_OFF) && (st_scatlog_wnd.stat_obj[ID_MKLOG_ITEM_SCCHART_PB_CHART])) {
		    
					st_scatlog_wnd.stat_obj[ID_MKLOG_ITEM_SCCHART_PB_PAUSE] = L_OFF;
					SetTimer(hWnd, ID_LOG_TIMER_SCAT, (UINT)(logbuf[type].header.d100[MKLOG_INDEX_SCAN_MS]), NULL);
					st_work_wnd.is_timer_active[type] = L_ON;
					SetWindowText(st_scatlog_wnd.hwnd_obj[ID_MKLOG_ITEM_SCCHART_PB_PAUSE], L"PAUSE");			}

				}
		}break;
		case MKLOG_ID_WND_CTRL_SCAT + ID_MKLOG_ITEM_SCCHART_PB_BMP: {			//RESTART

		}break;
		case MKLOG_ID_WND_CTRL_SCAT + ID_MKLOG_ITEM_SCCHART_PB_FILE: {			//ログファイル書き出し
			if (get_logstatus(type) & MKLOG_CODE_LOG_FILE_OUT_ACTIVE) {
				set_logstatus(type, (get_logstatus(type) & ~MKLOG_CODE_LOG_FILE_OUT_ACTIVE));
				SetWindowText(st_timelog_wnd.hwnd_obj[ID_MKLOG_ITEM_TMCHART_ST_INF], L"ログを出力しました");
			}
			else {
				set_logstatus(type, (get_logstatus(type) | MKLOG_CODE_LOG_FILE_OUT_ACTIVE));
				unsigned thrdID;
				HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, CMKLog::logfileout, &type, 0, &thrdID);
			}
		}break;
		case MKLOG_ID_WND_CTRL_SCAT + ID_MKLOG_ITEM_SCCHART_PB_TIMEx2:
		case MKLOG_ID_WND_CTRL_SCAT + ID_MKLOG_ITEM_SCCHART_PB_TIMEd2:
		{
			if (wmId== MKLOG_ID_WND_CTRL_SCAT + ID_MKLOG_ITEM_SCCHART_PB_TIMEd2) {
				st_sc_chart_ctrl.plot_disp_range /= 2;
				if (st_sc_chart_ctrl.plot_disp_range < 10)st_sc_chart_ctrl.plot_disp_range = 10;
			}
			else {
				st_sc_chart_ctrl.plot_disp_range *= 2;
				if (st_sc_chart_ctrl.plot_disp_range > 1000)st_sc_chart_ctrl.plot_disp_range = 1000;
			}
			wos.str(L""); wos << (st_sc_chart_ctrl.plot_disp_range * st_sc_chart_ctrl.scan_time_ms) << L" ms";
			SetWindowText(st_scatlog_wnd.hwnd_obj[ID_MKLOG_ITEM_SCCHART_ST_DISPTIME], wos.str().c_str());

			PatBlt(st_scatlog_wnd.hdc[ID_MKLOG_GR_MEM_GR], st_sc_chart_ctrl.area[ID_MKLOG_SC_GR_AREA1].x + MKLOG_SC_DISP_X_MARGINE, st_sc_chart_ctrl.area[ID_MKLOG_SC_GR_AREA1].y, st_sc_chart_ctrl.area[ID_MKLOG_SC_GR_AREA1].w + MKLOG_SC_DISP_X_MARGINE, st_sc_chart_ctrl.area[ID_MKLOG_SC_GR_AREA1].h, WHITENESS);
			PatBlt(st_scatlog_wnd.hdc[ID_MKLOG_GR_MEM_GR], st_sc_chart_ctrl.area[ID_MKLOG_SC_GR_AREA2].x + MKLOG_SC_DISP_X_MARGINE, st_sc_chart_ctrl.area[ID_MKLOG_SC_GR_AREA2].y, st_sc_chart_ctrl.area[ID_MKLOG_SC_GR_AREA2].w + MKLOG_SC_DISP_X_MARGINE, st_sc_chart_ctrl.area[ID_MKLOG_SC_GR_AREA2].h, WHITENESS);

		}break;

		case MKLOG_ID_WND_CTRL_SCAT + ID_MKLOG_ITEM_SCCHART_PB_LIST: {			//ログファイル書き出し
			if (st_sc_chart_ctrl.chart_item_txt_mode == ID_MKLOG_ITEM_TXT_NAME) {
				st_sc_chart_ctrl.chart_item_txt_mode = ID_MKLOG_ITEM_TXT_100;
				SetWindowText(st_scatlog_wnd.hwnd_obj[ID_MKLOG_ITEM_SCCHART_PB_LIST], L"NAME");
			}
			else {
				st_sc_chart_ctrl.chart_item_txt_mode = ID_MKLOG_ITEM_TXT_NAME;
				SetWindowText(st_scatlog_wnd.hwnd_obj[ID_MKLOG_ITEM_SCCHART_PB_LIST], L"100%");
			}
			init_log_setting(type, L_OFF);
		}break;

		case MKLOG_ID_WND_CTRL_SCAT + ID_MKLOG_ITEM_SCCHART_PB_F1Xx1:
		case MKLOG_ID_WND_CTRL_SCAT + ID_MKLOG_ITEM_SCCHART_PB_F1Xx2:
		case MKLOG_ID_WND_CTRL_SCAT + ID_MKLOG_ITEM_SCCHART_PB_F1Xx4:
		case MKLOG_ID_WND_CTRL_SCAT + ID_MKLOG_ITEM_SCCHART_PB_F1XP10:
		case MKLOG_ID_WND_CTRL_SCAT + ID_MKLOG_ITEM_SCCHART_PB_F1XN10:
		{
			switch (wmId) {
			case MKLOG_ID_WND_CTRL_SCAT + ID_MKLOG_ITEM_SCCHART_PB_F1Xx1:
				st_sc_chart_ctrl.disp_mag_xc[ID_MKLOG_PLOT_GRAPH1_A] = 100; break;
			case MKLOG_ID_WND_CTRL_SCAT + ID_MKLOG_ITEM_SCCHART_PB_F1Xx2:
				st_sc_chart_ctrl.disp_mag_xc[ID_MKLOG_PLOT_GRAPH1_A] = 200; break;
			case MKLOG_ID_WND_CTRL_SCAT + ID_MKLOG_ITEM_SCCHART_PB_F1Xx4:
				st_sc_chart_ctrl.disp_mag_xc[ID_MKLOG_PLOT_GRAPH1_A] = 400; break;
			case MKLOG_ID_WND_CTRL_SCAT + ID_MKLOG_ITEM_SCCHART_PB_F1XP10:
				st_sc_chart_ctrl.disp_offset_x[ID_MKLOG_PLOT_GRAPH1_A] += 10;
				if (st_sc_chart_ctrl.disp_offset_x[ID_MKLOG_PLOT_GRAPH1_A] > 200)st_sc_chart_ctrl.disp_offset_x[ID_MKLOG_PLOT_GRAPH1_A] = 200;
				break;
			case MKLOG_ID_WND_CTRL_SCAT + ID_MKLOG_ITEM_SCCHART_PB_F1XN10:
				st_sc_chart_ctrl.disp_offset_x[ID_MKLOG_PLOT_GRAPH1_A] -= 10;
				if (st_sc_chart_ctrl.disp_offset_x[ID_MKLOG_PLOT_GRAPH1_A] < -200)st_sc_chart_ctrl.disp_offset_x[ID_MKLOG_PLOT_GRAPH1_A] = -200;
				break;
			}
			INT xmiddle_c = -st_sc_chart_ctrl.disp_offset_x[ID_MKLOG_PLOT_GRAPH1_A] * st_sc_chart_ctrl.disp_mag_xc[ID_MKLOG_PLOT_GRAPH1_A] / 100;
			INT xmax_c = st_sc_chart_ctrl.disp_mag_xc[ID_MKLOG_PLOT_GRAPH1_A] + xmiddle_c;
			INT xmin_c = xmiddle_c - (xmax_c - xmiddle_c);

			wos.str(L""); wos << xmax_c << L"%";
			for (int i = 0; i < MKLOG_N_LOG_TITLE_WCH; i++) {
				if (i < wos.str().length())
					st_scatlog_wnd.tx_obj[ID_MKLOG_ITEM_SCCHART_ST_F1X_MAX][i] = *(wos.str().c_str() + i);
				else
					st_scatlog_wnd.tx_obj[ID_MKLOG_ITEM_SCCHART_ST_F1X_MAX][i] = L'\0';
			};

			wos.str(L""); wos << xmiddle_c << L"%";
			for (int i = 0; i < MKLOG_N_LOG_TITLE_WCH; i++) {
				if (i < wos.str().length())
					st_scatlog_wnd.tx_obj[ID_MKLOG_ITEM_SCCHART_ST_F1X_MIDDLE][i] = *(wos.str().c_str() + i);
				else
					st_scatlog_wnd.tx_obj[ID_MKLOG_ITEM_SCCHART_ST_F1X_MIDDLE][i] = L'\0';
			};

			wos.str(L""); wos << xmin_c << L"%";
			for (int i = 0; i < MKLOG_N_LOG_TITLE_WCH; i++) {
				if (i < wos.str().length())
					st_scatlog_wnd.tx_obj[ID_MKLOG_ITEM_SCCHART_ST_F1X_MIN][i] = *(wos.str().c_str() + i);
				else
					st_scatlog_wnd.tx_obj[ID_MKLOG_ITEM_SCCHART_ST_F1X_MIN][i] = L'\0';
			};

			PatBlt(st_scatlog_wnd.hdc[ID_MKLOG_GR_MEM_GR], st_sc_chart_ctrl.area[ID_MKLOG_SC_GR_AREA1].x + MKLOG_SC_DISP_X_MARGINE, st_sc_chart_ctrl.area[ID_MKLOG_SC_GR_AREA1].y, st_sc_chart_ctrl.area[ID_MKLOG_SC_GR_AREA1].w + MKLOG_SC_DISP_X_MARGINE, st_sc_chart_ctrl.area[ID_MKLOG_SC_GR_AREA1].h, WHITENESS);

		}break;

		case MKLOG_ID_WND_CTRL_SCAT + ID_MKLOG_ITEM_SCCHART_PB_F1Yx1:
		case MKLOG_ID_WND_CTRL_SCAT + ID_MKLOG_ITEM_SCCHART_PB_F1Yx2:
		case MKLOG_ID_WND_CTRL_SCAT + ID_MKLOG_ITEM_SCCHART_PB_F1Yx4:
		case MKLOG_ID_WND_CTRL_SCAT + ID_MKLOG_ITEM_SCCHART_PB_F1YP10:
		case MKLOG_ID_WND_CTRL_SCAT + ID_MKLOG_ITEM_SCCHART_PB_F1YN10:
		{
			switch (wmId) {
			case MKLOG_ID_WND_CTRL_SCAT + ID_MKLOG_ITEM_SCCHART_PB_F1Yx1:
				st_sc_chart_ctrl.disp_mag_yc[ID_MKLOG_PLOT_GRAPH1_A] = 100; break;
			case MKLOG_ID_WND_CTRL_SCAT + ID_MKLOG_ITEM_SCCHART_PB_F1Yx2:
				st_sc_chart_ctrl.disp_mag_yc[ID_MKLOG_PLOT_GRAPH1_A] = 200; break;
			case MKLOG_ID_WND_CTRL_SCAT + ID_MKLOG_ITEM_SCCHART_PB_F1Yx4:
				st_sc_chart_ctrl.disp_mag_yc[ID_MKLOG_PLOT_GRAPH1_A] = 400; break;
			case MKLOG_ID_WND_CTRL_SCAT + ID_MKLOG_ITEM_SCCHART_PB_F1YP10:
				st_sc_chart_ctrl.disp_offset_y[ID_MKLOG_PLOT_GRAPH1_A] += 10;
				if (st_sc_chart_ctrl.disp_offset_y[ID_MKLOG_PLOT_GRAPH1_A] > 200)st_sc_chart_ctrl.disp_offset_y[ID_MKLOG_PLOT_GRAPH1_A] = 200;
				break;
			case MKLOG_ID_WND_CTRL_SCAT + ID_MKLOG_ITEM_SCCHART_PB_F1YN10:
				st_sc_chart_ctrl.disp_offset_y[ID_MKLOG_PLOT_GRAPH1_A] -= 10;
				if (st_sc_chart_ctrl.disp_offset_y[ID_MKLOG_PLOT_GRAPH1_A] < -200)st_sc_chart_ctrl.disp_offset_y[ID_MKLOG_PLOT_GRAPH1_A] = -200;
				break;
			}
			INT ymiddle_c = -st_sc_chart_ctrl.disp_offset_y[ID_MKLOG_PLOT_GRAPH1_A] * st_sc_chart_ctrl.disp_mag_yc[ID_MKLOG_PLOT_GRAPH1_A] / 100;
			INT ymax_c = st_sc_chart_ctrl.disp_mag_yc[ID_MKLOG_PLOT_GRAPH1_A] + ymiddle_c;
			INT ymin_c = ymiddle_c - (ymax_c - ymiddle_c);

			wos.str(L""); wos << ymax_c << L"%";
			for (int i = 0; i < MKLOG_N_LOG_TITLE_WCH; i++) {
				if (i < wos.str().length())
					st_scatlog_wnd.tx_obj[ID_MKLOG_ITEM_SCCHART_ST_F1Y_MAX][i] = *(wos.str().c_str() + i);
				else
					st_scatlog_wnd.tx_obj[ID_MKLOG_ITEM_SCCHART_ST_F1Y_MAX][i] = L'\0';
			};

			wos.str(L""); wos << ymiddle_c << L"%";
			for (int i = 0; i < MKLOG_N_LOG_TITLE_WCH; i++) {
				if (i < wos.str().length())
					st_scatlog_wnd.tx_obj[ID_MKLOG_ITEM_SCCHART_ST_F1Y_MIDDLE][i] = *(wos.str().c_str() + i);
				else
					st_scatlog_wnd.tx_obj[ID_MKLOG_ITEM_SCCHART_ST_F1Y_MIDDLE][i] = L'\0';
			};

			wos.str(L""); wos << ymin_c << L"%";
			for (int i = 0; i < MKLOG_N_LOG_TITLE_WCH; i++) {
				if (i < wos.str().length())
					st_scatlog_wnd.tx_obj[ID_MKLOG_ITEM_SCCHART_ST_F1Y_MIN][i] = *(wos.str().c_str() + i);
				else
					st_scatlog_wnd.tx_obj[ID_MKLOG_ITEM_SCCHART_ST_F1Y_MIN][i] = L'\0';
			};

			PatBlt(st_scatlog_wnd.hdc[ID_MKLOG_GR_MEM_GR], st_sc_chart_ctrl.area[ID_MKLOG_SC_GR_AREA1].x + MKLOG_SC_DISP_X_MARGINE, st_sc_chart_ctrl.area[ID_MKLOG_SC_GR_AREA1].y, st_sc_chart_ctrl.area[ID_MKLOG_SC_GR_AREA1].w + MKLOG_SC_DISP_X_MARGINE, st_sc_chart_ctrl.area[ID_MKLOG_SC_GR_AREA1].h, WHITENESS);

		}break;

		case MKLOG_ID_WND_CTRL_SCAT + ID_MKLOG_ITEM_SCCHART_PB_F2Xx1:
		case MKLOG_ID_WND_CTRL_SCAT + ID_MKLOG_ITEM_SCCHART_PB_F2Xx2:
		case MKLOG_ID_WND_CTRL_SCAT + ID_MKLOG_ITEM_SCCHART_PB_F2Xx4:
		case MKLOG_ID_WND_CTRL_SCAT + ID_MKLOG_ITEM_SCCHART_PB_F2XP10:
		case MKLOG_ID_WND_CTRL_SCAT + ID_MKLOG_ITEM_SCCHART_PB_F2XN10:
		{
			switch (wmId) {
			case MKLOG_ID_WND_CTRL_SCAT + ID_MKLOG_ITEM_SCCHART_PB_F2Xx1:
				st_sc_chart_ctrl.disp_mag_xc[ID_MKLOG_PLOT_GRAPH2_A] = 100; break;
			case MKLOG_ID_WND_CTRL_SCAT + ID_MKLOG_ITEM_SCCHART_PB_F2Xx2:
				st_sc_chart_ctrl.disp_mag_xc[ID_MKLOG_PLOT_GRAPH2_A] = 200; break;
			case MKLOG_ID_WND_CTRL_SCAT + ID_MKLOG_ITEM_SCCHART_PB_F2Xx4:
				st_sc_chart_ctrl.disp_mag_xc[ID_MKLOG_PLOT_GRAPH2_A] = 400; break;
			case MKLOG_ID_WND_CTRL_SCAT + ID_MKLOG_ITEM_SCCHART_PB_F2XP10:
				st_sc_chart_ctrl.disp_offset_x[ID_MKLOG_PLOT_GRAPH2_A] += 10;
				if (st_sc_chart_ctrl.disp_offset_x[ID_MKLOG_PLOT_GRAPH2_A] > 200)st_sc_chart_ctrl.disp_offset_x[ID_MKLOG_PLOT_GRAPH2_A] = 200;
				break;
			case MKLOG_ID_WND_CTRL_SCAT + ID_MKLOG_ITEM_SCCHART_PB_F2XN10:
				st_sc_chart_ctrl.disp_offset_x[ID_MKLOG_PLOT_GRAPH2_A] -= 10;
				if (st_sc_chart_ctrl.disp_offset_x[ID_MKLOG_PLOT_GRAPH2_A] < -200)st_sc_chart_ctrl.disp_offset_x[ID_MKLOG_PLOT_GRAPH2_A] = -200;
				break;
			}
			INT xmiddle_c = -st_sc_chart_ctrl.disp_offset_x[ID_MKLOG_PLOT_GRAPH2_A] * st_sc_chart_ctrl.disp_mag_xc[ID_MKLOG_PLOT_GRAPH2_A] / 100;
			INT xmax_c = st_sc_chart_ctrl.disp_mag_xc[ID_MKLOG_PLOT_GRAPH2_A] + xmiddle_c;
			INT xmin_c = xmiddle_c - (xmax_c - xmiddle_c);

			wos.str(L""); wos << xmax_c << L"%";
			for (int i = 0; i < MKLOG_N_LOG_TITLE_WCH; i++) {
				if (i < wos.str().length())
					st_scatlog_wnd.tx_obj[ID_MKLOG_ITEM_SCCHART_ST_F2X_MAX][i] = *(wos.str().c_str() + i);
				else
					st_scatlog_wnd.tx_obj[ID_MKLOG_ITEM_SCCHART_ST_F2X_MAX][i] = L'\0';
			};

			wos.str(L""); wos << xmiddle_c << L"%";
			for (int i = 0; i < MKLOG_N_LOG_TITLE_WCH; i++) {
				if (i < wos.str().length())
					st_scatlog_wnd.tx_obj[ID_MKLOG_ITEM_SCCHART_ST_F2X_MIDDLE][i] = *(wos.str().c_str() + i);
				else
					st_scatlog_wnd.tx_obj[ID_MKLOG_ITEM_SCCHART_ST_F2X_MIDDLE][i] = L'\0';
			};

			wos.str(L""); wos << xmin_c << L"%";
			for (int i = 0; i < MKLOG_N_LOG_TITLE_WCH; i++) {
				if (i < wos.str().length())
					st_scatlog_wnd.tx_obj[ID_MKLOG_ITEM_SCCHART_ST_F2X_MIN][i] = *(wos.str().c_str() + i);
				else
					st_scatlog_wnd.tx_obj[ID_MKLOG_ITEM_SCCHART_ST_F2X_MIN][i] = L'\0';
			};

			PatBlt(st_scatlog_wnd.hdc[ID_MKLOG_GR_MEM_GR], st_sc_chart_ctrl.area[ID_MKLOG_SC_GR_AREA2].x + MKLOG_SC_DISP_X_MARGINE, st_sc_chart_ctrl.area[ID_MKLOG_SC_GR_AREA2].y, st_sc_chart_ctrl.area[ID_MKLOG_SC_GR_AREA2].w + MKLOG_SC_DISP_X_MARGINE, st_sc_chart_ctrl.area[ID_MKLOG_SC_GR_AREA2].h, WHITENESS);

		}break;

		case MKLOG_ID_WND_CTRL_SCAT + ID_MKLOG_ITEM_SCCHART_PB_F2Yx1:
		case MKLOG_ID_WND_CTRL_SCAT + ID_MKLOG_ITEM_SCCHART_PB_F2Yx2:
		case MKLOG_ID_WND_CTRL_SCAT + ID_MKLOG_ITEM_SCCHART_PB_F2Yx4:
		case MKLOG_ID_WND_CTRL_SCAT + ID_MKLOG_ITEM_SCCHART_PB_F2YP10:
		case MKLOG_ID_WND_CTRL_SCAT + ID_MKLOG_ITEM_SCCHART_PB_F2YN10:
		{
			switch (wmId) {
			case MKLOG_ID_WND_CTRL_SCAT + ID_MKLOG_ITEM_SCCHART_PB_F2Yx1:
				st_sc_chart_ctrl.disp_mag_yc[ID_MKLOG_PLOT_GRAPH2_A] = 100; break;
			case MKLOG_ID_WND_CTRL_SCAT + ID_MKLOG_ITEM_SCCHART_PB_F2Yx2:
				st_sc_chart_ctrl.disp_mag_yc[ID_MKLOG_PLOT_GRAPH2_A] = 200; break;
			case MKLOG_ID_WND_CTRL_SCAT + ID_MKLOG_ITEM_SCCHART_PB_F2Yx4:
				st_sc_chart_ctrl.disp_mag_yc[ID_MKLOG_PLOT_GRAPH2_A] = 400; break;
			case MKLOG_ID_WND_CTRL_SCAT + ID_MKLOG_ITEM_SCCHART_PB_F2YP10:
				st_sc_chart_ctrl.disp_offset_y[ID_MKLOG_PLOT_GRAPH2_A] += 10;
				if (st_sc_chart_ctrl.disp_offset_y[ID_MKLOG_PLOT_GRAPH2_A] > 200)st_sc_chart_ctrl.disp_offset_y[ID_MKLOG_PLOT_GRAPH2_A] = 200;
				break;
			case MKLOG_ID_WND_CTRL_SCAT + ID_MKLOG_ITEM_SCCHART_PB_F2YN10:
				st_sc_chart_ctrl.disp_offset_y[ID_MKLOG_PLOT_GRAPH2_A] -= 10;
				if (st_sc_chart_ctrl.disp_offset_y[ID_MKLOG_PLOT_GRAPH2_A] < -200)st_sc_chart_ctrl.disp_offset_y[ID_MKLOG_PLOT_GRAPH2_A] = -200;
				break;
			}
			INT ymiddle_c = -st_sc_chart_ctrl.disp_offset_y[ID_MKLOG_PLOT_GRAPH2_A] * st_sc_chart_ctrl.disp_mag_yc[ID_MKLOG_PLOT_GRAPH2_A] / 100;
			INT ymax_c = st_sc_chart_ctrl.disp_mag_yc[ID_MKLOG_PLOT_GRAPH2_A] + ymiddle_c;
			INT ymin_c = ymiddle_c - (ymax_c - ymiddle_c);

			wos.str(L""); wos << ymax_c << L"%";
			for (int i = 0; i < MKLOG_N_LOG_TITLE_WCH; i++) {
				if (i < wos.str().length())
					st_scatlog_wnd.tx_obj[ID_MKLOG_ITEM_SCCHART_ST_F2Y_MAX][i] = *(wos.str().c_str() + i);
				else
					st_scatlog_wnd.tx_obj[ID_MKLOG_ITEM_SCCHART_ST_F2Y_MAX][i] = L'\0';
			};

			wos.str(L""); wos << ymiddle_c << L"%";
			for (int i = 0; i < MKLOG_N_LOG_TITLE_WCH; i++) {
				if (i < wos.str().length())
					st_scatlog_wnd.tx_obj[ID_MKLOG_ITEM_SCCHART_ST_F2Y_MIDDLE][i] = *(wos.str().c_str() + i);
				else
					st_scatlog_wnd.tx_obj[ID_MKLOG_ITEM_SCCHART_ST_F2Y_MIDDLE][i] = L'\0';
			};

			wos.str(L""); wos << ymin_c << L"%";
			for (int i = 0; i < MKLOG_N_LOG_TITLE_WCH; i++) {
				if (i < wos.str().length())
					st_scatlog_wnd.tx_obj[ID_MKLOG_ITEM_SCCHART_ST_F2Y_MIN][i] = *(wos.str().c_str() + i);
				else
					st_scatlog_wnd.tx_obj[ID_MKLOG_ITEM_SCCHART_ST_F2Y_MIN][i] = L'\0';
			};

			PatBlt(st_scatlog_wnd.hdc[ID_MKLOG_GR_MEM_GR], st_sc_chart_ctrl.area[ID_MKLOG_SC_GR_AREA2].x + MKLOG_SC_DISP_X_MARGINE, st_sc_chart_ctrl.area[ID_MKLOG_SC_GR_AREA2].y, st_sc_chart_ctrl.area[ID_MKLOG_SC_GR_AREA2].w + MKLOG_SC_DISP_X_MARGINE, st_sc_chart_ctrl.area[ID_MKLOG_SC_GR_AREA2].h, WHITENESS);

		}break;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);

		}
	}break;
	case WM_PAINT: {

		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);

		if (st_sc_chart_ctrl.req_bg_update) {		//背景
			draw_bg(MKLOG_ID_TYPE_SCAT);

			BitBlt(
				hdc, st_scatlog_wnd.pt_obj[ID_MKLOG_ITEM_SCCHART_GRAPH1].x, st_scatlog_wnd.pt_obj[ID_MKLOG_ITEM_SCCHART_GRAPH1].y, st_scatlog_wnd.size_obj[ID_MKLOG_ITEM_SCCHART_GRAPH1].cx, st_scatlog_wnd.size_obj[ID_MKLOG_ITEM_SCCHART_GRAPH1].cy,
				st_scatlog_wnd.hdc[ID_MKLOG_HDC_BG], st_sc_chart_ctrl.area[ID_MKLOG_SC_GR_AREA1].x + MKLOG_SC_DISP_X_MARGINE, st_sc_chart_ctrl.area[ID_MKLOG_SC_GR_AREA1].y,
				SRCCOPY
			);
			BitBlt(
				hdc, st_scatlog_wnd.pt_obj[ID_MKLOG_ITEM_SCCHART_GRAPH2].x, st_scatlog_wnd.pt_obj[ID_MKLOG_ITEM_SCCHART_GRAPH2].y, st_scatlog_wnd.size_obj[ID_MKLOG_ITEM_SCCHART_GRAPH2].cx, st_scatlog_wnd.size_obj[ID_MKLOG_ITEM_SCCHART_GRAPH2].cy,
				st_scatlog_wnd.hdc[ID_MKLOG_HDC_BG], st_sc_chart_ctrl.area[ID_MKLOG_SC_GR_AREA2].x + MKLOG_SC_DISP_X_MARGINE, st_sc_chart_ctrl.area[ID_MKLOG_SC_GR_AREA2].y,
				SRCCOPY
			);
		}
		if (st_sc_chart_ctrl.req_graph_update) {

	//		if (st_sc_chart_ctrl.req_bg_update) draw_bg(MKLOG_ID_TYPE_SCAT);

			//	PatBlt(hdc, 0, 0, st_scatlog_wnd.size_obj[ID_MKLOG_ITEM_SCCHART_FRAME].cx, st_scatlog_wnd.size_obj[ID_MKLOG_ITEM_SCCHART_FRAME].cy, WHITENESS);



			// MAIN DC に　INFO DC MEMの内容をコピー
			//CHART INFO
			draw_info(MKLOG_ID_TYPE_SCAT);

			INT xs, ys, ws, hs, xd, yd, wd, hd;

			// INFO DC MEMにBG DC MEMの内容をコピー
			xs = st_sc_chart_ctrl.area[ID_MKLOG_SC_GR_AREA1].x + MKLOG_SC_DISP_X_MARGINE; ys = st_sc_chart_ctrl.area[ID_MKLOG_SC_GR_AREA1].y;
			ws = st_sc_chart_ctrl.area[ID_MKLOG_SC_GR_AREA1].w - MKLOG_SC_DISP_X_MARGINE; hs = st_sc_chart_ctrl.area[ID_MKLOG_SC_GR_AREA1].h - MKLOG_SC_DISP_H_MARGINE;



			xd = st_scatlog_wnd.pt_obj[ID_MKLOG_ITEM_SCCHART_GRAPH1].x; 
			yd = st_scatlog_wnd.pt_obj[ID_MKLOG_ITEM_SCCHART_GRAPH1].y;
			wd = ws; hd = hs;

			BitBlt(st_scatlog_wnd.hdc[ID_MKLOG_HDC_MEM_INFO],xd ,yd, wd, hd,st_scatlog_wnd.hdc[ID_MKLOG_HDC_BG], xs, ys,SRCCOPY);

			// INFO MEMにGRAPHIC DC MEMの内容をコピー
			INT x1_offset = st_sc_chart_ctrl.disp_offset_x[ID_MKLOG_PLOT_GRAPH1_A];
			INT y1_offset = st_sc_chart_ctrl.disp_offset_y[ID_MKLOG_PLOT_GRAPH1_A];
			TransparentBlt(st_scatlog_wnd.hdc[ID_MKLOG_HDC_MEM_INFO], xd, yd, wd, hd, st_scatlog_wnd.hdc[ID_MKLOG_HDC_MEM_GR], xs - x1_offset, ys + y1_offset, ws, hs, RGB(255, 255, 255));


			xd = 0;
			yd = st_scatlog_wnd.pt_obj[ID_MKLOG_ITEM_SCCHART_GRAPH1].y;
			wd = st_scatlog_wnd.size_obj[ID_MKLOG_ITEM_SCCHART_GRAPH1].cx + MKLOG_SC_DISP_X_MARGINE;
			hd = st_scatlog_wnd.size_obj[ID_MKLOG_ITEM_SCCHART_GRAPH1].cy + MKLOG_SC_DISP_H_MARGINE;

			xs = xd;
			ys = yd;
			ws = wd;
			hs = hd;
//			TransparentBlt(hdc, xd, yd, wd, hd, st_scatlog_wnd.hdc[ID_MKLOG_HDC_MEM_INFO], xs, ys, ws, hs, RGB(255, 255, 255));
			BitBlt(hdc, xd, yd, wd, hd, st_scatlog_wnd.hdc[ID_MKLOG_HDC_MEM_INFO], xs, ys, SRCCOPY);




			// INFO DC MEMにBG DC MEMの内容をコピー
			xs = st_sc_chart_ctrl.area[ID_MKLOG_SC_GR_AREA2].x + MKLOG_SC_DISP_X_MARGINE; ys = st_sc_chart_ctrl.area[ID_MKLOG_SC_GR_AREA2].y;
			ws = st_sc_chart_ctrl.area[ID_MKLOG_SC_GR_AREA2].w - MKLOG_SC_DISP_X_MARGINE; hs = st_sc_chart_ctrl.area[ID_MKLOG_SC_GR_AREA2].h - MKLOG_SC_DISP_H_MARGINE;

	
			xd = st_scatlog_wnd.pt_obj[ID_MKLOG_ITEM_SCCHART_GRAPH2].x;
			yd = st_scatlog_wnd.pt_obj[ID_MKLOG_ITEM_SCCHART_GRAPH2].y;
			wd = ws; hd = hs;

			BitBlt(st_scatlog_wnd.hdc[ID_MKLOG_HDC_MEM_INFO], xd, yd, wd, hd, st_scatlog_wnd.hdc[ID_MKLOG_HDC_BG], xs, ys, SRCCOPY);

			// INFO MEMにGRAPHIC DC MEMの内容をコピー
			INT x2_offset = st_sc_chart_ctrl.disp_offset_x[ID_MKLOG_PLOT_GRAPH2_A];
			INT y2_offset = st_sc_chart_ctrl.disp_offset_y[ID_MKLOG_PLOT_GRAPH2_A];

			TransparentBlt(st_scatlog_wnd.hdc[ID_MKLOG_HDC_MEM_INFO], xd, yd, wd, hd, st_scatlog_wnd.hdc[ID_MKLOG_HDC_MEM_GR], xs - x2_offset, ys + y2_offset, ws, hs, RGB(255, 255, 255));

			xd = 0;
			yd = st_scatlog_wnd.pt_obj[ID_MKLOG_ITEM_SCCHART_GRAPH2].y;
			wd = st_scatlog_wnd.size_obj[ID_MKLOG_ITEM_SCCHART_GRAPH2].cx + MKLOG_SC_DISP_X_MARGINE;
			hd = st_scatlog_wnd.size_obj[ID_MKLOG_ITEM_SCCHART_GRAPH2].cy + MKLOG_SC_DISP_H_MARGINE;

			xs = xd;
			ys = yd;
			ws = wd;
			hs = hd;
//			TransparentBlt(hdc, xd, yd, wd, hd, st_scatlog_wnd.hdc[ID_MKLOG_HDC_MEM_INFO], xs, ys, ws, hs, RGB(255, 255, 255));
			BitBlt(hdc, xd, yd, wd, hd, st_scatlog_wnd.hdc[ID_MKLOG_HDC_MEM_INFO], xs, ys, SRCCOPY);
		}

		EndPaint(hWnd, &ps);
		;

	}break;
	case WM_DESTROY: {
		is_log_wnd_active.i16[MKLOG_ID_TYPE_SCAT] = L_OFF;
		set_logstatus(type, (get_logstatus(type) & ~MKLOG_CODE_LOG_REC_AND_CHART));
		delete_wnd_objects(type);
		//PostQuitMessage(0);
	}break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return S_OK;
}
LRESULT CALLBACK CMKLog::TrapLogWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

	HINSTANCE hInst = GetModuleHandle(0);

	int type = MKLOG_ID_TYPE_TRAP;
	switch (message)
	{

	case WM_CREATE: {
		is_log_wnd_active.i16[MKLOG_ID_TYPE_TRAP] = L_ON;
		InitCommonControls();//コモンコントロール初期化
		//ウィンドウにコントロール追加

	}break;
	case WM_COMMAND: {
		int wmId = LOWORD(wParam);
		// 選択されたメニューの解析:
		switch (wmId)
		{
		case MKLOG_ID_WND_CTRL_TRAP :

			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}break;
	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
	}break;
	case WM_DESTROY: {
		is_log_wnd_active.i16[MKLOG_ID_TYPE_TRAP] = L_OFF;
		//PostQuitMessage(0);
	}break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return S_OK;
}

HWND CMKLog::open_log_event(HWND hwnd_parent) {

	HINSTANCE hInst = GetModuleHandle(0);
	WNDCLASSEX wc;

	int type = MKLOG_ID_TYPE_EVENT;

	ZeroMemory(&wc, sizeof(wc));
	wc.cbSize = sizeof(WNDCLASSEX);

	if (st_work_wnd.hwnd[type] != NULL) {
		DestroyWindow(st_work_wnd.hwnd[type]);
		st_work_wnd.hwnd[type] = NULL;
		return NULL;
	}

	wc.lpfnWndProc = EventLogWndProc;// !CALLBACKでreturnを返していないとWindowClassの登録に失敗する
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInst;
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = TEXT("MKLogEVENT");
	wc.hIconSm = NULL;
	ATOM fb = RegisterClassExW(&wc);

	//Windowクリエイト
	st_work_wnd.hwnd[type] = CreateWindow(TEXT("MKLogEVENT"),
		TEXT("MKLOG EVENT"),
		WS_POPUPWINDOW | WS_VISIBLE | WS_CAPTION,
		st_work_wnd.pt_wnd[type].x, st_work_wnd.pt_wnd[type].y, st_work_wnd.size_wnd[type].cx, st_work_wnd.size_wnd[type].cy,
		hwnd_parent,
		0,
		hInst,
		NULL);

	//Windowを表示
	ShowWindow(st_work_wnd.hwnd[type], SW_SHOW);
	UpdateWindow(st_work_wnd.hwnd[type]);

	return st_work_wnd.hwnd[type];
};
HWND CMKLog::open_log_time(HWND hwnd_parent) {

	HINSTANCE hInst = GetModuleHandle(0);
	WNDCLASSEX wc;

	int type = MKLOG_ID_TYPE_TIME;

	ZeroMemory(&wc, sizeof(wc));
	wc.cbSize = sizeof(WNDCLASSEX);

	if (st_work_wnd.hwnd[type] != NULL) {
		DestroyWindow(st_work_wnd.hwnd[type]);
		st_work_wnd.hwnd[type] = NULL;
		return NULL;
	}

	wc.lpfnWndProc = TimeLogWndProc;// !CALLBACKでreturnを返していないとWindowClassの登録に失敗する
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInst;
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = TEXT("MKLogTime");
	wc.hIconSm = NULL;
	ATOM fb = RegisterClassExW(&wc);

	//Windowクリエイト
	st_work_wnd.hwnd[type]= st_timelog_wnd.hwnd_obj[ID_MKLOG_ITEM_TMCHART_FRAME] = CreateWindow(TEXT("MKLogTime"),
		TEXT("MKLOG TIME"),
		WS_POPUPWINDOW | WS_VISIBLE | WS_CAPTION,
		st_timelog_wnd.pt_obj[ID_MKLOG_ITEM_TMCHART_FRAME].x, st_timelog_wnd.pt_obj[ID_MKLOG_ITEM_TMCHART_FRAME].y, st_timelog_wnd.size_obj[ID_MKLOG_ITEM_TMCHART_FRAME].cx, st_timelog_wnd.size_obj[ID_MKLOG_ITEM_TMCHART_FRAME].cy,
		hwnd_parent,
		0,
		hInst,
		NULL);

	//Windowを表示
	ShowWindow(st_work_wnd.hwnd[type], SW_SHOW);
	UpdateWindow(st_work_wnd.hwnd[type]);

	return st_work_wnd.hwnd[type];
};
HWND CMKLog::open_log_scat(HWND hwnd_parent) {

	HINSTANCE hInst = GetModuleHandle(0);
	WNDCLASSEX wc;

	int type=MKLOG_ID_TYPE_SCAT;

	ZeroMemory(&wc, sizeof(wc));
	wc.cbSize = sizeof(WNDCLASSEX);

	if (st_work_wnd.hwnd[type] != NULL) {
		DestroyWindow(st_work_wnd.hwnd[type]);
		st_work_wnd.hwnd[type] = NULL;
		return NULL;
	}

	wc.lpfnWndProc = ScatLogWndProc;// !CALLBACKでreturnを返していないとWindowClassの登録に失敗する
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInst;
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = TEXT("MKLogScat");
	wc.hIconSm = NULL;
	ATOM fb = RegisterClassExW(&wc);

	//Windowクリエイト
	st_work_wnd.hwnd[type] = CreateWindow(TEXT("MKLogScat"),
		TEXT("MKLOG SCAT"),
		WS_POPUPWINDOW | WS_VISIBLE | WS_CAPTION,
		st_work_wnd.pt_wnd[type].x, st_work_wnd.pt_wnd[type].y, st_work_wnd.size_wnd[type].cx, st_work_wnd.size_wnd[type].cy,
		hwnd_parent,
		0,
		hInst,
		NULL);

	//Windowを表示
	ShowWindow(st_work_wnd.hwnd[type], SW_SHOW);
	UpdateWindow(st_work_wnd.hwnd[type]);

	return st_work_wnd.hwnd[type];
};
HWND CMKLog::open_log_trap(HWND hwnd_parent) {

	HINSTANCE hInst = GetModuleHandle(0);
	WNDCLASSEX wc;

	int type = MKLOG_ID_TYPE_TRAP;

	ZeroMemory(&wc, sizeof(wc));
	wc.cbSize = sizeof(WNDCLASSEX);

	if (st_work_wnd.hwnd[type] != NULL) {
		DestroyWindow(st_work_wnd.hwnd[type]);
		st_work_wnd.hwnd[type] = NULL;
		return NULL;
	}

	wc.lpfnWndProc = TrapLogWndProc;// !CALLBACKでreturnを返していないとWindowClassの登録に失敗する
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInst;
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = TEXT("MKLogTrap");
	wc.hIconSm = NULL;
	ATOM fb = RegisterClassExW(&wc);

	//Windowクリエイト
	st_work_wnd.hwnd[type] = CreateWindow(TEXT("MKLogTrap"),
		TEXT("MKLOG TRAP"),
		WS_POPUPWINDOW | WS_VISIBLE | WS_CAPTION,
		st_work_wnd.pt_wnd[type].x, st_work_wnd.pt_wnd[type].y, st_work_wnd.size_wnd[type].cx, st_work_wnd.size_wnd[type].cy,
		hwnd_parent,
		0,
		hInst,
		NULL);

	//Windowを表示
	ShowWindow(st_work_wnd.hwnd[type], SW_SHOW);
	UpdateWindow(st_work_wnd.hwnd[type]);

	return st_work_wnd.hwnd[type];
};

int CMKLog::close_log(int logID) {

	DestroyWindow(st_work_wnd.hwnd[logID]);
	st_work_wnd.hwnd[logID] = st_timelog_wnd.hwnd_obj[ID_MKLOG_ITEM_TMCHART_FRAME] = NULL;

	return 0;
}

static tm rectm;
static time_t rectime;

 unsigned __stdcall CMKLog::logfileout(void* params) {

	 UINT32 type = *(PUINT32)params;

	 SYSTEMTIME time; GetSystemTime(&time);

	 std::wostringstream filepath;

	 LPST_LOG_HEADER phead;
	 LPST_LOG_RECORD prec;

	 //######## 出力したいファイル名を指定する
	 filepath << LOGFOLDER_OF_MKLOG;
	 switch (type) {
	 case MKLOG_ID_TYPE_TIME: {
		 filepath << FILECAPS_OF_MKLOG_TIME;
	 }break;
	 case MKLOG_ID_TYPE_TRAP: {
		 filepath << FILECAPS_OF_MKLOG_TRAP;
	 }break;
	 case MKLOG_ID_TYPE_EVENT: {
		 filepath << FILECAPS_OF_MKLOG_EVENT;
	 }break;
	 case MKLOG_ID_TYPE_SCAT: {
		 filepath << FILECAPS_OF_MKLOG_SCAT;
	 }break;
	 default: return 1; break;
	 }
	 auto now = std::chrono::system_clock::now();
	 time_t time_now = std::chrono::system_clock::to_time_t(now);
	 tm local_time; localtime_s(&local_time ,&time_now);
	 wchar_t buf[14];
	 std::wcsftime(buf, sizeof(buf), L"%m%d%H%M%S", &local_time);
	 filepath << buf << L".csv";

	 //#########   出力したいファイルを開く　#########
	 std::wofstream outputfile(filepath.str().c_str());

	 //#########    ヘッダ部を書き込む
	 int nitem_header = logbuf[type].n_item + MKLOG_INDEX_LOG_DATA0;
	 int Nitem = logbuf[type].n_item;
	 phead = &logbuf[type].header;
 
	 //ROW1

	 for (int i = 0; i < MKLOG_INDEX_LOG_DATA0; i++) {
		 outputfile << phead->code[i] << L",";
	 }
	 //CODE
	 for (int i = 0; i < Nitem; i++) {	//codeは下位BYTEがType code
		 outputfile << (0x00ff & phead->code[i + MKLOG_INDEX_LOG_DATA0]) << L",";
	 }
	 outputfile <<  std::endl;
 
	 //ROW2	 
	 outputfile << type << L"," << st_tm_chart_ctrl.scan_time_ms << L",";
	 for (int i = 0; i < Nitem; i++) {
		 outputfile << pdb->item[phead->code[i + MKLOG_INDEX_LOG_DATA0]].d100 << L",";
	 }
	 outputfile << std::endl;

	 //ROW3	
	 //Title
	 outputfile << L"Time" << L"," << L"COUNT" << L",";
	 for (int i = 0; i < Nitem; i++) {
		 outputfile << pdb->item[phead->code[i + MKLOG_INDEX_LOG_DATA0]].title << L",";
	 }
	 outputfile << std::endl;
	
	 //########   データ部を書き込む #########

	 prec = logbuf[type].records;

	 wchar_t formatted_time[26];

	 switch (type) {
	 case MKLOG_ID_TYPE_TIME: {
		 //一周していないケース
		 if ((logbuf[type].records[logbuf[type].iw].id == 0)||(logbuf[type].iw >= MKLOG_N_LOG_RECORD - 1)) {
			 for (int i = 0; i < logbuf[type].iw-1; i++) {
				 rectime = (prec + i)->time;
				 localtime_s(&rectm, &rectime);

//				 wcsftime(formatted_time, sizeof(formatted_time), L"%m%d%H:%M:%S", &rectm);
				 wcsftime(formatted_time, sizeof(formatted_time), L"%H:%M:%S", &rectm);
				 outputfile << formatted_time << L"," ;

				 outputfile << (prec + i)->id << L",";
				 for (int k = MKLOG_INDEX_LOG_DATA0; k < Nitem + MKLOG_INDEX_LOG_DATA0; k++) {
					 outputfile << (prec + i)->data[k] << L",";
				 }
				 outputfile << std::endl;
			 }
		 }
		 //一周しているケース
		 else {
			 for (int i = logbuf[type].iw; i < MKLOG_N_LOG_RECORD; i++) {

				 outputfile << (prec + i)->time << L"," << (prec + i)->id << L",";
				 for (int k = MKLOG_INDEX_LOG_DATA0; k < Nitem + MKLOG_INDEX_LOG_DATA0; k++) {
					 outputfile << (prec + i)->data[k] << L",";
				 }
				 outputfile << std::endl;
			 }

			 for (int i = 0; i < logbuf[type].iw; i++) {

				 outputfile << (prec + i)->time << L"," << (prec + i)->id << L",";
				 for (int k = MKLOG_INDEX_LOG_DATA0; k < Nitem + MKLOG_INDEX_LOG_DATA0; k++) {
					 outputfile << (prec + i)->data[k] << L",";
				 }
				 outputfile << std::endl;
			 }
		 }
	 }break;
	 case MKLOG_ID_TYPE_TRAP: {

	 }break;
	 case MKLOG_ID_TYPE_EVENT: {

	 }break;
	 case MKLOG_ID_TYPE_SCAT: {
		 //一周していないケース
		 if ((logbuf[type].records[logbuf[type].iw].id == 0) || (logbuf[type].iw >= MKLOG_N_LOG_RECORD - 1)) {//次書き込みポイントの項目ID　0か末尾レコードの時
			 for (int i = 0; i < logbuf[type].iw - 1; i++) {
				 rectime = (prec + i)->time;
				 localtime_s(&rectm, &rectime);

				 //				 wcsftime(formatted_time, sizeof(formatted_time), L"%m%d%H:%M:%S", &rectm);
				 wcsftime(formatted_time, sizeof(formatted_time), L"%H:%M:%S", &rectm);
				 outputfile << formatted_time << L",";

				 outputfile << (prec + i)->id << L",";
				 for (int k = MKLOG_INDEX_LOG_DATA0; k < Nitem + MKLOG_INDEX_LOG_DATA0; k++) {
					 outputfile << (prec + i)->data[k] << L",";
				 }
				 outputfile << std::endl;
			 }
		 }
		 //一周しているケース
		 else {
			 for (int i = logbuf[type].iw; i < MKLOG_N_LOG_RECORD; i++) {

				 outputfile << (prec + i)->time << L"," << (prec + i)->id << L",";
				 for (int k = MKLOG_INDEX_LOG_DATA0; k < Nitem + MKLOG_INDEX_LOG_DATA0; k++) {
					 outputfile << (prec + i)->data[k] << L",";
				 }
				 outputfile << std::endl;
			 }

			 for (int i = 0; i < logbuf[type].iw; i++) {

				 outputfile << (prec + i)->time << L"," << (prec + i)->id << L",";
				 for (int k = MKLOG_INDEX_LOG_DATA0; k < Nitem + MKLOG_INDEX_LOG_DATA0; k++) {
					 outputfile << (prec + i)->data[k] << L",";
				 }
				 outputfile << std::endl;
			 }
		 }
	 }break;
	 default: return 1; break;
	 }

	 //#########   ファイルを閉じる　#########
	 outputfile.close();



	return 0;
}

/// <summary>
/// ウィンドウにコントロール追加
/// </summary>
/// <param name="hWnd"></param>
/// <returns></returns>
int CMKLog::setup_timelog_wnd(HWND hWnd) {

	InitCommonControls();//コモンコントロール初期化
	HINSTANCE hInst = GetModuleHandle(0);

	//ウィンドウにコントロール追加

	//ID_MKLOG_ITEM_TMCHART_CB_ACT	Actチェックボックス
	int item = ID_MKLOG_ITEM_TMCHART_CB_ACT;
	st_timelog_wnd.hwnd_obj[item] = CreateWindowW(TEXT("BUTTON"), st_timelog_wnd.tx_obj[item], WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
		st_timelog_wnd.pt_obj[item].x, st_timelog_wnd.pt_obj[item].y, st_timelog_wnd.size_obj[item].cx, st_timelog_wnd.size_obj[item].cy,
		hWnd, (HMENU)((LONGLONG)(MKLOG_ID_WND_CTRL_TIME + item)), hInst, NULL);

	//PB
	for (item = ID_MKLOG_ITEM_TMCHART_PB_FILE; item <= ID_MKLOG_ITEM_TMCHART_PB_F2YN10; item++) {
		st_timelog_wnd.hwnd_obj[item] = CreateWindowW(TEXT("BUTTON"), st_timelog_wnd.tx_obj[item], WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_PUSHLIKE,
			st_timelog_wnd.pt_obj[item].x, st_timelog_wnd.pt_obj[item].y, st_timelog_wnd.size_obj[item].cx, st_timelog_wnd.size_obj[item].cy,
			hWnd, (HMENU)((LONGLONG)(MKLOG_ID_WND_CTRL_TIME + item)), hInst, NULL);
	}

	//STATIC
	for (item = ID_MKLOG_ITEM_TMCHART_ST_INF; item <= ID_MKLOG_ITEM_TMCHART_ST_F2CODE8; item++) {
		st_timelog_wnd.hwnd_obj[item] = CreateWindowW(TEXT("STATIC"), st_timelog_wnd.tx_obj[item], WS_CHILD | WS_VISIBLE | SS_LEFT,
			st_timelog_wnd.pt_obj[item].x, st_timelog_wnd.pt_obj[item].y, st_timelog_wnd.size_obj[item].cx, st_timelog_wnd.size_obj[item].cy,
			hWnd, (HMENU)((LONGLONG)(MKLOG_ID_WND_CTRL_TIME + item)), hInst, NULL);

		//FONT設定
		SendMessage(st_timelog_wnd.hwnd_obj[item], WM_SETFONT, (WPARAM)st_work_wnd.hfont[ID_MKLOG_FONT12], TRUE);
	}

	for (item = ID_MKLOG_ITEM_TMCHART_ST_LABEL_X; item <= ID_MKLOG_ITEM_TMCHART_ST_LABEL_F2Y; item++) {
		st_timelog_wnd.hwnd_obj[item] = CreateWindowW(TEXT("STATIC"), st_timelog_wnd.tx_obj[item], WS_CHILD | WS_VISIBLE | SS_CENTER,
			st_timelog_wnd.pt_obj[item].x, st_timelog_wnd.pt_obj[item].y, st_timelog_wnd.size_obj[item].cx, st_timelog_wnd.size_obj[item].cy,
			hWnd, (HMENU)((LONGLONG)(MKLOG_ID_WND_CTRL_TIME + item)), hInst, NULL);

		//FONT設定
		SendMessage(st_timelog_wnd.hwnd_obj[item], WM_SETFONT, (WPARAM)st_work_wnd.hfont[ID_MKLOG_FONT20], TRUE);
	}

	//EDITBOX
	for (item = ID_MKLOG_ITEM_TMCHART_ED_F1CODE1; item <= ID_MKLOG_ITEM_TMCHART_ED_F2CODE8; item++) {
		st_timelog_wnd.hwnd_obj[item] = CreateWindowW(TEXT("edit"), st_timelog_wnd.tx_obj[item], WS_CHILD | WS_VISIBLE | ES_LEFT | WS_BORDER,
			st_timelog_wnd.pt_obj[item].x, st_timelog_wnd.pt_obj[item].y, st_timelog_wnd.size_obj[item].cx, st_timelog_wnd.size_obj[item].cy,
			hWnd, (HMENU)((LONGLONG)(MKLOG_ID_WND_CTRL_TIME + item)), hInst, NULL);
	}

return 0;

}

/// <summary>
/// ウィンドウにコントロール追加
/// </summary>
/// <param name="hWnd"></param>
/// <returns></returns>
int CMKLog::setup_scatlog_wnd(HWND hWnd) {

	InitCommonControls();//コモンコントロール初期化
	HINSTANCE hInst = GetModuleHandle(0);

	//ID_MKLOG_ITEM_TMCHART_CB_ACT	Actチェックボックス
	int item = ID_MKLOG_ITEM_SCCHART_CB_ACT;
	st_scatlog_wnd.hwnd_obj[item] = CreateWindowW(TEXT("BUTTON"), st_scatlog_wnd.tx_obj[item], WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
		st_scatlog_wnd.pt_obj[item].x, st_scatlog_wnd.pt_obj[item].y, st_scatlog_wnd.size_obj[item].cx, st_scatlog_wnd.size_obj[item].cy,
		hWnd, (HMENU)((LONGLONG)(MKLOG_ID_WND_CTRL_SCAT + item)), hInst, NULL);

	//PB
	for (item = ID_MKLOG_ITEM_SCCHART_PB_FILE; item <= ID_MKLOG_ITEM_SCCHART_PB_F2YN10; item++) {
		st_scatlog_wnd.hwnd_obj[item] = CreateWindowW(TEXT("BUTTON"), st_scatlog_wnd.tx_obj[item], WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_PUSHLIKE,
			st_scatlog_wnd.pt_obj[item].x, st_scatlog_wnd.pt_obj[item].y, st_scatlog_wnd.size_obj[item].cx, st_scatlog_wnd.size_obj[item].cy,
			hWnd, (HMENU)((LONGLONG)(MKLOG_ID_WND_CTRL_SCAT + item)), hInst, NULL);
	}

	//STATIC

	for (item = ID_MKLOG_ITEM_SCCHART_ST_INF; item <= ID_MKLOG_ITEM_SCCHART_ST_DISPTIME; item++) {
		st_scatlog_wnd.hwnd_obj[item] = CreateWindowW(TEXT("STATIC"), st_scatlog_wnd.tx_obj[item], WS_CHILD | WS_VISIBLE | SS_LEFT,
			st_scatlog_wnd.pt_obj[item].x, st_scatlog_wnd.pt_obj[item].y, st_scatlog_wnd.size_obj[item].cx, st_scatlog_wnd.size_obj[item].cy,
			hWnd, (HMENU)((LONGLONG)(MKLOG_ID_WND_CTRL_SCAT + item)), hInst, NULL);
		//FONT設定
		SendMessage(st_scatlog_wnd.hwnd_obj[item], WM_SETFONT, (WPARAM)st_work_wnd.hfont[ID_MKLOG_FONT12], TRUE);
	}

	wos.str(L""); wos << (st_sc_chart_ctrl.plot_disp_range * st_sc_chart_ctrl.scan_time_ms) << L" ms";

	SetWindowText(st_scatlog_wnd.hwnd_obj[ID_MKLOG_ITEM_SCCHART_ST_DISPTIME], wos.str().c_str());


	for (item = ID_MKLOG_ITEM_SCCHART_ST_LABEL_FX1; item <= ID_MKLOG_ITEM_SCCHART_ST_LABEL_DISPTIME; item++) {
		st_scatlog_wnd.hwnd_obj[item] = CreateWindowW(TEXT("STATIC"), st_scatlog_wnd.tx_obj[item], WS_CHILD | WS_VISIBLE | SS_CENTER,
			st_scatlog_wnd.pt_obj[item].x, st_scatlog_wnd.pt_obj[item].y, st_scatlog_wnd.size_obj[item].cx, st_scatlog_wnd.size_obj[item].cy,
			hWnd, (HMENU)((LONGLONG)(MKLOG_ID_WND_CTRL_SCAT + item)), hInst, NULL);

		//FONT設定
		SendMessage(st_scatlog_wnd.hwnd_obj[item], WM_SETFONT, (WPARAM)st_work_wnd.hfont[ID_MKLOG_FONT12], TRUE);
	}


	//EDITBOX
	for (item = ID_MKLOG_ITEM_SCCHART_ED_F1CODE1; item <= ID_MKLOG_ITEM_SCCHART_ED_F2CODE8; item++) {
		st_scatlog_wnd.hwnd_obj[item] = CreateWindowW(TEXT("edit"), st_scatlog_wnd.tx_obj[item], WS_CHILD | WS_VISIBLE | ES_LEFT | WS_BORDER,
			st_scatlog_wnd.pt_obj[item].x, st_scatlog_wnd.pt_obj[item].y, st_scatlog_wnd.size_obj[item].cx, st_scatlog_wnd.size_obj[item].cy,
			hWnd, (HMENU)((LONGLONG)(MKLOG_ID_WND_CTRL_SCAT + item)), hInst, NULL);
	}

	return 0;

}

/// <summary>
///  描画用ビットマップ,デバイスコンテキスト,Gdiグラフィック生成
/// </summary>
/// <param name="hWnd"></param>
void CMKLog::create_wnd_objects(int log_type) {

	switch (log_type) {
	case MKLOG_ID_TYPE_TIME: {

		for (int i = 0; i < MKLOG_N_HDC; i++)	if (st_timelog_wnd.hdc[i] != NULL)DeleteDC(st_timelog_wnd.hdc[i]);
		for (int i = 0; i < MKLOG_N_HBMAP; i++) if (st_timelog_wnd.hBmp[i] != NULL)DeleteObject(st_timelog_wnd.hBmp[i]);

		HDC hdc = GetDC(st_timelog_wnd.hwnd_obj[ID_MKLOG_ITEM_TMCHART_FRAME]);

		for (int i = ID_MKLOG_HDC_MEM0; i <= ID_MKLOG_GR_MEM_INFO; i++) {
			//bit mapの領域はウィンドよりも大きく取って冗長部分にも描画できるようにすること！！
			st_timelog_wnd.hBmp[i] = CreateCompatibleBitmap(hdc, st_timelog_wnd.size_obj[ID_MKLOG_ITEM_TMCHART_MEM_GRAPH1].cx, st_timelog_wnd.size_obj[ID_MKLOG_ITEM_TMCHART_MEM_GRAPH1].cy * 2);
			st_timelog_wnd.hdc[i] = CreateCompatibleDC(hdc);
			SelectObject(st_timelog_wnd.hdc[i], st_timelog_wnd.hBmp[i]);
			st_timelog_wnd.pgraphic[i] = new Gdiplus::Graphics(st_timelog_wnd.hdc[i]);
			PatBlt(st_timelog_wnd.hdc[i], 0, 0, st_timelog_wnd.size_obj[ID_MKLOG_ITEM_TMCHART_MEM_GRAPH1].cx, st_timelog_wnd.size_obj[ID_MKLOG_ITEM_TMCHART_MEM_GRAPH1].cy * 2, WHITENESS);
		}

		ReleaseDC(st_timelog_wnd.hwnd_obj[ID_MKLOG_ITEM_TMCHART_FRAME], hdc);

	}break;
	case MKLOG_ID_TYPE_TRAP: {

	}break;
	case MKLOG_ID_TYPE_EVENT: {

	}break;
	case 	MKLOG_ID_TYPE_SCAT: {
		for (int i = 0; i < MKLOG_N_HDC; i++)	if (st_scatlog_wnd.hdc[i] != NULL)DeleteDC(st_scatlog_wnd.hdc[i]);
		for (int i = 0; i < MKLOG_N_HBMAP; i++) if (st_scatlog_wnd.hBmp[i] != NULL)DeleteObject(st_scatlog_wnd.hBmp[i]);

		HDC hdc = GetDC(st_scatlog_wnd.hwnd_obj[ID_MKLOG_ITEM_SCCHART_FRAME]);

		for (int i = ID_MKLOG_HDC_MEM0; i <= ID_MKLOG_GR_MEM_INFO; i++) {
			//bit mapの領域はウィンドよりも大きく取って冗長部分にも描画できるようにすること！！
			st_scatlog_wnd.hBmp[i] = CreateCompatibleBitmap(hdc, st_scatlog_wnd.size_obj[ID_MKLOG_ITEM_SCCHART_MEM_GRAPH1].cx, st_scatlog_wnd.size_obj[ID_MKLOG_ITEM_SCCHART_MEM_GRAPH1].cy * 2);
			st_scatlog_wnd.hdc[i] = CreateCompatibleDC(hdc);
			SelectObject(st_scatlog_wnd.hdc[i], st_scatlog_wnd.hBmp[i]);
			st_scatlog_wnd.pgraphic[i] = new Gdiplus::Graphics(st_scatlog_wnd.hdc[i]);
			PatBlt(st_scatlog_wnd.hdc[i], 0, 0, st_scatlog_wnd.size_obj[ID_MKLOG_ITEM_SCCHART_MEM_GRAPH1].cx, st_scatlog_wnd.size_obj[ID_MKLOG_ITEM_SCCHART_MEM_GRAPH1].cy * 2, WHITENESS);
		}
		ReleaseDC(st_scatlog_wnd.hwnd_obj[ID_MKLOG_ITEM_SCCHART_FRAME], hdc);
	}break;
	case MKLOG_ID_TYPE_ALL:
	default: {
		for (int i = ID_MKLOG_COLOR_BLACK; i <= ID_MKLOG_COLOR_YELLOW; i++) {
			st_work_wnd.hbrush[i] = CreateSolidBrush(st_work_wnd.color_pallet[i]);
			st_work_wnd.hpen[i] = CreatePen(PS_SOLID, 2, st_work_wnd.color_pallet[i]);
		}

		GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

		//gdi+
		st_work_wnd.pbrush[ID_MKLOG_COLOR_BLACK] = new SolidBrush(Gdiplus::Color(DEF_MKLOG_COLOR_BLACK));
		st_work_wnd.pbrush[ID_MKLOG_COLOR_WHITE] = new SolidBrush(Gdiplus::Color(DEF_MKLOG_COLOR_WHITE));
		st_work_wnd.pbrush[ID_MKLOG_COLOR_GLAY_BK] = new SolidBrush(Gdiplus::Color(DEF_MKLOG_COLOR_GLAY_BK));
		st_work_wnd.pbrush[ID_MKLOG_COLOR_RED] = new SolidBrush(Gdiplus::Color(DEF_MKLOG_COLOR_RED));
		st_work_wnd.pbrush[ID_MKLOG_COLOR_ORANGE] = new SolidBrush(Gdiplus::Color(DEF_MKLOG_COLOR_ORANGE));
		st_work_wnd.pbrush[ID_MKLOG_COLOR_BLUE] = new SolidBrush(Gdiplus::Color(DEF_MKLOG_COLOR_BLUE));
		st_work_wnd.pbrush[ID_MKLOG_COLOR_GREEN] = new SolidBrush(Gdiplus::Color(DEF_MKLOG_COLOR_GREEN));
		st_work_wnd.pbrush[ID_MKLOG_COLOR_CYAN] = new SolidBrush(Gdiplus::Color(DEF_MKLOG_COLOR_CYAN));
		st_work_wnd.pbrush[ID_MKLOG_COLOR_PURPLE] = new SolidBrush(Gdiplus::Color(DEF_MKLOG_COLOR_PURPLE));
		st_work_wnd.pbrush[ID_MKLOG_COLOR_BROWN] = new SolidBrush(Gdiplus::Color(DEF_MKLOG_COLOR_BROWN));
		st_work_wnd.pbrush[ID_MKLOG_COLOR_MAZENDA] = new SolidBrush(Gdiplus::Color(DEF_MKLOG_COLOR_MAZENDA));
		st_work_wnd.pbrush[ID_MKLOG_COLOR_YELLOW] = new SolidBrush(Gdiplus::Color(DEF_MKLOG_COLOR_YELLOW));


		st_work_wnd.ppen[ID_MKLOG_COLOR_BLACK] = new Pen(Gdiplus::Color(DEF_MKLOG_COLOR_BLACK0));
		st_work_wnd.ppen[ID_MKLOG_COLOR_WHITE] = new Pen(Gdiplus::Color(DEF_MKLOG_COLOR_WHITE));
		st_work_wnd.ppen[ID_MKLOG_COLOR_GLAY_BK] = new Pen(Gdiplus::Color(DEF_MKLOG_COLOR_GLAY_BK));
		st_work_wnd.ppen[ID_MKLOG_COLOR_RED] = new Pen(Gdiplus::Color(DEF_MKLOG_COLOR_RED));
		st_work_wnd.ppen[ID_MKLOG_COLOR_ORANGE] = new Pen(Gdiplus::Color(DEF_MKLOG_COLOR_ORANGE));
		st_work_wnd.ppen[ID_MKLOG_COLOR_BLUE] = new Pen(Gdiplus::Color(DEF_MKLOG_COLOR_BLUE));
		st_work_wnd.ppen[ID_MKLOG_COLOR_GREEN] = new Pen(Gdiplus::Color(DEF_MKLOG_COLOR_GREEN));
		st_work_wnd.ppen[ID_MKLOG_COLOR_CYAN] = new Pen(Gdiplus::Color(DEF_MKLOG_COLOR_CYAN));
		st_work_wnd.ppen[ID_MKLOG_COLOR_PURPLE] = new Pen(Gdiplus::Color(DEF_MKLOG_COLOR_PURPLE));
		st_work_wnd.ppen[ID_MKLOG_COLOR_BROWN] = new Pen(Gdiplus::Color(DEF_MKLOG_COLOR_BROWN));
		st_work_wnd.ppen[ID_MKLOG_COLOR_MAZENDA] = new Pen(Gdiplus::Color(DEF_MKLOG_COLOR_MAZENDA));
		st_work_wnd.ppen[ID_MKLOG_COLOR_YELLOW] = new Pen(Gdiplus::Color(DEF_MKLOG_COLOR_YELLOW));

		// カスタム破線のペンを作成
		st_work_wnd.ppen[ID_MKLOG_PEN_DASH_GRAY] = new Pen(Gdiplus::Color(DEF_MKLOG_COLOR_DASH_GRAY));
		float dashPattern[] = { 5.0f, 2.0f, 5.0f, 2.0f };
		st_work_wnd.ppen[ID_MKLOG_PEN_DASH_GRAY]->SetDashPattern(dashPattern, sizeof(dashPattern)/ sizeof(float));


		//表示フォント設定
		st_work_wnd.hfont[ID_MKLOG_FONT8] = CreateFont(8, 0, 0, 0, 0, FALSE, FALSE, FALSE, SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, FIXED_PITCH | FF_MODERN, TEXT("Arial"));
		st_work_wnd.hfont[ID_MKLOG_FONT6] = CreateFont(6, 0, 0, 0, 0, FALSE, FALSE, FALSE, SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, FIXED_PITCH | FF_MODERN, TEXT("Arial"));
		st_work_wnd.hfont[ID_MKLOG_FONT12] = CreateFont(12, 0, 0, 0, 0, FALSE, FALSE, FALSE, SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, FIXED_PITCH | FF_MODERN, TEXT("Arial"));
		st_work_wnd.hfont[ID_MKLOG_FONT20] = CreateFont(20, 0, 0, 0, 0, FALSE, FALSE, FALSE, SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, FIXED_PITCH | FF_MODERN, TEXT("Arial"));
		st_work_wnd.hfont[ID_MKLOG_FONT10] = CreateFont(10, 0, 0, 0, 0, FALSE, FALSE, FALSE, SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, FIXED_PITCH | FF_MODERN, TEXT("Arial"));

	}break;
	}

}
void CMKLog::delete_wnd_objects(int log_type) {

	switch (log_type) {
	case MKLOG_ID_TYPE_TIME: {
		for (int i = 0; i < MKLOG_N_HDC; i++) {
			if(st_timelog_wnd.hdc[i] != NULL)DeleteDC(st_timelog_wnd.hdc[i]);
			st_timelog_wnd.hdc[i] = NULL;
		}
		for (int i = 0; i < MKLOG_N_HBMAP; i++) {
			if (st_timelog_wnd.hBmp[i] != NULL)DeleteObject(st_timelog_wnd.hBmp[i]);
			st_timelog_wnd.hBmp[i] = NULL;
		}
	}break;
	case MKLOG_ID_TYPE_TRAP: {

	}break;
	case MKLOG_ID_TYPE_EVENT: {

	}break;
	case 	MKLOG_ID_TYPE_SCAT: {
		for (int i = 0; i < MKLOG_N_HDC; i++) {
			if (st_scatlog_wnd.hdc[i] != NULL)DeleteDC(st_scatlog_wnd.hdc[i]);
			st_scatlog_wnd.hdc[i] = NULL;
		}
		for (int i = 0; i < MKLOG_N_HBMAP; i++) {
			if (st_scatlog_wnd.hBmp[i] != NULL)DeleteObject(st_scatlog_wnd.hBmp[i]);
			st_scatlog_wnd.hBmp[i] = NULL;
		}

	}break;

	case MKLOG_ID_TYPE_ALL:
	default: {
		for (int i = ID_MKLOG_COLOR_BLACK; i <= MKLOG_N_BRUSH; i++) {
			if(st_work_wnd.hbrush[i] !=NULL)			DeleteObject(st_work_wnd.hbrush[i]);
			if (st_work_wnd.pbrush[i] != NULL)			delete st_work_wnd.pbrush[i];
			if (st_work_wnd.hpen[i] != NULL)			DeleteObject(st_work_wnd.hpen[i]);
			if (st_work_wnd.ppen[i] != NULL)			delete st_work_wnd.ppen[i];
		}
	}break;

	}

}

/// グラフィック背景描画
/// 
/// </summary>
/// <param name="log_type"></param>
void CMKLog::draw_bg(int log_type) {

	switch (log_type) {
	case MKLOG_ID_TYPE_TIME: {

		//グラフエリア
		st_timelog_wnd.pgraphic[ID_MKLOG_GR_BG]->FillRectangle(st_work_wnd.pbrush[ID_MKLOG_COLOR_BLACK],
			(INT)st_timelog_wnd.pt_obj[ID_MKLOG_ITEM_TMCHART_MEM_GRAPH1].x, (INT)st_timelog_wnd.pt_obj[ID_MKLOG_ITEM_TMCHART_MEM_GRAPH1].y,
			(INT)st_timelog_wnd.size_obj[ID_MKLOG_ITEM_TMCHART_MEM_GRAPH1].cx, (INT)st_timelog_wnd.size_obj[ID_MKLOG_ITEM_TMCHART_MEM_GRAPH1].cy
		);

		st_timelog_wnd.pgraphic[ID_MKLOG_GR_BG]->FillRectangle(st_work_wnd.pbrush[ID_MKLOG_COLOR_BLACK],
			(INT)st_timelog_wnd.pt_obj[ID_MKLOG_ITEM_TMCHART_MEM_GRAPH2].x, (INT)st_timelog_wnd.pt_obj[ID_MKLOG_ITEM_TMCHART_MEM_GRAPH2].y, 
			(INT)st_timelog_wnd.size_obj[ID_MKLOG_ITEM_TMCHART_MEM_GRAPH2].cx, (INT)st_timelog_wnd.size_obj[ID_MKLOG_ITEM_TMCHART_MEM_GRAPH2].cy
		);

	
	
		//Y軸目盛り
		Gdiplus::Pen* ppen = st_work_wnd.ppen[ID_MKLOG_PEN_DASH_GRAY];

		INT dy = st_tm_chart_ctrl.division_y_axis;
		INT y0 = (INT)(st_timelog_wnd.pt_obj[ID_MKLOG_ITEM_TMCHART_MEM_GRAPH1].y);

		for (int i = 1; i < st_timelog_wnd.size_obj[ID_MKLOG_ITEM_TMCHART_MEM_GRAPH1].cy/dy; i++) {
			st_timelog_wnd.pgraphic[ID_MKLOG_GR_BG]->DrawLine(ppen, st_tm_chart_ctrl.line[ID_MKLOG_TM_CHART1_X0].x1, y0 + i * dy, st_tm_chart_ctrl.line[ID_MKLOG_TM_CHART1_X0].x2, y0 + i * dy);
		}

		y0 = (INT)(st_timelog_wnd.pt_obj[ID_MKLOG_ITEM_TMCHART_MEM_GRAPH2].y);

		for (int i = 1; i < st_timelog_wnd.size_obj[ID_MKLOG_ITEM_TMCHART_MEM_GRAPH1].cy/dy; i++ ) {
			st_timelog_wnd.pgraphic[ID_MKLOG_GR_BG]->DrawLine(ppen, st_tm_chart_ctrl.line[ID_MKLOG_TM_CHART2_X0].x1, y0 + i * dy, st_tm_chart_ctrl.line[ID_MKLOG_TM_CHART2_X0].x2, y0 + i * dy);
		}

		//座標軸描画（line配列）
		ppen = st_work_wnd.ppen[ID_MKLOG_COLOR_BLACK];
		ppen->SetWidth(1.0);

		for (int i = ID_MKLOG_TM_CHART1_X0; i <= ID_MKLOG_TM_CHART2_Y0; i++) {
			st_timelog_wnd.pgraphic[ID_MKLOG_GR_BG]->DrawLine(ppen, st_tm_chart_ctrl.line[i].x1, st_tm_chart_ctrl.line[i].y1, st_tm_chart_ctrl.line[i].x2, st_tm_chart_ctrl.line[i].y2);
		}
		
		st_tm_chart_ctrl.req_bg_update = L_OFF;

	}break;
	case MKLOG_ID_TYPE_TRAP: {

	}break;
	case MKLOG_ID_TYPE_EVENT: {

	}break;
	case MKLOG_ID_TYPE_SCAT: {
		//グラフエリア
		st_scatlog_wnd.pgraphic[ID_MKLOG_GR_BG]->FillRectangle(st_work_wnd.pbrush[ID_MKLOG_COLOR_BLACK],
			(INT)st_scatlog_wnd.pt_obj[ID_MKLOG_ITEM_SCCHART_MEM_GRAPH1].x, (INT)st_scatlog_wnd.pt_obj[ID_MKLOG_ITEM_SCCHART_MEM_GRAPH1].y,
			(INT)st_scatlog_wnd.size_obj[ID_MKLOG_ITEM_SCCHART_MEM_GRAPH1].cx, (INT)st_scatlog_wnd.size_obj[ID_MKLOG_ITEM_SCCHART_MEM_GRAPH1].cy
		);

		st_scatlog_wnd.pgraphic[ID_MKLOG_GR_BG]->FillRectangle(st_work_wnd.pbrush[ID_MKLOG_COLOR_BLACK],
			(INT)st_scatlog_wnd.pt_obj[ID_MKLOG_ITEM_SCCHART_MEM_GRAPH2].x, (INT)st_scatlog_wnd.pt_obj[ID_MKLOG_ITEM_SCCHART_MEM_GRAPH2].y,
			(INT)st_scatlog_wnd.size_obj[ID_MKLOG_ITEM_SCCHART_MEM_GRAPH2].cx, (INT)st_scatlog_wnd.size_obj[ID_MKLOG_ITEM_SCCHART_MEM_GRAPH2].cy
		);

		//Y軸目盛り
		Gdiplus::Pen* ppen = st_work_wnd.ppen[ID_MKLOG_PEN_DASH_GRAY];

		INT x = 0;
		INT y0 = st_scatlog_wnd.pt_obj[ID_MKLOG_ITEM_SCCHART_MEM_GRAPH1].y;
		INT y1 = st_scatlog_wnd.pt_obj[ID_MKLOG_ITEM_SCCHART_MEM_GRAPH1].y + st_scatlog_wnd.size_obj[ID_MKLOG_ITEM_SCCHART_MEM_GRAPH1].cy;

		for (int i = 0; i <= st_scatlog_wnd.size_obj[ID_MKLOG_ITEM_SCCHART_MEM_GRAPH1].cx / st_sc_chart_ctrl.division_x_axis; i++) {
			st_scatlog_wnd.pgraphic[ID_MKLOG_GR_BG]->DrawLine(ppen, x + i * st_sc_chart_ctrl.division_x_axis, y0, x + i * st_sc_chart_ctrl.division_x_axis, y1);
		}

		x = st_sc_chart_ctrl.line[ID_MKLOG_SC_CHART2_X0].x1;
		y0 = st_scatlog_wnd.pt_obj[ID_MKLOG_ITEM_SCCHART_MEM_GRAPH2].y;
		y1 = st_scatlog_wnd.pt_obj[ID_MKLOG_ITEM_SCCHART_MEM_GRAPH2].y + st_scatlog_wnd.size_obj[ID_MKLOG_ITEM_SCCHART_MEM_GRAPH2].cy;

		for (int i = 0; i <= st_scatlog_wnd.size_obj[ID_MKLOG_ITEM_SCCHART_MEM_GRAPH2].cx / st_sc_chart_ctrl.division_x_axis; i++) {
			st_scatlog_wnd.pgraphic[ID_MKLOG_GR_BG]->DrawLine(ppen, x + i * st_sc_chart_ctrl.division_x_axis, y0, x + i * st_sc_chart_ctrl.division_x_axis, y1);
		}

		//X軸目盛り
	
		INT dy = st_sc_chart_ctrl.division_y_axis;
		y0 = (INT)(st_scatlog_wnd.pt_obj[ID_MKLOG_ITEM_SCCHART_MEM_GRAPH1].y);

		for (int i = 1; i < st_scatlog_wnd.size_obj[ID_MKLOG_ITEM_SCCHART_MEM_GRAPH1].cy / dy; i++) {
			st_scatlog_wnd.pgraphic[ID_MKLOG_GR_BG]->DrawLine(ppen, st_sc_chart_ctrl.line[ID_MKLOG_SC_CHART1_X0].x1, y0 + i * dy, st_sc_chart_ctrl.line[ID_MKLOG_SC_CHART1_X0].x2, y0 + i * dy);
		}

		y0 = (INT)(st_scatlog_wnd.pt_obj[ID_MKLOG_ITEM_SCCHART_MEM_GRAPH2].y);

		for (int i = 1; i < st_scatlog_wnd.size_obj[ID_MKLOG_ITEM_SCCHART_MEM_GRAPH1].cy / dy; i++) {
			st_scatlog_wnd.pgraphic[ID_MKLOG_GR_BG]->DrawLine(ppen, st_sc_chart_ctrl.line[ID_MKLOG_TM_CHART2_X0].x1, y0 + i * dy, st_sc_chart_ctrl.line[ID_MKLOG_TM_CHART2_X0].x2, y0 + i * dy);
		}


		//座標軸描画（line配列）
		ppen = st_work_wnd.ppen[ID_MKLOG_COLOR_BLACK];
		ppen->SetWidth(1.0);

		for (int i = ID_MKLOG_SC_CHART1_X0; i <= ID_MKLOG_SC_CHART2_Y0; i++) {
			st_scatlog_wnd.pgraphic[ID_MKLOG_GR_BG]->DrawLine(ppen, st_sc_chart_ctrl.line[i].x1, st_sc_chart_ctrl.line[i].y1, st_sc_chart_ctrl.line[i].x2, st_sc_chart_ctrl.line[i].y2);
		}

		st_sc_chart_ctrl.req_bg_update = L_OFF;

	}break;
	case MKLOG_ID_TYPE_ALL:
	default:break;

	}

	return;
}

/// メイングラフィック描画
/// 
/// </summary>
/// <param name="log_type"></param>
/// <param name="be_reflesh"></param>
void CMKLog::draw_graphic(int log_type,bool be_reflesh) {

	switch (log_type) {
	case MKLOG_ID_TYPE_TIME: {

		INT x0 = st_tm_chart_ctrl.pt_time_plot_pix;							//今回プロットする時間軸のPIXEL
		//INT y10 = st_tm_chart_ctrl.mem_offset_y[ID_MKLOG_PLOT_GRAPH1_A];	//グラフィックメモリのX軸のｙ値　グラフ1
		//INT y20 = st_tm_chart_ctrl.mem_offset_y[ID_MKLOG_PLOT_GRAPH2_A];	//グラフィックメモリのX軸のｙ値　グラフ2

		//今回のＸ軸プロット位置と前回のプロット位置の差　⇒　一周したタイミングでマイナスになる
		INT chk_x = st_tm_chart_ctrl.pt_time_plot_pix - st_tm_chart_ctrl.pt_time_plot_pix_last;
		INT plot_y=0;
	
		for (int i = ID_MKLOG_TM_CHART1_1; i <= ID_MKLOG_TM_CHART1_8; i++) {

			INT y10 = st_tm_chart_ctrl.chart_item_disp_y0[i];

			if (st_tm_chart_ctrl.plot_item_code[i]) {//設定コード!=0

				//x1,y1：前回座標点 x2,y2今回座標点
				if (chk_x < 0) {//前回値＞今回値　→　時間レンジ振切
					st_tm_chart_ctrl.line[i].x1 = x0 + chk_x;//前回x座標を
				}
				else {
					st_tm_chart_ctrl.line[i].x1 = st_tm_chart_ctrl.line[i].x2;
				}
				st_tm_chart_ctrl.line[i].x2 = x0;
				st_tm_chart_ctrl.line[i].y1 = st_tm_chart_ctrl.line[i].y2;

				//100%レンジ（SCADA値）を表示レンジに変換
				plot_y = -(INT)loghot[log_type][i + MKLOG_INDEX_LOG_DATA0] / st_tm_chart_ctrl.disp_mag_yc[ID_MKLOG_PLOT_GRAPH1_A];

				st_tm_chart_ctrl.line[i].y2 = plot_y + y10;		//y10： x軸（y＝0）PIXEL

				if (be_reflesh) {//チャート再開時は開始と終了を一致させる
					st_tm_chart_ctrl.line[i].x1 = st_tm_chart_ctrl.line[i].x2;
					st_tm_chart_ctrl.line[i].y1 = st_tm_chart_ctrl.line[i].y2;
				}

				//ライン描画
				st_timelog_wnd.pgraphic[ID_MKLOG_GR_MEM_GR]->DrawLine(st_work_wnd.ppen[st_tm_chart_ctrl.color_chart_item[i]],
					st_tm_chart_ctrl.line[i].x1, st_tm_chart_ctrl.line[i].y1, st_tm_chart_ctrl.line[i].x2, st_tm_chart_ctrl.line[i].y2);
			}
		}
		for (int i = ID_MKLOG_TM_CHART2_1; i <= ID_MKLOG_TM_CHART2_8; i++) {

			INT y20 = st_tm_chart_ctrl.chart_item_disp_y0[i];

			if (st_tm_chart_ctrl.plot_item_code[i]) {
				if (chk_x < 0) {
					st_tm_chart_ctrl.line[i].x1 = x0 + chk_x;
				}
				else {
					st_tm_chart_ctrl.line[i].x1 = st_tm_chart_ctrl.line[i].x2;
				}
				st_tm_chart_ctrl.line[i].x2 = x0;
				st_tm_chart_ctrl.line[i].y1 = st_tm_chart_ctrl.line[i].y2;
	
//				plot_y = -(INT)loghot[log_type][i + MKLOG_INDEX_LOG_DATA0] / 100;
				plot_y = -(INT)loghot[log_type][i + MKLOG_INDEX_LOG_DATA0] / st_tm_chart_ctrl.disp_mag_yc[ID_MKLOG_PLOT_GRAPH2_A];
	
				st_tm_chart_ctrl.line[i].y2 = plot_y + y20;

				if (be_reflesh) {//チャート再開時は開始と終了を一致させる
					st_tm_chart_ctrl.line[i].x1 = st_tm_chart_ctrl.line[i].x2;
					st_tm_chart_ctrl.line[i].y1 = st_tm_chart_ctrl.line[i].y2;
				}


				st_timelog_wnd.pgraphic[ID_MKLOG_GR_MEM_GR]->DrawLine(st_work_wnd.ppen[st_tm_chart_ctrl.color_chart_item[i]],
					st_tm_chart_ctrl.line[i].x1, st_tm_chart_ctrl.line[i].y1, st_tm_chart_ctrl.line[i].x2, st_tm_chart_ctrl.line[i].y2);
			}
		}
		//過去の描画部分をクリアして時間軸を描画
		INT n_area;
		if (chk_x < 0) {
			st_tm_chart_ctrl.pt_time_erase_pix = x0 - st_tm_chart_ctrl.pix_dot_size;
			n_area = 0;
		}
		else {
			st_tm_chart_ctrl.pt_time_erase_pix = x0 + st_tm_chart_ctrl.pix_dot_size;
			n_area = st_tm_chart_ctrl.pt_time_plot_pix / st_tm_chart_ctrl.division_time_axis + 1;
		}

		st_timelog_wnd.pgraphic[ID_MKLOG_GR_MEM_GR]->FillRectangle(st_work_wnd.pbrush[ID_MKLOG_COLOR_WHITE],
			st_tm_chart_ctrl.pt_time_erase_pix, (INT)st_timelog_wnd.pt_obj[ID_MKLOG_ITEM_TMCHART_MEM_GRAPH1].y,
			st_tm_chart_ctrl.division_time_axis -1, (INT)st_timelog_wnd.size_obj[ID_MKLOG_ITEM_TMCHART_MEM_GRAPH1].cy
		);
		st_timelog_wnd.pgraphic[ID_MKLOG_GR_MEM_GR]->FillRectangle(st_work_wnd.pbrush[ID_MKLOG_COLOR_WHITE],
			st_tm_chart_ctrl.pt_time_erase_pix, (INT)st_timelog_wnd.pt_obj[ID_MKLOG_ITEM_TMCHART_MEM_GRAPH2].y,
			st_tm_chart_ctrl.division_time_axis -1, (INT)st_timelog_wnd.size_obj[ID_MKLOG_ITEM_TMCHART_MEM_GRAPH2].cy
		);

		//CHART 縦軸目盛 描画
		INT div_x = st_tm_chart_ctrl.division_time_axis * n_area + st_timelog_wnd.pt_obj[ID_MKLOG_ITEM_TMCHART_MEM_GRAPH1].x;
		Gdiplus::Pen* ppen = st_work_wnd.ppen[ID_MKLOG_PEN_DASH_GRAY];
		st_timelog_wnd.pgraphic[ID_MKLOG_GR_MEM_GR]->DrawLine(ppen, 
			div_x, st_timelog_wnd.pt_obj[ID_MKLOG_ITEM_TMCHART_MEM_GRAPH1].y,
			div_x, st_timelog_wnd.pt_obj[ID_MKLOG_ITEM_TMCHART_MEM_GRAPH1].y + st_timelog_wnd.size_obj[ID_MKLOG_ITEM_TMCHART_MEM_GRAPH1].cy);
		st_timelog_wnd.pgraphic[ID_MKLOG_GR_MEM_GR]->DrawLine(ppen,
			div_x, st_timelog_wnd.pt_obj[ID_MKLOG_ITEM_TMCHART_MEM_GRAPH2].y,
			div_x, st_timelog_wnd.pt_obj[ID_MKLOG_ITEM_TMCHART_MEM_GRAPH2].y + st_timelog_wnd.size_obj[ID_MKLOG_ITEM_TMCHART_MEM_GRAPH2].cy);

		//前回値保持
		st_tm_chart_ctrl.pt_time_plot_pix_last = st_tm_chart_ctrl.pt_time_plot_pix;

	}break;
	case MKLOG_ID_TYPE_TRAP: {

	}break;
	case MKLOG_ID_TYPE_EVENT: {

	}break;
	case MKLOG_ID_TYPE_SCAT: {

		INT x10 = st_sc_chart_ctrl.line[ID_MKLOG_SC_CHART1_Y0].x1;  //グラフ１の原点
		INT x20 = st_sc_chart_ctrl.line[ID_MKLOG_SC_CHART2_Y0].x1;	//グラフ１のX軸原点

		INT y10 = st_sc_chart_ctrl.line[ID_MKLOG_SC_CHART1_X0].y1;	//グラフィックメモリのX軸のｙ値　グラフ1
		INT y20 = st_sc_chart_ctrl.line[ID_MKLOG_SC_CHART2_X0].y1;	//グラフィックメモリのX軸のｙ値　グラフ2


		INT plot_x, plot_y, index;

		//Graphic
		if (logbuf[log_type].iw == 0) {//書き込みポインタが0⇒初期値かバッファ１周
			st_sc_chart_ctrl.pt_plot_index = MKLOG_N_LOG_RECORD-1;//プロット対象はバッファ末尾
			st_sc_chart_ctrl.pt_plot_erase_index = st_sc_chart_ctrl.pt_plot_index - st_sc_chart_ctrl.plot_disp_range;
		}
		else {
			st_sc_chart_ctrl.pt_plot_index = logbuf[log_type].iw - 1;

			st_sc_chart_ctrl.pt_plot_erase_index = st_sc_chart_ctrl.pt_plot_index - st_sc_chart_ctrl.plot_disp_range;
			if (st_sc_chart_ctrl.pt_plot_erase_index < 0)st_sc_chart_ctrl.pt_plot_erase_index += MKLOG_N_LOG_RECORD;
		}

		for (int i = ID_MKLOG_SC_CHART1_1; i <= ID_MKLOG_SC_CHART4_1; i+=2) {
			if ((st_sc_chart_ctrl.plot_item_code[i]) && (st_sc_chart_ctrl.plot_item_code[i+1])) {//設定コード!=0

				//100%レンジ（SCADA値）を表示レンジに変換
				plot_x = logbuf[log_type].records[st_sc_chart_ctrl.pt_plot_index].data[i + MKLOG_INDEX_LOG_DATA0] / st_sc_chart_ctrl.disp_mag_xc[ID_MKLOG_PLOT_GRAPH1_A] + x10 - st_sc_chart_ctrl.pix_half_dot_size;
				plot_y = logbuf[log_type].records[st_sc_chart_ctrl.pt_plot_index].data[i + MKLOG_INDEX_LOG_DATA0 + 1] / st_sc_chart_ctrl.disp_mag_yc[ID_MKLOG_PLOT_GRAPH1_A] + y10 - st_sc_chart_ctrl.pix_half_dot_size;

				//ドット描画
				st_scatlog_wnd.pgraphic[ID_MKLOG_GR_MEM_GR]->FillRectangle(
					st_work_wnd.pbrush[st_sc_chart_ctrl.color_chart_item[i]],plot_x,plot_y, st_sc_chart_ctrl.pix_dot_size, st_sc_chart_ctrl.pix_dot_size);

				//ドット消去
				plot_x = logbuf[log_type].records[st_sc_chart_ctrl.pt_plot_erase_index].data[i + MKLOG_INDEX_LOG_DATA0] / st_sc_chart_ctrl.disp_mag_xc[ID_MKLOG_PLOT_GRAPH1_A] + x10 - st_sc_chart_ctrl.pix_half_dot_size;
				plot_y = logbuf[log_type].records[st_sc_chart_ctrl.pt_plot_erase_index].data[i + MKLOG_INDEX_LOG_DATA0 + 1] / st_sc_chart_ctrl.disp_mag_yc[ID_MKLOG_PLOT_GRAPH1_A] + y10 - st_sc_chart_ctrl.pix_half_dot_size;
				st_scatlog_wnd.pgraphic[ID_MKLOG_GR_MEM_GR]->FillRectangle(
					st_work_wnd.pbrush[ID_MKLOG_COLOR_WHITE], plot_x, plot_y, st_sc_chart_ctrl.pix_dot_size, st_sc_chart_ctrl.pix_dot_size);
			}
		}

		for (int i = ID_MKLOG_SC_CHART5_1; i <= ID_MKLOG_SC_CHART8_1; i += 2) {
			if ((st_sc_chart_ctrl.plot_item_code[i]) && (st_sc_chart_ctrl.plot_item_code[i + 1])) {//設定コード!=0

				//100%レンジ（SCADA値）を表示レンジに変換
				plot_x = logbuf[log_type].records[st_sc_chart_ctrl.pt_plot_index].data[i + MKLOG_INDEX_LOG_DATA0] / st_sc_chart_ctrl.disp_mag_xc[ID_MKLOG_PLOT_GRAPH2_A] + x20 - st_sc_chart_ctrl.pix_half_dot_size;
				plot_y = logbuf[log_type].records[st_sc_chart_ctrl.pt_plot_index].data[i + MKLOG_INDEX_LOG_DATA0 + 1] / st_sc_chart_ctrl.disp_mag_yc[ID_MKLOG_PLOT_GRAPH2_A] + y20 - st_sc_chart_ctrl.pix_half_dot_size;

				//ドット描画
				st_scatlog_wnd.pgraphic[ID_MKLOG_GR_MEM_GR]->FillRectangle(
					st_work_wnd.pbrush[st_sc_chart_ctrl.color_chart_item[i]], plot_x, plot_y, st_sc_chart_ctrl.pix_dot_size, st_sc_chart_ctrl.pix_dot_size);

				//ドット消去
				plot_x = logbuf[log_type].records[st_sc_chart_ctrl.pt_plot_erase_index].data[i + MKLOG_INDEX_LOG_DATA0] / st_sc_chart_ctrl.disp_mag_xc[ID_MKLOG_PLOT_GRAPH2_A] + x20 - st_sc_chart_ctrl.pix_half_dot_size;
				plot_y = logbuf[log_type].records[st_sc_chart_ctrl.pt_plot_erase_index].data[i + MKLOG_INDEX_LOG_DATA0 + 1] / st_sc_chart_ctrl.disp_mag_yc[ID_MKLOG_PLOT_GRAPH2_A] + y20 - st_sc_chart_ctrl.pix_half_dot_size;
				st_scatlog_wnd.pgraphic[ID_MKLOG_GR_MEM_GR]->FillRectangle(
					st_work_wnd.pbrush[ID_MKLOG_COLOR_WHITE], plot_x, plot_y, st_sc_chart_ctrl.pix_dot_size, st_sc_chart_ctrl.pix_dot_size);
			}
		}
		//前回値保持
		st_sc_chart_ctrl.pt_plot_index_last = st_sc_chart_ctrl.pt_plot_index;

	}break;
	case MKLOG_ID_TYPE_ALL:
	default:break;

	}

	return;	
}

//Information描画
void CMKLog::draw_info(int log_type) {
	switch (log_type) {
	case MKLOG_ID_TYPE_TIME: {

		PatBlt(st_timelog_wnd.hdc[ID_MKLOG_GR_MEM_INFO], 0, 0, st_timelog_wnd.size_obj[ID_MKLOG_ITEM_TMCHART_GRAPH1].cx, st_timelog_wnd.size_obj[ID_MKLOG_ITEM_TMCHART_GRAPH1].cy * 2, WHITENESS);
		//テキスト
		for (int i = ID_MKLOG_ITEM_TMCHART_ST_F1X_MIN; i <= ID_MKLOG_ITEM_TMCHART_ST_F2Y_MAX; i++) {
			TextOutW(st_timelog_wnd.hdc[ID_MKLOG_GR_MEM_INFO], st_timelog_wnd.pt_obj[i].x, st_timelog_wnd.pt_obj[i].y, L"                       ", 16);
			wos.str(L""); wos << st_timelog_wnd.tx_obj[i];	
			TextOutW(st_timelog_wnd.hdc[ID_MKLOG_GR_MEM_INFO], st_timelog_wnd.pt_obj[i].x, st_timelog_wnd.pt_obj[i].y, wos.str().c_str(), (int)wos.str().length());
		}

	}break;
	case MKLOG_ID_TYPE_TRAP: {

	}break;
	case MKLOG_ID_TYPE_EVENT: {

	}break;
	case 	MKLOG_ID_TYPE_SCAT: {
		for (int i = ID_MKLOG_ITEM_SCCHART_ST_F1X_MIN; i <= ID_MKLOG_ITEM_SCCHART_ST_F2Y_MAX; i++) {
			TextOutW(st_scatlog_wnd.hdc[ID_MKLOG_GR_MEM_INFO], st_scatlog_wnd.pt_obj[i].x, st_scatlog_wnd.pt_obj[i].y, L"                       ", 16);
			wos.str(L""); wos << st_scatlog_wnd.tx_obj[i];	TextOutW(st_scatlog_wnd.hdc[ID_MKLOG_GR_MEM_INFO], st_scatlog_wnd.pt_obj[i].x, st_scatlog_wnd.pt_obj[i].y, wos.str().c_str(), (int)wos.str().length());
		}
	}break;
	case MKLOG_ID_TYPE_ALL:
	default:break;

	}

	return;
}

void CMKLog::init_graphic(int log_type) {

	switch (log_type) {
	case MKLOG_ID_TYPE_TIME: {

		//st_tm_chart_ctrl.disp_offset_y[]

		//CHARTエリア全体(軸目盛　文字領域含む）
		st_tm_chart_ctrl.area[ID_MKLOG_TM_GR_AREA1].x = (INT)(st_timelog_wnd.pt_obj[ID_MKLOG_ITEM_TMCHART_MEM_GRAPH1].x);
		st_tm_chart_ctrl.area[ID_MKLOG_TM_GR_AREA1].y = st_tm_chart_ctrl.mem_offset_y[ID_MKLOG_PLOT_GRAPH1_A] - (INT)(st_timelog_wnd.size_obj[ID_MKLOG_ITEM_TMCHART_GRAPH1].cy/2);
		st_tm_chart_ctrl.area[ID_MKLOG_TM_GR_AREA1].w = (INT)(st_timelog_wnd.pt_obj[ID_MKLOG_ITEM_TMCHART_MEM_GRAPH1].x + st_timelog_wnd.size_obj[ID_MKLOG_ITEM_TMCHART_MEM_GRAPH1].cx + 5);
		st_tm_chart_ctrl.area[ID_MKLOG_TM_GR_AREA1].h = (INT)(st_timelog_wnd.size_obj[ID_MKLOG_ITEM_TMCHART_MEM_GRAPH1].cy + 30);

		st_tm_chart_ctrl.area[ID_MKLOG_TM_GR_AREA2].x = (INT)(st_timelog_wnd.pt_obj[ID_MKLOG_ITEM_TMCHART_MEM_GRAPH1].x);
		st_tm_chart_ctrl.area[ID_MKLOG_TM_GR_AREA2].y = st_tm_chart_ctrl.mem_offset_y[ID_MKLOG_PLOT_GRAPH2_A] - (INT)(st_timelog_wnd.size_obj[ID_MKLOG_ITEM_TMCHART_GRAPH2].cy / 2);
		st_tm_chart_ctrl.area[ID_MKLOG_TM_GR_AREA2].w = (INT)(st_timelog_wnd.pt_obj[ID_MKLOG_ITEM_TMCHART_MEM_GRAPH1].x + st_timelog_wnd.size_obj[ID_MKLOG_ITEM_TMCHART_MEM_GRAPH1].cx + 5);
		st_tm_chart_ctrl.area[ID_MKLOG_TM_GR_AREA2].h = (INT)(st_timelog_wnd.size_obj[ID_MKLOG_ITEM_TMCHART_MEM_GRAPH1].cy + 30);

		//InvalidiatrRect用
		st_timelog_wnd.rc[ID_MKLOG_ITEM_TMCHART_GRAPH1] = {
					0,
					st_timelog_wnd.pt_obj[ID_MKLOG_ITEM_TMCHART_GRAPH1].y,
					st_timelog_wnd.size_obj[ID_MKLOG_ITEM_TMCHART_GRAPH1].cx + 50,
					st_timelog_wnd.pt_obj[ID_MKLOG_ITEM_TMCHART_GRAPH1].y + st_timelog_wnd.size_obj[ID_MKLOG_ITEM_TMCHART_GRAPH1].cy + 30,
		};
		st_timelog_wnd.rc[ID_MKLOG_ITEM_TMCHART_GRAPH2] = {
					0,
					st_timelog_wnd.pt_obj[ID_MKLOG_ITEM_TMCHART_GRAPH2].y,
					st_timelog_wnd.size_obj[ID_MKLOG_ITEM_TMCHART_GRAPH2].cx + 50,
					st_timelog_wnd.pt_obj[ID_MKLOG_ITEM_TMCHART_GRAPH2].y + st_timelog_wnd.size_obj[ID_MKLOG_ITEM_TMCHART_GRAPH2].cy + 30,
		};

		//CHART分割エリア
		st_tm_chart_ctrl.area[ID_MKLOG_TM_CHART_AREA1_1].x = (INT)(st_timelog_wnd.pt_obj[ID_MKLOG_ITEM_TMCHART_MEM_GRAPH1].x);
		st_tm_chart_ctrl.area[ID_MKLOG_TM_CHART_AREA1_1].y = st_tm_chart_ctrl.mem_offset_y[ID_MKLOG_PLOT_GRAPH1_A] - (INT)(st_timelog_wnd.size_obj[ID_MKLOG_ITEM_TMCHART_MEM_GRAPH1].cy / 2);
		st_tm_chart_ctrl.area[ID_MKLOG_TM_CHART_AREA1_1].w = st_tm_chart_ctrl.pix_disp_range;
		st_tm_chart_ctrl.area[ID_MKLOG_TM_CHART_AREA1_1].h = (INT)(st_timelog_wnd.size_obj[ID_MKLOG_ITEM_TMCHART_MEM_GRAPH1].cy);

		st_tm_chart_ctrl.area[ID_MKLOG_TM_CHART_AREA1_2].x = (INT)(st_timelog_wnd.pt_obj[ID_MKLOG_ITEM_TMCHART_MEM_GRAPH1].x + st_tm_chart_ctrl.pix_disp_range);
		st_tm_chart_ctrl.area[ID_MKLOG_TM_CHART_AREA1_2].y = st_tm_chart_ctrl.area[ID_MKLOG_TM_CHART_AREA1_1].y;
		st_tm_chart_ctrl.area[ID_MKLOG_TM_CHART_AREA1_2].w = st_tm_chart_ctrl.pix_disp_range;
		st_tm_chart_ctrl.area[ID_MKLOG_TM_CHART_AREA1_2].h = (INT)(st_timelog_wnd.size_obj[ID_MKLOG_ITEM_TMCHART_MEM_GRAPH1].cy);

		st_tm_chart_ctrl.area[ID_MKLOG_TM_CHART_AREA2_1].x = (INT)(st_timelog_wnd.pt_obj[ID_MKLOG_ITEM_TMCHART_MEM_GRAPH2].x);
		st_tm_chart_ctrl.area[ID_MKLOG_TM_CHART_AREA2_1].y = st_tm_chart_ctrl.mem_offset_y[ID_MKLOG_PLOT_GRAPH2_A] - (INT)(st_timelog_wnd.size_obj[ID_MKLOG_ITEM_TMCHART_MEM_GRAPH2].cy / 2);
		st_tm_chart_ctrl.area[ID_MKLOG_TM_CHART_AREA2_1].w = st_tm_chart_ctrl.pix_disp_range;
		st_tm_chart_ctrl.area[ID_MKLOG_TM_CHART_AREA2_1].h = (INT)(st_timelog_wnd.size_obj[ID_MKLOG_ITEM_TMCHART_MEM_GRAPH2].cy);

		st_tm_chart_ctrl.area[ID_MKLOG_TM_CHART_AREA2_2].x = (INT)(st_timelog_wnd.pt_obj[ID_MKLOG_ITEM_TMCHART_MEM_GRAPH2].x + st_tm_chart_ctrl.pix_disp_range);
		st_tm_chart_ctrl.area[ID_MKLOG_TM_CHART_AREA2_2].y = st_tm_chart_ctrl.area[ID_MKLOG_TM_CHART_AREA2_1].y;
		st_tm_chart_ctrl.area[ID_MKLOG_TM_CHART_AREA2_2].w = st_tm_chart_ctrl.pix_disp_range;
		st_tm_chart_ctrl.area[ID_MKLOG_TM_CHART_AREA2_2].h = (INT)(st_timelog_wnd.size_obj[ID_MKLOG_ITEM_TMCHART_MEM_GRAPH2].cy);

		//CHART 主軸
		st_tm_chart_ctrl.line[ID_MKLOG_TM_CHART1_X0].x1 = (INT)st_timelog_wnd.pt_obj[ID_MKLOG_ITEM_TMCHART_MEM_GRAPH1].x;
		st_tm_chart_ctrl.line[ID_MKLOG_TM_CHART1_X0].y1 = st_tm_chart_ctrl.mem_offset_y[ID_MKLOG_PLOT_GRAPH1_A];
		st_tm_chart_ctrl.line[ID_MKLOG_TM_CHART1_X0].x2 = st_tm_chart_ctrl.line[ID_MKLOG_TM_CHART1_X0].x1 + (INT)st_timelog_wnd.size_obj[ID_MKLOG_ITEM_TMCHART_MEM_GRAPH1].cx;
		st_tm_chart_ctrl.line[ID_MKLOG_TM_CHART1_X0].y2 = st_tm_chart_ctrl.line[ID_MKLOG_TM_CHART1_X0].y1;

		st_tm_chart_ctrl.line[ID_MKLOG_TM_CHART2_X0].x1 = (INT)st_timelog_wnd.pt_obj[ID_MKLOG_ITEM_TMCHART_MEM_GRAPH2].x;
		st_tm_chart_ctrl.line[ID_MKLOG_TM_CHART2_X0].y1 = st_tm_chart_ctrl.mem_offset_y[ID_MKLOG_PLOT_GRAPH2_A];
		st_tm_chart_ctrl.line[ID_MKLOG_TM_CHART2_X0].x2 = st_tm_chart_ctrl.line[ID_MKLOG_TM_CHART2_X0].x1 + (INT)st_timelog_wnd.size_obj[ID_MKLOG_ITEM_TMCHART_MEM_GRAPH2].cx;
		st_tm_chart_ctrl.line[ID_MKLOG_TM_CHART2_X0].y2 = st_tm_chart_ctrl.line[ID_MKLOG_TM_CHART2_X0].y1;
		
		
		st_tm_chart_ctrl.line[ID_MKLOG_TM_CHART1_Y0].x1 = st_tm_chart_ctrl.line[ID_MKLOG_TM_CHART1_X0].x1+1;
		st_tm_chart_ctrl.line[ID_MKLOG_TM_CHART1_Y0].y1 = (INT)st_timelog_wnd.pt_obj[ID_MKLOG_ITEM_TMCHART_MEM_GRAPH1].y;
		st_tm_chart_ctrl.line[ID_MKLOG_TM_CHART1_Y0].x2 = st_tm_chart_ctrl.line[ID_MKLOG_TM_CHART1_Y0].x1;
		st_tm_chart_ctrl.line[ID_MKLOG_TM_CHART1_Y0].y2 = (INT)(st_timelog_wnd.pt_obj[ID_MKLOG_ITEM_TMCHART_MEM_GRAPH1].y + st_timelog_wnd.size_obj[ID_MKLOG_ITEM_TMCHART_MEM_GRAPH1].cy);

		st_tm_chart_ctrl.line[ID_MKLOG_TM_CHART2_Y0].x1 = st_tm_chart_ctrl.line[ID_MKLOG_TM_CHART2_X0].x1+1;
		st_tm_chart_ctrl.line[ID_MKLOG_TM_CHART2_Y0].y1 = (INT)st_timelog_wnd.pt_obj[ID_MKLOG_ITEM_TMCHART_MEM_GRAPH2].y;
		st_tm_chart_ctrl.line[ID_MKLOG_TM_CHART2_Y0].x2 = st_tm_chart_ctrl.line[ID_MKLOG_TM_CHART2_Y0].x1;
		st_tm_chart_ctrl.line[ID_MKLOG_TM_CHART2_Y0].y2 = (INT)(st_timelog_wnd.pt_obj[ID_MKLOG_ITEM_TMCHART_MEM_GRAPH2].y + st_timelog_wnd.size_obj[ID_MKLOG_ITEM_TMCHART_MEM_GRAPH2].cy);

		//CHART 目盛
		//!!!MEM GRに縦軸目盛り描画
		//時間軸目盛り線
		Gdiplus::Pen* ppen = st_work_wnd.ppen[ID_MKLOG_PEN_DASH_GRAY];

		INT x = 0;
		INT y0 = st_timelog_wnd.pt_obj[ID_MKLOG_ITEM_TMCHART_MEM_GRAPH1].y;
		INT y1 = st_timelog_wnd.pt_obj[ID_MKLOG_ITEM_TMCHART_MEM_GRAPH1].y + st_timelog_wnd.size_obj[ID_MKLOG_ITEM_TMCHART_MEM_GRAPH1].cy;

		for (int i = 0; i <= st_timelog_wnd.size_obj[ID_MKLOG_ITEM_TMCHART_MEM_GRAPH1].cx/ st_tm_chart_ctrl.division_time_axis; i++) {
			st_timelog_wnd.pgraphic[ID_MKLOG_GR_MEM_GR]->DrawLine(ppen,  x + i * st_tm_chart_ctrl.division_time_axis, y0, x + i * st_tm_chart_ctrl.division_time_axis, y1);
		}

		x = st_tm_chart_ctrl.line[ID_MKLOG_TM_CHART2_X0].x1;
		y0 = st_timelog_wnd.pt_obj[ID_MKLOG_ITEM_TMCHART_MEM_GRAPH2].y;
		y1 = st_timelog_wnd.pt_obj[ID_MKLOG_ITEM_TMCHART_MEM_GRAPH2].y + st_timelog_wnd.size_obj[ID_MKLOG_ITEM_TMCHART_MEM_GRAPH2].cy;
		
		for (int i = 0; i <= st_timelog_wnd.size_obj[ID_MKLOG_ITEM_TMCHART_MEM_GRAPH2].cx / st_tm_chart_ctrl.division_time_axis; i++) {
			st_timelog_wnd.pgraphic[ID_MKLOG_GR_MEM_GR]->DrawLine(ppen, x + i * st_tm_chart_ctrl.division_time_axis, y0, x + i * st_tm_chart_ctrl.division_time_axis, y1);
		}

	}break;
	case MKLOG_ID_TYPE_TRAP: {

	}break;
	case MKLOG_ID_TYPE_EVENT: {

	}break;
	case 	MKLOG_ID_TYPE_SCAT: {
		//st_tm_chart_ctrl.disp_offset_y[]

		//CHARTエリア全体(軸目盛　文字領域含む）
		st_sc_chart_ctrl.area[ID_MKLOG_SC_GR_AREA1].x = (INT)(st_scatlog_wnd.size_obj[ID_MKLOG_ITEM_SCCHART_GRAPH1].cx * 3 / 2 - MKLOG_SC_DISP_X_MARGINE);
		st_sc_chart_ctrl.area[ID_MKLOG_SC_GR_AREA1].y = (INT)(st_scatlog_wnd.size_obj[ID_MKLOG_ITEM_SCCHART_GRAPH1].cy * 3 / 2);
		st_sc_chart_ctrl.area[ID_MKLOG_SC_GR_AREA1].w = (INT)(st_scatlog_wnd.size_obj[ID_MKLOG_ITEM_SCCHART_GRAPH1].cx + MKLOG_SC_DISP_X_MARGINE);
		st_sc_chart_ctrl.area[ID_MKLOG_SC_GR_AREA1].h = (INT)(st_scatlog_wnd.size_obj[ID_MKLOG_ITEM_SCCHART_GRAPH1].cy + MKLOG_SC_DISP_H_MARGINE);

		st_sc_chart_ctrl.area[ID_MKLOG_SC_GR_AREA2].x = (INT)(st_scatlog_wnd.size_obj[ID_MKLOG_ITEM_SCCHART_GRAPH2].cx * 3 / 2 - MKLOG_SC_DISP_X_MARGINE);
		st_sc_chart_ctrl.area[ID_MKLOG_SC_GR_AREA2].y = (INT)(st_scatlog_wnd.pt_obj[ID_MKLOG_ITEM_SCCHART_MEM_GRAPH2].y + st_scatlog_wnd.size_obj[ID_MKLOG_ITEM_SCCHART_GRAPH2].cy * 3 / 2);
		st_sc_chart_ctrl.area[ID_MKLOG_SC_GR_AREA2].w = (INT)(st_scatlog_wnd.size_obj[ID_MKLOG_ITEM_SCCHART_GRAPH2].cx + MKLOG_SC_DISP_X_MARGINE);
		st_sc_chart_ctrl.area[ID_MKLOG_SC_GR_AREA2].h = (INT)(st_scatlog_wnd.size_obj[ID_MKLOG_ITEM_SCCHART_GRAPH2].cy + MKLOG_SC_DISP_H_MARGINE);

		//InvalidiatrRect用
		st_scatlog_wnd.rc[ID_MKLOG_ITEM_SCCHART_GRAPH1] = {
					0,
					st_scatlog_wnd.pt_obj[ID_MKLOG_ITEM_SCCHART_GRAPH1].y,
					st_scatlog_wnd.size_obj[ID_MKLOG_ITEM_SCCHART_GRAPH1].cx + MKLOG_SC_DISP_X_MARGINE,
					st_scatlog_wnd.pt_obj[ID_MKLOG_ITEM_SCCHART_GRAPH1].y + st_scatlog_wnd.size_obj[ID_MKLOG_ITEM_SCCHART_GRAPH1].cy + MKLOG_SC_DISP_H_MARGINE,
		};
		st_scatlog_wnd.rc[ID_MKLOG_ITEM_SCCHART_GRAPH2] = {
					0,
					st_scatlog_wnd.pt_obj[ID_MKLOG_ITEM_SCCHART_GRAPH2].y,
					st_scatlog_wnd.size_obj[ID_MKLOG_ITEM_SCCHART_GRAPH2].cx + MKLOG_SC_DISP_X_MARGINE,
					st_scatlog_wnd.pt_obj[ID_MKLOG_ITEM_SCCHART_GRAPH2].y + st_scatlog_wnd.size_obj[ID_MKLOG_ITEM_SCCHART_GRAPH2].cy + MKLOG_SC_DISP_H_MARGINE,
		};


		//CHART 主軸
		st_sc_chart_ctrl.line[ID_MKLOG_SC_CHART1_X0].x1 = (INT)st_scatlog_wnd.pt_obj[ID_MKLOG_ITEM_SCCHART_MEM_GRAPH1].x;
		st_sc_chart_ctrl.line[ID_MKLOG_SC_CHART1_X0].y1 = (INT)st_scatlog_wnd.size_obj[ID_MKLOG_ITEM_SCCHART_MEM_GRAPH1].cy/2;
		st_sc_chart_ctrl.line[ID_MKLOG_SC_CHART1_X0].x2 = st_sc_chart_ctrl.line[ID_MKLOG_SC_CHART1_X0].x1 + (INT)st_scatlog_wnd.size_obj[ID_MKLOG_ITEM_SCCHART_MEM_GRAPH1].cx;
		st_sc_chart_ctrl.line[ID_MKLOG_SC_CHART1_X0].y2 = st_sc_chart_ctrl.line[ID_MKLOG_SC_CHART1_X0].y1;

		st_sc_chart_ctrl.line[ID_MKLOG_SC_CHART2_X0].x1 = (INT)st_scatlog_wnd.pt_obj[ID_MKLOG_ITEM_SCCHART_MEM_GRAPH2].x;
		st_sc_chart_ctrl.line[ID_MKLOG_SC_CHART2_X0].y1 = (INT)st_scatlog_wnd.pt_obj[ID_MKLOG_ITEM_SCCHART_MEM_GRAPH2].y + (INT)st_scatlog_wnd.size_obj[ID_MKLOG_ITEM_SCCHART_MEM_GRAPH1].cy / 2;
		st_sc_chart_ctrl.line[ID_MKLOG_SC_CHART2_X0].x2 = st_sc_chart_ctrl.line[ID_MKLOG_SC_CHART2_X0].x1 + (INT)st_scatlog_wnd.size_obj[ID_MKLOG_ITEM_SCCHART_MEM_GRAPH2].cx;
		st_sc_chart_ctrl.line[ID_MKLOG_SC_CHART2_X0].y2 = st_sc_chart_ctrl.line[ID_MKLOG_SC_CHART2_X0].y1;


		st_sc_chart_ctrl.line[ID_MKLOG_SC_CHART1_Y0].x1 = (INT)(st_scatlog_wnd.pt_obj[ID_MKLOG_ITEM_SCCHART_MEM_GRAPH1].x + st_scatlog_wnd.size_obj[ID_MKLOG_ITEM_SCCHART_MEM_GRAPH1].cx / 2);
		st_sc_chart_ctrl.line[ID_MKLOG_SC_CHART1_Y0].y1 = (INT)st_scatlog_wnd.pt_obj[ID_MKLOG_ITEM_SCCHART_MEM_GRAPH1].y;
		st_sc_chart_ctrl.line[ID_MKLOG_SC_CHART1_Y0].x2 = st_sc_chart_ctrl.line[ID_MKLOG_SC_CHART1_Y0].x1;
		st_sc_chart_ctrl.line[ID_MKLOG_SC_CHART1_Y0].y2 = st_sc_chart_ctrl.line[ID_MKLOG_SC_CHART1_Y0].y1 + (INT)(st_scatlog_wnd.size_obj[ID_MKLOG_ITEM_SCCHART_MEM_GRAPH1].cy );

		st_sc_chart_ctrl.line[ID_MKLOG_SC_CHART2_Y0].x1 = (INT)(st_scatlog_wnd.pt_obj[ID_MKLOG_ITEM_SCCHART_MEM_GRAPH2].x + st_scatlog_wnd.size_obj[ID_MKLOG_ITEM_SCCHART_MEM_GRAPH2].cx / 2);
		st_sc_chart_ctrl.line[ID_MKLOG_SC_CHART2_Y0].y1 = (INT)st_scatlog_wnd.pt_obj[ID_MKLOG_ITEM_SCCHART_MEM_GRAPH2].y;
		st_sc_chart_ctrl.line[ID_MKLOG_SC_CHART2_Y0].x2 = st_sc_chart_ctrl.line[ID_MKLOG_SC_CHART2_Y0].x1;
		st_sc_chart_ctrl.line[ID_MKLOG_SC_CHART2_Y0].y2 = st_sc_chart_ctrl.line[ID_MKLOG_SC_CHART2_Y0].y1 + (INT)(st_scatlog_wnd.size_obj[ID_MKLOG_ITEM_SCCHART_MEM_GRAPH2].cy);

	}break;
	case MKLOG_ID_TYPE_ALL:
	default:break;

	}

	return;
}

void CMKLog::init_log_setting(int log_type, int update_log_source) {

	switch (log_type) {
	case MKLOG_ID_TYPE_TIME: {
		INT32 n_onoff1 = 0, n_onoff2 = 0;

		clear_record(log_type);

		if (update_log_source == L_ON) {//LOG SOURCEを書き換え
			wchar_t buffer[256];
			for (int i = ID_MKLOG_ITEM_TMCHART_ED_F1CODE1, k = 0; i <= ID_MKLOG_ITEM_TMCHART_ED_F2CODE8; i++, k++) {
				GetWindowText(st_timelog_wnd.hwnd_obj[i], buffer, 256);
				std::wistringstream iss(buffer);
				int code = _wtoi(buffer);
				st_tm_chart_ctrl.chart_item_set_code[k]= st_tm_chart_ctrl.plot_item_code[k] =code;
				//スタティックテキスト更新
				if (st_tm_chart_ctrl.chart_item_txt_mode == ID_MKLOG_ITEM_TXT_100) {//100%表示の時
					if (code != 0) {
						std::wostringstream wostr;
						wostr << pdb->item[st_tm_chart_ctrl.chart_item_set_code[k]].d100;
						SetWindowText(st_timelog_wnd.hwnd_obj[i - 31], wostr.str().c_str());
					}
				}
				else {																//項目名の時
					SetWindowText(st_timelog_wnd.hwnd_obj[i - 31], pdb->item[st_tm_chart_ctrl.chart_item_set_code[k]].title);
				}

				INT32 item_type = pdb->item[phead[log_type]->code[k + 2]].code;//CHART項目のコード
				//チャートプロットのベースオフセット設定　アナロググラフ　ビットグラフ
				if (i < ID_MKLOG_ITEM_TMCHART_ED_F2CODE1) {//チャートフレーム１
					//if ((item_type & MKLOG_ITEM_TYPE_BIT) == MKLOG_ITEM_TYPE_BIT) {//ビットグラフ
					if (((item_type & MKLOG_ITEM_TYPE_BIT) >= MKLOG_ITEM_TYPE_BIT)&&((item_type & MKLOG_ITEM_TYPE_BIT) <= MKLOG_ITEM_TYPE_MENTE)){//ビットグラフ
						n_onoff1++;//ON/OFFアイテムの数
						//デジタル0点位置セット
						st_tm_chart_ctrl.chart_item_disp_y0[i - ID_MKLOG_ITEM_TMCHART_ED_F1CODE1] = st_tm_chart_ctrl.mem_offset_y[ID_MKLOG_PLOT_GRAPH1_D];
						//デジタル点数に応じてオフセット値更新		
						st_tm_chart_ctrl.chart_item_disp_y0[i - ID_MKLOG_ITEM_TMCHART_ED_F1CODE1] -= n_onoff1 * st_tm_chart_ctrl.division_y_axis;

					}
					else {
						st_tm_chart_ctrl.chart_item_disp_y0[i - ID_MKLOG_ITEM_TMCHART_ED_F1CODE1] = st_tm_chart_ctrl.mem_offset_y[ID_MKLOG_PLOT_GRAPH1_A];
					}
				}
				else {//チャートフレーム2

			//		if ((item_type & MKLOG_ITEM_TYPE_BIT) == MKLOG_ITEM_TYPE_BIT) {//ビットグラフ
					if (((item_type & MKLOG_ITEM_TYPE_BIT) >= MKLOG_ITEM_TYPE_BIT) && ((item_type & MKLOG_ITEM_TYPE_BIT) <= MKLOG_ITEM_TYPE_MENTE)) {//ビットグラフ
						n_onoff2++;
						st_tm_chart_ctrl.chart_item_disp_y0[i - ID_MKLOG_ITEM_TMCHART_ED_F1CODE1] = st_tm_chart_ctrl.mem_offset_y[ID_MKLOG_PLOT_GRAPH2_D];
						st_tm_chart_ctrl.chart_item_disp_y0[i - ID_MKLOG_ITEM_TMCHART_ED_F1CODE1] -= n_onoff2 * st_tm_chart_ctrl.division_y_axis;

					}
					else {
						st_tm_chart_ctrl.chart_item_disp_y0[i - ID_MKLOG_ITEM_TMCHART_ED_F1CODE1] = st_tm_chart_ctrl.mem_offset_y[ID_MKLOG_PLOT_GRAPH2_A];
					}
				}
			}
			if (st_work_wnd.phost != NULL) {//ホスト（scada）のクラスメソッドで上書き
				st_work_wnd.phost->req_command(MKLOG_MSGID_REFRESH_TMCHART_ITEM, log_type, st_tm_chart_ctrl.chart_item_set_code);
			}

		}
		else {//LOG SOURCEから読み込み
	
			for (int i = ID_MKLOG_ITEM_TMCHART_ED_F1CODE1, k = 0; i <= ID_MKLOG_ITEM_TMCHART_ED_F2CODE8; i++, k++) {

				st_tm_chart_ctrl.chart_item_set_code[k] = phead[log_type]->code[k + 2];
				std::wostringstream oss; oss.str(L"");
				oss << st_tm_chart_ctrl.chart_item_set_code[k];
				SetWindowText(st_timelog_wnd.hwnd_obj[i], oss.str().c_str());

				//スタティックテキスト表示更新
				if (st_tm_chart_ctrl.chart_item_txt_mode == ID_MKLOG_ITEM_TXT_100) {//100%表示の時
						std::wostringstream wostr;
						wostr << pdb->item[st_tm_chart_ctrl.chart_item_set_code[k]].d100;
						SetWindowText(st_timelog_wnd.hwnd_obj[i - 31], wostr.str().c_str());
				}
				else {//項目名表示の時
					SetWindowText(st_timelog_wnd.hwnd_obj[i - 31], pdb->item[st_tm_chart_ctrl.chart_item_set_code[k]].title);
				}

				INT32 item_type = pdb->item[phead[log_type]->code[k + 2]].code;//CHART項目のコード
				//チャートプロットのベースオフセット設定　アナロググラフ　ビットグラフ
				if (i < ID_MKLOG_ITEM_TMCHART_ED_F2CODE1) {//チャートフレーム１
					if ((item_type & MKLOG_ITEM_TYPE_BIT) == MKLOG_ITEM_TYPE_BIT) {//ビットグラフ
						n_onoff1++;//ON/OFFアイテムの数
						//デジタル0点位置セット
						st_tm_chart_ctrl.chart_item_disp_y0[i- ID_MKLOG_ITEM_TMCHART_ED_F1CODE1] = st_tm_chart_ctrl.mem_offset_y[ID_MKLOG_PLOT_GRAPH1_D];
						//デジタル点数に応じてオフセット値更新		
						st_tm_chart_ctrl.chart_item_disp_y0[i - ID_MKLOG_ITEM_TMCHART_ED_F1CODE1] -= n_onoff1 * st_tm_chart_ctrl.division_y_axis;

					}
					else {
						st_tm_chart_ctrl.chart_item_disp_y0[i - ID_MKLOG_ITEM_TMCHART_ED_F1CODE1] = st_tm_chart_ctrl.mem_offset_y[ID_MKLOG_PLOT_GRAPH1_A];
					}
				}
				else {//チャートフレーム2
	
					if ((item_type & MKLOG_ITEM_TYPE_BIT) == MKLOG_ITEM_TYPE_BIT) {//ビットグラフ
						n_onoff2++;
						st_tm_chart_ctrl.chart_item_disp_y0[i - ID_MKLOG_ITEM_TMCHART_ED_F1CODE1] = st_tm_chart_ctrl.mem_offset_y[ID_MKLOG_PLOT_GRAPH2_D];
						st_tm_chart_ctrl.chart_item_disp_y0[i - ID_MKLOG_ITEM_TMCHART_ED_F1CODE1] -= n_onoff2 * st_tm_chart_ctrl.division_y_axis;

					}
					else {
						st_tm_chart_ctrl.chart_item_disp_y0[i - ID_MKLOG_ITEM_TMCHART_ED_F1CODE1] = st_tm_chart_ctrl.mem_offset_y[ID_MKLOG_PLOT_GRAPH2_A];
					}
				}
			}
		}
	}break;
	case MKLOG_ID_TYPE_TRAP: {

	}break;
	case MKLOG_ID_TYPE_EVENT: {

	}break;
	case 	MKLOG_ID_TYPE_SCAT: {
		clear_record(log_type);

		if (update_log_source == L_ON) {//LOG SOURCEを書き換え
			wchar_t buffer[256];
			for (int i = ID_MKLOG_ITEM_SCCHART_ED_F1CODE1, k = 0; i <= ID_MKLOG_ITEM_SCCHART_ED_F2CODE8; i++, k++) {
				GetWindowText(st_scatlog_wnd.hwnd_obj[i], buffer, 256);
				int code = _wtoi(buffer);
				st_sc_chart_ctrl.chart_item_set_code[k] = st_sc_chart_ctrl.plot_item_code[k] = code;


				if (st_sc_chart_ctrl.chart_item_txt_mode == ID_MKLOG_ITEM_TXT_100) {
					std::wostringstream wostr;
					wostr << pdb->item[st_sc_chart_ctrl.chart_item_set_code[k]].d100;
					//スタティックテキスト更新
					SetWindowText(st_scatlog_wnd.hwnd_obj[i - 31], wostr.str().c_str());
				}
				else {
					//スタティックテキスト更新
					SetWindowText(st_scatlog_wnd.hwnd_obj[i - 31], pdb->item[st_sc_chart_ctrl.chart_item_set_code[k]].title);

				}
			}
			//SCADA管理のlogsource更新
			if (st_work_wnd.phost != NULL) {
				st_work_wnd.phost->req_command(MKLOG_MSGID_REFRESH_SCCHART_ITEM, log_type, st_sc_chart_ctrl.chart_item_set_code);
			}
		}
		else {//LOG SOURCEから読み込み
			for (int i = ID_MKLOG_ITEM_SCCHART_ED_F1CODE1, k = 0; i <= ID_MKLOG_ITEM_SCCHART_ED_F2CODE8; i++, k++) {
				st_sc_chart_ctrl.chart_item_set_code[k] = phead[log_type]->code[k + 2];
				std::wostringstream oss; oss.str(L"");
				oss << st_sc_chart_ctrl.chart_item_set_code[k];
				SetWindowText(st_scatlog_wnd.hwnd_obj[i], oss.str().c_str());

				if (st_sc_chart_ctrl.chart_item_txt_mode == ID_MKLOG_ITEM_TXT_100) {
						std::wostringstream wostr;
						wostr << pdb->item[st_sc_chart_ctrl.chart_item_set_code[k]].d100;
						//スタティックテキスト更新
						SetWindowText(st_scatlog_wnd.hwnd_obj[i - 31], wostr.str().c_str());

				}
				else {
					//スタティックテキスト更新
					SetWindowText(st_scatlog_wnd.hwnd_obj[i - 31], pdb->item[st_sc_chart_ctrl.chart_item_set_code[k]].title);

				}

			}
		}

	}break;
	case MKLOG_ID_TYPE_ALL:
	default:break;

	}

	return;
}

void CMKLog::draw_icon(HDC hdc,int log_type) {

	Gdiplus::Graphics graphics(hdc);
	int offset_x = 50;

	switch (log_type) {
	case MKLOG_ID_TYPE_TIME: {
		st_sc_chart_ctrl.icon_draw = L_ON;
		int id = ID_MKLOG_ITEM_TMCHART_ED_F1CODE1;
		graphics.FillRectangle(st_work_wnd.pbrush[ID_MKLOG_COLOR_RED], (INT)st_timelog_wnd.pt_obj[id].x		+ offset_x, (INT)st_timelog_wnd.pt_obj[id].y, 20, 25); id++;
		graphics.FillRectangle(st_work_wnd.pbrush[ID_MKLOG_COLOR_ORANGE], (INT)st_timelog_wnd.pt_obj[id].x	+ offset_x, (INT)st_timelog_wnd.pt_obj[id].y, 20, 25); id++;
		graphics.FillRectangle(st_work_wnd.pbrush[ID_MKLOG_COLOR_GREEN], (INT)st_timelog_wnd.pt_obj[id].x	+ offset_x, (INT)st_timelog_wnd.pt_obj[id].y, 20, 25); id++;
		graphics.FillRectangle(st_work_wnd.pbrush[ID_MKLOG_COLOR_CYAN], (INT)st_timelog_wnd.pt_obj[id].x	+ offset_x, (INT)st_timelog_wnd.pt_obj[id].y, 20, 25); id++;
		graphics.FillRectangle(st_work_wnd.pbrush[ID_MKLOG_COLOR_BLUE], (INT)st_timelog_wnd.pt_obj[id].x	+ offset_x, (INT)st_timelog_wnd.pt_obj[id].y, 20, 25); id++;
		graphics.FillRectangle(st_work_wnd.pbrush[ID_MKLOG_COLOR_PURPLE], (INT)st_timelog_wnd.pt_obj[id].x	+ offset_x, (INT)st_timelog_wnd.pt_obj[id].y, 20, 25); id++;
		graphics.FillRectangle(st_work_wnd.pbrush[ID_MKLOG_COLOR_BROWN], (INT)st_timelog_wnd.pt_obj[id].x	+ offset_x, (INT)st_timelog_wnd.pt_obj[id].y, 20, 25); id++;
		graphics.FillRectangle(st_work_wnd.pbrush[ID_MKLOG_COLOR_MAZENDA], (INT)st_timelog_wnd.pt_obj[id].x + offset_x, (INT)st_timelog_wnd.pt_obj[id].y, 20, 25); id++;
		graphics.FillRectangle(st_work_wnd.pbrush[ID_MKLOG_COLOR_RED], (INT)st_timelog_wnd.pt_obj[id].x		+ offset_x, (INT)st_timelog_wnd.pt_obj[id].y, 20, 25); id++;
		graphics.FillRectangle(st_work_wnd.pbrush[ID_MKLOG_COLOR_ORANGE], (INT)st_timelog_wnd.pt_obj[id].x	+ offset_x, (INT)st_timelog_wnd.pt_obj[id].y, 20, 25); id++;
		graphics.FillRectangle(st_work_wnd.pbrush[ID_MKLOG_COLOR_GREEN], (INT)st_timelog_wnd.pt_obj[id].x	+ offset_x, (INT)st_timelog_wnd.pt_obj[id].y, 20, 25); id++;
		graphics.FillRectangle(st_work_wnd.pbrush[ID_MKLOG_COLOR_CYAN], (INT)st_timelog_wnd.pt_obj[id].x	+ offset_x, (INT)st_timelog_wnd.pt_obj[id].y, 20, 25); id++;
		graphics.FillRectangle(st_work_wnd.pbrush[ID_MKLOG_COLOR_BLUE], (INT)st_timelog_wnd.pt_obj[id].x	+ offset_x, (INT)st_timelog_wnd.pt_obj[id].y, 20, 25); id++;
		graphics.FillRectangle(st_work_wnd.pbrush[ID_MKLOG_COLOR_PURPLE], (INT)st_timelog_wnd.pt_obj[id].x	+ offset_x, (INT)st_timelog_wnd.pt_obj[id].y, 20, 25); id++;
		graphics.FillRectangle(st_work_wnd.pbrush[ID_MKLOG_COLOR_BROWN], (INT)st_timelog_wnd.pt_obj[id].x	+ offset_x, (INT)st_timelog_wnd.pt_obj[id].y, 20, 25); id++;
		graphics.FillRectangle(st_work_wnd.pbrush[ID_MKLOG_COLOR_MAZENDA], (INT)st_timelog_wnd.pt_obj[id].x + offset_x, (INT)st_timelog_wnd.pt_obj[id].y, 20, 25); id++;
	//	graphics.FillRectangle(st_work_wnd.pbrush[ID_MKLOG_COLOR_RED],0,0,200,200);

	}break;
	case MKLOG_ID_TYPE_TRAP: {

	}break;
	case MKLOG_ID_TYPE_EVENT: {

	}break;
	case 	MKLOG_ID_TYPE_SCAT: {
		st_sc_chart_ctrl.icon_draw = L_ON;
		int id = ID_MKLOG_ITEM_SCCHART_ED_F1CODE1;

		graphics.FillRectangle(st_work_wnd.pbrush[ID_MKLOG_COLOR_RED], (INT)st_scatlog_wnd.pt_obj[id].x		+ offset_x, (INT)st_scatlog_wnd.pt_obj[id].y, 20, 25); id++;
		graphics.FillRectangle(st_work_wnd.pbrush[ID_MKLOG_COLOR_RED], (INT)st_scatlog_wnd.pt_obj[id].x		+ offset_x, (INT)st_scatlog_wnd.pt_obj[id].y, 20, 25); id++;
		graphics.FillRectangle(st_work_wnd.pbrush[ID_MKLOG_COLOR_GREEN], (INT)st_scatlog_wnd.pt_obj[id].x	+ offset_x, (INT)st_scatlog_wnd.pt_obj[id].y, 20, 25); id++;
		graphics.FillRectangle(st_work_wnd.pbrush[ID_MKLOG_COLOR_GREEN], (INT)st_scatlog_wnd.pt_obj[id].x	+ offset_x, (INT)st_scatlog_wnd.pt_obj[id].y, 20, 25); id++;
		graphics.FillRectangle(st_work_wnd.pbrush[ID_MKLOG_COLOR_BLUE], (INT)st_scatlog_wnd.pt_obj[id].x	+ offset_x, (INT)st_scatlog_wnd.pt_obj[id].y, 20, 25); id++;
		graphics.FillRectangle(st_work_wnd.pbrush[ID_MKLOG_COLOR_BLUE], (INT)st_scatlog_wnd.pt_obj[id].x	+ offset_x, (INT)st_scatlog_wnd.pt_obj[id].y, 20, 25); id++;
		graphics.FillRectangle(st_work_wnd.pbrush[ID_MKLOG_COLOR_MAZENDA], (INT)st_scatlog_wnd.pt_obj[id].x + offset_x, (INT)st_scatlog_wnd.pt_obj[id].y, 20, 25); id++;
		graphics.FillRectangle(st_work_wnd.pbrush[ID_MKLOG_COLOR_MAZENDA], (INT)st_scatlog_wnd.pt_obj[id].x + offset_x, (INT)st_scatlog_wnd.pt_obj[id].y, 20, 25); id++;
		graphics.FillRectangle(st_work_wnd.pbrush[ID_MKLOG_COLOR_RED], (INT)st_scatlog_wnd.pt_obj[id].x		+ offset_x, (INT)st_scatlog_wnd.pt_obj[id].y, 20, 25); id++;
		graphics.FillRectangle(st_work_wnd.pbrush[ID_MKLOG_COLOR_RED], (INT)st_scatlog_wnd.pt_obj[id].x		+ offset_x, (INT)st_scatlog_wnd.pt_obj[id].y, 20, 25); id++;
		graphics.FillRectangle(st_work_wnd.pbrush[ID_MKLOG_COLOR_GREEN], (INT)st_scatlog_wnd.pt_obj[id].x	+ offset_x, (INT)st_scatlog_wnd.pt_obj[id].y, 20, 25); id++;
		graphics.FillRectangle(st_work_wnd.pbrush[ID_MKLOG_COLOR_GREEN], (INT)st_scatlog_wnd.pt_obj[id].x	+ offset_x, (INT)st_scatlog_wnd.pt_obj[id].y, 20, 25); id++;
		graphics.FillRectangle(st_work_wnd.pbrush[ID_MKLOG_COLOR_BLUE], (INT)st_scatlog_wnd.pt_obj[id].x	+ offset_x, (INT)st_scatlog_wnd.pt_obj[id].y, 20, 25); id++;
		graphics.FillRectangle(st_work_wnd.pbrush[ID_MKLOG_COLOR_BLUE], (INT)st_scatlog_wnd.pt_obj[id].x	+ offset_x, (INT)st_scatlog_wnd.pt_obj[id].y, 20, 25); id++;
		graphics.FillRectangle(st_work_wnd.pbrush[ID_MKLOG_COLOR_MAZENDA], (INT)st_scatlog_wnd.pt_obj[id].x + offset_x, (INT)st_scatlog_wnd.pt_obj[id].y, 20, 25); id++;
		graphics.FillRectangle(st_work_wnd.pbrush[ID_MKLOG_COLOR_MAZENDA], (INT)st_scatlog_wnd.pt_obj[id].x + offset_x, (INT)st_scatlog_wnd.pt_obj[id].y, 20, 25); id++;

	}break;
	case MKLOG_ID_TYPE_ALL:
	default:break;

	}
	return;
}
