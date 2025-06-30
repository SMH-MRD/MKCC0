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
ST_STRUCTURE	st_struct1 = {
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
	ID_HOIST,//INT32 Axis_id
	
	{//double Notch_spd_f[N_NOTCH_MODE][N_NOTCH_MAX];		//�m�b�`�w�ߑ��x
		{0.0,0.023,0.070,0.140,0.233,0.233},
		{0.0,0.060,0.182,0.364,0.605,0.605},
		{0.0,0.0,0.0,0.0,0.0,0.0}
	},
	
	{//double Notch_spd_r[N_NOTCH_MODE][N_NOTCH_MAX];		//�m�b�`�w�ߑ��x
		{0.0,-0.023,-0.070,-0.140,-0.233,-0.233},
		{0.0,-0.06,-0.18,-0.36,-0.60,-0.60},
		{0.0,0.0,0.0,0.0,0.0,0.0}
	},
	{2000,5000,10000,25000,0x7FFF,0x7FFF},		//INT16 Notch_pad_f[N_NOTCH_MAX];	//GamePad�̃m�b�`�z��
	{-2000,-5000,-10000,-25000,0x8000,0x8000},	//INT16 Notch_pad_r[N_NOTCH_MAX];	//GamePad�̃m�b�`�z��
	
	0.233,			//double V_rated;��i���x�im/s, rad/s�F100��)
	750.0,			// Rpm_rated;	��i��]���irpm�F100��)
	220.0,			// Kw_rated;	��i�o�́ikW�F100��)
	31.129,			// Gear_ratio;	������(���[�^��] /�h������] �j
	0.624,			// Ddrm0;		�h�����a0
	0.720,			// Ddrm1;		�h�����a1
	4.0,			// Nwire0;		���[�v�|��
	2.0,			// Nwire1;		���[�v�|��
	1.5,			// Ta0;			��������0
	2.5,			// Ta1;			��������1
	1.5,			// Td0;			��������0
	2.5,			// Td1;			��������1
	34,				// Ndmizo0;		�h�����a��0
	21,				// Ndmizo1;		�h�����a��1
	2048,			// CntPgR;		PG1��]�J�E���g���~���{
	100000000,		// CntPgSet0;	PG�v���Z�b�g0�J�E���g�l
	0.0,			// CntPgSet1;	PG�v���Z�b�g1�J�E���g�l
	64.0,			// CntAbsR;		�A�u�\�R�[�_1��]�J�E���g���~���͎��M�A��
	50000.0,		// CntAbsSet0;	PG�v���Z�b�g0�J�E���g�l
	0.0	,			// CntAbsSet1;	PG�v���Z�b�g1�J�E���g�l
	0.04157,		// dDdrm;		�h����1�w�ǉ��a
	2801.127,		// Trq_rated;	��i�g���N�i���[�^���j
	135.1407,		// Pwr_base;	��i�K�v�o��
	175.39915,		// Trq_base;	��i�K�v�g���N
	26.748792,		// Ia;			�������[�����g
	387.55,			// Lfull;		�t���X�p���im�j
};
ST_AXIS_ITEMS	base_mh1 = {
	ID_HOIST,//INT32 Axis_id

	{//double Notch_spd_f[N_NOTCH_MODE][N_NOTCH_MAX];		//�m�b�`�w�ߑ��x
		{0.0,0.023,0.070,0.140,0.233,0.233},
		{0.0,0.060,0.182,0.364,0.605,0.605},
		{0.0,0.0,0.0,0.0,0.0,0.0}
	},

	{//double Notch_spd_r[N_NOTCH_MODE][N_NOTCH_MAX];		//�m�b�`�w�ߑ��x
		{0.0,-0.023,-0.070,-0.140,-0.233,-0.233},
		{0.0,-0.06,-0.18,-0.36,-0.60,-0.60},
		{0.0,0.0,0.0,0.0,0.0,0.0}
	},
	{2000,5000,10000,25000,0x7FFF,0x7FFF},		//INT16 Notch_pad_f[N_NOTCH_MAX];	//GamePad�̃m�b�`�z��
	{-2000,-5000,-10000,-25000,0x8000,0x8000},	//INT16 Notch_pad_r[N_NOTCH_MAX];	//GamePad�̃m�b�`�z��

	0.233,			//double V_rated;��i���x�im/s, rad/s�F100��)
	750.0,			// Rpm_rated;	��i��]���irpm�F100��)
	220.0,			// Kw_rated;	��i�o�́ikW�F100��)
	31.129,			// Gear_ratio;	������(���[�^��] /�h������] �j
	0.624,			// Ddrm0;		�h�����a0
	0.720,			// Ddrm1;		�h�����a1
	4.0,			// Nwire0;		���[�v�|��
	2.0,			// Nwire1;		���[�v�|��
	1.5,			// Ta0;			��������0
	2.5,			// Ta1;			��������1
	1.5,			// Td0;			��������0
	2.5,			// Td1;			��������1
	34,				// Ndmizo0;		�h�����a��0
	21,				// Ndmizo1;		�h�����a��1
	2048,			// CntPgR;		PG1��]�J�E���g���~���{
	100000000,		// CntPgSet0;	PG�v���Z�b�g0�J�E���g�l
	0.0,			// CntPgSet1;	PG�v���Z�b�g1�J�E���g�l
	64.0,			// CntAbsR;		�A�u�\�R�[�_1��]�J�E���g���~���͎��M�A��
	50000.0,		// CntAbsSet0;	PG�v���Z�b�g0�J�E���g�l
	0.0	,			// CntAbsSet1;	PG�v���Z�b�g1�J�E���g�l
	0.04157,		// dDdrm;		�h����1�w�ǉ��a
	2801.127,		// Trq_rated;	��i�g���N�i���[�^���j
	135.1407,		// Pwr_base;	��i�K�v�o��
	175.39915,		// Trq_base;	��i�K�v�g���N
	26.748792,		// Ia;			�������[�����g
	387.55,			// Lfull;		�t���X�p���im�j
};
ST_AUTO_SPEC	CSpec::auto_mh;
ST_REMOTE_SPEC	CSpec::rmt_mh;

