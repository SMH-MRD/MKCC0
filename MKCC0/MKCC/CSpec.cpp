#include "CSpec.h"
#include "COMMON_DEF.h"

ST_STRUCTURE	CSpec::st_struct;//�\��
ST_STRUCTURE	st_struct0 = {
	700000.0,	//��i�׏dKg
	84.0,		//�W�u����
	70.0,		//�x�~���g��
	45.0,		//�x�~�����a
	90.0,		//�����h��������/�努����]���̘a
	72.50,		//���������h�����x�~����]��
	17.50,		//�����努�h�����x�~����]��
	138.1,		//�努�h�����x�~����]��
	35.0,		//�|�X�g�㕔����(�W�u�s��-A�t���[��TOP�j
	38.5,		//�|�X�g��������(�W�u�s������)
};

//�努
ST_AXIS_ITEMS	CSpec::base_mh;
ST_AXIS_ITEMS	base_mh0 = {
	ID_HOIST,//INT32 axis_id
	
	{//double notch_spd_f[N_NOTCH_MODE][N_NOTCH_MAX];		//�m�b�`�w�ߑ��x
		{0.0,0.023,0.070,0.140,0.233,0.233},
		{0.0,0.060,0.182,0.364,0.605,0.605},
		{0.0,0.0,0.0,0.0,0.0,0.0}
	},
	
	{//double notch_spd_r[N_NOTCH_MODE][N_NOTCH_MAX];		//�m�b�`�w�ߑ��x
		{0.0,-0.023,-0.070,-0.140,-0.233,-0.233},
		{0.0,-0.06,-0.18,-0.36,-0.60,-0.60},
		{0.0,0.0,0.0,0.0,0.0,0.0}
	},
	{2000,5000,10000,25000,0x7FFF,0x7FFF},		//INT16 notch_pad_f[N_NOTCH_MAX];	//GamePad�̃m�b�`�z��
	{-2000,-5000,-10000,-25000,0x8000,0x8000},	//INT16 notch_pad_r[N_NOTCH_MAX];	//GamePad�̃m�b�`�z��
	
	0.233,//double v_rated;��i���x�im/s, rad/s�F100��)

	/*
	//INT32 notch_spd_nrm_f[N_NOTCH_MODE][N_NOTCH_MAX];	//�m�b�`�w�ߑ��x���K��100%=1000
	{{0,100,300,600,10000,10000},{0,258,773,1545,2575,2575},{0,0,0,0,0,0}},
	//INT32 notch_spd_nrm_r[N_NOTCH_MODE][N_NOTCH_MAX];//�m�b�`�w�ߑ��x���K��100%=10000
	{{0,-100,-300,-600,-10000,-10000},{0,-258,-773,-1545,-2575,-2575},{0,0,0,0,0,0}},

	//double acc;�����x
	{0.1533,0.1491,0.0},
	//double dec;�����x
	{0.-1533,-0.1491,0.0},
	//double t_acc;������
	{1.5,2.5,0.0},
	//double t_dec;������
	{1.5,2.5,0.0},

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
	//double pos_limit_r[N_POS_LIMIT_TYPE];������Ɍ�
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
	0.0,
	//INT32 v100plc
	1000
	*/
};
ST_AUTO_SPEC	CSpec::auto_mh;
ST_REMOTE_SPEC	CSpec::rmt_mh;

