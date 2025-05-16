#include "CSpec.h"
#include "COMMON_DEF.h"

ST_STRUCTURE	CSpec::st_struct;//�\��
ST_STRUCTURE	st_struct0 = {
	 /*wheel_span	*/ 20.0,	
	 /*leg_span		*/ 15.0,		
	 /*Hp			*/ 47.7,		
	 /*Lb			*/ 64.009,	
	 /*Lm			*/ 63.000,	
	 /*La_add		*/ 4.981,	
	 /*rad_mh_ah	*/ 2.371,	
	 /*La			*/ 66.566,	
	 /*rad_Lm_La	*/ 0.0537,	
	 /*Lmb			*/ 1.676,
	 /*Lma			*/ 4.981,
	 /*Lba			*/ 5.512,
	 /*Th_pba		*/ 2.00301,
	 /*Alpa_db		*/ 0.90266,
	 /*Alpa_b		*/ 0.02109,	
	 /*Alpa_a		*/ 0.05373,	
	 /*Phbm			*/ 2.20547,
	 /*Lp			*/ 25.174,
	 /*Php			*/ 1.6583,
	 /*Lmf			*/ 2.9000,
	 /*Laf			*/ 2.2910,
	 /*Th_rest		*/ -0.7113,	
	 /*D_min		*/ 41.425,	
	 /*D_rest		*/ 83.905,	
	 /*Hb_min		*/ 59.397,	
	 /*Hb_rest		*/ -41.786,	
	 /*Dttb			*/ 3.652,	
	 /*Kttb			*/ 0.08434,	
	 /*Nbh_drum		*/ 67.974,	
	 /*Load0_mh		*/ 11000.0,	
	 /*Load0_ah		*/ 1900.0	
};

//�努
ST_AXIS_ITEMS	CSpec::base_mh;
ST_AXIS_ITEMS	base_mh0 = {
	//double notch_spd_f[N_NOTCH_MODE][N_NOTCH_MAX];		//�m�b�`�w�ߑ��x
	{{0.0,0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0,0.0}},
	//double notch_spd_r[N_NOTCH_MODE][N_NOTCH_MAX];		//�m�b�`�w�ߑ��x
	{{0.0,0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0,0.0}},
	//double notch_spd_nrm_f[N_NOTCH_MODE][N_NOTCH_MAX];	//�m�b�`�w�ߑ��x���K��100%=1.0
	{{0.0,0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0,0.0}},
	//double notch_spd_nrm_r[N_NOTCH_MODE][N_NOTCH_MAX]; 	//�m�b�`�w�ߑ��x���K��100%=1.0
	{{0.0,0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0,0.0}},
	//INT16 notch_pad_f[N_NOTCH_MAX];						//GamePad�̃m�b�`�z��
	{5000,5000,20000,35000,40000,40000},
	//INT16 notch_pad_r[N_NOTCH_MAX];						//GamePad�̃m�b�`�z��
	{-5000,-5000.-20000,-35000,-40000,-40000},
	//double acc;�����x
	0.0,
	//double dec;�����x
	0.0,
	//double v_rated;��i���x�im/s, rad/s�F100��)
	0.0,
	//double t_acc;�������ԁi100%)
	0.0,
	//double motor_rps;���[�^��i��]���irps�@100%�j
	0.0,
	//double motor_rpm;���[�^��i��]���irpm�@100%�j
	0.0,
	//double gear_ratio;������(�h������] / ���[�^��]�j
	0.0,
	//double drum_rps;��i�h������]���x�i100��rps)
	0.0,
	//double drum_rpps;��i�h������]�����x�ir/s~2)
	0.0,
	//double pos_limit_f[N_POS_LIMIT_TYPE];������Ɍ�
	{0.0,0.0,0.0,0.0},
	//double n_wire;		// ���C���|��
	0.0,
	//double n_boom_wire;	// ���C���|���u�[����
	0.0,
	//double n_ex_wind;	// ��������
	0.0,
	//double n_tread;		// �h�����a��
	0.0,
	//double d_drum;		// �P�w�h�����a
	0.0,
	//double d_wire;		// ���C���a
	0.0,
	//double d_drum_layer;// �h�����a�w���Z�l
	0.0,
	//double l_wire;		// ����C����(�����TTB�~��
	0.0
};
ST_AUTO_SPEC	CSpec::auto_mh;
ST_REMOTE_SPEC	CSpec::rmt_mh;

