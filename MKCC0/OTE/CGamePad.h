#pragma once

#include <Windows.h>
#include <Xinput.h>

class CGamePad
{
public:
	CGamePad() {}
	virtual ~CGamePad(void) {}

	XINPUT_STATE state;
	DWORD	error_code;
	int controllerId = 0;

	void VibrateController(int controllerId, WORD leftMotor, WORD rightMotor);
	DWORD PollController(int controllerId);

	void set_id(int id) { controllerId = id; return; }
	WORD get_up(){return state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP;}
	WORD get_down(){return state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN;}
	WORD get_left(){return state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT;}
	WORD get_right(){return state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT;}
	WORD get_start(){return state.Gamepad.wButtons & XINPUT_GAMEPAD_START;}
	WORD get_back(){return state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK;}
	WORD get_thumbl(){return state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB;}
	WORD get_thumbr() {return state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB;}
	WORD get_shoulderl(){return state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER;}
	WORD get_shoulderr(){return state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER;}
	WORD get_A(){return state.Gamepad.wButtons & XINPUT_GAMEPAD_A;}
	WORD get_B(){return state.Gamepad.wButtons & XINPUT_GAMEPAD_B;}
	WORD get_X(){return state.Gamepad.wButtons & XINPUT_GAMEPAD_X;}
	WORD get_Y(){return state.Gamepad.wButtons & XINPUT_GAMEPAD_Y;}

	SHORT get_LX(){ return state.Gamepad.sThumbLX; }
	SHORT get_LY() { return state.Gamepad.sThumbLY; }
	SHORT get_RX() { return state.Gamepad.sThumbRX; }
	SHORT get_RY() { return state.Gamepad.sThumbRY; }
		
};

