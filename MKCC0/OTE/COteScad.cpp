#include "COteScad.h"
#include "resource.h"
#include "framework.h"
#include "CCrane.H"
#include "COteEnv.h"
#include "COpePanel.h"
#include "CPanelWindow.h"

extern vector<CBasicControl*>	VectCtrlObj;
extern BC_TASK_ID st_task_id;

extern CSharedMem* pOteEnvInfObj;
extern CSharedMem* pOteCsInfObj;
extern CSharedMem* pOteCcInfObj;
extern CSharedMem* pOteUiObj;

extern CCrane* pCrane;
extern ST_DEVICE_CODE g_my_code;

ST_OTE_SCAD_MON1 COteScad::st_mon1;
ST_OTE_SCAD_MON2 COteScad::st_mon2;

ST_OTE_UI COteScad::st_work;


CPanelBase* pPanelBase;

extern ST_DRAWING_BASE		drawing_items;

static CSubPanelWindow* psubpanel = NULL;
//���L������
static LPST_OTE_ENV_INF	pOteEnvInf;
static LPST_OTE_UI		pOteUI;
static LPST_OTE_CC_IF	pOteCCIf;
static LPST_OTE_CS_INF	pOteCsInf;

static COteEnv* pEnvObj;

static LARGE_INTEGER start_count_s, end_count_r;			//�V�X�e���J�E���g
static LARGE_INTEGER frequency;								//�V�X�e�����g��
static LONGLONG res_delay_max, res_delay_min = 10000000;	//C��������

static wostringstream wos_msg;
/****************************************************************************/
/*   �f�t�H���g�֐�											                    */
/****************************************************************************/
COteScad::COteScad() {
}
COteScad::~COteScad() {
}

HRESULT COteScad::initialize(LPVOID lpParam) {

	//�V�X�e�����g���ǂݍ���
	QueryPerformanceFrequency(&frequency);

	HRESULT hr = S_OK;
	//### �o�͗p���L�������擾
	out_size = sizeof(ST_OTE_CC_IF);
	if (OK_SHMEM != pOteCcInfObj->create_smem(SMEM_OTE_CC_IF_NAME, out_size, MUTEX_OTE_CC_IF_NAME)) {
		err |= SMEM_NG_CS_INF; hr = S_FALSE;
	}
	set_outbuf(pOteCcInfObj->get_pMap());

	//### ���L�������擾
	pOteCCIf = (LPST_OTE_CC_IF)(pOteCcInfObj->get_pMap());
	pOteCsInf = (LPST_OTE_CS_INF)(pOteCsInfObj->get_pMap());
	pOteUI = (LPST_OTE_UI)pOteUiObj->get_pMap();
	pOteEnvInf = (LPST_OTE_ENV_INF)pOteEnvInfObj->get_pMap();

	if ((pOteCCIf == NULL) || (pOteCsInf == NULL) || (pOteUI == NULL) || (pOteEnvInf == NULL))
		hr = S_FALSE;

	if (hr == S_FALSE) {
		wos.str(L""); wos << L"Initialize : SMEM NG"; msg2listview(wos.str());
		return hr;
	};

	//### Environment�N���X�C���X�^���X�̃|�C���^�擾
	pEnvObj = (COteEnv*)VectCtrlObj[st_task_id.ENV];

	//### �p�l���֘A�Z�b�g�A�b�v
	CPanelBase::hr_init_setting = CPanelBase::setup_common_base(pOteUI);//�x�[�X�N���X������
	CMainPanelWindow::set_up(pOteUI, pOteCsInf, pOteCCIf, pOteEnvInf);						//���C���p�l���Z�b�g�A�b�v

	//###  �I�y���[�V�����p�l���ݒ�
	//Function mode RADIO1
	inf.panel_func_id = IDC_TASK_FUNC_RADIO1;
	SendMessage(GetDlgItem(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO1), BM_SETCHECK, BST_CHECKED, 0L);
	for (int i = 1; i < 6; i++)
		SendMessage(GetDlgItem(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO1 + i), BM_SETCHECK, BST_UNCHECKED, 0L);
	//���[�h�ݒ�0
	inf.mode_id = BC_ID_MODE0;
	SendMessage(GetDlgItem(inf.hwnd_opepane, IDC_TASK_MODE_RADIO0), BM_SETCHECK, BST_CHECKED, 0L);
	//���j�^�E�B���h�E�e�L�X�g	
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_MON_CHECK2, L"MKCC IF");
	set_item_chk_txt();
	set_panel_tip_txt();
	//���j�^�QCB��ԃZ�b�g	
	if (st_mon2.hwnd_mon != NULL)
		SendMessage(GetDlgItem(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK1), BM_SETCHECK, BST_CHECKED, 0L);
	else
		SendMessage(GetDlgItem(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK1), BM_SETCHECK, BST_UNCHECKED, 0L);

	return hr;
}

HRESULT COteScad::routine_work(void* pObj) {
	if (inf.total_act % 20 == 0) {
		wos.str(L""); wos << inf.status << L":" << std::setfill(L'0') << std::setw(4) << inf.act_time;
		msg2host(wos.str());
	}
	input();
	parse();
	output();
	return S_OK;
}

int COteScad::input() {

	return S_OK;
}

int COteScad::parse() {
	set_panel_io();
	return S_OK;
}

int COteScad::output() {          //�o�͏���
	memcpy_s(pOteUI, sizeof(ST_OTE_UI), &st_work, sizeof(ST_OTE_UI));
	return STAT_OK;
}

int COteScad::close() {

	return 0;
}

HRESULT COteScad::open_ope_window() {

	WPARAM wp = MAKELONG(inf.index, WM_USER_WPH_OPEN_IF_WND);//HWORD:�R�}���h�R�[�h, LWORD:�^�X�N�C���f�b�N�X
	LPARAM lp = BC_ID_MON1;
	SendMessage(inf.hwnd_opepane, WM_USER_TASK_REQ, wp, lp);

	return S_OK;
}

