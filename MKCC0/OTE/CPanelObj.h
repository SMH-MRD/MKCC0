#pragma once
#include "CPanelIo.h"
#include "CPanelGr.h"

using namespace std;

#define ID_MAIN_PNL_OBJ_BASE				60000
#define ID_MAIN_PNL_OBJ_STR_MESSAGE			60000
#define ID_MAIN_PNL_OBJ_PB_REMOTE			60001
#define ID_MAIN_PNL_OBJ_LMP_REMOTE			60002
#define ID_MAIN_PNL_OBJ_TXT_UID				60003
#define ID_MAIN_PNL_OBJ_PB_AUTH				60004
#define ID_MAIN_PNL_OBJ_TXT_PC_COM_STAT		60006
#define ID_MAIN_PNL_OBJ_TXT_PLC_COM_STAT	60007
#define ID_MAIN_PNL_OBJ_LMP_PCR				60008
#define ID_MAIN_PNL_OBJ_LMP_PCS				60009
#define ID_MAIN_PNL_OBJ_LMP_PLCR			60010
#define ID_MAIN_PNL_OBJ_LMP_PLCS			60011
#define ID_MAIN_PNL_OBJ_CB_ESTOP			60012
#define ID_MAIN_PNL_OBJ_LMP_ESTOP			60013
#define ID_MAIN_PNL_OBJ_PB_SYUKAN_ON		60014
#define ID_MAIN_PNL_OBJ_LMP_SYUKAN_ON		60015
#define ID_MAIN_PNL_OBJ_PB_SYUKAN_OFF		60016
#define ID_MAIN_PNL_OBJ_LMP_SYUKAN_OFF		60017
#define ID_MAIN_PNL_OBJ_CB_PNL_NOTCH		60018
#define ID_MAIN_PNL_OBJ_PB_PAD_MODE			60019
#define ID_MAIN_PNL_OBJ_LMP_PAD_MODE		60020
#define ID_MAIN_PNL_OBJ_PB_ASSIST_FUNC		60021
#define ID_MAIN_PNL_OBJ_TXT_OPE_TYPE		60022
#define ID_MAIN_PNL_OBJ_PB_OTE_TYPE_WND		60023
#define ID_MAIN_PNL_OBJ_TXT_LINK_CRANE		60024
#define ID_MAIN_PNL_OBJ_PB_CRANE_SEL_WND	60025

#define ID_MAIN_PNL_OBJ_RDO_DISP_MODE1		60030
#define ID_MAIN_PNL_OBJ_RDO_DISP_MODE2		60031
#define ID_MAIN_PNL_OBJ_RDO_DISP_MODE		60032

#define ID_MAIN_PNL_OBJ_RDO_OPT_WND_FLT		60040
#define ID_MAIN_PNL_OBJ_RDO_OPT_WND_SET		60041
#define ID_MAIN_PNL_OBJ_RDO_OPT_WND_COM		60042
#define ID_MAIN_PNL_OBJ_RDO_OPT_WND_CAM		60043
#define ID_MAIN_PNL_OBJ_RDO_OPT_WND_STAT	60044
#define ID_MAIN_PNL_OBJ_RDO_OPT_WND_CLR		60045
#define ID_MAIN_PNL_OBJ_RDO_OPT_WND			60046

#define ID_MAIN_PNL_OBJ_PB_FRESET			60047
#define ID_MAIN_PNL_OBJ_LMP_FRESET			60048
#define ID_MAIN_PNL_OBJ_TXT_FRESET			60049

#define ID_PNL_SOCK_STAT_CLOSED				0//OFF
#define ID_PNL_SOCK_STAT_INIT				1//青
#define ID_PNL_SOCK_STAT_INIT_ERROR			2//橙
#define ID_PNL_SOCK_STAT_STANDBY			3//黄
#define ID_PNL_SOCK_STAT_ACT_RCV			4//緑
#define ID_PNL_SOCK_STAT_ACT_SND			4//緑
#define ID_PNL_SOCK_STAT_RCV_ERR			5//赤
#define ID_PNL_SOCK_STAT_SND_ERR			5//赤


