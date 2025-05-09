#pragma once
#include "PLC_DEF.h"

/*** PLC IF�o�b�t�@�\���̒�` ***/
//�f�t�H���g�i�݂炢�j�̒�`
typedef struct _ST_PLC_WBUF {//����PC��PLC
	INT16   helthy;               //PC�w���V�[�o�͐M��
	INT16   ctrl_mode;            //��������t���O
	UINT16  cab_ai[8];            //�^�]��PLC AI�M���y�����p�z
	INT16   cab_bi[6];            //�^�]��PLC���d�C��PLC b�o��
	INT16   spare0[8];            //�^�]��PLC���d�C��PLC b�o��
	UINT32  hcounter[4];          //�����J�E���^���j�b�g 
	UINT32  absocoder[3];         //�A�u�\�R�[�_ 
	INT16   spare1[2];            //�\��
	INT16   pc_fault[2];          //PC���o�ُ�}�b�v
	INT16   spare2[16];           //�\��
	INT16   erm_unitx[4];         //�d�C�����j�b�gX 020-03F�F�J�E���^���j�b�g�@040-04F�@120-12F�FABS���j�b�g
	INT16   erm_x[8];             //�d�C��X 060-0CF
	INT16   inv_cc_x[6];          //�C���o�[�^FB�������ݒl�@���f�o�C�X
	INT16   inv_cc_Wr1[6];        //�C���o�[�^FB�������ݒl�@rpm
	INT16   inv_cc_Wr2[6];        //�C���o�[�^FB�������ݒl�@�g���N0.1%
	INT16   spare3[40];
}ST_PLC_WBUF, * LPST_PLC_WBUF;
typedef struct _ST_PLC_RBUF {
	INT16   helthy;               //PLC�w���V�[�J�E���^
	INT16   plc_ctrl;             // PLC�^�]���[�h
	UINT16  cab_ai[6];            //�^�]��PLC���d�C��PLC W�o��
	INT16   cab_bi[5];            //�^�]��PLC���d�C��PLC b�o��
	INT16   erm_900;
	INT16   erm_bo[6];            //�d�C��PLC b�o��
	INT32   pos[5];               //�e���ʒu�M��
	INT16   spd_tg[6];            //�e���w�ߑ��x�ڕW
	INT16   plc_fault[18];        //�e�����x�M��
	INT16   erm_y[4];             //�d�C��PLC Y�o��
	INT16   erm_x[8];             //�d�C��PLC X����
	INT16   inv_cc_y[6];          //�C���o�[�^PLC DO�w��
	INT16   inv_cc_Ww1[6];        //�C���o�[�^PLC ���x�w�߁@rpm
	INT16   inv_cc_x[6];          //�C���o�[�^FB�������ݒl�@���f�o�C�X
	INT16   inv_cc_Wr1[6];        //�C���o�[�^FB�������ݒl�@rpm
	INT16   inv_cc_Wr2[6];        //�C���o�[�^FB�������ݒl�@�g���N0.1%
	INT16   spare1[4];            //�\��
}ST_PLC_RBUF, * LPST_PLC_RBUF;

