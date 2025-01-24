#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <ws2tcpip.h>
#include "framework.h"

#include "common_def.h"
#include "CBasicControl.h"



#define TILT_WND_X  0
#define TILT_WND_Y  0
#define TILT_WND_W  320
#define TILT_WND_H  240

class CTiltSensor :
    public CBasicControl
{
public:
    CTiltSensor() {};
    ~CTiltSensor() {};

	HANDLE td_hndl_init;
	HANDLE td_hndl_run;
    hLANIO hlanio;

    virtual unsigned initialize(LPVOID lpParam) override;
  	
private:
    //�I�[�o�[���C�h

    virtual int routine_work(void* pObj) override;

    int set_outbuf(LPVOID) {//�o�̓o�b�t�@�Z�b�g
        return STAT_NG;
    }                 

    int input() {           //���͏���
        return STAT_NG;
    }
    int parse() {           //���C������
        return STAT_NG;
    }
    int output() {          //�o�͏���
        return STAT_NG;
    }
    int close() { 
        CloseHandle(td_hndl_init);
        CloseHandle(td_hndl_run);
    } 
};

