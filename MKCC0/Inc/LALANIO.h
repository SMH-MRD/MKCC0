//***************************************************************************
//* LANIO���C�u���� �w�b�_�t�@�C��
//***************************************************************************

#ifdef EXPORT_DECLSPEC
#define DECLSPEC extern "C" __declspec(dllexport) int __stdcall
#else
#define DECLSPEC extern "C" __declspec(dllimport) int __stdcall
#endif

//***************************************************************************
//* �^��`�A�萔��`
//***************************************************************************

//***************************************************************************
//* LANIO�n���h��
//***************************************************************************
#define hLANIO int

//***************************************************************************
//* LALanioGetLastError���Ԃ��G���[�R�[�h
//***************************************************************************
enum LANIOERROR {
    LALANIOERR_NOINIT = 1,          //* ����������Ă��܂���BLALanioInit�����s���Ă��������B
    LALANIOERR_NOLANIO,             //* �����������ʁALANIO�������ł��܂���ł����B
    LALANIOERR_NOTEXIST,            //* �����Ŕ�������LANIO��葽���l���w�肵�܂����B
    LALANIOERR_NOTFOUND,            //* �w�肳�ꂽ������LANIO�𔭌��ł��܂���ł����B
    LALANIOERR_NOCONNECTED,         //* LANIO�Ɛڑ����Ă��܂���BLALanioConnect�����s���Ă��������B
    LALANIOERR_INVALIDOPERATION,    //* �w�肳�ꂽ����͎��s�ł��܂���B
    LALANIOERR_ALREADY_CONNECTED,   //* ���łɐڑ����Ă��܂��B
    LALANIOERR_OVERCONNECTMAX,      //* �ڑ��ő吔�𒴉߂��܂����B
    LALANIOERR_INVALIDRETURNVALUE,  //* �s���Ȗ߂�l���Ԃ�܂����B
};

//***************************************************************************
//* LALanioGetModel�Ŏ擾����LANIO���f��
//***************************************************************************
enum LANIOMODEL {
    NOTLANIO = 0,
    LA2R3A   = 8,
    LA2A3P   = 9,
    LA2R3AV2 = 10,
    LA3A2P   = 11,
    LA5AI    = 12,
    NO_GETMODEL = -1,
};

//***************************************************************************
//* �ݒ�萔
//***************************************************************************
enum LANIO_AI_RANGE {
    LALANIO_AI_RANGE_100mV          = 0,
    LALANIO_AI_RANGE_1V             = 1,
    LALANIO_AI_RANGE_10V            = 2,
    LALANIO_AI_RANGE_30V            = 3,
    LALANIO_AI_RANGE_20mA_250orm    = 4,
    LALANIO_AI_RANGE_20mA_50orm     = 5,
    LALANIO_AI_RANGE_TC             = 6,
};

enum LALANIO_AI_TRANSMITCYCLE {
    LALANIO_AI_TRANSMITCYCLE_0_05sec = 0,
    LALANIO_AI_TRANSMITCYCLE_0_1sec  = 1,
    LALANIO_AI_TRANSMITCYCLE_0_2sec  = 2,
    LALANIO_AI_TRANSMITCYCLE_0_5sec  = 3,
    LALANIO_AI_TRANSMITCYCLE_1sec    = 4,
    LALANIO_AI_TRANSMITCYCLE_2sec    = 5,
    LALANIO_AI_TRANSMITCYCLE_5sec    = 6,
    LALANIO_AI_TRANSMITCYCLE_10sec   = 7,
    LALANIO_AI_TRANSMITCYCLE_20sec   = 8,
    LALANIO_AI_TRANSMITCYCLE_30sec   = 9,
    LALANIO_AI_TRANSMITCYCLE_1min    = 10,
    LALANIO_AI_TRANSMITCYCLE_2min    = 11,
    LALANIO_AI_TRANSMITCYCLE_5min    = 12,
    LALANIO_AI_TRANSMITCYCLE_10min   = 13,
};

enum LALANIO_AO_RANGE {
    LALANIO_AO_RANGE_10V             = 0,
    LALANIO_AO_RANGE_20mA            = 1,
};

