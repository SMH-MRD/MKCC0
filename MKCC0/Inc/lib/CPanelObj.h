#pragma once
#include "CPanelIo.h"
#include "CPanelGr.h"

using namespace std;

#define ID_MAIN_PNL_OBJ_BASE				90000
#define ID_MAIN_PNL_OBJ_STR_MESSAGE			90000
#define ID_MAIN_PNL_OBJ_PB_REMOTE			90001
#define ID_MAIN_PNL_OBJ_LMP_REMOTE			90002
#define ID_MAIN_PNL_OBJ_TXT_UID				90003
#define ID_MAIN_PNL_OBJ_PB_AUTH				90004
#define ID_MAIN_PNL_OBJ_LAB_PC_COM_STAT		90006
#define ID_MAIN_PNL_OBJ_LAB_PLC_COM_STAT	90007
#define ID_MAIN_PNL_OBJ_LMP_PCR				90008
#define ID_MAIN_PNL_OBJ_LMP_PCS				90009
#define ID_MAIN_PNL_OBJ_LMP_PLCR			90010
#define ID_MAIN_PNL_OBJ_LMP_PLCS			90011
#define ID_MAIN_PNL_OBJ_CB_ESTOP			90012
#define ID_MAIN_PNL_OBJ_LMP_ESTOP			90013
#define ID_MAIN_PNL_OBJ_PB_CSOURCE			90014
#define ID_MAIN_PNL_OBJ_LMP_CSOURCE			90015
#define ID_MAIN_PNL_OBJ_CB_PNL_NOTCH		90016
#define ID_MAIN_PNL_OBJ_PB_PAD_MODE			90017
#define ID_MAIN_PNL_OBJ_LMP_PAD_MODE		90018
#define ID_MAIN_PNL_OBJ_PB_ASSIST_FUNC		90019
#define ID_MAIN_PNL_OBJ_TXT_OPE_TYPE		90020
#define ID_MAIN_PNL_OBJ_PB_OTE_TYPE_WND		90021
#define ID_MAIN_PNL_OBJ_TXT_LINK_CRANE		90022
#define ID_MAIN_PNL_OBJ_PB_CRANE_SEL_WND	90023

#define ID_MAIN_PNL_OBJ_RDO_DISP_MODE1		90030
#define ID_MAIN_PNL_OBJ_RDO_DISP_MODE2		90031
#define ID_MAIN_PNL_OBJ_RDO_DISP_MODE		90032

#define ID_MAIN_PNL_OBJ_RDO_OPT_WND_FLT		90040
#define ID_MAIN_PNL_OBJ_RDO_OPT_WND_SET		90041
#define ID_MAIN_PNL_OBJ_RDO_OPT_WND_COM		90042
#define ID_MAIN_PNL_OBJ_RDO_OPT_WND_CAM		90043
#define ID_MAIN_PNL_OBJ_RDO_OPT_WND_STAT	90044
#define ID_MAIN_PNL_OBJ_RDO_OPT_WND_CLR		90045
#define ID_MAIN_PNL_OBJ_RDO_OPT_WND			90046

#define N_MAIN_PNL_WCHAR	128

struct ST_OBJ_PROPERTY {
	INT32 id;	//ID
	Point pt;	//�\���ʒu
	Size sz;	//�\���T�C�Y
	WCHAR txt[N_MAIN_PNL_WCHAR];	//�\��������
};

class CPanelObjBase {
public:

	CPanelObjBase(HWND _hwnd)
	{
		hWnd = _hwnd;
		RECT rect;
		GetClientRect(hWnd, &rect);
		rc_panel.X = rect.left; rc_panel.Y = rect.top; rc_panel.Width = rect.right - rect.left; rc_panel.Height = rect.bottom - rect.top;
		hdc = GetDC(hWnd);

		hBmp_mem = CreateCompatibleBitmap(hdc, rc_panel.Width, rc_panel.Height);
		hBmp_bk = CreateCompatibleBitmap(hdc, rc_panel.Width, rc_panel.Height);
		hBmp_inf = CreateCompatibleBitmap(hdc, rc_panel.Width, rc_panel.Height);
		if (hBmp_mem == NULL || hBmp_bk == NULL || hBmp_inf == NULL) {
			hr = E_FAIL;
			return;
		}

		hdc_mem = CreateCompatibleDC(hdc);
		hdc_bk = CreateCompatibleDC(hdc);
		hdc_inf = CreateCompatibleDC(hdc);
		if (hdc_mem == NULL || hdc_bk == NULL || hdc_inf == NULL) {
			hr = E_FAIL;
			return;
		}
		SelectObject(hdc_mem, hBmp_mem);
		SelectObject(hdc_bk, hBmp_bk);
		SelectObject(hdc_inf, hBmp_inf);

		pgraph_mem = new Graphics(hdc_mem);
		pgraph_bk = new Graphics(hdc_bk);
		pgraph_inf = new Graphics(hdc_inf);
		if (pgraph_mem == NULL || pgraph_bk == NULL || pgraph_inf == NULL) {
			hr = E_FAIL;
			return;
		}
		pgraph_mem->SetSmoothingMode(SmoothingModeAntiAlias);
		pgraph_bk->SetSmoothingMode(SmoothingModeAntiAlias);
		pgraph_inf->SetSmoothingMode(SmoothingModeAntiAlias);
	}
	virtual ~CPanelObjBase() {
		delete pgraph_mem;
		delete pgraph_bk;
		delete pgraph_inf;
		DeleteDC(hdc_mem);
		DeleteDC(hdc_bk);
		DeleteDC(hdc_inf);
		DeleteObject(hBmp_mem);
		DeleteObject(hBmp_bk);
		DeleteObject(hBmp_inf);
		ReleaseDC(hWnd, hdc);
	}

