#pragma once

#include "COMMON_DEF.h"

using namespace std;

#define ID_PANEL_COLOR_BLACK	0
#define ID_PANEL_COLOR_GRAY		1
#define ID_PANEL_COLOR_WHITE	2
#define ID_PANEL_COLOR_RED		3
#define ID_PANEL_COLOR_GREEN	4
#define ID_PANEL_COLOR_BLUE		5
#define ID_PANEL_COLOR_YELLOW	6
#define ID_PANEL_COLOR_ORANGE	7
#define ID_PANEL_COLOR_MAZENDA	8
#define ID_PANEL_COLOR_BROWN	9

#define ID_PANEL_FONT_6			0
#define ID_PANEL_FONT_8			1
#define ID_PANEL_FONT_10		2
#define ID_PANEL_FONT_12		3
#define ID_PANEL_FONT_14		4
#define ID_PANEL_FONT_20		5
#define ID_PANEL_FONT_24		6
#define ID_PANEL_FONT_32		7
#define ID_PANEL_FONT_48		8

#define N_PANEL_COLOR_PEN		16
#define N_PANEL_COLOR_BRUSH		16
#define N_PANEL_FONT			16

typedef struct _ST_DRAWING_BASE {
	Pen*		ppen[N_PANEL_COLOR_PEN];
	SolidBrush* pbrush[N_PANEL_COLOR_BRUSH];
	HFONT		hfont[N_PANEL_FONT];
}ST_DRAWING_BASE,LPST_DRAWING_BASE;


class CImgObj {

public:
	CImgObj(LPCWSTR str_src, HDC hdc_src, HDC hdc_dest) {
		_hdc_dest	= hdc_dest;
		_hdc_src	= hdc_src;
		pimg = new Image(str_src);
		im_area_src = Rect(0, 0, pimg->GetWidth(), pimg->GetHeight());
		im_area_dst = Rect(0, 0, pimg->GetWidth(), pimg->GetHeight());
	}
	virtual ~CImgObj() {
		delete pimg;
	}

	HRESULT set_area_src(int x, int y) {
		im_area_src.X = x; im_area_src.Y = y; 
		return S_OK;
	}
	HRESULT put_img(INT x, INT y, HDC hdc_src) {
		im_area_dst.X = x; im_area_dst.Y = y;
		BitBlt(_hdc_dest, im_area_dst.X, im_area_dst.Y, im_area_dst.Width, im_area_dst.Height,
			hdc_src, im_area_src.X, im_area_src.Y, SRCCOPY);	
		return S_OK;
	}

protected:
	Image* pimg;
	Rect im_area_src;
	Rect im_area_dst;
	HDC _hdc_src;
	HDC _hdc_dest;
};

class CMemDC {
public:
	CMemDC(HDC _hdc, int w,int h) {
		hBmap = CreateCompatibleBitmap(_hdc, w, h);
		hdc = CreateCompatibleDC(_hdc);
		SelectObject(hdc, hBmap);
		pgraphic = new Graphics(hdc);
	}
	virtual ~CMemDC() {
		delete pgraphic;
		DeleteObject(hBmap);
		DeleteDC(hdc);
	}

	HDC get() { return hdc; }

protected:
	HDC hdc;
	HBITMAP hBmap;
	Graphics* pgraphic;
};