//�N��
ST_AXIS_ITEMS	CSpec::base_bh;
ST_AXIS_ITEMS	base_bh0 = {

	ID_BOOM_H,//INT32 Axis_id;
	{//double Notch_spd_f[N_NOTCH_MODE][N_NOTCH_MAX];		//�m�b�`�w�ߑ��x
		{0.0,0.33,0.1,0.2,0.33,0.33},
		{0.0,0.0,0.0,0.0,0.0,0.0},
		{0.0,0.0,0.0,0.0,0.0,0.0}
	},
	
	{//double Notch_spd_r[N_NOTCH_MODE][N_NOTCH_MAX];		//�m�b�`�w�ߑ��x
		{-0.0,-0.33,-0.1,-0.2,-0.33,-0.33},
		{0.0,0.0,0.0,0.0,0.0,0.0},
		{0.0,0.0,0.0,0.0,0.0,0.0}
	},
	{2000,5000,10000,25000,0x7FFF,0x7FFF},		//INT16 Notch_pad_f[N_NOTCH_MAX];	//GamePad�̃m�b�`�z��
	{-2000,-5000,-10000,-25000,0x8000,0x8000},	//INT16 Notch_pad_r[N_NOTCH_MAX];	//GamePad�̃m�b�`�z��
	0.33,				//double V_rated;��i���x�im/s, rad/s�F100��)
	1750.0,				// Rpm_rated;	��i��]���irpm�F100��)
	90.0,				// Kw_rated;	��i�o�́ikW�F100��)
	78.12,				// Gear_ratio;	������(���[�^��] /�h������] �j
	0.720,				// Ddrm0;		�h�����a0
	0.0,				// Ddrm1;		�h�����a1
	6.0,				// Nwire0;		���[�v�|��
	1.0,				// Nwire1;		���[�v�|��
	1.5,				// Ta0;			��������0
	0.0,				// Ta1;			��������1
	1.5,				// Td0;			��������0
	0.0,				// Td1;			��������1
	21.0,				// Ndmizo0;		�h�����a��0
	0.0,				// Ndmizo1;		�h�����a��1
	2048,				// CntPgR;		PG1��]�J�E���g���~���{
	86673,				// CntPgSet0;	PG�v���Z�b�g0�J�E���g�l
	0.0,				// CntPgSet1;	PG�v���Z�b�g1�J�E���g�l
	0.0,				// CntAbsR;		�A�u�\�R�[�_1��]�J�E���g���~���͎��M�A��
	0.0,				// CntAbsSet0;	�A�u�\�R�[�_�v���Z�b�g0�J�E���g�l
	0.0,				// CntAbsSet1;	�A�u�\�R�[�_�v���Z�b�g1�J�E���g�l
	0.04157,			// dDdrm;		�h����1�w�ǉ����a
	491.1067,			// Trq_rated;	��i�g���N�i���[�^���j
	0.0,				// Pwr_base;	��i�K�v�o��
	0.0,				// Trq_base;	��i�K�v�g���N
	4.885128,			// Ia;			�������[�����g
	348.50,				// Lfull;		�t���X�p���im�j

};
ST_AXIS_ITEMS	base_bh1 = {

	ID_BOOM_H,//INT32 Axis_id;
	{//double Notch_spd_f[N_NOTCH_MODE][N_NOTCH_MAX];		//�m�b�`�w�ߑ��x
		{0.0,0.33,0.1,0.2,0.33,0.33},
		{0.0,0.0,0.0,0.0,0.0,0.0},
		{0.0,0.0,0.0,0.0,0.0,0.0}
	},

	{//double Notch_spd_r[N_NOTCH_MODE][N_NOTCH_MAX];		//�m�b�`�w�ߑ��x
		{-0.0,-0.33,-0.1,-0.2,-0.33,-0.33},
		{0.0,0.0,0.0,0.0,0.0,0.0},
		{0.0,0.0,0.0,0.0,0.0,0.0}
	},
	{2000,5000,10000,25000,0x7FFF,0x7FFF},		//INT16 Notch_pad_f[N_NOTCH_MAX];	//GamePad�̃m�b�`�z��
	{-2000,-5000,-10000,-25000,0x8000,0x8000},	//INT16 Notch_pad_r[N_NOTCH_MAX];	//GamePad�̃m�b�`�z��
	0.33,				//double V_rated;��i���x�im/s, rad/s�F100��)
	1750.0,				// Rpm_rated;	��i��]���irpm�F100��)
	90.0,				// Kw_rated;	��i�o�́ikW�F100��)
	78.12,				// Gear_ratio;	������(���[�^��] /�h������] �j
	0.720,				// Ddrm0;		�h�����a0
	0.0,				// Ddrm1;		�h�����a1
	6.0,				// Nwire0;		���[�v�|��
	1.0,				// Nwire1;		���[�v�|��
	1.5,				// Ta0;			��������0
	0.0,				// Ta1;			��������1
	1.5,				// Td0;			��������0
	0.0,				// Td1;			��������1
	21.0,				// Ndmizo0;		�h�����a��0
	0.0,				// Ndmizo1;		�h�����a��1
	2048,				// CntPgR;		PG1��]�J�E���g���~���{
	86673,				// CntPgSet0;	PG�v���Z�b�g0�J�E���g�l
	0.0,				// CntPgSet1;	PG�v���Z�b�g1�J�E���g�l
	0.0,				// CntAbsR;		�A�u�\�R�[�_1��]�J�E���g���~���͎��M�A��
	0.0,				// CntAbsSet0;	�A�u�\�R�[�_�v���Z�b�g0�J�E���g�l
	0.0,				// CntAbsSet1;	�A�u�\�R�[�_�v���Z�b�g1�J�E���g�l
	0.04157,			// dDdrm;		�h����1�w�ǉ����a
	491.1067,			// Trq_rated;	��i�g���N�i���[�^���j
	0.0,				// Pwr_base;	��i�K�v�o��
	0.0,				// Trq_base;	��i�K�v�g���N
	4.885128,			// Ia;			�������[�����g
	348.50,				// Lfull;		�t���X�p���im�j
};
ST_AUTO_SPEC	CSpec::auto_bh;
ST_REMOTE_SPEC	CSpec::rmt_bh;

