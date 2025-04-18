#pragma once
#include <Windows.h>
#include <string>
#include "COMMON_DEF.H"


//######################################################################
// Value
//######################################################################

#define CODE_VALUE_ACTION_NON  0

#define ID_VALUE_OBJ_MAIN	0
#define ID_VALUE_OBJ_SUB	1

#define ID_VALUE_POINT_DST		0
#define ID_VALUE_POINT_SRC_ON	1
#define ID_VALUE_POINT_SRC_OFF	2

#define ID_SOURCE_LAMP_ON		0
#define ID_SOURCE_LAMP_OFF		1


#define VAL_PB_OFF_DELAY_COUNT	0x0002	//ボタンOFFディレイカウント値

#define VAL_TRIGGER_NON			0		//トリガ無し
#define VAL_TRIGGER_ON			1		//ONトリガ
#define VAL_TRIGGER_OFF			-1		//OFFトリガ

#define N_VAL_RADIO_MAX			10		//RADIO PB最大数

template <class T> class CPnlParts {
public:
	CPnlParts() { value = 0;}
	virtual ~CPnlParts() {}

public:
    T value;	    //現在値
 
	Point pt= { 0,0 };//表示位置
	Size sz	= { 0,0 };//表示サイズ
	std::wstring txt;

	HWND hWnd			= NULL;	//ウィンドウハンドル
	INT32 id			= 0;	//メニューID

public:

	HRESULT set_base(Point* ppt, Size* psz, LPCWSTR ptext) {
		pt = *ppt; sz = *psz; std::wstring _txt = ptext; txt = _txt; return S_OK;
	};


	virtual T get() { return value; }
	HRESULT set(T _value) { value = _value;  return S_OK; }

	HRESULT set_wnd(HWND _hWnd) { if (_hWnd == NULL) return S_FALSE;  hWnd = _hWnd; return S_OK; }
	
	INT32 get_id() { return id; }
	HRESULT set_id(INT32 _id) { id = _id; return S_OK; }

	int chk_trig(T _value) {
		int chk = VAL_TRIGGER_NON;
		if (_value > value)	chk = VAL_TRIGGER_ON;	//ONトリガ
		if (_value < value)	chk = VAL_TRIGGER_OFF;	//OFFトリガ
		set(_value);
		return chk;
	}
};

/// <summary>
/// STATIC コントロール
/// ボタンのトリガでカウントをセット→updateメソッドを定周期呼び出しでカウントダウン
/// </summary>
class CStaticCtrl : public CPnlParts<INT16> {	//スタティック

public:
	CStaticCtrl(INT32 _id, Point* ppt, Size* psz, LPCWSTR ptext) {
		set_id(_id);
		set_base(ppt, psz, L"");
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
	CPbCtrl( INT32 _id, Point* ppt, Size* psz, LPWSTR ptext) { 
		set_id(_id) ;
		set_base(ppt, psz, ptext);
	}
	virtual ~CPbCtrl() {}

	INT16 count_off_set_val = VAL_PB_OFF_DELAY_COUNT;	//オフディレイカウント値
	INT16 update(bool is_trigger) {				//更新 
		if (is_trigger) set(count_off_set_val); //トリガーONでディレイカウント値セット
		else if (value > 0) value--;			//トリガー無しでカウントダウン
		else;
		return get(); //戻り値はカウント値
	}

	void set_off_delay(INT16 val_delay) { count_off_set_val = val_delay; return; }

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
	CCbCtrl(INT32 _id,Point* ppt, Size* psz, LPWSTR ptext)
	{
		set_id(_id); 
		set_base(ppt, psz, ptext);
	}
	virtual ~CCbCtrl() {}

	virtual INT16 get() { value = SendMessage(hWnd, BM_GETCHECK, 0, 0); return value; }
	
	virtual HRESULT set(INT16 val) { 
		if (val)	SendMessage(hWnd, BM_SETCHECK, BST_CHECKED, 0); 
		else		SendMessage(hWnd, BM_SETCHECK, BST_UNCHECKED, 0);
		value = val;
		return S_OK; 
	};

};

/// <summary>
/// RADIO コントロール
/// </summary>
class CRadioCtrl : public CPnlParts<INT16> {	//ボタン

private:
	INT16 n_radio = 0;	//ラジオボタンの数

public:

