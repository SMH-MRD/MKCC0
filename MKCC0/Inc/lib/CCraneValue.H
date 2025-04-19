#pragma once
#include <string>
#include <windows.h>
#include "COMMON_DEF.h"

using namespace std;

template <class T> class CCraneValue {
public:
	CCraneValue() { value = value_hold = 0; }
	virtual ~CCraneValue() {}

protected:
	T value;	    //現在値
	T value_hold;

public:
	T get() { return value; }
	HRESULT set(T _value, bool flg_update_hold) { if (flg_update_hold)value_hold = value;   value = _value;  return S_OK; }
	HRESULT set(T _value) { value = _value;  return S_OK; }

	BOOL chk_trig()		{ set(value, true); if (value != value_hold)return true;return false; }
	BOOL chk_trig_on()	{ set(value, true); if (value >  value_hold)return true;return false; }
	BOOL chk_trig_off() { set(value, true); if (value <  value_hold)return true;return false; }

	BOOL operator == (const CCraneValue<T>& other) const {
		return this->value == other.value;
	}
	BOOL operator != (const CCraneValue<T>& other) const {
		return this->value != other.value;
	}
};

// 軸速度Value
class CSignal : public CCraneValue<INT16> {
public:
	CSignal() {}
	virtual ~CSignal() {}
};

class CCraneV : public CCraneValue<double> {
private:
	ST_CRANE_V_SPEC st_spec;
	INT16 n_notch;							//ノッチ数
	double tbl_notch_v_ref[N_NOTCH_MAX];	//指令出力時のノッチ速度
	double tbl_notch_v_fb[N_NOTCH_MAX];		//FBノッチ換算時の速度

public:
	INT16 notch_mode;//ノッチモード
	CCraneV(LPST_CRANE_V_SPEC pst_spec, INT16 _n_notch) {
		st_spec = *pst_spec;
		n_notch = _n_notch;
	}
	virtual ~CCraneV() {}
	HRESULT set_notch_mode(INT16 mode) {
		notch_mode = mode;
		for (int i = 0; i < N_NOTCH_MAX; i++) {
			tbl_notch_v_ref[i] = st_spec.v100 * st_spec.norm_notch[mode][i];
			//ノッチ判定値は、上ノッチ指令との平均;
			tbl_notch_v_fb[i] = 0.5 * st_spec.v100 * (st_spec.norm_notch[mode][i] + st_spec.norm_notch[mode][i+1]);
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
