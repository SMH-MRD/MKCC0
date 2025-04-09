#pragma once

#include "resource.h"

namespace MKCC
{
#define N_MKCC_TASK                  6

    //-�e�^�X�N�A�C�R���p�C���[�W���X�g�ݒ�l
#define ICON_IMG_W					32		//�A�C�R���C���[�W��
#define ICON_IMG_H					32		//�A�C�R���C���[�W����

//-Tweet Message�\���pStatic Window�̃T�C�Y�ݒ�l
#define MSG_WND_H				20		//Height of tweet window
#define MSG_WND_W				590		//Width of  tweet window
#define MSG_WND_ORG_X			35		//Origin X of tweet window position 
#define MSG_WND_ORG_Y			10		//Origin Y of tweet window position 
#define MSG_WND_Y_SPACE			12		//Space between tweet windows Y direction 
#define TWEET_IMG_ORG_X			5		//Origin X of bitmap tweet icon position

//-�^�X�N�ݒ�^�u�z�u�ݒ�
#define TAB_DIALOG_W			610
#define TAB_DIALOG_H			320
#define TAB_POS_X				5
#define TAB_POS_Y				10
#define TAB_SIZE_H				35
#define TAB_SPACE			    20		 

#define MAIN_WND_X              0
#define MAIN_WND_Y              0
#define MAIN_WND_W              640
#define MAIN_WND_H              460
#define MAIN_WND_MIN_H          100

#define ENV_SCAN_MS			    20
#define CS_SCAN_MS				100
#define AGENT_SCAN_MS			20
#define POLICY_SCAN_MS			100
#define SCADA_SCAN_MS			100
#define SIM_SCAN_MS			    20

#define NAME_OF_INIFILE					L"mkcc"			//ini�t�@�C���t�@�C����
#define EXT_OF_INIFILE					L"ini"			//ini�t�@�C���g���q
#define PATH_OF_INIFILE					pszInifile		//ini�t�@�C���p�X

#define OPATH_SECT_OF_INIFILE			L"PATH"			//�I�v�V���i���p�X�Z�N�V����
#define OBJ_NAME_SECT_OF_INIFILE		L"OBJECT_NAME"	//�^�X�N�I�u�W�F�N�g�t���l�[���Z�N�V����
#define OBJ_SNAME_SECT_OF_INIFILE		L"OBJECT_SNAME"	//�^�X�N�I�u�W�F�N�g���̃Z�N�V����
#define NETWORK_SECT_OF_INIFILE			L"NW_PARAM"		//Communicator�^�X�N�Z�N�V����
#define SMEM_SECT_OF_INIFILE			L"SMEM"			//���L�������Z�N�V����
#define SYSTEM_SECT_OF_INIFILE			L"SYSTEM"		//�V�X�e���ݒ�Z�N�V����
#define SYSTEM_SECT_OF_SIMULATION		L"SIM"		    //�V�~�����[�V�����Z�N�V����

//�V�X�e���֘A�L�[
#define ODER_CODE_KEY_OF_INIFILE        L"ORDER_CODE"
#define PC_TYPE_KEY_OF_INIFILE          L"PC_TYPE"
#define PC_SERIAL_KEY_OF_INIFILE        L"PC_SERIAL"
#define PC_OPTION_KEY_OF_INIFILE        L"PC_OPTION"

//�e�Z�N�V�����̊e�^�X�N�p�̏����Q�Ƃ��邽�߂̃L�[
#define MAIN_KEY_OF_INIFILE				L"MAIN"
#define ENV_KEY_OF_INIFILE	            L"ENV"
#define CS_KEY_OF_INIFILE	            L"CLIENT"
#define POL_KEY_OF_INIFILE	            L"POL"
#define AGENT_KEY_OF_INIFILE	        L"AGENT"
#define SCAD_KEY_OF_INIFILE	            L"SCAD"
#define SIM_KEY_OF_INIFILE	            L"SIM"

     //-ID��` Main�X���b�h�p�@5100 +��
#define ID_TASK_SET_TAB				5098
#define ID_STATUS_BAR				5099
#define IDC_OBJECT_BASE				5100

#define PRM_N_TASK_MSGLIST_ROW      100 //�^�X�N���b�Z�[�W���X�g�s��
#define PRM_N_MCC_COLOR_BRUSH       8   //�^�X�N���b�Z�[�W���X�g�s��

    typedef struct _ST_MAIN_WND {
        DWORD com_static_set     = 0;//�^�X�N���b�Z�[�W�\���X�^�e�B�b�N�̐ݒ�R�}���h
        DWORD com_prm_set_color  = 0;
        HBRUSH hbrush[PRM_N_MCC_COLOR_BRUSH];
        HWND hWnd_status_bar;
        HWND hWnd;
    } ST_MAIN_WND, * LPST_MAIN_WND;
}

