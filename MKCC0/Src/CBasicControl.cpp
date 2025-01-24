#include "..\SENSOR\framework.h"
#include "common_def.h"
#include "CBasicControl.h"
#include <windowsx.h>
#include <timeapi.h>

#include <commctrl.h> //�R�����R���g���[���p

#include "..\SENSOR\resource.h"

unsigned WINAPI CBasicControl::run(LPVOID lpParam)
{
	CBasicControl* pobj = (CBasicControl*)lpParam;

	LARGE_INTEGER start_count;  // ���݂̃J�E���g��
	LONGLONG      span_msec;    // ���Ԃ̊Ԋu[usec]
	
	QueryPerformanceFrequency(&inf.sys_freq);  // �V�X�e���̎��g��
	QueryPerformanceCounter(&inf.sys_count); // �p�t�H�[�}���X�J�E���^�[���ݒl
	
	while (pobj->inf.command != BC_CODE_COM_TERMINATE) {

		QueryPerformanceCounter(&start_count);  // ���݂̃J�E���g��
		pobj->inf.event_triggered = WaitForMultipleObjects(pobj->inf.n_active_events, pobj->inf.hevents, FALSE, INFINITE);//���C���X���b�h�����SIGNAL��ԑ҂�


		span_msec = ((start_count.QuadPart - inf.sys_count.QuadPart) * 1000000L) / inf.sys_freq.QuadPart;
		QueryPerformanceCounter(&inf.sys_count); // �p�t�H�[�}���X�J�E���^�[���ݒl

		wos.str(L""); wos << inf.status << L":"  << std::setfill(L'0') << std::setw(4)<< span_msec;
		msg2host(wos.str());

		//���������m�F�p
		DWORD start_time = timeGetTime();
		pobj->inf.period = start_time - pobj->inf.start_time;
		pobj->inf.start_time = start_time;

		switch (pobj->inf.status) {
		case BC_CODE_STAT_INIT_REQ: {
			if (STAT_OK == pobj->initialize(pobj)) {
				pobj->inf.status = BC_CODE_STAT_RUN_REQ;
				wos.str(L"Initialize OK");msg2listview(wos.str());
			}
			else {
				pobj->inf.status = BC_CODE_STAT_INIT_ERROR;
			}
		}break;
		case BC_CODE_STAT_RUN_REQ:
		{
			if (STAT_OK == pobj->routine_work(pobj))pobj->inf.status = BC_CODE_STAT_RUNNING;
			else									pobj->inf.status = BC_CODE_STAT_ERROR;
		}break;
		case BC_CODE_STAT_RUNNING:
		{
			if (STAT_OK != pobj->routine_work(pobj))			pobj->inf.status = BC_CODE_STAT_ERROR;
		}break;
		case BC_CODE_STAT_ERROR:
		case BC_CODE_STAT_INIT_ERROR:
		{
			pobj->inf.status = BC_CODE_STAT_INIT_WAIT;
		}break;
		case BC_CODE_STAT_INIT_WAIT:
		{
			if (start_time % pobj->inf.retry_count == 0)
				pobj->inf.status = BC_CODE_STAT_INIT_REQ;
		}break;
		default: {//��ԑJ�ڃX�e�[�^�X�s��
			pobj->inf.status = BC_CODE_STAT_IDLE;
			return 0;
		}break;
		}

		pobj->inf.act_time = timeGetTime() - start_time;//���������ԑ���p
		if (this->inf.command != BC_CODE_COM_REPEAT_SCAN) break;//���������̏ꍇ�͔�����
	}

	pobj->inf.status = BC_CODE_STAT_IDLE;
	return 0;
}

