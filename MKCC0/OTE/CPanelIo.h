#pragma once
#include <Windows.h>
#include <string>
#include "COMMON_DEF.H"


//######################################################################
// Value
//######################################################################

#define ID_PANEL_ACTION_NON	0

#define ID_PANEL_POINT_DST		0
#define ID_PANEL_POINT_SRC_ON	1
#define ID_PANEL_POINT_SRC_OFF	2

#define VAL_PB_OFF_DELAY_COUNT	0x000A	//ボタンOFFディレイカウント値

#define VAL_TRIGGER_NON			0		//トリガ無し
#define VAL_TRIGGER_VALUE		3		//数値変更トリガ
#define VAL_TRIGGER_FLICK		2		//フリッカ
#define VAL_TRIGGER_ON			1		//ONトリガ
#define VAL_TRIGGER_OFF			-1		//OFFトリガ

#define N_VAL_RADIO_MAX			10		//RADIO PB最大数

template <class T> class CPnlParts {
public:
	CPnlParts() {}
	virtual ~CPnlParts() {}

public:
    T value;	    //現在値
 
	Point pt= { 0,0 };			//表示位置
	Size sz	= { 0,0 };			//表示サイズ
	std::wstring txt;

	HWND hWnd			= NULL;	//ウィンドウハンドル
	INT32 id			= 0;	//メニューID

	Rect rc;					//表示領域int
	RectF frc;					//表示領域float

public:

	HRESULT set_base(Point* ppt, Size* psz, LPCWSTR ptext) {
		pt = *ppt; sz = *psz; std::wstring _txt = ptext; txt = _txt; return S_OK;
	};


	virtual T get() { return value; }
	virtual HRESULT set(T _value) { value = _value;  return S_OK; }
	virtual HRESULT set_wnd(HWND _hWnd) { if (_hWnd == NULL) return S_FALSE;  hWnd = _hWnd; return S_OK; }
	
	INT32 get_id() { return id; }
	HRESULT set_id(INT32 _id) { id = _id; return S_OK; }

	virtual int chk_trig(T _value) {
		int chk = VAL_TRIGGER_NON;
		if (_value > value)	chk = VAL_TRIGGER_ON;	//ONトリガ
		if (_value < value)	chk = VAL_TRIGGER_OFF;	//OFFトリガ
		set(_value);
		return chk;
	}
};

/// <summary>
/// STATIC コントロール
/// </summary>
class CStaticCtrl : public CPnlParts<INT16> {	//スタティック

public:
	CStaticCtrl(INT32 _id, Point* ppt, Size* psz, LPCWSTR ptext) {
		set_id(_id);
		set_base(ppt, psz, ptext);
	}
	virtual ~CStaticCtrl() {}
	int  update(LPCWSTR ptext) {				//更新 
		return SetWindowText(hWnd,ptext); //戻り値はカウント値
	}
};

/// <summary>
/// PB コントロール
/// ボタンのトリガでカウントをセット→updateメソッドを定周期呼び出しでカウントダウン
/// </summary>
class CPbCtrl : public CPnlParts<INT16> {	//ボタン

public:
	CPbCtrl( INT32 _id, Point* ppt, Size* psz, LPCWSTR ptext,Graphics* _pgraphics,Pen* ppenON,Pen* ppenOFF) { 
		set_id(_id) ;
		set_base(ppt, psz, ptext);
		rc = { pt.X,pt.Y,sz.Width,sz.Height };
		frc = { (REAL)rc.X,(REAL)rc.X,(REAL)rc.Width,(REAL)rc.Height };
		pPenOn = ppenON; pPenOff = ppenOFF;
		pgraphics = _pgraphics;
	}
	virtual ~CPbCtrl() {}

	Pen* pPenOn, *pPenOff;
	INT16 status = 0;
	Graphics* pgraphics;

	INT16 count_off_set_val = VAL_PB_OFF_DELAY_COUNT;	//オフディレイカウント値
	INT16 update(bool is_trigger) {				//更新 
		if (is_trigger) set(count_off_set_val); //トリガーONでディレイカウント値セット
		else if (value > 0) value--;			//トリガー無しでカウントダウン
		else;

		disp_status();

		return get(); //戻り値はカウント値
	}