//����
ST_AXIS_ITEMS	CSpec::base_sl;
ST_AXIS_ITEMS	base_sl0 = {
	ID_SLEW,//INT32 Axis_id;
	{//double Notch_spd_f[N_NOTCH_MODE][N_NOTCH_MAX];		//�m�b�`�w�ߑ��x
		{0.0,0.00175,0.0175,0.035,0.035,0.035},
		{0.0,0.0013,0.013,0.026,0.026,0.026},
		{0.0,0.0,0.0,0.0,0.0,0.0}
	},
	{//double Notch_spd_r[N_NOTCH_MODE][N_NOTCH_MAX];		//�m�b�`�w�ߑ��x
		{0.0,-0.00175,-0.0175,-0.035,-0.035,-0.035},
		{0.0,-0.0013,-0.013,-0.026,-0.026,-0.026},
		{0.0,0.0,0.0,0.0,0.0,0.0}
	},
	{2000,5000,10000,25000,0x7FFF,0x7FFF},		//INT16 Notch_pad_f[N_NOTCH_MAX];						//GamePad�̃m�b�`�z��
	{-2000,-5000,-10000,-25000,0x8000,0x8000},	//INT16 Notch_pad_r[N_NOTCH_MAX];						//GamePad�̃m�b�`�z��

	0.0349,				//1/3rpm double V_rated;��i���x�im/s, rad/s�F100��)
	1800.0,				// Rpm_rated;	��i��]���irpm�F100��)
	33.0,				// Kw_rated;	��i�o�́ikW�F100��)
	479.4,				// Gear_ratio;	������(���[�^��] /�h������] �j
	0.33,				// Ddrm0;		�h�����a0
	0.0,				// Ddrm1;		�h�����a1
	1.0,				// Nwire0;		���[�v�|��
	1.0,				// Nwire1;		���[�v�|��
	12.0,				// Ta0;			��������0
	20.0,				// Ta1;			��������1
	12.0,				// Td0;			��������0
	20.0,				// Td1;			��������1
	1.0,				// Ndmizo0;		�h�����a��0
	1.0,				// Ndmizo1;		�h�����a��1
	2048,				// CntPgR;		PG1��]�J�E���g���~���{
	15000000,			// CntPgSet0;	PG�v���Z�b�g0�J�E���g�l
	20432689,			// CntPgSet1;	PG�v���Z�b�g1�J�E���g�l
	0.0,				// CntAbsR;		�A�u�\�R�[�_1��]�J�E���g���~���͎��M�A��
	0.0,				// CntAbsSet0;	�A�u�\�R�[�_PG�v���Z�b�g0�J�E���g�l
	0.0,				// CntAbsSet1;	�A�u�\�R�[�_PG�v���Z�b�g1�J�E���g�l
	0.0,				// dDdrm;		�h����1�w�ǉ��a
	175.0704,			// Trq_rated;	��i�g���N�i���[�^���j
	0.0,				// Pwr_base;	��i�K�v�o��
	0.0,				// Trq_base;	��i�K�v�g���N
	30.39636,			// Ia;			�������[�����g
	11.473,				// Lfull;		�t���X�p���im�j
};
ST_AXIS_ITEMS	base_sl1 = {
	ID_SLEW,//INT32 Axis_id;
	{//double Notch_spd_f[N_NOTCH_MODE][N_NOTCH_MAX];		//�m�b�`�w�ߑ��x
		{0.0,0.00175,0.0175,0.035,0.035,0.035},
		{0.0,0.0013,0.013,0.026,0.026,0.026},
		{0.0,0.0,0.0,0.0,0.0,0.0}
	},
	{//double Notch_spd_r[N_NOTCH_MODE][N_NOTCH_MAX];		//�m�b�`�w�ߑ��x
		{0.0,-0.00175,-0.0175,-0.035,-0.035,-0.035},
		{0.0,-0.0013,-0.013,-0.026,-0.026,-0.026},
		{0.0,0.0,0.0,0.0,0.0,0.0}
	},
	{2000,5000,10000,25000,0x7FFF,0x7FFF},		//INT16 Notch_pad_f[N_NOTCH_MAX];						//GamePad�̃m�b�`�z��
	{-2000,-5000,-10000,-25000,0x8000,0x8000},	//INT16 Notch_pad_r[N_NOTCH_MAX];						//GamePad�̃m�b�`�z��

	0.0349,				//1/3rpm double V_rated;��i���x�im/s, rad/s�F100��)
	1800.0,				// Rpm_rated;	��i��]���irpm�F100��)
	33.0,				// Kw_rated;	��i�o�́ikW�F100��)
	479.4,				// Gear_ratio;	������(���[�^��] /�h������] �j
	0.33,				// Ddrm0;		�h�����a0
	0.0,				// Ddrm1;		�h�����a1
	1.0,				// Nwire0;		���[�v�|��
	1.0,				// Nwire1;		���[�v�|��
	12.0,				// Ta0;			��������0
	20.0,				// Ta1;			��������1
	12.0,				// Td0;			��������0
	20.0,				// Td1;			��������1
	1.0,				// Ndmizo0;		�h�����a��0
	1.0,				// Ndmizo1;		�h�����a��1
	2048,				// CntPgR;		PG1��]�J�E���g���~���{
	15000000,			// CntPgSet0;	PG�v���Z�b�g0�J�E���g�l
	20432689,			// CntPgSet1;	PG�v���Z�b�g1�J�E���g�l
	0.0,				// CntAbsR;		�A�u�\�R�[�_1��]�J�E���g���~���͎��M�A��
	0.0,				// CntAbsSet0;	�A�u�\�R�[�_PG�v���Z�b�g0�J�E���g�l
	0.0,				// CntAbsSet1;	�A�u�\�R�[�_PG�v���Z�b�g1�J�E���g�l
	0.0,				// dDdrm;		�h����1�w�ǉ��a
	175.0704,			// Trq_rated;	��i�g���N�i���[�^���j
	0.0,				// Pwr_base;	��i�K�v�o��
	0.0,				// Trq_base;	��i�K�v�g���N
	30.39636,			// Ia;			�������[�����g
	11.473,				// Lfull;		�t���X�p���im�j
};
ST_AUTO_SPEC	CSpec::auto_sl;
ST_REMOTE_SPEC	CSpec::rmt_sl;

