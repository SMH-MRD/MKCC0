#pragma once

#include "CBasicControl.h"
#include <opencv2/opencv.hpp>
#include "SENSOR_DEF.H"

// �摜���
// �摜���f�[�^
typedef struct TAG_INFO_IMGBUF_DATA {
    cv::Mat cv_mat; // CpenCV
} INFO_IMGBUF_DATA, * PINFO_IMGBUF_DATA;
// �摜���
typedef struct TAG_INFO_IMGBUF {
    CRITICAL_SECTION cs;
    INFO_IMGBUF_DATA data;  // �摜���f�[�^
} INFO_IMGBUF, * PINFO_IMGBUF;

// �摜�������
// �^�[�Q�b�g���o�f�[�^

#define  SWAY_SENSOR__RANGE_OVER_COUNT_LIMIT    40

typedef struct TAG_TARGET_DATA {
    BOOL     valid;                                         // ���o���
    int      range_over_count;                              //�����W�I�[�o�[�J�E���g
    double   max_val;                                       // �ő�P�x
    double   pos[static_cast<uint32_t>(ENUM_AXIS::E_MAX)];  // ���o�ʒu[pixel]
    int32_t  size;                                          // ���o�T�C�Y
    cv::Rect roi;                                           // ROI
    cv::Size2i size_real;                                   // �^�[�Q�b�g���T�C�Ymm
    cv::Size2i size_expected;                              // �J�����Ƃ̋����ɂ��z��^�[�Q�b�g�T�C�Y
    cv::Size2i size_detected;                              // ���o�^�[�Q�b�g�T�C�Y
    cv::Size2i size_roi_spd_margin;                        // scan��roi�̈ړ�
    double dist_camera;                                     // �J�����|�^�[�Q�b�g�ԋ���

    BOOL    chk_flg;                                        //�f�o�b�O�p

    TAG_TARGET_DATA()
        : valid(FALSE)
        , range_over_count(0)
        , max_val(0.0)
        , pos{ 0.0, 0.0 }
        , size(0)
        , roi(0, 0, 100, 100)
        , size_real(80, 80)
        , size_expected(10, 10)
        , size_detected(10, 10)
        , size_roi_spd_margin(50, 50)
        , dist_camera(1.0)
        , chk_flg(FALSE)
    {
    }
} TARGET_DATA, * PTARGET_DATA;
// �U�ꌟ�o�f�[�^(�^�[�Q�b�g���o�f�[�^�̒��S)
typedef struct TAG_SWAY_DATA {
    double target_pos;  // �^�[�Q�b�g�ʒu[pixel]
    double target_tilt; // �^�[�Q�b�g�X��[pixel]
    double sway_angle;  // �U��p[pixel]
    double sway_speed;  // �U�ꑬ�x[pixel/s]
    double sway_zero;   // �U�ꒆ�S[pixel]

    TAG_SWAY_DATA()
        : target_pos(0.0)
        , target_tilt(0.0)
        , sway_angle(0.0)
        , sway_speed(0.0)
        , sway_zero(0.0)
    {
    }
} SWAY_DATA, * PSWAY_DATA;
// �摜�������f�[�^
typedef struct TAG_INFO_IMGPRC_DATA {
    std::vector<std::vector<cv::Point>> contours[static_cast<uint32_t>(ENUM_IMAGE_MASK::E_MAX)];    // �֊s���W
    TARGET_DATA target_data[static_cast<uint32_t>(ENUM_IMAGE_MASK::E_MAX)];                         // �^�[�Q�b�g���o�f�[�^
    SWAY_DATA   sway_data[static_cast<uint32_t>(ENUM_AXIS::E_MAX)];                                 // �U�ꌟ�o�f�[�^(�^�[�Q�b�g���o�f�[�^�̒��S)
    double      target_size;                                                                        // �^�[�Q�b�g�T�C�Y(�^�[�Q�b�g���o�f�[�^�̕���)
    uint32_t    status;                                                                             // ���
    double      img_fps;                                                                            // �t���[�����[�g[fps]
    double      img_val;                                                                            // ���x
    int32_t     exps_mode;                                                                          // �����I���R�}���h(0:��~ 1:Up -1:Down)
    int32_t     exps_ctrl_mode;                                                                     // �I���ݒ胂�[�h(0:RESET 1:ROI)
    double      exps_time;                                                                          // �I������[us]
    int32_t     exps_step_count;
    double      exps_chk_brightness;                                                                //�I�����Ԓ����p�P�x
    cv::Scalar mean_hsv;    //HSV�t�H�[�}�b�g�ł̕��ϒl

    TAG_INFO_IMGPRC_DATA()
        : target_size(0.0)
        , status(static_cast<uint32_t>(ENUM_PROCCESS_STATUS::DEFAULT))
        , img_fps(0.0)
        , img_val(0.0)
        , exps_mode(0)
        , exps_ctrl_mode(0)
        , exps_time(0.0)
    {
    }
} INFO_IMGPRC_DATA, * PINFO_IMGPRC_DATA;
// �摜�������
typedef struct TAG_INFO_IMGPRC {
    CRITICAL_SECTION cs;
    INFO_IMGPRC_DATA data;  // �摜�������f�[�^
} INFO_IMGPRC, * PINFO_IMGPRC;

// �V�X�e�����
// �V�X�e�����f�[�^
typedef struct TAG_INFO_SYSTEM_DATA {
    uint32_t status;    // �X�e�[�^�X

    TAG_INFO_SYSTEM_DATA()
        : status(static_cast<uint32_t>(ENUM_SYSTEM_STATUS::DEFAULT))
    {
    }
} INFO_SYSTEM_DATA, * PINFO_SYSTEM_DATA;
// �V�X�e�����
typedef struct TAG_INFO_SYSTEM {
    CRITICAL_SECTION cs;
    INFO_SYSTEM_DATA data;  // �V�X�e�����f�[�^
} INFO_SYSTEM, * PINFO_SYSTEM;

// ***�A�v���P�[�V�������
typedef struct TAG_APP_INFO {
    INFO_IMGBUF imgbuf[static_cast<uint32_t>(ENUM_IMAGE::E_MAX)];   // �摜���
    INFO_TILTMT tiltmt;                                             // �X�Όv���
    INFO_CLIENT client;                                             // �N���C�A���g���
    INFO_ADJUST adjust;                                             // �������
    INFO_IMGPRC imgprc;                                             // �摜�������
    INFO_SYSTEM system;                                             // �V�X�e�����
} APP_INFO, * PAPP_INFO;

class CSway :   public CBasicControl
{
};

