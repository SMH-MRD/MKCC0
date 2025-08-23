#pragma once

#include <windows.h>
#include <iostream>
#include <iomanip>
#include <sstream>

using namespace std;

#define BC_CODE_COM_TERMINATE    0x80000000
#define BC_CODE_COM_REPEAT_SCAN  0x00000001

#define BC_CODE_STAT_IDLE        0x00000000
#define BC_CODE_STAT_INIT_REQ    0x00000001
#define BC_CODE_STAT_INIT_WAIT   0x00000002
#define BC_CODE_STAT_RUN_REQ     0x00000004
#define BC_CODE_STAT_RUNNING     0x00000008
#define BC_CODE_STAT_ERROR       0x00000010
#define BC_CODE_STAT_INIT_ERROR  0x00000020

#define BC_EVENT_TYPE_TIME       0x00000000
#define BC_EVENT_TYPE_SEQENCE    0x00000001

#define BC_CODE_TYPE_EVENT       0x00000000
#define BC_CODE_TYPE_PERIODIC    0x00000001

#define BC_EVENT_MAX                        8
#define BC_DEFAUT_THREAD_CYCLE	            25		//������N���f�t�H���g�l msec
#define BC_LISTVIEW_ROW_MAX                 100     // ���X�g�r���[�s��
#define BC_LISTVIEW_COLUMN_MAX              2       // ���X�g�r���[��
#define BC_LISTVIEW_COLUMN_WIDTH_TIME       90      // ���X�g�r���[��(Time)
#define BC_LISTVIEW_COLUMN_WIDTH_MESSAGE    450     // ���X�g�r���[��(Message)

#define BC_N_FUNC_ITEM                      6     // ���X�g�r���[��(Message)
#define BC_N_ACT_ITEM                       6     // ���X�g�r���[��(Message)

#define BC_ID_MODE0                         0
#define BC_ID_MODE1                         1
#define BC_ID_MODE2                         2

#define BC_ID_MON1                          0
#define BC_ID_MON2                          1

#define BC_TASK_NAME_BUF_SIZE               32

#define BC_BK_COLOR_DEFAULT                 0
#define BC_BK_COLOR_LIGHT_GREEN             1
#define BC_BK_COLOR_LIGHT_YELLOW            2
#define BC_BK_COLOR_LIGHT_BLUE              3
#define BC_BK_COLOR_LIGHT_RED               4

#define BC_EVENT_REC_SET_BBK_COLOR          0x00000001

// �^�X�N�X���b�hID
struct BC_TASK_ID
{
    int ENV;
    int CS;
    int SCAD;
    int POL;
    int AGENT;
    int SIM;
    int NUM_OF_THREAD;
};

//�}���`���f�B�A�^�C�}�[�Ǘ��\����
#define TARGET_RESOLUTION			1		//�}���`���f�B�A�^�C�}�[����\ msec
#define SYSTEM_TICK_ms				20		//���C���X���b�h���� msec
#define INITIAL_TASK_STACK_SIZE		16384	//�^�X�N�I�u�W�F�N�g�X���b�h�p�X�^�b�N�T�C�Y

typedef struct stKnlManageSetTag {
    WORD mmt_resolution = TARGET_RESOLUTION;			//�}���`���f�B�A�^�C�}�[�̕���\
    unsigned int cycle_base = SYSTEM_TICK_ms;			//�}���`���f�B�A�^�C�}�[�̕���\
    WORD KnlTick_TimerID = 0;							//�}���`���f�B�A�^�C�}�[��ID
    unsigned int num_of_task = 0;						//�A�v���P�[�V�����ŗ��p����X���b�h��
    unsigned long sys_counter = 0;						//�}���`���f�B�A�N���^�C�}�J�E���^
    SYSTEMTIME Knl_Time = { 0,0,0,0,0,0,0,0 };			//�A�v���P�[�V�����J�n����̌o�ߎ���
    unsigned int stackSize = INITIAL_TASK_STACK_SIZE;	//�^�X�N�̏����X�^�b�N�T�C�Y
}ST_KNL_MANAGE_SET, * LPST_KNL_MANAGE_SET;

