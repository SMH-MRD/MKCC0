#pragma once
#include <Windows.h>
#include "COMMON_DEF.h"

#define N_PLC_FAULT_BUF				18
#define N_ALL_FAULT_BUF				N_PLC_FAULT_BUF
#define N_ALL_FAULTS				(N_PLC_FAULT_BUF)*16

//�̏��ʃr�b�g
#define FAULT_CLEAR					BIT0
#define FAULT_HEAVY1				BIT1
#define FAULT_HEAVY2				BIT2
#define FAULT_HEAVY3				BIT3
#define FAULT_LIGHT					BIT4
#define FAULT_INTERLOCK				BIT5


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
	WORK = 0,	//��Ɨp
	HEVY1,		//�d�̏�1
	HEVY2,		//�d�̏�2
	HEVY3,		//�d�̏�3
	LIGHT,		//�y�̏�
	IL,			//�a��
	RMT,		//�����[�g
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
	ST_FAULT_ITEM faults[N_ALL_FAULTS];	//�o�^�t�H���g��
	INT16 fault_mask[FAULT_TYPE::ALL][N_ALL_FAULT_BUF];	//�o�^�t�H���g��
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
	virtual ~CFaults() {
		;
	}

	ST_FAULT_LIST flt_list;									//�t�H���g��`�\����
	

	int get_id() { return crane_id; };
	int setup(int crane_id); 

	UINT16 faults_hold[N_PLC_FAULT_BUF];			//�t�H���g�r�b�g��ێ�(�O��l�j
	UINT16 faults_disp[N_PLC_FAULT_BUF];			//�\���p�t�H���g�r�b�g��
	UINT16 faults_trig_on[N_PLC_FAULT_BUF];			//�����t�H���g�r�b�g��   
	UINT16 faults_trig_off[N_PLC_FAULT_BUF];		//�����t�H���g�r�b�g��
	UINT16 faults_work[N_PLC_FAULT_BUF];			//��Ɨp�t�H���g�r�b�g��
	UINT16 faults_chkmask[N_PLC_FAULT_BUF];			//�`�F�b�N�p�}�X�N�r�b�g��

	int chk_flt_trig();//����/�����t�H���g�r�b�g��Z�b�g
	void set_flt_mask(int code);//�t�H���g�`�F�b�N�}�X�N�Z�b�g
	int set_disp_buf(int code);//�\���p�t�H���g�r�b�g��Z�b�g

};

