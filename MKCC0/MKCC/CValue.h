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
	T value=0;	    //���ݒl
	T value_hold=0;	//�z�[���h�l
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

//���x�o�����[
class CVValue : public CValue<double> {
private:
	INT32 axis_id;//��ID
	LPST_AXIS_ITEMS p_spec;

public:
	CVValue(LPST_AXIS_ITEMS _p_spec,INT32 id) {//�����Fspec�ݒ�l�@��ID
		p_spec = _p_spec;
		axis_id = id;
	}
	virtual ~CVValue() {}

	//���x�w���PLC IO�p�R�[�h�̎擾,�o��
	INT16 get_code4(int mode, LPST_PLC_IO_DEF piodef) {//���x�w���PLC IO�p�R�[�h�擾(4�m�b�`�^�C�v�j
		return CNotchHelper::get_code4_by_v(value, p_spec->notch_spd_f[mode], p_spec->notch_spd_r[mode], piodef->shift);
	}
	INT16 set_code4(int mode, LPST_PLC_IO_DEF piodef) {//���x�w���PLC IO�p�R�[�h�ݒ�(4�m�b�`�^�C�v�j
		return CNotchHelper::set_code4_by_v(piodef->pi16, value, p_spec->notch_spd_f[mode], p_spec->notch_spd_r[mode], piodef->shift);
	}
	INT16 get_notch4(int mode, LPST_PLC_IO_DEF piodef) {//���x�w��Ńm�b�`�擾(4�m�b�`�^�C�v�j
		return CNotchHelper::get_notch4_by_v(value, p_spec->notch_spd_f[mode], p_spec->notch_spd_r[mode]);
	}
	INT16 get_iui4_by_v(int mode, LPST_PLC_IO_DEF piodef) {//UI�z��p�C���f�N�X�擾:UI�z��R5-F5�̏�
		return CNotchHelper::get_iui4_by_v(value, p_spec->notch_spd_f[mode], p_spec->notch_spd_r[mode]);
	}
	double set_from_plc(LPST_PLC_IO_DEF piodef) {
		double val = p_spec->v_rated * (double)(*piodef->pi16);
		return set(val);
	}
};

class CPadNotch :public CValue<INT16> {
private:
	INT32 axis_id;//��ID
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