//***************************************************************************
//* �������A�I���A�G���[�擾�֐�
//***************************************************************************

//***************************************************************************
//*     LANIO���C�u���������������܂��B
//***************************************************************************
DECLSPEC LALanioInit();

//***************************************************************************
//*     LANIO���C�u�����̎g�p���I�����܂��B
//***************************************************************************
DECLSPEC LALanioEnd();

//***************************************************************************
//*     ���O�ɔ��������G���[�ԍ����擾���܂��B
//*     �߂�l : �G���[�̏���Ԃ��܂�
//***************************************************************************
DECLSPEC LALanioGetLastError();

//***************************************************************************
//* LANIO�����֐�
//***************************************************************************

//***************************************************************************
//*     �����I��LANIO��ID��Model���擾����@�\�ɂ��Ă̐ݒ�����܂��B
//*     ����   : enable  ���̋@�\���g�p����ꍇ��TRUE(1)�A���Ȃ��ꍇ��FALSE(0)
//*              port    ID��Model�������擾����V���A���̃|�[�g�ԍ�
//***************************************************************************
DECLSPEC LALanioSetAutoRequestIdModel(BOOL enable, int port);

//***************************************************************************
//*     LAN���ɂ���LANIO���������܂��B
//*     ����   : msec    �^�C���A�E�g���ԁi�P�� m�b�j
//*     �߂�l : ��������LANIO�̐��B�����ł��Ȃ������ꍇ��0�B�G���[����-1
//***************************************************************************
DECLSPEC LALanioSearch(int msec = 100);

//***************************************************************************
//* LANIO���擾�֐�
//***************************************************************************

//***************************************************************************
//*     ��������LANIO��IP�A�h���X���擾���܂��B
//*     ����   : lanio       LANIO�ԍ�
//*              ipaddress   IP�A�h���X���擾����o�b�t�@�ւ̃|�C���^(16byte�K�v)
//***************************************************************************
DECLSPEC LALanioGetIpAddress(int lanio, char *ipaddress);

//***************************************************************************
//*     ��������LANIO��MAC�A�h���X���擾���܂��B
//*     ����   : lanio       LANIO�ԍ�
//*              macaddress  MAC�A�h���X���擾����o�b�t�@�ւ̃|�C���^(6byte�K�v)
//***************************************************************************
DECLSPEC LALanioGetMacAddress(int lanio, unsigned char *macaddress);

//***************************************************************************
//*     LANIO��ID�ԍ����擾���܂��B
//*     ����   : lanio   LANIO�ԍ�
//*              id      LANIO��ID���擾����o�b�t�@�ւ̃|�C���^
//***************************************************************************
DECLSPEC LALanioGetId(int lanio, int *id);

//***************************************************************************
//*     LANIO�̃��f�����擾���܂��B
//*     ����   : lanio   LANIO�ԍ�
//*              model   LANIO�̃��f�����擾����o�b�t�@�ւ̃|�C���^
//***************************************************************************
DECLSPEC LALanioGetModel(int lanio, int *model);

//***************************************************************************
//*     ��������LANIO�ɑ΂���ID�ƃ��f���ԍ������₵�܂��B
//*     ����   : lanio   LANIO�ԍ�
//*              id      LANIO��ID���擾����o�b�t�@�ւ̃|�C���^
//*              model   LANIO�̃��f�����擾����o�b�t�@�ւ̃|�C���^
//***************************************************************************
DECLSPEC LALanioRequestIdModel(int lanio, int *id, int *model);

//***************************************************************************
//* LANIO�ڑ��֐�
//***************************************************************************

//***************************************************************************
//*     LALanioSearch�ɂ���Ĕ������ꂽ���Ԃ��w�肵�Đڑ����܂��B
//*     ����   : lanio   LANIO�ԍ�
//*     �߂�l : �G���[����-1�A����I������LANIO����n���h����Ԃ��܂�
//***************************************************************************
DECLSPEC LALanioConnect(int lanio);