//���s
ST_AXIS_ITEMS	CSpec::base_gt;
ST_AXIS_ITEMS	base_gt0 = {
	ID_GANTRY,//INT32 Axis_id;
	{//double Notch_spd_f[N_NOTCH_MODE][N_NOTCH_MAX];		//�m�b�`�w�ߑ��x
		{0.0,0.058,0.175,0.350,0.580,0.580},
		{0.0,0.0,0.0,0.0,0.0,0.0},
		{0.0,0.0,0.0,0.0,0.0,0.0}
	},
	{//double Notch_spd_r[N_NOTCH_MODE][N_NOTCH_MAX];		//�m�b�`�w�ߑ��x
		{0.0,-0.058,-0.175,-0.350,-0.580,-0.580},
		{0.0,0.0,0.0,0.0,0.0,0.0},
		{0.0,0.0,0.0,0.0,0.0,0.0}
	},
	
	{2000,5000,10000,25000,0x7FFF,0x7FFF},		//INT16 Notch_pad_f[N_NOTCH_MAX];	//GamePad�̃m�b�`�z��
	{-2000,-5000,-10000,-25000,0x8000,0x8000},	//INT16 Notch_pad_r[N_NOTCH_MAX];	//GamePad�̃m�b�`�z��
	
	0.58,			//double V_rated;��i���x�im/s, rad/s�F100��)
	1700.0,			// Rpm_rated;	��i��]���irpm�F100��)
	44.0,			// Kw_rated;	��i�o�́ikW�F100��)
	88.0,			// Gear_ratio;	������(���[�^��] /�h������] �j
	0.5,			// Ddrm0;		�h�����a0
	0.0,			// Ddrm1;		�h�����a1
	1.0,			// Nwire0;		���[�v�|��
	1.0,			// Nwire1;		���[�v�|��
	9.0,			// Ta0;			��������0
	9.0,			// Ta1;			��������1
	9.0,			// Td0;			��������0
	5.0,			// Td1;			��������1
	1.0,			// Ndmizo0;		�h�����a��0
	0.0,			// Ndmizo1;		�h�����a��1
	0.0,			// CntPgR;		PG1��]�J�E���g���~���{
	0.0,			// CntPgSet0;	PG�v���Z�b�g0�J�E���g�l
	0.0,			// CntPgSet1;	PG�v���Z�b�g1�J�E���g�l
	0.0,			// CntAbsR;		�A�u�\�R�[�_1��]�J�E���g���~���͎��M�A��
	0.0,			// CntAbsSet0;	PG�v���Z�b�g0�J�E���g�l
	0.0,			// CntAbsSet1;	PG�v���Z�b�g1�J�E���g�l
	0.0,			// dDdrm;		�h����1�w�ǉ��a
	0.0,			// Trq_rated;	��i�g���N�i���[�^���j
	0.0,			// Pwr_base;	��i�K�v�o��
	0.0,			// Trq_base;	��i�K�v�g���N
	6.24755,		// Ia;			�������[�����g
	1000.0,			// Lfull;		�t���X�p���im�j

};
ST_AXIS_ITEMS	base_gt1 = {
	ID_GANTRY,//INT32 Axis_id;
	{//double Notch_spd_f[N_NOTCH_MODE][N_NOTCH_MAX];		//�m�b�`�w�ߑ��x
		{0.0,0.058,0.175,0.350,0.580,0.580},
		{0.0,0.0,0.0,0.0,0.0,0.0},
		{0.0,0.0,0.0,0.0,0.0,0.0}
	},
	{//double Notch_spd_r[N_NOTCH_MODE][N_NOTCH_MAX];		//�m�b�`�w�ߑ��x
		{0.0,-0.058,-0.175,-0.350,-0.580,-0.580},
		{0.0,0.0,0.0,0.0,0.0,0.0},
		{0.0,0.0,0.0,0.0,0.0,0.0}
	},

	{2000,5000,10000,25000,0x7FFF,0x7FFF},		//INT16 Notch_pad_f[N_NOTCH_MAX];	//GamePad�̃m�b�`�z��
	{-2000,-5000,-10000,-25000,0x8000,0x8000},	//INT16 Notch_pad_r[N_NOTCH_MAX];	//GamePad�̃m�b�`�z��

	0.58,			//double V_rated;��i���x�im/s, rad/s�F100��)
	1700.0,			// Rpm_rated;	��i��]���irpm�F100��)
	44.0,			// Kw_rated;	��i�o�́ikW�F100��)
	88.0,			// Gear_ratio;	������(���[�^��] /�h������] �j
	0.5,			// Ddrm0;		�h�����a0
	0.0,			// Ddrm1;		�h�����a1
	1.0,			// Nwire0;		���[�v�|��
	1.0,			// Nwire1;		���[�v�|��
	9.0,			// Ta0;			��������0
	9.0,			// Ta1;			��������1
	9.0,			// Td0;			��������0
	5.0,			// Td1;			��������1
	1.0,			// Ndmizo0;		�h�����a��0
	0.0,			// Ndmizo1;		�h�����a��1
	0.0,			// CntPgR;		PG1��]�J�E���g���~���{
	0.0,			// CntPgSet0;	PG�v���Z�b�g0�J�E���g�l
	0.0,			// CntPgSet1;	PG�v���Z�b�g1�J�E���g�l
	0.0,			// CntAbsR;		�A�u�\�R�[�_1��]�J�E���g���~���͎��M�A��
	0.0,			// CntAbsSet0;	PG�v���Z�b�g0�J�E���g�l
	0.0,			// CntAbsSet1;	PG�v���Z�b�g1�J�E���g�l
	0.0,			// dDdrm;		�h����1�w�ǉ��a
	0.0,			// Trq_rated;	��i�g���N�i���[�^���j
	0.0,			// Pwr_base;	��i�K�v�o��
	0.0,			// Trq_base;	��i�K�v�g���N
	6.24755,		// Ia;			�������[�����g
	1000.0,			// Lfull;		�t���X�p���im�j
};
ST_AUTO_SPEC	CSpec::auto_gt;
ST_REMOTE_SPEC	CSpec::rmt_gt;

//�⊪
ST_AXIS_ITEMS	CSpec::base_ah;
ST_AXIS_ITEMS	base_ah0 = {
	ID_AHOIST,//INT32 Axis_id;
};
ST_AXIS_ITEMS	base_ah1 = {
	ID_AHOIST,//INT32 Axis_id;
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
		memcpy_s(&st_struct, sizeof(ST_STRUCTURE), &st_struct1, sizeof(ST_STRUCTURE));
		memcpy_s(&base_mh, sizeof(ST_AXIS_ITEMS), &base_mh1, sizeof(ST_AXIS_ITEMS));
		memcpy_s(&base_bh, sizeof(ST_AXIS_ITEMS), &base_bh1, sizeof(ST_AXIS_ITEMS));
		memcpy_s(&base_sl, sizeof(ST_AXIS_ITEMS), &base_sl1, sizeof(ST_AXIS_ITEMS));
		memcpy_s(&base_gt, sizeof(ST_AXIS_ITEMS), &base_gt1, sizeof(ST_AXIS_ITEMS));
		memcpy_s(&base_ah, sizeof(ST_AXIS_ITEMS), &base_ah1, sizeof(ST_AXIS_ITEMS));
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

