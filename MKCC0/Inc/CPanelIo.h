#pragma once
#include <Windows.h>
#include <string>
#include "COMMON_DEF.H"
//@ GDI+
#include <objbase.h>//gdiplus.hのコンパイルを通すために必要
#include <gdiplus.h>

using namespace Gdiplus;

#define CODE_VALUE_ACTION_NON  0

#define ID_VALUE_OBJ_MAIN	0
#define ID_VALUE_OBJ_SUB	1

template <class T> class CValue {
public:
	CValue() { value = value_hold = 0; }
	virtual ~CValue() {}

protected:
    T value;	    //現在値
	T value_hold;
 
	POINT pt;
	SIZE size;
	std::wstring text;

    INT16	act = CODE_VALUE_ACTION_NON;	//動作内容

	Pen* ppen[2]			= {NULL,NULL};
	SolidBrush* pbrush[2]	= {NULL,NULL};
	HFONT hfont				= NULL;
	
	PLONG pcount;

public:
	HRESULT set_base(POINT* ppt, SIZE* psz, LPWSTR ptext, PLONG _pcount) {
		pt = *ppt; size = *psz; pcount = _pcount;
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

class CPblVal : public CValue<INT16> {
public:
	CPblVal(
		POINT* ppt, SIZE* psz, LPWSTR ptext, PLONG _pcount,
		Pen** _pppen, SolidBrush** _ppbrush, HFONT _hfont	) 
	{ 
		set_base(ppt, psz, ptext, pcount);
		set_graphic(_pppen, _ppbrush, _hfont);
	}

	virtual ~CPblVal() {}
};

class CI32Val : public CValue<INT32> {
public:
	CI32Val(
		POINT* ppt, SIZE* psz, LPWSTR ptext, PLONG _pcount,
		Pen** _pppen, SolidBrush** _ppbrush, HFONT _hfont)
	{
		set_base(ppt, psz, ptext, pcount);
		set_graphic(_pppen, _ppbrush, _hfont);
	}

	virtual ~CI32Val() {}
};

class CDblVal : public CValue<double> {
public:
	CDblVal(
		POINT* ppt, SIZE* psz, LPWSTR ptext, PLONG _pcount,
		Pen** _pppen, SolidBrush** _ppbrush, HFONT _hfont)
	{
		set_base(ppt, psz, ptext, pcount);
		set_graphic(_pppen, _ppbrush, _hfont);
	}

	virtual ~CDblVal() {}
};