#define N_MAIN_PNL_WCHAR	128

//各クレーン用のオブジェクト初期化用プロパティ値保持用構造体
struct ST_OBJ_PROPERTY {
	INT32 id;	//ID
	Point pt;	//表示位置
	Size sz;	//表示サイズ
	WCHAR txt[N_MAIN_PNL_WCHAR];	//表示文字列
};

class CPanelObjBase {
public:

	CPanelObjBase(HWND _hwnd);

	virtual ~CPanelObjBase(); 

	HRESULT hr = S_OK;
	int panel_code;	//パネルコード

	HWND hPnlWnd;			//パネルのウィンドウハンドル
	Rect rc_panel;			//パネルの表示位置

	HBITMAP hBmp0;			//イメージ素材用ビットマップ
	HBITMAP hBmp_img;		//イメージ素材用ビットマップ
	HBITMAP hBmp_bk;		//背景用ビットマップ
	HBITMAP hBmp_inf;		//情報表示用ビットマップ

	Bitmap* pbmp0;			//背景用ビットマップ
	Bitmap* pbmp_img;		//背景用ビットマップ
	Bitmap* pbmp_bk;		//背景用ビットマップ
	Bitmap* pbmp_inf;		//背景用ビットマップ

	HDC hdc;				//パネルへ書き込み用DC
	HDC hdc_img;			//イメージ素材用DC
	HDC hdc_bk;				//背景用DC
	HDC hdc_inf;			//情報表示用DC

	Graphics* pgraphic;		//描画用グラフィックス
	Graphics* pgraphic_img;	//イメージ素材用グラフィックス
	Graphics* pgraphic_bk;	//背景用グラフィックス
	Graphics* pgraphic_inf;	//情報表示用グラフィックス

	SolidBrush* pBrushBk;	//背景塗りつぶし用ブラシ

	Color colorkey;				//画像を重ね合わせる時の透過色
	ImageAttributes attr;		//カラーキーを透過させる設定

	int set_panel_code(int code) { panel_code = code; return panel_code; };
	void set_bk_brush(SolidBrush* pbr) { pBrushBk = pbr; return; };
	virtual HRESULT setup_graphics(HWND hwnd);
	virtual void clear_graghics();

	virtual HRESULT setup_obj() = 0;
	virtual void delete_obj() = 0;

};

class CMainPanelObj :public CPanelObjBase
{
private:
	int crane_id;
public:
	CMainPanelObj(HWND _hwnd, int _crane_id) : CPanelObjBase(_hwnd) {
		crane_id = _crane_id;
		set_panel_code(ID_MAIN_PNL_OBJ_BASE);
		setup_obj();
	}
	virtual ~CMainPanelObj() {
	}

	CStringGdi	* str_message;
	CPbCtrl		* pb_remote;
	CLampCtrl	* lmp_remote;		//遠隔設定ランプ
	CStaticCtrl	* txt_uid;
	CPbCtrl		* pb_auth;			//認証PB
	CStaticCtrl* str_pc_com_stat;	//PC通信インジケータ用ラベル
	CStaticCtrl* str_plc_com_stat;	//PLC通信インジケータ用ラベル
	CSwitchImg	* lmp_pcr;			//PC通信インジケータ
	CSwitchImg	* lmp_pcs;			//PC通信インジケータ
	CSwitchImg	* lmp_plcr;			//PLC通信インジケータ
	CSwitchImg	* lmp_plcs;			//PLC通信インジケータ

