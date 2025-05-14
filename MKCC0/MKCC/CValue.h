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
	T value;	    //現在値
	T value_hold;	//ホールド値
public:
	T get() { return value; }
	T set(T _value) { value_hold = value;   value = _value;  return value; }

	INT16 chk_trig() {
		if (value > value_hold)return CODE_TRIG_ON;
		if (value < value_hold)return CODE_TRIG_OFF;
		return CODE_TRIG_NON;
	}

	BOOL operator == (const CValue<T>& other) const {
		return this->value == other.value;
	}
	BOOL operator != (const CValue<T>& other) const {
		return this->value != other.value;
	}
};

class CCraneV : public CValue<double> {
private:
	INT32 axis_id;//軸ID
	LPST_AXIS_ITEMS p_spec;

public:
	CCraneV(LPST_AXIS_ITEMS _p_spec) {
		p_spec = _p_spec;
	}
	virtual ~CCraneV() {}

	INT16 get_notch(double v,INT16 mode) {
		INT16 notch;
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
			value =  p_spec->notch_spd_r[mode][notch];
			return S_OK;
		}
		if ((notch > 0) && (notch < N_NOTCH_MAX)) {
			value = p_spec->notch_spd_f[mode][notch];
			return S_OK;
		}
		return S_FALSE;
	}
};


