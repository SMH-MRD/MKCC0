#include "COpePanel.h"
#include "CPanelWindow.h"
#include "CPanelObj.h"
#include <commctrl.h>       //�R�����R���g���[���p
#include "CBasicControl.h"
#include <vector>
#include "COteEnv.h"

extern vector<CBasicControl*>	VectCtrlObj;

static COteEnv* pEnvObj;

HWND CMainPanelWindow::hWnd;
HWND CMainPanelWindow::hParentWnd;
CPanelBase* CMainPanelWindow::pPanelBase;
int CMainPanelWindow::crane_id;
int CMainPanelWindow::wnd_code;

LPST_OTE_UI CMainPanelWindow::pUi;
LPST_OTE_CS_INF CMainPanelWindow::pCsInf;
LPST_OTE_CC_IF CMainPanelWindow::pCcIf;

CSubPanelWindow* CMainPanelWindow::pSubPanelWnd;

CMainPanelWindow::CMainPanelWindow(HINSTANCE hInstance, HWND hParent, int _crane_id, int _wnd_code, CPanelBase* _pPanelBase) {
	pPanelBase = _pPanelBase;
	crane_id = _crane_id;
	wnd_code = _wnd_code;
	hParentWnd = hParent;

	const wchar_t CLASS_NAME[] =	 L"MainWindowClass";
	const wchar_t CLASS_NAME_HHGH29[] = L"MainWindowHHGH29";

	const wchar_t* pClassName;

	WNDCLASS wc = { };
	switch (crane_id) {
	case CARNE_ID_HHGH29:
		wc.lpfnWndProc = WndProcHHGH29;
		wc.hInstance = hInstance;
		wc.lpszClassName = pClassName = CLASS_NAME_HHGH29;
		break;
	default:
		wc.lpfnWndProc = WndProc;
		wc.hInstance = hInstance;
		wc.lpszClassName = pClassName = CLASS_NAME;
		break;
	}
	RegisterClass(&wc);

	hWnd = CreateWindowW(pClassName, TEXT("Operation Panel"), WS_OVERLAPPEDWINDOW,
		MAIN_PNL_WND_X, MAIN_PNL_WND_Y, MAIN_PNL_WND_W, MAIN_PNL_WND_H,
		hParentWnd , nullptr, hInstance, nullptr);

	//hWnd = CreateWindowEx(
	//	0,																// Optional window styles
	//	pClassName,														// Window class
	//	L"���C���E�B���h�E",												// Window text
	//	WS_CHILD | WS_BORDER | WS_THICKFRAME | WS_CAPTION,               // Window style
	//	SUB_PNL_WND_X, SUB_PNL_WND_Y, SUB_PNL_WND_W, SUB_PNL_WND_H,
	//	hParent, nullptr, hInstance, nullptr
	//);

	if (hWnd) {
		ShowWindow(hWnd, SW_SHOW);
		UpdateWindow(hWnd);
	}
}
CMainPanelWindow::~CMainPanelWindow()
{
	close();
}
int CMainPanelWindow::close()
{
	DestroyWindow(hWnd);
	return 0;
}


HWND CSubPanelWindow::hWnd;
HWND CSubPanelWindow::hParentWnd; 
CPanelBase* CSubPanelWindow::pPanelBase;
int CSubPanelWindow::crane_id;
int CSubPanelWindow::wnd_code;

LPST_OTE_UI CSubPanelWindow::pUi;
LPST_OTE_CS_INF CSubPanelWindow::pCsInf;
LPST_OTE_CC_IF CSubPanelWindow::pCcIf;

CSubPanelWindow::CSubPanelWindow(HINSTANCE hInstance, HWND hParent, int _crane_id, int wnd_code, CPanelBase* _pPanelBase) {
	pPanelBase = _pPanelBase;
	hParentWnd = hParent;

    const wchar_t CLASS_NAME[] = L"SubWindowClass";
    const wchar_t CLASS_NAME_FLT[] = L"SubWindowFltClass";
    const wchar_t CLASS_NAME_SET[] = L"SubWindowSetClass";
    const wchar_t CLASS_NAME_COM[] = L"SubWindowComClass";
    const wchar_t CLASS_NAME_CAM[] = L"SubWindowCamClass";
    const wchar_t CLASS_NAME_STAT[] = L"SubWindowStatClass";

    const wchar_t* pClassName;

    WNDCLASS wc = { };
	switch (wnd_code) {
	case ID_MAIN_PNL_OBJ_RDO_OPT_WND_FLT:
        wc.lpfnWndProc = WndProcFlt;
        wc.hInstance = hInstance;
        wc.lpszClassName = pClassName = CLASS_NAME_FLT;
		break;
	case ID_MAIN_PNL_OBJ_RDO_OPT_WND_SET:
        wc.lpfnWndProc = WndProcSet;
        wc.hInstance = hInstance;
        wc.lpszClassName = pClassName = CLASS_NAME_SET;
		break;
	case ID_MAIN_PNL_OBJ_RDO_OPT_WND_COM:
        wc.lpfnWndProc = WndProcCom;
        wc.hInstance = hInstance;
        wc.lpszClassName = pClassName = CLASS_NAME_COM;
		break;
	case ID_MAIN_PNL_OBJ_RDO_OPT_WND_CAM:
        wc.lpfnWndProc = WndProcCam;
        wc.hInstance = hInstance;
        wc.lpszClassName = pClassName = CLASS_NAME_CAM;
		break;
	case ID_MAIN_PNL_OBJ_RDO_OPT_WND_STAT:
        wc.lpfnWndProc = WndProcStat;
        wc.hInstance = hInstance;
        wc.lpszClassName = pClassName = CLASS_NAME_STAT;
		break;
	default:
        wc.lpfnWndProc = pSubWndProc;
        wc.hInstance = hInstance;
        wc.lpszClassName = pClassName = CLASS_NAME;
		break;
	}
    RegisterClass(&wc);

    hWnd = CreateWindowEx(
        0,																// Optional window styles
        pClassName,														// Window class
        L"�T�u�E�B���h�E",												// Window text
        WS_CHILD | WS_BORDER | WS_THICKFRAME | WS_CAPTION,               // Window style
        SUB_PNL_WND_X, SUB_PNL_WND_Y, SUB_PNL_WND_W, SUB_PNL_WND_H,
        hParent, nullptr, hInstance, nullptr
    );

    if (hWnd) {
        ShowWindow(hWnd, SW_SHOW);
        UpdateWindow(hWnd);
    }
}
CSubPanelWindow::~CSubPanelWindow()
{
    close();
}
int CSubPanelWindow::close()
{
	DestroyWindow(hWnd); 
	return 0;
}


