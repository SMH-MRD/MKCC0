#pragma once
#include <Windows.h>
#include <string>
#include "COMMON_DEF.H"


//######################################################################
// Value
//######################################################################

#define CODE_VALUE_ACTION_NON	0

#define ID_VALUE_POINT_DST		0
#define ID_VALUE_POINT_SRC_ON	1
#define ID_VALUE_POINT_SRC_OFF	2

#define VAL_PB_OFF_DELAY_COUNT	0x0002	//�{�^��OFF�f�B���C�J�E���g�l

#define VAL_TRIGGER_NON			0		//�g���K����
#define VAL_TRIGGER_ON			1		//ON�g���K
#define VAL_TRIGGER_OFF			-1		//OFF�g���K

#define N_VAL_RADIO_MAX			10		//RADIO PB�ő吔

template <class T> class CPnlParts {
public:
	CPnlParts() {}
	virtual ~CPnlParts() {}

public:
    T value;	    //���ݒl
 
	Point pt= { 0,0 };//�\���ʒu
	Size sz	= { 0,0 };//�\���T�C�Y
	std::wstring txt;

	HWND hWnd			= NULL;	//�E�B���h�E�n���h��
	INT32 id			= 0;	//���j���[ID

public:

	HRESULT set_base(Point* ppt, Size* psz, LPCWSTR ptext) {
		pt = *ppt; sz = *psz; std::wstring _txt = ptext; txt = _txt; return S_OK;
	};


	virtual T get() { return value; }
	virtual HRESULT set(T _value) { value = _value;  return S_OK; }

	HRESULT set_wnd(HWND _hWnd) { if (_hWnd == NULL) return S_FALSE;  hWnd = _hWnd; return S_OK; }
	
	INT32 get_id() { return id; }
	HRESULT set_id(INT32 _id) { id = _id; return S_OK; }

	int chk_trig(T _value) {
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
		set_base(ppt, psz, L"");
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
	CPbCtrl( INT32 _id, Point* ppt, Size* psz, LPCWSTR ptext) { 
		set_id(_id) ;
		set_base(ppt, psz, ptext);
	}
	virtual ~CPbCtrl() {}

	INT16 count_off_set_val = VAL_PB_OFF_DELAY_COUNT;	//�I�t�f�B���C�J�E���g�l
	INT16 update(bool is_trigger) {				//�X�V 
		if (is_trigger) set(count_off_set_val); //�g���K�[ON�Ńf�B���C�J�E���g�l�Z�b�g
		else if (value > 0) value--;			//�g���K�[�����ŃJ�E���g�_�E��
		else;
		return get(); //�߂�l�̓J�E���g�l
	}

	void set_off_delay(INT16 val_delay) { count_off_set_val = val_delay; return; }

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
	CCbCtrl(INT32 _id,Point* ppt, Size* psz, LPWSTR ptext)
	{
		set_id(_id); 
		set_base(ppt, psz, ptext);
	}
	virtual ~CCbCtrl() {}

	//�R���g���[���̏�Ԃ�get,set
	INT16 get_ctrl() { value = (INT16)SendMessage(hWnd, BM_GETCHECK, 0, 0); return value; }
	HRESULT set_ctrl(INT16 val) { 
		if (val)	SendMessage(hWnd, BM_SETCHECK, BST_CHECKED, 0); 
		else		SendMessage(hWnd, BM_SETCHECK, BST_UNCHECKED, 0);
		value = val;
		return S_OK; 
	};
};

/// <summary>
/// RADIO �R���g���[��
/// </summary>
class CRadioCtrl : public CPnlParts<INT16> {	//�{�^��

private:
	INT16 n_radio = 0;	//���W�I�{�^���̐�

public:

	CRadioCtrl(int _n_radio) {
		
		if (n_radio > N_VAL_RADIO_MAX) n_radio = N_VAL_RADIO_MAX;
		else if (_n_radio < 0)			n_radio = 0;
		else							n_radio = _n_radio;
	}
	virtual ~CRadioCtrl() {}

	CCbCtrl* pradio[N_VAL_RADIO_MAX] = {					//�`�F�b�N�{�^���^�̃|�C���^�z��
		NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL
	};

	void set_radio(int _n_radio, CCbCtrl* _pradio) {
		if (_n_radio >= n_radio) return;
		pradio[_n_radio] = _pradio;
	}

	INT16 check() { 
		INT16 val = -1;
		for (int i = 0; i < n_radio; i++) {
			if (pradio[i]->get() == L_ON) {
				value = val = i;
				break;
			}
		}
		return value = val;
	}

	INT16 check_ctrl() {
		INT16 val = -1;
		for (int i = 0; i < n_radio; i++) {
			if (pradio[i]->get_ctrl() == L_ON) {
				value = val = i;
				break;
			}
		}
	}

	virtual HRESULT set(INT16 val) {
		if ((val < 0) || (val >= n_radio)) return S_FALSE;

		pradio[val]->set(L_ON);	//Value�̃Z�b�g
		value = val;
		return S_OK;
	};

	virtual HRESULT set_ctrl(INT16 val) {
		if ((val < 0) || (val >= n_radio)) return S_FALSE;

		pradio[val]->set_ctrl(L_ON);	//�`�F�b�N�{�^���̃Z�b�g
		value = val;
		return S_OK;
	};

	
};

#define CODE_VALUE_LAMP_OFF			0x00
#define CODE_VALUE_LAMP_ON			0x01
#define CODE_VALUE_LAMP_FLICK		0xFF

#define CODE_TYPE_LAMP_ELLI			0
#define CODE_TYPE_LAMP_RECT			1

/// <summary>
/// DrawLamp
/// ON/OFF�Ńu���V��ؑ�
/// �f�o�C�X�R���e�L�X�g�ɋ�`�܂��͉~�`�̓h��Ԃ�
/// </summary>
class CLampDraw : public CPnlParts<INT16> {
public:
	CLampDraw(
		INT32 _id, Point* ppt, Size* psz, LPWSTR ptext,
		Graphics* _pgraphic, SolidBrush* _pbrush_on, SolidBrush* _pbrush_off, Font* _pfont,
		INT32 _fcount
	) 
	{
		set_id(_id);
		set_base(ppt, psz, ptext);
		pgraphic = _pgraphic;
		pbrush_on = _pbrush_on;
		pbrush_off = _pbrush_off;
		pfont = _pfont;
		fcount = _fcount;
			
		value = CODE_VALUE_LAMP_OFF;
	}

	virtual ~CLampDraw() {}

	SolidBrush* pbrush_on	= NULL;
	SolidBrush* pbrush_off	= NULL;
	Font* pfont = NULL;

	Graphics* pgraphic = NULL;	//�`��p�O���t�B�b�N�I�u�W�F�N�g

	INT32 count = 0;	//�_�Ń^�C�~���O�p

	INT16 type		= CODE_TYPE_LAMP_ELLI;	//�����v�̃^�C�v
	INT32 fcount	= 6;					//�_�ŃJ�E���g�������
	INT32 fcount_on = 3;					//�_�ŃJ�E���gON
	bool is_text_draw = false;				//�����\�����邩�H

	HRESULT set_txt_mode(bool _is_text_draw) {
		is_text_draw = _is_text_draw;
		return S_OK;
	}

	virtual HRESULT set(INT16 code) {

		SolidBrush* pbrush = NULL;
		if (code == CODE_VALUE_LAMP_OFF) {
			value = CODE_VALUE_LAMP_OFF;
			pbrush = pbrush_off;
			count = 0;
		}
		else if (code == CODE_VALUE_LAMP_ON) {
			value = CODE_VALUE_LAMP_ON;
			pbrush = pbrush_on;
			count = 0;
		}
		else if (code == CODE_VALUE_LAMP_FLICK) {
			count++; if (count >= fcount) count = 0;
			if (count > fcount_on) {
				value = CODE_VALUE_LAMP_OFF;
				pbrush = pbrush_off;
			}
			else {
				value = CODE_VALUE_LAMP_ON;
				pbrush = pbrush_on;
			}
		}
		else return S_FALSE;

		if (type == CODE_TYPE_LAMP_ELLI) {
			pgraphic->FillEllipse(pbrush, pt.X, pt.Y, sz.Width, sz.Height);;
		}
		else {
			pgraphic->FillRectangle(pbrush, pt.X, pt.Y, sz.Width, sz.Height);
		}

		return S_OK;
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

		value = CODE_VALUE_LAMP_OFF;
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
		if (code == CODE_VALUE_LAMP_OFF) {
			value = CODE_VALUE_LAMP_OFF;
			rc_src = rc_src_off;
			count = 0;
		}
		else if (code == CODE_VALUE_LAMP_ON) {
			value = CODE_VALUE_LAMP_ON;
			rc_src = rc_src_on;
			count = 0;
		}
		else if (code == CODE_VALUE_LAMP_FLICK) {
			count++; if (count >= fcount) count = 0;
			if (count > fcount_on) {
				value = CODE_VALUE_LAMP_OFF;
				rc_src = rc_src_off;
			}
			else {
				value = CODE_VALUE_LAMP_ON;
				rc_src = rc_src_on;
			}
		}
		else return S_FALSE;
		
		BitBlt(dc_dst, pt.X, pt.Y, sz.Width, sz.Height, dc_src,rc_src.X,rc_src.Y, SRCCOPY);

		return S_OK;
	}
};

/// <summary>
/// ImageLamp
/// �w��摜��ؑ֕\��
/// Imagge��ǂݍ���ŃO���t�B�b�N�ɏ�������
/// �o�^1�Ԗڂ̉摜��OFF�A2�Ԗڂ̉摜��ON�ŗ\��
/// </summary>
#define N_IMG_SWITCH_MAX			8
#define CODE_IMG_SWITCH_OFF			0
#define CODE_IMG_SWITCH_ON			1

class CSwitchImg : public CPnlParts<INT16> {
public:
	CSwitchImg(
		INT32 _id, Point* ppt, Size* psz, LPWSTR ptext,
		Graphics* _pgraphic, Image** _ppimg, INT32 _n_switch,
		INT32 _fcount
	)
	{
		set_id(_id);
		set_base(ppt, psz, ptext);
		pgraphic = _pgraphic;
		for (int i = 0; i < N_IMG_SWITCH_MAX; i++) {
			if (i < _n_switch) pimg[i] = _ppimg[i];
			else pimg[i] = NULL;
		}
		n_switch = _n_switch;
		fcount = _fcount;

		Rect _rc(0, 0, sz.Width, sz.Height); rc = _rc;
		RectF _frc(0, 0, sz.Width, sz.Height); frc = _frc;

		value = CODE_IMG_SWITCH_OFF;
	}
	virtual ~CSwitchImg() {
		for (int i = 0; i < N_IMG_SWITCH_MAX; i++) {
			delete pimg[i];
		}
	}

	Graphics* pgraphic;
	Image* pimg[N_IMG_SWITCH_MAX];
	INT32 n_switch		= 2;	//�X�C�b�`��
	INT32 n_flick		= 2;	//�t���b�J������X�C�b�`��

	INT32 count			= 0;	//�_�Ń^�C�~���O�p
	INT32 fcount		= 3;	//�\���J�E���g�P�C���[�W��
	INT32 id_disp		= 0;	//�\���C���[�WID
	INT32 list_flick_id[N_IMG_SWITCH_MAX] = { 0,1,0,0,0,0,0,0 };

	StringFormat* pStrFormat;
	SolidBrush* pTxtBrush;
	Font* pFont;
	Rect rc;
	RectF frc;
	
	HRESULT set_txt_items(Font* pfont, StringFormat* pformat, SolidBrush* pbrush) {
		pFont = pfont; pStrFormat = pformat; pTxtBrush = pbrush;
		return S_OK;
	}

	//
	DRAWITEMSTRUCT dis = { 0 };
	HRESULT set_dis(CPnlParts* pPB) {
		if (pPB == NULL)return S_FALSE;
		if (pPB->hWnd == NULL) return S_FALSE;
		
		dis.CtlType		= ODT_BUTTON;
		dis.CtlID		= pPB->id;
		dis.hwndItem	= pPB->hWnd;
		dis.hDC			= GetDC(pPB->hWnd);

		Rect _rc(0, 0, sz.Width, sz.Height); rc = _rc;
		RectF _frc(0, 0, sz.Width, sz.Height); frc = _frc;
		
		return S_OK;
	}

	HRESULT setup_flick(INT32 _n_flick, INT32 _fcount, INT32* pid) {
		fcount = _fcount;
		n_flick = _n_flick;
		for (int i = 0; i < n_flick; i++) {
			if (i < N_IMG_SWITCH_MAX) list_flick_id[i] = pid[i];
		}
		value = CODE_VALUE_LAMP_FLICK;
		return S_OK;
	}

	virtual HRESULT set(INT16 code) {

		Image* pimg_disp = pimg[CODE_IMG_SWITCH_OFF];
		value = code;

		if (code == CODE_VALUE_LAMP_FLICK) {//�t���b�J�@list_flick_id�ɓo�^�������ŏ����؂�ւ�
			count++; if (count > fcount) {
				count = 0;
				id_disp++; if (id_disp >= n_flick) id_disp = 0;
			}
			pimg_disp = pimg[list_flick_id[id_disp]];
		}
		else if ((code < n_switch) && (code >= 0)) {//�w�肵���C���[�W��\��
			id_disp = code;
			pimg_disp = pimg[id_disp];
		}
		else  return S_FALSE;

		pgraphic->DrawImage(pimg_disp, pt.X, pt.Y);

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
		INT32 _id, Point* ppt, Size* psz, LPWSTR ptext, Graphics* _pgraphic,Rect _rc_bk
	)
	{
		set_id(_id);
		set_base(ppt, psz, ptext);
		pgraphic = _pgraphic;
		PointF	pointf((REAL)pt.X, (REAL)pt.Y);	ptf = pointf;
		RectF rc_bk_f((REAL)_rc_bk.X, (REAL)_rc_bk.Y, (REAL)_rc_bk.Width, (REAL)_rc_bk.Height);
				
		value = txt;	
	}
	virtual ~CStringGdi() {}

	Graphics* pgraphic;
	PointF ptf = { 0,0 };	//�\���ʒu
	RectF rc_bk = { 0,0,0,0 };	//�\����`

	virtual HRESULT update(Font* pfont, SolidBrush* pbrush) {
		pgraphic->DrawString(txt.c_str(), (INT)txt.length(), pfont, ptf, NULL, pbrush);
		return S_OK;
	}
	virtual HRESULT update_bk(SolidBrush* pbrush) {
		pgraphic->FillRectangle(pbrush, rc_bk.X, rc_bk.Y, rc_bk.Width, rc_bk.Height);
		return S_OK;
	}
};


