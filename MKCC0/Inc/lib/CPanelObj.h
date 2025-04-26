#pragma once
#include "CPanelIo.h"
#include "CPanelGr.h"

using namespace std;

#define ID_MAIN_PNL_OBJ_BASE				60000
#define ID_MAIN_PNL_OBJ_STR_MESSAGE			60000
#define ID_MAIN_PNL_OBJ_PB_REMOTE			60001
#define ID_MAIN_PNL_OBJ_LMP_REMOTE			60002
#define ID_MAIN_PNL_OBJ_TXT_UID				60003
#define ID_MAIN_PNL_OBJ_PB_AUTH				60004
#define ID_MAIN_PNL_OBJ_STR_PC_COM_STAT		60006
#define ID_MAIN_PNL_OBJ_STR_PLC_COM_STAT	60007
#define ID_MAIN_PNL_OBJ_LMP_PCR				60008
#define ID_MAIN_PNL_OBJ_LMP_PCS				60009
#define ID_MAIN_PNL_OBJ_LMP_PLCR			60010
#define ID_MAIN_PNL_OBJ_LMP_PLCS			60011
#define ID_MAIN_PNL_OBJ_CB_ESTOP			60012
#define ID_MAIN_PNL_OBJ_LMP_ESTOP			60013
#define ID_MAIN_PNL_OBJ_PB_CSOURCE			60014
#define ID_MAIN_PNL_OBJ_LMP_CSOURCE			60015
#define ID_MAIN_PNL_OBJ_CB_PNL_NOTCH		60016
#define ID_MAIN_PNL_OBJ_PB_PAD_MODE			60017
#define ID_MAIN_PNL_OBJ_LMP_PAD_MODE		60018
#define ID_MAIN_PNL_OBJ_PB_ASSIST_FUNC		60019
#define ID_MAIN_PNL_OBJ_TXT_OPE_TYPE		60020
#define ID_MAIN_PNL_OBJ_PB_OTE_TYPE_WND		60021
#define ID_MAIN_PNL_OBJ_TXT_LINK_CRANE		60022
#define ID_MAIN_PNL_OBJ_PB_CRANE_SEL_WND	60023

#define ID_MAIN_PNL_OBJ_RDO_DISP_MODE1		60030
#define ID_MAIN_PNL_OBJ_RDO_DISP_MODE2		60031
#define ID_MAIN_PNL_OBJ_RDO_DISP_MODE		60032

#define ID_MAIN_PNL_OBJ_RDO_OPT_WND_FLT		60040
#define ID_MAIN_PNL_OBJ_RDO_OPT_WND_SET		60041
#define ID_MAIN_PNL_OBJ_RDO_OPT_WND_COM		60042
#define ID_MAIN_PNL_OBJ_RDO_OPT_WND_CAM		60043
#define ID_MAIN_PNL_OBJ_RDO_OPT_WND_STAT	60044
#define ID_MAIN_PNL_OBJ_RDO_OPT_WND_CLR		60045
#define ID_MAIN_PNL_OBJ_RDO_OPT_WND			60046

#define N_MAIN_PNL_WCHAR	128

struct ST_OBJ_PROPERTY {
	INT32 id;	//ID
	Point pt;	//�\���ʒu
	Size sz;	//�\���T�C�Y
	WCHAR txt[N_MAIN_PNL_WCHAR];	//�\��������
};

class CPanelObjBase {
public:

	CPanelObjBase(HWND _hwnd);

	virtual ~CPanelObjBase(); 

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

	Graphics* pgraphic;		//�`��p�O���t�B�b�N�X
	Graphics* pgraphic_mem;	//�`��p�O���t�B�b�N�X
	Graphics* pgraphic_bk;	//�w�i�p�O���t�B�b�N�X
	Graphics* pgraphic_inf;	//���\���p�O���t�B�b�N�X

	SolidBrush* pBrushBk;	//�w�i�h��Ԃ��p�u���V

	void set_bk_brush(SolidBrush* pbr) { pBrushBk = pbr; return; };

	virtual HRESULT setup_obj() = 0;
	virtual void delete_obj() = 0;
};

class CMainPanelObj :public CPanelObjBase
{
public:
	CMainPanelObj(HWND _hwnd) : CPanelObjBase(_hwnd) {

		setup_obj();
	}
	virtual ~CMainPanelObj() {
	}

	CStringGdi	* str_message;
	CPbCtrl		* pb_remote;
	CLampCtrl	* lmp_remote;		//���u�ݒ胉���v
	CStaticCtrl	* txt_uid;
	CPbCtrl		* pb_auth;			//�F��PB
	CStringGdi	* str_pc_com_stat;	//PC�ʐM�C���W�P�[�^�p���x��
	CStringGdi	* str_plc_com_stat;	//PLC�ʐM�C���W�P�[�^�p���x��
	CSwitchImg	* lmp_pcr;			//PC�ʐM�C���W�P�[�^
	CSwitchImg	* lmp_pcs;			//PC�ʐM�C���W�P�[�^
	CSwitchImg	* lmp_plcr;			//PLC�ʐM�C���W�P�[�^
	CSwitchImg	* lmp_plcs;			//PLC�ʐM�C���W�P�[�^

	CCbCtrl		* cb_estop;			//�ً}��~�`�F�b�N�{�b�N�X
	CLampCtrl	* lmp_estop;		//�ً}��~�����v
	CPbCtrl		* pb_csource;		//�劲PB
	CLampCtrl* lmp_csource;		//�劲�����v
	CCbCtrl		* cb_pnl_notch;		//�p�l���m�b�`PB
	CPbCtrl		* pb_pad_mode;		//�W���C�p�b�h���[�hPB
	CLampCtrl* lmp_pad_mode;		//�W���C�p�b�h���[�h�����v
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