	virtual int chk_trig() {
		if (value != status) {						//status=カウントダウン前のvalueのホールド値が前回値と異なる
			if ((value == 0) || (status == 0)) {	//status　or　valueが0⇒立ち上がりまたは立下り
				status = value; return L_ON;		//statusを更新してTriger ON
			}
		}
		return L_OFF;
	}
	HRESULT disp_status() { //枠線を表示させたくないときはPENをONもOFFのPENにする
		if (chk_trig()) {	//トリガのタイミングvalueが0⇒！0　!0⇒0で枠線描画　valueの値でpenを変える
			Pen* ppen = pPenOff;
			if (value != 0) ppen = pPenOn;
			pgraphics->DrawRectangle(ppen, rc);
		}
		return S_OK;
	}	

	//ボタンオフディレイのカウント値更新
	void set_off_delay(INT16 val_delay) { count_off_set_val = val_delay; return; }//

	bool is_on() { //ON状態か？
		if (value > 0) return true;	
		else return false;
	}
};

/// <summary>
/// CECK BOX コントロール
/// </summary>
class CCbCtrl : public CPnlParts<INT16> {	//ボタン

public:
	CCbCtrl(INT32 _id,Point* ppt, Size* psz, LPWSTR ptext, Graphics* _pgraphics, Pen* ppenON, Pen* ppenOFF)
	{
		set_id(_id); 
		set_base(ppt, psz, ptext);
		value = BST_UNCHECKED;
		rc = { pt.X,pt.Y,sz.Width,sz.Height };
		frc = { (REAL)rc.X,(REAL)rc.X,(REAL)rc.Width,(REAL)rc.Height };
		pPenOn = ppenON; pPenOff = ppenOFF;
		//オーナードローの時、グラフィックスのポインタはNULL
		if (_pgraphics == NULL) {
			is_owner_draw = true;
		}
		else {
			pgraphics = _pgraphics;
		}
	}
	virtual ~CCbCtrl() {}

	Pen* pPenOn, * pPenOff;
	INT16 status = 0;
	Graphics* pgraphics;
	BOOL is_owner_draw = false;

	//コントロールの状態でget,set
	virtual INT16 get() { 
		if (!is_owner_draw) {
			if (BST_CHECKED == SendMessage(hWnd, BM_GETCHECK, 0, 0)) {
				value = BST_CHECKED;
			}
			else {
				value = BST_UNCHECKED;
			}
		}
		return value;
	}

	virtual HRESULT set(INT16 val) { 
		if (is_owner_draw)			value = val;
		else if (val==BST_CHECKED)	SendMessage(hWnd, BM_SETCHECK, BST_CHECKED, 0); 
		else						SendMessage(hWnd, BM_SETCHECK, BST_UNCHECKED, 0);
		get();
		return S_OK; 
	};

	INT16 update() {				//更新 
		if (!is_owner_draw) {//オーナードロータイプでなければON/OFF枠表示
			Pen* ppen = pPenOff;
			if (value != 0) ppen = pPenOn;
			pgraphics->DrawRectangle(ppen, rc);
		}
		return get(); //戻り値はカウント値
	}
	//INT16 update(INT16 signal) {	//更新 
	//	if (!is_owner_draw) {//オーナードロータイプでなければON/OFF枠表示
	//		Pen* ppen = pPenOff;
	//		if (value != 0) ppen = pPenOn;
	//		pgraphics->DrawRectangle(ppen, rc);
	//	}
	//	else {				//オーナードローで信号表示に応じて枠線表示
	//		if (signal) pgraphics->DrawRectangle(pPenOn, rc);
	//		else		pgraphics->DrawRectangle(pPenOff, rc);
	//	}
	//	return signal;
	//}
};

/// <summary>
/// RADIO コントロール
/// </summary>
class CRadioCtrl : public CPnlParts<INT16> {	//ボタン

private:
	INT16 n_radio = 0;	//ラジオボタンの数

public:

	CRadioCtrl(int _n_radio, CCbCtrl** ppcb) {
		
		if (n_radio > N_VAL_RADIO_MAX) n_radio = N_VAL_RADIO_MAX;
		else if (_n_radio < 0)			n_radio = 0;
		else							n_radio = _n_radio;
		if(S_OK == set_radio(ppcb, n_radio))
			pradio[0]->set(BST_CHECKED);
	}
	virtual ~CRadioCtrl() {}

	CCbCtrl* pradio[N_VAL_RADIO_MAX] = {					//チェックボタン型のポインタ配列
		NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL
	};

