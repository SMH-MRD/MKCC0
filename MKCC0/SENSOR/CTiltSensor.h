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
    //オーバーライド

    virtual int routine_work(void* pObj) override;

    int set_outbuf(LPVOID) {//出力バッファセット
        return STAT_NG;
    }                 

    int input() {           //入力処理
        return STAT_NG;
    }
    int parse() {           //メイン処理
        return STAT_NG;
    }
    int output() {          //出力処理
        return STAT_NG;
    }
    int close() { 
        CloseHandle(td_hndl_init);
        CloseHandle(td_hndl_run);
    } 
};

