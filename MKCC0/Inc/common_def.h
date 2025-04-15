#pragma once
#include <windows.h>

//@ GDI+
#include <objbase.h>//gdiplus.h�̃R���p�C����ʂ����߂ɕK�v
#include <gdiplus.h>

//#define _DVELOPMENT_MODE                //�J������L���ɂ���

///# �����萔�A�ϊ��W��
#pragma region PHISIC
#define GA				9.80665     //�d�͉����x

#define PI360           6.2832      //2��
#define PI330           5.7596   
#define PI315           5.4978
#define PI270           4.7124
#define PI180           3.1416      //��
#define PI165           2.8798
#define PI150           2.6180
#define PI135           2.3562
#define PI120           2.0944
#define PI90            1.5708     //��/2
#define PI60            1.0472
#define PI45            0.7854
#define PI30            0.5236
#define PI15            0.2618
#define PI10            0.17453
#define PI5             0.0873
#define PI0             0.00000

#define RAD1DEG         0.017453    //rad/deg
#define DEG1RAD         57.29578    //deg/rad

#define RAD1DEG_SQ      3282.6      //(rad/deg)^2
#define DEG1RAD_SQ      0.00030462  //(deg/rad)^2

#define PERCENT_NORM	1.0         //���K���� 100% = 1.0

#pragma endregion �����萔�Q�W��

///# �R�[�h
#pragma region ID
//�N���[����� ID  �z��INDEX
#define CRANE_TYPE_MAX          16  //�J��
#define CRANE_TYPE_JC0          0   //�W�u�N���[�� �努�̂�
#define CRANE_TYPE_JC1          1   //�W�u�N���[�� �⊪����
#define CRANE_TYPE_GC           2   //�S���C�A�X�N���[��

//�N���[������ ID INDEX
#define CRANE_ID_MAX            32  //�J��
#define CRANE_ID_NULL           0   //����
#define CRANE_ID_H6R602         1   //�݂炢
#define CARNE_ID_HHGH29         2   //���������x��70t�@102��  
#define CARNE_ID_HHGQ18         3   //���������x��300t 10��
#define CARNE_ID_HHFR22         4   //�����ۋT1200t 1��
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

#define NOTCH_0	                0
#define NOTCH_1	                1
#define NOTCH_2	                2
#define NOTCH_3	                3
#define NOTCH_4	                4
#define NOTCH_5	                5

#define FUNC_DEACTIVE           0
#define FUNC_ACTIVE             1
#define FUNC_PAUSE              2
#define FUNC_RESTART            3

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

#define NUM_FORMAT_TYPE_DEC     0x0000
#define NUM_FORMAT_TYPE_HEX     0x0001
#define NUM_FORMAT_TYPE_16      0x0010
#define NUM_FORMAT_TYPE_32     0x00020

#define NUM_FORMAT_TYPE_HEX16   0x0011
#define NUM_FORMAT_TYPE_HEX32   0x0021

#define NUM_FORMAT_TYPE_DEC16   0x0010
#define NUM_FORMAT_TYPE_DEC32   0x0020


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
#define WM_USER_WPH_OPEN_IF_WND  0x0001         // wp�@code IF�E�B���h�EOPEN   lp �E�B���h�E��ID
#define WM_USER_WPH_CLOSE_IF_WND 0x0002         // wp�@code IF�E�B���h�ECLOSE   lp �E�B���h�E��ID

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

#pragma endregion �}�N��

///# �z��Q�Ɨp�@����C���f�b�N�X
#pragma region ARR_MOTION

#define MOTION_ID_MAX   8  //���䎲�ő吔

#define ID_HOIST                0   //�� �@       ID
#define ID_GANTRY               1   //���s        ID
#define ID_TROLLY               2   //���s        ID
#define ID_BHMH                 2   //�����努    ID
#define ID_BOOM_H               3   //����        ID
#define ID_SLEW                 4   //����        ID
#define ID_AHOIST               5   //�⊪�@      ID
#define ID_OP_ROOM              6   //�^�]���ړ��@ID
#define ID_H_ASSY               7   //�݋      ID

#define BIT_SEL_HST             0x00000001
#define BIT_SEL_GNT             0x00000002
#define BIT_SEL_TRY             0x00000004
#define BIT_SEL_BHMH            0x00000004
#define BIT_SEL_BH              0x00000008
#define BIT_SEL_SLW             0x00000010
#define BIT_SEL_AH              0x00000020  //JC
#define BIT_SEL_OPR             0x00000040
#define BIT_SEL_ASSY            0x00000080
#define BIT_SEL_ALL_0NOTCH      0x10000000
#define BIT_SEL_MOTION          BIT_SEL_HST|BIT_SEL_GNT|BIT_SEL_BH|BIT_SEL_SLW|BIT_SEL_AH
#define BIT_SEL_STATUS          0xFFFF0000

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
	WCHAR crane_id[7]=L"XXXXXX";   //���ԃR�[�h
    WCHAR pc_type[7]=L"??????";    //PC TYPE
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
#pragma endregion �r�b�g��`

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

typedef struct _MKCC_COLOR_PALLET {
    BYTE rgba[32][4] = {
        //GRAY                                                      RED     
        {0,0,0,0},{192,192,192,0},{240,240,240,0},{255,255,255,0},  {255,0,0,0},{128,0,0,0},{64,0,0,0},{255, 201, 252, 0},
        //GREEN                                                     BLUE     
        {0,255,0,0},{0,255,0,0},{211, 255, 222},{211, 255, 222},    {0,0,255,0},{0,0,128,0},{153, 255, 249, 0},{153, 255, 249, 0},
        //YELLOW                                                    ORANGE       
        {255,255,0,0},{255,255,0,0},{255, 248, 51},{255, 248, 51},  {255,106,0,0},{255,106,0,0},{255,221,198,0},{255,221,198,0},
        //MAZENDA                                                    BROWN       
        {255,0,200,0},{255,85,200,0},{255,150,20,0},{255,220,20,0}, {155,30,0,0},{155,50,30,0},{155,120,120,0},{155,130,125,0}
    };
}MKCC_COLOR_PALLET, * LPMKCC_COLOR_PALLET;

using namespace Gdiplus;

typedef struct _PANEL_COLOR_PALLET {

    Gdiplus::Color color[32] = {

        //GRAY                                                                                      RED     
        Color(0,0,0,0),     Color(192,192,192,0),   Color(240, 240, 240, 0),Color(255, 255, 255, 0),Color(128, 0, 0, 0),Color(64, 0, 0, 0), Color(32, 0, 0, 0),     Color(255, 201, 252, 0),
        //GREEN                                                                                     BLUE     
        Color(0,255,0,0),   Color(0,255,0,0),       Color(211, 255, 222,0), Color(211, 255, 222,0), Color(0,0,255,0),   Color(0, 0, 255, 0),Color(0,0,255,0),       Color(0, 0, 255, 0),
        //YELLOW                                                                                    ORANGE       
        Color(255,255,0,0), Color(255,255,0,0),     Color(255, 248, 51,0),  Color(255, 248, 51,0),  Color(255,106,0,0), Color(255,106,0,0), Color(255,221,198,0),   Color(255,221,198,0),
        //-                                                         -       
        Color(0,0,0,0),     Color(0, 0, 0, 0),      Color(0, 0, 0, 0),      Color(0, 0, 0, 0),      Color(0, 0, 0, 0),  Color(0, 0, 0, 0),  Color(0, 0, 0, 0),      Color(0, 0, 0, 0) 
    };
}PANEL_COLOR_PALLET, * LPPANEL_COLOR_PALLET;

#pragma endregion
