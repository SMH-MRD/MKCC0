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
#define ID_MAIN_PNL_OBJ_TXT_PC_COM_STAT		60006
#define ID_MAIN_PNL_OBJ_TXT_PLC_COM_STAT	60007
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

//�e�N���[���p�̃I�u�W�F�N�g�������p�v���p�e�B�l�ێ��p�\����
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
	int panel_code;	//�p�l���R�[�h

	HWND hPnlWnd;			//�p�l���̃E�B���h�E�n���h��
	Rect rc_panel;			//�p�l���̕\���ʒu

	HBITMAP hBmp0;			//�C���[�W�f�ޗp�r�b�g�}�b�v
	HBITMAP hBmp_img;		//�C���[�W�f�ޗp�r�b�g�}�b�v
	HBITMAP hBmp_bk;		//�w�i�p�r�b�g�}�b�v
	HBITMAP hBmp_inf;		//���\���p�r�b�g�}�b�v

	Bitmap* pbmp0;			//�w�i�p�r�b�g�}�b�v
	Bitmap* pbmp_img;		//�w�i�p�r�b�g�}�b�v
	Bitmap* pbmp_bk;		//�w�i�p�r�b�g�}�b�v
	Bitmap* pbmp_inf;		//�w�i�p�r�b�g�}�b�v

	HDC hdc;				//�p�l���֏������ݗpDC
	HDC hdc_img;			//�C���[�W�f�ޗpDC
	HDC hdc_bk;				//�w�i�pDC
	HDC hdc_inf;			//���\���pDC

	Graphics* pgraphic;		//�`��p�O���t�B�b�N�X
	Graphics* pgraphic_img;	//�C���[�W�f�ޗp�O���t�B�b�N�X
	Graphics* pgraphic_bk;	//�w�i�p�O���t�B�b�N�X
	Graphics* pgraphic_inf;	//���\���p�O���t�B�b�N�X

	SolidBrush* pBrushBk;	//�w�i�h��Ԃ��p�u���V

	Color colorkey;				//�摜���d�ˍ��킹�鎞�̓��ߐF
	ImageAttributes attr;		//�J���[�L�[�𓧉߂�����ݒ�

	int set_panel_code(int code) { panel_code = code; return panel_code; };
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
		set_panel_code(ID_MAIN_PNL_OBJ_BASE);
		setup_obj();
	}
	virtual ~CMainPanelObj() {
	}

	CStringGdi	* str_message;
	CPbCtrl		* pb_remote;
	CLampCtrl	* lmp_remote;		//���u�ݒ胉���v
	CStaticCtrl	* txt_uid;
	CPbCtrl		* pb_auth;			//�F��PB
	CStaticCtrl* str_pc_com_stat;	//PC�ʐM�C���W�P�[�^�p���x��
	CStaticCtrl* str_plc_com_stat;	//PLC�ʐM�C���W�P�[�^�p���x��
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
	CLampCtrl	* lmp_pad_mode;		//�W���C�p�b�h���[�h�����v
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

	CPbCtrl		* pb_freset;			//�̏჊�Z�b�gPB
	CLampCtrl	* lmp_freset;		//�̏჊�Z�b�g�����v
	CStaticCtrl	* txt_freset;	//�̏჊�Z�b�g�\���e�L�X�g

	virtual HRESULT setup_obj();
	virtual void delete_obj();
	void refresh_obj_graphics();//�I�u�W�F�N�g�̃O���t�B�b�N�I�u�W�F�N�g�̐ݒ�X�V
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

