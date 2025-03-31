#pragma once
#include <Windows.h>
#include "COMMON_DEF.H"

typedef struct _LIST_ID_W_WRITE {
	INT16   helthy;            //PCヘルシー出力信号
	INT16   ctrl_mode;         //自動制御フラグ
	INT16	cab_ai;            //運転室PLC AI信号【将来用】
	INT16   cab_di;            //運転室PLC→電気室PLC b出力
	INT16	hcounter;          //高速カウンタユニット 
	INT16	absocoder;         //アブソコーダ 
	INT16   pc_fault;          //PC検出異常マップ
	INT16   erm_unitx;         //電気室ユニットX 020-03F：カウンタユニット　040-04F　120-12F：ABSユニット
	INT16   erm_x;             //電気室X 060-0CF
	INT16   inv_cc_x;          //インバータFB書き込み値　ｘデバイス
	INT16   inv_cc_Wr1;        //インバータFB書き込み値　rpm
	INT16   inv_cc_Wr2;        //インバータFB書き込み値　トルク0.1%
}LIST_ID_W_WRITE, *LPLIST_ID_W_WRITE;

typedef struct _LIST_ID_W_READ {
	INT16   helthy;            //PLCヘルシーカウンタ
	INT16   plc_ctrl;          // PLC運転モード
	INT16   cab_ai;            //運転室PLC→電気室PLC W出力
	INT16   cab_bi;            //運転室PLC→電気室PLC b出力
	INT16   erm_900;
	INT16   erm_bo;            //電気室PLC b出力
	INT16   pos;               //各軸位置信号
	INT16   spd_tg;            //各軸指令速度目標
	INT16   plc_fault;         //各軸速度信号
	INT16   erm_y;             //電気室PLC Y出力
	INT16   erm_x;             //電気室PLC X入力
	INT16   inv_cc_y;          //インバータPLC DO指令
	INT16   inv_cc_Ww1;        //インバータPLC 速度指令　rpm
	INT16   inv_cc_x;          //インバータFB書き込み値　ｘデバイス
	INT16   inv_cc_Wr1;        //インバータFB書き込み値　rpm
	INT16   inv_cc_Wr2;        //インバータFB書き込み値　トルク0.1%
}LIST_ID_W_READ, * LPLIST_ID_W_READ;


//union UN_PLC_DATA
//{
//    double d;
//    float  f;
//    INT32  i32;
//    UINT32 ui32;
//    INT16  i16;
//    UINT16 ui16;
//    bool   bit;
//};

template <class T> class CPlcIO {
public:
	CPlcIO() {}
	virtual ~CPlcIO() {}

protected:
    T _wval_plc;	    //PLCへの出力値
    T _rval_plc;		//PLCからの入力値

	T*		_pwbuf;	//IO WRITE	バッファへのポインタ
	T*		_prbuf;	//IO READ	バッファへのポインタ
    PINT16		_pwprm;	//パラメータへのポインタ
	PINT16		_prprm;	//パラメータへのポインタ

public:
	virtual HRESULT set_w(T val) { _wval_plc = val; return S_OK;};
	T get_w() { return _wval_plc; }
	T get_r() { return _rval_plc; }

	HRESULT set_wbuf(PINT16 p) { _pwbuf = p; return S_OK; }
	HRESULT set_rbuf(PINT16 p) { _prbuf = p; return S_OK; }

	virtual HRESULT write_io(T val)		= 0;    //_wval_plcの内容をセットしてIOバッファに出力
    virtual T read_io()					= 0;	// dispの内容をIOバッファから読み出し
};

//*CNotchIO *****************************************************************
#define PLCIO_N_NOTCH_INDEX     11
#define PLCIO_0_NOTCH_INDEX     5
class CNotchIO : public CPlcIO<INT16> {
private:
	INT16 o_ptn[PLCIO_N_NOTCH_INDEX];	    // ノッチ指令パターン　0 Notch:index5  4 Notch:index9  -4 Notch:index1
	INT16 i_ptn[PLCIO_N_NOTCH_INDEX];	    // ノッチFBパターン　　0 Notch:index5  4 Notch:index9  -4 Notch:index1
public:
	CNotchIO(PINT16 prbuf, PINT16 pwbuf, PINT16 pwprm, PINT16 prprm){
		_prbuf = prbuf;_pwbuf = pwbuf;_pwprm = pwprm; _prprm = prprm;
		for (int i = 0; i < PLCIO_N_NOTCH_INDEX; i++) {
			o_ptn[i] = *(pwprm + i);
			i_ptn[i] = *(prprm + i);
		}
	}
    ~CNotchIO() {}