	HRESULT set_radio(CCbCtrl** ppcb, int n_radio) {
		if ((n_radio > 0) && (n_radio < N_VAL_RADIO_MAX)) {
			for (int i = 0; i < n_radio; i++) {
				pradio[i] = ppcb[i];
			}
		}
		else {
			return S_FALSE;
		}

		return S_OK;
	}

	INT16 update(bool is_non_owner_draw) {				//更新 
		INT16 val = check();
		if (is_non_owner_draw) {//オーナードロータイプでなければON/OFF枠表示
			for (int i = 0; i < n_radio; i++) {
				pradio[i]->update();
			}
		}
		return val; //戻り値はID値
	}

	INT16 check() { 
		for (int i = 0; i < n_radio; i++) {
			if (pradio[i]->get() == BST_CHECKED) {
				value = i;
			}
		}
		return -1;
	}

	virtual HRESULT set(INT16 val) {
		if ((val < 0) || (val >= n_radio)) return S_FALSE;
		pradio[val]->set(L_ON);	//Valueのセット
		value = val;
		return S_OK;
	};

};

#define ID_PANEL_LAMP_OFF			0x00
#define ID_PANEL_LAMP_ON			0x01
#define ID_PANEL_LAMP_FLICK			0xFF

/// <summary>
/// ImageLamp
/// 指定画像を切替表示
/// Imageを読み込んでグラフィックに書き込み
/// 登録1番目の画像をOFF、2番目の画像をONで予約
/// </summary>
#define N_IMG_SWITCH_MAX			8

class CSwitchImg : public CPnlParts<INT16> {
public:
	CSwitchImg(
		INT32 _id, Point* ppt, Size* psz, LPWSTR ptext,
		Image** _ppimg, INT32 _n_switch,INT32 _fcount,Graphics* _pgraphic
	)
	{
		set_id(_id);
		set_base(ppt, psz, ptext);
		for (int i = 0; i < N_IMG_SWITCH_MAX; i++) {
			if (i < _n_switch) pimg[i] = _ppimg[i];
			else pimg[i] = NULL;
		}
		n_switch = _n_switch;
		fcount = _fcount;

		Rect _rc(pt.X, pt.Y, sz.Width, sz.Height); rc = _rc;
		RectF _frc((REAL)pt.X, (REAL)pt.Y, (REAL)sz.Width, (REAL)sz.Height); frc = _frc;

		rect = { pt.X,pt.Y,pt.X + sz.Width,pt.Y + sz.Height };

		pgraphic = _pgraphic;
		value = ID_PANEL_LAMP_OFF;
	}
	virtual ~CSwitchImg() {
		for (int i = 0; i < N_IMG_SWITCH_MAX; i++) {
			delete pimg[i];
		}
	}

	Image* pimg[N_IMG_SWITCH_MAX];
	INT32 n_switch = 2;	//スイッチ数
	INT32 n_flick = 2;	//フリッカさせるスイッチ数

	INT32 count = 0;	//点滅タイミング用
	INT32 fcount = 3;	//表示カウント１イメージ分
	INT16 id_disp = 0;	//表示イメージID
	INT32 list_flick_id[N_IMG_SWITCH_MAX] = { 0,1,0,0,0,0,0,0 };

	StringFormat* pStrFormat;
	Font* pFont;
	Rect rc;
	RECT rect;
	RectF frc;
	Graphics* pgraphic;

	Image* pimg_disp;

	HRESULT setup_flick(INT32 _n_flick, INT32 _fcount, INT32* pid) {
		fcount = _fcount;
		n_flick = _n_flick;
		for (int i = 0; i < n_flick; i++) {//フリッカさせる画像の順序取り込み
			if (i < N_IMG_SWITCH_MAX) list_flick_id[i] = pid[i];
		}
		return S_OK;
	}

	virtual int chk_trig(INT16 id) {//トリガは表示イメージのid
		if (id_disp != id) {
			id_disp = id; return VAL_TRIGGER_VALUE;
		}
		return VAL_TRIGGER_NON;
	}

	virtual INT16 get() {
		return id_disp;
	}

	HRESULT update() {
		INT16 id = 0;
		if (value != ID_PANEL_LAMP_FLICK) {
			id_disp = value;
			pgraphic->DrawImage(pimg[id_disp], rc);
			InvalidateRect(hWnd, &rect, FALSE);
			return S_OK;
		}
		else {
			count++; if (count > fcount) {
				count = 0;
				id_disp++; if (id_disp >= n_flick) id_disp = 0;
				pgraphic->DrawImage(pimg[list_flick_id[id_disp]], rc);
				InvalidateRect(hWnd, &rect, FALSE);
			}
			return S_OK;
		}
		return S_FALSE;
	}

};

