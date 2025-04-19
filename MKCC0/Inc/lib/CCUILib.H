#pragma once

#include <Windows.h>

#include <dinput.h>

//DIRECTINPUT
#define DIRECTINPUT_VERSION 0x0800
#define GPAD_TYPE_DINPUT		0
#define GPAD_TYPE_XINPUT		1

class CGPad
{
public:
	CGPad() {}
	virtual ~CGPad(void) {}

	HRESULT Initialize(HWND hWnd);
	HRESULT Close();
	HRESULT GetStat(DIJOYSTATE* pStat);
	VOID VibrateController();
	UINT32 SetPadMode(UINT32 mode);
	void ClearData() { memset(&pad_data,0,sizeof(DIJOYSTATE)); };

	static LPDIRECTINPUT8		pDI;			//!< DIRECTINPUT8のポインタ
	static LPDIRECTINPUTDEVICE8	pGamePad;			//!< DIRECTINPUTDEVICE8のポインタ
	static DIJOYSTATE			pad_data;				//!< DIRECTINPUTDEVICE8の状態読み込みバッファ

	static LPDIRECTINPUTEFFECT	lpdiEffect;		// receives pointer to created effect
	static DIEFFECT				diEffect;		// parameters for created effect
	static DWORD				dwAxes[2];
	static LONG					lDirection[2];
	static DICONSTANTFORCE		diConstantForce;
		
	static UINT32				pad_mode;
	static UINT32				pad_enable;

};