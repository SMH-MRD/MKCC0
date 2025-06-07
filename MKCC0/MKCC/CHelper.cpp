#include "CHelper.h"
#include <locale.h>
#include "COMMON_DEF.h"

/*****************************************************************************
NotchHelper
*****************************************************************************/
ST_NOTCH_CODE CNotchHelper::code;
INT16 CNotchHelper::get_code4_by_notch(int notch,int bit_shift) {
	INT16 ans;
	if (notch == 0) ans = PTN_NOTCH4_0;
	else if ((notch < 0) && (notch >=-4)) ans = code.r4[-notch];
	else if ((notch > 0) && (notch <= 4)) ans = code.f4[notch];
	else ans = PTN_NOTCH4_0;
	
	if (bit_shift) ans <<= bit_shift;
	return ans;
}

INT16 CNotchHelper::set_code4_by_notch(PINT16 pdst, int notch, int bit_shift) {
	INT16 setval= get_code4_by_notch(notch, bit_shift);
	INT16 set_mask = PTN_NOTCH4_MASK;

	if (bit_shift) 	set_mask <<= bit_shift;
	*pdst &= ~set_mask;
	*pdst |= setval;
	return setval;
}

//code�w��Ńm�b�`�擾
INT16 CNotchHelper::get_notch4_by_code(PINT16 pcodebuf, int bit_shift) {
	INT16 ans,chk_code;
	INT16 chk_mask = PTN_NOTCH4_MASK; 

	chk_mask <<= bit_shift;
	chk_code = *pcodebuf & chk_mask;
	chk_code >>= bit_shift;

	if (chk_code == code.f4[0])return 0;
	for (int i = 1; i < N_NOTCH_MAX; i++) {
		if (chk_code == code.f4[i])return i;
		if (chk_code == code.r4[i])return -i;
	}
	return 0;
}

//���x�w��Ńm�b�`�擾
INT16 CNotchHelper::get_notch4_by_v(double v, double* vtbl_f, double* vtbl_r) {
	INT16 ans, i;
	double* pd;
	if (v == 0.0) return 0;
	else if (v < 0.0) {
		for (i = 1; i < N_NOTCH_MAX; i++) {
			pd = vtbl_r + i;
			if (v > *pd) {
				if (v > (*pd + *(pd - 1)) / 2.0) i--;//�O�̃e�[�u���l�Ƃ̕��ψȏ�Ȃ�O�̃m�b�`
				break;
			}
		}
		if (i > 4)	i = 4;
		return -i;
	}
	else if (v > 0.0) {
		for (i = 1; i < N_NOTCH_MAX; i++) {
			pd = vtbl_f + i;
			if (v < *pd) {
				if (v < (*pd + *(pd - 1)) / 2.0) i--;//�O�̃e�[�u���l�Ƃ̕��ψȉ��Ȃ�O�̃m�b�`
				break;
			}
		}
		if (i >4)	i = 4;
		return i;
	}
	else return 0;

	return 0;
}

//PAD�̃A�i���O�o�͒l����m�b�`�擾 0x8000�`0x7FFF�F
INT16 CNotchHelper::get_notch_by_pad(INT16 val, PINT16 vtbl_f,PINT16 vtbl_r) {
	INT16 ans=0, i=0;
	INT16 val0 = 0;
	PINT16 p;

	if (val == val0) return 0;
	else if (val < val0) {
		p = vtbl_r;
		for (i = 1; i < N_NOTCH_MAX; i++) {
			INT16 _val = *(p + i);
			if (val >= _val) break;
		}
		if (i >= N_NOTCH_MAX)	i = N_NOTCH_MAX - 1;
		return -i;
	}
	else if (val > val0) {
		p = vtbl_f;
		for (i = 1; i < N_NOTCH_MAX; i++) {
			INT16 _val = *(p + i);
			if (val <= _val)break;
		}
		if (i >= N_NOTCH_MAX)	i = N_NOTCH_MAX - 1;
		return i;
	}
	else return 0;
	return 0;
}

