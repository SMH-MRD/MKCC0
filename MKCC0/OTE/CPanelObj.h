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
#define ID_MAIN_PNL_OBJ_PB_SYUKAN_ON		60014
#define ID_MAIN_PNL_OBJ_LMP_SYUKAN_ON		60015
#define ID_MAIN_PNL_OBJ_PB_SYUKAN_OFF		60016
#define ID_MAIN_PNL_OBJ_LMP_SYUKAN_OFF		60017
#define ID_MAIN_PNL_OBJ_CB_PNL_NOTCH		60018
#define ID_MAIN_PNL_OBJ_PB_PAD_MODE			60019
#define ID_MAIN_PNL_OBJ_LMP_PAD_MODE		60020
#define ID_MAIN_PNL_OBJ_PB_ASSIST_FUNC		60021
#define ID_MAIN_PNL_OBJ_TXT_OPE_TYPE		60022
#define ID_MAIN_PNL_OBJ_PB_OTE_TYPE_WND		60023
#define ID_MAIN_PNL_OBJ_TXT_LINK_CRANE		60024
#define ID_MAIN_PNL_OBJ_PB_CRANE_SEL_WND	60025

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

#define ID_MAIN_PNL_OBJ_PB_FRESET			60047
#define ID_MAIN_PNL_OBJ_LMP_FRESET			60048
#define ID_MAIN_PNL_OBJ_TXT_FRESET			60049

#define ID_PNL_SOCK_STAT_CLOSED				0//OFF
#define ID_PNL_SOCK_STAT_INIT				1//��
#define ID_PNL_SOCK_STAT_INIT_ERROR			2//��
#define ID_PNL_SOCK_STAT_STANDBY			3//��
#define ID_PNL_SOCK_STAT_ACT_RCV			4//��
#define ID_PNL_SOCK_STAT_ACT_SND			4//��
#define ID_PNL_SOCK_STAT_RCV_ERR			5//��
#define ID_PNL_SOCK_STAT_SND_ERR			5//��


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

	HWND hWnd;				//�E�B���h�E�n���h��
	Rect rc_panel;			//�p�l���̕\���ʒu

	HBITMAP hBmp_img;		//�C���[�W�f�ޗp�r�b�g�}�b�v
	HBITMAP hBmp_bk;		//�w�i�p�r�b�g�}�b�v
	HBITMAP hBmp_inf;		//���\���p�r�b�g�}�b�v

	HDC hdc;				//�p�l���֏������ݗpDC
	HDC hdc_img;			//�C���[�W�f�ޗpDC
	HDC hdc_bk;				//�w�i�pDC
	HDC hdc_inf;			//���\���pDC

	Graphics* pgraphic;		//�`��p�O���t�B�b�N�X
	Graphics* pgraphic_img;	//�C���[�W�f�ޗp�O���t�B�b�N�X
	Graphics* pgraphic_bk;	//�w�i�p�O���t�B�b�N�X
	Graphics* pgraphic_inf;	//���\���p�O���t�B�b�N�X

	SolidBrush* pBrushBk;	//�w�i�h��Ԃ��p�u���V

	void set_bk_brush(SolidBrush* pbr) { pBrushBk = pbr; return; };
	virtual HRESULT setup_graphics(HWND hwnd);
	virtual void clear_graghics();

	virtual HRESULT setup_obj() = 0;
	virtual void delete_obj() = 0;

};

class CMainPanelObj :public CPanelObjBase
{
private:
	int crane_id;
public:
	CMainPanelObj(HWND _hwnd, int _crane_id) : CPanelObjBase(_hwnd) {
		crane_id = _crane_id;
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
	CPbCtrl		* pb_syukan_on;		//�劲��PB
	CLampCtrl	* lmp_syukan_on;	//�劲�������v
	CPbCtrl		* pb_syukan_off;	//�劲��PB
	CLampCtrl	* lmp_syukan_off;	//�劲�؃����v
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

	CPbCtrl* pb_freset;			//�̏჊�Z�b�gPB
	CLampCtrl* lmp_freset;		//�̏჊�Z�b�g�����v
	CStaticCtrl* txt_freset;	//�̏჊�Z�b�g�\���e�L�X�g

	virtual HRESULT setup_obj();
	virtual void delete_obj();
};


#define ID_SUB_PNL_SET_OBJ_BASE					60200
#define ID_SUB_PNL_SET_OBJ_RDO_MHSPD_0			60200
#define ID_SUB_PNL_SET_OBJ_RDO_MHSPD_1			60201
#define ID_SUB_PNL_SET_OBJ_RDO_MHSPD_2			60202
#define ID_SUB_PNL_SET_OBJ_RDO_MHSPD			60203
#define ID_SUB_PNL_SET_OBJ_LMP_MHSPD			60204

#define ID_SUB_PNL_SET_OBJ_RDO_BHR_0			60205
#define ID_SUB_PNL_SET_OBJ_RDO_BHR_1			60206
#define ID_SUB_PNL_SET_OBJ_RDO_BHR_2			60207
#define ID_SUB_PNL_SET_OBJ_RDO_BHR				60208
#define ID_SUB_PNL_SET_OBJ_LMP_BHR				60209


class CSubPanelObj :public CPanelObjBase
{
private:
	int crane_id;
public:
	CSubPanelObj(HWND _hwnd, int _crane_id) : CPanelObjBase(_hwnd) {
		crane_id = _crane_id;
		setup_obj();
	}
	virtual ~CSubPanelObj() {
	}

	CCbCtrl*	cb_mh_spd_mode0;	//�努���x���[�h�I�����W�I�{�^��
	CCbCtrl*	cb_mh_spd_mode1;	//�努���x���[�h�I�����W�I�{�^��
	CCbCtrl*	cb_mh_spd_mode2;	//�努���x���[�h�I�����W�I�{�^��
	CRadioCtrl* rdo_mh_spd_mode;	//�努���x���[�h�I�����W�I�{�^��
	CLampCtrl*	lmp_mh_spd_mode;		//�ً}��~�����v

	CCbCtrl*	cb_bh_r_mode0;		//�努���x���[�h�I�����W�I�{�^��
	CCbCtrl*	cb_bh_r_mode1;		//�努���x���[�h�I�����W�I�{�^��
	CCbCtrl*	cb_bh_r_mode2;		//�努���x���[�h�I�����W�I�{�^��
	CRadioCtrl* rdo_bh_r_mode;		//�努���x���[�h�I�����W�I�{�^��
	CLampCtrl*	lmp_bh_r_mode;		//�ً}��~�����v

	virtual HRESULT setup_obj();
	virtual void delete_obj();
};