#pragma once
#include <Windows.h>
#include <string>
#include "COMMON_DEF.H"


//######################################################################
// Value
//######################################################################

#define ID_PANEL_ACTION_NON	0

#define ID_PANEL_POINT_DST		0
#define ID_PANEL_POINT_SRC_ON	1
#define ID_PANEL_POINT_SRC_OFF	2

#define VAL_PB_OFF_DELAY_COUNT	0x000A	//�{�^��OFF�f�B���C�J�E���g�l

#define VAL_TRIGGER_NON			0		//�g���K����
#define VAL_TRIGGER_VALUE		3		//���l�ύX�g���K
#define VAL_TRIGGER_FLICK		2		//�t���b�J
#define VAL_TRIGGER_ON			1		//ON�g���K
#define VAL_TRIGGER_OFF			-1		//OFF�g���K

#define N_VAL_RADIO_MAX			10		//RADIO PB�ő吔

template <class T> class CPnlParts {
public:
	CPnlParts() {}
	virtual ~CPnlParts() {}

public:
    T value;	    //���ݒl
 
	Point pt= { 0,0 };			//�\���ʒu
	Size sz	= { 0,0 };			//�\���T�C�Y
	std::wstring txt;

	HWND hWnd			= NULL;	//�E�B���h�E�n���h��
	INT32 id			= 0;	//���j���[ID

	Rect rc;					//�\���̈�int
	RectF frc;					//�\���̈�float

public:

	HRESULT set_base(Point* ppt, Size* psz, LPCWSTR ptext) {
		pt = *ppt; sz = *psz; std::wstring _txt = ptext; txt = _txt; return S_OK;
	};


	virtual T get() { return value; }
	virtual HRESULT set(T _value) { value = _value;  return S_OK; }
	virtual HRESULT set_wnd(HWND _hWnd) { if (_hWnd == NULL) return S_FALSE;  hWnd = _hWnd; return S_OK; }
	
	INT32 get_id() { return id; }
	HRESULT set_id(INT32 _id) { id = _id; return S_OK; }

	virtual int chk_trig(T _value) {
		int chk = VAL_TRIGGER_NON;
		if (_value > value)	chk = VAL_TRIGGER_ON;	//ON�g���K
		if (_value < value)	chk = VAL_TRIGGER_OFF;	//OFF�g���K
		set(_value);
		return chk;
	}
};

/// <summary>
/// STATIC �R���g���[��
/// </summary>
class CStaticCtrl : public CPnlParts<INT16> {	//�X�^�e�B�b�N

public:
	CStaticCtrl(INT32 _id, Point* ppt, Size* psz, LPCWSTR ptext) {
		set_id(_id);
		set_base(ppt, psz, ptext);
	}
	virtual ~CStaticCtrl() {}
	int  update(LPCWSTR ptext) {				//�X�V 
		return SetWindowText(hWnd,ptext); //�߂�l�̓J�E���g�l
	}
};

/// <summary>
/// PB �R���g���[��
/// �{�^���̃g���K�ŃJ�E���g���Z�b�g��update���\�b�h�������Ăяo���ŃJ�E���g�_�E��
/// </summary>
class CPbCtrl : public CPnlParts<INT16> {	//�{�^��

public:
	CPbCtrl( INT32 _id, Point* ppt, Size* psz, LPCWSTR ptext,Graphics* _pgraphics,Pen* ppenON,Pen* ppenOFF) { 
		set_id(_id) ;
		set_base(ppt, psz, ptext);
		rc = { pt.X,pt.Y,sz.Width,sz.Height };
		frc = { (REAL)rc.X,(REAL)rc.X,(REAL)rc.Width,(REAL)rc.Height };
		pPenOn = ppenON; pPenOff = ppenOFF;
		pgraphics = _pgraphics;
	}
	virtual ~CPbCtrl() {}

	Pen* pPenOn, *pPenOff;
	INT16 status = 0;
	Graphics* pgraphics;

	INT16 count_off_set_val = VAL_PB_OFF_DELAY_COUNT;	//�I�t�f�B���C�J�E���g�l
	INT16 update(bool is_trigger) {				//�X�V 
		if (is_trigger) set(count_off_set_val); //�g���K�[ON�Ńf�B���C�J�E���g�l�Z�b�g
		else if (value > 0) value--;			//�g���K�[�����ŃJ�E���g�_�E��
		else;

		disp_status();

		return get(); //�߂�l�̓J�E���g�l
	}