void COteScad::set_panel_io() {
	//st_work�ɃZ�b�g����output�ŋ��L�������֏o�͂���
	// !!GamePad�̓��͂�MON1�E�B���h�E��TIMER��CS�̋��L��������GPAD�����݂�PanelBase�̃I�u�W�F�N�g�ɃZ�b�g���Ă���
	if (pPanelBase != NULL) {
		st_work.ctrl_stat[OTE_PNL_CTRLS::estop]			= pPanelBase->pmainobjs->cb_estop->get();
		st_work.ctrl_stat[OTE_PNL_CTRLS::syukan_on]		= pPanelBase->pmainobjs->pb_syukan_on->get();
		st_work.ctrl_stat[OTE_PNL_CTRLS::syukan_off]	= pPanelBase->pmainobjs->pb_syukan_off->get();
		st_work.ctrl_stat[OTE_PNL_CTRLS::remote]		= pPanelBase->pmainobjs->pb_remote->get();
		if (pPanelBase->pmainobjs->pb_remote->get() != 0)
			int a = 0;
		st_work.ctrl_stat[OTE_PNL_CTRLS::game_pad]		= pPanelBase->pmainobjs->pb_pad_mode->get();
		st_work.ctrl_stat[OTE_PNL_CTRLS::fault_reset]	= pPanelBase->pmainobjs->pb_freset->get();
	}
	return;
}