	CCbCtrl		* cb_estop;			//緊急停止チェックボックス
	CLampCtrl	* lmp_estop;		//緊急停止ランプ
	CPbCtrl		* pb_syukan_on;		//主幹入PB
	CLampCtrl	* lmp_syukan_on;	//主幹入ランプ
	CPbCtrl		* pb_syukan_off;	//主幹切PB
	CLampCtrl	* lmp_syukan_off;	//主幹切ランプ
	CCbCtrl		* cb_pnl_notch;		//パネルノッチPB
	CPbCtrl		* pb_pad_mode;		//ジョイパッドモードPB
	CLampCtrl	* lmp_pad_mode;		//ジョイパッドモードランプ
	CPbCtrl		* pb_assist_func;	//アシスト機能設定
	CStaticCtrl	* txt_ote_type;		//端末タイプ表示テキスト
	CPbCtrl		* pb_ote_type_wnd;	//端末タイプ設定ウィンドウ
	CStaticCtrl	* txt_link_crane;	//接続先クレーン表示テキスト
	CPbCtrl		* pb_crane_sel_wnd;	//接続先クレーン選択ウィンドウ

	CCbCtrl		* cb_disp_mode1;	//表示モード選択ラジオボタン
	CCbCtrl		* cb_disp_mode2;	//表示モード選択ラジオボタン
	CRadioCtrl	* rdo_disp_mode;	//表示モード選択ラジオボタン

	CCbCtrl		* cb_opt_flt;	//オプションウィンドウラジオボタン
	CCbCtrl		* cb_opt_set;	//オプションウィンドウラジオボタン
	CCbCtrl		* cb_opt_com;	//オプションウィンドウラジオボタン
	CCbCtrl		* cb_opt_cam;	//オプションウィンドウラジオボタン
	CCbCtrl		* cb_opt_stat;	//オプションウィンドウラジオボタン
	CCbCtrl		* cb_opt_clr;	//オプションウィンドウラジオボタン
	CRadioCtrl	* rdo_opt_wnd;	//オプションウィンドウラジオボタン

	CPbCtrl		* pb_freset;			//故障リセットPB
	CLampCtrl	* lmp_freset;		//故障リセットランプ
	CStaticCtrl	* txt_freset;	//故障リセット表示テキスト

	virtual HRESULT setup_obj();
	virtual void delete_obj();
	void refresh_obj_graphics();//オブジェクトのグラフィックオブジェクトの設定更新
};


#define ID_SUB_PNL_SET_OBJ_BASE					60200
#define ID_SUB_PNL_SET_OBJ_RDO_MHSPD_0			60200
#define ID_SUB_PNL_SET_OBJ_RDO_MHSPD_1			60201
#define ID_SUB_PNL_SET_OBJ_RDO_MHSPD_2			60202
#define ID_SUB_PNL_SET_OBJ_RDO_MHSPD			60203
#define ID_SUB_PNL_SET_OBJ_LMP_MHSPD			60204

#define ID_SUB_PNL_SET_OBJ_RDO_BHR_0			60205
#define ID_SUB_PNL_SET_OBJ_RDO_BHR_1			60206
#define ID_SUB_PNL_SET_OBJ_RDO_BHR_2			60207
#define ID_SUB_PNL_SET_OBJ_RDO_BHR				60208
#define ID_SUB_PNL_SET_OBJ_LMP_BHR				60209