	virtual int chk_trig() {
		if (value != status) {						//status=�J�E���g�_�E���O��value�̃z�[���h�l���O��l�ƈقȂ�
			if ((value == 0) || (status == 0)) {	//status�@or�@value��0�˗����オ��܂��͗�����
				status = value; return L_ON;		//status���X�V����Triger ON
			}
		}
		return L_OFF;
	}
	HRESULT disp_status() { //�g����\�����������Ȃ��Ƃ���PEN��ON��OFF��PEN�ɂ���
		if (chk_trig()) {	//�g���K�̃^�C�~���Ovalue��0�ˁI0�@!0��0�Řg���`��@value�̒l��pen��ς���
			Pen* ppen = pPenOff;
			if (value != 0) ppen = pPenOn;
			pgraphics->DrawRectangle(ppen, rc);
		}
		return S_OK;
	}	

	//�{�^���I�t�f�B���C�̃J�E���g�l�X�V
	void set_off_delay(INT16 val_delay) { count_off_set_val = val_delay; return; }//

	bool is_on() { //ON��Ԃ��H
		if (value > 0) return true;	
		else return false;
	}
};

/// <summary>
/// CECK BOX �R���g���[��
/// </summary>
class CCbCtrl : public CPnlParts<INT16> {	//�{�^��

public:
	CCbCtrl(INT32 _id,Point* ppt, Size* psz, LPWSTR ptext, Graphics* _pgraphics, Pen* ppenON, Pen* ppenOFF)
	{
		set_id(_id); 
		set_base(ppt, psz, ptext);
		value = BST_UNCHECKED;
		rc = { pt.X,pt.Y,sz.Width,sz.Height };
		frc = { (REAL)rc.X,(REAL)rc.X,(REAL)rc.Width,(REAL)rc.Height };
		pPenOn = ppenON; pPenOff = ppenOFF;
		//�I�[�i�[�h���[�̎��A�O���t�B�b�N�X�̃|�C���^��NULL
		if (_pgraphics == NULL) {
			is_owner_draw = true;
		}
		else {
			pgraphics = _pgraphics;
		}
	}
	virtual ~CCbCtrl() {}

	Pen* pPenOn, * pPenOff;
	INT16 status = 0;
	Graphics* pgraphics;
	BOOL is_owner_draw = false;

	//�R���g���[���̏�Ԃ�get,set
	virtual INT16 get() { 
		if (!is_owner_draw) {
			if (BST_CHECKED == SendMessage(hWnd, BM_GETCHECK, 0, 0)) {
				value = BST_CHECKED;
			}
			else {
				value = BST_UNCHECKED;
			}
		}
		return value;
	}

	virtual HRESULT set(INT16 val) { 
		if (is_owner_draw)			value = val;
		else if (val==BST_CHECKED)	SendMessage(hWnd, BM_SETCHECK, BST_CHECKED, 0); 
		else						SendMessage(hWnd, BM_SETCHECK, BST_UNCHECKED, 0);
		get();
		return S_OK; 
	};

	INT16 update() {				//�X�V 
		if (!is_owner_draw) {//�I�[�i�[�h���[�^�C�v�łȂ����ON/OFF�g�\��
			Pen* ppen = pPenOff;
			if (value != 0) ppen = pPenOn;
			pgraphics->DrawRectangle(ppen, rc);
		}
		return get(); //�߂�l�̓J�E���g�l
	}
	//INT16 update(INT16 signal) {	//�X�V 
	//	if (!is_owner_draw) {//�I�[�i�[�h���[�^�C�v�łȂ����ON/OFF�g�\��
	//		Pen* ppen = pPenOff;
	//		if (value != 0) ppen = pPenOn;
	//		pgraphics->DrawRectangle(ppen, rc);
	//	}
	//	else {				//�I�[�i�[�h���[�ŐM���\���ɉ����Ęg���\��
	//		if (signal) pgraphics->DrawRectangle(pPenOn, rc);
	//		else		pgraphics->DrawRectangle(pPenOff, rc);
	//	}
	//	return signal;
	//}
};

/// <summary>
/// RADIO �R���g���[��
/// </summary>
class CRadioCtrl : public CPnlParts<INT16> {	//�{�^��

private:
	INT16 n_radio = 0;	//���W�I�{�^���̐�

public:

	CRadioCtrl(int _n_radio, CCbCtrl** ppcb) {
		
		if (n_radio > N_VAL_RADIO_MAX) n_radio = N_VAL_RADIO_MAX;
		else if (_n_radio < 0)			n_radio = 0;
		else							n_radio = _n_radio;
		if(S_OK == set_radio(ppcb, n_radio))
			pradio[0]->set(BST_CHECKED);
	}
	virtual ~CRadioCtrl() {}