/****************************************************************************/
/*   �^�X�N�ݒ�^�u�p�l���E�B���h�E�̃R�[���o�b�N�֐�                       */
/****************************************************************************/
LRESULT CALLBACK COteScad::PanelProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp) {

	switch (msg) {
	case WM_USER_TASK_REQ: {
		if (HIWORD(wp) == WM_USER_WPH_OPEN_IF_WND) {
			wos.str(L"");
			if (lp == BC_ID_MON1) st_mon1.hwnd_mon = open_monitor_wnd(hDlg, lp);
			if (lp == BC_ID_MON2) st_mon2.hwnd_mon = open_monitor_wnd(hDlg, lp);
		}
		else if (wp == WM_USER_WPH_CLOSE_IF_WND) 	close_monitor_wnd(lp);
		else;
	}break;
	case WM_COMMAND:
		switch (LOWORD(wp)) {
		case IDC_TASK_FUNC_RADIO1:
		case IDC_TASK_FUNC_RADIO2:
		case IDC_TASK_FUNC_RADIO3:
		case IDC_TASK_FUNC_RADIO4:
		case IDC_TASK_FUNC_RADIO5:
		case IDC_TASK_FUNC_RADIO6:
		{
			inf.panel_func_id = LOWORD(wp);
			set_panel_tip_txt();
			set_item_chk_txt();
			set_PNLparam_value(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
		}break;

		case IDC_TASK_ITEM_CHECK1: {
			switch (inf.panel_func_id) {
			case IDC_TASK_FUNC_RADIO4:
				set_item_chk_txt();
				break;
			default:break;
			}

		}break;
		case IDC_TASK_ITEM_CHECK2:
		case IDC_TASK_ITEM_CHECK3:
		case IDC_TASK_ITEM_CHECK4:
		case IDC_TASK_ITEM_CHECK5:
		case IDC_TASK_ITEM_CHECK6:
		{
			if (IsDlgButtonChecked(hDlg, LOWORD(wp)) == BST_CHECKED)
				inf.panel_act_chk[inf.panel_func_id - IDC_TASK_FUNC_RADIO1][LOWORD(wp) - IDC_TASK_ITEM_CHECK1] = true;
			else
				inf.panel_act_chk[inf.panel_func_id - IDC_TASK_FUNC_RADIO1][LOWORD(wp) - IDC_TASK_ITEM_CHECK1] = false;
		}break;

		case IDSET:
		{
			wstring wstr, wstr_tmp;

			wstr += L"Param 1(d):";
			int n = GetDlgItemText(hDlg, IDC_TASK_EDIT1, (LPTSTR)wstr_tmp.c_str(), 128);
			msg2listview(wstr);

		}break;
		case IDRESET:
		{
			set_PNLparam_value(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);

		}break;

		case IDC_TASK_MODE_RADIO0:
		{
			inf.mode_id = BC_ID_MODE0;
		}break;
		case IDC_TASK_MODE_RADIO1:
		{
			inf.mode_id = BC_ID_MODE1;
		}break;
		case IDC_TASK_MODE_RADIO2:
		{
			inf.mode_id = BC_ID_MODE2;
		}break;

		case IDC_TASK_MON_CHECK1:
		{
			if (IsDlgButtonChecked(hDlg, IDC_TASK_MON_CHECK1) == BST_CHECKED) {
				open_monitor_wnd(inf.hwnd_parent, BC_ID_MON1);
			}
			else {
				close_monitor_wnd(BC_ID_MON1);
			}
		}break;

		case IDC_TASK_MON_CHECK2: {

			if (IsDlgButtonChecked(hDlg, IDC_TASK_MON_CHECK2) == BST_CHECKED) {
				show_monitor_wnd(BC_ID_MON2);
			}
			else {
				hide_monitor_wnd(BC_ID_MON2);
			}
		}break;
		}
	}
	return 0;
};

/****************************************************************************/
/*   ���j�^�E�B���h�E									                    */
/****************************************************************************/
static bool is_initial_draw_mon1 = false;

LRESULT CALLBACK COteScad::Mon1Proc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
	
	switch (msg)
	{
	case WM_CREATE: {
		InitCommonControls();//�R�����R���g���[��������
		HINSTANCE hInst = (HINSTANCE)GetModuleHandle(0);

		pPanelBase = new CPanelBase(pOteEnvInf->selected_crane, pOteEnvInf->device_code.serial_no, hWnd);

		//�E�B���h�E�ɃR���g���[���ǉ�

		int i;
		//STATIC,LABEL
		CStaticCtrl* pst = pPanelBase->pmainobjs->txt_uid;
		pst->set_wnd( CreateWindowW(TEXT("STATIC"), pst->txt.c_str(), WS_CHILD | WS_VISIBLE | SS_LEFT,
			pst->pt.X, pst->pt.Y, pst->sz.Width, pst->sz.Height, hWnd, (HMENU)(pst->id), hInst, NULL));


		pst = pPanelBase->pmainobjs->txt_ote_type;
		pst->set_wnd(CreateWindowW(TEXT("STATIC"), pst->txt.c_str(), WS_CHILD | WS_VISIBLE | SS_LEFT,
			pst->pt.X, pst->pt.Y, pst->sz.Width, pst->sz.Height, hWnd, (HMENU)(pst->id), hInst, NULL));

		pst = pPanelBase->pmainobjs->txt_link_crane;
		pst->set_wnd(CreateWindowW(TEXT("STATIC"), pst->txt.c_str(), WS_CHILD | WS_VISIBLE | SS_LEFT,
			pst->pt.X, pst->pt.Y, pst->sz.Width, pst->sz.Height, hWnd, (HMENU)(pst->id), hInst, NULL));
	
		pst = pPanelBase->pmainobjs->txt_freset;
		pst->set_wnd(CreateWindowW(TEXT("STATIC"), pst->txt.c_str(), WS_CHILD | WS_VISIBLE | SS_LEFT,
			pst->pt.X, pst->pt.Y, pst->sz.Width, pst->sz.Height, hWnd, (HMENU)(pst->id), hInst, NULL));

		//CB
		CCbCtrl* pcb = pPanelBase->pmainobjs->cb_estop;
		i = OTE_SCAD_ID_MON1_CB_ESTP;
		pcb->set_wnd(CreateWindowW(TEXT("BUTTON"), pcb->txt.c_str(), WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX | BS_PUSHLIKE | BS_MULTILINE | BS_OWNERDRAW,
					pcb->pt.X, pcb->pt.Y, pcb->sz.Width, pcb->sz.Height,hWnd, (HMENU)(pcb->id), hInst, NULL));
		pPanelBase->pmainobjs->lmp_estop->set_ctrl(pcb);//�����v�Ƀ{�^���̃{�^���R���g���[�����Z�b�g

		//�����
		pcb = pPanelBase->pmainobjs->cb_pnl_notch;
		pcb->set_wnd(CreateWindowW(TEXT("BUTTON"), pcb->txt.c_str(), WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX| BS_PUSHLIKE | BS_MULTILINE,
			pcb->pt.X, pcb->pt.Y, pcb->sz.Width, pcb->sz.Height, hWnd, (HMENU)(pcb->id), hInst, NULL));

		//# PBL !!OWNER DRAW
		//�劲
		CPbCtrl* ppb = pPanelBase->pmainobjs->pb_syukan_on;
		ppb->set_wnd(CreateWindowW(TEXT("BUTTON"), ppb->txt.c_str(), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_PUSHLIKE| BS_OWNERDRAW,
			ppb->pt.X, ppb->pt.Y, ppb->sz.Width, ppb->sz.Height, hWnd, (HMENU)(ppb->id), hInst, NULL));
		pPanelBase->pmainobjs->lmp_syukan_on->set_ctrl(ppb);//�����v�Ƀ{�^���̃{�^���R���g���[�����Z�b�g
		ppb = pPanelBase->pmainobjs->pb_syukan_off;
		ppb->set_wnd(CreateWindowW(TEXT("BUTTON"), ppb->txt.c_str(), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_PUSHLIKE | BS_OWNERDRAW,
			ppb->pt.X, ppb->pt.Y, ppb->sz.Width, ppb->sz.Height, hWnd, (HMENU)(ppb->id), hInst, NULL));
		pPanelBase->pmainobjs->lmp_syukan_off->set_ctrl(ppb);//�����v�Ƀ{�^���̃{�^���R���g���[�����Z�b�g
		//Remote
		ppb = pPanelBase->pmainobjs->pb_remote;
		ppb->set_wnd(CreateWindowW(TEXT("BUTTON"), ppb->txt.c_str(), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_PUSHLIKE | BS_OWNERDRAW,
		ppb->pt.X, ppb->pt.Y, ppb->sz.Width, ppb->sz.Height, hWnd, (HMENU)(ppb->id), hInst, NULL));
		pPanelBase->pmainobjs->lmp_remote->set_ctrl(ppb);//�����v�Ƀ{�^���̃{�^���R���g���[�����Z�b�g
		
		//PAD MODE
		ppb = pPanelBase->pmainobjs->pb_pad_mode;
		ppb->set_wnd(CreateWindowW(TEXT("BUTTON"), ppb->txt.c_str(), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_PUSHLIKE | BS_OWNERDRAW,
			ppb->pt.X, ppb->pt.Y, ppb->sz.Width, ppb->sz.Height, hWnd, (HMENU)(ppb->id), hInst, NULL));
		pPanelBase->pmainobjs->lmp_pad_mode->set_ctrl(ppb);//�����v�Ƀ{�^���̃E�B���h�E�n���h�����Z�b�g

		//PB
		//�F��
		ppb = pPanelBase->pmainobjs->pb_auth;
		ppb->set_wnd(CreateWindowW(TEXT("BUTTON"), ppb->txt.c_str(), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_PUSHLIKE ,
			ppb->pt.X, ppb->pt.Y, ppb->sz.Width, ppb->sz.Height, hWnd, (HMENU)(ppb->id), hInst, NULL));
		//�[���ݒ�
		ppb = pPanelBase->pmainobjs->pb_ote_type_wnd;
		ppb->set_wnd(CreateWindowW(TEXT("BUTTON"), ppb->txt.c_str(), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_PUSHLIKE ,
			ppb->pt.X, ppb->pt.Y, ppb->sz.Width, ppb->sz.Height, hWnd, (HMENU)(ppb->id), hInst, NULL));
		//�A�V�X�g
		ppb=pPanelBase->pmainobjs->pb_assist_func;
		ppb->set_wnd(CreateWindowW(TEXT("BUTTON"), ppb->txt.c_str(), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_PUSHLIKE ,
			ppb->pt.X, ppb->pt.Y, ppb->sz.Width, ppb->sz.Height, hWnd, (HMENU)(ppb->id), hInst, NULL));
		//�N���[���I��
		ppb=pPanelBase->pmainobjs->pb_crane_sel_wnd;
		ppb->set_wnd(CreateWindowW(TEXT("BUTTON"), ppb->txt.c_str(), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_PUSHLIKE,
			ppb->pt.X, ppb->pt.Y, ppb->sz.Width, ppb->sz.Height, hWnd, (HMENU)(ppb->id), hInst, NULL));
		//�̏჊�Z�b�g
		ppb = pPanelBase->pmainobjs->pb_freset;
		ppb->set_wnd(CreateWindowW(TEXT("BUTTON"), ppb->txt.c_str(), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_PUSHLIKE | BS_OWNERDRAW,
			ppb->pt.X, ppb->pt.Y, ppb->sz.Width, ppb->sz.Height, hWnd, (HMENU)(ppb->id), hInst, NULL));
		pPanelBase->pmainobjs->lmp_freset->set_ctrl(ppb);//�����v�Ƀ{�^���̃{�^���R���g���[�����Z�b�g



		//RADIO BUTTON
		pcb = pPanelBase->pmainobjs->cb_disp_mode1;
		pcb->set_wnd(CreateWindowW(TEXT("BUTTON"), pcb->txt.c_str(), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_PUSHLIKE | BS_MULTILINE | WS_GROUP,
			pcb->pt.X, pcb->pt.Y, pcb->sz.Width, pcb->sz.Height, hWnd, (HMENU)(pcb->id), hInst, NULL));
		pcb = pPanelBase->pmainobjs->cb_disp_mode2;
		pcb->set_wnd(CreateWindowW(TEXT("BUTTON"), pcb->txt.c_str(), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_PUSHLIKE | BS_MULTILINE,
			pcb->pt.X, pcb->pt.Y, pcb->sz.Width, pcb->sz.Height, hWnd, (HMENU)(pcb->id), hInst, NULL));

		pcb = pPanelBase->pmainobjs->cb_opt_flt;
		pcb->set_wnd(CreateWindowW(TEXT("BUTTON"), pcb->txt.c_str(), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_PUSHLIKE | BS_MULTILINE |WS_GROUP,
			pcb->pt.X, pcb->pt.Y, pcb->sz.Width, pcb->sz.Height, hWnd, (HMENU)(pcb->id), hInst, NULL));
		pcb = pPanelBase->pmainobjs->cb_opt_set;
		pcb->set_wnd(CreateWindowW(TEXT("BUTTON"), pcb->txt.c_str(), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_PUSHLIKE | BS_MULTILINE,
			pcb->pt.X, pcb->pt.Y, pcb->sz.Width, pcb->sz.Height, hWnd, (HMENU)(pcb->id), hInst, NULL));
		pcb = pPanelBase->pmainobjs->cb_opt_com;
		pcb->set_wnd(CreateWindowW(TEXT("BUTTON"), pcb->txt.c_str(), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_PUSHLIKE | BS_MULTILINE,
			pcb->pt.X, pcb->pt.Y, pcb->sz.Width, pcb->sz.Height, hWnd, (HMENU)(pcb->id), hInst, NULL));
		pcb = pPanelBase->pmainobjs->cb_opt_cam;
		pcb->set_wnd(CreateWindowW(TEXT("BUTTON"), pcb->txt.c_str(), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_PUSHLIKE | BS_MULTILINE,
			pcb->pt.X, pcb->pt.Y, pcb->sz.Width, pcb->sz.Height, hWnd, (HMENU)(pcb->id), hInst, NULL));
		pcb = pPanelBase->pmainobjs->cb_opt_stat;
		pcb->set_wnd(CreateWindowW(TEXT("BUTTON"), pcb->txt.c_str(), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_PUSHLIKE | BS_MULTILINE,
			pcb->pt.X, pcb->pt.Y, pcb->sz.Width, pcb->sz.Height, hWnd, (HMENU)(pcb->id), hInst, NULL));
		pcb = pPanelBase->pmainobjs->cb_opt_clr;
		pcb->set_wnd(CreateWindowW(TEXT("BUTTON"), pcb->txt.c_str(), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_PUSHLIKE | BS_MULTILINE,
			pcb->pt.X, pcb->pt.Y, pcb->sz.Width, pcb->sz.Height, hWnd, (HMENU)(pcb->id), hInst, NULL));

		 //String ���񏑂����݃t���O���Z�b�g����TIMER�C�x���g�ŏ�������
		 is_initial_draw_mon1 = true;

		 //Lamp �E�B���h�E�n���h���Z�b�g,�t���b�J�ݒ�
		 //CC�Ƃ̒ʐM��ԕ\��
		 INT32 id_list[2] = { 4,0 };//2��t���b�J�@��/�Ð�
		 pPanelBase->pmainobjs->lmp_pcr->set_wnd(hWnd);		pPanelBase->pmainobjs->lmp_pcr->setup_flick(2, 3, id_list);
		 pPanelBase->pmainobjs->lmp_pcs->set_wnd(hWnd);		pPanelBase->pmainobjs->lmp_pcs->setup_flick(2, 3, id_list);
		 pPanelBase->pmainobjs->lmp_plcr->set_wnd(hWnd);	pPanelBase->pmainobjs->lmp_plcr->setup_flick(2, 3, id_list);
		 pPanelBase->pmainobjs->lmp_plcs->set_wnd(hWnd);	pPanelBase->pmainobjs->lmp_plcs->setup_flick(2, 3, id_list);

		//�\���X�V�p�^�C�}�[
		SetTimer(hWnd, OTE_SCAD_ID_MON1_TIMER, st_mon1.timer_ms, NULL);
		break;
	}
	case WM_COMMAND: {
		int wmId = LOWORD(wp);
		// �I�����ꂽ���j���[�̉��:
		switch (wmId)
		{
		case ID_MAIN_PNL_OBJ_CB_ESTOP: {
			if (pPanelBase->pmainobjs->cb_estop->get() == BST_CHECKED) {
				pPanelBase->pmainobjs->cb_estop->set(BST_UNCHECKED);
				pPanelBase->pmainobjs->lmp_estop->set(L_OFF);
			}
			else{ 
				pPanelBase->pmainobjs->cb_estop->set(BST_CHECKED);
				pPanelBase->pmainobjs->lmp_estop->set(L_ON);
			}
		}break;
		case ID_MAIN_PNL_OBJ_PB_SYUKAN_ON: {
			pPanelBase->pmainobjs->pb_syukan_on->update(true);
		}break;
		case ID_MAIN_PNL_OBJ_PB_SYUKAN_OFF: {
			pPanelBase->pmainobjs->pb_syukan_off->update(true);
		}break;
		case ID_MAIN_PNL_OBJ_PB_REMOTE: {
			pPanelBase->pmainobjs->pb_remote->update(true);
		}break;
		case ID_MAIN_PNL_OBJ_PB_PAD_MODE: {

			pPanelBase->pmainobjs->pb_pad_mode->update(true);
		}break;
		case ID_MAIN_PNL_OBJ_PB_ASSIST_FUNC: {
			pPanelBase->pmainobjs->pb_assist_func->update(true);
		}break;
		case ID_MAIN_PNL_OBJ_PB_OTE_TYPE_WND: {
			pPanelBase->pmainobjs->pb_ote_type_wnd->update(true);
		}break;
		case ID_MAIN_PNL_OBJ_PB_CRANE_SEL_WND: {
			pPanelBase->pmainobjs->pb_crane_sel_wnd->update(true);
		}break;
		case ID_MAIN_PNL_OBJ_PB_AUTH: {
			pPanelBase->pmainobjs->pb_auth->update(true);
		}break;
		case ID_MAIN_PNL_OBJ_CB_PNL_NOTCH: {
			pPanelBase->pmainobjs->cb_pnl_notch->update();
		}break;

		case ID_MAIN_PNL_OBJ_RDO_DISP_MODE1:
		case ID_MAIN_PNL_OBJ_RDO_DISP_MODE2:
		{
			pPanelBase->pmainobjs->rdo_disp_mode->update(true);
		}break;

		case ID_MAIN_PNL_OBJ_RDO_OPT_WND_FLT:
		case ID_MAIN_PNL_OBJ_RDO_OPT_WND_SET: 
		case ID_MAIN_PNL_OBJ_RDO_OPT_WND_COM :
		case ID_MAIN_PNL_OBJ_RDO_OPT_WND_CAM :
		case ID_MAIN_PNL_OBJ_RDO_OPT_WND_STAT:
		{
			HINSTANCE hInst = (HINSTANCE)GetModuleHandle(0);

			if (psubpanel == NULL) {
				psubpanel = new CSubPanelWindow(hInst, hWnd, pCrane->get_id(), wmId, pPanelBase);
			}
			else {
				if (psubpanel->wnd_code == wmId) {
					delete psubpanel;
					psubpanel = NULL;
				}
				else{
					delete psubpanel;
					psubpanel = new CSubPanelWindow(hInst, hWnd, pCrane->get_id(), wmId, pPanelBase);
				}
			}

			InvalidateRect(hWnd, NULL, TRUE); // �E�B���h�E�S�̂��ĕ`��

		}break;
		case ID_MAIN_PNL_OBJ_RDO_OPT_WND_CLR :
		{
			delete psubpanel;
			psubpanel = NULL;
			InvalidateRect(hWnd, NULL, TRUE); // �E�B���h�E�S�̂��ĕ`��
		}break;

		case ID_MAIN_PNL_OBJ_PB_FRESET: {
			pPanelBase->pmainobjs->pb_freset->update(true);
		}break;

		default:
			return DefWindowProc(hWnd, msg, wp, lp);
		}
	}break;

	case WM_LBUTTONUP: {//�}�E�X���{�^�������Ń��j�^�E�B���h�E�`��X�V
		InvalidateRect(hWnd, NULL, TRUE); // �E�B���h�E�S�̂��ĕ`��
	}
	case WM_CTLCOLORSTATIC: {//�X�^�e�B�b�N�e�L�X�g�̐F�Z�b�g
		SetTextColor((HDC)wp, RGB(220, 220, 220)); // ���C�g�O���[
		SetBkMode((HDC)wp, TRANSPARENT);
	}return (LRESULT)GetStockObject(NULL_BRUSH); // �w�i�F�ɍ��킹��

	case WM_ERASEBKGND: {//�E�B���h�E�̔w�i�F���O���[��
		pPanelBase->pmainobjs->pgraphic->FillRectangle(pPanelBase->pmainobjs->pBrushBk,pPanelBase->pmainobjs->rc_panel);

	}return 1; // �w�i�������������Ƃ�����

	case WM_TIMER: {
		//# LAMP(CTRL)�X�V
		//e-stop : PLC�̔F����ESTOP�̎��g�L�\��
		INT16 code = pOteCCIf->st_msg_pc_u_rcv.body.st.lamp[OTE_PNL_CTRLS::estop].code;//����PC��M�o�b�t�@�̎w�ߓ��e
		int val = pPanelBase->pmainobjs->cb_estop->get();									//CB�̏��
		//CB��ԂŐ���PC��ON�\��������Ε\���摜��ؑ�
		if (val == BST_CHECKED) { if (code) val = 3; }
		else {if (code)val = 2;}
		pPanelBase->pmainobjs->lmp_estop->set(val);
		pPanelBase->pmainobjs->lmp_estop->update();
		
		//�劲
		code = pOteCCIf->st_msg_pc_u_rcv.body.st.lamp[OTE_PNL_CTRLS::syukan_on].code;
		pPanelBase->pmainobjs->lmp_syukan_on->set(code); pPanelBase->pmainobjs->lmp_syukan_on->update();
		code = pOteCCIf->st_msg_pc_u_rcv.body.st.lamp[OTE_PNL_CTRLS::syukan_off].code;
		pPanelBase->pmainobjs->lmp_syukan_off->set(code); pPanelBase->pmainobjs->lmp_syukan_off->update();

		//���u
		pPanelBase->pmainobjs->lmp_remote->set(pOteCsInf->st_body.remote);
		pPanelBase->pmainobjs->lmp_remote->update();

		pPanelBase->pmainobjs->lmp_pad_mode->set(pOteCsInf->st_body.game_pad_mode);
		pPanelBase->pmainobjs->lmp_pad_mode->update();

		//�̏჊�Z�b�g
	//	pPanelBase->pmainobjs->lmp_freset->set(pOteCsInf->st_body.ctrl_ope[OTE_PNL_CTRLS::fault_reset]);
		if(st_work.ctrl_stat[OTE_PNL_CTRLS::fault_reset])
			pPanelBase->pmainobjs->lmp_freset->set(L_ON);
		else 
			pPanelBase->pmainobjs->lmp_freset->set(L_OFF);

		pPanelBase->pmainobjs->lmp_freset->update();


		//# SwitchImg�X�V(�����v�j
		//CC�Ƃ̒ʐM��ԕ\��(��M�j
		if(pOteCCIf->cc_com_stat_r == ID_PNL_SOCK_STAT_ACT_RCV) 
			pPanelBase->pmainobjs->lmp_pcr->set(ID_PANEL_LAMP_FLICK);
		else pPanelBase->pmainobjs->lmp_pcr->set(pOteCCIf->cc_com_stat_r);
		pPanelBase->pmainobjs->lmp_pcr->update();
		//CC�Ƃ̒ʐM��ԕ\��(���M�j
		if (pOteCCIf->cc_com_stat_s == ID_PNL_SOCK_STAT_ACT_SND)
			pPanelBase->pmainobjs->lmp_pcs->set(ID_PANEL_LAMP_FLICK);
		else pPanelBase->pmainobjs->lmp_pcs->set(pOteCCIf->cc_com_stat_s);
		pPanelBase->pmainobjs->lmp_pcs->update();
		//PLC�Ƃ̒ʐM��ԕ\��(��M�j
		pPanelBase->pmainobjs->lmp_plcr->update();
		//PLC�Ƃ̒ʐM��ԕ\��(���M�j
		pPanelBase->pmainobjs->lmp_plcs->update();
		
		//PB��ԍX�V(�I�t�f�B���C�J�E���g�_�E��)
		pPanelBase->pmainobjs->pb_syukan_on->update(false);
		pPanelBase->pmainobjs->pb_syukan_off->update(false);
		pPanelBase->pmainobjs->pb_remote->update(false);
		pPanelBase->pmainobjs->pb_auth->update(false);
		pPanelBase->pmainobjs->pb_assist_func->update(false);
		pPanelBase->pmainobjs->pb_crane_sel_wnd->update(false);
		pPanelBase->pmainobjs->pb_ote_type_wnd->update(false);
		pPanelBase->pmainobjs->pb_pad_mode->update(false);
		pPanelBase->pmainobjs->pb_freset->update(false);
		
		//String�X�V
		if (is_initial_draw_mon1) {
			pPanelBase->pmainobjs->str_message->update();
			pPanelBase->pmainobjs->str_pc_com_stat->update();
			pPanelBase->pmainobjs->str_plc_com_stat->update();
		//	is_initial_draw_mon1 = false;
		}

		//GamePad�`�F�b�N
		if(pOteCsInf->gpad_in.syukan_on) pPanelBase->pmainobjs->pb_syukan_on->update(true);
		if (pOteCsInf->gpad_in.syukan_off)pPanelBase->pmainobjs->pb_syukan_off->update(true);
		if (pOteCsInf->gpad_in.remote)pPanelBase->pmainobjs->pb_remote->update(true);
		if (pOteCsInf->gpad_in.estop)pPanelBase->pmainobjs->cb_estop->set(BST_CHECKED);
		if (pOteCsInf->gpad_in.f_reset) {//�Q�[���p�b�h�̔���~�N���A��ResetPB�Ŏ��s
			pPanelBase->pmainobjs->pb_freset->update(true);
			pPanelBase->pmainobjs->cb_estop->set(BST_UNCHECKED);
		}
		
	}break;

	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);

		//PatBlt(hdc, 0, 0, OTE_SCAD_MON1_WND_W, OTE_SCAD_MON1_WND_H, RGB(64, 64, 64));

		EndPaint(hWnd, &ps);
	}break;
	case WM_DRAWITEM: {//�����v�\�����X�V TIMER�C�x���g�ŏ�ԕω��`�F�b�N����InvalidiateRect�ŌĂяo��
		DRAWITEMSTRUCT* pDIS = (DRAWITEMSTRUCT*)lp;
		Image* image;
		Graphics gra(pDIS->hDC); 
		Font* pfont =NULL;
		CMainPanelObj* pos= pPanelBase->pmainobjs;
		CLampCtrl* plamp = NULL;

		if (pDIS->CtlID == pos->cb_estop->id) {
			plamp = pos->lmp_estop;	pfont = NULL;
		}
		else if (pDIS->CtlID == pos->pb_remote->id) {//�����[�g�����v
			plamp = pos->lmp_remote;pfont = plamp->pFont;
		}
		else if (pDIS->CtlID == pos->pb_syukan_on->id) {//�劲�������v
			plamp = pos->lmp_syukan_on; pfont = plamp->pFont;
		}
		else if (pDIS->CtlID == pos->pb_syukan_off->id) {//�劲�؃����v
			plamp = pos->lmp_syukan_off; pfont = plamp->pFont;
		}
		else if (pDIS->CtlID == pos->pb_pad_mode->id) {//PAD�����v
			plamp = pos->lmp_pad_mode; pfont = plamp->pFont;
		}
		else if (pDIS->CtlID == pos->pb_freset->id) {//FAULT�����v
			plamp = pos->lmp_freset; pfont = plamp->pFont;
		}
		else return false;

		image = plamp->pimg[plamp->get()];
		gra.FillRectangle(pPanelBase->pmainobjs->pBrushBk, plamp->rc);											//�w�i�F�Z�b�g
		if (image) gra.DrawImage(image, plamp->rc);															//�C���[�W�`��
		if (pfont != NULL) 
			gra.DrawString(plamp->txt.c_str(), -1, pfont, plamp->frc, plamp->pStrFormat, plamp->pTxtBrush);	//�e�L�X�g�`��

	}return true;

	case WM_DESTROY: {
		st_mon1.hwnd_mon = NULL;
		KillTimer(hWnd, OTE_SCAD_ID_MON1_TIMER);
		//### �I�[�v�j���O��ʂ��ĕ\��
		pEnvObj->open_opening_window();

	}break;
	default:
		return DefWindowProc(hWnd, msg, wp, lp);
	}
	return S_OK;
};

