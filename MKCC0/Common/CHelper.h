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
#include "COMMON_DEF.h"

//ノッチビットパターン

#define PTN_NOTCH4_MASK		0x003F
#define PTN_NOTCH4_CLR		0xFFC0
#define PTN_NOTCH4_0		0x0001
#define PTN_NOTCH4_F1		0x0002
#define PTN_NOTCH4_F2		0x000A
#define PTN_NOTCH4_F3		0x001A
#define PTN_NOTCH4_F4		0x003A
#define PTN_NOTCH4_R1		0x0004
#define PTN_NOTCH4_R2		0x000C
#define PTN_NOTCH4_R3		0x001C
#define PTN_NOTCH4_R4		0x003C

//PLCのノッチコード
struct ST_NOTCH_CODE {
	INT16 f4[N_NOTCH_MAX] = { PTN_NOTCH4_0,PTN_NOTCH4_F1,PTN_NOTCH4_F2,PTN_NOTCH4_F3,PTN_NOTCH4_F4,PTN_NOTCH4_F4 };
	INT16 r4[N_NOTCH_MAX] = { PTN_NOTCH4_0,PTN_NOTCH4_R1,PTN_NOTCH4_R2,PTN_NOTCH4_R3,PTN_NOTCH4_R4,PTN_NOTCH4_R4 };
};

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
	static void fit_ph_range_upto_pi(double* th); //　位相座標を±πのレンジに校正する
};

#define PLC_IO_CS_MH_SPD_MODE	20
#define PLC_IO_CS_BH_R_MODE		21

//CSの出力パターン
#define PLC_IO_CS_TYPE_A		0	//HHGH29
#define PLC_IO_CS_TYPE_B		1	//
#define PLC_IO_CS_TYPE_C		2	//
#define PLC_IO_CS_TYPE_D		3	//

#define PLC_IO_CS_MH_SPD_MODE0 BITOFF
#define PLC_IO_CS_MH_SPD_MODE1 BIT2
#define PLC_IO_CS_MH_SPD_MODE2 BIT2
#define PLC_IO_CS_MH_SPD_MODE3 BIT3 //HHGH29は中速まで
#define PLC_IO_CS_BH_R_MODE0   BITOFF
#define PLC_IO_CS_BH_R_MODE1   BIT4
#define PLC_IO_CS_BH_R_MODE2   BIT5
#define PLC_IO_CS_BH_R_MODE3   BIT6//HHGH29はジブレスト

class CPlcCSHelper
{
public:
	static INT16 get_mode_by_code(INT16 value,int cs_code, int type);
	static INT16 get_code_by_mode(INT16 mode, int cs_code, int type);
};

class CUIHelper
{
public:
	static wstring crane_txt;
	static LPCWSTR get_crane_txt_by_code(INT32 value);
};

#define HELPER_DATA_TYPE_LONG		0
#define HELPER_DATA_TYPE_DOUBLE		1
#define HELPER_DATA_TYPE_LONGLONG	2
#define HELPER_DATA_COM_UPDATE		0
#define HELPER_DATA_COM_REFRESH		1
#define HELPER_DATA_BUF_MAX			1024



union UHelperStatisData {
	long l;
	double d;
	LONGLONG ll;
};

struct StHelperStatisData {
	UHelperStatisData Ave;
	UHelperStatisData min_val;
	UHelperStatisData max_val;
	UHelperStatisData Sum;
	int cycle_count;//評価データ数
};

class CStatisticsHelper
{
public:
	CStatisticsHelper() {};
	~CStatisticsHelper() {};

	long* buf_long;
	PLONGLONG buf_llong;
	double* buf_double;

	int buf_size;	//バッファサイズ		
	int iw;			//バッファ書き込み位置
	int count;		//カウント数
	int count_max;	//カウント最大値
	int type;		//カウント最大値
	int mask;		//カウントマスク
	int init_ok;	//初期化完了フラグ

