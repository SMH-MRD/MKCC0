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

	
	QueryPerformanceFrequency(&inf.sys_freq);  // �V�X�e���̎��g��
	QueryPerformanceCounter(&inf.sys_count); // �p�t�H�[�}���X�J�E���^�[���ݒl
	
	while (pobj->inf.command != BC_CODE_COM_TERMINATE) {

		QueryPerformanceCounter(&start_count);  // ���݂̃J�E���g��
		pobj->inf.event_triggered = WaitForMultipleObjects(pobj->inf.n_active_events, pobj->inf.hevents, FALSE, INFINITE);//���C���X���b�h�����SIGNAL��ԑ҂�


		pobj->inf.act_time = ((start_count.QuadPart - inf.sys_count.QuadPart) * 1000000L) / inf.sys_freq.QuadPart;
		QueryPerformanceCounter(&inf.sys_count); // �p�t�H�[�}���X�J�E���^�[���ݒl

		wos.str(L""); wos << inf.status << L":"  << std::setfill(L'0') << std::setw(4)<< pobj->inf.act_time;
		msg2host(wos.str());

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

///###	���C���p�l����
void CBasicControl::msg2host(wstring wstr) {

	if(inf.hwnd_msgstatics != NULL) SetWindowTextW(inf.hwnd_msgstatics, wstr.c_str());
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
