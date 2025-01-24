#pragma once
#include <windows.h>

//#define _DVELOPMENT_MODE                //�J������L���ɂ���

///# �����萔�A�W��
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

///# �R�[�h
#define L_ON            0x01  // ON
#define L_OFF           0x00  // OFF
#define STAT_OK         0x01  // ON
#define STAT_NG         0x00  // OFF

#define UNIT_CODE_M     0//�P�ʂ̃R�[�h m
#define UNIT_CODE_RAD   1//�P�ʂ̃R�[�h rad
#define UNIT_CODE_MM    2//�P�ʂ̃R�[�h mm
#define UNIT_CODE_DEG   3//�P�ʂ̃R�[�h deg


/*** ���� ***/
#define CODE_ACCEPTED      1        //��t����
#define CODE_REJECTED      -1       //��t�s��
#define CODE_NO_REQUEST    0        //�v������
#define CODE_EXIST         1       //�L��
#define CODE_NA            0       //���ɂȂ�ee
#define CODE_OK            1       //�L��
#define CODE_NG            -1      //NG


///# �}�N��
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

///# �z��Q�Ɨp�@����C���f�b�N�X
#define MOTION_ID_MAX   8  //���䎲�ő吔

#define ID_HOIST        0   //�� �@       ID
#define ID_GANTRY       1   //���s        ID
#define ID_TROLLY       2   //���s        ID
#define ID_BHMH         2   //�����努    ID
#define ID_BOOM_H       3   //����        ID
#define ID_SLEW         4   //����        ID
#define ID_AHOIST       5   //�⊪�@      ID
#define ID_OP_ROOM      6   //�^�]���ړ��@ID
#define ID_H_ASSY       7   //�݋      ID

#define BIT_SEL_HST         0x00000001
#define BIT_SEL_GNT         0x00000002
#define BIT_SEL_TRY         0x00000004
#define BIT_SEL_BHMH        0x00000004
#define BIT_SEL_BH          0x00000008
#define BIT_SEL_SLW         0x00000010
#define BIT_SEL_AH          0x00000020  //JC
#define BIT_SEL_OPR         0x00000040
#define BIT_SEL_ASSY        0x00000080
#define BIT_SEL_ALL_0NOTCH  0x10000000
#define BIT_SEL_MOTION      BIT_SEL_HST|BIT_SEL_GNT|BIT_SEL_BH|BIT_SEL_SLW|BIT_SEL_AH
#define BIT_SEL_STATUS      0xFFFF0000

#define ID_UP           0   //�㑤
#define ID_DOWN         1   //����
#define ID_FWD          0   //�O�i
#define ID_REV          1   //��i
#define ID_RIGHT        0   //�E��
#define ID_LEFT         1   //����
#define ID_ACC          0   //����
#define ID_DEC          1   //����

#define ID_STOP         -1  //��~
#define ID_SELECT       2   //�I��

#define ID_X           0   // X����
#define ID_Y           1   // Y����
#define ID_Z           2   // Z����

#define ID_T           0   // �ڐ�����
#define ID_R		   1   // ���a����

///# �V�~�����[�V�������[�h
#define MODE_PRODUCTION         0x0000//���@
#define MODE_CRANE_SIM          0x0001//�蓮�����̃Z���TFB���V�~�����[�^�̏o�͒l�ɂ���
#define MODE_SENSOR_SIM		    0x0002//�U��Z���T��������,���u�p�Z���T�M�̉������N���[�������V�~�����[�^�̏o�͂��琶������

///# �r�b�g��`
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



typedef struct DeviceCode {
    char    order[2];   //����
    char    system[2];  //�@�B�A�V�X�e���R�[�h    :�N���[���ԍ���
    char    type[2];    //�f�o�C�X��ʁ@          :����PC,�[����
    INT16   no;         //�V���A���ԍ�
}ST_DEVICE_CODE, * LPST_DEVICE_CODE;

