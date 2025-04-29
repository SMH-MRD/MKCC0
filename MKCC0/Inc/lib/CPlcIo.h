#pragma once
#include <Windows.h>
#include "COMMON_DEF.H"
#include "PLC_DEF.H"
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

typedef struct _ST_PLC_RBUF {
	INT16 helthy;               //PLC�w���V�[�J�E���^
	INT16 plc_ctrl;             // PLC�^�]���[�h
	UINT16 cab_ai[6];            //�^�]��PLC���d�C��PLC W�o��
	INT16 cab_bi[5];            //�^�]��PLC���d�C��PLC b�o��
	INT16 erm_900;
	INT16 erm_bo[6];            //�d�C��PLC b�o��
	INT32 pos[5];               //�e���ʒu�M��
	INT16 spd_tg[6];            //�e���w�ߑ��x�ڕW
	INT16 plc_fault[18];        //�e�����x�M��
	INT16 erm_y[4];             //�d�C��PLC Y�o��
	INT16 erm_x[8];             //�d�C��PLC X����
	INT16 inv_cc_y[6];          //�C���o�[�^PLC DO�w��
	INT16 inv_cc_Ww1[6];        //�C���o�[�^PLC ���x�w�߁@rpm
	INT16 inv_cc_x[6];          //�C���o�[�^FB�������ݒl�@���f�o�C�X
	INT16 inv_cc_Wr1[6];        //�C���o�[�^FB�������ݒl�@rpm
	INT16 inv_cc_Wr2[6];        //�C���o�[�^FB�������ݒl�@�g���N0.1%
	INT16 spare1[4];            //�\��
}ST_PLC_RBUF, * LPST_PLC_RBUF;

union UN_PLC_RBUF {
	INT16 rbuf[CC_MC_SIZE_W_READ];
	ST_PLC_RBUF st;
};

typedef struct _ST_PLC_WBUF {
	INT16 helthy;               //PC�w���V�[�o�͐M��
	INT16 ctrl_mode;            //��������t���O
	UINT16 cab_ai[8];            //�^�]��PLC AI�M���y�����p�z
	INT16 cab_di[6];            //�^�]��PLC���d�C��PLC b�o��
	INT16 spare0[8];            //�^�]��PLC���d�C��PLC b�o��
	UINT32 hcounter[4];          //�����J�E���^���j�b�g 
	UINT32 absocoder[3];         //�A�u�\�R�[�_ 
	INT16 spare1[2];            //�\��
	INT16 pc_fault[2];          //PC���o�ُ�}�b�v
	INT16 spare2[16];           //�\��
	INT16 erm_unitx[4];         //�d�C�����j�b�gX 020-03F�F�J�E���^���j�b�g�@040-04F�@120-12F�FABS���j�b�g
	INT16 erm_x[8];             //�d�C��X 060-0CF
	INT16 inv_cc_x[6];          //�C���o�[�^FB�������ݒl�@���f�o�C�X
	INT16 inv_cc_Wr1[6];        //�C���o�[�^FB�������ݒl�@rpm
	INT16 inv_cc_Wr2[6];        //�C���o�[�^FB�������ݒl�@�g���N0.1%
	INT16 spare3[12];
}ST_PLC_WBUF, * LPST_PLC_WBUF;

union UN_PLC_WBUF {
	INT16 rbuf[CC_MC_SIZE_W_WRITE];
	ST_PLC_WBUF st;
};
