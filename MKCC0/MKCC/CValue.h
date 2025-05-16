#pragma once
#include <string>
#include <windows.h>
#include "COMMON_DEF.h"
#include "CSpec.h"

using namespace std;

#define CODE_TRIG_NON		0
#define CODE_TRIG_ON		1
#define CODE_TRIG_OFF		-1


template <class T> class CValue {
public:
	CValue() { value = value_hold = 0; }
	virtual ~CValue() {}

protected:
	T value=0;	    //現在値
	T value_hold=0;	//ホールド値
public:
	T get() { return value; }
	T set(T _value) { value_hold = value;   value = _value;  return value; }

	INT16 chk_trig() {
		if ((value > value_hold)&&(value_hold==0))return CODE_TRIG_ON;
		if ((value < value_hold) && (value == 0))return CODE_TRIG_OFF;
		return CODE_TRIG_NON;
	}
	INT16 chk_trig(T _value) {
		set(_value);
		return chk_trig();
	}

	BOOL operator == (const CValue<T>& other) const {
		return this->value == other.value;
	}
	BOOL operator != (const CValue<T>& other) const {
		return this->value != other.value;
	}
};

class CMotionV : public CValue<double> {
private:
	INT32 axis_id;//軸ID
	LPST_AXIS_ITEMS p_spec;

public:
	CMotionV(LPST_AXIS_ITEMS _p_spec,INT32 id) {
		p_spec = _p_spec;
		axis_id = id;
	}
	virtual ~CMotionV() {}

	INT16 get_notch(double v,INT16 mode) {
		INT16 notch=0;
		if (value == 0.0) { return 0; }
		else if (value < 0.0) {
			double* ptbl = p_spec->notch_spd_r[mode];
			for (int notch = 0; notch < N_NOTCH_MAX; notch++) {
				if (value > *(ptbl + notch)	)
					return notch;
			}
			return notch;
		}
		else {
			double* ptbl = p_spec->notch_spd_f[mode];
			for (int notch = 0; notch < N_NOTCH_MAX; notch++) {
				if (value > *(ptbl + notch))
					return notch;
			}
			return notch;
		}
		return 0;
	}
	HRESULT set_by_notch(INT16 notch,INT16 mode) {
		value = 0.0;
		if (notch == 0) { value = 0.0; return S_OK; }

		if ((notch < 0)&&(notch > -N_NOTCH_MAX )){
			value =  p_spec->notch_spd_r[mode][-notch];
			return S_OK;
		}
		if ((notch > 0) && (notch < N_NOTCH_MAX)) {
			value = p_spec->notch_spd_f[mode][notch];
			return S_OK;
		}
		return S_FALSE;
	}
};

class CPadNotch :public CValue<INT16> {
private:
	LPST_AXIS_ITEMS p_spec;
	INT16 n;//ノッチ数

public:
	CPadNotch(LPST_AXIS_ITEMS _p_spec) {
		p_spec = _p_spec;
	}
	virtual ~CPadNotch() {}

	INT16 get_notch() { return n; }
	INT16 get_notch(INT16 pad_value) {
		
		set(pad_value);
		if (pad_value == 0) return n = 0;
		if (pad_value > 0) {
			PINT16 p = p_spec->notch_pad_f;
			for (int i = 1; i < N_NOTCH_MAX; i++) {
				if (*(p + i) > pad_value) return n=i;
			}
		}
		else {
			PINT16 p = p_spec->notch_pad_r;
			for (int i = 1; i < N_NOTCH_MAX; i++) {
				if (*(p + i) <  pad_value) return n = i;
			}
		}
		return n = 0;
	}
};