typedef struct _tagTHREAD_INFO {
    // �I�u�W�F�N�g���ʏ��
    std::wstring name;  // �I�u�W�F�N�g��
    std::wstring sname; // �I�u�W�F�N�g����
    HBITMAP      hbmp;  // �I�u�W�F�N�g���ʗp�r�b�g�}�b�v
    HINSTANCE	hinstance; // �C���X�^���X�n���h��

    // �X���b�h�ݒ���e
    int32_t  index;                     // �X���b�hIndex
    uint32_t id;                        // �X���b�hID
    HANDLE   hndl;                      // �X���b�h�n���h��
    HANDLE   hevents[BC_EVENT_MAX];     // �C�x���g�n���h��
    int32_t  n_active_events;           // �L���ȃC�x���g��

    uint32_t cycle_ms;                  // �X���b�h���s�ݒ����
    uint32_t cycle_count;               // �X���b�h���s�ݒ���� Tick count(ms/system tick)
    uint32_t retry_count;               // �X���b�h���s�ݒ���� Tick count(ms/system tick)
    int32_t  trigger_type;              // �X���b�h�N������ ����� or �C�x���g
    int32_t  priority;                  // �X���b�h�̃v���C�I���e�B
    int32_t  status;                    // �X���b�h�����ԃt���O 0:�J��Ԃ� 1:���f ����ȊO:��������:
    int32_t  command;                    // �X���b�h�����ԃt���O 0:�J��Ԃ� 1:���f ����ȊO:��������:

    // �X���b�h���j�^���
    LARGE_INTEGER sys_count;
    LARGE_INTEGER sys_freq;
    
    LONGLONG    start_time;        // ���X�L�����̃X���b�h�J�n����
    LONGLONG    act_time;          // 1�X�L�����̃X���b�h����������
    LONGLONG    period;            // �X���b�h���[�v��������
    LONGLONG    act_count;         // �X���b�h���[�v�J�E���^
    LONGLONG    total_act;         // �N���ώZ�J�E���^
    LONGLONG	event_triggered;   // �X���b�h�g���K�C�x���g
    uint32_t    time_over_count;   // �\��������I�[�o�[������

	LONGLONG    cnt_unit_ms;       // �p�t�H�[�}���X�J�E���^�̒P�ʎ���(1ms������̃J�E���g��)
	LONGLONG    cnt_unit_us;      // �p�t�H�[�}���X�J�E���^�̒P�ʎ���(1us������̃J�E���g��)
	double      dt;                // ���уX�L�����i�ʕb�P�ʂ̃J�E���g���j

    // �֘A�E�B���h�E�n���h��
    HWND hwnd_parent;       // �e�E�B���h�E�̃n���h��
    HWND hwnd_msgstatics;   // �e�E�B���h�E���b�Z�[�W�\���p�E�B���h�E�ւ̃n���h��
    HWND hwnd_opepane;      // �����C���E�B���h�E�̃n���h��(���C���t���[����ɔz�u,�^�u�I���ŕ\���j
    HWND hwnd_msglist;      // �����C���E�B���h�E�̃��b�Z�[�W�\���p���X�g�R���g���[���ւ̃n���h��
    HINSTANCE hInstance;    // �A�v���P�[�V�����̃C���X�^���X

    // ����p�l���֘A
    int32_t panel_msglist_count;                                // �p�l�����b�Z�[�W���X�g�̃J�E���g��
    int32_t panel_func_id;                                      // �p�l��function�{�^���̑I����e
    bool panel_act_chk[BC_N_FUNC_ITEM][BC_N_ACT_ITEM];          // �p�l��type�{�^���̑I����e
    int32_t mode_id;                                            // 
    
    // �O���C���^�[�t�F�[�X
    unsigned long* psys_counter;    // ���C���V�X�e���J�E���^�[�̎Q�Ɛ�|�C���^

    _tagTHREAD_INFO()
        : name(L"no name")
        , sname(L"TBD")
        , hbmp(NULL)
        , hInstance(NULL)
        , index(0)
        , id(0)
        , hndl(NULL)
        , hevents{}
        , n_active_events(1)
        , cycle_ms(BC_DEFAUT_THREAD_CYCLE)
        , cycle_count(4)
        , retry_count(100)
        , trigger_type(0)
        , priority(THREAD_PRIORITY_NORMAL)
        , status(BC_CODE_STAT_IDLE)
        , command(BC_CODE_COM_REPEAT_SCAN)
        , sys_count{0}
        , sys_freq{1000}
        , start_time(0)
        , act_time(0)
        , period(100)
        , act_count(0)
        , total_act(0)
        , event_triggered(0)
        , time_over_count(1000)

        ,cnt_unit_ms(0) 
        ,cnt_unit_us(0) 
        ,dt(1.0)   

        , hwnd_parent(NULL)
        , hwnd_msgstatics(NULL)
        , hwnd_opepane(NULL)
        , hwnd_msglist(NULL)
        , hinstance(NULL)
        , panel_msglist_count(0)
        , panel_func_id(0)
        , panel_act_chk{
            false, false, false, false, false, false,
            false, false, false, false, false, false,
            false, false, false, false, false, false,
            false, false, false, false, false, false,
            false, false, false, false, false, false,
            false, false, false, false, false, false
        }
        , mode_id(0)
        , psys_counter(NULL)
    {
    }
} THREAD_INFO, * PTHREAD_INFO;

