#pragma once
#include <Windows.h>
#include "COMMON_DEF.H"
#include "PLC_DEF.H"
#include "..\MKCC\\CPlc.h"
/// <summary>
/// PLC�Ƃ�IO�M����舵���N���X
/// wval:PC���㏑������l�@rval:PLC������Ɏg���l�i�㏑�����鎞�͏㏑����̌��ʁj
/// wprm,rprm��W���W�X�^(INT16�j����M�������o���Ƃ��̃}�X�N���@
/// </summary>
/// <typeparam name="T"></typeparam>
template <class T> class CPlcIO {
public:
	CPlcIO() {}
	virtual ~CPlcIO() {}

protected:
    T _wval_plc = 0;		//PLC�ւ̏o�͒l
    T _rval_plc = 0;		//PLC����̓��͒l

	T*		_pwbuf = NULL;	//IO WRITE	�o�b�t�@�ւ̃|�C���^
	T*		_prbuf = NULL;	//IO READ	�o�b�t�@�ւ̃|�C���^
    PINT16	_pwprm = NULL;	//�p�����[�^�ւ̃|�C���^
	PINT16	_prprm = NULL;	//�p�����[�^�ւ̃|�C���^
	T dummy_buf = 0;

public:
	virtual HRESULT set_w(T val) { _wval_plc = val; return S_OK;};

	T get_w() { return _wval_plc; }
	T get_r() { return _rval_plc; }

	HRESULT set_wbuf(PINT16 p) {				//PLC�ւ̏������݃o�b�t�@�̏������ݐ�|�C���^�Z�b�g
		if (p == NULL)	{ return S_FALSE; }
		else			{ _pwbuf = p; return S_OK; }
	}
	HRESULT set_rbuf(PINT16 p) {				//PLC����̓ǂݍ��݃o�b�t�@�̓ǂݍ��ݐ�|�C���^�Z�b�g
		if (p == NULL) { return S_FALSE; }
		else { _prbuf = p; return S_OK; }
	}

	virtual HRESULT write_io(T val)		= 0;    //_wval_plc�̓��e���Z�b�g����PLC�ւ̏������݃o�b�t�@�ɏo��
    virtual T read_io()					= 0;	// PLC�ǂݍ��݃o�b�t�@����ǂݏo��_rval_plc�ɃZ�b�g
};

//*CNotchIO *****************************************************************
#define PLCIO_N_NOTCH_INDEX     11
#define PLCIO_0_NOTCH_INDEX     5
#define PLCIO_N_PTN_BIT         6	//�p�^�[���ݒ�@�r�b�g��

class CNotchIO : public CPlcIO<INT16> {
private:
	INT16 bit_mask_w = 0, bit_mask_r = 0;
public:
	INT16 o_ptn[PLCIO_N_NOTCH_INDEX]	    // �m�b�`�w�߃p�^�[���@0 Notch:index5  4 Notch:index9  -4 Notch:index1
		= { PTN_NOTCH_R4, PTN_NOTCH_R4, PTN_NOTCH_R3, PTN_NOTCH_R2, PTN_NOTCH_R1, PTN_NOTCH_0, PTN_NOTCH_F1, PTN_NOTCH_F2,PTN_NOTCH_F3, PTN_NOTCH_F4, PTN_NOTCH_F4 };
	INT16 i_ptn[PLCIO_N_NOTCH_INDEX]		// �m�b�`FB�p�^�[���@�@0 Notch:index5  4 Notch:index9  -4 Notch:index1
		= { PTN_NOTCH_R4, PTN_NOTCH_R4, PTN_NOTCH_R3, PTN_NOTCH_R2, PTN_NOTCH_R1, PTN_NOTCH_0, PTN_NOTCH_F1, PTN_NOTCH_F2,PTN_NOTCH_F3, PTN_NOTCH_F4, PTN_NOTCH_F4 };
public:
	CNotchIO(PINT16 prbuf, PINT16 pwbuf, PINT16 pwprm, PINT16 prprm){
		_prbuf = prbuf;	_pwbuf = pwbuf;	//PLC IO�o�b�t�@
		_pwprm = pwprm; _prprm = prprm;	//�p�����[�^�̓r�b�g�}�X�N
		if(pwprm != NULL)bit_mask_w = *pwprm;
		if(prprm != NULL)bit_mask_r = *prprm;
		setup_notch_ptn();
	}
    ~CNotchIO() {}

