#pragma once

#include <windows.h>
// C �����^�C�� �w�b�_�[ �t�@�C��
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include <string>
#include <time.h>
#include <vector>
#include <mmsystem.h>
#include "COMMON_DEF.h"

//�m�b�`�r�b�g�p�^�[��

#define PTN_NOTCH4_MASK		0x003F
#define PTN_NOTCH4_CLR		0xFFC0
#define PTN_NOTCH4_0		0x0001
#define PTN_NOTCH4_F1		0x0002
#define PTN_NOTCH4_F2		0x000A
#define PTN_NOTCH4_F3		0x001A
#define PTN_NOTCH4_F4		0x003A
#define PTN_NOTCH4_R1		0x0004
#define PTN_NOTCH4_R2		0x000C
#define PTN_NOTCH4_R3		0x001C
#define PTN_NOTCH4_R4		0x003C

//PLC�̃m�b�`�R�[�h
struct ST_NOTCH_CODE {
	INT16 f4[N_NOTCH_MAX] = { PTN_NOTCH4_0,PTN_NOTCH4_F1,PTN_NOTCH4_F2,PTN_NOTCH4_F3,PTN_NOTCH4_F4,PTN_NOTCH4_F4 };
	INT16 r4[N_NOTCH_MAX] = { PTN_NOTCH4_0,PTN_NOTCH4_R1,PTN_NOTCH4_R2,PTN_NOTCH4_R3,PTN_NOTCH4_R4,PTN_NOTCH4_R4 };
};

using namespace std;
class CNotchHelper
{
public:
	CNotchHelper() {};
	~CNotchHelper() {};
	static ST_NOTCH_CODE code;

	//�m�b�`�w���PLC IO�p�R�[�h�̎擾,�o��
	static INT16 get_code4_by_notch(int notch, int bit_shift);
	static INT16 set_code4_by_notch(PINT16 pdst,int notch,int bit_shift);
	//���x�w���PLC IO�p�R�[�h�̎擾,�o��
	static INT16 get_code4_by_v(double v, double* vtbl_f, double* vtbl_r, int bit_shift);
	static INT16 set_code4_by_v(PINT16 pdst, double v, double* vtbl_f, double* vtbl_r, int bit_shift);

	//code�w��Ńm�b�`�擾
	static INT16 get_notch4_by_code(PINT16 pbuf, int bit_shift);
	//���x�w��Ńm�b�`�擾
	static INT16 get_notch4_by_v(double v, double* vtbl_f, double* vtbl_r);
	static INT16 get_notch_by_pad(INT16 val, PINT16 vtbl_f, PINT16 vtbl_r);

	//UI�z��p�C���f�N�X�擾
	static INT16 get_iui4_by_notch(int notch);
	static INT16 get_iui4_by_v(double v, double* vtbl_f, double* vtbl_r);
};
class CStrHelper
{
public:
	CStrHelper() {};
	~CStrHelper() {};
	//������
	static void Str2Wstr(const string& src, wstring& dest);									//string��wstring�ɕϊ�
	//Bitmap
	static void put_bmp_built(HDC hdc, HBITMAP hbmp, POINT dst_pt, POINT src_pt);						//�r�b�g�}�b�v�\��
	static void put_bmp_stretch(HDC hdc, HBITMAP hbmp, POINT dst_pt, POINT src_pt, int retio_persent);  //�r�b�g�}�b�v�g��k���\��

	//�r�b�g���J�E���g
	static UINT	bits_count(UINT16 x);
	static UINT	bits_count(UINT32 x);
	static UINT	bits_count(UINT64 x);
	static void fit_ph_range_upto_pi(double* th); //�@�ʑ����W���}�΂̃����W�ɍZ������

};
class CBitHelper
{
public:
	CBitHelper() {};
	~CBitHelper() {};
	//�r�b�g���J�E���g
	static UINT	bits_count(UINT16 x);
	static UINT	bits_count(UINT32 x);
	static UINT	bits_count(UINT64 x);
};
class CBmpHelper
{
public:
	CBmpHelper() {};
	~CBmpHelper() {};
	//Bitmap
	static void put_bmp_built(HDC hdc, HBITMAP hbmp, POINT dst_pt, POINT src_pt);						//�r�b�g�}�b�v�\��
	static void put_bmp_stretch(HDC hdc, HBITMAP hbmp, POINT dst_pt, POINT src_pt, int retio_persent);  //�r�b�g�}�b�v�g��k���\��

};
class CPhaseHelper
{
public:
	static void fit_ph_range_upto_pi(double* th); //�@�ʑ����W���}�΂̃����W�ɍZ������
};

#define PLC_IO_CS_MH_SPD_MODE	20
#define PLC_IO_CS_BH_R_MODE		21

#define PLC_IO_CS_MH_SPD_MODE0 BIT0
#define PLC_IO_CS_MH_SPD_MODE1 BIT1
#define PLC_IO_CS_MH_SPD_MODE2 BIT5
#define PLC_IO_CS_BH_R_MODE0   BIT0
#define PLC_IO_CS_BH_R_MODE1   BIT1
#define PLC_IO_CS_BH_R_MODE2   BIT2

class CPlcCSHelper
{
public:
	static INT16 get_mode_by_code(INT16 value,int cs_code, int crane_id);
	static INT16 get_code_by_mode(INT16 mode, int cs_code, int crane_id);
};