#define ID_SUB_PNL_STAT_OBJ_BASE				60300
#define ID_SUB_PNL_STAT_OBJ_PB_NEXT				60300
#define ID_SUB_PNL_STAT_OBJ_PB_BACK				60301
#define ID_SUB_PNL_STAT_OBJ_STATIC_MH_DIR		60302
#define ID_SUB_PNL_STAT_OBJ_STATIC_BH_DIR		60303
#define ID_SUB_PNL_STAT_OBJ_STATIC_SL_DIR		60304
#define ID_SUB_PNL_STAT_OBJ_STATIC_GT_DIR		60305
#define ID_SUB_PNL_STAT_OBJ_STATIC_MH_TG_V		60306
#define ID_SUB_PNL_STAT_OBJ_STATIC_BH_TG_V		60307
#define ID_SUB_PNL_STAT_OBJ_STATIC_SL_TG_V		60308
#define ID_SUB_PNL_STAT_OBJ_STATIC_GT_TG_V		60309
#define ID_SUB_PNL_STAT_OBJ_STATIC_MH_REF_V		60310
#define ID_SUB_PNL_STAT_OBJ_STATIC_BH_REF_V		60311
#define ID_SUB_PNL_STAT_OBJ_STATIC_SL_REF_V		60312
#define ID_SUB_PNL_STAT_OBJ_STATIC_GT_REF_V		60313
#define ID_SUB_PNL_STAT_OBJ_STATIC_MH_FB_V		60314
#define ID_SUB_PNL_STAT_OBJ_STATIC_BH_FB_V		60315
#define ID_SUB_PNL_STAT_OBJ_STATIC_SL_FB_V		60316
#define ID_SUB_PNL_STAT_OBJ_STATIC_GT_FB_V		60317
#define ID_SUB_PNL_STAT_OBJ_STATIC_MH_REF_TRQ	60318
#define ID_SUB_PNL_STAT_OBJ_STATIC_BH_REF_TRQ	60319

#define ID_SUB_PNL_FLT_OBJ_BASE					60330
#define ID_SUB_PNL_FLT_OBJ_IMG_BK				60330
#define ID_SUB_PNL_FLT_OBJ_PB_NEXT				60331
#define ID_SUB_PNL_FLT_OBJ_CB_HISTORY			60332
#define ID_SUB_PNL_FLT_OBJ_CB_HEAVY1			60333
#define ID_SUB_PNL_FLT_OBJ_CB_LITE				60334
#define ID_SUB_PNL_FLT_OBJ_CB_IL				60335
#define ID_SUB_PNL_FLT_OBJ_CB_BYPASS			60336
#define ID_SUB_PNL_OBJ_STR_FLT_MESSAGE			60337
#define ID_SUB_PNL_FLT_OBJ_CB_HEAVY2			60338
#define ID_SUB_PNL_FLT_OBJ_CB_HEAVY3			60339
#define ID_SUB_PNL_FLT_OBJ_PB_PLCMAP			60340
#define ID_SUB_PNL_FLT_OBJ_LV_FAULTS			60341


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
	int i_disp_page = 0;
	int n_disp_page = 1;

	//�ݒ�T�u�E�B���h�E
	CCbCtrl*	cb_mh_spd_mode0;	//�努���x���[�h�I�����W�I�{�^��
	CCbCtrl*	cb_mh_spd_mode1;	//�努���x���[�h�I�����W�I�{�^��
	CCbCtrl*	cb_mh_spd_mode2;	//�努���x���[�h�I�����W�I�{�^��
	CRadioCtrl* rdo_mh_spd_mode;	//�努���x���[�h�I�����W�I�{�^��
	CSwitchImg*	lmp_mh_spd_mode;	//�ً}��~�����v

	CCbCtrl*	cb_bh_r_mode0;		//�努���x���[�h�I�����W�I�{�^��
	CCbCtrl*	cb_bh_r_mode1;		//�努���x���[�h�I�����W�I�{�^��
	CCbCtrl*	cb_bh_r_mode2;		//�努���x���[�h�I�����W�I�{�^��
	CRadioCtrl* rdo_bh_r_mode;		//�努���x���[�h�I�����W�I�{�^��
	CSwitchImg*	lmp_bh_r_mode;		//�ً}��~�����v

	//��ԕ\���T�u�E�B���h�E�̃I�u�W�F�N�g
	CPbCtrl*	pb_stat_next;		//���\��PB
	CPbCtrl*	pb_stat_back;		//�O�\��PB

	CStaticCtrl* st_mh_notch_dir;
	CStaticCtrl* st_bh_notch_dir;
	CStaticCtrl* st_sl_notch_dir;
	CStaticCtrl* st_gt_notch_dir;
	CStaticCtrl* st_mh_target_v;
	CStaticCtrl* st_bh_target_v;
	CStaticCtrl* st_sl_target_v;
	CStaticCtrl* st_gt_target_v;
	CStaticCtrl* st_mh_ref_v;
	CStaticCtrl* st_bh_ref_v;
	CStaticCtrl* st_sl_ref_v;
	CStaticCtrl* st_gt_ref_v;
	CStaticCtrl* st_mh_fb_v;
	CStaticCtrl* st_bh_fb_v;
	CStaticCtrl* st_sl_fb_v;
	CStaticCtrl* st_gt_fb_v;
	CStaticCtrl* st_mh_ref_trq;
	CStaticCtrl* st_bh_ref_trq;

	//�̏�\���T�u�E�B���h�E�̃I�u�W�F�N�g
	CSwitchImg* img_flt_bk;			//PLC�ʐM�C���W�P�[�^
	CPbCtrl* pb_flt_next;			//���\��PB
	CCbCtrl* cb_disp_history;		//����\���`�F�b�NBOX
	CCbCtrl* cb_disp_interlock;		//�努���x���[�h�I�����W�I�{�^��
	CCbCtrl* cb_disp_flt_light;		//�y�̏�\���`�F�b�N�{�^��
	CCbCtrl* cb_disp_flt_heavy1;	//�d�̏�1�\���`�F�b�N�{�^��
	CCbCtrl* cb_flt_bypass;			//IL�o�C�p�X�`�F�b�N�{�^��
	CStringGdi* str_flt_message;	//�̏�\�����b�Z�[�W���x��
	CCbCtrl* cb_disp_flt_heavy2;	//�d�̏�2�\���`�F�b�N�{�^��
	CCbCtrl* cb_disp_flt_heavy3;	//�d�̏�3�\���`�F�b�N�{�^��
	CPbCtrl* pb_disp_flt_plcmap;	//PLC faulrmap�\���`�F�b�N�{�^��
	CListViewCtrl* lv_flt_list;		//�̏჊�X�g�r���[
	INT16 flt_req_code = 0;			//�̏჊�X�g�X�V�v���R�[�h


	virtual HRESULT setup_obj();
	virtual void delete_obj();
	void refresh_obj_graphics();//�I�u�W�F�N�g�̃O���t�B�b�N�I�u�W�F�N�g�̐ݒ�X�V
};

