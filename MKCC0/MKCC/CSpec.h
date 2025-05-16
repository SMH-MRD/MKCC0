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
	double wheel_span = 20.0;	// �z�C���X�p��
	double leg_span = 15.0;		// �r�ԃX�p��
	double Hp = 47.7;			// �u�[������������
	double Lb = 64.009;			// �u�[������

	double Lm = 63.000;			//�W�upin-�努�V�[�u���S����
	double La_add = 4.981;		//�W�u��⊪�V�[�u�ԋ���
	double rad_mh_ah = 2.371;	//�W�u��⊪�V�[�u�Ԋp�x
	double La = 66.566;			//�W�upin-�⊪�V�[�u���S����
	double rad_Lm_La = 0.0537;	//�W�u��⊪�V�[�u�Ԋp�x
	double Lmb = 1.676;
	double Lma = 4.981;
	double Lba = 5.512;
	double Th_pba = 2.00301;
	double Alpa_db = 0.90266;
	double Alpa_b = 0.02109;	//�W�u�努���|�N�����Ԋp�x
	double Alpa_a = 0.05373;	//�W�u�⊪���|�努���Ԋp�x
	double Phbm = 2.20547;
	double Lp = 25.174;
	double Php = 1.6583;
	double Lmf = 2.9000;
	double Laf = 2.2910;

	double Th_rest = -0.7113;	//�W�u���X�g���̋N���p
	double D_min = 41.425;	//�ŏ����a���̋N���V�[�u�ԋ���
	double D_rest = 83.905;	//�W�u���X�g���̋N���V�[�u�ԋ���
	double Hb_min = 59.397;	//�ŏ����a���̋N������
	double Hb_rest = -41.786;	//�W�u���X�g���N������
	double Dttb = 3.652;	//TTB�a
	double Kttb = 0.08434;	//�s�j�I���a/TTB�a
	double Nbh_drum = 67.974;	//�����h������]���̘a�i�����{�努�j
	double Load0_mh = 11000.0;	//�努�݋�׏d
	double Load0_ah = 1900.0;	//�⊪�݋�׏d
}ST_STRUCTURE, * LPST_STRUCTURE;

#define N_NOTCH_MAX					6	//�m�b�`�ő吔�@0�m�b�`���܂�

#define N_NOTCH_MODE				3
#define NOTCH_MODE_BASE				0
#define NOTCH_MODE_1				1
#define NOTCH_MODE_2				2

#define N_POS_LIMIT_TYPE			4	//������Ɍ�
#define END_LIMIT_POS				0
#define SPD_LIMIT_POS1				1
#define SPD_LIMIT_POS2				2
#define SPD_LIMIT_POS3				3
typedef struct _ST_AXIS_ITEMS {
	double notch_spd_f[N_NOTCH_MODE][N_NOTCH_MAX];		//�m�b�`�w�ߑ��x
	double notch_spd_r[N_NOTCH_MODE][N_NOTCH_MAX];		//�m�b�`�w�ߑ��x
	double notch_spd_nrm_f[N_NOTCH_MODE][N_NOTCH_MAX];	//�m�b�`�w�ߑ��x���K��100%=1.0
	double notch_spd_nrm_r[N_NOTCH_MODE][N_NOTCH_MAX]; 	//�m�b�`�w�ߑ��x���K��100%=1.0

	INT16 notch_pad_f[N_NOTCH_MAX];						//GamePad�̃m�b�`�z��
	INT16 notch_pad_r[N_NOTCH_MAX];						//GamePad�̃m�b�`�z��

	double acc;			//�����x
	double dec;			//�����x
	double v_rated;		// ��i���x�im/s, rad/s�F100��)
	double t_acc;		// �������ԁi100%)

	double motor_rps;	// ���[�^��i��]���irps�@100%�j
	double motor_rpm;	// ���[�^��i��]���irpm�@100%�j
	double gear_ratio;	// ������(�h������] / ���[�^��]�j
	double drum_rps;	// ��i�h������]���x�i100��rps)
	double drum_rpps;	// ��i�h������]�����x�ir/s~2)

	double pos_limit_f[N_POS_LIMIT_TYPE] ;//������Ɍ�

	double n_wire;		// ���C���|��
	double n_boom_wire;	// ���C���|���u�[����
	double n_ex_wind;	// ��������
	double n_tread;		// �h�����a��
	double d_drum;		// �P�w�h�����a
	double d_wire;		// ���C���a
	double d_drum_layer;// �h�����a�w���Z�l
	double l_wire;		// ����C����(�����TTB�~��
} ST_AXIS_ITEMS, * LPST_AXIS_ITEMS;

#define N_SWAY_LEVEL				3	//����,�g���K,����
#define N_POS_LEVEL					3	//����,�g���K,����
#define ID_SWAY_LV_COMP				0	//����,�g���K,����
#define ID_SWAY_LV_TRIG				1	//����,�g���K,����
#define ID_SWAY_LV_LIMIT			2	//����,�g���K,����

#define N_DELAY_PTN					5	//��������FB�x�ꎞ�ԕ]���p�^�[����
#define ID_DLY_0START				0	//��~����̉���
#define ID_DLY_ACC_DEC				1	//�������̌����ؑ֎�
#define ID_DLY_DEC_ACC				2	//�������̉����ؑ֎�
#define ID_DLY_CNT_ACC				3	//�葬����̉�����
#define ID_DLY_CNT_DEC				4	//�葬����̌�����	

#define N_MEM_TARGET				8	//�������ڕW�z�[���h��

typedef struct _ST_AUTO_SPEC
{
	double as_rad_level[N_SWAY_LEVEL];	//�U��~�ߔ���@�U��p���x��(rad)
	double as_rad2_level[N_SWAY_LEVEL];	//�U��~�ߔ���@�U��p���x��(rad^2) 
	double as_m_level[N_SWAY_LEVEL];	//�U��~�ߔ���@�U�ꕝ���x��(m)
	double as_m2_level[N_SWAY_LEVEL];	//�U��~�ߔ���@�U�ꕝ���x��(m^2)

	double as_pos_level[N_POS_LEVEL];	//�ʒu���ߐ��䔻��@�ʒu���ꃌ�x��(m,rad)
	double pos_check_limit[N_POS_LEVEL];//�ʒu���B����͈�(m,rad) 

	double sway_ph_chk_margin;			//�U��ʑ����B����͈�
	double delay_time[N_DELAY_PTN];		//�x�ꎞ��,���ʎ��Ԓ�`
	double default_target[N_MEM_TARGET];//�f�t�H���g�̓o�^�ڕW�ʒu

}ST_AUTO_SPEC, * LPST_AUTO_SPEC;

typedef struct _ST_REMOTE_SPEC
{
	double notch_spd_f[N_NOTCH_MODE][N_NOTCH_MAX];		//# �m�b�`�w�ߑ��x
	double notch_spd_r[N_NOTCH_MODE][N_NOTCH_MAX];		//# �m�b�`�w�ߑ��x
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