	CCbCtrl* pradio[N_VAL_RADIO_MAX] = {					//�`�F�b�N�{�^���^�̃|�C���^�z��
		NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL
	};

	HRESULT set_radio(CCbCtrl** ppcb, int n_radio) {
		if ((n_radio > 0) && (n_radio < N_VAL_RADIO_MAX)) {
			for (int i = 0; i < n_radio; i++) {
				pradio[i] = ppcb[i];
			}
		}
		else {
			return S_FALSE;
		}

		return S_OK;
	}

	INT16 update(bool is_non_owner_draw) {				//�X�V 
		INT16 val = check();
		if (is_non_owner_draw) {//�I�[�i�[�h���[�^�C�v�łȂ����ON/OFF�g�\��
			for (int i = 0; i < n_radio; i++) {
				pradio[i]->update();
			}
		}
		return val; //�߂�l��ID�l
	}

	INT16 check() { 
		for (int i = 0; i < n_radio; i++) {
			if (pradio[i]->get() == BST_CHECKED) {
				value = i;
			}
		}
		return -1;
	}

	virtual HRESULT set(INT16 val) {
		if ((val < 0) || (val >= n_radio)) return S_FALSE;
		pradio[val]->set(L_ON);	//Value�̃Z�b�g
		value = val;
		return S_OK;
	};

};

#define ID_PANEL_LAMP_OFF			0x00
#define ID_PANEL_LAMP_ON			0x01
#define ID_PANEL_LAMP_FLICK			0xFF

/// <summary>
/// ImageLamp
/// �w��摜��ؑ֕\��
/// Image��ǂݍ���ŃO���t�B�b�N�ɏ�������
/// �o�^1�Ԗڂ̉摜��OFF�A2�Ԗڂ̉摜��ON�ŗ\��
/// </summary>
#define N_IMG_SWITCH_MAX			8

class CSwitchImg : public CPnlParts<INT16> {
public:
	CSwitchImg(
		INT32 _id, Point* ppt, Size* psz, LPWSTR ptext,
		Image** _ppimg, INT32 _n_switch,INT32 _fcount,Graphics* _pgraphic
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
		RectF _frc((REAL)pt.X, (REAL)pt.Y, (REAL)sz.Width, (REAL)sz.Height); frc = _frc;

		rect = { pt.X,pt.Y,pt.X + sz.Width,pt.Y + sz.Height };

		pgraphic = _pgraphic;
		value = ID_PANEL_LAMP_OFF;
	}
	virtual ~CSwitchImg() {
		for (int i = 0; i < N_IMG_SWITCH_MAX; i++) {
			delete pimg[i];
		}
	}

	Image* pimg[N_IMG_SWITCH_MAX];
	INT32 n_switch = 2;	//�X�C�b�`��
	INT32 n_flick = 2;	//�t���b�J������X�C�b�`��

	INT32 count = 0;	//�_�Ń^�C�~���O�p
	INT32 fcount = 3;	//�\���J�E���g�P�C���[�W��
	INT16 id_disp = 0;	//�\���C���[�WID
	INT32 list_flick_id[N_IMG_SWITCH_MAX] = { 0,1,0,0,0,0,0,0 };

	StringFormat* pStrFormat;
	Font* pFont;
	Rect rc;
	RECT rect;
	RectF frc;
	Graphics* pgraphic;

	Image* pimg_disp;

	HRESULT setup_flick(INT32 _n_flick, INT32 _fcount, INT32* pid) {
		fcount = _fcount;
		n_flick = _n_flick;
		for (int i = 0; i < n_flick; i++) {//�t���b�J������摜�̏�����荞��
			if (i < N_IMG_SWITCH_MAX) list_flick_id[i] = pid[i];
		}
		return S_OK;
	}

	virtual int chk_trig(INT16 id) {//�g���K�͕\���C���[�W��id
		if (id_disp != id) {
			id_disp = id; return VAL_TRIGGER_VALUE;
		}
		return VAL_TRIGGER_NON;
	}

	virtual INT16 get() {
		return id_disp;
	}

