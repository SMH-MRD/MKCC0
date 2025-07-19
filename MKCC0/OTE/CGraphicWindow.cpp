#include "CGraphicWindow.h"
#include "COpePanel.h"
#include "CPanelWindow.h"
#include "CPanelObj.h"
#include <commctrl.h>       //�R�����R���g���[���p
#include "CBasicControl.h"
#include <vector>
#include "COteEnv.h"
#include "CFaults.h"


extern vector<CBasicControl*>	VectCtrlObj;
extern BC_TASK_ID st_task_id;
extern CCrane* pCrane;

static COteEnv* pEnvObj;

HWND CGraphicWindow::hGWnd;
HWND CGraphicWindow::hParentWnd;
CPanelBase* CGraphicWindow::pPanelBase;
int CGraphicWindow::crane_id;

LPST_OTE_UI CGraphicWindow::pUi;
LPST_OTE_CS_INF CGraphicWindow::pCsInf;
LPST_OTE_CC_IF CGraphicWindow::pCcIf;
LPST_OTE_ENV_INF CGraphicWindow::pOteEnvInf;

CGraphicWindow::CGraphicWindow(HINSTANCE hInstance, HWND hParent, int _crane_id, CPanelBase* _pPanelBase) {
	pPanelBase = _pPanelBase;
	hParentWnd = hParent;
	
	const wchar_t CLASS_NAME[]			= L"GWindowClass";
	const wchar_t CLASS_NAME_HHGH29[]	= L"GWindowHHGH29Class";

	const wchar_t* pClassName;

	WNDCLASS wc = { };
	switch (_crane_id) {

	case CARNE_ID_HHGH29:
		wc.lpfnWndProc = GWndProcHHGH29;
		wc.hInstance = hInstance;
		wc.lpszClassName = pClassName = CLASS_NAME_HHGH29;
		break;
	default:
		wc.lpfnWndProc = GWndProc;
		wc.hInstance = hInstance;
		wc.lpszClassName = pClassName = CLASS_NAME;
		break;
	}
	RegisterClass(&wc);

	hGWnd = CreateWindowEx(
		0,																// Optional window styles
		pClassName,														// Window class
		L"MAIN GRAPHIC",												// Window text
		WS_CHILD | WS_BORDER,											// Window style
		GMAIN_PNL_WND_X, GMAIN_PNL_WND_Y, GMAIN_PNL_WND_W, GMAIN_PNL_WND_H,
		hParent, nullptr, hInstance, nullptr
	);

	if (hGWnd) {
		ShowWindow(hGWnd, SW_SHOW);
		UpdateWindow(hGWnd);
	}
}
CGraphicWindow::~CGraphicWindow()
{
	close();
}
int CGraphicWindow::close()
{
	DestroyWindow(hGWnd);
	return 0;
}
void CGraphicWindow::set_up(LPST_OTE_UI _pUi, LPST_OTE_CS_INF _pCsInf, LPST_OTE_CC_IF _pCcIf, LPST_OTE_ENV_INF _pOteEnvInf, int _crane_id) {
	pUi = _pUi;
	pCsInf = _pCsInf;
	pCcIf = _pCcIf;
	pOteEnvInf = _pOteEnvInf;
	crane_id = _crane_id; //�N���[��ID���Z�b�g
	//### Environment�N���X�C���X�^���X�̃|�C���^�擾
	pEnvObj = (COteEnv*)VectCtrlObj[st_task_id.ENV];

	//PLC�̌̏���|�C���^�Z�b�g
	switch (crane_id) {
	case CARNE_ID_HHGH29:
	default:
	{
		LPST_PLC_RBUF_HHGH29 prbuf = (LPST_PLC_RBUF_HHGH29)pCcIf->st_msg_pc_u_rcv.body.st.buf_io_read;
	}
	break;
	}
	return;
};