//�N��
ST_AXIS_ITEMS	CSpec::base_bh;
ST_AXIS_ITEMS	base_bh0 = {

	ID_BOOM_H,//INT32 axis_id;
	{//double notch_spd_f[N_NOTCH_MODE][N_NOTCH_MAX];		//�m�b�`�w�ߑ��x
		{0.0,0.33,0.1,0.2,0.33,0.33},
		{0.0,0.0,0.0,0.0,0.0,0.0},
		{0.0,0.0,0.0,0.0,0.0,0.0}
	},
	
	{//double notch_spd_r[N_NOTCH_MODE][N_NOTCH_MAX];		//�m�b�`�w�ߑ��x
		{-0.0,-0.33,-0.1,-0.2,-0.33,-0.33},
		{0.0,0.0,0.0,0.0,0.0,0.0},
		{0.0,0.0,0.0,0.0,0.0,0.0}
	},
	{2000,5000,10000,25000,0x7FFF,0x7FFF},		//INT16 notch_pad_f[N_NOTCH_MAX];	//GamePad�̃m�b�`�z��
	{-2000,-5000,-10000,-25000,0x8000,0x8000},	//INT16 notch_pad_r[N_NOTCH_MAX];	//GamePad�̃m�b�`�z��
	0.33,//double v_rated;��i���x�im/s, rad/s�F100��)

	/*
	//INT32 notch_spd_nrm_f[N_NOTCH_MODE][N_NOTCH_MAX];	//�m�b�`�w�ߑ��x���K��100%=10000
	{{0,1000,3030,6061,10000,10000},{0,0,0,0,0,0},{0,0,0,0,0,0}},
	//INT32 notch_spd_nrm_r[N_NOTCH_MODE][N_NOTCH_MAX];//�m�b�`�w�ߑ��x���K��100%=10000
	{{0,-1000,-3030,-6061,-10000,-10000},{0,0,0,0,0,0},{0,0,0,0,0,0}},

	//double acc;�����x
	{0.22,0.0,0.0},
	//double dec;�����x
	{-0.22,0.0,0.0},
	//double t_acc;������
	{1.5,0.0,0.0},
	//double t_dec;������
	{1.5,0.0,0.0},

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
	//double pos_limit_r[N_POS_LIMIT_TYPE];������Ɍ�
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
	0.0,
	//INT32 v100plc
	1000
	*/
};
ST_AUTO_SPEC	CSpec::auto_bh;
ST_REMOTE_SPEC	CSpec::rmt_bh;

//����
ST_AXIS_ITEMS	CSpec::base_sl;
ST_AXIS_ITEMS	base_sl0 = {
	ID_SLEW,//INT32 axis_id;
	{//double notch_spd_f[N_NOTCH_MODE][N_NOTCH_MAX];		//�m�b�`�w�ߑ��x
		{0.0,0.00175,0.0175,0.035,0.035,0.035},
		{0.0,0.0013,0.013,0.026,0.026,0.026},
		{0.0,0.0,0.0,0.0,0.0,0.0}
	},
	{//double notch_spd_r[N_NOTCH_MODE][N_NOTCH_MAX];		//�m�b�`�w�ߑ��x
		{0.0,-0.00175,-0.0175,-0.035,-0.035,-0.035},
		{0.0,-0.0013,-0.013,-0.026,-0.026,-0.026},
		{0.0,0.0,0.0,0.0,0.0,0.0}
	},
	{2000,5000,10000,25000,0x7FFF,0x7FFF},		//INT16 notch_pad_f[N_NOTCH_MAX];						//GamePad�̃m�b�`�z��
	{-2000,-5000,-10000,-25000,0x8000,0x8000},	//INT16 notch_pad_r[N_NOTCH_MAX];						//GamePad�̃m�b�`�z��

	0.0349,	//1/3rpm double v_rated;��i���x�im/s, rad/s�F100��)
	/*
	//INT32 notch_spd_nrm_f[N_NOTCH_MODE][N_NOTCH_MAX];	//�m�b�`�w�ߑ��x���K��100%=10000
	{{0,500,5000,10000,10000,10000},{0,371,3714,7429,7429,7429},{0,0,0,0,0,0}},
	//INT32 notch_spd_nrm_r[N_NOTCH_MODE][N_NOTCH_MAX];//�m�b�`�w�ߑ��x���K��100%=10000
	{{0,-500,-5000,-10000,-10000,-10000},{0,-371,-3714,-7429,-7429,-7429},{0,0,0,0,0,0}},

	//double acc;�����x
	{0.02971,0.01750,0.0},
	//double dec;�����x
	{-0.02971,-0.01750,0.0},
	//double t_acc;������
	{12.0,20.0,0.0},
	//double t_dec;������
	{12.0,20.0,0.0},

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
	//double pos_limit_r[N_POS_LIMIT_TYPE];������Ɍ�
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
	0.0,
	//INT32 v100plc
	1000
	*/
};
ST_AUTO_SPEC	CSpec::auto_sl;
ST_REMOTE_SPEC	CSpec::rmt_sl;

