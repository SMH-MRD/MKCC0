#pragma once

#include <windows.h>
// C ランタイム ヘッダー ファイル
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include <string>
#include <time.h>
#include <vector>
#include <mmsystem.h>
#include "params.h"


using namespace std;
class CNotchHelper
{
public:
	CNotchHelper() {};
	~CNotchHelper() {};
	static ST_NOTCH_CODE code;

	//ノッチ指定でPLC IO用コードの取得,出力
	static INT16 get_code4_by_notch(int notch, int bit_shift);
	static INT16 set_code4_by_notch(PINT16 pdst,int notch,int bit_shift);
	//速度指定でPLC IO用コードの取得,出力
	static INT16 get_code4_by_v(double v, double* vtbl_f, double* vtbl_r, int bit_shift);
	static INT16 set_code4_by_v(PINT16 pdst, double v, double* vtbl_f, double* vtbl_r, int bit_shift);

	//code指定でノッチ取得
	static INT16 get_notch4_by_code(PINT16 pbuf, int bit_shift);
	//速度指定でノッチ取得
	static INT16 get_notch4_by_v(double v, double* vtbl_f, double* vtbl_r);
	static INT16 get_notch_by_pad(INT16 val, PINT16 vtbl_f, PINT16 vtbl_r);

	//UI配列用インデクス取得
	static INT16 get_iui4_by_notch(int notch);
	static INT16 get_iui4_by_v(double v, double* vtbl_f, double* vtbl_r);
};

class CStrHelper
{
public:
	CStrHelper() {};
	~CStrHelper() {};
	//文字列
	static void Str2Wstr(const string& src, wstring& dest);									//string→wstringに変換
	//Bitmap
	static void put_bmp_built(HDC hdc, HBITMAP hbmp, POINT dst_pt, POINT src_pt);						//ビットマップ表示
	static void put_bmp_stretch(HDC hdc, HBITMAP hbmp, POINT dst_pt, POINT src_pt, int retio_persent);  //ビットマップ拡大縮小表示

	//ビット数カウント
	static UINT	bits_count(UINT16 x);
	static UINT	bits_count(UINT32 x);
	static UINT	bits_count(UINT64 x);
	static void fit_ph_range_upto_pi(double* th); //　位相座標を±πのレンジに校正する

};
class CBitHelper
{
public:
	CBitHelper() {};
	~CBitHelper() {};
	//ビット数カウント
	static UINT	bits_count(UINT16 x);
	static UINT	bits_count(UINT32 x);
	static UINT	bits_count(UINT64 x);
};
class CBmpHelper
{
public:
	CBmpHelper() {};
	~CBmpHelper() {};
	//Bitmap
	static void put_bmp_built(HDC hdc, HBITMAP hbmp, POINT dst_pt, POINT src_pt);						//ビットマップ表示
	static void put_bmp_stretch(HDC hdc, HBITMAP hbmp, POINT dst_pt, POINT src_pt, int retio_persent);  //ビットマップ拡大縮小表示

};
class CPhaseHelper
{
public:
	CPhaseHelper() {};
	~CPhaseHelper() {};

	static void fit_ph_range_upto_pi(double* th); //　位相座標を±πのレンジに校正する

};