// �`�揈���֐�
static int gwin_count = 0;
/// <summary>
/// �E�B���h�E�̃y�C���g�C�x���g���ɁA�_�u���o�b�t�@�����O��p����
/// �w�i�摜�A�A�C�R���摜�A���W�������`�悵�A��ʂɓ]�����܂��B
/// �`��i���̌���ⓧ�ߏ������s���܂��B
/// </summary>
/// <param name="hdc">�`��ΏۃE�B���h�E�̃f�o�C�X�R���e�L�X�g�n���h���B</param>
/// <param name="hWnd">�`��ΏۃE�B���h�E�̃n���h���B</param>
/// ########################################################################
void CGraphicWindow::OnPaint(HDC hdc, HWND hWnd){

int width = GMAIN_PNL_WND_W;
int height = GMAIN_PNL_WND_H;

gwin_count++; if (gwin_count > 1000) gwin_count = 0; // �J�E���g�����Z�b�g

// 1. �w�i�摜�̕`��(pbmp_bk�j
pPanelBase->pgwinobjs->lmg_bk_gwindow->set(0);		// �w�i�摜��������
pPanelBase->pgwinobjs->lmg_bk_gwindow->update();	// �w�i�摜��������
pPanelBase->pgwinobjs->lmg_crane_gt_base->set(0);
pPanelBase->pgwinobjs->lmg_crane_gt_base->update();	// �N���[���摜��������


// 2. �N���[���摜�̕`��(pbmp_img�j 
pPanelBase->pgwinobjs->lmg_crane_bm_xy->set(0);
pPanelBase->pgwinobjs->lmg_crane_bm_xy->update();	// �N���[���摜��������
//pPanelBase->pgwinobjs->lmg_crane_bm_xy->update_with_scale_angle(40,0,1.0,1.0,0.0);	// �N���[���摜��������
//pPanelBase->pgwinobjs->lmg_crane_bm_xy->update_with_center(gwin_count * -1.0,0);	// �N���[���摜��������
pPanelBase->pgwinobjs->lmg_crane_potal->set(0);
pPanelBase->pgwinobjs->lmg_crane_potal->update();	// �N���[���摜��������



// 3. Info�摜�̕`��(pbmp_inf�j 
wostringstream wo; 
wo.str(L""); wo << L"�g���F " << 50.0 + gwin_count*0.1;
pPanelBase->pgwinobjs->str_pos_mh->update(wo.str().c_str());	// �努�ʒu��������
wo.str(L""); wo << L"���a�F " << 50.0 - gwin_count * 0.1;
pPanelBase->pgwinobjs->str_pos_bh->update(wo.str().c_str());	// ���a��������
wo.str(L""); wo << L"����p�F " << 0.0 + gwin_count * 0.1;
pPanelBase->pgwinobjs->str_pos_sl->update(wo.str().c_str());	// ����e��������
wo.str(L""); wo << L"���s�ʒu�F " << 100.0 + gwin_count * 0.1;
pPanelBase->pgwinobjs->str_pos_gt->update(wo.str().c_str());	// �努�ʒu��������

// �N���[���摜��w�i�摜�ɏ�������
Status drawStatus = pPanelBase->pgwinobjs->pgraphic_bk->DrawImage(
	pPanelBase->pgwinobjs->pbmp_img,
	pPanelBase->pgwinobjs->rc_panel,
	0, 0, GMAIN_PNL_WND_W, GMAIN_PNL_WND_H,
	UnitPixel,
	&pPanelBase->pgwinobjs->attr
);

// ���摜��w�i�摜�ɏ�������
 drawStatus = pPanelBase->pgwinobjs->pgraphic_bk->DrawImage(
	pPanelBase->pgwinobjs->pbmp_inf,
	pPanelBase->pgwinobjs->rc_panel,
	0, 0, GMAIN_PNL_WND_W, GMAIN_PNL_WND_H,
	UnitPixel,
	&pPanelBase->pgwinobjs->attr
);

// �o�b�N�o�b�t�@�̓��e����x�ɉ�ʂɓ]��
pPanelBase->pgwinobjs->pgraphic->DrawImage(pPanelBase->pgwinobjs->pbmp_bk, 0, 0);
}