//�N��
ST_AXIS_ITEMS	CSpec::base_bh;
ST_AXIS_ITEMS	base_bh0 = {
	//double notch_spd_f[N_NOTCH_MODE][N_NOTCH_MAX];		//�m�b�`�w�ߑ��x
	{{0.0,0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0,0.0}},
	//double notch_spd_r[N_NOTCH_MODE][N_NOTCH_MAX];		//�m�b�`�w�ߑ��x
	{{0.0,0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0,0.0}},
	//double notch_spd_nrm_f[N_NOTCH_MODE][N_NOTCH_MAX];	//�m�b�`�w�ߑ��x���K��100%=1.0
	{{0.0,0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0,0.0}},
	//double notch_spd_nrm_r[N_NOTCH_MODE][N_NOTCH_MAX]; 	//�m�b�`�w�ߑ��x���K��100%=1.0
	{{0.0,0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0,0.0}},
	//INT16 notch_pad_f[N_NOTCH_MAX];						//GamePad�̃m�b�`�z��
	{5000,5000,20000,35000,40000,40000},
	//INT16 notch_pad_r[N_NOTCH_MAX];						//GamePad�̃m�b�`�z��
	{-5000,-5000. - 20000,-35000,-40000,-40000},
	//double acc;�����x
	0.0,
	//double dec;�����x
	0.0,
	//double v_rated;��i���x�im/s, rad/s�F100��)
	0.0,
	//double t_acc;�������ԁi100%)
	0.0,
	//double motor_rps;���[�^��i��]���irps�@100%�j
	0.0,
	//double motor_rpm;���[�^��i��]���irpm�@100%�j
	0.0,
	//double gear_ratio;������(�h������] / ���[�^��]�j
	0.0,
	//double drum_rps;��i�h������]���x�i100��rps)
	0.0,
	//double drum_rpps;��i�h������]�����x�ir/s~2)
	0.0,
	//double pos_limit_f[N_POS_LIMIT_TYPE];������Ɍ�
	{0.0,0.0,0.0,0.0},
	//double n_wire;		// ���C���|��
	0.0,
	//double n_boom_wire;	// ���C���|���u�[����
	0.0,
	//double n_ex_wind;	// ��������
	0.0,
	//double n_tread;		// �h�����a��
	0.0,
	//double d_drum;		// �P�w�h�����a
	0.0,
	//double d_wire;		// ���C���a
	0.0,
	//double d_drum_layer;// �h�����a�w���Z�l
	0.0,
	//double l_wire;		// ����C����(�����TTB�~��
	0.0
};
ST_AUTO_SPEC	CSpec::auto_bh;
ST_REMOTE_SPEC	CSpec::rmt_bh;

