#include "framework.h"
#include "CPanelIo.h"
#include <gdiplus.h>

CSwitchImg::CSwitchImg(
	INT32 _id, Point* ppt, Size* psz, LPWSTR ptext,
	Image** _ppimg, INT32 _n_switch, INT32 _fcount, Gdiplus::Graphics* _pgraphic
)
{
	set_id(_id);
	set_base(ppt, psz, ptext);
	for (int i = 0; i < N_IMG_SWITCH_MAX; i++) {
		if (i < _n_switch) pimg[i] = _ppimg[i];
		else pimg[i] = NULL;
	}
	n_switch = _n_switch;
	fcount = _fcount;

	Rect _rc(pt.X, pt.Y, sz.Width, sz.Height); rc = _rc;
	rc_clear = { rc.X- rc.Width/2 , rc.Y - rc.Height / 2, rc.Width*2, rc.Height*2}; //�w�i�F�Z�b�g�p�̋�`
	RectF _frc((REAL)pt.X, (REAL)pt.Y, (REAL)sz.Width, (REAL)sz.Height); frc = _frc;

	rect = { pt.X,pt.Y,pt.X + sz.Width,pt.Y + sz.Height };

	pgraphics = _pgraphic;

	value = ID_PANEL_LAMP_OFF;
}
CSwitchImg:: ~CSwitchImg() {
	for (int i = 0; i < N_IMG_SWITCH_MAX; i++) {
		delete pimg[i];
	}
}


HRESULT CSwitchImg::setup_flick(INT32 _n_flick, INT32 _fcount, INT32* pid) {
	fcount = _fcount;
	n_flick = _n_flick;
	for (int i = 0; i < n_flick; i++) {//�t���b�J������摜�̏�����荞��
		if (i < N_IMG_SWITCH_MAX) list_flick_id[i] = pid[i];
	}
	return S_OK;
}

/// <summary>
/// �摜�\�����X�V���A�p�l�������v�̏�Ԃɉ����ĕ`�揈�����s���܂��B
/// </summary>
/// <returns>����������Ɋ��������ꍇ�� S_OK�A�����Ȓl�̏ꍇ�� S_FALSE ��Ԃ��܂��B</returns>

static SolidBrush blackBrush(Color::Black);

HRESULT CSwitchImg::update() {
	INT16 id = 0;
	if (value != ID_PANEL_LAMP_FLICK) {
		if (value < 0) return S_FALSE;			//-1�͖����l
		if (value >= n_switch) return S_FALSE;	//n_switch�ȏ�͖����l
		id_disp = value;
		pgraphics->DrawImage(pimg[id_disp], rc);
		InvalidateRect(hWnd, &rect, FALSE);
		return S_OK;
	}
	else {
		count++; if (count > fcount) {
			count = 0;
			id_disp++; if (id_disp >= n_flick) id_disp = 0;
			pgraphics->DrawImage(pimg[list_flick_id[id_disp]], rc);
			InvalidateRect(hWnd, &rect, FALSE);
		}
		return S_OK;
	}
	return S_FALSE;
}

/// <summary>
/// �摜�̒��S���w�肵�\���w��ʒu�ɍ��킹�ĉ摜��\�����܂��B
/// </summary>
/// <param name="x_center">���S��X���W�B</param>
/// <param name="y_center">���S��Y���W�B</param>
/// <returns>���������������ꍇ�� S_OK ��Ԃ��܂��B</returns>
HRESULT CSwitchImg::update(int x, int y) {
	if (value < 0) return S_FALSE;			//-1�͖����l
	if (value >= n_switch) return S_FALSE;	//n_switch�ȏ�͖����l
	id_disp = value;

	Rect dest_rc(x,	y,rc.Width,rc.Height);

	pgraphics->FillRectangle(&blackBrush, rc_clear); // �w�i�F�Z�b�g
	pgraphics->DrawImage(pimg[id_disp], dest_rc);
}

/// <summary>
/// �摜�̒��S���w�肵�A�X�ɃX�P�[���A��]�p�x���w�肵�ĕ\�����s���܂��B
/// �摜�̒��S�̓X�P�[���ɉ����ĕ␳���A�\���w��ʒu�ɍ��킹�ĕ\�����܂��B
/// </summary>
/// <param name="x_center">�摜�̒��S��X���W�B</param>
/// <param name="y_center">�摜�̒��S��Y���W�B</param>
/// <param name="scale_x">X�����̃X�P�[���l�i�g��,�k�����j</param>
/// <param name="scale_y">Y�����̃X�P�[���l�i�g��,�k�����j</param>
/// <param name="angle">��]�p�x�i�x�j�B</param>
/// <returns>���삪���������ꍇ�� S_OK ��Ԃ��܂��B</returns>
HRESULT CSwitchImg::update(int x_center, int y_center, float w_retio, float h_retio, float angle) {
	if (value < 0) return S_FALSE;			//-1�͖����l
	if (value >= n_switch) return S_FALSE;	//n_switch�ȏ�͖����l
	id_disp = value;

	// 1.�O���t�B�b�N�I�u�W�F�N�g���݂̏�Ԃ�ۑ�
	Gdiplus::GraphicsState state = pgraphics->Save();

	pgraphics->TranslateTransform(rc.X+x_center, rc.Y+y_center);
	//pgraphics->TranslateTransform(670, 475);
	pgraphics->RotateTransform(angle);

	Rect rc_dest(-x_center,	-y_center, rc.Width*w_retio,rc.Height*h_retio);
	

	//�`��
//	pgraphics->FillRectangle(&blackBrush, rc_clear); // �w�i�F�Z�b�g
	pgraphics->DrawImage(pimg[id_disp], rc_dest);

	pgraphics->ResetTransform();

	return S_OK;
}