	CRadioCtrl(int _n_radio) {
		
		if (n_radio > N_VAL_RADIO_MAX) n_radio = N_VAL_RADIO_MAX;
		else if (_n_radio < 0)			n_radio = 0;
		else							n_radio = _n_radio;
	}
	virtual ~CRadioCtrl() {}

	CCbCtrl* pradio[N_VAL_RADIO_MAX] = {					//チェックボタン型のポインタ配列
		NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL
	};

	void set_radio(int _n_radio, CCbCtrl* _pradio) {
		if (_n_radio >= n_radio) return;
		pradio[_n_radio] = _pradio;
	}

	INT16 check() { 
		INT16 val = -1;
		for (int i = 0; i < n_radio; i++) {
			if (pradio[i]->get() == L_ON) {
				value = val = i;
				break;
			}
		}
		return value = val;
	}

	virtual HRESULT set(INT16 val) {
		if ((val < 0) || (val >= n_radio)) return S_FALSE;

		pradio[val]->set(L_ON);	//チェックボタンのセット
		value = val;
		return S_OK;
	};

	
};

#define CODE_VALUE_LAMP_OFF			0x00
#define CODE_VALUE_LAMP_ON			0x01
#define CODE_VALUE_LAMP_FLICK		0xFF

#define CODE_TYPE_LAMP_ELLI			0
#define CODE_TYPE_LAMP_RECT			1

/// <summary>
/// DrawLamp
/// ON/OFFでブラシを切替
/// デバイスコンテキストに矩形または円形の塗りつぶし
/// </summary>
class CLampDraw : public CPnlParts<INT16> {
public:
	CLampDraw(
		INT32 _id, Point* ppt, Size* psz, LPWSTR ptext,
		Graphics* _pgraphic, SolidBrush* _pbrush_on, SolidBrush* _pbrush_off, Font* _pfont,
		INT32 _fcount
	) 
	{
		set_id(_id);
		set_base(ppt, psz, ptext);
		pgraphic = _pgraphic;
		pbrush_on = _pbrush_on;
		pbrush_off = _pbrush_off;
		pfont = _pfont;
		fcount = _fcount;
			
		value = CODE_VALUE_LAMP_OFF;
	}

	virtual ~CLampDraw() {}

	SolidBrush* pbrush_on	= NULL;
	SolidBrush* pbrush_off	= NULL;
	Font* pfont = NULL;

	Graphics* pgraphic = NULL;	//描画用グラフィックオブジェクト

	INT32 count = 0;	//点滅タイミング用

	INT16 type		= CODE_TYPE_LAMP_ELLI;	//ランプのタイプ
	INT32 fcount	= 6;					//点滅カウント一周期分
	INT32 fcount_on = 3;					//点滅カウントON
	bool is_text_draw = false;				//文字表示するか？

	HRESULT set_txt_mode(bool _is_text_draw) {
		is_text_draw = _is_text_draw;
		return S_OK;
	}