//�����x��70tJC102���̒�`
typedef struct _ST_PLC_WBUF_HHGH29 {//����PC��PLC
	INT16   helthy;               //D10200:PC�w���V�[�o�͐M��
	INT16   ctrl_mode;            //D10201:��������t���O
	UINT16  cab_ai[8];            //D10202:�^�]��PLC AI�M���y���[�����g���~�b�^�z
	INT16   cab_bi[4];            //D10210:�^�]��PLC���d�C��PLC b�o��
	UINT32  hcounter[4];          //D10214:�����J�E���^���j�b�g 
	UINT32  absocoder[3];         //D10222�A�u�\�R�[�_ 
	INT16   pc_fault[2];          //PC���o�ُ�}�b�v
	INT16   erm_unitx[6];         //�d�C�����j�b�gX 020-03F�F�J�E���^���j�b�g�@040-04F�FABS���j�b�g
	INT16   erm_x[7];             //�d�C��X 080-0EF
	INT16   inv_cc_x[6];          //�C���o�[�^FB�������ݒl�@���f�o�C�X
	INT16   inv_cc_Wr1[6];        //�C���o�[�^FB�������ݒl�@rpm
	INT16   inv_cc_Wr2[6];        //�C���o�[�^FB�������ݒl�@�g���N0.1%
	INT16   spare4[30];
}ST_PLC_WBUF_HHGH29, * LPST_PLC_WBUF_HHGH29;
typedef struct _ST_PLC_RBUF_HHGH29 {
	INT16   helthy;             // D10400:PLC�w���V�[�J�E���^
	INT16   plc_ctrl;           // D10401: PLC�^�]���[�h
	UINT16  cab_ai[4];          // D10402:�^�]��PLC���d�C��PLC W�o��
	INT16   cab_bi[4];          // D10406:�^�]��PLC���d�C��PLC b�o��
	INT16   cab_bxi[4];         // D10410:�^�]��PLC���d�C��PLC b�o��
	INT16   erm_900;			// D10414:
	INT16   erm_bo[8];          // D10415:�d�C��PLC b�o��
	INT32   pos[4];             // D10423:�e���ʒu�M��
	INT16   spd_tg[6];          // D10431:�e���w�ߑ��x�ڕW
	INT16   plc_fault[17];      // D10437:�̏�M��
	INT16   erm_y[5];           // D10454:�d�C��PLC Y�o��
	INT16   erm_x[7];           // D10459:�d�C��PLC X����
	INT16   inv_cc_y[6];        //�C���o�[�^PLC DO�w��
	INT16   inv_cc_x[6];        //�C���o�[�^FB�������ݒl�@���f�o�C�X
	INT16   inv_cc_Ww1[6];      //�C���o�[�^PLC ���x�w�߁@rpm
	INT16   inv_cc_Wr1[6];      //�C���o�[�^FB�������ݒl�@rpm
	INT16   inv_cc_Wr2[6];      //�C���o�[�^FB�������ݒl�@�g���N0.1%
	INT16   spare1[3];          //�\��
}ST_PLC_RBUF_HHGH29, * LPST_PLC_RBUF_HHGH29;

union UN_PLC_RBUF {
	INT16 rbuf[CC_MC_SIZE_W_READ];
	ST_PLC_RBUF			st;
	ST_PLC_RBUF_HHGH29	st_hhgh29;
};

union UN_PLC_WBUF {
	INT16 rbuf[CC_MC_SIZE_W_WRITE];
	ST_PLC_WBUF			st;
	ST_PLC_WBUF_HHGH29	st_hhgh29;
};

//�m�b�`�r�b�g�p�^�[��
#define PTN_NOTCH_0			0x0001
#define PTN_NOTCH_F1		0x0002
#define PTN_NOTCH_F2		0x000A
#define PTN_NOTCH_F3		0x001A
#define PTN_NOTCH_F4		0x003A
#define PTN_NOTCH_R1		0x0004
#define PTN_NOTCH_R2		0x000C
#define PTN_NOTCH_R3		0x001C
#define PTN_NOTCH_R4		0x003C

/*** PLC IO�\���̒�` ***/
#define CODE_PLCIO_WORD		0
#define CODE_PLCIO_BIT		1
#define CODE_PLCIO_DWORD	2
#define CODE_PLCIO_UNIT		3 //NOTCH CS��
#define CODE_PLCIO_FAULT	4 //�̏�

typedef struct _ST_PLC_IO_DEF {
	INT16   i;		//��񂪊܂܂�Ă���o�b�t�@�̃C���f�b�N�X
	INT16	mask;
	INT16	type;
	INT16	prm;	//�m�b�`,CS�F�r�b�g�V�t�g��,�@Fault�F�o�b�t�@�T�C�Y
}ST_PLC_IO_DEF, * LPST_PLC_IO_DEF;

typedef struct _ST_PLC_IO_RDEF {
	ST_PLC_IO_DEF estop;
	ST_PLC_IO_DEF csource;
	ST_PLC_IO_DEF faults;
}ST_PLC_IO_RDEF, * LPST_PLC_IO_RDEF;

typedef struct _ST_PLC_IO_WDEF {
	ST_PLC_IO_DEF estop;
	ST_PLC_IO_DEF csource;
	ST_PLC_IO_DEF faults;
}ST_PLC_IO_WDEF, * LPST_PLC_IO_WDEF;

class CPlc
{
	private:
	int crane_id;

public:
	CPlc(int _crane_id) {
		crane_id = _crane_id;
		setup(crane_id);
	};
	virtual ~CPlc() {};

	static ST_PLC_IO_RDEF plc_io_rdef;
	static ST_PLC_IO_WDEF plc_io_wdef;

	int setup(int cran_id);

};

