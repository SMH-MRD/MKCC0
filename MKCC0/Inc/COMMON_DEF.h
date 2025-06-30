#pragma once
#include <windows.h>

//@ GDI+
#include <objbase.h>//gdiplus.h�̃R���p�C����ʂ����߂ɕK�v
#include <gdiplus.h>
using namespace Gdiplus;

//#define _DVELOPMENT_MODE                //�J������L���ɂ���

///# �R�[�h
#pragma region ID
//�N���[����� ID  �z��INDEX
#define CRANE_TYPE_MAX          16  //�J��
#define CRANE_TYPE_JC0          0   //�W�u�N���[�� �努�̂�
#define CRANE_TYPE_JC1          1   //�W�u�N���[�� �⊪����
#define CRANE_TYPE_GC           2   //�S���C�A�X�N���[��

//�N���[������ �R�[�h
#define CRANE_ID_CODE_MASK      0x0000FF00           
#define CRANE_ID_MAX            255          //
#define CRANE_ID_NULL           0x0000      //����
#define CRANE_ID_H6R602         0x0100      //�݂炢
#define CARNE_ID_HHGH29         0x0200      //���������x��70t�@102��  
#define CARNE_ID_HHGQ18         0x0300      //���������x��300t 10��
#define CARNE_ID_HHFR22         0x0400      //�����ۋT1200t 1��
#define CARNE_ID_PC0            0xFF00      //PC�P��
#pragma endregion ����ID

#pragma region STATUS
#define ID_UP                   0   //�㑤
#define ID_DOWN                 1   //����
#define ID_FWD                  0   //�O�i
#define ID_REV                  1   //��i
#define ID_RIGHT                0   //�E��
#define ID_LEFT                 1   //����
#define ID_ACC                  0   //����
#define ID_DEC                  1   //����

#define ID_STOP                 -1  //��~
#define ID_SELECT               2   //�I��

#define ID_X                    0   // X����
#define ID_Y                    1   // Y����
#define ID_Z                    2   // Z����

#define ID_T                    0   // �ڐ�����
#define ID_R		            1   // ���a����


#define N_NOTCH_MAX	            6	            //�m�b�`�ő吔�@0�m�b�`���܂�
#define N_NOTCH_TBL	            N_NOTCH_MAX*2	//�m�b�`�e�[�u�����@0�m�b�`���܂�
#define NOTCH_0	                0
#define NOTCH_1	                1
#define NOTCH_2	                2
#define NOTCH_3	                3
#define NOTCH_4	                4
#define NOTCH_5	                5

#define NOTCH_F1	            1
#define NOTCH_F2	            2
#define NOTCH_F3	            3
#define NOTCH_F4	            4
#define NOTCH_F5	            5
#define NOTCH_C0                6
#define NOTCH_R1	            7
#define NOTCH_R2	            8
#define NOTCH_R3	            9
#define NOTCH_R4	            10
#define NOTCH_R5	            11

#define FUNC_DEACTIVE           0
#define FUNC_ACTIVE             1
#define FUNC_PAUSE              2
#define FUNC_RESTART            3

#define CODE_MODE0				0   //���[�h0
#define CODE_MODE1				1   //���[�h1
#define CODE_MODE2				2   //���[�h2
#define CODE_MODE3				3   //���[�h3
#define CODE_MODE4				4   //���[�h4
#define CODE_MODE5				5   //���[�h5
#define CODE_MODE6				6   //���[�h6
#define CODE_MODE7				7   //���[�h7

#define CODE_DIR_STP			0   //��~
#define CODE_DIR_FWD			1   //���]
#define CODE_DIR_REV			-1  //�t�]

#pragma endregion �X�e�[�^�X�R�[�h

#pragma region TYPE
#define UNIT_CODE_M             0   //�P�ʂ̃R�[�h m
#define UNIT_CODE_RAD           1   //�P�ʂ̃R�[�h rad
#define UNIT_CODE_MM            2   //�P�ʂ̃R�[�h mm
#define UNIT_CODE_DEG           3   //�P�ʂ̃R�[�h deg

//PC TYPE CODE
#define ID_PC_TYPE_DEFAULT      0
#define ID_PC_TYPE_MAIN_CONTROL 1
#define ID_PC_TYPE_SENSOR       2
#define ID_PC_TYPE_OTE          3
#define ID_PC_TYPE_PLC          4

//OTE TYPE CODE
#define OTE_TYPE_MONITOR        0   //���j�^�[��p�i��~����̂ݗL���j
#define OTE_TYPE_MOBILE         1   //���o�C���[��
#define OTE_TYPE_REMOTE_ROOM    2   //���u���쎺
#define OTE_TYPE_REMOTE_SITE    3   //���u�T�C�g

#pragma endregion ���

#pragma region RESPONCE
#define L_ON            0x01    // ON
#define L_OFF           0x00    // OFF
#define STAT_OK         L_ON   // ON
#define STAT_NG         L_OFF  // OFF