	virtual HRESULT set(INT16 code) {

		SolidBrush* pbrush = NULL;
		if (code == CODE_VALUE_LAMP_OFF) {
			value = CODE_VALUE_LAMP_OFF;
			pbrush = pbrush_off;
			count = 0;
		}
		else if (code == CODE_VALUE_LAMP_ON) {
			value = CODE_VALUE_LAMP_ON;
			pbrush = pbrush_on;
			count = 0;
		}
		else if (code == CODE_VALUE_LAMP_FLICK) {
			count++; if (count >= fcount) count = 0;
			if (count > fcount_on) {
				value = CODE_VALUE_LAMP_OFF;
				pbrush = pbrush_off;
			}
			else {
				value = CODE_VALUE_LAMP_ON;
				pbrush = pbrush_on;
			}
		}
		else return S_FALSE;

		if (type == CODE_TYPE_LAMP_ELLI) {
			pgraphic->FillEllipse(pbrush, pt.X, pt.Y, sz.Width, sz.Height);;
		}
		else {
			pgraphic->FillRectangle(pbrush, pt.X, pt.Y, sz.Width, sz.Height);
		}

		return S_OK;
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

		value = CODE_VALUE_LAMP_OFF;
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
		if (code == CODE_VALUE_LAMP_OFF) {
			value = CODE_VALUE_LAMP_OFF;
			rc_src = rc_src_off;
			count = 0;
		}
		else if (code == CODE_VALUE_LAMP_ON) {
			value = CODE_VALUE_LAMP_ON;
			rc_src = rc_src_on;
			count = 0;
		}
		else if (code == CODE_VALUE_LAMP_FLICK) {
			count++; if (count >= fcount) count = 0;
			if (count > fcount_on) {
				value = CODE_VALUE_LAMP_OFF;
				rc_src = rc_src_off;
			}
			else {
				value = CODE_VALUE_LAMP_ON;
				rc_src = rc_src_on;
			}
		}
		else return S_FALSE;
		
		BitBlt(dc_dst, pt.X, pt.Y, sz.Width, sz.Height, dc_src,rc_src.X,rc_src.Y, SRCCOPY);

		return S_OK;
	}
};

/// <summary>
/// ImageLamp
/// 指定画像を切替表示
/// Imaggeを読み込んでグラフィックに書き込み
/// </summary>
#define N_IMG_SWITCH_MAX			8
#define CODE_IMG_SWITCH_OFF			0
#define CODE_IMG_SWITCH_ON			1

class CSwitchImg : public CPnlParts<INT16> {
public:
	CSwitchImg(
		INT32 _id, Point* ppt, Size* psz, LPWSTR ptext,
		Graphics* _pgraphic, Image** _ppimg, INT32 _n_switch,
		INT32 _fcount
	)
	{
		set_id(_id);
		set_base(ppt, psz, ptext);
		pgraphic = _pgraphic;
		for (int i = 0; i < N_IMG_SWITCH_MAX; i++) {
			if (i < _n_switch) pimg[i] = _ppimg[i];
			else pimg[i] = NULL;
		}
		n_switch = _n_switch;
		fcount = _fcount;

		value = CODE_IMG_SWITCH_OFF;
	}
	virtual ~CSwitchImg() {}

	Graphics* pgraphic;
	Image* pimg[N_IMG_SWITCH_MAX];
	INT32 n_switch		= 2;	//スイッチ数
	INT32 n_flick		= 2;	//フリッカさせるスイッチ数

	INT32 count			= 0;	//点滅タイミング用
	INT32 fcount		= 3;	//表示カウント１イメージ分
	INT32 id_disp		= 0;	//表示イメージID	


	HRESULT setup_flick(INT32 _n_flick, INT32 _fcount) {
		fcount = _fcount;
		n_flick = _n_flick;
		return S_OK;
	}

	virtual HRESULT set(INT16 code) {

		Image* pimg_disp = pimg[CODE_IMG_SWITCH_OFF];
		value = code;

		if (code == CODE_VALUE_LAMP_FLICK) {
			count++; if (count > fcount) {
				count = 0;
				id_disp++; if (id_disp >= n_flick) id_disp = 0;
			}
			pimg_disp = pimg[id_disp];
		}
		else if ((code < n_switch) && (code >= 0)) {
			id_disp = code;
			pimg_disp = pimg[CODE_IMG_SWITCH_ON];
		}
		else  return S_FALSE;

		pgraphic->DrawImage(pimg_disp, pt.X, pt.Y);

		return S_OK;
	}
};