LRESULT CALLBACK CBasicControl::PanelProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp) {

	switch (msg) {
	case WM_COMMAND:
		switch (LOWORD(wp)) {
		case IDC_TASK_FUNC_RADIO1:
		case IDC_TASK_FUNC_RADIO2:
		case IDC_TASK_FUNC_RADIO3:
		case IDC_TASK_FUNC_RADIO4:
		case IDC_TASK_FUNC_RADIO5:
		case IDC_TASK_FUNC_RADIO6:
			inf.panel_func_id = LOWORD(wp); set_panel_tip_txt(); 
			set_PNLparam_value(0.0, 0.0, 0.0, 0.0, 0.0, 0.0); 
			for (int i = 0; i < BC_N_ACT_ITEM; i++) {
				if (inf.panel_act_chk[wp - IDC_TASK_FUNC_RADIO1][i]) 
					SendMessage(GetDlgItem(hDlg, IDC_TASK_ITEM_CHECK1+i), BM_SETCHECK, BST_CHECKED, 0L);
				else 
					SendMessage(GetDlgItem(hDlg, IDC_TASK_ITEM_CHECK1 + i), BM_SETCHECK, BST_UNCHECKED, 0L);
			}
		break;

		case IDC_TASK_ITEM_CHECK1:
		case IDC_TASK_ITEM_CHECK2:
		case IDC_TASK_ITEM_CHECK3:
		case IDC_TASK_ITEM_CHECK4:
		case IDC_TASK_ITEM_CHECK5:
		case IDC_TASK_ITEM_CHECK6:
			if (IsDlgButtonChecked(hDlg, LOWORD(wp)) == BST_CHECKED) 
				inf.panel_act_chk[inf.panel_func_id - IDC_TASK_FUNC_RADIO1][LOWORD(wp) - IDC_TASK_ITEM_CHECK1] = true;
			else 
				inf.panel_act_chk[inf.panel_func_id - IDC_TASK_FUNC_RADIO1][LOWORD(wp) - IDC_TASK_ITEM_CHECK1] = false;
		 break;

		case IDSET: {
			wstring wstr, wstr_tmp;

			//�T���v���Ƃ��Ă��낢��Ȍ^�œǂݍ���ŕ\�����Ă���
			wstr += L"Param 1(d):";
			int n = GetDlgItemText(hDlg, IDC_TASK_EDIT1, (LPTSTR)wstr_tmp.c_str(), 128);
			if (n) wstr_tmp = to_wstring(stod(wstr_tmp));	wstr = wstr + wstr_tmp.c_str();

			wstr += L",  Param 2(i):";
			n = GetDlgItemText(hDlg, IDC_TASK_EDIT2, (LPTSTR)wstr_tmp.c_str(), 128);
			if (n) wstr_tmp = to_wstring(stoi(wstr_tmp));	wstr = wstr + wstr_tmp.c_str();

			wstr += L",  Param 3(f):";
			n = GetDlgItemText(hDlg, IDC_TASK_EDIT3, (LPTSTR)wstr_tmp.c_str(), 128);
			if (n) wstr_tmp = to_wstring(stof(wstr_tmp));	wstr = wstr + wstr_tmp.c_str();

			wstr += L",  Param 4(l):";
			n = GetDlgItemText(hDlg, IDC_TASK_EDIT4, (LPTSTR)wstr_tmp.c_str(), 128);
			if (n) wstr_tmp = to_wstring(stol(wstr_tmp));	wstr = wstr + wstr_tmp.c_str();

			wstr += L",  Param 5(c):";
			n = GetDlgItemText(hDlg, IDC_TASK_EDIT5, (LPTSTR)wstr_tmp.c_str(), 128);
			wstr += wstr_tmp.c_str();

			wstr += L",   Param 6(c):";
			n = GetDlgItemText(hDlg, IDC_TASK_EDIT6, (LPTSTR)wstr_tmp.c_str(), 128);
			wstr += wstr_tmp.c_str();

			msg2listview(wstr);

		}break;
		case IDRESET: {
			set_PNLparam_value(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
	
		}break;

		case IDC_TASK_MODE_RADIO0:
		case IDC_TASK_MODE_RADIO1:
		case IDC_TASK_MODE_RADIO2:

		case IDC_TASK_MON_CHECK1:
		{

		}break;

		case IDC_TASK_MON_CHECK2: {
			if (IsDlgButtonChecked(hDlg, IDC_TASK_MON_CHECK2) == BST_CHECKED) {
				SendMessage(GetDlgItem(hDlg, IDC_TASK_MON_CHECK1), BM_SETCHECK, BST_CHECKED, 0L);
			}
			else {
				SendMessage(GetDlgItem(hDlg, IDC_TASK_MON_CHECK1), BM_SETCHECK, BST_UNCHECKED, 0L);
			}
		}
			break;
		}
	}
	return 0;
};

///###	�^�u�p�l����Function�{�^����Static�e�L�X�g��ݒ�
void CBasicControl::msg2host(wstring wstr) {

	if(inf.hwnd_msgstatics != NULL) SetWindowTextW(inf.hwnd_msgstatics, wstr.c_str());
	return;
}

///###	�^�u�p�l����Function�{�^����Static�e�L�X�g��ݒ�
void CBasicControl::msg2listview(wstring wstr) {

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

void CBasicControl::set_PNLparam_value(float p1, float p2, float p3, float p4, float p5, float p6) {
	wstring wstr;
	wstr += std::to_wstring(p1); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT1), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p2); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT2), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p3); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT3), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p4); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT4), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p5); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT5), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p6); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT6), wstr.c_str());
}