INT16 CNotchHelper::get_code4_by_v(double v, double* vtbl_f, double* vtbl_r, int bit_shift) {
	INT16 ans;
	INT16 notch = get_notch4_by_v(v, vtbl_f, vtbl_r);
	return get_code4_by_notch(notch, bit_shift);
}
INT16 CNotchHelper::set_code4_by_v(PINT16 pdst, double v, double* vtbl_f, double* vtbl_r, int bit_shift) {
	INT16 ans;
	INT16 notch = get_notch4_by_v(v, vtbl_f, vtbl_r);
	return set_code4_by_notch(pdst, notch, bit_shift);
}

//UI�p�z��p�C���f�N�X�擾
INT16 CNotchHelper::get_iui4_by_notch(int notch) {
	INT16 ans=0;
	if (notch == 0) return ans;
	if (notch < 0) ans = N_NOTCH_MAX - notch;
	if (notch > 0) ans = notch;
	if (ans >= N_NOTCH_TBL) ans = 0;
	return ans;
}
INT16 CNotchHelper::get_iui4_by_v(double v, double* vtbl_f, double* vtbl_r) {
	return get_iui4_by_notch(get_notch4_by_v(v, vtbl_f, vtbl_r));
}


/*****************************************************************************
StrHelper
*****************************************************************************/
void CStrHelper::Str2Wstr(const std::string& src, std::wstring& dest) {

	setlocale(LC_ALL, "");

	wchar_t* wcs = new wchar_t[src.length() + 1];

	size_t ret_val;
	mbstowcs_s(&ret_val, wcs, src.length() + 1, src.c_str(), _TRUNCATE);

	dest = wcs;delete[] wcs;
	return;
}

/*****************************************************************************************
BmpHelper
******************************************************************************************/
void CBmpHelper::put_bmp_built(HDC hdc, HBITMAP hbmp, POINT dst_pt, POINT src_pt) {

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
void CBmpHelper::put_bmp_stretch(HDC hdc, HBITMAP hbmp, POINT dst_pt, POINT src_pt, int retio_percent) {

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
BiHelper
****************************************************************************************************************************************************/
UINT	CBitHelper::bits_count(UINT16 x) { int ret = 0; while (x) { x &= x - 1; ret++; } return ret; }
UINT	CBitHelper::bits_count(UINT32 x) { int ret = 0; while (x) { x &= x - 1; ret++; } return ret; }
UINT	CBitHelper::bits_count(UINT64 x) { int ret = 0; while (x) { x &= x - 1; ret++; } return ret; }

/*****************************************************************************
PhaseHelper
*****************************************************************************/
void CPhaseHelper::fit_ph_range_upto_pi(double* pd) {

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

/*****************************************************************************
PlcCSHelper
*****************************************************************************/

INT16 CPlcCSHelper::get_mode_by_code(INT16 value, int cs_code, int crane_id) {
	switch (cs_code) {
	case PLC_IO_CS_MH_SPD_MODE: {
		if (value & PLC_IO_CS_MH_SPD_MODE0) return 0;
		if (value & PLC_IO_CS_MH_SPD_MODE1) return 1;
		if (value & PLC_IO_CS_MH_SPD_MODE2) return 2;
	}break;
	case PLC_IO_CS_BH_R_MODE: {
		if (value & PLC_IO_CS_BH_R_MODE0) return 0;
		if (value & PLC_IO_CS_BH_R_MODE1) return 1;
		if (value & PLC_IO_CS_BH_R_MODE2) return 2;
	}break;
	default:
		break;
	}
	return 0;
}
INT16 CPlcCSHelper::get_code_by_mode(INT16 mode, int cs_code, int crane_id) {
	switch (cs_code) {
	case PLC_IO_CS_MH_SPD_MODE: {
		if (mode == 0) return PLC_IO_CS_MH_SPD_MODE0;
		if (mode == 1) return PLC_IO_CS_MH_SPD_MODE1;
		if (mode == 2) return PLC_IO_CS_MH_SPD_MODE2;
	}break;
	case PLC_IO_CS_BH_R_MODE: {
		if (mode == 0) return PLC_IO_CS_BH_R_MODE0;
		if (mode == 1) return PLC_IO_CS_BH_R_MODE1;
		if (mode == 2) return PLC_IO_CS_BH_R_MODE2;
	}break;
	default:
		break;
	}
	return 0;
}