#define CODE_ACCEPTED      1        //��t����
#define CODE_REJECTED      -1       //��t�s��
#define CODE_NO_REQUEST    0        //�v������
#define CODE_EXIST         1       //�L��
#define CODE_NA            0       //���ɂȂ�ee
#define CODE_OK            1       //�L��
#define CODE_NG            -1      //NG
#pragma endregion ����

#pragma region MESSAGE
// �^�X�N�X���b�h����̗v���C�x���g
//wp:�v���C�x���g�̃n���h���v����ʁ@lp:�p�����[�^
#define WM_USER_TASK_REQ         0x0405         // wp LOW:�^�X�NID�@HIGH:�R�}���h�R�[�h�@lp �p�����[�^D  
#define WM_USER_SET_BK_COLOR_REQ 0x0406         // wp LOW:�^�X�NID�@HIGH:�R�}���h�R�[�h�@lp �p�����[�^D 

#define WM_USER_WPH_OPEN_IF_WND  0x0001         // wp HWORD�@code IF�E�B���h�EOPEN 
#define WM_USER_WPH_CLOSE_IF_WND 0x0002         // wp HWORD  code IF�E�B���h�ECLOSE

#pragma endregion �E�B���h�E���b�Z�[�W

///# �}�N��
#pragma region MACRO_DEF

#ifndef MIN
#  define MIN(a,b)  ((a) > (b) ? (b) : (a))
#endif

#ifndef MAX
#  define MAX(a,b)  ((a) < (b) ? (b) : (a))
#endif

#ifndef iABS
#  define iABS(a)  (a < 0 ? -a : a)
#endif

#ifndef fABS
#  define fABS(a)  (a < 0.0 ? -a : a)
#endif

#ifndef dABS
#  define dABS(a)  (a < 0.0 ? -a : a)
#endif

#ifndef Bitcheck
#define Bitcheck(a,b)	(a >> b) & 1
#endif

#ifndef Bitset
#define Bitset(a,b)		a |= (1<<b)
#endif

#ifndef Bitclear
#define Bitclear(a,b)	a &= ~(1<<b)
#endif

#pragma endregion �}�N��

///# �z��Q�Ɨp�@����C���f�b�N�X
#pragma region ARR_MOTION

#define MOTION_ID_MAX   8  //���䎲�ő吔

#define ID_HOIST                0   //�� �@       ID
#define ID_TROLLY               1   //���s        ID
#define ID_BOOM_H               1   //����        ID
#define ID_SLEW                 2   //����        ID
#define ID_GANTRY               3   //���s        ID
#define ID_AHOIST               4   //�⊪�@      ID
#define ID_OP_ROOM              5   //�^�]���ړ��@ID
#define ID_BH_HST               5   //�N���h�����努  ID
#define ID_H_ASSY               6   //�݋      ID

enum ID_AXIS {
    mh = 0,
    bh,
    sl,
    gt,
    ah,
    Max
};


#pragma endregion ����C���f�b�N�X

//�^�X�N�ُ�t���O
#pragma region ERR_DEF

#define SMEM_NG_OTE_INF			0x0000000000010000
#define SMEM_NG_CRANE_STAT	    0x0000000000020000
#define SMEM_NG_CS_INF			0x0000000000040000
#define SMEM_NG_PLC_IO			0x0000000000080000
#define SMEM_NG_AGENT_INF		0x0000000000100000
#define SMEM_NG_SIM_INF		    0x0000000000200000
#define SMEM_NG_POL_INF			0x0000000000400000
#define SMEM_NG_OTE_CS_INF  	0x0000000100000000
#define SMEM_NG_AUX_ENV_INF  	0x0000010000000000
#define SMEM_NG_AUX_CS_INF  	0x0000020000000000
#define SMEM_NG_AUX_AGENT_INF  	0x0000040000000000

#define SOCK_NG_UNICAST			0x0000000000000100
#define SOCK_NG_MULTICAST		0x0000000000000200

#pragma endregion �^�X�N�G���[�r�b�g��`

//�f�o�C�X�R�[�h
typedef struct _ST_DEVICE_CODE {
	WCHAR crane_id[8]=L"XXXXXX";   //���ԃR�[�h
    WCHAR pc_type[8]=L"??????";    //PC TYPE
    INT32 serial_no;
    INT32 option;
 }ST_DEVICE_CODE, * LPST_DEVICE_CODE;
//��{�\����
typedef struct _ST_MOVE_SET {
    double p;	//�ʒu
    double v;	//���x
    double a;	//�����x
}ST_MOVE_SET, * LPST_MOVE_SET;

typedef struct _ST_XYZ {
    double x;
    double y;
    double z;
}ST_XYZ, * LPST_XYZ;


///#�r�b�g��` 
#pragma region BIT_DEF
#define BITS_WORD   0xFFFF //WORD�f�[�^
#define BIT0        0x0001
#define BIT1        0x0002
#define BIT2        0x0004
#define BIT3        0x0008
#define BIT4        0x0010
#define BIT5        0x0020
#define BIT6        0x0040
#define BIT7        0x0080
#define BIT8        0x0100
#define BIT9        0x0200
#define BIT10       0x0400
#define BIT11       0x0800
#define BIT12       0x1000
#define BIT13       0x2000
#define BIT14       0x4000
#define BIT15       0x8000
#define BITFF       0xFFFF
#pragma endregion �r�b�g��`