/// <summary>
/// LampCtrl
/// Common Controlへの描画（OWNER DRAW）
/// </summary>
class CLampCtrl : public CPnlParts<INT16> {
public:
	CLampCtrl(
		INT32 _id, Point* ppt, Size* psz, LPWSTR ptext,
		Image** _ppimg, INT32 _n_switch,
		INT32 _fcount
	)
	{
		set_id(_id);
		set_base(ppt, psz, ptext);
		for (int i = 0; i < N_IMG_SWITCH_MAX; i++) {
			if (i < _n_switch) pimg[i] = _ppimg[i];
			else pimg[i] = NULL;
		}
		n_switch = _n_switch;
		fcount = _fcount;

		Rect _rc(0, 0, sz.Width, sz.Height); rc = _rc;
		RectF _frc(0, 0, sz.Width, sz.Height); frc = _frc;

		value = ID_PANEL_LAMP_OFF;
	}
	virtual ~CLampCtrl() {
		for (int i = 0; i < N_IMG_SWITCH_MAX; i++) {
			delete pimg[i];
		}
	}

	Image* pimg[N_IMG_SWITCH_MAX];
	INT32 n_switch = 2;	//スイッチ数
	INT32 n_flick = 2;	//フリッカさせるスイッチ数

	INT32 count = 0;	//点滅タイミング用
	INT32 fcount = 3;	//表示カウント１イメージ分
	INT16 id_disp = 0;	//表示イメージID
	INT32 list_flick_id[N_IMG_SWITCH_MAX] = { 0,1,0,0,0,0,0,0 };

	StringFormat* pStrFormat;
	SolidBrush* pTxtBrush;
	Font* pFont;
	Rect rc;
	RectF frc;

	Image* pimg_disp;

	HRESULT set_txt_items(Font* pfont, StringFormat* pformat, SolidBrush* pbrush) {
		pFont = pfont; pStrFormat = pformat; pTxtBrush = pbrush;
		return S_OK;
	}
	HRESULT set_ctrl(CPnlParts* pCtrl) {
		if (pCtrl == NULL)return S_FALSE;
		hWnd = pCtrl->hWnd;
		rc = { 0,0,pCtrl->sz.Width, pCtrl->sz.Height };
		frc = { 0.0,0.0,(REAL)rc.Width,(REAL)rc.Height };
		return S_OK;
	}

	HRESULT setup_flick(INT32 _n_flick, INT32 _fcount, INT32* pid) {
		fcount = _fcount;
		n_flick = _n_flick;
		for (int i = 0; i < n_flick; i++) {//フリッカさせる画像の順序取り込み
			if (i < N_IMG_SWITCH_MAX) list_flick_id[i] = pid[i];
		}
		return S_OK;
	}

	virtual int chk_trig(INT16 id) {//トリガは表示イメージのid
		if (id_disp != id) {
			id_disp = id; return VAL_TRIGGER_VALUE;
		}
		return VAL_TRIGGER_NON;
	}

	virtual INT16 get() {
		return id_disp;
	}
	
	HRESULT update() {//WM_DRAWITEMをInvalidiateRectで呼び出してvalueにセットしたindexの画像を描画
		INT16 id = 0;
		if (value != ID_PANEL_LAMP_FLICK) {
			id = value;	if (chk_trig(id)) InvalidateRect(hWnd, NULL, TRUE);
			return S_OK;
		}
		else {
			count++; if (count > fcount) {
				count = 0;
				id_disp++; if (id_disp >= n_flick) id_disp = 0;
				InvalidateRect(hWnd, NULL, TRUE);
			}
			return S_OK;
		}
		return S_FALSE;
	}
};

/// <summary>
/// DCLamp
/// ON/OFFで画像を切替
/// ソースデバイスコンテキストから切り出しBitBilt
/// </summary>
class CLampDC : public CPnlParts<INT16> {
public:
	CLampDC(
		INT32 _id, Point* ppt, Size* psz, LPWSTR ptext,
		HDC _dc_dst, HDC _dc_src, Rect _rc_src_on, Rect _rc_src_off,
		INT32 _fcount
	)
	{
		set_id(_id);
		set_base(ppt, psz, ptext);
		dc_dst = _dc_dst;
		dc_src = _dc_src;
		rc_src_on = _rc_src_on;
		rc_src_off = _rc_src_off;
		fcount = _fcount;

		value = ID_PANEL_LAMP_OFF;
	}
	virtual ~CLampDC() {}

