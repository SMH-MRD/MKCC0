#pragma once
#include <Windows.h>
#include <string>
#include "COMMON_DEF.H"
//@ GDI+
#include <objbase.h>//gdiplus.hのコンパイルを通すために必要
#include <gdiplus.h>

using namespace Gdiplus;

//######################################################################
// Value
//######################################################################

#define CODE_VALUE_ACTION_NON  0

#define ID_VALUE_OBJ_MAIN	0
#define ID_VALUE_OBJ_SUB	1

#define ID_VALUE_POINT_DST	0
#define ID_VALUE_POINT_SRC0	1
#define ID_VALUE_POINT_SRC1	2

template <class T> class CCtrl {
public:
	CCtrl() { value = value_hold = 0; }
	virtual ~CCtrl() {}

protected:
    T value;	    //現在値
	T value_hold;
 
	POINT pt[3];//表示位置
	SIZE size[2];
	std::wstring text;

    INT16	act = CODE_VALUE_ACTION_NON;	//動作内容

	Pen* ppen[2]			= {NULL,NULL};
	SolidBrush* pbrush[2]	= {NULL,NULL};
	HFONT hfont				= NULL;
		
	PLONG pcount;		//カウンタのポインタ　点滅タイミング等

public:

	HRESULT set_base(POINT* ppt, SIZE* psz, LPWSTR ptext) {
		pt[0] = *ppt;	pt[1] = *(ppt+1); pt[2] = *(ppt + 2);
		size[0] = *psz; size[1] = *(psz+1);
		std::wstring _text = ptext; text = _text;
		return S_OK;
	};
	HRESULT set_graphic(Pen** _pppen, SolidBrush** _ppbrush, HFONT _hfont) {
		ppen [ID_VALUE_OBJ_MAIN] = *_pppen; ppen[ID_VALUE_OBJ_SUB] = *(_pppen + 1);
		pbrush[ID_VALUE_OBJ_MAIN] = *_ppbrush; pbrush[ID_VALUE_OBJ_SUB] = *(_ppbrush + 1);
		hfont = _hfont;
		return S_OK;
	}

	T get() { return value; }
	HRESULT set(T _value, bool flg_update_hold) { if (flg_update_hold)value_hold = value;   value = _value;  return S_OK; }
	HRESULT set(T _value) { value = _value;  return S_OK; }
	
	BOOL chk_trig() { set(value, true); if (value != value_hold)return true;  return false; }
	BOOL chk_trig_on() { set(value, true); if (value > value_hold)return true; return false; }
	BOOL chk_trig_off() { set(value, true); if (value < value_hold)return true; return false; }
	
};

#define ID_VALUE_TYPE_PB			0
#define ID_VALUE_TYPE_CHECK_BOX		1
#define ID_VALUE_TYPE_RADIO			2
#define ID_VALUE_TYPE_LAMP			3

/// <summary>
/// PB Value
/// value：0=OFF, 0以外=ON
/// ボタンのトリガでカウントをセット→updateメソッドを定周期呼び出しでカウントダウン
/// </summary>
class CPbCtrl : public CCtrl<INT16> {	//ボタン

public:
	CPbCtrl(
		INT16 pb_type, HWND _hwnd, INT16 _count_val,
		POINT** ppt, SIZE** psz, LPWSTR ptext) 
	{ 
		set_base(*ppt, *psz, ptext);
		hwnd = _hwnd;
	}

	HWND hwnd;					//ボタンのハンドル
	INT16 type;					//ボタンのタイプ
	INT16 count_off_set_val;	//オフディレイカウント値
	
	INT16 update(bool is_trigger) { //更新
		if (is_trigger) set(count_off_set_val);  //トリガーでカウントセット
		else if (value > 0) value--;
		else;

		if (type != ID_VALUE_TYPE_PB){
			if(value <= 0)SendMessage(hwnd, BM_SETCHECK, BST_UNCHECKED, 0L);
		}
		return get(); 
	}

	HRESULT clear() { 
		value = value_hold = 0; 
		if (type != ID_VALUE_TYPE_PB) SendMessage(hwnd, BM_SETCHECK, BST_UNCHECKED, 0L);
		return S_OK; 
	}	//クリア	
	virtual ~CPbCtrl() {}
};

/// <summary>
/// CECK BOX Value
/// </summary>
class CCbCtrl : public CCtrl<INT16> {	//ボタン

public:
	CCbCtrl(
		HWND _hwnd,	INT16 _val_on, POINT** ppt, SIZE** psz, LPWSTR ptext)
	{
		set_base(*ppt, *psz, ptext);
		hwnd = _hwnd;
	}

	HWND hwnd;					//ボタンのハンドル
	INT16 val_on;			//オンセット値

	INT16 update() { //更新
		if (BST_CHECKED == SendMessage(hwnd, BM_GETCHECK, 0, 0))
			set(val_on);
		else
			set(0);
	
		return get();
	}
	virtual ~CCbCtrl() {}
};