	HRESULT update() {
		INT16 id = 0;
		if (value != ID_PANEL_LAMP_FLICK) {
			id_disp = value;
			pgraphic->DrawImage(pimg[id_disp], rc);
			InvalidateRect(hWnd, &rect, FALSE);
			return S_OK;
		}
		else {
			count++; if (count > fcount) {
				count = 0;
				id_disp++; if (id_disp >= n_flick) id_disp = 0;
				pgraphic->DrawImage(pimg[list_flick_id[id_disp]], rc);
				InvalidateRect(hWnd, &rect, FALSE);
			}
			return S_OK;
		}
		return S_FALSE;
	}

};

/// <summary>
/// LampCtrl
/// Common Control�ւ̕`��iOWNER DRAW�j
/// </summary>
class CLampCtrl : public CPnlParts<INT16> {
public:
	CLampCtrl(
		INT32 _id, Point* ppt, Size* psz, LPWSTR ptext,
		Image** _ppimg, INT32 _n_switch,
		INT32 _fcount
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

		Rect _rc(0, 0, sz.Width, sz.Height); rc = _rc;
		RectF _frc(0, 0, sz.Width, sz.Height); frc = _frc;

		value = ID_PANEL_LAMP_OFF;
	}
	virtual ~CLampCtrl() {
		for (int i = 0; i < N_IMG_SWITCH_MAX; i++) {
			delete pimg[i];
		}
	}

	Image* pimg[N_IMG_SWITCH_MAX];
	INT32 n_switch = 2;	//�X�C�b�`��
	INT32 n_flick = 2;	//�t���b�J������X�C�b�`��

	INT32 count = 0;	//�_�Ń^�C�~���O�p
	INT32 fcount = 3;	//�\���J�E���g�P�C���[�W��
	INT16 id_disp = 0;	//�\���C���[�WID
	INT32 list_flick_id[N_IMG_SWITCH_MAX] = { 0,1,0,0,0,0,0,0 };

	StringFormat* pStrFormat;
	SolidBrush* pTxtBrush;
	Font* pFont;
	Rect rc;
	RectF frc;

	Image* pimg_disp;

	HRESULT set_txt_items(Font* pfont, StringFormat* pformat, SolidBrush* pbrush) {
		pFont = pfont; pStrFormat = pformat; pTxtBrush = pbrush;
		return S_OK;
	}
	HRESULT set_ctrl(CPnlParts* pCtrl) {
		if (pCtrl == NULL)return S_FALSE;
		hWnd = pCtrl->hWnd;
		rc = { 0,0,pCtrl->sz.Width, pCtrl->sz.Height };
		frc = { 0.0,0.0,(REAL)rc.Width,(REAL)rc.Height };
		return S_OK;
	}

	HRESULT setup_flick(INT32 _n_flick, INT32 _fcount, INT32* pid) {
		fcount = _fcount;
		n_flick = _n_flick;
		for (int i = 0; i < n_flick; i++) {//�t���b�J������摜�̏�����荞��
			if (i < N_IMG_SWITCH_MAX) list_flick_id[i] = pid[i];
		}
		return S_OK;
	}

	virtual int chk_trig(INT16 id) {//�g���K�͕\���C���[�W��id
		if (id_disp != id) {
			id_disp = id; return VAL_TRIGGER_VALUE;
		}
		return VAL_TRIGGER_NON;
	}

	virtual INT16 get() {
		return id_disp;
	}
	
	HRESULT update() {//WM_DRAWITEM��InvalidiateRect�ŌĂяo����value�ɃZ�b�g����index�̉摜��`��
		INT16 id = 0;
		if (value != ID_PANEL_LAMP_FLICK) {
			id = value;	if (chk_trig(id)) InvalidateRect(hWnd, NULL, TRUE);
			return S_OK;
		}
		else {
			count++; if (count > fcount) {
				count = 0;
				id_disp++; if (id_disp >= n_flick) id_disp = 0;
				InvalidateRect(hWnd, NULL, TRUE);
			}
			return S_OK;
		}
		return S_FALSE;
	}
};

/// <summary>
/// DCLamp
/// ON/OFF�ŉ摜��ؑ�
/// �\�[�X�f�o�C�X�R���e�L�X�g����؂�o��BitBilt
/// </summary>
class CLampDC : public CPnlParts<INT16> {
public:
	CLampDC(
		INT32 _id, Point* ppt, Size* psz, LPWSTR ptext,
		HDC _dc_dst, HDC _dc_src, Rect _rc_src_on, Rect _rc_src_off,
		INT32 _fcount
	)
	{
		set_id(_id);
		set_base(ppt, psz, ptext);
		dc_dst = _dc_dst;
		dc_src = _dc_src;
		rc_src_on = _rc_src_on;
		rc_src_off = _rc_src_off;
		fcount = _fcount;

		value = ID_PANEL_LAMP_OFF;
	}
	virtual ~CLampDC() {}

