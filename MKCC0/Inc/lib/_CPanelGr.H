#pragma once

#include "COMMON_DEF.h"

using namespace std;
using namespace Gdiplus;



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



