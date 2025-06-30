#pragma once

#include "CBasicControl.h"

#include "framework.h"
#include "CSHAREDMEM.H"

#define SIM_MON1_WND_X     640
#define SIM_MON1_WND_Y     0
#define SIM_MON1_WND_W     320
#define SIM_MON1_WND_H     240
#define SIM_MON1_N_CTRL    32
#define SIM_MON1_N_WCHAR   64

#define SIM_ID_MON1_CTRL_BASE   56100
#define SIM_ID_MON1_STATIC_INF0     0

#define SIM_ID_MON2_CTRL_BASE   56140

#define SIM_ID_MON1_TIMER  56190
#define SIM_ID_MON2_TIMER  56191

#define SIM_PRM_MON1_TIMER_MS  200
#define SIM_PRM_MON2_TIMER_MS  200


typedef struct _ST_SIM_MON1 {
    int timer_ms = SIM_PRM_MON1_TIMER_MS;
    wostringstream wo;

    HWND hwnd_mon;
    HWND hctrl[SIM_MON1_N_CTRL] = {
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
    };
    POINT pt[SIM_MON1_N_CTRL] = {
        5,5, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0
    };
    SIZE sz[SIM_MON1_N_CTRL] = {
        295,190, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0
    };
    WCHAR text[SIM_MON1_N_CTRL][SIM_MON1_N_WCHAR] = {
        L"-", L"", L"", L"", L"", L"", L"", L"",
        L"", L"", L"", L"", L"", L"", L"", L"",
        L"", L"", L"", L"", L"", L"", L"", L"",
        L"", L"", L"", L"", L"", L"", L"", L""
    };
}ST_SIM_MON1, * LPST_SIM_MON1;

#define SIM_MON2_WND_X     SIM_MON1_WND_X
#define SIM_MON2_WND_Y     SIM_MON1_WND_Y + SIM_MON1_WND_H   
#define SIM_MON2_WND_W     320
#define SIM_MON2_WND_H     240

typedef struct _ST_SIM_MON2 {
    HWND hwnd_mon;

}ST_SIM_MON2, * LPST_SIM_MON2;

typedef struct _ST_SIM_LOAD {//����
    double m;	//����
    double wx;	//��
    double dy;	//���s
    double hz;	//����
}ST_SIM_LOAD, * LPST_SIM_LOAD;

typedef struct _ST_SIM_AXIS {//��
    double mtrq;					//���[�^�g���Nfb
    ST_SIM_LOAD load;				//�����ׁi���͉׏d�j
    ST_MOVE_SET	ref;				//���w��
    ST_MOVE_SET	fb;					//�����W
    ST_MOVE_SET	nd;					//�h������]����(���x��rps)
    INT32  i_layer;				    //�h�������ݑw��
	INT32  hcount;				    //�����J�E���^�l
	INT32  absocoder;				//�A�u�\�R�[�_�l
    double  n_layer;				//�h�������ݑw���搔
    double  l_drum;					//�h����������
	INT32   brake;					//�u���[�L���
}ST_SIM_AXIS, * LPST_SIM_AXIS;

typedef struct _ST_CC_SIM_WORK {
    //���L�������o��
    DWORD mode;
    DWORD helthy_cnt;
 
    ST_AUX_ENV_INF sway_io;
	ST_PLC_IO_WIF st_plc_w;			                //PLC IO��������IF

    ST_SWAY_SERVER_MSG rcv_msg;
    ST_SWAY_CLIENT_MSG snd_msg;
    
    ST_SIM_AXIS axis[MOTION_ID_MAX];				//���v�Z�l

    Vector3 L, vL,L2, vL2;							//۰���޸��(�努�U��j(�⊪�U��j
    double kbh;										//�������a�Ɉˑ����鑬�x�A�����x�␳�W��   
    ST_MOVE_SET	d;									//�|�X�g�]�N���V�[�u�ԏ�ԁi�����E���x�E�����x�j
    ST_MOVE_SET	db;									//�W�u�|�X�g�]�N���V�[�u�ԏ�ԁi�����E���x�E�����x�j
    ST_MOVE_SET	ph;									//��
    ST_MOVE_SET	phb;								//��
    ST_MOVE_SET	th;									//��

    double rad_cam_x, rad_cam_y, w_cam_x, w_cam_y;	//�J�������W�U��p,�U��p���x
    double T;										//�努�U�����
    double Tah;										//�⊪�U�����
    double w;										//�努�U��p���g��
    double wah;										//�⊪�U��p���g��
    double tht_swx_mh, tht_swy_mh, tht_swx_ah, tht_swy_ah;

}ST_CC_SIM_WORK, * LPST_CC_SIM_WORK;

class CSim : public CBasicControl
{
public:
    CSim();
    ~CSim();

    virtual HRESULT initialize(LPVOID lpParam) override;

    LRESULT CALLBACK PanelProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);

    static LRESULT CALLBACK Mon1Proc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);
    static LRESULT CALLBACK Mon2Proc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);

    static ST_SIM_MON1 st_mon1;
    static ST_SIM_MON2 st_mon2;

    //�^�X�N�o�͗p�\����
    static ST_CC_SIM_INF st_sim_inf;
    static ST_CC_SIM_WORK st_work;

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

	HRESULT init_drm_motion();  //�h�����p�����[�^�ݒ�(�����,�w��,���x,�����x�j
    HRESULT set_drm_condition();//�h������Ԑݒ�(�u���[�L,�Ɍ�,�׏d�j
	HRESULT set_drm_motion();   //�h��������ݒ�(�����x,���x,�ʒu,�w�j
    HRESULT set_sensor_fb();            //�����J�E���^,�A�u�\�R�[�_,LS��
	
    HRESULT calc_axis_motion();         //���ד���v�Z
    HRESULT calc_load_motion();         //�݉ד���v�Z
	HRESULT calc_plc_output();          //����ԏo�́iPLC IO�p�j

};