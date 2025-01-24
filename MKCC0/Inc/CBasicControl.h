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
#define BC_LISTVIEW_ROW_MAX                 1000    // ���X�g�r���[�s��
#define BC_LISTVIEW_COLUMN_MAX              2       // ���X�g�r���[��
#define BC_LISTVIEW_COLUMN_WIDTH_TIME       90      // ���X�g�r���[��(Time)
#define BC_LISTVIEW_COLUMN_WIDTH_MESSAGE    450     // ���X�g�r���[��(Message)

#define BC_N_FUNC_ITEM                      6     // ���X�g�r���[��(Message)
#define BC_N_ACT_ITEM                       6     // ���X�g�r���[��(Message)

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
    
    DWORD    start_time;        // ���X�L�����̃X���b�h�J�n����
    DWORD    act_time;          // 1�X�L�����̃X���b�h����������
    DWORD    period;            // �X���b�h���[�v��������
    DWORD    act_count;         // �X���b�h���[�v�J�E���^
    DWORD    total_act;         // �N���ώZ�J�E���^
    DWORD	 event_triggered;   // �X���b�h�g���K�C�x���g
    uint32_t time_over_count;   // �\��������I�[�o�[������

    // �֘A�E�B���h�E�n���h��
    HWND hwnd_parent;       // �e�E�B���h�E�̃n���h��
    HWND hwnd_msgstatics;   // �e�E�B���h�E���b�Z�[�W�\���p�E�B���h�E�ւ̃n���h��
    HWND hwnd_opepane;      // �����C���E�B���h�E�̃n���h��(���C���t���[����ɔz�u,�^�u�I���ŕ\���j
    HWND hwnd_msglist;      // �����C���E�B���h�E�̃��b�Z�[�W�\���p���X�g�R���g���[���ւ̃n���h��
    HWND hwnd_work;         // ����p��Ɨp�E�C���h�E�̃n���h��
    HINSTANCE hInstance;    // �A�v���P�[�V�����̃C���X�^���X

    // ����p�l���֘A
    int32_t panel_msglist_count;                                // �p�l�����b�Z�[�W���X�g�̃J�E���g��
    int32_t panel_func_id;                                      // �p�l��function�{�^���̑I����e
    bool panel_act_chk[BC_N_FUNC_ITEM][BC_N_ACT_ITEM];          // �p�l��type�{�^���̑I����e
    int32_t panel_mode_id;                                      // �p�l��type�{�^���̑I����e
    
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
        , start_time(0)
        , act_time(0)
        , period(100)
        , act_count(0)
        , total_act(0)
        , event_triggered(0)
        , time_over_count(1000)
        , hwnd_parent(NULL)
        , hwnd_msgstatics(NULL)
        , hwnd_opepane(NULL)
        , hwnd_msglist(NULL)
        , hwnd_work(NULL)
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
        , psys_counter(NULL)
    {
    }
} THREAD_INFO, * PTHREAD_INFO;

class CBasicControl //��{����N���X
{
public:
    THREAD_INFO inf;
    wostringstream wos;

    LPVOID poutput = NULL;     //���ʏo�̓�����
    DWORD  out_size = 0;        //�o�̓o�b�t�@�̃T�C�Y

    CBasicControl() { inf.status = BC_CODE_STAT_IDLE; }
    ~CBasicControl() { delete inf.hbmp; }
 
    unsigned WINAPI run(void* param) ;        //�X���b�h���s

    virtual unsigned __stdcall initialize(void* param) { return 0; }; //�X���b�h���s

    //���^�X�N�ݒ�^�u�p�l���E�B���h�E�����֐�
    virtual LRESULT CALLBACK PanelProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);

    //�^�u�p�l����Static�e�L�X�g��ݒ�
    virtual void set_panel_tip_txt() { return; };
 
    //�^�u�p�l����Function�{�^����Static�e�L�X�g��ݒ�
    virtual void set_panel_pb_txt() { return; }

    //���C���p�l����StaticWindow�ɃR�����g�o��
    virtual void msg2host(wstring wstr);

    //�^�u�p�l����ListView�ɃR�����g�o��
    virtual void msg2listview(wstring wstr);

    //�p�����[�^�����\���l�ݒ�
    virtual void set_PNLparam_value(float p1, float p2, float p3, float p4, float p5, float p6) ;

    //�^�u�p�l����Function�{�^���̃��Z�b�g
    virtual void reset_panel_func_pb(HWND hDlg) { return; };
 
    virtual int req_command(WORD com_code, WORD param, void* pparam) { return 0; };

private:

    virtual int set_outbuf(LPVOID) { return 0; };         //�o�̓o�b�t�@�Z�b�g
    virtual int input(void* param) { return 0; }                    //���͏���
    virtual int parse(void* param) { return 0; }                    //���C������
    virtual int output(void* param) { return 0; }                   //�o�͏���
    virtual int clear() { return 0; }

	virtual int routine_work(void* param) { 
		input(param);
        parse(param);
        output(param);
        return STAT_OK; 
    };
};