//***************************************************************************
//*     ID�ƃ��f�����w�肵�Đڑ����܂��B
//*     ����   : id      LANIO��ID�ԍ�
//*              model   LANIO�̃��f��
//*     �߂�l : �G���[����-1�A����I������LANIO����n���h����Ԃ��܂�
//***************************************************************************
DECLSPEC LALanioConnectByIdModel(int id, int model);

//***************************************************************************
//*     IP�A�h���X���w�肵�Đڑ����܂��B
//*     ����   : ipaddress   IP�A�h���X������o�b�t�@�ւ̃|�C���^
//*     �߂�l : �G���[����-1�A����I������LANIO����n���h����Ԃ��܂�
//***************************************************************************
DECLSPEC LALanioConnectByIpAddress(char *ipaddress);

//***************************************************************************
//*     MAC�A�h���X���w�肵�Đڑ����܂��B
//*     ����   : macaddress  MAC�A�h���X�o�b�t�@�ւ̃|�C���^
//*     �߂�l : �G���[����-1�A����I������LANIO����n���h����Ԃ��܂�
//***************************************************************************
DECLSPEC LALanioConnectByMacAddress(unsigned char *macaddress);

//***************************************************************************
//*     IP�A�h���X��������URL���w�肵�Đڑ����܂��B
//*     ����   : address IP�A�h���X�܂���url������o�b�t�@�ւ̃|�C���^
//*              msec    �^�C���A�E�g���ԁi�P�� m�b�j
//*     �߂�l : �G���[����-1�A����I������LANIO����n���h����Ԃ��܂�
//***************************************************************************
DECLSPEC LALanioConnectDirect(char *address, int msec = 1000);

//***************************************************************************
//*     IP�A�h���X��������URL�A�|�[�g�ԍ����w�肵�Đڑ����܂��B�ڑ�����UDP���g�p���܂���B
//*     ����   : address    IP�A�h���X�܂���url������o�b�t�@�ւ̃|�C���^
//*              msec       �^�C���A�E�g���ԁi�P�� m�b�j
//*              serialport �|�[�g�ԍ�
//*     �߂�l : �G���[����-1�A����I������LANIO����n���h����Ԃ��܂�
//***************************************************************************
DECLSPEC LALanioConnectDirect2(char *address, int msec, int serialport);

//***************************************************************************
//* LANIO�ؒf�֐�
//***************************************************************************

//***************************************************************************
//*     LANIO�Ƃ̐ڑ���ؒf���܂��B
//*     ����   : handle  LANIO����n���h��
//***************************************************************************
DECLSPEC LALanioClose(hLANIO handle);

//***************************************************************************
//* ���o�͊֐�
//***************************************************************************

//***************************************************************************
//*     LANIO��DO�𑀍삵�܂��B�S�Ă�DO�ɑ΂���Ăɑ��삵�܂��B
//*     ����   : handle  LANIO����n���h��
//*              signal  DO�|�[�g�̐ݒ�B�ŉ��ʃr�b�g����DO1�ADO2...�ɑΉ����܂�
//***************************************************************************
DECLSPEC LALanioDO(hLANIO handle, int signal);

//***************************************************************************
//*     LANIO�̌��݂�DO��Ԃ��擾���܂��B�S�Ă�DO�ɑ΂���ĂɎ擾���܂��B
//*     ����   : handle  LANIO����n���h��
//*              signal  DO�|�[�g�̓��e������o�b�t�@�ւ̃|�C���^
//*                      �ŉ��ʃr�b�g����DO1�ADO2...
//***************************************************************************
DECLSPEC LALanioGetDO(hLANIO handle, int *signal);

//***************************************************************************
//*     LANIO��DI���擾���܂��B�S�Ă�DI�ɑ΂���ĂɎ擾���܂��B
//*     ����   : handle  LANIO����n���h��
//*              signal  DI�|�[�g�̓��e������o�b�t�@�ւ̃|�C���^
//*                      �ŉ��ʃr�b�g����DI1�ADI2...
//***************************************************************************
DECLSPEC LALanioDI(hLANIO handle, int *signal);