//����
ST_AXIS_ITEMS	CSpec::base_sl;
ST_AXIS_ITEMS	base_sl0 = {
	//double notch_spd_f[N_NOTCH_MODE][N_NOTCH_MAX];		//�m�b�`�w�ߑ��x
	{{0.0,0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0,0.0}},
	//double notch_spd_r[N_NOTCH_MODE][N_NOTCH_MAX];		//�m�b�`�w�ߑ��x
	{{0.0,0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0,0.0}},
	//double notch_spd_nrm_f[N_NOTCH_MODE][N_NOTCH_MAX];	//�m�b�`�w�ߑ��x���K��100%=1.0
	{{0.0,0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0,0.0}},
	//double notch_spd_nrm_r[N_NOTCH_MODE][N_NOTCH_MAX]; 	//�m�b�`�w�ߑ��x���K��100%=1.0
	{{0.0,0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0,0.0}},
	//INT16 notch_pad_f[N_NOTCH_MAX];						//GamePad�̃m�b�`�z��
	{5000,5000,20000,35000,40000,40000},
	//INT16 notch_pad_r[N_NOTCH_MAX];						//GamePad�̃m�b�`�z��
	{-5000,-5000. - 20000,-35000,-40000,-40000},
	//double acc;�����x
	0.0,
	//double dec;�����x
	0.0,
	//double v_rated;��i���x�im/s, rad/s�F100��)
	0.0,
	//double t_acc;�������ԁi100%)
	0.0,
	//double motor_rps;���[�^��i��]���irps�@100%�j
	0.0,
	//double motor_rpm;���[�^��i��]���irpm�@100%�j
	0.0,
	//double gear_ratio;������(�h������] / ���[�^��]�j
	0.0,
	//double drum_rps;��i�h������]���x�i100��rps)
	0.0,
	//double drum_rpps;��i�h������]�����x�ir/s~2)
	0.0,
	//double pos_limit_f[N_POS_LIMIT_TYPE];������Ɍ�
	{0.0,0.0,0.0,0.0},
	//double n_wire;		// ���C���|��
	0.0,
	//double n_boom_wire;	// ���C���|���u�[����
	0.0,
	//double n_ex_wind;	// ��������
	0.0,
	//double n_tread;		// �h�����a��
	0.0,
	//double d_drum;		// �P�w�h�����a
	0.0,
	//double d_wire;		// ���C���a
	0.0,
	//double d_drum_layer;// �h�����a�w���Z�l
	0.0,
	//double l_wire;		// ����C����(�����TTB�~��
	0.0
};
ST_AUTO_SPEC	CSpec::auto_sl;
ST_REMOTE_SPEC	CSpec::rmt_sl;

//���s
ST_AXIS_ITEMS	CSpec::base_gt;
ST_AXIS_ITEMS	base_gt0 = {
	//double notch_spd_f[N_NOTCH_MODE][N_NOTCH_MAX];		//�m�b�`�w�ߑ��x
	{{0.0,0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0,0.0}},
	//double notch_spd_r[N_NOTCH_MODE][N_NOTCH_MAX];		//�m�b�`�w�ߑ��x
	{{0.0,0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0,0.0}},
	//double notch_spd_nrm_f[N_NOTCH_MODE][N_NOTCH_MAX];	//�m�b�`�w�ߑ��x���K��100%=1.0
	{{0.0,0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0,0.0}},
	//double notch_spd_nrm_r[N_NOTCH_MODE][N_NOTCH_MAX]; 	//�m�b�`�w�ߑ��x���K��100%=1.0
	{{0.0,0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0,0.0}},
	//INT16 notch_pad_f[N_NOTCH_MAX];						//GamePad�̃m�b�`�z��
	{5000,5000,20000,35000,40000,40000},
	//INT16 notch_pad_r[N_NOTCH_MAX];						//GamePad�̃m�b�`�z��
	{-5000,-5000. - 20000,-35000,-40000,-40000},
	//double acc;�����x
	0.0,
	//double dec;�����x
	0.0,
	//double v_rated;��i���x�im/s, rad/s�F100��)
	0.0,
	//double t_acc;�������ԁi100%)
	0.0,
	//double motor_rps;���[�^��i��]���irps�@100%�j
	0.0,
	//double motor_rpm;���[�^��i��]���irpm�@100%�j
	0.0,
	//double gear_ratio;������(�h������] / ���[�^��]�j
	0.0,
	//double drum_rps;��i�h������]���x�i100��rps)
	0.0,
	//double drum_rpps;��i�h������]�����x�ir/s~2)
	0.0,
	//double pos_limit_f[N_POS_LIMIT_TYPE];������Ɍ�
	{0.0,0.0,0.0,0.0},
	//double n_wire;		// ���C���|��
	0.0,
	//double n_boom_wire;	// ���C���|���u�[����
	0.0,
	//double n_ex_wind;	// ��������
	0.0,
	//double n_tread;		// �h�����a��
	0.0,
	//double d_drum;		// �P�w�h�����a
	0.0,
	//double d_wire;		// ���C���a
	0.0,
	//double d_drum_layer;// �h�����a�w���Z�l
	0.0,
	//double l_wire;		// ����C����(�����TTB�~��
	0.0
};
ST_AUTO_SPEC	CSpec::auto_gt;
ST_REMOTE_SPEC	CSpec::rmt_gt;