class CBasicControl //��{����N���X
{
public:
    THREAD_INFO inf;
    wostringstream wos;
    ULONGLONG err;

    LPVOID poutput = NULL;     //���ʏo�̓�����
    DWORD  out_size = 0;        //�o�̓o�b�t�@�̃T�C�Y

    CBasicControl() { inf.status = BC_CODE_STAT_IDLE; }
    ~CBasicControl() { delete inf.hbmp; }
 
    unsigned WINAPI run(void* param) ;        //�X���b�h���s

    virtual HRESULT __stdcall initialize(void* param) ; 

    //���^�X�N�ݒ�^�u�p�l���E�B���h�E�����֐�
    virtual LRESULT CALLBACK PanelProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);

    //�^�u�p�l����Static�e�L�X�g��ݒ�
    virtual void set_panel_tip_txt();
    //�^�u�p�l����Function�{�^����Static�e�L�X�g��ݒ�
    virtual void set_func_pb_txt();
    //�^�u�p�l����Item chk�e�L�X�g��ݒ�
    virtual void set_item_chk_txt();
    //���C���p�l����StaticWindow�ɃR�����g�o��
    virtual void msg2host(wstring wstr);
    virtual void set_color_of_hostmsg(int txtr, int txtg, int txtb, int bkr, int bkg, int bkb);

    //�^�u�p�l����ListView�ɃR�����g�o��
    virtual void msg2listview(wstring wstr);

    //�p�����[�^�����\���l�ݒ�
    virtual void set_PNLparam_value(float p1, float p2, float p3, float p4, float p5, float p6) ;

    //�^�u�p�l����Function�{�^���̃��Z�b�g
    virtual void reset_panel_func_pb(HWND hDlg) { return; };
 
    virtual int req_command(WORD com_code, WORD param, void* pparam) { return 0; };

   virtual int set_outbuf(LPVOID) { return 0; };         //�o�̓o�b�t�@�Z�b�g
   virtual int close() { return 0; }

private:
 
    virtual int input(void* param) { return 0; }                    //���͏���
    virtual int parse(void* param) { return 0; }                    //���C������
    virtual int output(void* param) { return 0; }                   //�o�͏���

	virtual HRESULT routine_work(void* param) { 
        if (inf.total_act % 20 == 0) {
            wos.str(L""); wos << inf.status << L":" << std::setfill(L'0') << std::setw(4) << inf.act_time;
            msg2host(wos.str());
        }
		input(param);
        parse(param);
        output(param);
        return S_OK; 
    };
};