//***************************************************************************
//*     LANIO��AO����d�����o�͂��܂�
//*     ����   : handle  LANIO����n���h��
//*              ch      �`�����l�����w�肵�܂��B 0:AO1 1:AO2 ...
//*              val     �o�͒l���w�肵�܂��B
//***************************************************************************
DECLSPEC LALanioAOVoltage(hLANIO handle, int ch, int val);

//***************************************************************************
//*     LANIO��AO����d�����o�͂��܂�
//*     ����   : handle  LANIO����n���h��
//*              ch      �`�����l�����w�肵�܂��B 0:AO1 1:AO2 ...
//*              val     �o�͒l���w�肵�܂��B
//***************************************************************************
DECLSPEC LALanioAOAmpere(hLANIO handle, int ch, int val);

//***************************************************************************
//*     LANIO��AO����d���E�d�����o�͂��܂�
//*     ����   : handle   LANIO����n���h��
//*              ch_count �o�͂���`�����l�������w�肵�܂�
//*              ch       �o�͂���`�����l�����������z����w�肵�܂� 0:AO1 1:AO2 ...
//*              range    �o�͂���d��/�d�����������z����w�肵�܂� 0:�d�� 1:�d��
//*              val      �o�͒l���w�肵�܂��B
//***************************************************************************
DECLSPEC LALanioAOMulti(hLANIO xport, int ch_count, int *ch, int *range, int *val);

//***************************************************************************
//*     LANIO��AO�̏����l��ݒ肵�܂�
//*     ����   : handle  LANIO����n���h��
//*              ch      �`�����l�����w�肵�܂��B 0:AO1 1:AO2 ...
//*              range   �d��/�d�����w�肵�܂� 0:�d�� 1:�d��
//*              val     �o�͒l���w�肵�܂��B
//***************************************************************************
DECLSPEC LALanioSetInitialAO(hLANIO handle, int ch, int range, int val);

//***************************************************************************
//*     LANIO��AO�̏o�͒l���擾���܂�
//*     ����   : handle  LANIO����n���h��
//*              ch      �`�����l�����w�肵�܂��B 0:AO1 1:AO2 ...
//*              range   �d��/�d��������o�b�t�@�ւ̃|�C���^ 0:�d�� 1:�d��
//*              val     �o�͒l������o�b�t�@�ւ̃|�C���^
//***************************************************************************
DECLSPEC LALanioGetAO(hLANIO handle, int ch, int *range, int *val);

//***************************************************************************
//*     LANIO��AI������͂��܂�
//*     ����   : handle  LANIO����n���h��
//*              ch      �`�����l�����w�肵�܂��B 0:AI1 1:AI2 ...
//*              val     ���͒l������o�b�t�@�ւ̃|�C���^
//***************************************************************************
DECLSPEC LALanioAI(hLANIO handle, int ch, int *val);

//***************************************************************************
//*     LANIO��AI�S�`�����l��������͂��܂�
//*     ����   : handle  LANIO����n���h��
//*              val     ���͒l������o�b�t�@�ւ̃|�C���^
//***************************************************************************
DECLSPEC LALanioAIAll(hLANIO handle, int *val);

//***************************************************************************
//*     LANIO��AI�̃����W��ݒ肵�܂�
//*     ����   : handle  LANIO����n���h��
//*              ch      �`�����l�����w�肵�܂��B 0:AI1 1:AI2 ...
//*              range   �����W�ݒ�
//*                      0:100mV  1:1V  2:10V  3:30V  4:20mA(250��)  5:20mA(50��)  6:�M�d��
//***************************************************************************
DECLSPEC LALanioSetAIRange(hLANIO handle, int ch, int range);

