#include "CHelper.h"
#include <locale.h>

CHelper::CHelper() {}

CHelper::~CHelper() {}

/*****************************************************************************
string�^��wstring�^�ɕϊ����܂�
- �����@const std::string &src�F�ϊ��O���́@, std::wstring &dest�F�ϊ���o�́@
- �߂�l�@NA
*****************************************************************************/
void CHelper::Str2Wstr(const std::string& src, std::wstring& dest) {

	setlocale(LC_ALL, "");

	wchar_t* wcs = new wchar_t[src.length() + 1];

	size_t ret_val;
	mbstowcs_s(&ret_val, wcs, src.length() + 1, src.c_str(), _TRUNCATE);

	dest = wcs;delete[] wcs;
	return;
}

/*****************************************************************************************
�w�肳�ꂽDC�A�ʒu�Ɏw�肳�ꂽ�r�b�g�}�b�v���������ށi���{�j
- ����	HDC hdc�F������DC, HBITMAP hbmp�F�f�ރr�b�g�}�b�v,
		POINT dst_pt�F������̈ʒu, POINT src_pt�F�������̈ʒu
- �߂�l�@NA
******************************************************************************************/
void CHelper::put_bmp_built(HDC hdc, HBITMAP hbmp, POINT dst_pt, POINT src_pt) {

	if ((hdc == NULL) || (hbmp == NULL)) return;

	BITMAP bmp;
	GetObject(hbmp, sizeof(BITMAP), &bmp);
	int BMP_W = (int)bmp.bmWidth;
	int BMP_H = (int)bmp.bmHeight;

	HDC hmdc = CreateCompatibleDC(hdc);
	SelectObject(hmdc, hbmp);

	BitBlt(hdc, dst_pt.x, dst_pt.y, BMP_W, BMP_H, hmdc, src_pt.x, src_pt.y, SRCCOPY);
	DeleteDC(hmdc);

	return;
};

/******************************************************************************************************
�w�肳�ꂽDC�A�ʒu�Ɏw�肳�ꂽ�r�b�g�}�b�v���������ށi�ϔ{�j
- ����	HDC hdc�F������DC, HBITMAP hbmp�F�f�ރr�b�g�}�b�v,
		POINT dst_pt�F������̈ʒu, POINT src_pt�F�������̈ʒu,int retio_percent:������{��
- �߂�l	NA
******************************************************************************************************/
void CHelper::put_bmp_stretch(HDC hdc, HBITMAP hbmp, POINT dst_pt, POINT src_pt, int retio_percent) {

	if ((hdc == NULL) || (hbmp == NULL)) return;

	BITMAP bmp;
	GetObject(hbmp, sizeof(BITMAP), &bmp);
	int BMP_W = (int)bmp.bmWidth;
	int BMP_H = (int)bmp.bmHeight;

	HDC hmdc = CreateCompatibleDC(hdc);
	SelectObject(hmdc, hbmp);

	StretchBlt(hdc, dst_pt.x, dst_pt.y, BMP_W * retio_percent / 100, BMP_H * retio_percent / 100, hmdc, src_pt.x, src_pt.y, BMP_W, BMP_H, SRCCOPY);
	DeleteDC(hmdc);

	return;
};

/****************************************************************************************************************************************************
������bit�����J�E���g����
- �����@�J�E���g���鐮��
- �߂�l�@bit��
****************************************************************************************************************************************************/
UINT	CHelper::bits_count(UINT16 x) { int ret = 0; while (x) { x &= x - 1; ret++; } return ret; }
UINT	CHelper::bits_count(UINT32 x) { int ret = 0; while (x) { x &= x - 1; ret++; } return ret; }
UINT	CHelper::bits_count(UINT64 x) { int ret = 0; while (x) { x &= x - 1; ret++; } return ret; }

/*****************************************************************************
�p�x�f�[�^�����̊p�x�͈�(-180�x�`�{180�x�ɍZ������
- �����@double* �@
- �߂�l�@NA
*****************************************************************************/
void CHelper::fit_ph_range_upto_pi(double* pd) {

	int n;
	double pi2 = 6.2832;
	double pi = 3.1416;

	if (*pd > 0.0) {
		n = (int)(*pd / pi2);		// ��2��
		*pd -= (double)n * pi2;
		if(*pd > pi) *pd -= pi2; //-�΁`�΂̃����W�֕ϊ�
	}
	else {
		n = (int)(*pd / -pi2);
		*pd += (double)n * pi2;
		if (*pd < -pi) *pd += pi2;
	}

	return;
}
