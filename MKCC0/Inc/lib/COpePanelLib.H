#pragma once

#include "SPEC_DEF.h"
#include "COMMON_DEF.h"
#include "PLC_DEF.h"
#include "CPanelIo.h"
#include "CPanelGr.h"

using namespace std;

class CPanelObj {
public:

	CPanelObj(HWND _hwnd,Rect _rc) 
	{
		hWnd = _hwnd;
		rc_panel = _rc;
		hdc = GetDC(hWnd);

		hBmp_mem = CreateCompatibleBitmap(hdc, rc_panel.Width, rc_panel.Height);
		hBmp_bk = CreateCompatibleBitmap(hdc, rc_panel.Width, rc_panel.Height);
		hBmp_inf = CreateCompatibleBitmap(hdc, rc_panel.Width, rc_panel.Height);
		if (hBmp_mem == NULL || hBmp_bk == NULL || hBmp_inf == NULL) {
			hr = E_FAIL;
			return;
		}

		hdc_mem = CreateCompatibleDC(hdc);
		hdc_bk = CreateCompatibleDC(hdc);
		hdc_inf = CreateCompatibleDC(hdc);
		if (hdc_mem == NULL || hdc_bk == NULL || hdc_inf == NULL) {
			hr = E_FAIL;
			return;
		}
		SelectObject(hdc_mem, hBmp_mem);
		SelectObject(hdc_bk, hBmp_bk);
		SelectObject(hdc_inf, hBmp_inf);

		pgraph_mem = new Graphics(hdc_mem);
		pgraph_bk = new Graphics(hdc_bk);
		pgraph_inf = new Graphics(hdc_inf);
		if (pgraph_mem == NULL || pgraph_bk == NULL || pgraph_inf == NULL) {
			hr = E_FAIL;
			return;
		}
		pgraph_mem->SetSmoothingMode(SmoothingModeAntiAlias);
		pgraph_bk->SetSmoothingMode(SmoothingModeAntiAlias);
		pgraph_inf->SetSmoothingMode(SmoothingModeAntiAlias);
	}
	virtual ~CPanelObj() {
		delete pgraph_mem;
		delete pgraph_bk;	
		delete pgraph_inf;	
		DeleteDC(hdc_mem);	
		DeleteDC(hdc_bk);	
		DeleteDC(hdc_inf);	
		DeleteObject(hBmp_mem);	
		DeleteObject(hBmp_bk);	
		DeleteObject(hBmp_inf);
		ReleaseDC(hWnd, hdc);	
	}

	HRESULT hr = S_OK;

	HWND hWnd;			//ウィンドウハンドル
	Rect rc_panel;		//パネルの表示位置

	HBITMAP hBmp_mem;	//背景用ビットマップ
	HBITMAP hBmp_bk;	//パネル用ビットマップ
	HBITMAP hBmp_inf;	//情報表示用ビットマップ

	HDC hdc;			//パネルへ書き込み用DC
	HDC hdc_mem;		//パネルへ書き込み用DC
	HDC hdc_bk;			//背景用DC
	HDC hdc_inf;		//情報表示用DC

	Graphics* pgraph_mem;	//描画用グラフィックス
	Graphics* pgraph_bk;	//背景用グラフィックス
	Graphics* pgraph_inf;	//情報表示用グラフィックス

	virtual HRESULT setup_obj() = 0;
	virtual void delete_obj() = 0;
};

class CMainPanelObj :CPanelObj 
{
public:
	CMainPanelObj(HWND _hwnd, Rect _rc) : CPanelObj(_hwnd, _rc) {
		setup_obj();
	}
	virtual ~CMainPanelObj() {
	}

	HRESULT setup_obj();
	void delete_obj();

static	CStringGdi* str_message;
static	CPbCtrl* pb_remote;
static	CSwitchImg* lmp_remote;
static	CRadioCtrl*	rdo_disp_mode;	//表示モード選択ラジオボタン
static	CStaticCtrl* txt_uid;
static	CPbCtrl* pb_auth;			//認証PB
static	CRadioCtrl*	radio_opt_wnd;		//オプションウィンドウラジオボタン


static	CCbCtrl*cb_estop;			//緊急停止チェックボックス
static	CSwitchImg*	lamp_estop;			//緊急停止ランプ
static	CPbCtrl* pb_ctrl_source;		//主幹PB
};


class CPanelBase {

private:
	static int _panel_id;
	static int _crane_id;

public:
	static CPanelBase* pPanel;
	static CMainPanelObj* pobj;
	
	CPanelBase(int crane_id, int panel_id) {
		setup_drawing_base();
	}
	virtual ~CPanelBase(void) {
		close_drawing_base();
	}


	HRESULT set_crane_id(int crane_id) {  _crane_id = crane_id; S_OK; }
	HRESULT set_panel_id(int panel_id) {  _panel_id = panel_id; S_OK; }
	int get_crane_id() { return _crane_id; }
	int get_panel_id() { return _panel_id; }

	HRESULT setup_drawing_base();
	void	close_drawing_base();
	HRESULT setup_graphic(int crane_id, int panel_id);
	void	close_graphic();
	HRESULT setup_panel(int crane_id, int panel_id);
	void	close_panel();

};