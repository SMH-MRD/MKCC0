#pragma once
#include <string>
#include <windows.h>
#include "COMMON_DEF.h"

using namespace std;

#define CODE_TRIG_NON		0
#define CODE_TRIG_ON		1
#define CODE_TRIG_OFF		-1


template <class T> class CCraneValue {
public:
	CCraneValue() { value = value_hold = 0; }
	virtual ~CCraneValue() {}

protected:
	T value;	    //現在値
	T value_hold;
public:
	T get() { return value; }
	T set(T _value) { value_hold = value;   value = _value;  return value; }

	INT16 chk_trig()	{ 
		if (value >  value_hold)return CODE_TRIG_ON; 
		if (value <  value_hold)return CODE_TRIG_OFF;
		return CODE_TRIG_NON;
	}

	BOOL operator == (const CCraneValue<T>& other) const {
		return this->value == other.value;
	}
	BOOL operator != (const CCraneValue<T>& other) const {
		return this->value != other.value;
	}
};

// 軸速度Value
class CBitSignal : public CCraneValue<INT16> {
private:
	INT16 ibuf;		//バッファのインデックス
	INT16 bit_mask;	//ビット位置マスク

public:
	CBitSignal() {
		ibuf = 0; bit_mask = 0x0000;
	}
	virtual ~CBitSignal() {}

	void out_bit(INT16* pwbufi16) {   
		if (value)	*(pwbufi16 + ibuf) |= bit_mask;
		else		*(pwbufi16 + ibuf) &= ~bit_mask;
		return; 
	};

	INT16 read_bit(INT16* prbufi16) {
		INT16 val = *(prbufi16 + ibuf) &= bit_mask;
		if (val) set(L_ON); 
		else     set(L_OFF);
		return value;
	}

	HRESULT set_bit_addr(INT16 _ibuf, INT16 _bit_mask) {
		ibuf=_ibuf;	bit_mask = _bit_mask;
		return S_OK;
	}
};

class CCraneV : public CCraneValue<double> {
private:
	INT16 n_notch;							//ノッチ数
	double tbl_notch_v_ref[N_NOTCH_MAX];	//指令出力時のノッチ速度
	double tbl_notch_v_fb[N_NOTCH_MAX];		//FBノッチ換算時の速度

public:
	INT16 notch_mode;//ノッチモード
	CCraneV(INT16 _n_notch) {
		n_notch = _n_notch;
	}
	virtual ~CCraneV() {}
	HRESULT set_notch_mode(INT16 mode) {
		notch_mode = mode;
		for (int i = 0; i < N_NOTCH_MAX; i++) {

		}
		return S_OK;
	}

	INT16 get_notch_mode() {
		return notch_mode;
	}

	INT16 get_notch() {
		INT16 notch = 0;
		if (value == 0.0) { return 0; }
		else if(value < 0.0) {
			for (int i = 0; i < N_NOTCH_MAX; i++) {
				if (value > -tbl_notch_v_fb[i]) {
					notch = -i;
					return notch;
				}
			}
		}
		else {
			for (int i = 0; i < N_NOTCH_MAX; i++) {
				if (value < tbl_notch_v_fb[i]) {
					notch = i;
					return notch;
				}
			}
		}
		return N_NOTCH_MAX;
	}
	HRESULT set_notch_val(INT16 notch) {
		if (notch < 0) {
			value = -tbl_notch_v_ref[notch];
		}
		else if (notch > 0) {
			value = tbl_notch_v_ref[notch];
		}
		else {
			value = 0.0;
		}
		return S_OK;
	}
};
