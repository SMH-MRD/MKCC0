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

union UN_PLC_DATA
{
    double d;
    float  f;
    INT32  i32;
    UINT32 ui32;
    INT16  i16;
    UINT16 ui16;
    bool   bit;
};

class CPlcIO {
public:
	CPlcIO() { _io_val.i16 = 0, _disp_val.i16 = 0, _prbuf = NULL, _pwbuf = NULL, _param=0; }
	virtual ~CPlcIO() {}

protected:
    UN_PLC_DATA _io_val;	    //値
    UN_PLC_DATA _disp_val;	//値
	INT16*		_prbuf;	//IO READ	バッファへのポインタ
	INT16*		_pwbuf;	//IO WRITE	バッファへのポインタ
    INT16		_param;	//ビット位置

public:
	UN_PLC_DATA get_disp() { return _disp_val; }
    UN_PLC_DATA get_io() { return _io_val; }
	
	virtual void set_disp(UN_PLC_DATA _val) = 0;
	virtual void write_io() = 0;					//disp_valの内容をIOバッファに出力
	virtual void write_io(UN_PLC_DATA _val) = 0;    //disp_valの内容をセットしてIOバッファに出力
    virtual UN_PLC_DATA read_io() = 0;						//dispの内容をIOバッファから読み出し
};

//*CNotchIO *****************************************************************
#define PLCIO_N_NOTCH_INDEX     11
#define PLCIO_0_NOTCH_INDEX     5
class CNotchIO : public CPlcIO {
private:
	INT16 io_ptn[PLCIO_N_NOTCH_INDEX];	    // 0 Notch:index5  4 Notch:index9  -4 Notch:index1

public:
	CNotchIO(INT16* prbuf, INT16* pwbuf, UINT16 param, PINT16 pio_ptn){
		_prbuf = prbuf;_pwbuf = pwbuf;_param = param;
		for (int i = 0; i < PLCIO_N_NOTCH_INDEX; i++) io_ptn[i] = *(pio_ptn + i);
	}
    ~CNotchIO() {}
	virtual void set_disp(UN_PLC_DATA _val);
	virtual void write_io();
	virtual void write_io(UN_PLC_DATA _val);
	virtual UN_PLC_DATA read_io();
};

//*CBitIO *****************************************************************
class CBitIO : public CPlcIO {
public:
	CBitIO(INT16* prbuf, INT16* pwbuf, UINT16 param) {
		_prbuf = prbuf;
		_pwbuf = pwbuf;
		_param = param;
	}
	~CBitIO() {}
	virtual void set_disp(UN_PLC_DATA _val) { _disp_val.bit = _val.bit; return; }
	virtual void write_io() { 
		if (_disp_val.bit== true) *_pwbuf |= _param; else *_pwbuf &= ~_param; return; 
	}
	virtual void write_io(UN_PLC_DATA _val) {set_disp(_val);	write_io(); return;}
	virtual UN_PLC_DATA read_io() {
		if (*_prbuf & _param)_io_val.bit = true ; else _disp_val.bit = _io_val.bit = false; return _io_val;
	}
};

//*CWordIO *****************************************************************
#define PLCIO_CODE_INT16     16
#define PLCIO_CODE_UINT16    17
#define PLCIO_CODE_INT32     32
#define PLCIO_CODE_UINT32    33
#define PLCIO_CODE_FLOAT     64
#define PLCIO_CODE_DOUBLE    65
class CWordIO : public CPlcIO {
public:
	CWordIO(INT16* prbuf, INT16* pwbuf, UINT16 param) {
		_prbuf = prbuf;
		_pwbuf = pwbuf;
		_param = param;
	}
	~CWordIO() {}
	virtual void set_disp(UN_PLC_DATA _val);
	virtual void write_io();
	virtual void write_io(UN_PLC_DATA _val);
	virtual UN_PLC_DATA read_io();
};

//*CModeIO *****************************************************************
class CModeIO : public CPlcIO {
private:
	INT16 bit_ptn[16] = { (INT16)BIT0,(INT16)BIT1,(INT16)BIT2,(INT16)BIT3,(INT16)BIT4,(INT16)BIT5,(INT16)BIT6 ,(INT16)BIT7 ,(INT16)BIT8 ,(INT16)BIT9 ,(INT16)BIT10 ,(INT16)BIT11 ,(INT16)BIT12 ,(INT16)BIT13,(INT16)BIT14 ,(INT16)BIT15 };
	INT16 nbit = 0,ibit0=0;
public:
	CModeIO(INT16* _prbuf, INT16* _pwbuf, UINT16 param) {
		_prbuf = _prbuf;
		_pwbuf = _pwbuf;
		_param = param;
		for (int i = 0; i < 16; i++) {
			if (_param & bit_ptn[i]) {
				nbit++;
				if (nbit == 1) ibit0 = i;
			}
		}
	}
	~CModeIO() {}
	virtual void set_disp(UN_PLC_DATA _val);
	virtual void write_io();
	virtual void write_io(UN_PLC_DATA _val);
	virtual UN_PLC_DATA read_io();
};