	StHelperStatisData result;	//結果格納用構造体

	UHelperStatisData default_min;	//デフォルト最小値
	UHelperStatisData default_max;	//デフォルト最大値

	int init(int _type, void* pbuf, int buf_size, int cycle_count, UHelperStatisData def_min, UHelperStatisData def_max) {
		init_ok = L_OFF;
		type = _type;
		if (type == HELPER_DATA_TYPE_LONG) {
			buf_long = (long*)pbuf;
		}
		else if (type == HELPER_DATA_TYPE_DOUBLE) {
			buf_double = (double*)pbuf;
		}
		else if (type == HELPER_DATA_TYPE_LONGLONG) {
			buf_llong = (PLONGLONG)pbuf;
		}
		else return init_ok;

		if (cycle_count <= 16) mask = 0x000F;
		else if (cycle_count <= 32) mask = 0x0000001F;
		else if (cycle_count <= 64) mask = 0x0000003F;
		else if (cycle_count <= 128) mask = 0x0000007F;
		else if (cycle_count <= 256) mask = 0x000000FF;
		else if (cycle_count <= 512) mask = 0x000001FF;
		else  mask = 0x000003FF;

		if (buf_size < (mask + 1)) return init_ok;

		default_min = def_min;
		default_max = def_max;

		init_ok = L_ON;

		return init_ok;
	};

	int update(UHelperStatisData data, int com) {
		
		if (init_ok == L_OFF)return L_OFF;	

		if (com == HELPER_DATA_COM_REFRESH) {
			//結果リフレッシュ
			count = 0;
			if (type == HELPER_DATA_TYPE_LONG) {
				*buf_long = data.l;
				result.Ave.l = result.Sum.l = data.l;
				result.min_val.l = data.l;
				result.max_val.l = data.l;
			}
			if (type == HELPER_DATA_TYPE_DOUBLE) {
				*buf_double = data.d;
				result.Ave.d = result.Sum.d = data.d;
				result.min_val.d = data.d;
				result.max_val.d = data.d;
			}
			if (type == HELPER_DATA_TYPE_LONGLONG) {
				*buf_llong = data.ll;
				result.Ave.ll = result.Sum.ll = data.ll;
				result.min_val.ll = data.ll;
				result.max_val.ll = data.ll;
			}
		}
		else {
			//データ更新
			count++;
			iw = count & mask;

			//平均値更新
			if (type == HELPER_DATA_TYPE_LONG) {
				if (count > mask) result.Sum.l = result.Sum.l - *(buf_long + iw) + data.l;
				else result.Sum.l += data.l;
				
				*(buf_long + iw) = data.l;
				result.Ave.l = result.Sum.l / (count > mask ? (mask + 1) : count);
				result.min_val.l = min(result.min_val.l, data.l);
				result.max_val.l = max(result.max_val.l, data.l);
			}
			else if (type == HELPER_DATA_TYPE_DOUBLE) {

				if (count > mask) result.Sum.d = result.Sum.d - *(buf_double + iw) + data.d;
				else result.Sum.d += data.d;
				
				*(buf_double + iw) = data.d;
				result.Ave.d = result.Sum.d / (count > mask ? (mask + 1) : count);
				result.min_val.d = min(result.min_val.d, data.d);
				result.max_val.d = max(result.max_val.d, data.d);
			}
			else if (type == HELPER_DATA_TYPE_LONGLONG) {

				if (count > mask)result.Sum.ll = result.Sum.ll - *(buf_llong + iw) + data.ll;
				else result.Sum.ll += data.ll;

				*(buf_llong + iw) = data.ll;
				result.Ave.ll = result.Sum.ll / (count > mask ? (mask + 1) : count);
				result.min_val.ll = min(result.min_val.ll, data.ll);
				result.max_val.ll = max(result.max_val.ll, data.ll);
			}
			else {
				return L_OFF;
			}
		}

		//if(count <= mask)return L_OFF;

		return L_ON;
	}
};