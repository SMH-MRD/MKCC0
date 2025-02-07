// CCUILib.cpp : スタティック ライブラリ用の関数を定義します。
//

#include "pch.h"
#include "framework.h"
#include "CCUILib.h"

#include <dinput.h>


LPDIRECTINPUT8			CGPad::pDI;
LPDIRECTINPUTDEVICE8	CGPad::pGamePad;
DIJOYSTATE				CGPad::pad_data;
						
LPDIRECTINPUTEFFECT		CGPad::lpdiEffect;
DIEFFECT				CGPad::diEffect;
DWORD					CGPad::dwAxes[2];
LONG					CGPad::lDirection[2];
DICONSTANTFORCE			CGPad::diConstantForce;
						
UINT32					CGPad::pad_mode;
UINT32					CGPad::pad_enable;



HRESULT CGPad::Initialize(HWND hWnd) {
	HRESULT hr;
	// DirectInput デバイスの作成
	hr = DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&pDI, NULL);
	if (FAILED(hr)) return S_FALSE;

	// ジョイスティック デバイスの作成
	hr = pDI->CreateDevice(GUID_Joystick, &pGamePad, NULL);
	if (FAILED(hr))	return S_FALSE;

	// デバイスのデータフォーマットを設定
	hr = pGamePad->SetDataFormat(&c_dfDIJoystick);
	if (FAILED(hr)) return S_FALSE;

	// デバイスの協調レベルを設定
	hr = pGamePad->SetCooperativeLevel(hWnd, DISCL_EXCLUSIVE | DISCL_BACKGROUND);
	if (FAILED(hr)) return S_FALSE;

	// デバイスのアクセスを開始	
	hr = pGamePad->Acquire();
	if (FAILED(hr)) return S_FALSE;

	//エフェクト作成
	diConstantForce.lMagnitude = DI_FFNOMINALMAX;				// Full force
	//	diConstantForce.lMagnitude = 100;							// 1% force

	dwAxes[0] = DIJOFS_X; dwAxes[1] = DIJOFS_Y ;
	lDirection[0] = 18000; lDirection[1] = 0;

	diEffect.dwSize = sizeof(DIEFFECT);
	diEffect.dwFlags = DIEFF_POLAR | DIEFF_OBJECTOFFSETS;
	diEffect.dwDuration = (DWORD)(3.0 * DI_SECONDS);			//とりあえず3秒間のOFFDELAY
	diEffect.dwSamplePeriod = 0;								// = default 
	diEffect.dwGain = DI_FFNOMINALMAX;							// No scaling
	diEffect.dwTriggerButton = DIEB_NOTRIGGER;					// Not a button response
	diEffect.dwTriggerRepeatInterval = 0;						// Not applicable
	diEffect.cAxes = 2;
	diEffect.rgdwAxes = &dwAxes[0];
	diEffect.rglDirection = &lDirection[0];
	diEffect.lpEnvelope = NULL;
	diEffect.cbTypeSpecificParams = sizeof(DICONSTANTFORCE);
	diEffect.lpvTypeSpecificParams = &diConstantForce;

	hr = pGamePad->CreateEffect(GUID_ConstantForce,
		&diEffect,
		&lpdiEffect,
		NULL);

	if (FAILED(hr)) {
		return S_FALSE;
	}

	return S_OK;
}

HRESULT CGPad::GetStat(DIJOYSTATE* pStat) {

	if (pGamePad != NULL) {
		pad_enable = pGamePad->GetDeviceState(sizeof(DIJOYSTATE), pStat);//hr成功時　DI_OK(0);
		if (FAILED(pad_enable)) {//エラー時リトライ
			pad_enable = S_FALSE;
			pGamePad->Acquire();
			pGamePad->GetDeviceState(sizeof(DIJOYSTATE), pStat);
		}

		if ((pStat->rgdwPOV[1] != 0xffffffff))
			pad_enable = S_FALSE;
		else
			pad_enable = S_OK;

	}
	else {
		pad_enable = S_FALSE;
	}
	return pad_enable;
}

HRESULT CGPad::Close() {
	if (pGamePad != NULL) {
		pGamePad->Unacquire();
		pGamePad->Release();
		pGamePad = NULL;
	}

	if (pDI) {
		pDI->Release();
		pDI = NULL;
	}
	return S_OK;

}

VOID CGPad::VibrateController() {
	// デバイスにエフェクトを送信
	if (lpdiEffect == NULL)return;
	if (FAILED(lpdiEffect->Start(1, 0))) {
		lpdiEffect->Release();
		return;
	}
	return;
}

UINT32 CGPad::SetPadMode(UINT32 mode) {
	pad_mode = mode;
	return pad_mode;
}