#define ID_GWIN_MAIN_OBJ_BASE				60800
#define ID_GWIN_MAIN_OBJ_IMG_BK				60800
#define ID_GWIN_MAIN_OBJ_IMG_BOOM_XY		60801
#define ID_GWIN_MAIN_OBJ_IMG_GT_BASE		60802
#define ID_GWIN_MAIN_OBJ_IMG_POTAL			60803

#define ID_GWIN_MAIN_OBJ_STR_POS_MH			60820		
#define ID_GWIN_MAIN_OBJ_STR_POS_BH			60821		
#define ID_GWIN_MAIN_OBJ_STR_POS_SL			60822		
#define ID_GWIN_MAIN_OBJ_STR_POS_GT			60823		

class CGWindowObj :public CPanelObjBase
{
private:
	int crane_id;

public:
	CGWindowObj(HWND _hwnd, int _crane_id) : CPanelObjBase(_hwnd) {
		crane_id = _crane_id;
		setup_obj();
	}
	virtual ~CGWindowObj() {
	}

	//�O���t�B�b�N���C���E�B���h�E
	CSwitchImg* lmg_bk_gwindow;		//�O���t�B�b�N�E�B���h�E�̔w�i
	CSwitchImg* lmg_crane_bm_xy;	//�u�[�����
	CSwitchImg* lmg_crane_potal;	//�|�[�^����

	CSwitchImg* lmg_crane_gt_base;	//���s���u
	CStringGdi* str_pos_mh;			//�努����
	CStringGdi* str_pos_bh;			//���񔼌a
	CStringGdi* str_pos_sl;			//����p�x
	CStringGdi* str_pos_gt;			//���s�ʒu


	//�O���t�B�b�N�T�u�E�B���h�E
	CSwitchImg* lmg_crane_bm_yz;	//�u�[������


	virtual HRESULT setup_obj();
	virtual void delete_obj();
	void refresh_obj_graphics();//�I�u�W�F�N�g�̃O���t�B�b�N�I�u�W�F�N�g�̐ݒ�X�V
};