#define ID_SUB_PNL_STAT_OBJ_BASE				60300
#define ID_SUB_PNL_STAT_OBJ_PB_NEXT				60300
#define ID_SUB_PNL_STAT_OBJ_PB_BACK				60301
#define ID_SUB_PNL_STAT_OBJ_STATIC_MH_DIR		60302
#define ID_SUB_PNL_STAT_OBJ_STATIC_BH_DIR		60303
#define ID_SUB_PNL_STAT_OBJ_STATIC_SL_DIR		60304
#define ID_SUB_PNL_STAT_OBJ_STATIC_GT_DIR		60305
#define ID_SUB_PNL_STAT_OBJ_STATIC_MH_TG_V		60306
#define ID_SUB_PNL_STAT_OBJ_STATIC_BH_TG_V		60307
#define ID_SUB_PNL_STAT_OBJ_STATIC_SL_TG_V		60308
#define ID_SUB_PNL_STAT_OBJ_STATIC_GT_TG_V		60309
#define ID_SUB_PNL_STAT_OBJ_STATIC_MH_REF_V		60310
#define ID_SUB_PNL_STAT_OBJ_STATIC_BH_REF_V		60311
#define ID_SUB_PNL_STAT_OBJ_STATIC_SL_REF_V		60312
#define ID_SUB_PNL_STAT_OBJ_STATIC_GT_REF_V		60313
#define ID_SUB_PNL_STAT_OBJ_STATIC_MH_FB_V		60314
#define ID_SUB_PNL_STAT_OBJ_STATIC_BH_FB_V		60315
#define ID_SUB_PNL_STAT_OBJ_STATIC_SL_FB_V		60316
#define ID_SUB_PNL_STAT_OBJ_STATIC_GT_FB_V		60317
#define ID_SUB_PNL_STAT_OBJ_STATIC_MH_REF_TRQ	60318
#define ID_SUB_PNL_STAT_OBJ_STATIC_BH_REF_TRQ	60319

#define ID_SUB_PNL_FLT_OBJ_BASE					60330
#define ID_SUB_PNL_FLT_OBJ_IMG_BK				60330
#define ID_SUB_PNL_FLT_OBJ_PB_NEXT				60331
#define ID_SUB_PNL_FLT_OBJ_CB_HISTORY			60332
#define ID_SUB_PNL_FLT_OBJ_CB_HEAVY1			60333
#define ID_SUB_PNL_FLT_OBJ_CB_LITE				60334
#define ID_SUB_PNL_FLT_OBJ_CB_IL				60335
#define ID_SUB_PNL_FLT_OBJ_CB_BYPASS			60336
#define ID_SUB_PNL_OBJ_STR_FLT_MESSAGE			60337
#define ID_SUB_PNL_FLT_OBJ_CB_HEAVY2			60338
#define ID_SUB_PNL_FLT_OBJ_CB_HEAVY3			60339
#define ID_SUB_PNL_FLT_OBJ_PB_PLCMAP			60340
#define ID_SUB_PNL_FLT_OBJ_LV_FAULTS			60341


class CSubPanelObj :public CPanelObjBase
{
private:
	int crane_id;

public:
	CSubPanelObj(HWND _hwnd, int _crane_id) : CPanelObjBase(_hwnd) {
		crane_id = _crane_id;
		setup_obj();
	}
	virtual ~CSubPanelObj() {
	}
	int i_disp_page = 0;
	int n_disp_page = 1;

	//設定サブウィンドウ
	CCbCtrl*	cb_mh_spd_mode0;	//主巻速度モード選択ラジオボタン
	CCbCtrl*	cb_mh_spd_mode1;	//主巻速度モード選択ラジオボタン
	CCbCtrl*	cb_mh_spd_mode2;	//主巻速度モード選択ラジオボタン
	CRadioCtrl* rdo_mh_spd_mode;	//主巻速度モード選択ラジオボタン
	CSwitchImg*	lmp_mh_spd_mode;	//緊急停止ランプ

	CCbCtrl*	cb_bh_r_mode0;		//主巻速度モード選択ラジオボタン
	CCbCtrl*	cb_bh_r_mode1;		//主巻速度モード選択ラジオボタン
	CCbCtrl*	cb_bh_r_mode2;		//主巻速度モード選択ラジオボタン
	CRadioCtrl* rdo_bh_r_mode;		//主巻速度モード選択ラジオボタン
	CSwitchImg*	lmp_bh_r_mode;		//緊急停止ランプ

	//状態表示サブウィンドウのオブジェクト
	CPbCtrl*	pb_stat_next;		//次表示PB
	CPbCtrl*	pb_stat_back;		//前表示PB

