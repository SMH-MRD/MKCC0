#pragma once

#include "SPEC_DEF.h"
#include "COMMON_DEF.h"
#include "PLC_DEF.h"
#include "CPanelObj.H"
#include "SmemOte.H"

/// <summary>
/// CPanelBase クラスは、アプリケーションウィンドウに配置するオブジェクトを定義するベースクラスです。
/// MainWindow,SubWindow,GraphicWindow用のオブジェクトを保持するクラスのインスタンスを保持して
/// 各ウィンドウのコールバック関数から利用出来るようにします。
/// </summary>
class CPanelBase {

private:
	static int _panel_id;	//適用元のパネルの種別（操作卓用,タブレット端末用など）
	static int _crane_id;	//クレーンID（HHGH29,HHGH30など）
public:
	static CPanelBase* pPanel;
	static CMainPanelObj*		pmainobjs;
	static CSubPanelObj*		psubobjs;
	static CGWindowObj*			pgwinobjs;
	static CGSubWindowObj*		pgsubwinobjs;
	static LPST_DRAWING_BASE	pdrawing_items;
	static LPST_OTE_UI			puiif;
	static HRESULT hr_init_setting;
	//メインウィンドウ生成時にWM＿CREATEでインスタンス生成
	CPanelBase(int crane_id, int panel_id,HWND hwnd_parent) {
		_panel_id=panel_id;
		_crane_id = crane_id;
		pmainobjs = new CMainPanelObj(hwnd_parent,crane_id);
		//親ウィンドウの設定はメインパネルからインスタンス生成されるときに再設定が必要
		psubobjs = new CSubPanelObj(hwnd_parent, crane_id);
		pgwinobjs = new CGWindowObj(hwnd_parent, crane_id);
		pgsubwinobjs = new CGSubWindowObj(hwnd_parent, crane_id);
	}
	virtual ~CPanelBase(void) {
		close_drawing_base();
	}

	HRESULT set_crane_id(int crane_id) {  _crane_id = crane_id; return S_OK; }
	HRESULT set_panel_id(int panel_id) {  _panel_id = panel_id; return S_OK; }
	int get_crane_id() { return _crane_id; }
	int get_panel_id() { return _panel_id; }

	static HRESULT setup_common_base(LPST_OTE_UI pui);
	void	close_drawing_base();
	HRESULT setup_graphic();
	void	close_graphic();
	HRESULT setup_panel();
	void	close_panel();
};

