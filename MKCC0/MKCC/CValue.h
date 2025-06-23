#pragma once
#include <string>
#include <windows.h>
#include "COMMON_DEF.h"
#include "CSpec.h"
#include "CHelper.h"
#include "CPlc.h"

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
	T set(T _value) { 
		value_hold = value;   
		value = _value;  
		return value; 
	}

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

//速度バリュー
class CVValue : public CValue<double> {
private:
	INT32 axis_id;//軸ID
	LPST_AXIS_ITEMS p_spec;

public:
	CVValue(LPST_AXIS_ITEMS _p_spec,INT32 id) {//引数：spec設定値　軸ID
		p_spec = _p_spec;
		axis_id = id;
	}
	virtual ~CVValue() {}

	//速度指定でPLC IO用コードの取得,出力
	INT16 get_code4(int mode, LPST_PLC_IO_DEF piodef) {//速度指定でPLC IO用コード取得(4ノッチタイプ）
		return CNotchHelper::get_code4_by_v(value, p_spec->notch_spd_f[mode], p_spec->notch_spd_r[mode], piodef->shift);
	}
	INT16 set_code4(int mode, LPST_PLC_IO_DEF piodef) {//速度指定でPLC IO用コード設定(4ノッチタイプ）
		return CNotchHelper::set_code4_by_v(piodef->pi16, value, p_spec->notch_spd_f[mode], p_spec->notch_spd_r[mode], piodef->shift);
	}
	INT16 get_notch4(int mode, LPST_PLC_IO_DEF piodef) {//速度指定でノッチ取得(4ノッチタイプ）
		return CNotchHelper::get_notch4_by_v(value, p_spec->notch_spd_f[mode], p_spec->notch_spd_r[mode]);
	}
	INT16 get_iui4_by_v(int mode, LPST_PLC_IO_DEF piodef) {//UI配列用インデクス取得:UI配列R5-F5の順
		return CNotchHelper::get_iui4_by_v(value, p_spec->notch_spd_f[mode], p_spec->notch_spd_r[mode]);
	}
	double set_from_plc(LPST_PLC_IO_DEF piodef) {
		double val = p_spec->v_rated * (double)(*piodef->pi16);
		return set(val);
	}
};

class CPadNotch :public CValue<INT16> {
private:
	INT32 axis_id;//軸ID
	LPST_AXIS_ITEMS p_spec;

public:
	CPadNotch(LPST_AXIS_ITEMS _p_spec, INT32 id) {
		p_spec = _p_spec;
		axis_id = id;
	}
	virtual ~CPadNotch() {}

	INT16 get_notch() {
		return CNotchHelper::get_notch_by_pad(value, p_spec->notch_pad_f, p_spec->notch_pad_r);
	}

	double get_notch_v(int mode) {
		INT16 notch = get_notch();
		if (notch < 0)	return p_spec->notch_spd_r[mode][-notch];
		else return p_spec->notch_spd_f[mode][-notch];
	}

};