	HDC dc_dst = NULL;
	HDC dc_src = NULL;
	Rect rc_src_on = { 0,0,0,0 };
	Rect rc_src_off = { 0,0,0,0 };

	INT32 count = 0;			//�_�Ń^�C�~���O�p
	INT32 fcount = 6;			//�_�ŃJ�E���g�������
	INT32 fcount_on = 3;		//�_�ŃJ�E���gON

	virtual HRESULT set(INT16 code) {

		Rect rc_src;
		if (code == ID_PANEL_LAMP_OFF) {
			value = ID_PANEL_LAMP_OFF;
			rc_src = rc_src_off;
			count = 0;
		}
		else if (code == ID_PANEL_LAMP_ON) {
			value = ID_PANEL_LAMP_ON;
			rc_src = rc_src_on;
			count = 0;
		}
		else if (code == ID_PANEL_LAMP_FLICK) {
			count++; if (count >= fcount) count = 0;
			if (count > fcount_on) {
				value = ID_PANEL_LAMP_OFF;
				rc_src = rc_src_off;
			}
			else {
				value = ID_PANEL_LAMP_ON;
				rc_src = rc_src_on;
			}
		}
		else return S_FALSE;
		
		BitBlt(dc_dst, pt.X, pt.Y, sz.Width, sz.Height, dc_src,rc_src.X,rc_src.Y, SRCCOPY);

		return S_OK;
	}
};

/// <summary>
/// StringGdi
/// Gdi+�Ńe�L�X�g�o��
/// pt�͕�����̕\���ʒu,Size�͔w�i��`�̃T�C�Y�Aptext�́A����value�l
/// </summary>

class CStringGdi : public CPnlParts<std::wstring> {
public:
	CStringGdi(
		INT32 _id, Point* ppt, Size* psz, LPWSTR ptext, 
		Graphics* _pgraphic,StringFormat* _pStrFormat,SolidBrush* _pTxtBrush,Font* _pFont
	)
	{
		set_id(_id);
		set_base(ppt, psz, ptext);
		pgraphic = _pgraphic;
		pStrFormat = _pStrFormat; pTxtBrush = _pTxtBrush; pFont = _pFont; 
		Rect _rc(pt.X, pt.Y, sz.Width, sz.Height); rc = _rc;
		RectF _frc((REAL)pt.X, (REAL)pt.Y, (REAL)sz.Width, (REAL)sz.Height); frc = _frc;
		value = txt;	
	}
	virtual ~CStringGdi() {}

	Graphics* pgraphic;
	StringFormat* pStrFormat;
	SolidBrush* pTxtBrush;	//�e�L�X�g�u���V
	SolidBrush* pBkBrush;	//�w�i�u���V
	Font* pFont;
	Rect rc;
	RectF frc,frc_bk;

	HRESULT set_str_items(Font* pfont, StringFormat* pformat, SolidBrush* pbrush) {
		pFont = pfont; pStrFormat = pformat; pTxtBrush = pbrush;
		return S_OK;
	}

	HRESULT update(Graphics* _pgraphic, const LPCWSTR pstr) {
		if ((_pgraphic == NULL) || (pstr == NULL))return S_FALSE;
		value = pstr;
		pgraphic = _pgraphic;
		pgraphic->DrawString(value.c_str(), -1, pFont, frc, pStrFormat, pTxtBrush);
		return S_OK;
	}

	HRESULT update(Graphics* _pgraphic) {//�`���DC�ύX
		update(pgraphic, value.c_str());
		return S_OK;
	}

	HRESULT update() {//�ĕ`��
		update(pgraphic, value.c_str());
		return S_OK;
	}

	HRESULT update(const LPCWSTR pstr) {//�e�L�X�g�ύX
		update(pgraphic, pstr);
		return S_OK;
	}

	HRESULT update(SolidBrush* pbr_bk) {//�w�i�̂ݕύX
		pgraphic->FillRectangle(pbr_bk, rc);
		update(pgraphic, value.c_str());
		return S_OK;
	}

	HRESULT update(const LPCWSTR pstr,SolidBrush* pbr_bk) {//�e�L�X�g�Ɣw�i�ύX
		pgraphic->FillRectangle(pbr_bk, rc);
		update(pstr);
		return S_OK;
	}

};