LRESULT CALLBACK CGraphicWindow::GWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_CREATE: {
		//�\���X�V�p�^�C�}�[
		SetTimer(hwnd, ID_GMAIN_TIMER, ID_GMAIN_TIMER_MS, NULL);
	}break;

	case WM_LBUTTONUP: {//�}�E�X���{�^�������Ń��j�^�E�B���h�E�`��X�V
		InvalidateRect(hwnd, NULL, FALSE); // �E�B���h�E�S�̂��ĕ`��
	}
	case WM_CTLCOLORSTATIC: {//�X�^�e�B�b�N�e�L�X�g�̐F�Z�b�g
		SetTextColor((HDC)wParam, RGB(220, 220, 220)); // ���C�g�O���[
		SetBkMode((HDC)wParam, TRANSPARENT);
	}return (LRESULT)GetStockObject(NULL_BRUSH); // �w�i�F�ɍ��킹��

	case WM_ERASEBKGND: {//�E�B���h�E�̔w�i�F���O���[��
		pPanelBase->psubobjs->pgraphic->FillRectangle(pPanelBase->pgwinobjs->pBrushBk, pPanelBase->pgwinobjs->rc_panel);
	}return 1; // �w�i�������������Ƃ�����

	case WM_NOTIFY: {
	}break;

	case WM_TIMER: {

	}break;
	case WM_COMMAND: {

	}break;
	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);

		Gdiplus::Bitmap* pbmp_bk = Gdiplus::Bitmap::FromHBITMAP(pPanelBase->pgwinobjs->hBmp_bk, NULL);
		Gdiplus::Bitmap* pbmp_inf = Gdiplus::Bitmap::FromHBITMAP(pPanelBase->pgwinobjs->hBmp_inf, NULL);
		Rect destRect(0, 0, GMAIN_PNL_WND_W, GMAIN_PNL_WND_H);

		//�w�i�摜�`��
		pPanelBase->pgwinobjs->pgraphic->DrawImage(pbmp_bk, destRect, 0, 0, GMAIN_PNL_WND_W, GMAIN_PNL_WND_H, UnitPixel);

		//���摜�̔w�i�������h��Ԃ��ă��b�Z�[�W��������
		PatBlt(pPanelBase->pgwinobjs->hdc_inf, 0, 0, GMAIN_PNL_WND_W, GMAIN_PNL_WND_H, BLACKNESS);
		pPanelBase->pgwinobjs->str_pos_mh->update(); //�̏Ⴡ�b�Z�[�W�X�V

		Status drawStatus = pPanelBase->pgwinobjs->pgraphic->DrawImage(pbmp_inf, destRect, 0, 0, GMAIN_PNL_WND_W, GMAIN_PNL_WND_H, UnitPixel, &pPanelBase->pgwinobjs->attr);

		EndPaint(hwnd, &ps);
	}break;
	case WM_DRAWITEM: {//�����v�\�����X�V TIMER�C�x���g�ŏ�ԕω��`�F�b�N����InvalidiateRect�ŌĂяo��
		DRAWITEMSTRUCT* pDIS = (DRAWITEMSTRUCT*)lParam;

		Gdiplus::Graphics gra(pDIS->hDC);
		Font* pfont = NULL;
		CSubPanelObj* pos = pPanelBase->psubobjs;
		CLampCtrl* plamp = NULL;

	}return true;
	case WM_DESTROY: {
		//�\���X�V�p�^�C�}�[
		KillTimer(hwnd, ID_GMAIN_TIMER);
		// PostQuitMessage(0);
	}return 0;
	case WM_CLOSE: {
		DestroyWindow(hwnd);
	}return 0;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
LRESULT CALLBACK CGraphicWindow::GWndProcHHGH29(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_CREATE: {
		//�O���t�B�b�N�I�u�W�F�N�g�̏�����
		pPanelBase->pgwinobjs->setup_graphics(hwnd);
		pPanelBase->pgwinobjs->refresh_obj_graphics();

		pPanelBase->psubobjs->colorkey.SetValue(Color::Black);//���𓧉�
		Status status = pPanelBase->pgwinobjs->attr.SetColorKey(
			pPanelBase->pgwinobjs->colorkey,
			pPanelBase->pgwinobjs->colorkey,
			ColorAdjustTypeDefault // Default�ł͂Ȃ�Bitmap���w�肷��������m
		);

		//�\���X�V�p�^�C�}�[
		SetTimer(hwnd, ID_GMAIN_TIMER, ID_GMAIN_TIMER_MS, NULL);

	}break;

	case WM_LBUTTONUP: {//�}�E�X���{�^�������Ń��j�^�E�B���h�E�`��X�V
		InvalidateRect(hwnd, NULL, FALSE); // �E�B���h�E�S�̂��ĕ`��
	}
	case WM_CTLCOLORSTATIC: {//�X�^�e�B�b�N�e�L�X�g�̐F�Z�b�g
		SetTextColor((HDC)wParam, RGB(220, 220, 220)); // ���C�g�O���[
		SetBkMode((HDC)wParam, TRANSPARENT);
	}return (LRESULT)GetStockObject(NULL_BRUSH); // �w�i�F�ɍ��킹��

	case WM_ERASEBKGND: {//�E�B���h�E�̔w�i�F����������
		pPanelBase->pgwinobjs->lmg_bk_gwindow->set(0);
		pPanelBase->pgwinobjs->lmg_bk_gwindow->update();
	}return 1; // �w�i�������������Ƃ�����

	case WM_NOTIFY: {
	}break;

	case WM_TIMER: {
		InvalidateRect(hwnd, NULL, false);
	}break;
	case WM_COMMAND: {

	}break;
	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);

		OnPaint(hdc, hwnd); // �`�揈���֐����Ăяo��

		EndPaint(hwnd, &ps);
	}break;
	case WM_DRAWITEM: {//�����v�\�����X�V TIMER�C�x���g�ŏ�ԕω��`�F�b�N����InvalidiateRect�ŌĂяo��
		DRAWITEMSTRUCT* pDIS = (DRAWITEMSTRUCT*)lParam;
		Gdiplus::Graphics gra(pDIS->hDC);
	}return true;
	case WM_DESTROY: {
		//�\���X�V�p�^�C�}�[
	
		// PostQuitMessage(0);
	}return 0;
	case WM_CLOSE: {
		DestroyWindow(hwnd);
	}return 0;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
