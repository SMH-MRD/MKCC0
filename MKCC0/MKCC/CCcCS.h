#pragma once
#include "CBasicControl.h"
#include "CSHAREDMEM.H"

#define CS_SEMIAUTO_TG_SEL_DEFAULT      0
#define CS_SEMIAUTO_TG_SEL_CLEAR		0
#define CS_SEMIAUTO_TG_SEL_ACTIVE       1
#define CS_SEMIAUTO_TG_SEL_FIXED        2
#define CS_SEMIAUTO_TG_MAX				16

#define CS_ID_SEMIAUTO_TOUCH_PT			8

#define CS_ID_NOTCH_POS_HOLD			0
#define CS_ID_NOTCH_POS_TRIG			1

#define CS_CODE_OTE_REMOTE_ENABLE		0x0001	//PLC�ւ̑���M���L��
#define CS_CODE_OTE_SOURCE_ENABLE		0x0002	//�劲ON�\

#define CS_ID_MON1_TIMER  51190
#define CS_ID_MON2_TIMER  51191

#define CS_PRM_MON1_TIMER_MS  200
#define CS_PRM_MON2_TIMER_MS  200

#define CS_MON1_WND_X     0
#define CS_MON1_WND_Y     0
#define CS_MON1_WND_W     320
#define CS_MON1_WND_H     240
#define CS_MON1_N_CTRL    32
#define CS_MON1_N_WCHAR   64

#define CS_MON1_N_CTRL    32
#define CS_MON1_N_WCHAR   64

#define CS_ID_MON1_CTRL_BASE   51100
#define CS_ID_MON1_STATIC_1     0

typedef struct _ST_CS_MON1 {
    int timer_ms = CS_PRM_MON1_TIMER_MS;
    HWND hwnd_mon;
    bool is_monitor_active = false; //�\�P�b�g����\��������e�R�[�h

    HWND hctrl[CS_MON1_N_CTRL] = {
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
    };
    POINT pt[CS_MON1_N_CTRL] = {
        5,5, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0
    };
    SIZE sz[CS_MON1_N_CTRL] = {
        295,190, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0
    };
    WCHAR text[CS_MON1_N_CTRL][CS_MON1_N_WCHAR] = {
        L"MON1", L"", L"", L"", L"", L"", L"", L"",
        L"", L"", L"", L"", L"", L"", L"", L"",
        L"", L"", L"", L"", L"", L"", L"", L"",
        L"", L"", L"", L"", L"", L"", L"", L""
    };
}ST_CS_MON1, * LPST_CS_MON1;

#define CS_MON2_WND_X     CS_MON1_WND_X
#define CS_MON2_WND_Y     620   
#define CS_MON2_WND_W     640
#define CS_MON2_WND_H     345

#define CS_MON2_N_CTRL    32
#define CS_MON2_N_WCHAR   64

#define CS_ID_MON2_CTRL_BASE   51140
#define CS_ID_MON2_STATIC_UNI      0
#define CS_ID_MON2_STATIC_MUL_PC   1
#define CS_ID_MON2_STATIC_MUL_OTE  2
#define CS_ID_MON2_LABEL_UNI       3
#define CS_ID_MON2_LABEL_MUL_PC    4
#define CS_ID_MON2_LABEL_MUL_OTE   5
#define CS_ID_MON2_STATIC_MSG      6
#define CS_ID_MON2_RADIO_RCV       16
#define CS_ID_MON2_RADIO_SND       17
#define CS_ID_MON2_RADIO_INFO      18
#define CS_ID_MON2_CB_BODY         19
#define CS_ID_MON2_PB_PAGE_UP_UNI  20
#define CS_ID_MON2_PB_PAGE_UP_MPC  21
#define CS_ID_MON2_PB_PAGE_UP_MOT  22

#define CS_ID_MON2_N_PAGE_UP_UNI   4
#define CS_ID_MON2_N_PAGE_UP_MPC   2
#define CS_ID_MON2_N_PAGE_UP_MOT   2