//***************************************************************************
//*     LANIO��AI�̐ݒ���擾���܂�
//*     ����   : handle        LANIO����n���h��
//*              ch            �`�����l�����w�肵�܂��B 0:AI1 1:AI2 ...
//*              range         �����W�ݒ肪����o�b�t�@�ւ̃|�C���^
//*                            0:100mV  1:1V  2:10V  3:30V  4:20mA(250��)  5:20mA(50��)  6:�M�d��
//*              transmitcycle �����]���T�C�N��������o�b�t�@�ւ̃|�C���^ 
//*                            0:0.05sec  1:0.1sec  2:0.2sec  3:0.5sec
//*                            4:1sec  5:2sec  6:5sec  7:10sec  8:20sec  9:30sec
//*                            10:1min  11:2min  12:5min  13:10min
//***************************************************************************
DECLSPEC LALanioGetAISetting(hLANIO handle, int ch, int *range, int *transmitcycle);

//***************************************************************************
//*     LANIO��AD�R���o�[�^�ϊ����x��ݒ肵�܂�
//*     ����   : handle  LANIO����n���h��
//*              speed   �ϊ����x
//*                      0 10sps      1 16.6sps    2 50sps      3 60sps
//*                      4 400sps     5 1200sps    6 3600sps    7 14400sps
//***************************************************************************
DECLSPEC LALanioSetADCsps(hLANIO handle, int speed);

//***************************************************************************
//*     LANIO��AD�R���o�[�^�ϊ����x���擾���܂�
//*     ����   : handle  LANIO����n���h��
//*              speed   �ϊ����x������o�b�t�@�ւ̃|�C���^
//*                      0 10sps      1 16.6sps    2 50sps      3 60sps
//*                      4 400sps     5 1200sps    6 3600sps    7 14400sps
//***************************************************************************
DECLSPEC LALanioGetADCsps(hLANIO handle, int *speed);

//***************************************************************************
//*     LANIO��AI�̎����]�����x��ݒ肵�܂�
//*     ����   : handle        LANIO����n���h��
//*              transmitcycle �����]���T�C�N��������o�b�t�@�ւ̃|�C���^ 
//*                            0:0.05sec  1:0.1sec  2:0.2sec  3:0.5sec
//*                            4:1sec  5:2sec  6:5sec  7:10sec  8:20sec  9:30sec
//*                            10:1min  11:2min  12:5min  13:10min
//***************************************************************************
DECLSPEC LALanioSetCyclicTransmit(hLANIO handle, int transmitcycle);

//***************************************************************************
//*     LANIO��AI�̎����]�����J�n���܂�
//*     ����   : handle   LANIO����n���h��
//*              CallBack �f�[�^�������]�������R�[���o�b�N�֐�
//*              User     �R�[���o�b�N�֐����Ă΂����Ɏw��ł��郆�[�U�f�[�^
//***************************************************************************
DECLSPEC LALanioStartCyclicTransmit(hLANIO handle, void *CallBack, void *User);

//***************************************************************************
//*     LANIO��AI�̎����]�����I�����܂�
//*     ����   : handle   LANIO����n���h��
//***************************************************************************
DECLSPEC LALanioStopCyclicTransmit(hLANIO handle);

//***************************************************************************
//*     LANIO��LED�_��臒l��ݒ肵�܂�
//*     ����   : handle  LANIO����n���h��
//*              ch      �`�����l�����w�肵�܂��B 0:A*1 1:A*2 ...
//*              led     0:�w�肵���͈͓���LED�_�� 1:�͈͊O��LED�_��
//*              lthd    臒l����
//*              hthd    臒l���
//***************************************************************************
DECLSPEC LALanioSetThreshold(hLANIO handle, int ch, int led, int lthd, int hthd);

//***************************************************************************
//*     LANIO��LED�_��臒l���擾���܂�
//*     ����   : handle  LANIO����n���h��
//*              ch      �`�����l�����w�肵�܂��B 0:A*1 1:A*2 ...
//*              led     0:�w�肵���͈͓���LED�_�� 1:�͈͊O��LED�_��
//*              lthd    臒l����������o�b�t�@�ւ̃|�C���^
//*              hthd    臒l���������o�b�t�@�ւ̃|�C���^
//***************************************************************************
DECLSPEC LALanioGetThreshold(hLANIO handle, int ch, int *led, int *lthd, int *hthd);

