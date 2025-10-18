#pragma once
#include "common_def.h"

/*** �d�l��`�\���� ***/
//����p�����[�^
#define ID_LV_DIR_CHECK_MARGIN		0	//�ړ���������}�[�W��
#define ID_LV_COMP_POS_PASSED		1	//�ʉߔ���}�[�W��
#define ID_LV_COMP_POS_RANGE		2	//�ʒu���ߊ�������}�[�W��

//�N���[���\����`�\����
typedef struct _ST_STRUCTURE
{
	double Mr		= 700000.0;	//��i�׏dKg
	double Lb		= 84.0;		//�W�u����
	double Zbase	= 70.0;		//�x�~���g��
	double Rbase	= 45.0;		//�x�~�����a
	double Nttl_bh	= 90.0;		//�����h��������/�努����]���̘a
	double Nbhb_base= 72.50;	//���������h�����x�~����]��
	double Nbhm_base= 17.50;	//�����努�h�����x�~����]��
	double Nmh_base = 138.1;	//�努�h�����x�~����]��
	double Ha		= 35.0;		//�|�X�g�㕔����(�W�u�s��-A�t���[��TOP�j
	double Hp		= 38.5;		//�|�X�g��������(�W�u�s������)
	double d0		= 52.809;	//�N�������d	
	double Whook	= 10000.0;	//�t�b�N�d��KG	

}ST_STRUCTURE, * LPST_STRUCTURE;

#define N_NOTCH_MAX					6	//�m�b�`�ő吔�@0�m�b�`���܂�

#define N_NOTCH_MODE				3
#define NOTCH_MODE_BASE				0
#define NOTCH_MODE_1				1
#define NOTCH_MODE_2				2

#define N_ACC_MODE					3
#define ACC_MODE_BASE				0
#define ACC_MODE_1					1
#define ACC_MODE_2					2

#define N_POS_LIMIT_TYPE			4	//������Ɍ�
#define END_LIMIT_POS				0
#define SPD_LIMIT_POS1				1
#define SPD_LIMIT_POS2				2
#define SPD_LIMIT_POS3				3
typedef struct _ST_AXIS_ITEMS {
	INT32 Axis_id;
	double	Notch_spd_f[N_NOTCH_MODE][N_NOTCH_MAX];		//�m�b�`�w�ߑ��x
	double	Notch_spd_r[N_NOTCH_MODE][N_NOTCH_MAX];		//�m�b�`�w�ߑ��x
	INT16	Notch_pad_f[N_NOTCH_MAX];					//GamePad�̃m�b�`�z��
	INT16	Notch_pad_r[N_NOTCH_MAX];					//GamePad�̃m�b�`�z��
	
	double V_rated;						// ��i���x�im/s, rad/s�F100��)
	double Rpm_rated;					// ��i��]���irpm�F100��)
	double Kw_rated;					// ��i�o�́ikW�F100��)
	double Gear_ratio;					// ������(���[�^��] /�h������] �j
	double Ddrm0;						// �h�����a0
	double Ddrm1;						// �h�����a1
	double Nwire0;						// ���[�v�|��
	double Nwire1;						// ���[�v�|��
	double Ta0;							// ��������0
	double Ta1;							// ��������1
	double Td0;							// ��������0
	double Td1;							// ��������1
	double Ndmizo0;						// �h�����a��0
	double Ndmizo1;						// �h�����a��1
	double CntPgR;						// PG1��]�J�E���g���~���{
	double CntPgDrumR;					// �h����1��]PG�J�E���g��
	double CntPgSet0;					// PG�v���Z�b�g0�J�E���g�l
	double NdrmPgSet0;					// PG�v���Z�b�g0�h������]��
	double CntAbsR;						// �A�u�\�R�[�_1��]�J�E���g���~���͎��M�A��
	double CntAbsSet0;					// �A�u�\�R�[�_�v���Z�b�g0�J�E���g�l
	double NdrmAbsSet0;					// �A�u�\�R�[�_�v���Z�b�g0�h������]��
	double Kp;							// �ʒu�ϊ��W��

	double dDdrm;						// �h����1�w�ǉ����a
	double Trq_rated;					// ��i�g���N�i���[�^���j
	double Pwr_base;					// ��i�K�v�o��
	double Trq_base;					// ��i�K�v�g���N
	double Ia;							// �������[�����g
	double Lfull;						// �t���X�p���im�j
	double PosPreset;					// �v���Z�b�g���̐ݒ�ʒu�im�j


} ST_AXIS_ITEMS, * LPST_AXIS_ITEMS;

