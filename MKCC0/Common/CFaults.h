#pragma once
#include <Windows.h>
#include "COMMON_DEF.h"

#define N_PLC_FAULT_BUF				18
#define N_PC_FAULT_BUF				4
#define N_ALL_FAULTS				(N_PLC_FAULT_BUF + N_PC_FAULT_BUF)*16
#define N_PLC_FAULT_ITEM			N_PLC_FAULT_BUF * 16
#define N_PC_FAULT_ITEM				N_PC_FAULT_BUF * 16

#define N_RPC_FLT_CODE_OFFSET		850
#define N_PC_FLT_CODE_OFFSET		550
#define N_FLT_DISP_CODE_OFFSET		300

//�̏��ʃr�b�g
#define FAULT_CLEAR					BIT0
#define FAULT_HEAVY1				BIT1
#define FAULT_HEAVY2				BIT2
#define FAULT_HEAVY3				BIT3
#define FAULT_LIGHT					BIT4
#define FAULT_INTERLOCK				BIT5
#define FAULT_PC_CTRL				BIT6
#define FAULT_HISTORY				BIT7//����


//���쐧���r�b�g
#define FAULT_COMMON_HEVY			BIT0
#define FAULT_AXIS_MH_HEVY			BIT1
#define FAULT_AXIS_BH_HEVY			BIT2
#define FAULT_AXIS_SL_HEVY			BIT3
#define FAULT_AXIS_GT_HEVY			BIT4
#define FAULT_AXIS_AH_HEVY			BIT5

#define FAULT_COMMON_LIGT			BIT8
#define FAULT_AXIS_MH_LIGT			BIT9
#define FAULT_AXIS_BH_LIGT			BIT10
#define FAULT_AXIS_SL_LIGT			BIT11
#define FAULT_AXIS_GT_LIGT			BIT12
#define FAULT_AXIS_AH_LIGT			BIT13




enum FAULT_TYPE {
	BASE = 0,	//��Ɨp
	HEVY1,		//�d�̏�1
	HEVY2,		//�d�̏�2
	HEVY3,		//�d�̏�3
	LIGHT,		//�y�̏�
	IL,			//�a��
	PC,			//PC
	AUTO,		//����
	ALL			//�S��
};

#define PLC_IF_N_ITEM_CHAR			48
#define PLC_IF_N_MSG_CHAR			48

typedef struct _ST_FAULT_ITEM {
	INT16	type;						//��ʁ@
	INT16	limit;						//��������鎲	0�|8bit�F���@
	WCHAR	item[PLC_IF_N_MSG_CHAR];	//�\���e�L�X�g(���ږ�)
	WCHAR	comment[PLC_IF_N_MSG_CHAR];	//�\���e�L�X�g(�R�����g)
}ST_FAULT_ITEM, * LPST_FAULT_ITEM;

typedef struct _ST_FAULT_LIST {
	ST_FAULT_ITEM plc_faults[N_PLC_FAULT_ITEM];	//�o�^�t�H���g��
	INT16 plc_fault_mask[FAULT_TYPE::ALL][N_PLC_FAULT_BUF];	//�o�^�t�H���g��
	ST_FAULT_ITEM pc_faults[N_PC_FAULT_ITEM];	//�o�^�t�H���g��
}ST_FAULT_LIST, * LPST_FAULT_LIST;


class CFaults
{
private:
	int crane_id;

public:
	INT16* prbuf;		//PLC����̓ǂݎ��o�b�t�@�A�h���X
	INT16* pwbuf;		//PLC�ւ̏������݃o�b�t�@�A�h���X(���u,�����pFault�Z�b�g�p�j
	INT16* prfltbuf;	//PLC����̌̏�r�b�g�o�b�t�@�A�h���X
	INT16* pwfltbuf;	//PLC�ւ̌̏�r�b�g�o�b�t�@�A�h���X(���u,�����pFault�Z�b�g�p�j

	CFaults(int _crane_id,INT16* _prbuf,INT16* _pwbuf) {
		crane_id = _crane_id;
		prbuf = _prbuf;
		pwbuf = _pwbuf;
		setup(crane_id);
	}
	CFaults(int _crane_id) {
		crane_id = _crane_id;
		setup(crane_id);
	}
	virtual ~CFaults() {
		;
	}

	ST_FAULT_LIST flt_list;									//�t�H���g��`�\����
	

	int get_id() { return crane_id; };
	int setup(int crane_id); 

	UINT16 faults_disp[N_PLC_FAULT_BUF];			//�\���p�t�H���g�r�b�g��
	UINT16 faults_work[N_PLC_FAULT_BUF];			//��Ɨp�t�H���g�r�b�g��
	UINT16 faults_chkmask[N_PLC_FAULT_BUF];			//�`�F�b�N�p�}�X�N�r�b�g��

	void set_flt_mask(int code);//�t�H���g�`�F�b�N�}�X�N�Z�b�g
	int set_disp_buf(int code);//�\���p�t�H���g�r�b�g��Z�b�g

};

//## ����PC���o�t�H���g�}�b�v��`
#define FLTS_ID_ERR_CPC_PLC_COMM		0
#define FLTS_MASK_ERR_CPC_PLC_COMM		0x0001
#define FLTS_ID_ERR_CPC_RPC_COMM		0
#define FLTS_MASK_ERR_CPC_RPC_COMM		0x0002
#define FLTS_ID_ERR_CPC_SLBRK_COMM		0
#define FLTS_MASK_ERR_CPC_SLBRK_COMM	0x0004
#define FLTS_ID_RMTSW_OFF				0
#define FLTS_MASK_RMTSW_OFF				0x0008
#define FLTS_ID_ERR_RPC_RPLC_COMM		0
#define FLTS_MASK_ERR_RPC_ESTP			0x0010
#define FLTS_ID_RMT_OPE_DEACTIVE		0
#define FLTS_MASK_ERR_RPC_RPLC_COMM		0x0020
#define FLTS_ID_RMT_OPE_DEACTIVE		0
#define FLTS_MASK_RMT_OPE_DEACTIVE		0x0040

#define FLTS_ID_ERR_SLBRK_ESTOP			0
#define FLTS_MASK_ERR_SLBRK_ESTOP		0x0400
#define FLTS_ID_ERR_SLBRK_TIME_OV		0
#define FLTS_MASK_ERR_SLBRK_TIME_OV		0x0800
#define FLTS_ID_ERR_SLBRK_PLC_ERR		0
#define FLTS_MASK_ERR_SLBRK_PLC_ERR		0x1000
#define FLTS_ID_ERR_SLBRK_HW_ERR		0
#define FLTS_MASK_ERR_SLBRK_HW_ERR		0x2000
#define FLTS_ID_ERR_SLBRK_LOCAL_MODE	0
#define FLTS_MASK_ERR_SLBRK_LOCAL_MODE	0x4000
#define FLTS_ID_ERR_SLBRK_HEALTHY_ERR	0
#define FLTS_MASK_ERR_SLBRK_HEALTHY_ERR	0x8000