//static wostringstream mon2wos;
LRESULT CALLBACK COteScad::Mon2Proc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
	switch (msg)
	{
	case WM_CREATE: {
		InitCommonControls();//�R�����R���g���[��������
		HINSTANCE hInst = (HINSTANCE)GetModuleHandle(0);
		//�E�B���h�E�ɃR���g���[���ǉ�
		//STATIC,LABEL
		for (int i = OTE_SCAD_ID_MON2_STATIC_UNI; i <= OTE_SCAD_ID_MON2_STATIC_MSG; i++) {
			st_mon2.hctrl[i] = CreateWindowW(TEXT("STATIC"), st_mon2.text[i], WS_CHILD | WS_VISIBLE | SS_LEFT,
				st_mon2.pt[i].x, st_mon2.pt[i].y, st_mon2.sz[i].cx, st_mon2.sz[i].cy,
				hWnd, (HMENU)(OTE_SCAD_ID_MON2_CTRL_BASE + i), hInst, NULL);
		}
		//RADIO PB
		for (int i = OTE_SCAD_ID_MON2_RADIO_RCV; i <= OTE_SCAD_ID_MON2_RADIO_INFO; i++) {
			if (i == OTE_SCAD_ID_MON2_RADIO_RCV) {
				st_mon2.hctrl[i] = CreateWindowW(TEXT("BUTTON"), st_mon2.text[i], WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_PUSHLIKE | WS_GROUP,
					st_mon2.pt[i].x, st_mon2.pt[i].y, st_mon2.sz[i].cx, st_mon2.sz[i].cy,
					hWnd, (HMENU)(OTE_SCAD_ID_MON2_CTRL_BASE + i), hInst, NULL);

				st_mon2.sock_inf_id = OTE_SCAD_ID_MON2_RADIO_RCV;
				SendMessage(st_mon2.hctrl[i], BM_SETCHECK, BST_CHECKED, 0L);
			}
			else
				st_mon2.hctrl[i] = CreateWindowW(TEXT("BUTTON"), st_mon2.text[i], WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_PUSHLIKE,
					st_mon2.pt[i].x, st_mon2.pt[i].y, st_mon2.sz[i].cx, st_mon2.sz[i].cy,
					hWnd, (HMENU)(OTE_SCAD_ID_MON2_CTRL_BASE + i), hInst, NULL);
		}

		UINT rtn = SetTimer(hWnd, OTE_SCAD_ID_MON2_TIMER, OTE_SCAD_PRM_MON2_TIMER_MS, NULL);
	}break;
	case WM_COMMAND: {
		int wmId = LOWORD(wp);
		// �I�����ꂽ���j���[�̉��:
		int _Id = wmId - OTE_SCAD_ID_MON2_CTRL_BASE;
		// �I�����ꂽ���j���[�̉��:
		switch (_Id)
		{
		case OTE_SCAD_ID_MON2_RADIO_RCV: {
			st_mon2.sock_inf_id = OTE_SCAD_ID_MON2_RADIO_RCV;
		}break;
		case OTE_SCAD_ID_MON2_RADIO_SND: {
			st_mon2.sock_inf_id = OTE_SCAD_ID_MON2_RADIO_SND;
		}break;
		case OTE_SCAD_ID_MON2_RADIO_INFO: {
			st_mon2.sock_inf_id = OTE_SCAD_ID_MON2_RADIO_INFO;
		}break;
		default:
			return DefWindowProc(hWnd, msg, wp, lp);
		}break;
	}
	case WM_TIMER: {

	}break;
	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
	}break;
	case WM_DESTROY: {
		KillTimer(hWnd, OTE_SCAD_ID_MON2_TIMER);
		st_mon2.hwnd_mon = NULL;
	}break;
	default:
		return DefWindowProc(hWnd, msg, wp, lp);
	}

	return S_OK;
}