//�⊪
ST_AXIS_ITEMS	CSpec::base_ah;
ST_AXIS_ITEMS	base_ah0 = {
	//double notch_spd_f[N_NOTCH_MODE][N_NOTCH_MAX];		//�m�b�`�w�ߑ��x
	{{0.0,0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0,0.0}},
	//double notch_spd_r[N_NOTCH_MODE][N_NOTCH_MAX];		//�m�b�`�w�ߑ��x
	{{0.0,0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0,0.0}},
	//double notch_spd_nrm_f[N_NOTCH_MODE][N_NOTCH_MAX];	//�m�b�`�w�ߑ��x���K��100%=1.0
	{{0.0,0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0,0.0}},
	//double notch_spd_nrm_r[N_NOTCH_MODE][N_NOTCH_MAX]; 	//�m�b�`�w�ߑ��x���K��100%=1.0
	{{0.0,0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0,0.0}},
	//INT16 notch_pad_f[N_NOTCH_MAX];						//GamePad�̃m�b�`�z��
	{5000,5000,20000,35000,40000,40000},
	//INT16 notch_pad_r[N_NOTCH_MAX];						//GamePad�̃m�b�`�z��
	{-5000,-5000. - 20000,-35000,-40000,-40000},
	//double acc;�����x
	0.0,
	//double dec;�����x
	0.0,
	//double v_rated;��i���x�im/s, rad/s�F100��)
	0.0,
	//double t_acc;�������ԁi100%)
	0.0,
	//double motor_rps;���[�^��i��]���irps�@100%�j
	0.0,
	//double motor_rpm;���[�^��i��]���irpm�@100%�j
	0.0,
	//double gear_ratio;������(�h������] / ���[�^��]�j
	0.0,
	//double drum_rps;��i�h������]���x�i100��rps)
	0.0,
	//double drum_rpps;��i�h������]�����x�ir/s~2)
	0.0,
	//double pos_limit_f[N_POS_LIMIT_TYPE];������Ɍ�
	{0.0,0.0,0.0,0.0},
	//double n_wire;		// ���C���|��
	0.0,
	//double n_boom_wire;	// ���C���|���u�[����
	0.0,
	//double n_ex_wind;	// ��������
	0.0,
	//double n_tread;		// �h�����a��
	0.0,
	//double d_drum;		// �P�w�h�����a
	0.0,
	//double d_wire;		// ���C���a
	0.0,
	//double d_drum_layer;// �h�����a�w���Z�l
	0.0,
	//double l_wire;		// ����C����(�����TTB�~��
	0.0
};
ST_AUTO_SPEC	CSpec::auto_ah;
ST_REMOTE_SPEC	CSpec::rmt_ah;