//���s
ST_AXIS_ITEMS	CSpec::base_gt;
ST_AXIS_ITEMS	base_gt0 = {
	ID_GANTRY,//INT32 axis_id;
	{//double notch_spd_f[N_NOTCH_MODE][N_NOTCH_MAX];		//�m�b�`�w�ߑ��x
		{0.0,0.058,0.175,0.350,0.580,0.580},
		{0.0,0.0,0.0,0.0,0.0,0.0},
		{0.0,0.0,0.0,0.0,0.0,0.0}
	},
	{//double notch_spd_r[N_NOTCH_MODE][N_NOTCH_MAX];		//�m�b�`�w�ߑ��x
		{0.0,-0.058,-0.175,-0.350,-0.580,-0.580},
		{0.0,0.0,0.0,0.0,0.0,0.0},
		{0.0,0.0,0.0,0.0,0.0,0.0}
	},
	
	{2000,5000,10000,25000,0x7FFF,0x7FFF},		//INT16 notch_pad_f[N_NOTCH_MAX];	//GamePad�̃m�b�`�z��
	{-2000,-5000,-10000,-25000,0x8000,0x8000},	//INT16 notch_pad_r[N_NOTCH_MAX];	//GamePad�̃m�b�`�z��
	
	0.58,//double v_rated;��i���x�im/s, rad/s�F100��)

	/*
	//INT32 notch_spd_nrm_f[N_NOTCH_MODE][N_NOTCH_MAX];	//�m�b�`�w�ߑ��x���K��100%=10000
	{{0,1000,3017,6034,10000,10000},{0,0,0,0,0,0},{0,0,0,0,0,0}},
	//INT32 notch_spd_nrm_r[N_NOTCH_MODE][N_NOTCH_MAX];//�m�b�`�w�ߑ��x���K��100%=10000
	{{0,-1000,-3017,-6034,-10000,-10000},{0,0,0,0,0,0},{0,0,0,0,0,0}},

	//double acc;�����x
	{0.0644,0.0644,0.0},
	//double dec;�����x
	{-0.0644,-0.116,0.0},
	//double t_acc;������
	{9.0,9.0,0.0},
	//double t_dec;������
	{9.0,5.0,0.0},

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
	//double pos_limit_r[N_POS_LIMIT_TYPE];������Ɍ�
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
	0.0,
	//INT32 v100plc
	1000
	*/
};
ST_AUTO_SPEC	CSpec::auto_gt;
ST_REMOTE_SPEC	CSpec::rmt_gt;

//�⊪
ST_AXIS_ITEMS	CSpec::base_ah;
ST_AXIS_ITEMS	base_ah0 = {
	ID_AHOIST,//INT32 axis_id;
	/*
	//double notch_spd_f[N_NOTCH_MODE][N_NOTCH_MAX];		//�m�b�`�w�ߑ��x
	{{0.0,0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0,0.0}},
	//double notch_spd_r[N_NOTCH_MODE][N_NOTCH_MAX];		//�m�b�`�w�ߑ��x
	{{0.0,0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0,0.0}},
	//INT32 notch_spd_nrm_f[N_NOTCH_MODE][N_NOTCH_MAX];	//�m�b�`�w�ߑ��x���K��100%=10000
	{{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0}},
	//INT32 notch_spd_nrm_r[N_NOTCH_MODE][N_NOTCH_MAX];//�m�b�`�w�ߑ��x���K��100%=10000
	{{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0}},
	//INT16 notch_pad_f[N_NOTCH_MAX];						//GamePad�̃m�b�`�z��
	{2000,5000,10000,25000,0x7FFF,0x7FFF},
	//INT16 notch_pad_r[N_NOTCH_MAX];						//GamePad�̃m�b�`�z��
	{-2000,-5000,-10000,-25000,0x8000,0x8000},
	//double acc;�����x
	{0.0,0.0,0.0},
	//double dec;�����x
	{0.0,0.0,0.0},
	//double t_acc;������
	{0.0,0.0,0.0},
	//double t_dec;������
	{0.0,0.0,0.0},
	//double v_rated;��i���x�im/s, rad/s�F100��)
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
	//double pos_limit_r[N_POS_LIMIT_TYPE];������Ɍ�
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
	*/
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

