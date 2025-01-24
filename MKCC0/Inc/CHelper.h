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

#define Bitcheck(a,b)	(a >> b) & 1
#define Bitset(a,b)		a |= (1<<b)
#define Bitclear(a,b)		a &= ~(1<<b)

using namespace std;

class CHelper
{
public:
	CHelper();
	~CHelper();
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