	virtual HRESULT write_io(INT16 val);
	virtual INT16 read_io();

	HRESULT setup_notch_ptn();	//�m�b�`�p�^�[�����Z�b�g�A�b�v���@IO���t�ɉ����ăV�t�g
};

//*CBitIO *****************************************************************

class CBitIO : public CPlcIO <INT16>{
private:
	INT16 bit_mask_w = 0,bit_mask_r = 0;
public:
	CBitIO(PINT16 prbuf, PINT16 pwbuf, PINT16 pwprm, PINT16 prprm) {
		_prbuf = prbuf;	_pwbuf = pwbuf;
		_pwprm = pwprm; _prprm = prprm;

		if (pwprm != NULL)bit_mask_w = *(pwprm);
		if (prprm != NULL)bit_mask_r = *(prprm);
	}
	~CBitIO() {}

	virtual HRESULT write_io(INT16 val) { 
	
		if (_pwbuf == NULL) return S_FALSE;

		_wval_plc = val; 
		if (_wval_plc == L_ON)	*_pwbuf |= bit_mask_w; 
		else					*_pwbuf &= ~bit_mask_w;
		return S_OK; 
	}

	virtual INT16 read_io() {
		if (_prbuf == NULL) return S_FALSE;

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
		_pwprm = pwprm; _prprm = prprm;
	}
	~CWordIO() {}
	virtual HRESULT write_io(INT16 val) {
		if (_pwbuf == NULL) return S_FALSE;

		set_w(val); *_pwbuf = val; return S_OK;
	}
	virtual INT16 read_io() {
		if (_prbuf == NULL) return S_FALSE;

		return _rval_plc = *_prbuf;
	}
};

//*CDWordIO *****************************************************************
class CDWordIO : public CPlcIO<INT32> {
public:
	CDWordIO(PINT32 prbuf, PINT32 pwbuf, PINT16 pwprm, PINT16 prprm)
	{
		_prbuf = prbuf;	_pwbuf = pwbuf;
		_pwprm = pwprm; _prprm = prprm;
	}
	~CDWordIO() {}
	virtual HRESULT write_io(INT16 val) {
		if (_pwbuf == NULL) return S_FALSE;

		set_w(val); *_pwbuf = val; return S_OK;
	}
	virtual INT32 read_io() {
		if (_prbuf == NULL) return S_FALSE;
		return _rval_plc = *_prbuf;
	}
};

//*CModeIO *****************************************************************
class CModeIO : public CPlcIO<INT16> {
private:
	INT16 bit_ptn[16] = { (INT16)BIT0,(INT16)BIT1,(INT16)BIT2,(INT16)BIT3,(INT16)BIT4,(INT16)BIT5,(INT16)BIT6 ,(INT16)BIT7 ,(INT16)BIT8 ,(INT16)BIT9 ,(INT16)BIT10 ,(INT16)BIT11 ,(INT16)BIT12 ,(INT16)BIT13,(INT16)BIT14 ,(INT16)BIT15 };

	INT16 bit_mask_w = 0, nbit_w = 0, ibit0_w = 0;
	INT16 bit_mask_r = 0, nbit_r = 0, ibit0_r = 0;

public:
	CModeIO(PINT16 prbuf, PINT16 pwbuf, PINT16 pwprm, PINT16 prprm) {
		_prbuf = prbuf;	_pwbuf = pwbuf;
		_pwprm = pwprm; _prprm = prprm;

		bit_mask_r = *(prprm); 
		bit_mask_w = *(pwprm);

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

