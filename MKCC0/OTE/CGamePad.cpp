#include "CGamePad.h"
#include <windows.h>

#include <iostream>
#pragma comment(lib, "xinput.lib")

void CGamePad::VibrateController(int controllerId, WORD leftMotor, WORD rightMotor) {
    XINPUT_VIBRATION vibration = {};
    vibration.wLeftMotorSpeed = leftMotor;  // 0〜65535 0〜FFFF
    vibration.wRightMotorSpeed = rightMotor;
    XInputSetState(controllerId, &vibration);
}

DWORD CGamePad::PollController(int controllerId) {
    return error_code = XInputGetState(controllerId, &state);
}