HWND COteScad::open_monitor_wnd(HWND h_parent_wnd, int id) {

	InitCommonControls();//�R�����R���g���[��������
	HINSTANCE hInst = GetModuleHandle(0);

	WNDCLASSEXW wcex;
//	ATOM fb = RegisterClassExW(&wcex);

	if (id == BC_ID_MON1) {
#if 1
		CMainPanelWindow* pMainWnd = new CMainPanelWindow(
			hInst, h_parent_wnd,
			pOteEnvInf->selected_crane,
			pOteEnvInf->selected_crane,
			&pPanelBase
		);

		
		SetLayeredWindowAttributes(pMainWnd->hWnd, 0, 200, LWA_ALPHA);
		
		//show_monitor_wnd(id);

		ShowWindow(pMainWnd->hWnd, SW_SHOW);
		UpdateWindow(pMainWnd->hWnd);
		st_mon1.is_monitor_active = true;

		wos.str(L"");
		if (pMainWnd->hWnd != NULL) wos << L"Succeed CRANE ID:"<< pPanelBase->get_crane_id() << L" Open";
		else                          wos << L"!! Failed  CRANE ID: << pPanelBase->get_crane_id()";
		msg2listview(wos.str());

		return pMainWnd->hWnd;
#else
		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = Mon1Proc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = hInst;
		wcex.hIcon = NULL;
		wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wcex.lpszMenuName = TEXT("OTE_SCAD_MON1");
		wcex.lpszClassName = TEXT("OTE_SCAD_MON1");
		wcex.hIconSm = NULL;

		ATOM fb = RegisterClassExW(&wcex);

		st_mon1.hwnd_mon = CreateWindowW(TEXT("OTE_SCAD_MON1"), TEXT("Operation Panel"), WS_OVERLAPPEDWINDOW,
			OTE_SCAD_MON1_WND_X, OTE_SCAD_MON1_WND_Y, OTE_SCAD_MON1_WND_W, OTE_SCAD_MON1_WND_H,
			h_parent_wnd, nullptr, hInst, nullptr);

		SetLayeredWindowAttributes(st_mon1.hwnd_mon, 0, 200, LWA_ALPHA);

		show_monitor_wnd(id);

		wos.str(L"");
		if (st_mon1.hwnd_mon != NULL) wos << L"Succeed : MON1 open";
		else                          wos << L"!! Failed : MON1 open";
		msg2listview(wos.str());

		return st_mon1.hwnd_mon;


#endif
	}
	else if (id == BC_ID_MON2) {//�ʐM�p�E�B���h�E
		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = Mon2Proc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = hInst;
		wcex.hIcon = NULL;
		wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wcex.lpszMenuName = TEXT("OTE_SCAD_MON2");
		wcex.lpszClassName = TEXT("OTE_SCAD_MON2");
		wcex.hIconSm = NULL;

		ATOM fb = RegisterClassExW(&wcex);

		st_mon2.hwnd_mon = CreateWindowW(TEXT("OTE_SCAD_MON2"), TEXT("OTE_SCAD_MON2"), WS_OVERLAPPEDWINDOW,
			OTE_SCAD_MON2_WND_X, OTE_SCAD_MON2_WND_Y, OTE_SCAD_MON2_WND_W, OTE_SCAD_MON2_WND_H,
			h_parent_wnd, nullptr, hInst, nullptr);

		//show_monitor_wnd(id);MON2�̓I�[�v�����\�����Ȃ�

		wos.str(L"");
		if (st_mon2.hwnd_mon != NULL) wos << L"Succeed : MON2 open";
		else                          wos << L"!! Failed : MON2 open";
		msg2listview(wos.str());


		return st_mon2.hwnd_mon;
	}
	else
	{
		return NULL;
	};

	return NULL;
}
void COteScad::close_monitor_wnd(int id) {
	wos.str(L"");
	if (id == BC_ID_MON1) {
		DestroyWindow(st_mon1.hwnd_mon);
		wos << L"MON1 closed";
	}
	else if (id == BC_ID_MON2) {
		DestroyWindow(st_mon2.hwnd_mon);
		wos << L"MON2 closed";
	}
	else;
	msg2listview(wos.str());
	return;
}
void COteScad::show_monitor_wnd(int id) {
	if ((id == BC_ID_MON1) && (st_mon1.hwnd_mon != NULL)) {
		ShowWindow(st_mon1.hwnd_mon, SW_SHOW);
		UpdateWindow(st_mon1.hwnd_mon);
		st_mon1.is_monitor_active = true;
	}
	else if ((id == BC_ID_MON2) && (st_mon2.hwnd_mon != NULL)) {
		ShowWindow(st_mon2.hwnd_mon, SW_SHOW);
		UpdateWindow(st_mon2.hwnd_mon);
		st_mon2.is_monitor_active = true;
	}
	else;
	return;
}
void COteScad::hide_monitor_wnd(int id) {
	if ((id == BC_ID_MON1) && (st_mon1.hwnd_mon != NULL)) {
		ShowWindow(st_mon1.hwnd_mon, SW_HIDE);
		st_mon1.is_monitor_active = false;
	}
	else if ((id == BC_ID_MON2) && (st_mon2.hwnd_mon != NULL)) {
		ShowWindow(st_mon2.hwnd_mon, SW_HIDE);
		st_mon2.is_monitor_active = false;
	}
	else;
	return;
}

