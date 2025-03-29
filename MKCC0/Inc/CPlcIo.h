#pragma once
#include <Windows.h>
#include "COMMON_DEF.H"

typedef struct _LIST_ID_W_WRITE {
	INT16   helthy;            //PC�w���V�[�o�͐M��
	INT16   ctrl_mode;         //��������t���O
	INT16	cab_ai;            //�^�]��PLC AI�M���y�����p�z
	INT16   cab_di;            //�^�]��PLC���d�C��PLC b�o��
	INT16	hcounter;          //�����J�E���^���j�b�g 
	INT16	absocoder;         //�A�u�\�R�[�_ 
	INT16   pc_fault;          //PC���o�ُ�}�b�v
	INT16   erm_unitx;         //�d�C�����j�b�gX 020-03F�F�J�E���^���j�b�g�@040-04F�@120-12F�FABS���j�b�g
	INT16   erm_x;             //�d�C��X 060-0CF
	INT16   inv_cc_x;          //�C���o�[�^FB�������ݒl�@���f�o�C�X
	INT16   inv_cc_Wr1;        //�C���o�[�^FB�������ݒl�@rpm
	INT16   inv_cc_Wr2;        //�C���o�[�^FB�������ݒl�@�g���N0.1%
}LIST_ID_W_WRITE, *LPLIST_ID_W_WRITE;

typedef struct _LIST_ID_W_READ {
	INT16   helthy;            //PLC�w���V�[�J�E���^
	INT16   plc_ctrl;          // PLC�^�]���[�h
	INT16   cab_ai;            //�^�]��PLC���d�C��PLC W�o��
	INT16   cab_bi;            //�^�]��PLC���d�C��PLC b�o��
	INT16   erm_900;
	INT16   erm_bo;            //�d�C��PLC b�o��
	INT16   pos;               //�e���ʒu�M��
	INT16   spd_tg;            //�e���w�ߑ��x�ڕW
	INT16   plc_fault;         //�e�����x�M��
	INT16   erm_y;             //�d�C��PLC Y�o��
	INT16   erm_x;             //�d�C��PLC X����
	INT16   inv_cc_y;          //�C���o�[�^PLC DO�w��
	INT16   inv_cc_Ww1;        //�C���o�[�^PLC ���x�w�߁@rpm
	INT16   inv_cc_x;          //�C���o�[�^FB�������ݒl�@���f�o�C�X
	INT16   inv_cc_Wr1;        //�C���o�[�^FB�������ݒl�@rpm
	INT16   inv_cc_Wr2;        //�C���o�[�^FB�������ݒl�@�g���N0.1%
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
    UN_PLC_DATA _io_val;	    //�l
    UN_PLC_DATA _disp_val;	//�l
	INT16*		_prbuf;	//IO READ	�o�b�t�@�ւ̃|�C���^
	INT16*		_pwbuf;	//IO WRITE	�o�b�t�@�ւ̃|�C���^
    INT16		_param;	//�r�b�g�ʒu

public:
	UN_PLC_DATA get_disp() { return _disp_val; }
    UN_PLC_DATA get_io() { return _io_val; }
	
	virtual void set_disp(UN_PLC_DATA _val) = 0;
	virtual void write_io() = 0;					//disp_val�̓��e��IO�o�b�t�@�ɏo��
	virtual void write_io(UN_PLC_DATA _val) = 0;    //disp_val�̓��e���Z�b�g����IO�o�b�t�@�ɏo��
    virtual UN_PLC_DATA read_io() = 0;						//disp�̓��e��IO�o�b�t�@����ǂݏo��
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