//***************************************************************************
//*     LANIO�̔M�d�Α����ݒ肵�܂�
//*     ����   : handle  LANIO����n���h��
//*              ch      �`�����l�����w�肵�܂��B 0:AI1 1:AI2 ...
//*              tc_type �M�d�΂̃^�C�v���w��
//*                      0:K 1:J 2:T 3:E 4:N 5:R 6:S 7:B
//*              j       1�ŗ�ړ_�⏞�L��
//*              bd      1�Œf�����m�L��
//*              bf      �f�����m���̑���l 0:800000 1:7FFFFF
//***************************************************************************
DECLSPEC LALanioSetTC(hLANIO handle, int ch, int tc_type, int j, int bd, int bf);

//***************************************************************************
//*     LANIO�̔M�d�ΑΌ��o�͂�ݒ肵�܂�
//*     ����   : handle         LANIO����n���h��
//*              ch             �`�����l�����w�肵�܂��B 0:AI1 1:AI2 ...
//*              range          �Ό��o�̓����W
//*                             0:0-10V 1:0-5V 2:1-5V 3:4-20mA
//*              scale_lowest   �X�P�[�������ݒ�
//*              scale_highest  �X�P�[������ݒ�
//***************************************************************************
DECLSPEC LALanioSetTCnotice(hLANIO handle, int ch, int range, int scale_lowest, int scale_highest);

//***************************************************************************
//*     LANIO�̔M�d�Α����ݒ���擾���܂�
//*     ����   : handle  LANIO����n���h��
//*              ch             �`�����l�����w�肵�܂��B 0:AI1 1:AI2 ...
//*              tc_type        �M�d�΂̃^�C�v���w��
//*                             0:K 1:J 2:T 3:E 4:N 5:R 6:S 7:B
//*              j              1�ŗ�ړ_�⏞�L��
//*              bd             1�Œf�����m�L��
//*              bf             �f�����m���̑���l 0:800000 1:7FFFFF
//*              range          �Ό��o�̓����W
//*                             0:0-10V 1:0-5V 2:1-5V 3:4-20mA
//*              scale_lowest   �X�P�[�������ݒ�
//*              scale_highest  �X�P�[������ݒ�
//***************************************************************************
DECLSPEC LALanioGetTC(hLANIO handle, int ch, int *tc_type, int *j, int *bd, int *bf, int *range, int *scale_lowest, int *scale_highest);

//***************************************************************************
//*     DO�A���[�g��ݒ肵�܂�
//*     ����   : handle  LANIO����n���h��
//*              do_ch   �`�����l�����w�肵�܂��B 0:DO1 1:DO2 ...
//*              ai_ch   ������̓`�����l�����w�肵�܂��B 0:AI1 1:AI2 ...
//*              hd      �q�X�e���V�X�����w�� 0:LED�����̈�D�� 1:LED�_���̈�D��
//*              width   �q�X�e���V�X��
//***************************************************************************
DECLSPEC LALanioSetDOAlart(hLANIO handle, int do_ch, int ai_ch, int hd, int width);

//***************************************************************************
//*     DO�A���[�g�J�n/��~��ݒ肵�܂�
//*     ����   : handle  LANIO����n���h��
//*              do_ch   �`�����l�����w�肵�܂��B 0:DO1 1:DO2 ...
//*              start   0�Œ�~ 1�ŊJ�n
//***************************************************************************
DECLSPEC LALanioStartStopDOAlart(hLANIO handle, int do_ch, int start);

//***************************************************************************
//*     DO�A���[�g�ݒ���擾���܂�
//*     ����   : handle  LANIO����n���h��
//*              do_ch   �`�����l�����w�肵�܂��B 0:DO1 1:DO2 ...
//*              start   0�Œ�~ 1�ŊJ�n
//*              ai_ch   ������̓`�����l�����w�肵�܂��B 0:AI1 1:AI2 ...
//*              hd      �q�X�e���V�X�����w�� 0:LED�����̈�D�� 1:LED�_���̈�D��
//*              width   �q�X�e���V�X��
//***************************************************************************
DECLSPEC LALanioGetDOAlart(hLANIO handle, int do_ch, int *start, int *ai_ch, int *hd, int *width);