LRESULT CALLBACK CSubPanelWindow::WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CREATE: {
        SetWindowText(hwnd, L"DefaultWnd");
    }break;

    case WM_DESTROY:
        // PostQuitMessage(0);
        return 0;
    case WM_CLOSE:
        DestroyWindow(hwnd);
        return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
LRESULT CALLBACK CSubPanelWindow::WndProcFlt(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CREATE: {
        SetWindowText(hwnd, L"�̏�");
    }break;

    case WM_DESTROY:
        // PostQuitMessage(0);
        return 0;
    case WM_CLOSE:
        DestroyWindow(hwnd);
        return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
LRESULT CALLBACK CSubPanelWindow::WndProcSet(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
#if 1	
	switch (uMsg) {
	case WM_CREATE: {
		SetWindowText(hwnd, L"�ݒ�");
	}break;

	case WM_DESTROY:
		// PostQuitMessage(0);
		return 0;
	case WM_CLOSE:
		DestroyWindow(hwnd);
		return 0;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
#else
	switch (uMsg)
	{

	case WM_CREATE: {
		InitCommonControls();//�R�����R���g���[��������
		HINSTANCE hInst = (HINSTANCE)GetModuleHandle(0);

		//�E�B���h�E�ɃR���g���[���ǉ�
		int i;

		//RADIO BUTTON
		CCbCtrl* pcb = pSubPanelBase->pobjs->cb_mh_spd_mode0;
		pcb->set_wnd(CreateWindowW(TEXT("BUTTON"), pcb->txt.c_str(), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_PUSHLIKE | BS_MULTILINE | WS_GROUP,
			pcb->pt.X, pcb->pt.Y, pcb->sz.Width, pcb->sz.Height, hWnd, (HMENU)(pcb->id), hInst, NULL));
		pcb = pSubPanelBase->pobjs->cb_mh_spd_mode1;
		pcb->set_wnd(CreateWindowW(TEXT("BUTTON"), pcb->txt.c_str(), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_PUSHLIKE | BS_MULTILINE,
			pcb->pt.X, pcb->pt.Y, pcb->sz.Width, pcb->sz.Height, hWnd, (HMENU)(pcb->id), hInst, NULL));
		pcb = pSubPanelBase->pobjs->cb_mh_spd_mode2;
		pcb->set_wnd(CreateWindowW(TEXT("BUTTON"), pcb->txt.c_str(), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_PUSHLIKE | BS_MULTILINE | WS_GROUP,
			pcb->pt.X, pcb->pt.Y, pcb->sz.Width, pcb->sz.Height, hWnd, (HMENU)(pcb->id), hInst, NULL));

		pcb = pSubPanelBase->pobjs->cb_bh_r_mode0;
		pcb->set_wnd(CreateWindowW(TEXT("BUTTON"), pcb->txt.c_str(), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_PUSHLIKE | BS_MULTILINE | WS_GROUP,
			pcb->pt.X, pcb->pt.Y, pcb->sz.Width, pcb->sz.Height, hWnd, (HMENU)(pcb->id), hInst, NULL));
		pcb = pSubPanelBase->pobjs->cb_bh_r_mode1;
		pcb->set_wnd(CreateWindowW(TEXT("BUTTON"), pcb->txt.c_str(), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_PUSHLIKE | BS_MULTILINE,
			pcb->pt.X, pcb->pt.Y, pcb->sz.Width, pcb->sz.Height, hWnd, (HMENU)(pcb->id), hInst, NULL));
		pcb = pSubPanelBase->pobjs->cb_bh_r_mode2;
		pcb->set_wnd(CreateWindowW(TEXT("BUTTON"), pcb->txt.c_str(), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_PUSHLIKE | BS_MULTILINE | WS_GROUP,
			pcb->pt.X, pcb->pt.Y, pcb->sz.Width, pcb->sz.Height, hWnd, (HMENU)(pcb->id), hInst, NULL));

		//Lamp �E�B���h�E�n���h���Z�b�g
		pSubPanelBase->pobjs->lmp_mh_spd_mode->set_wnd(hWnd);
		pSubPanelBase->pobjs->lmp_bh_r_mode->set_wnd(hWnd);

		//�\���X�V�p�^�C�}�[
		SetTimer(hWnd, ID_SUB_PANEL_TIMER, ID_SUB_PANEL_TIMER_MS, NULL);
		break;
	}
	case WM_COMMAND: {
		int wmId = LOWORD(wParam);
		// �I�����ꂽ���j���[�̉��:
		switch (wmId)
		{
		case ID_SUB_PNL_SET_OBJ_RDO_MHSPD_0: {

		}break;
		case ID_SUB_PNL_SET_OBJ_RDO_MHSPD_1: {
		}break;
		case ID_SUB_PNL_SET_OBJ_RDO_MHSPD_2: {
		}break;
		case ID_SUB_PNL_SET_OBJ_RDO_BHR_0: {
		}break;
		case ID_SUB_PNL_SET_OBJ_RDO_BHR_1: {
		}break;
		case ID_SUB_PNL_SET_OBJ_RDO_BHR_2: {
		}break;
		

		case ID_MAIN_PNL_OBJ_CB_ESTOP: {
			if (pPanelBase->pobjs->cb_estop->get() == BST_CHECKED) {
				pPanelBase->pobjs->cb_estop->set(BST_UNCHECKED);
				pPanelBase->pobjs->lmp_estop->set(L_OFF);
			}
			else {
				pPanelBase->pobjs->cb_estop->set(BST_CHECKED);
				pPanelBase->pobjs->lmp_estop->set(L_ON);
			}
		}break;
		case ID_MAIN_PNL_OBJ_PB_SYUKAN_ON: {
			pPanelBase->pobjs->pb_syukan_on->update(true);
		}break;
		case ID_MAIN_PNL_OBJ_PB_SYUKAN_OFF: {
			pPanelBase->pobjs->pb_syukan_off->update(true);
		}break;
		case ID_MAIN_PNL_OBJ_PB_REMOTE: {
			pPanelBase->pobjs->pb_remote->update(true);
		}break;
		case ID_MAIN_PNL_OBJ_PB_PAD_MODE: {

			pPanelBase->pobjs->pb_pad_mode->update(true);
		}break;
		case ID_MAIN_PNL_OBJ_PB_ASSIST_FUNC: {
			pPanelBase->pobjs->pb_assist_func->update(true);
		}break;
		case ID_MAIN_PNL_OBJ_PB_OTE_TYPE_WND: {
			pPanelBase->pobjs->pb_ote_type_wnd->update(true);
		}break;
		case ID_MAIN_PNL_OBJ_PB_CRANE_SEL_WND: {
			pPanelBase->pobjs->pb_crane_sel_wnd->update(true);
		}break;
		case ID_MAIN_PNL_OBJ_PB_AUTH: {
			pPanelBase->pobjs->pb_auth->update(true);
		}break;
		case ID_MAIN_PNL_OBJ_CB_PNL_NOTCH: {
			pPanelBase->pobjs->cb_pnl_notch->update();
		}break;

		case ID_MAIN_PNL_OBJ_RDO_DISP_MODE1:
		case ID_MAIN_PNL_OBJ_RDO_DISP_MODE2:
		{
			pPanelBase->pobjs->rdo_disp_mode->update(true);
		}break;


		default:
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
		}
	}break;

	case WM_LBUTTONUP: {//�}�E�X���{�^�������Ń��j�^�E�B���h�E�`��X�V
		InvalidateRect(hWnd, NULL, TRUE); // �E�B���h�E�S�̂��ĕ`��
	}
	case WM_CTLCOLORSTATIC: {//�X�^�e�B�b�N�e�L�X�g�̐F�Z�b�g
		SetTextColor((HDC)wParam, RGB(220, 220, 220)); // ���C�g�O���[
		SetBkMode((HDC)wParam, TRANSPARENT);
	}return (LRESULT)GetStockObject(NULL_BRUSH); // �w�i�F�ɍ��킹��

	case WM_ERASEBKGND: {//�E�B���h�E�̔w�i�F���O���[��
		pSubPanelBase->pobjs->pgraphic->FillRectangle(pSubPanelBase->pobjs->pBrushBk, pSubPanelBase->pobjs->rc_panel);

	}return 1; // �w�i�������������Ƃ�����

	case WM_TIMER: {
		//# LAMP(CTRL)�X�V
		//e-stop : PLC�̔F����ESTOP�̎��g�L�\��
		INT16 code = pOteCCIf->st_msg_pc_u_rcv.body.st.lamp[OTE_PNL_CTRLS::estop].code;//����PC��M�o�b�t�@�̎w�ߓ��e
		int val = pPanelBase->pobjs->cb_estop->get();									//CB�̏��
		//CB��ԂŐ���PC��ON�\��������Ε\���摜��ؑ�
		if (val == BST_CHECKED) { if (code) val = 3; }
		else { if (code)val = 2; }
		pPanelBase->pobjs->lmp_estop->set(val);
		pPanelBase->pobjs->lmp_estop->update();

		//�劲
		code = pOteCCIf->st_msg_pc_u_rcv.body.st.lamp[OTE_PNL_CTRLS::syukan_on].code;
		pPanelBase->pobjs->lmp_syukan_on->set(code); pPanelBase->pobjs->lmp_syukan_on->update();
		code = pOteCCIf->st_msg_pc_u_rcv.body.st.lamp[OTE_PNL_CTRLS::syukan_off].code;
		pPanelBase->pobjs->lmp_syukan_off->set(code); pPanelBase->pobjs->lmp_syukan_off->update();

		//���u
		pPanelBase->pobjs->lmp_remote->set(pOteCsInf->st_body.remote);
		pPanelBase->pobjs->lmp_remote->update();

		pPanelBase->pobjs->lmp_pad_mode->set(pOteCsInf->st_body.game_pad_mode);
		pPanelBase->pobjs->lmp_pad_mode->update();

		//�̏჊�Z�b�g
	//	pPanelBase->pobjs->lmp_freset->set(pOteCsInf->st_body.ctrl_ope[OTE_PNL_CTRLS::fault_reset]);
		if (st_work.ctrl_stat[OTE_PNL_CTRLS::fault_reset])
			pPanelBase->pobjs->lmp_freset->set(L_ON);
		else
			pPanelBase->pobjs->lmp_freset->set(L_OFF);

		pPanelBase->pobjs->lmp_freset->update();


		//# SwitchImg�X�V(�����v�j
		//CC�Ƃ̒ʐM��ԕ\��(��M�j
		if (pOteCCIf->cc_com_stat_r == ID_PNL_SOCK_STAT_ACT_RCV)
			pPanelBase->pobjs->lmp_pcr->set(ID_PANEL_LAMP_FLICK);
		else pPanelBase->pobjs->lmp_pcr->set(pOteCCIf->cc_com_stat_r);
		pPanelBase->pobjs->lmp_pcr->update();
		//CC�Ƃ̒ʐM��ԕ\��(���M�j
		if (pOteCCIf->cc_com_stat_s == ID_PNL_SOCK_STAT_ACT_SND)
			pPanelBase->pobjs->lmp_pcs->set(ID_PANEL_LAMP_FLICK);
		else pPanelBase->pobjs->lmp_pcs->set(pOteCCIf->cc_com_stat_s);
		pPanelBase->pobjs->lmp_pcs->update();
		//PLC�Ƃ̒ʐM��ԕ\��(��M�j
		pPanelBase->pobjs->lmp_plcr->update();
		//PLC�Ƃ̒ʐM��ԕ\��(���M�j
		pPanelBase->pobjs->lmp_plcs->update();

		//PB��ԍX�V(�I�t�f�B���C�J�E���g�_�E��)
		pPanelBase->pobjs->pb_syukan_on->update(false);
		pPanelBase->pobjs->pb_syukan_off->update(false);
		pPanelBase->pobjs->pb_remote->update(false);
		pPanelBase->pobjs->pb_auth->update(false);
		pPanelBase->pobjs->pb_assist_func->update(false);
		pPanelBase->pobjs->pb_crane_sel_wnd->update(false);
		pPanelBase->pobjs->pb_ote_type_wnd->update(false);
		pPanelBase->pobjs->pb_pad_mode->update(false);
		pPanelBase->pobjs->pb_freset->update(false);

		//String�X�V
		if (is_initial_draw_mon1) {
			pPanelBase->pobjs->str_message->update();
			pPanelBase->pobjs->str_pc_com_stat->update();
			pPanelBase->pobjs->str_plc_com_stat->update();
			//	is_initial_draw_mon1 = false;
		}

		//GamePad�`�F�b�N
		if (pOteCsInf->gpad_in.syukan_on) pPanelBase->pobjs->pb_syukan_on->update(true);
		if (pOteCsInf->gpad_in.syukan_off)pPanelBase->pobjs->pb_syukan_off->update(true);
		if (pOteCsInf->gpad_in.remote)pPanelBase->pobjs->pb_remote->update(true);
		if (pOteCsInf->gpad_in.estop)pPanelBase->pobjs->cb_estop->set(BST_CHECKED);
		if (pOteCsInf->gpad_in.f_reset) {//�Q�[���p�b�h�̔���~�N���A��ResetPB�Ŏ��s
			pPanelBase->pobjs->pb_freset->update(true);
			pPanelBase->pobjs->cb_estop->set(BST_UNCHECKED);
		}

	}break;

	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);

		//PatBlt(hdc, 0, 0, OTE_SCAD_MON1_WND_W, OTE_SCAD_MON1_WND_H, RGB(64, 64, 64));

		EndPaint(hWnd, &ps);
	}break;
	case WM_DRAWITEM: {//�����v�\�����X�V TIMER�C�x���g�ŏ�ԕω��`�F�b�N����InvalidiateRect�ŌĂяo��
		DRAWITEMSTRUCT* pDIS = (DRAWITEMSTRUCT*)lParam;
		Image* image;
		Graphics gra(pDIS->hDC);
		Font* pfont = NULL;
		CSubPanelObj* pos = pSubPanelBase->pobjs;
		CLampCtrl* plamp = NULL;

		if (pDIS->CtlID == pos->cb_bh_r_mode0->id) {
			plamp = pos->lmp_mh_spd_mode;	pfont = NULL;
		}
		else if (pDIS->CtlID == pos->pb_remote->id) {//�����[�g�����v
			plamp = pos->lmp_remote; pfont = plamp->pFont;
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
		gra.FillRectangle(pSubPanelBase->pobjs->pBrushBk, plamp->rc);											//�w�i�F�Z�b�g
		if (image) gra.DrawImage(image, plamp->rc);															//�C���[�W�`��
		if (pfont != NULL)
			gra.DrawString(plamp->txt.c_str(), -1, pfont, plamp->frc, plamp->pStrFormat, plamp->pTxtBrush);	//�e�L�X�g�`��

	}return true;

	case WM_DESTROY: {
		KillTimer(hWnd, ID_SUB_PANEL_TIMER);
	}break;
	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
#endif
}
LRESULT CALLBACK CSubPanelWindow::WndProcCom(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CREATE: {
        SetWindowText(hwnd, L"�ʐM");
    }break;

    case WM_DESTROY:
        // PostQuitMessage(0);
        return 0;
    case WM_CLOSE:
        DestroyWindow(hwnd);
        return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
LRESULT CALLBACK CSubPanelWindow::WndProcCam(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CREATE: {
        SetWindowText(hwnd, L"�J����");
    }break;

    case WM_DESTROY:
        // PostQuitMessage(0);
        return 0;
    case WM_CLOSE:
        DestroyWindow(hwnd);
        return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
LRESULT CALLBACK CSubPanelWindow::WndProcStat(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CREATE: {
        SetWindowText(hwnd, L"���");
    }break;

    case WM_DESTROY:
        // PostQuitMessage(0);
        return 0;
    case WM_CLOSE:
        DestroyWindow(hwnd);
        return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

static bool is_initial_draw_main = false;
LRESULT CALLBACK CMainPanelWindow::WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {

	switch (msg)
	{
	case WM_CREATE: {
		InitCommonControls();//�R�����R���g���[��������
		HINSTANCE hInst = (HINSTANCE)GetModuleHandle(0);

		pPanelBase = new CPanelBase(crane_id, CODE_OTE_PNL_TYPE_MAIN, hWnd);

		//�E�B���h�E�ɃR���g���[���ǉ�
		//STATIC,LABEL
		CStaticCtrl* pst = pPanelBase->pmainobjs->txt_uid;
		pst->set_wnd(CreateWindowW(TEXT("STATIC"), pst->txt.c_str(), WS_CHILD | WS_VISIBLE | SS_LEFT,
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
		pcb->set_wnd(CreateWindowW(TEXT("BUTTON"), pcb->txt.c_str(), WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX | BS_PUSHLIKE | BS_MULTILINE | BS_OWNERDRAW,
			pcb->pt.X, pcb->pt.Y, pcb->sz.Width, pcb->sz.Height, hWnd, (HMENU)(pcb->id), hInst, NULL));
		pPanelBase->pmainobjs->lmp_estop->set_ctrl(pcb);//�����v�Ƀ{�^���̃{�^���R���g���[�����Z�b�g

		//�����
		pcb = pPanelBase->pmainobjs->cb_pnl_notch;
		pcb->set_wnd(CreateWindowW(TEXT("BUTTON"), pcb->txt.c_str(), WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX | BS_PUSHLIKE | BS_MULTILINE,
			pcb->pt.X, pcb->pt.Y, pcb->sz.Width, pcb->sz.Height, hWnd, (HMENU)(pcb->id), hInst, NULL));

		//# PBL !!OWNER DRAW
		//�劲
		CPbCtrl* ppb = pPanelBase->pmainobjs->pb_syukan_on;
		ppb->set_wnd(CreateWindowW(TEXT("BUTTON"), ppb->txt.c_str(), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_PUSHLIKE | BS_OWNERDRAW,
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
		ppb->set_wnd(CreateWindowW(TEXT("BUTTON"), ppb->txt.c_str(), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_PUSHLIKE,
			ppb->pt.X, ppb->pt.Y, ppb->sz.Width, ppb->sz.Height, hWnd, (HMENU)(ppb->id), hInst, NULL));
		//�[���ݒ�
		ppb = pPanelBase->pmainobjs->pb_ote_type_wnd;
		ppb->set_wnd(CreateWindowW(TEXT("BUTTON"), ppb->txt.c_str(), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_PUSHLIKE,
			ppb->pt.X, ppb->pt.Y, ppb->sz.Width, ppb->sz.Height, hWnd, (HMENU)(ppb->id), hInst, NULL));
		//�A�V�X�g
		ppb = pPanelBase->pmainobjs->pb_assist_func;
		ppb->set_wnd(CreateWindowW(TEXT("BUTTON"), ppb->txt.c_str(), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_PUSHLIKE,
			ppb->pt.X, ppb->pt.Y, ppb->sz.Width, ppb->sz.Height, hWnd, (HMENU)(ppb->id), hInst, NULL));
		//�N���[���I��
		ppb = pPanelBase->pmainobjs->pb_crane_sel_wnd;
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
		pcb->set_wnd(CreateWindowW(TEXT("BUTTON"), pcb->txt.c_str(), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_PUSHLIKE | BS_MULTILINE | WS_GROUP,
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
		is_initial_draw_main = true;

		//Lamp �E�B���h�E�n���h���Z�b�g,�t���b�J�ݒ�
		//CC�Ƃ̒ʐM��ԕ\��
		INT32 id_list[2] = { 4,0 };//2��t���b�J�@��/�Ð�
		pPanelBase->pmainobjs->lmp_pcr->set_wnd(hWnd);		pPanelBase->pmainobjs->lmp_pcr->setup_flick(2, 3, id_list);
		pPanelBase->pmainobjs->lmp_pcs->set_wnd(hWnd);		pPanelBase->pmainobjs->lmp_pcs->setup_flick(2, 3, id_list);
		pPanelBase->pmainobjs->lmp_plcr->set_wnd(hWnd);	pPanelBase->pmainobjs->lmp_plcr->setup_flick(2, 3, id_list);
		pPanelBase->pmainobjs->lmp_plcs->set_wnd(hWnd);	pPanelBase->pmainobjs->lmp_plcs->setup_flick(2, 3, id_list);

		//�\���X�V�p�^�C�}�[
		SetTimer(hWnd, ID_MAIN_PANEL_TIMER, ID_MAIN_PANEL_TIMER_MS, NULL);
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
			else {
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
		case ID_MAIN_PNL_OBJ_RDO_OPT_WND_COM:
		case ID_MAIN_PNL_OBJ_RDO_OPT_WND_CAM:
		case ID_MAIN_PNL_OBJ_RDO_OPT_WND_STAT:
		{
			HINSTANCE hInst = (HINSTANCE)GetModuleHandle(0);

			if (pSubPanelWnd == NULL) {
				pSubPanelWnd = new CSubPanelWindow(hInst, hWnd, crane_id, wmId, pPanelBase);
			}
			else {
				if (pSubPanelWnd->wnd_code == wmId) {
					delete pSubPanelWnd;
					pSubPanelWnd = NULL;
				}
				else {
					delete pSubPanelWnd;
					pSubPanelWnd = new CSubPanelWindow(hInst, hWnd,crane_id, wmId,pPanelBase);
				}
			}

			InvalidateRect(hWnd, NULL, TRUE); // �E�B���h�E�S�̂��ĕ`��

		}break;
		case ID_MAIN_PNL_OBJ_RDO_OPT_WND_CLR:
		{
			delete pSubPanelWnd;
			pSubPanelWnd = NULL;
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
		pPanelBase->pmainobjs->pgraphic->FillRectangle(pPanelBase->pmainobjs->pBrushBk, pPanelBase->pmainobjs->rc_panel);

	}return 1; // �w�i�������������Ƃ�����

	case WM_TIMER: {
		//# LAMP(CTRL)�X�V
		//e-stop : PLC�̔F����ESTOP�̎��g�L�\��
		INT16 code = pCcIf->st_msg_pc_u_rcv.body.st.lamp[OTE_PNL_CTRLS::estop].code;//����PC��M�o�b�t�@�̎w�ߓ��e
		int val = pPanelBase->pmainobjs->cb_estop->get();									//CB�̏��
		//CB��ԂŐ���PC��ON�\��������Ε\���摜��ؑ�
		if (val == BST_CHECKED) { if (code) val = 3; }
		else { if (code)val = 2; }
		pPanelBase->pmainobjs->lmp_estop->set(val);
		pPanelBase->pmainobjs->lmp_estop->update();

		//�劲
		code = pCcIf->st_msg_pc_u_rcv.body.st.lamp[OTE_PNL_CTRLS::syukan_on].code;
		pPanelBase->pmainobjs->lmp_syukan_on->set(code); pPanelBase->pmainobjs->lmp_syukan_on->update();
		code = pCcIf->st_msg_pc_u_rcv.body.st.lamp[OTE_PNL_CTRLS::syukan_off].code;
		pPanelBase->pmainobjs->lmp_syukan_off->set(code); pPanelBase->pmainobjs->lmp_syukan_off->update();

		//���u
		pPanelBase->pmainobjs->lmp_remote->set(pCsInf->st_body.remote);
		pPanelBase->pmainobjs->lmp_remote->update();

		pPanelBase->pmainobjs->lmp_pad_mode->set(pCsInf->st_body.game_pad_mode);
		pPanelBase->pmainobjs->lmp_pad_mode->update();

		//�̏჊�Z�b�g
	//	pPanelBase->pmainobjs->lmp_freset->set(pOteCsInf->st_body.ctrl_ope[OTE_PNL_CTRLS::fault_reset]);
		if (pUi->ctrl_stat[OTE_PNL_CTRLS::fault_reset])
			pPanelBase->pmainobjs->lmp_freset->set(L_ON);
		else
			pPanelBase->pmainobjs->lmp_freset->set(L_OFF);

		pPanelBase->pmainobjs->lmp_freset->update();


		//# SwitchImg�X�V(�����v�j
		//CC�Ƃ̒ʐM��ԕ\��(��M�j
		if (pCcIf->cc_com_stat_r == ID_PNL_SOCK_STAT_ACT_RCV)
			pPanelBase->pmainobjs->lmp_pcr->set(ID_PANEL_LAMP_FLICK);
		else pPanelBase->pmainobjs->lmp_pcr->set(pCcIf->cc_com_stat_r);
		pPanelBase->pmainobjs->lmp_pcr->update();
		//CC�Ƃ̒ʐM��ԕ\��(���M�j
		if (pCcIf->cc_com_stat_s == ID_PNL_SOCK_STAT_ACT_SND)
			pPanelBase->pmainobjs->lmp_pcs->set(ID_PANEL_LAMP_FLICK);
		else pPanelBase->pmainobjs->lmp_pcs->set(pCcIf->cc_com_stat_s);
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
		if (is_initial_draw_main) {
			pPanelBase->pmainobjs->str_message->update();
			pPanelBase->pmainobjs->str_pc_com_stat->update();
			pPanelBase->pmainobjs->str_plc_com_stat->update();
			//	is_initial_draw_mon1 = false;
		}

		//GamePad�`�F�b�N
		if (pCsInf->gpad_in.syukan_on) pPanelBase->pmainobjs->pb_syukan_on->update(true);
		if (pCsInf->gpad_in.syukan_off)pPanelBase->pmainobjs->pb_syukan_off->update(true);
		if (pCsInf->gpad_in.remote)pPanelBase->pmainobjs->pb_remote->update(true);
		if (pCsInf->gpad_in.estop)pPanelBase->pmainobjs->cb_estop->set(BST_CHECKED);
		if (pCsInf->gpad_in.f_reset) {//�Q�[���p�b�h�̔���~�N���A��ResetPB�Ŏ��s
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
		Font* pfont = NULL;
		CMainPanelObj* pos = pPanelBase->pmainobjs;
		CLampCtrl* plamp = NULL;

		if (pDIS->CtlID == pos->cb_estop->id) {
			plamp = pos->lmp_estop;	pfont = NULL;
		}
		else if (pDIS->CtlID == pos->pb_remote->id) {//�����[�g�����v
			plamp = pos->lmp_remote; pfont = plamp->pFont;
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
		hWnd = NULL;
		KillTimer(hWnd, ID_MAIN_PANEL_TIMER);
		//### �I�[�v�j���O��ʂ��ĕ\��
		pEnvObj->open_opening_window();

	}break;
	default:
		return DefWindowProc(hWnd, msg, wp, lp);
	}
	return S_OK;
};
LRESULT CALLBACK CMainPanelWindow::WndProcHHGH29(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {

	switch (msg)
	{
	case WM_CREATE: {
		InitCommonControls();//�R�����R���g���[��������
		HINSTANCE hInst = (HINSTANCE)GetModuleHandle(0);

		pPanelBase = new CPanelBase(crane_id, CODE_OTE_PNL_TYPE_MAIN, hWnd);

		//�E�B���h�E�ɃR���g���[���ǉ�
		//STATIC,LABEL
		CStaticCtrl* pst = pPanelBase->pmainobjs->txt_uid;
		pst->set_wnd(CreateWindowW(TEXT("STATIC"), pst->txt.c_str(), WS_CHILD | WS_VISIBLE | SS_LEFT,
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
		pcb->set_wnd(CreateWindowW(TEXT("BUTTON"), pcb->txt.c_str(), WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX | BS_PUSHLIKE | BS_MULTILINE | BS_OWNERDRAW,
			pcb->pt.X, pcb->pt.Y, pcb->sz.Width, pcb->sz.Height, hWnd, (HMENU)(pcb->id), hInst, NULL));
		pPanelBase->pmainobjs->lmp_estop->set_ctrl(pcb);//�����v�Ƀ{�^���̃{�^���R���g���[�����Z�b�g

		//�����
		pcb = pPanelBase->pmainobjs->cb_pnl_notch;
		pcb->set_wnd(CreateWindowW(TEXT("BUTTON"), pcb->txt.c_str(), WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX | BS_PUSHLIKE | BS_MULTILINE,
			pcb->pt.X, pcb->pt.Y, pcb->sz.Width, pcb->sz.Height, hWnd, (HMENU)(pcb->id), hInst, NULL));

		//# PBL !!OWNER DRAW
		//�劲
		CPbCtrl* ppb = pPanelBase->pmainobjs->pb_syukan_on;
		ppb->set_wnd(CreateWindowW(TEXT("BUTTON"), ppb->txt.c_str(), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_PUSHLIKE | BS_OWNERDRAW,
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
		ppb->set_wnd(CreateWindowW(TEXT("BUTTON"), ppb->txt.c_str(), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_PUSHLIKE,
			ppb->pt.X, ppb->pt.Y, ppb->sz.Width, ppb->sz.Height, hWnd, (HMENU)(ppb->id), hInst, NULL));
		//�[���ݒ�
		ppb = pPanelBase->pmainobjs->pb_ote_type_wnd;
		ppb->set_wnd(CreateWindowW(TEXT("BUTTON"), ppb->txt.c_str(), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_PUSHLIKE,
			ppb->pt.X, ppb->pt.Y, ppb->sz.Width, ppb->sz.Height, hWnd, (HMENU)(ppb->id), hInst, NULL));
		//�A�V�X�g
		ppb = pPanelBase->pmainobjs->pb_assist_func;
		ppb->set_wnd(CreateWindowW(TEXT("BUTTON"), ppb->txt.c_str(), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_PUSHLIKE,
			ppb->pt.X, ppb->pt.Y, ppb->sz.Width, ppb->sz.Height, hWnd, (HMENU)(ppb->id), hInst, NULL));
		//�N���[���I��
		ppb = pPanelBase->pmainobjs->pb_crane_sel_wnd;
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
		pcb->set_wnd(CreateWindowW(TEXT("BUTTON"), pcb->txt.c_str(), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_PUSHLIKE | BS_MULTILINE | WS_GROUP,
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
		is_initial_draw_main = true;

		//Lamp �E�B���h�E�n���h���Z�b�g,�t���b�J�ݒ�
		//CC�Ƃ̒ʐM��ԕ\��
		INT32 id_list[2] = { 4,0 };//2��t���b�J�@��/�Ð�
		pPanelBase->pmainobjs->lmp_pcr->set_wnd(hWnd);		pPanelBase->pmainobjs->lmp_pcr->setup_flick(2, 3, id_list);
		pPanelBase->pmainobjs->lmp_pcs->set_wnd(hWnd);		pPanelBase->pmainobjs->lmp_pcs->setup_flick(2, 3, id_list);
		pPanelBase->pmainobjs->lmp_plcr->set_wnd(hWnd);	pPanelBase->pmainobjs->lmp_plcr->setup_flick(2, 3, id_list);
		pPanelBase->pmainobjs->lmp_plcs->set_wnd(hWnd);	pPanelBase->pmainobjs->lmp_plcs->setup_flick(2, 3, id_list);

		//�\���X�V�p�^�C�}�[
		SetTimer(hWnd, ID_MAIN_PANEL_TIMER, ID_MAIN_PANEL_TIMER_MS, NULL);
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
			else {
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
		case ID_MAIN_PNL_OBJ_RDO_OPT_WND_COM:
		case ID_MAIN_PNL_OBJ_RDO_OPT_WND_CAM:
		case ID_MAIN_PNL_OBJ_RDO_OPT_WND_STAT:
		{
			HINSTANCE hInst = (HINSTANCE)GetModuleHandle(0);

			if (pSubPanelWnd == NULL) {
				pSubPanelWnd = new CSubPanelWindow(hInst, hWnd, crane_id, wmId, pPanelBase);
			}
			else {
				if (pSubPanelWnd->wnd_code == wmId) {
					delete pSubPanelWnd;
					pSubPanelWnd = NULL;
				}
				else {
					delete pSubPanelWnd;
					pSubPanelWnd = new CSubPanelWindow(hInst, hWnd, crane_id, wmId, pPanelBase);
				}
			}

			InvalidateRect(hWnd, NULL, TRUE); // �E�B���h�E�S�̂��ĕ`��

		}break;
		case ID_MAIN_PNL_OBJ_RDO_OPT_WND_CLR:
		{
			delete pSubPanelWnd;
			pSubPanelWnd = NULL;
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
		pPanelBase->pmainobjs->pgraphic->FillRectangle(pPanelBase->pmainobjs->pBrushBk, pPanelBase->pmainobjs->rc_panel);

	}return 1; // �w�i�������������Ƃ�����

	case WM_TIMER: {
		//# LAMP(CTRL)�X�V
		//e-stop : PLC�̔F����ESTOP�̎��g�L�\��
		INT16 code = pCcIf->st_msg_pc_u_rcv.body.st.lamp[OTE_PNL_CTRLS::estop].code;//����PC��M�o�b�t�@�̎w�ߓ��e
		int val = pPanelBase->pmainobjs->cb_estop->get();									//CB�̏��
		//CB��ԂŐ���PC��ON�\��������Ε\���摜��ؑ�
		if (val == BST_CHECKED) { if (code) val = 3; }
		else { if (code)val = 2; }
		pPanelBase->pmainobjs->lmp_estop->set(val);
		pPanelBase->pmainobjs->lmp_estop->update();

		//�劲
		code = pCcIf->st_msg_pc_u_rcv.body.st.lamp[OTE_PNL_CTRLS::syukan_on].code;
		pPanelBase->pmainobjs->lmp_syukan_on->set(code); pPanelBase->pmainobjs->lmp_syukan_on->update();
		code = pCcIf->st_msg_pc_u_rcv.body.st.lamp[OTE_PNL_CTRLS::syukan_off].code;
		pPanelBase->pmainobjs->lmp_syukan_off->set(code); pPanelBase->pmainobjs->lmp_syukan_off->update();

		//���u
		pPanelBase->pmainobjs->lmp_remote->set(pCsInf->st_body.remote);
		pPanelBase->pmainobjs->lmp_remote->update();

		pPanelBase->pmainobjs->lmp_pad_mode->set(pCsInf->st_body.game_pad_mode);
		pPanelBase->pmainobjs->lmp_pad_mode->update();

		//�̏჊�Z�b�g
	//	pPanelBase->pmainobjs->lmp_freset->set(pOteCsInf->st_body.ctrl_ope[OTE_PNL_CTRLS::fault_reset]);
		if (pUi->ctrl_stat[OTE_PNL_CTRLS::fault_reset])
			pPanelBase->pmainobjs->lmp_freset->set(L_ON);
		else
			pPanelBase->pmainobjs->lmp_freset->set(L_OFF);

		pPanelBase->pmainobjs->lmp_freset->update();


		//# SwitchImg�X�V(�����v�j
		//CC�Ƃ̒ʐM��ԕ\��(��M�j
		if (pCcIf->cc_com_stat_r == ID_PNL_SOCK_STAT_ACT_RCV)
			pPanelBase->pmainobjs->lmp_pcr->set(ID_PANEL_LAMP_FLICK);
		else pPanelBase->pmainobjs->lmp_pcr->set(pCcIf->cc_com_stat_r);
		pPanelBase->pmainobjs->lmp_pcr->update();
		//CC�Ƃ̒ʐM��ԕ\��(���M�j
		if (pCcIf->cc_com_stat_s == ID_PNL_SOCK_STAT_ACT_SND)
			pPanelBase->pmainobjs->lmp_pcs->set(ID_PANEL_LAMP_FLICK);
		else pPanelBase->pmainobjs->lmp_pcs->set(pCcIf->cc_com_stat_s);
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
		if (is_initial_draw_main) {
			pPanelBase->pmainobjs->str_message->update();
			pPanelBase->pmainobjs->str_pc_com_stat->update();
			pPanelBase->pmainobjs->str_plc_com_stat->update();
			//	is_initial_draw_mon1 = false;
		}

		//GamePad�`�F�b�N
		if (pCsInf->gpad_in.syukan_on) pPanelBase->pmainobjs->pb_syukan_on->update(true);
		if (pCsInf->gpad_in.syukan_off)pPanelBase->pmainobjs->pb_syukan_off->update(true);
		if (pCsInf->gpad_in.remote)pPanelBase->pmainobjs->pb_remote->update(true);
		if (pCsInf->gpad_in.estop)pPanelBase->pmainobjs->cb_estop->set(BST_CHECKED);
		if (pCsInf->gpad_in.f_reset) {//�Q�[���p�b�h�̔���~�N���A��ResetPB�Ŏ��s
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
		Font* pfont = NULL;
		CMainPanelObj* pos = pPanelBase->pmainobjs;
		CLampCtrl* plamp = NULL;

		if (pDIS->CtlID == pos->cb_estop->id) {
			plamp = pos->lmp_estop;	pfont = NULL;
		}
		else if (pDIS->CtlID == pos->pb_remote->id) {//�����[�g�����v
			plamp = pos->lmp_remote; pfont = plamp->pFont;
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
		hWnd = NULL;
		KillTimer(hWnd, ID_MAIN_PANEL_TIMER);
		//### �I�[�v�j���O��ʂ��ĕ\��
		pEnvObj->open_opening_window();

	}break;
	default:
		return DefWindowProc(hWnd, msg, wp, lp);
	}
	return S_OK;
};