	virtual HRESULT write_io(INT16 val);
	virtual INT16 read_io();
};

//*CBitIO *****************************************************************

class CBitIO : public CPlcIO <INT16>{
private:
	INT16 bit_mask_w,bit_mask_r;
public:
	CBitIO(PINT16 prbuf, PINT16 pwbuf, PINT16 pwprm, PINT16 prprm) {
		_prbuf = prbuf; _pwbuf = pwbuf;
		_pwprm = pwprm; _pwprm = pwprm;
		bit_mask_w = *(pwprm);
		bit_mask_r = *(prprm);
	}
	~CBitIO() {}

	virtual HRESULT write_io(INT16 val) { 
		_wval_plc = val; 
		if (_wval_plc == L_ON)	*_pwbuf |= bit_mask_w; 
		else					*_pwbuf &= ~bit_mask_w;
		return S_OK; 
	}

	virtual INT16 read_io() {
		if (*_prbuf & bit_mask_r)	_rval_plc = L_ON;
		else					_rval_plc = L_OFF;
		return _rval_plc;
	}
};

//*CWordIO *****************************************************************
class CWordIO : public CPlcIO<INT16> {
public:
	CWordIO(PINT16 prbuf, PINT16 pwbuf, PINT16 pwprm, PINT16 prprm) {
		_prbuf = prbuf;	_pwbuf = pwbuf;
		_prprm = prprm;	_pwprm = pwprm;
	}
	~CWordIO() {}
	virtual HRESULT write_io(INT16 val) {
		set_w(val); *_pwbuf = val; return S_OK;
	}
	virtual INT16 read_io() {
		return _rval_plc = *_prbuf;
	}
};

//*CDWordIO *****************************************************************
class CDWordIO : public CPlcIO<INT32> {
public:
	CDWordIO(PINT32 prbuf, PINT32 pwbuf, PINT16 pwprm, PINT16 prprm)
{
		_prbuf = prbuf;	_pwbuf = pwbuf;
		_prprm = prprm; _pwprm = pwprm;
	}
	~CDWordIO() {}
	virtual HRESULT write_io(INT16 val) {
		set_w(val); *_pwbuf = val; return S_OK;
	}
	virtual INT32 read_io() {
		return _rval_plc = *_prbuf;
	}
};

//*CModeIO *****************************************************************
class CModeIO : public CPlcIO<INT16> {
private:
	INT16 bit_ptn[16] = { (INT16)BIT0,(INT16)BIT1,(INT16)BIT2,(INT16)BIT3,(INT16)BIT4,(INT16)BIT5,(INT16)BIT6 ,(INT16)BIT7 ,(INT16)BIT8 ,(INT16)BIT9 ,(INT16)BIT10 ,(INT16)BIT11 ,(INT16)BIT12 ,(INT16)BIT13,(INT16)BIT14 ,(INT16)BIT15 };

	INT16 bit_mask_w, nbit_w = 0, ibit0_w = 0;
	INT16 bit_mask_r, nbit_r = 0, ibit0_r = 0;

public:
	CModeIO(PINT16 _prbuf, PINT16 _pwbuf, PINT16 pwprm, PINT16 prprm) {
		_prbuf = _prbuf;_pwbuf = _pwbuf;
		_prprm = prprm; _pwprm = pwprm;
		bit_mask_r = *(prprm); bit_mask_w = *(pwprm);

		for (int i = 0; i < 16; i++) {
			if (bit_mask_w & bit_ptn[i]) {
				nbit_w++;
				if (nbit_w == 1) ibit0_w = i;
			}
			if (bit_mask_r & bit_ptn[i]) {
				nbit_r++;
				if (nbit_r == 1) ibit0_r = i;
			}
		}
	}
	~CModeIO() {}

	virtual HRESULT set_w(INT16 val) override;
	virtual HRESULT write_io(INT16 val);
	virtual INT16 read_io();
};