int CSpec::setup(int crane_id) {
	switch (crane_id) {
	case CRANE_ID_H6R602:{
		memcpy_s(&st_struct, sizeof(ST_STRUCTURE), &st_struct0, sizeof(ST_STRUCTURE));
		memcpy_s(&base_mh, sizeof(ST_AXIS_ITEMS), &base_mh0, sizeof(ST_AXIS_ITEMS));
		memcpy_s(&base_bh, sizeof(ST_AXIS_ITEMS), &base_bh0, sizeof(ST_AXIS_ITEMS));
		memcpy_s(&base_sl, sizeof(ST_AXIS_ITEMS), &base_sl0, sizeof(ST_AXIS_ITEMS));
		memcpy_s(&base_gt, sizeof(ST_AXIS_ITEMS), &base_gt0, sizeof(ST_AXIS_ITEMS));
		memcpy_s(&base_ah, sizeof(ST_AXIS_ITEMS), &base_ah0, sizeof(ST_AXIS_ITEMS));
	}break;
	case CARNE_ID_HHGH29:{
		memcpy_s(&st_struct, sizeof(ST_STRUCTURE), &st_struct0, sizeof(ST_STRUCTURE));
		memcpy_s(&base_mh, sizeof(ST_AXIS_ITEMS), &base_mh0, sizeof(ST_AXIS_ITEMS));
		memcpy_s(&base_bh, sizeof(ST_AXIS_ITEMS), &base_bh0, sizeof(ST_AXIS_ITEMS));
		memcpy_s(&base_sl, sizeof(ST_AXIS_ITEMS), &base_sl0, sizeof(ST_AXIS_ITEMS));
		memcpy_s(&base_gt, sizeof(ST_AXIS_ITEMS), &base_gt0, sizeof(ST_AXIS_ITEMS));
		memcpy_s(&base_ah, sizeof(ST_AXIS_ITEMS), &base_ah0, sizeof(ST_AXIS_ITEMS));
	}break;
	case CARNE_ID_HHGQ18:{
		memcpy_s(&st_struct, sizeof(ST_STRUCTURE), &st_struct0, sizeof(ST_STRUCTURE));
		memcpy_s(&base_mh, sizeof(ST_AXIS_ITEMS), &base_mh0, sizeof(ST_AXIS_ITEMS));
		memcpy_s(&base_bh, sizeof(ST_AXIS_ITEMS), &base_bh0, sizeof(ST_AXIS_ITEMS));
		memcpy_s(&base_sl, sizeof(ST_AXIS_ITEMS), &base_sl0, sizeof(ST_AXIS_ITEMS));
		memcpy_s(&base_gt, sizeof(ST_AXIS_ITEMS), &base_gt0, sizeof(ST_AXIS_ITEMS));
		memcpy_s(&base_ah, sizeof(ST_AXIS_ITEMS), &base_ah0, sizeof(ST_AXIS_ITEMS));
	}break;
	case CARNE_ID_HHFR22:{
		memcpy_s(&st_struct, sizeof(ST_STRUCTURE), &st_struct0, sizeof(ST_STRUCTURE));
		memcpy_s(&base_mh, sizeof(ST_AXIS_ITEMS), &base_mh0, sizeof(ST_AXIS_ITEMS));
		memcpy_s(&base_bh, sizeof(ST_AXIS_ITEMS), &base_bh0, sizeof(ST_AXIS_ITEMS));
		memcpy_s(&base_sl, sizeof(ST_AXIS_ITEMS), &base_sl0, sizeof(ST_AXIS_ITEMS));
		memcpy_s(&base_gt, sizeof(ST_AXIS_ITEMS), &base_gt0, sizeof(ST_AXIS_ITEMS));
		memcpy_s(&base_ah, sizeof(ST_AXIS_ITEMS), &base_ah0, sizeof(ST_AXIS_ITEMS));
	}break;
	default: {
		memcpy_s(&st_struct, sizeof(ST_STRUCTURE), &st_struct0, sizeof(ST_STRUCTURE));
		memcpy_s(&base_mh, sizeof(ST_AXIS_ITEMS), &base_mh0, sizeof(ST_AXIS_ITEMS));
		memcpy_s(&base_bh, sizeof(ST_AXIS_ITEMS), &base_bh0, sizeof(ST_AXIS_ITEMS));
		memcpy_s(&base_sl, sizeof(ST_AXIS_ITEMS), &base_sl0, sizeof(ST_AXIS_ITEMS));
		memcpy_s(&base_gt, sizeof(ST_AXIS_ITEMS), &base_gt0, sizeof(ST_AXIS_ITEMS));
		memcpy_s(&base_ah, sizeof(ST_AXIS_ITEMS), &base_ah0, sizeof(ST_AXIS_ITEMS));
	}break;
	}
	return 0;
}