#pragma region AXIS_MASK
#define SET_MASK_MH 0x0000000F //WORD�f�[�^
#define SET_MASK_BH 0x000000F0 //WORD�f�[�^
#define SET_MASK_SL 0x00000F00 //WORD�f�[�^
#define SET_MASK_GT 0x0000F000 //WORD�f�[�^
#define SET_MASK_AH 0x000F0000 //WORD�f�[�^
#pragma endregion ����ԃZ�b�g�}�X�N

///# �J���[�p���b�g
#pragma region COLOR

#define COLOR_ID_BLACK      0
#define COLOR_ID_DGRAY      1
#define COLOR_ID_LGRAY      2
#define COLOR_ID_WHITE      3
#define COLOR_ID_SRED       4   //STRONG
#define COLOR_ID_DRED       5   //DARK
#define COLOR_ID_LRED       6   //LIGHT
#define COLOR_ID_WRED       7   //WEAK
#define COLOR_ID_SGREEN     8   //STRONG
#define COLOR_ID_DGREEN     9   //DARK
#define COLOR_ID_LGREEN     10  //LIGHT
#define COLOR_ID_WGREEN     11  //WEAK
#define COLOR_ID_SBLUE      12  //STRONG
#define COLOR_ID_DBLUE      13  //DARK
#define COLOR_ID_LBLUE      14  //LIGHT
#define COLOR_ID_WBLUE      15  //WEAK
#define COLOR_ID_SYELLOW    16  //STRONG
#define COLOR_ID_DYELLOW    17  //DARK
#define COLOR_ID_LYELLOW    18  //LIGHT
#define COLOR_ID_WYELLOW    19  //WEAK
#define COLOR_ID_SORANGE    20  //STRONG
#define COLOR_ID_DORANGE    21  //DARK
#define COLOR_ID_LORANGE    22  //LIGHT
#define COLOR_ID_WORANGE    23  //WEAK
#define COLOR_ID_SMAZENDA   24  //STRONG
#define COLOR_ID_DMAZENDA   25  //DARK
#define COLOR_ID_LMAZENDA   26  //LIGHT
#define COLOR_ID_WMAZENDA   27  //WEAK
#define COLOR_ID_SBROWN     28  //STRONG
#define COLOR_ID_DBROWN     29  //DARK
#define COLOR_ID_LBROWN     30  //LIGHT
#define COLOR_ID_WBROWN     31  //WEAK

#define RGBA_ID_RED         0
#define RGBA_ID_GREEN       1
#define RGBA_ID_BLUE        2
#define RGBA_ID_ALPHA       3

using namespace Gdiplus;

typedef struct _PANEL_COLOR_PALLET {
    Gdiplus::Color color[32] = {

        //GRAY                                                                                              RED     
        Color(255,0,0,0),     Color(255,64,64,64),  Color(255,240, 240, 240),   Color(255,255, 255, 255),   Color(255,128, 0, 0),   Color(255,64, 0, 0),    Color(255,32, 0, 0),    Color(255,255, 201, 252),
        //GREEN                                                                                             BLUE     
        Color(100,0,255,0),   Color(255,0,255,0),   Color(255,211, 255, 222),   Color(255,211, 255, 222),   Color(255,0,0,255),     Color(255,0, 0, 255),   Color(255,0,0,255),     Color(255,0, 0, 255),
        //YELLOW                                                                                            ORANGE       
        Color(255,255,255,0), Color(255,255,255,0), Color(255,255, 248, 51),    Color(255,255, 248, 51),    Color(255,255,106,0),   Color(255,255,106,0),   Color(255,255,221,198), Color(255,255,221,198),
        //-                                                         -       
        Color(255,0,0,0),     Color(255,0, 0, 0),   Color(255,0, 0, 0),         Color(255,0, 0, 0),         Color(255,0, 0, 0),     Color(255,0, 0, 0),     Color(255,0, 0, 0),     Color(255,0, 0, 0)
    };
}PANEL_COLOR_PALLET, * LPPANEL_COLOR_PALLET;

#pragma endregion

///#�M���\�������v
#pragma region SIGNAL LAMPCOLOR

#define CODE_PNL_COM_DEACTIVE	-1
#define CODE_PNL_COM_OFF		0
#define CODE_PNL_COM_STANDBY    0
#define CODE_PNL_COM_ON		    1
#define CODE_PNL_COM_ACTIVE     1
#define CODE_PNL_COM_SELECTED	2
#define CODE_PNL_COM_FLICKER	255

typedef struct StLampCom {
    UINT8 com;
    UINT8 color;
}ST_LAMP_COM, * LPST_LAMP_COM;

union UN_LAMP_COM {
    INT16 code;
    ST_LAMP_COM st;
};

#pragma endregion