#define CODE_VALUE_LAMP_DRAW_RECT	0	// DRAWランプ長方形
#define CODE_VALUE_LAMP_DRAW_CIRCLE	1	// DRAWランプ円形
#define CODE_VALUE_LAMP_GRAPHIC		2	// グラフィックランプ

#define CODE_VALUE_LAMP_OFF			0
#define CODE_VALUE_LAMP_ON			1
#define CODE_VALUE_LAMP_BLINK		2

/// <summary>
/// LampValue
/// ON/OFFで画像を切替
/// グラフィックタイプは元のデバイスコンテキストから画像をコピー
/// その他は書き込みデバイスコンテキストに描画
/// </summary>
class CLampCtrl : public CCtrl<INT16> {	//ランプ

public:
	CLampCtrl(INT16 lamp_type, INT16 cnt_freq, HDC _hdc_dst, HDC _hdc_src,
		POINT** ppt, SIZE** psz, LPWSTR ptext, 
		Pen** _pppen, SolidBrush** _ppbrush, HFONT _hfont)
	{
		type = lamp_type;
		freq = cnt_freq; hfreq = freq / 2;
		hdc_dst = _hdc_dst;
		hdc_src = _hdc_src;
		set_base(*ppt, *psz, ptext);
		set_graphic(_pppen, _ppbrush, _hfont);
	}

	HDC hdc_dst,hdc_src;			//描画先元HDC
	INT16 type=0;					//ランプのタイプ
	INT16 freq=0,hfreq=0;			//点滅周期
	INT16 cnt_blink = 0;			//点滅カウント
	INT16 disp_stat = L_OFF;		//表示状態

	HRESULT draw() {
		if (value == CODE_VALUE_LAMP_BLINK) {
			value_hold++;
			if (value_hold > freq) {
				value_hold = 0; disp_stat = CODE_VALUE_LAMP_OFF;
			}
			else if (value_hold < hfreq) {
				disp_stat = CODE_VALUE_LAMP_ON;
			}
			else {
				disp_stat = CODE_VALUE_LAMP_OFF;
			}
		}
		else {
			disp_stat = value;
		}

		if (type == CODE_VALUE_LAMP_GRAPHIC) {
			if (value == CODE_VALUE_LAMP_ON) {
				BitBlt(hdc_dst, pt[0].x, pt[0].y, size[0].cx, size[0].cy,  hdc_src, pt[1].x, pt[1].y, SRCCOPY);
			}
			else {
				BitBlt(hdc_dst, pt[0].x, pt[0].y, size[0].cx, size[0].cy,hdc_src, pt[2].x, pt[2].y, SRCCOPY);
			}
		}
		else{
			if (value == CODE_VALUE_LAMP_ON) {
				SelectObject(hdc_dst, pbrush[ID_VALUE_OBJ_MAIN]);
			}
			else {
				SelectObject(hdc_dst, pbrush[ID_VALUE_OBJ_SUB]);
			}
			if (type == CODE_VALUE_LAMP_DRAW_RECT) {
				Rectangle(hdc_dst, pt[0].x, pt[0].y, pt[0].x + size[0].cx, pt[0].y + size[0].cy);
			}
			else {
				Ellipse(hdc_dst, pt[0].x, pt[0].y, pt[0].x + size[0].cx, pt[0].y + size[0].cy);
			}
		}
	}

	virtual ~CLampCtrl() {}
};

class CI16Ctrl : public CCtrl<INT16> {
public:
	CI16Ctrl(
		POINT* ppt, SIZE* psz, LPWSTR ptext, PLONG _pcount,
		Pen** _pppen, SolidBrush** _ppbrush, HFONT _hfont)
	{
		set_base(ppt, psz, ptext);
		set_graphic(_pppen, _ppbrush, _hfont);
	}

	virtual ~CI16Ctrl() {}
};

class CI32Ctrl : public CCtrl<INT32> {
public:
	CI32Ctrl(
		POINT* ppt, SIZE* psz, LPWSTR ptext, PLONG _pcount,
		Pen** _pppen, SolidBrush** _ppbrush, HFONT _hfont)
	{
		set_base(ppt, psz, ptext);
		set_graphic(_pppen, _ppbrush, _hfont);
	}

	virtual ~CI32Ctrl() {}
};

class CDblCtrl : public CCtrl<double> {
public:
	CDblCtrl(
		POINT* ppt, SIZE* psz, LPWSTR ptext, PLONG _pcount,
		Pen** _pppen, SolidBrush** _ppbrush, HFONT _hfont)
	{
		set_base(ppt, psz, ptext);
		set_graphic(_pppen, _ppbrush, _hfont);
	}

	virtual ~CDblCtrl() {}
};

class CPblCtrl {	//ボタン
public:
	CPblCtrl(CPbCtrl* _pb, CLampCtrl* _lamp) {
		pb = _pb;
		lamp = _lamp;
	}
	virtual ~CPblCtrl() {}

	CPbCtrl* pb = NULL;	//ボタンの値
	CLampCtrl* lamp = NULL;	//ランプの値
};