	HDC dc_dst = NULL;
	HDC dc_src = NULL;
	Rect rc_src_on = { 0,0,0,0 };
	Rect rc_src_off = { 0,0,0,0 };

	INT32 count = 0;			//点滅タイミング用
	INT32 fcount = 6;			//点滅カウント一周期分
	INT32 fcount_on = 3;		//点滅カウントON

	virtual HRESULT set(INT16 code) {

		Rect rc_src;
		if (code == ID_PANEL_LAMP_OFF) {
			value = ID_PANEL_LAMP_OFF;
			rc_src = rc_src_off;
			count = 0;
		}
		else if (code == ID_PANEL_LAMP_ON) {
			value = ID_PANEL_LAMP_ON;
			rc_src = rc_src_on;
			count = 0;
		}
		else if (code == ID_PANEL_LAMP_FLICK) {
			count++; if (count >= fcount) count = 0;
			if (count > fcount_on) {
				value = ID_PANEL_LAMP_OFF;
				rc_src = rc_src_off;
			}
			else {
				value = ID_PANEL_LAMP_ON;
				rc_src = rc_src_on;
			}
		}
		else return S_FALSE;
		
		BitBlt(dc_dst, pt.X, pt.Y, sz.Width, sz.Height, dc_src,rc_src.X,rc_src.Y, SRCCOPY);

		return S_OK;
	}
};

/// <summary>
/// StringGdi
/// Gdi+でテキスト出力
/// ptは文字列の表示位置,Sizeは背景矩形のサイズ、ptextは、初期value値
/// </summary>

class CStringGdi : public CPnlParts<std::wstring> {
public:
	CStringGdi(
		INT32 _id, Point* ppt, Size* psz, LPWSTR ptext, 
		Graphics* _pgraphic,StringFormat* _pStrFormat,SolidBrush* _pTxtBrush,Font* _pFont
	)
	{
		set_id(_id);
		set_base(ppt, psz, ptext);
		pgraphic = _pgraphic;
		pStrFormat = _pStrFormat; pTxtBrush = _pTxtBrush; pFont = _pFont; 
		Rect _rc(pt.X, pt.Y, sz.Width, sz.Height); rc = _rc;
		RectF _frc((REAL)pt.X, (REAL)pt.Y, (REAL)sz.Width, (REAL)sz.Height); frc = _frc;
		value = txt;	
	}
	virtual ~CStringGdi() {}

	Graphics* pgraphic;
	StringFormat* pStrFormat;
	SolidBrush* pTxtBrush;	//テキストブラシ
	SolidBrush* pBkBrush;	//背景ブラシ
	Font* pFont;
	Rect rc;
	RectF frc,frc_bk;

	HRESULT set_str_items(Font* pfont, StringFormat* pformat, SolidBrush* pbrush) {
		pFont = pfont; pStrFormat = pformat; pTxtBrush = pbrush;
		return S_OK;
	}

	HRESULT update(Graphics* _pgraphic, const LPCWSTR pstr) {
		if ((_pgraphic == NULL) || (pstr == NULL))return S_FALSE;
		value = pstr;
		pgraphic = _pgraphic;
		pgraphic->DrawString(value.c_str(), -1, pFont, frc, pStrFormat, pTxtBrush);
		return S_OK;
	}

	HRESULT update(Graphics* _pgraphic) {//描画先DC変更
		update(pgraphic, value.c_str());
		return S_OK;
	}

	HRESULT update() {//再描画
		update(pgraphic, value.c_str());
		return S_OK;
	}

	HRESULT update(const LPCWSTR pstr) {//テキスト変更
		update(pgraphic, pstr);
		return S_OK;
	}

	HRESULT update(SolidBrush* pbr_bk) {//背景のみ変更
		pgraphic->FillRectangle(pbr_bk, rc);
		update(pgraphic, value.c_str());
		return S_OK;
	}

	HRESULT update(const LPCWSTR pstr,SolidBrush* pbr_bk) {//テキストと背景変更
		pgraphic->FillRectangle(pbr_bk, rc);
		update(pstr);
		return S_OK;
	}

};