#define N_SWAY_LEVEL				3					//����,�g���K,����
#define N_POS_LEVEL					3					//����,�g���K,����
#define ID_SWAY_LV_COMP				0					//����,�g���K,����
#define ID_SWAY_LV_TRIG				1					//����,�g���K,����
#define ID_SWAY_LV_LIMIT			2					//����,�g���K,����

#define N_DELAY_PTN					5					//��������FB�x�ꎞ�ԕ]���p�^�[����
#define ID_DLY_0START				0					//��~����̉���
#define ID_DLY_ACC_DEC				1					//�������̌����ؑ֎�
#define ID_DLY_DEC_ACC				2					//�������̉����ؑ֎�
#define ID_DLY_CNT_ACC				3					//�葬����̉�����
#define ID_DLY_CNT_DEC				4					//�葬����̌�����	

#define N_MEM_TARGET				8					//�������ڕW�z�[���h��

typedef struct _ST_AUTO_SPEC
{
	double as_rad_level[N_SWAY_LEVEL];					//�U��~�ߔ���@�U��p���x��(rad)
	double as_rad2_level[N_SWAY_LEVEL];					//�U��~�ߔ���@�U��p���x��(rad^2) 
	double as_m_level[N_SWAY_LEVEL];					//�U��~�ߔ���@�U�ꕝ���x��(m)
	double as_m2_level[N_SWAY_LEVEL];					//�U��~�ߔ���@�U�ꕝ���x��(m^2)

	double as_pos_level[N_POS_LEVEL];					//�ʒu���ߐ��䔻��@�ʒu���ꃌ�x��(m,rad)
	double pos_check_limit[N_POS_LEVEL];				//�ʒu���B����͈�(m,rad) 

	double sway_ph_chk_margin;							//�U��ʑ����B����͈�
	double delay_time[N_DELAY_PTN];						//�x�ꎞ��,���ʎ��Ԓ�`
	double default_target[N_MEM_TARGET];				//�f�t�H���g�̓o�^�ڕW�ʒu

}ST_AUTO_SPEC, * LPST_AUTO_SPEC;

typedef struct _ST_REMOTE_SPEC
{
	double Notch_spd_f[N_NOTCH_MODE][N_NOTCH_MAX];		//# �m�b�`�w�ߑ��x
	double Notch_spd_r[N_NOTCH_MODE][N_NOTCH_MAX];		//# �m�b�`�w�ߑ��x
	double notch_spd_nrm_f[N_NOTCH_MODE][N_NOTCH_MAX];	//# �m�b�`�w�ߑ��x���K��100%=1.0
	double notch_spd_nrm_r[N_NOTCH_MODE][N_NOTCH_MAX]; 	//# �m�b�`�w�ߑ��x���K��100%=1.0
}ST_REMOTE_SPEC, * LPST_REMOTE_SPEC;

class CSpec {
private:
	int crane_id;
public:
	CSpec(int _crane_id) {
		crane_id = _crane_id;
		setup(crane_id);
	};
	virtual ~CSpec() {};

	int get_crane() { return crane_id; }
	//����
	static ST_STRUCTURE	st_struct;//�\��

	//�努
	static ST_AXIS_ITEMS	base_mh;
	static ST_AUTO_SPEC		auto_mh;
	static ST_REMOTE_SPEC	rmt_mh;

	//�N��
	static ST_AXIS_ITEMS	base_bh;
	static ST_AUTO_SPEC		auto_bh;
	static ST_REMOTE_SPEC	rmt_bh;

	//����
	static ST_AXIS_ITEMS	base_sl;
	static ST_AUTO_SPEC		auto_sl;
	static ST_REMOTE_SPEC	rmt_sl;

	//���s
	static ST_AXIS_ITEMS	base_gt;
	static ST_AUTO_SPEC		auto_gt;
	static ST_REMOTE_SPEC	rmt_gt;

	//�⊪
	static ST_AXIS_ITEMS	base_ah;
	static ST_AUTO_SPEC		auto_ah;
	static ST_REMOTE_SPEC	rmt_ah;
	
	int setup(int cran_id);

};