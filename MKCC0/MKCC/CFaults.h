#pragma once
#include <Windows.h>
#include "COMMON_DEF.h"

#define N_PLC_FAULT_BUF				18
#define N_AUTO_FAULT_BUF			6
#define N_ALL_FAULT_BUF				N_PLC_FAULT_BUF+N_AUTO_FAULT_BUF
#define N_ALL_FAULTS				(N_PLC_FAULT_BUF+N_AUTO_FAULT_BUF)*16

//���쐧���r�b�g
#define FAULT_AXIS_MH_HEVY			BIT0;
#define FAULT_AXIS_BH_HEVY			BIT1;
#define FAULT_AXIS_SL_HEVY			BIT2;
#define FAULT_AXIS_GT_HEVY			BIT3;
#define FAULT_AXIS_AH_HEVY			BIT4;
#define FAULT_AXIS_MH_LIGT			BIT5;
#define FAULT_AXIS_BH_LIGT			BIT6;
#define FAULT_AXIS_SL_LIGT			BIT7;
#define FAULT_AXIS_GT_LIGT			BIT8;
#define FAULT_AXIS_AH_LIGT			BIT9;
#define FAULT_AXIS_MH_IL			BIT10;
#define FAULT_AXIS_BH_IL			BIT11;
#define FAULT_AXIS_SL_IL			BIT12;
#define FAULT_AXIS_GT_IL			BIT13;
#define FAULT_AXIS_AH_IL			BIT14;
#define FAULT_AXIS_ESTP				BIT15;


enum FAULT_TYPE {
	HEVY = 0,	//�d�̏�
	LIGT,		//�y�̏�
	IL,			//�C���^�[���b�N
	RMT,		//�����[�g
	AUTO,		//����
	ALL			//�S��
};

#define PLC_IF_N_MSG_CHAR			48

typedef struct _ST_FAULT_LIST {
	INT16	type[N_ALL_FAULTS];
	INT16	limit[N_ALL_FAULTS];	//��������鎲	
	WCHAR	text[N_ALL_FAULTS][PLC_IF_N_MSG_CHAR];	//�\���e�L�X�g
}ST_FAULT_LIST, * LPST_FAULT_LIST;

class CFaults
{
private:
	INT16* pbuf;	//��񂪊܂܂�Ă���o�b�t�@�̃A�h���X
	int crane_id;
public:
	CFaults(int _crane_id) {
		crane_id = _crane_id;
		setup(crane_id);
	}
	virtual ~CFaults() {
		;
	}

	static ST_FAULT_LIST flt_list;									//�t�H���g��`�\����
	static INT16 fault_mask[FAULT_TYPE::ALL][N_ALL_FAULT_BUF];		//�t�H���g�}�X�N

	void set_faults_bits_buf(INT16* pbufi16) {
		pbuf = pbufi16; return;
	}
	INT16* get_pfault() { return pbuf; }

	int get_id() { return crane_id; };
	int setup(int crane_id); 

	INT16 fault_bits[N_ALL_FAULT_BUF];				//���o���t�H���g�r�b�g��
	INT16 faults_trig_on[N_ALL_FAULT_BUF];			//�����t�H���g�r�b�g��
	INT16 faults_trig_off[N_ALL_FAULT_BUF];			//�����t�H���g�r�b�g��

	INT16 faults_disp[N_ALL_FAULT_BUF];				//

	int chk_flt_trig();

	INT16 is_heavy_detect();//�߂�l�́A�ŏ��Ɍ��o���ꂽ�t�H���g�̃R�[�h
	INT16 is_light_detect();//�߂�l�́A�ŏ��Ɍ��o���ꂽ�t�H���g�̃R�[�h
	INT16 is_remote_detect();//�߂�l�́A�ŏ��Ɍ��o���ꂽ�t�H���g�̃R�[�h
	INT16 is_auto_detect();//�߂�l�́A�ŏ��Ɍ��o���ꂽ�t�H���g�̃R�[�h

};