///###	�^�u�p�l����ListView�Ƀ��b�Z�[�W���o��
void COteScad::msg2listview(wstring wstr) {

	const wchar_t* pwc; pwc = wstr.c_str();

	inf.hwnd_msglist = GetDlgItem(inf.hwnd_opepane, IDC_LIST1);
	LVITEM item;

	item.mask = LVIF_TEXT;
	item.pszText = (wchar_t*)pwc;								// �e�L�X�g
	item.iItem = inf.panel_msglist_count % BC_LISTVIEW_ROW_MAX;	// �ԍ�
	item.iSubItem = 1;											// �T�u�A�C�e���̔ԍ�
	ListView_SetItem(inf.hwnd_msglist, &item);

	SYSTEMTIME st; TCHAR tbuf[32];
	::GetLocalTime(&st);
	wsprintf(tbuf, L"%02d:%02d:%02d.%01d", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds / 100);

	item.pszText = tbuf;   // �e�L�X�g
	item.iSubItem = 0;											// �T�u�A�C�e���̔ԍ�
	ListView_SetItem(inf.hwnd_msglist, &item);

	//InvalidateRect(inf.hWnd_msgList, NULL, TRUE);
	inf.panel_msglist_count++;
	return;
}
void COteScad::set_PNLparam_value(float p1, float p2, float p3, float p4, float p5, float p6) {
	wstring wstr;
	wstr += std::to_wstring(p1); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT1), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p2); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT2), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p3); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT3), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p4); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT4), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p5); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT5), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p6); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT6), wstr.c_str());
}
//�^�u�p�l����Edit Box�����e�L�X�g��ݒ�
void COteScad::set_panel_tip_txt() {
	wstring wstr_type; wstring wstr;
	switch (inf.panel_func_id) {
	case IDC_TASK_FUNC_RADIO4: {
		wstr = L"1:-";
		SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_STATIC_ITEM3), wstr.c_str());
		wstr = L"2:-";
		SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_STATIC_ITEM4), wstr.c_str());
		wstr = L"3:-";
		SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_STATIC_ITEM5), wstr.c_str());
		wstr = L"4:-";
		SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_STATIC_ITEM6), wstr.c_str());
		wstr = L"5:-";
		SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_STATIC_ITEM7), wstr.c_str());
		wstr = L"6:-";
		SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_STATIC_ITEM8), wstr.c_str());
	}break;
	case IDC_TASK_FUNC_RADIO1:
	case IDC_TASK_FUNC_RADIO2:
	case IDC_TASK_FUNC_RADIO3:
	case IDC_TASK_FUNC_RADIO5:
	case IDC_TASK_FUNC_RADIO6:
	default:
	{
		wstr = L"1:-";
		SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_STATIC_ITEM3), wstr.c_str());
		wstr = L"2:-";
		SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_STATIC_ITEM4), wstr.c_str());
		wstr = L"3:-";
		SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_STATIC_ITEM5), wstr.c_str());
		wstr = L"4:-";
		SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_STATIC_ITEM6), wstr.c_str());
		wstr = L"5:-";
		SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_STATIC_ITEM7), wstr.c_str());
		wstr = L"6:-";
		SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_STATIC_ITEM8), wstr.c_str());
	}break;
	}
	return;
}
//�^�u�p�l����Function�{�^����Static�e�L�X�g��ݒ�
void COteScad::set_func_pb_txt() {
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO1, L"MKCC IF");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO2, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO3, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO4, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO5, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO6, L"-");
	return;
}
//�^�u�p�l����Item chk�e�L�X�g��ݒ�
void COteScad::set_item_chk_txt() {
	wstring wstr_type; wstring wstr;
	switch (inf.panel_func_id) {
	case IDC_TASK_FUNC_RADIO1: {
		SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK1, L"WND");
		SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK2, L"-");
		SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK3, L"-");
		SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK4, L"-");
		SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK5, L"-");
		SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK6, L"-");
	}break;

	case IDC_TASK_FUNC_RADIO2:
	case IDC_TASK_FUNC_RADIO3:
	case IDC_TASK_FUNC_RADIO4:
	case IDC_TASK_FUNC_RADIO5:
	case IDC_TASK_FUNC_RADIO6:
	default:
		SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK1, L"-");
		SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK2, L"-");
		SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK3, L"-");
		SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK4, L"-");
		SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK5, L"-");
		SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK6, L"-");
		break;
	}
	return;
}
