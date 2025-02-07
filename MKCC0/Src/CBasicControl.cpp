#include "common_def.h"
#include "CBasicControl.h"
#include <windowsx.h>
#include <timeapi.h>
#include <commctrl.h> //コモンコントロール用

HRESULT __stdcall CBasicControl::initialize(void* param) { 
	inf.panel_func_id;
	set_func_pb_txt(); 
	set_item_chk_txt();
	set_panel_tip_txt();
	return 0; 
}; //初期化処理

unsigned WINAPI CBasicControl::run(LPVOID lpParam)
{
	CBasicControl* pobj = (CBasicControl*)lpParam;

	LARGE_INTEGER start_count;  // 現在のカウント数

	
	QueryPerformanceFrequency(&inf.sys_freq);  // システムの周波数
	QueryPerformanceCounter(&inf.sys_count); // パフォーマンスカウンター現在値
	
	while (pobj->inf.command != BC_CODE_COM_TERMINATE) {

		QueryPerformanceCounter(&start_count);  // 現在のカウント数
		pobj->inf.event_triggered = WaitForMultipleObjects(pobj->inf.n_active_events, pobj->inf.hevents, FALSE, INFINITE);//メインスレッドからのSIGNAL状態待ち


		pobj->inf.act_time = ((start_count.QuadPart - inf.sys_count.QuadPart) * 1000000L) / inf.sys_freq.QuadPart;
		QueryPerformanceCounter(&inf.sys_count); // パフォーマンスカウンター現在値

		wos.str(L""); wos << inf.status << L":"  << std::setfill(L'0') << std::setw(4)<< pobj->inf.act_time;
		msg2host(wos.str());

		//処理周期確認用

		switch (pobj->inf.status) {
		case BC_CODE_STAT_INIT_REQ: {
			if (S_OK == pobj->initialize(pobj)) {
				pobj->inf.status = BC_CODE_STAT_RUN_REQ;
				wos.str(L"Initialize OK");msg2listview(wos.str());
			}
			else {
				pobj->inf.status = BC_CODE_STAT_INIT_ERROR;
			}
		}break;
		case BC_CODE_STAT_RUN_REQ:
		{
			if (S_OK == pobj->routine_work(pobj))pobj->inf.status = BC_CODE_STAT_RUNNING;
			else									pobj->inf.status = BC_CODE_STAT_ERROR;
		}break;
		case BC_CODE_STAT_RUNNING:
		{
			if (S_OK != pobj->routine_work(pobj))			pobj->inf.status = BC_CODE_STAT_ERROR;
		}break;
		case BC_CODE_STAT_ERROR:
		case BC_CODE_STAT_INIT_ERROR:
		{
			pobj->inf.status = BC_CODE_STAT_INIT_WAIT;
		}break;
		case BC_CODE_STAT_INIT_WAIT:
		{
			if ((inf.total_act % pobj->inf.retry_count) == 0)
				pobj->inf.status = BC_CODE_STAT_INIT_REQ;
		}break;
		default: {//状態遷移ステータス不定
			pobj->inf.status = BC_CODE_STAT_IDLE;
			return 0;
		}break;
		}

//		pobj->inf.act_time = timeGetTime() - start_time;//実処理時間測定用
		if (this->inf.command != BC_CODE_COM_REPEAT_SCAN) break;//逐次処理の場合は抜ける
	}

	pobj->inf.status = BC_CODE_STAT_IDLE;
	return 0;
}

LRESULT CALLBACK CBasicControl::PanelProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp) {

	switch (msg) {
	case WM_COMMAND:
		switch (LOWORD(wp)) {
		case 1:
		default:break;
		}
	}
	return 0;
};

///###	メインパネルの
void CBasicControl::msg2host(wstring wstr) {

	if(inf.hwnd_msgstatics != NULL) SetWindowTextW(inf.hwnd_msgstatics, wstr.c_str());
	return;
}

///###	タブパネルのListViewにメッセージを出力
void CBasicControl::msg2listview(wstring wstr) {
	return;
}

void CBasicControl::set_PNLparam_value(float p1, float p2, float p3, float p4, float p5, float p6) {
	return;
}

//タブパネルのStaticテキストを設定
void CBasicControl::set_panel_tip_txt() { 
	return; 
}
//タブパネルのFunctionボタンのStaticテキストを設定
void CBasicControl::set_func_pb_txt() {
	return; 
}
//タブパネルのItem chkテキストを設定
void CBasicControl::set_item_chk_txt() {

	return; 
}
