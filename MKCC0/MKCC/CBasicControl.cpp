#include "common_def.h"
#include "CBasicControl.h"
#include <windowsx.h>
#include <timeapi.h>
#include <commctrl.h> //�R�����R���g���[���p

HRESULT __stdcall CBasicControl::initialize(void* param) { 
	inf.panel_func_id;
	set_func_pb_txt(); 
	set_item_chk_txt();
	set_panel_tip_txt();
	return 0; 
}; //����������

unsigned WINAPI CBasicControl::run(LPVOID lpParam)
{
	CBasicControl* pobj = (CBasicControl*)lpParam;

	LARGE_INTEGER start_count;  // ���݂̃J�E���g��
	LONGLONG disp_count = 50/inf.cycle_count; //�\������

	
	QueryPerformanceFrequency(&inf.sys_freq);	// �V�X�e���̎��g��(1�b�Ԃ̃J�E���g���j
	QueryPerformanceCounter(&inf.sys_count);	// �p�t�H�[�}���X�J�E���^�[���ݒl

	inf.cnt_unit_ms = inf.sys_freq.QuadPart / 1000; //1ms������̃J�E���g��
	inf.cnt_unit_us = inf.sys_freq.QuadPart / 1000000; //1��s������̃J�E���g��
	
	while (pobj->inf.command != BC_CODE_COM_TERMINATE) {

		QueryPerformanceCounter(&start_count);  // ���݂̃J�E���g��

		//######�����Ń��C���X���b�h�����SIGNAL��ԑ҂�#######
		pobj->inf.event_triggered = WaitForMultipleObjects(pobj->inf.n_active_events, pobj->inf.hevents, FALSE, INFINITE);
		//######�����Ń��C���X���b�h�����SIGNAL��ԑ҂�#######

		//�X���b�h�������ԁ�(�O�񏈗��O�̃J�E���g-SIGNAL�҂��ɂȂ������̃J�E���g�j/1�ʕb�̃J�E���g��(�ʕb�j
		pobj->inf.act_time = (start_count.QuadPart - inf.sys_count.QuadPart) / inf.cnt_unit_us;
		
		QueryPerformanceCounter(&start_count);  // ���݂̃J�E���g��
		//�X�L��������
		pobj->inf.dt = (double)(start_count.QuadPart - inf.sys_count.QuadPart) / (double)inf.sys_freq.QuadPart;

		QueryPerformanceCounter(&inf.sys_count); // �p�t�H�[�}���X�J�E���^�[���ݒl

		//if (inf.total_act % disp_count == 0) {
		//	wos.str(L""); wos << inf.status << L":" << std::setfill(L'0') << std::setw(4) << pobj->inf.act_time;
		//	msg2host(wos.str());
		//}

		//���������m�F�p

		switch (pobj->inf.status) {
		case BC_CODE_STAT_INIT_REQ: {
			if (S_OK == pobj->initialize(pobj)) {
				pobj->inf.status = BC_CODE_STAT_RUN_REQ;
				wos.str(L"Initialize OK");msg2listview(wos.str());
			}
			else {
				pobj->inf.status = BC_CODE_STAT_INIT_ERROR;
			}
		}break;
		case BC_CODE_STAT_RUN_REQ:
		{
			if (S_OK == pobj->routine_work(pobj))pobj->inf.status = BC_CODE_STAT_RUNNING;
			else									pobj->inf.status = BC_CODE_STAT_ERROR;
		}break;
		case BC_CODE_STAT_RUNNING:
		{
			if (S_OK != pobj->routine_work(pobj))			pobj->inf.status = BC_CODE_STAT_ERROR;
		}break;
		case BC_CODE_STAT_ERROR:
		case BC_CODE_STAT_INIT_ERROR:
		{
			pobj->inf.status = BC_CODE_STAT_INIT_WAIT;
		}break;
		case BC_CODE_STAT_INIT_WAIT:
		{
			if ((inf.total_act % pobj->inf.retry_count) == 0)
				pobj->inf.status = BC_CODE_STAT_INIT_REQ;
		}break;
		default: {//��ԑJ�ڃX�e�[�^�X�s��
			pobj->inf.status = BC_CODE_STAT_IDLE;
			return 0;
		}break;
		}

//		pobj->inf.act_time = timeGetTime() - start_time;//���������ԑ���p
		if (this->inf.command != BC_CODE_COM_REPEAT_SCAN) break;//���������̏ꍇ�͔�����
	}

	pobj->inf.status = BC_CODE_STAT_IDLE;
	return 0;
}

LRESULT CALLBACK CBasicControl::PanelProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp) {

	switch (msg) {
	case WM_COMMAND:
		switch (LOWORD(wp)) {
		case 1:
		default:break;
		}
	}
	return 0;
};

///###	���C���p�l���̃X�^�e�B�b�N�e�L�X�g�֏o��
void CBasicControl::msg2host(wstring wstr) {

	if(inf.hwnd_msgstatics != NULL) SetWindowTextW(inf.hwnd_msgstatics, wstr.c_str());
	return;
}
void CBasicControl::set_color_of_hostmsg(int txtr, int txtg, int txtb, int bkr, int bkg, int bkb) {
	HBRUSH hbrush = CreateSolidBrush(RGB(bkr, bkg, bkb));

	//SetTextColor(inf.hwnd_msgstatics,)
	return;
}


///###	�^�u�p�l����ListView�Ƀ��b�Z�[�W���o��
void CBasicControl::msg2listview(wstring wstr) {
	return;
}

void CBasicControl::set_PNLparam_value(float p1, float p2, float p3, float p4, float p5, float p6) {
	return;
}

//�^�u�p�l����Static�e�L�X�g��ݒ�
void CBasicControl::set_panel_tip_txt() { 
	return; 
}
//�^�u�p�l����Function�{�^����Static�e�L�X�g��ݒ�
void CBasicControl::set_func_pb_txt() {
	return; 
}
//�^�u�p�l����Item chk�e�L�X�g��ݒ�
void CBasicControl::set_item_chk_txt() {

	return; 
}