	CStaticCtrl* st_mh_notch_dir;
	CStaticCtrl* st_bh_notch_dir;
	CStaticCtrl* st_sl_notch_dir;
	CStaticCtrl* st_gt_notch_dir;
	CStaticCtrl* st_mh_target_v;
	CStaticCtrl* st_bh_target_v;
	CStaticCtrl* st_sl_target_v;
	CStaticCtrl* st_gt_target_v;
	CStaticCtrl* st_mh_ref_v;
	CStaticCtrl* st_bh_ref_v;
	CStaticCtrl* st_sl_ref_v;
	CStaticCtrl* st_gt_ref_v;
	CStaticCtrl* st_mh_fb_v;
	CStaticCtrl* st_bh_fb_v;
	CStaticCtrl* st_sl_fb_v;
	CStaticCtrl* st_gt_fb_v;
	CStaticCtrl* st_mh_ref_trq;
	CStaticCtrl* st_bh_ref_trq;

	//故障表示サブウィンドウのオブジェクト
	CSwitchImg* img_flt_bk;			//PLC通信インジケータ
	CPbCtrl* pb_flt_next;			//次表示PB
	CCbCtrl* cb_disp_history;		//履歴表示チェックBOX
	CCbCtrl* cb_disp_interlock;		//主巻速度モード選択ラジオボタン
	CCbCtrl* cb_disp_flt_light;		//軽故障表示チェックボタン
	CCbCtrl* cb_disp_flt_heavy1;	//重故障1表示チェックボタン
	CCbCtrl* cb_flt_bypass;			//ILバイパスチェックボタン
	CStringGdi* str_flt_message;	//故障表示メッセージラベル
	CCbCtrl* cb_disp_flt_heavy2;	//重故障2表示チェックボタン
	CCbCtrl* cb_disp_flt_heavy3;	//重故障3表示チェックボタン
	CPbCtrl* pb_disp_flt_plcmap;	//PLC faulrmap表示チェックボタン
	CListViewCtrl* lv_flt_list;		//故障リストビュー
	INT16 flt_req_code = 0;			//故障リスト更新要求コード


	virtual HRESULT setup_obj();
	virtual void delete_obj();
	void refresh_obj_graphics();//オブジェクトのグラフィックオブジェクトの設定更新
};

#define ID_GWIN_MAIN_OBJ_BASE				60800
#define ID_GWIN_MAIN_OBJ_IMG_BK				60800
#define ID_GWIN_MAIN_OBJ_IMG_BOOM_XY		60801
#define ID_GWIN_MAIN_OBJ_IMG_GT_BASE		60802
#define ID_GWIN_MAIN_OBJ_IMG_POTAL			60803

#define ID_GWIN_MAIN_OBJ_STR_POS_MH			60820		
#define ID_GWIN_MAIN_OBJ_STR_POS_BH			60821		
#define ID_GWIN_MAIN_OBJ_STR_POS_SL			60822		
#define ID_GWIN_MAIN_OBJ_STR_POS_GT			60823		

class CGWindowObj :public CPanelObjBase
{
private:
	int crane_id;

public:
	CGWindowObj(HWND _hwnd, int _crane_id) : CPanelObjBase(_hwnd) {
		crane_id = _crane_id;
		setup_obj();
	}
	virtual ~CGWindowObj() {
	}

	//グラフィックメインウィンドウ
	CSwitchImg* lmg_bk_gwindow;		//グラフィックウィンドウの背景
	CSwitchImg* lmg_crane_bm_xy;	//ブーム上面
	CSwitchImg* lmg_crane_potal;	//ポータル部

	CSwitchImg* lmg_crane_gt_base;	//走行装置
	CStringGdi* str_pos_mh;			//主巻高さ
	CStringGdi* str_pos_bh;			//旋回半径
	CStringGdi* str_pos_sl;			//旋回角度
	CStringGdi* str_pos_gt;			//走行位置


	//グラフィックサブウィンドウ
	CSwitchImg* lmg_crane_bm_yz;	//ブーム側面


	virtual HRESULT setup_obj();
	virtual void delete_obj();
	void refresh_obj_graphics();//オブジェクトのグラフィックオブジェクトの設定更新
};

