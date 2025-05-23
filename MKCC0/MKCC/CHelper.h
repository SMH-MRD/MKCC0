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
#include "params.h"


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
	CPhaseHelper() {};
	~CPhaseHelper() {};

	static void fit_ph_range_upto_pi(double* th); //�@�ʑ����W���}�΂̃����W�ɍZ������

};