typedef struct _ST_CS_MON2 {
    HWND hwnd_mon;
    int timer_ms = CS_PRM_MON2_TIMER_MS;
	int sock_inf_id = CS_ID_MON2_RADIO_RCV;//�\�P�b�g����\��������e�R�[�h
    int ipage_uni = 0, ipage_mpc = 0, ipage_mot = 0;

    bool is_monitor_active = false; //���j�^��ʕ\�����t���O
    bool is_body_disp_mode = false; // �d���\�����[�hBODY

    wostringstream wo_uni, wo_mpc, wo_mote, wo_work;

    HWND hctrl[CS_MON2_N_CTRL] = {
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
    };
    POINT pt[CS_MON2_N_CTRL] = {
        5,50, 5,135, 5,220, 5,30, 5,115, 5,200, 5,5,0,0,  
        0,0, 0,0,0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        470,5,520,5,570,5, 380,285, 455,285, 510,285, 565,285, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0
    };
    SIZE sz[CS_MON2_N_CTRL] = {
        CS_MON2_WND_W- 25,60, CS_MON2_WND_W- 25,60, CS_MON2_WND_W- 25,60, CS_MON2_WND_W - 25,20, CS_MON2_WND_W - 25,20, CS_MON2_WND_W - 25,20, CS_MON2_WND_W - 180,20,0,0,
        0,0, 0,0,  0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        40,20, 40,20, 40,20, 65,20, 45,20, 45,20, 45,20, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0
    };
    WCHAR text[CS_MON2_N_CTRL][CS_MON2_N_WCHAR] = {
        L"-", L"-", L"-", L"UNI:", L"MPC:", L"MOTE:", L"MSG:",L"",
         L"",L"",  L"", L"", L"", L"", L"", L"",
        L"RCV", L"SND", L"INFO", L"BODY", L"UNI+", L"MPC+", L"MOT+", L"",
        L"", L"", L"", L"", L"", L"", L"", L""
    };

}ST_CS_MON2, * LPST_CS_MON2;

class CCcCS : public CBasicControl
{
public:
    CCcCS();
    ~CCcCS();

    virtual HRESULT initialize(LPVOID lpParam) override;

    LRESULT CALLBACK PanelProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);

    static LRESULT CALLBACK Mon1Proc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);
    static LRESULT CALLBACK Mon2Proc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);

    static ST_CS_MON1 st_mon1;
    static ST_CS_MON2 st_mon2;

    //�^�X�N�o�͗p�\����
    static ST_CC_CS_INF st_cs_work;
    static ST_CC_OTE_INF st_ote_work;
    
    static HRESULT rcv_uni_ote(LPST_OTE_U_MSG pbuf);
	static HRESULT rcv_mul_pc(LPST_PC_M_MSG pbuf);
    static HRESULT rcv_mul_ote(LPST_OTE_M_MSG pbuf);
	
    static LPST_PC_U_MSG set_msg_u(BOOL is_ope_mode, INT32 code,INT32 stat);
    static HRESULT snd_uni2ote(LPST_PC_U_MSG pbuf, SOCKADDR_IN* p_addrin_to);

	static LPST_PC_M_MSG set_msg_m(INT32 code, INT32 stat);
	static HRESULT snd_mul2pc(LPST_PC_M_MSG pbuf);
    static HRESULT snd_mul2ote(LPST_PC_M_MSG pbuf);


    //�^�u�p�l����Static�e�L�X�g��ݒ�
    virtual void set_panel_tip_txt() override;
    //�^�u�p�l����Function�{�^����Static�e�L�X�g��ݒ�
    virtual void set_func_pb_txt() override;
    //�^�u�p�l����Item chk�e�L�X�g��ݒ�
    virtual void set_item_chk_txt() override;
    //�^�u�p�l����ListView�ɃR�����g�o��
    virtual void msg2listview(wstring wstr) override;

    //�p�����[�^�����\���l�ݒ�
    virtual void set_PNLparam_value(float p1, float p2, float p3, float p4, float p5, float p6) override;

    //�^�u�p�l����Function�{�^���̃��Z�b�g
    virtual void reset_panel_func_pb(HWND hDlg) override { return; };

private:

    //�I�[�o�[���C�h

    virtual HRESULT routine_work(void* pObj) override;

    HWND open_monitor_wnd(HWND h_parent_wnd, int id);
    void close_monitor_wnd(int id);
    void show_monitor_wnd(int id);
    void hide_monitor_wnd(int id);


    int set_outbuf(LPVOID) {//�o�̓o�b�t�@�Z�b�g
        return STAT_NG;
    }

    int input();//���͏���

    int parse();
    int output();
    int close();
};