	HRESULT hr = S_OK;

	HWND hWnd;			//�E�B���h�E�n���h��
	Rect rc_panel;		//�p�l���̕\���ʒu

	HBITMAP hBmp_mem;	//�w�i�p�r�b�g�}�b�v
	HBITMAP hBmp_bk;	//�p�l���p�r�b�g�}�b�v
	HBITMAP hBmp_inf;	//���\���p�r�b�g�}�b�v

	HDC hdc;			//�p�l���֏������ݗpDC
	HDC hdc_mem;		//�p�l���֏������ݗpDC
	HDC hdc_bk;			//�w�i�pDC
	HDC hdc_inf;		//���\���pDC

	Graphics* pgraph_mem;	//�`��p�O���t�B�b�N�X
	Graphics* pgraph_bk;	//�w�i�p�O���t�B�b�N�X
	Graphics* pgraph_inf;	//���\���p�O���t�B�b�N�X

	virtual HRESULT setup_obj() = 0;
	virtual void delete_obj() = 0;
};

class CMainPanelObj :CPanelObjBase
{
public:
	CMainPanelObj(HWND _hwnd) : CPanelObjBase(_hwnd) {

		setup_obj();
	}
	virtual ~CMainPanelObj() {
	}

	CStringGdi	* str_message;
	CPbCtrl		* pb_remote;
	CSwitchImg	* lmp_remote;
	CStaticCtrl	* txt_uid;
	CPbCtrl		* pb_auth;			//�F��PB
	CStringGdi	* lab_pc_com_stat;	//PC�ʐM�C���W�P�[�^�p���x��
	CStringGdi	* lab_plc_com_stat;	//PLC�ʐM�C���W�P�[�^�p���x��
	CSwitchImg	* lmp_pcr;			//PC�ʐM�C���W�P�[�^
	CSwitchImg	* lmp_pcs;			//PC�ʐM�C���W�P�[�^
	CSwitchImg	* lmp_plcr;			//PLC�ʐM�C���W�P�[�^
	CSwitchImg	* lmp_plcs;			//PLC�ʐM�C���W�P�[�^

	CCbCtrl		* cb_estop;			//�ً}��~�`�F�b�N�{�b�N�X
	CSwitchImg	* lamp_estop;		//�ً}��~�����v
	CPbCtrl		* pb_csource;		//�劲PB
	CSwitchImg	* lmp_csource;		//�劲�����v
	CCbCtrl		* cb_pnl_notch;		//�p�l���m�b�`PB
	CPbCtrl		* pb_pad_mode;		//�W���C�p�b�h���[�hPB
	CSwitchImg	* lmp_pad_mode;		//�W���C�p�b�h���[�h�����v
	CPbCtrl		* pb_assist_func;	//�A�V�X�g�@�\�ݒ�
	CStaticCtrl	* txt_ote_type;		//�[���^�C�v�\���e�L�X�g
	CPbCtrl		* pb_ote_type_wnd;	//�[���^�C�v�ݒ�E�B���h�E
	CStaticCtrl	* txt_link_crane;	//�ڑ���N���[���\���e�L�X�g
	CPbCtrl		* pb_crane_sel_wnd;	//�ڑ���N���[���I���E�B���h�E

	CCbCtrl		* cb_disp_mode1;	//�\�����[�h�I�����W�I�{�^��
	CCbCtrl		* cb_disp_mode2;	//�\�����[�h�I�����W�I�{�^��
	CRadioCtrl	* rdo_disp_mode;	//�\�����[�h�I�����W�I�{�^��

	CCbCtrl		* cb_opt_flt;	//�I�v�V�����E�B���h�E���W�I�{�^��
	CCbCtrl		* cb_opt_set;	//�I�v�V�����E�B���h�E���W�I�{�^��
	CCbCtrl		* cb_opt_com;	//�I�v�V�����E�B���h�E���W�I�{�^��
	CCbCtrl		* cb_opt_cam;	//�I�v�V�����E�B���h�E���W�I�{�^��
	CCbCtrl		* cb_opt_stat;	//�I�v�V�����E�B���h�E���W�I�{�^��
	CCbCtrl		* cb_opt_clr;	//�I�v�V�����E�B���h�E���W�I�{�^��
	CRadioCtrl	* rdo_opt_wnd;	//�I�v�V�����E�B���h�E���W�I�{�^��

	virtual HRESULT setup_obj();
	virtual void delete_obj();
};

