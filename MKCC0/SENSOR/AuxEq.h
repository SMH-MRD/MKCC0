#pragma once

#include "resource.h"

namespace AUXEQ
{

#define N_AUXEQ_TASK                  6

//-�e�^�X�N�A�C�R���p�C���[�W���X�g�ݒ�l
#define ICON_IMG_W					32		//�A�C�R���C���[�W��
#define ICON_IMG_H					32		//�A�C�R���C���[�W����

//-Tweet Message�\���pStatic Window�̃T�C�Y�ݒ�l
#define MSG_WND_H					20		//Height of tweet window
#define MSG_WND_W					590		//Width of  tweet window
#define MSG_WND_ORG_X				35		//Origin X of tweet window position 
#define MSG_WND_ORG_Y				10		//Origin Y of tweet window position 
#define MSG_WND_Y_SPACE				12		//Space between tweet windows Y direction 
#define TWEET_IMG_ORG_X				5		//Origin X of bitmap tweet icon position

//-�^�X�N�ݒ�^�u�z�u�ݒ�
#define TAB_DIALOG_W				610
#define TAB_DIALOG_H				320
#define TAB_POS_X					5
#define TAB_POS_Y					10
#define TAB_SIZE_H					35
#define TAB_SPACE				    20		 

#define AUXEQ_WND_X                0+640+640
#define AUXEQ_WND_Y                0
#define AUXEQ_WND_W                640
#define AUXEQ_WND_H                460
#define AUXEQ_WND_MIN_H            100


#define TILT_SCAN_MS			100
#define CS_SCAN_MS				100


//�}���`���f�B�A�^�C�}�[�Ǘ��\����
#define TARGET_RESOLUTION			1		//�}���`���f�B�A�^�C�}�[����\ msec
#define SYSTEM_TICK_ms				20		//���C���X���b�h���� msec
#define INITIAL_TASK_STACK_SIZE		16384	//�^�X�N�I�u�W�F�N�g�X���b�h�p�X�^�b�N�T�C�Y

#define NAME_OF_INIFILE					L"ote"			//ini�t�@�C���t�@�C����
#define EXT_OF_INIFILE					L"ini"			//ini�t�@�C���g���q
#define PATH_OF_INIFILE					pszInifile		//ini�t�@�C���p�X

#define OPATH_SECT_OF_INIFILE			L"PATH"			//�I�v�V���i���p�X�Z�N�V����
#define OBJ_NAME_SECT_OF_INIFILE		L"OBJECT_NAME"	//�^�X�N�I�u�W�F�N�g�t���l�[���Z�N�V����
#define OBJ_SNAME_SECT_OF_INIFILE		L"OBJECT_SNAME"	//�^�X�N�I�u�W�F�N�g���̃Z�N�V����
#define NETWORK_SECT_OF_INIFILE			L"NW_PARAM"		//Communicator�^�X�N�Z�N�V����
#define SMEM_SECT_OF_INIFILE			L"SMEM"			//���L�������Z�N�V����
#define SYSTEM_SECT_OF_INIFILE			L"SYSTEM"		//�V�X�e���ݒ�Z�N�V����

//�e�Z�N�V�����̊e�^�X�N�p�̏����Q�Ƃ��邽�߂̃L�[
#define MAIN_KEY_OF_INIFILE				L"MAIN"
#define TILT_KEY_OF_INIFILE	            L"TILT"
#define CS_KEY_OF_INIFILE	            L"CLIENT"




    //-ID��` Main�X���b�h�p�@2100 +��
#define ID_TASK_SET_TAB				2098
#define ID_STATUS_BAR				2099
#define IDC_OBJECT_BASE				2100


#define PRM_N_TASK_MSGLIST_ROW      100 

    typedef struct stSensorWnd {
        HWND hWnd_status_bar;
        HWND hWnd;
    } ST_AUXEQ_WND, * LPST__AUXEQ_WND;

}

