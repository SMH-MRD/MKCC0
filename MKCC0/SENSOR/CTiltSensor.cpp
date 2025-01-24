
#include "CTiltSensor.h"

unsigned CTiltSensor::initialize(void* pObj) {

	CTiltSensor* pTiltObj = (CTiltSensor*)pObj;
	int code = 0;

	wos.str(L"");
	if ((code = LALanioInit()) == 0) {
		wos<<L"LANIO Init NG : Code "<< LALanioGetLastError();
		msg2listview(wos.str());
		return STAT_NG;
	}
	else{
		wos <<L"LANIO Init OK : Code "<< code;
	}
	msg2listview(wos.str());	

	int laniocount;
	int timeout = 1000;	
	wos.str(L"");

	if ((laniocount = LALanioSearch(timeout)) < 0) {
		wos << L"LANIO Search ERROR : Code "<< LALanioGetLastError();
		msg2listview(wos.str());
		return STAT_NG;
	}
	else if(laniocount == 0){
	//	LALanioConnectDirect(L"192,168,100,30", 1000);
		wos << L"LANIO Search CANNOT FIND";
		msg2listview(wos.str());
		return STAT_NG;
	}
	else {
		wos << L"LANIO Search OK : Count " << laniocount;
		char ipaddress[16];
        int error = LALanioGetIpAddress(0, ipaddress);
		wos <<L"=> IP : " << ipaddress;
		msg2listview(wos.str());

		hlanio = LALanioConnect(0);
		wos.str(L"");	
		if (hlanio == -1) {
			wos << L"LANIO Connect ERROR : Code " << LALanioGetLastError();
			msg2listview(wos.str());
			return STAT_NG;
		}
		else {
			wos << L"LANIO Connect OK : Handle " << hlanio;
			msg2listview(wos.str());
		}
	}


	return STAT_OK;
}
int CTiltSensor::routine_work(void* pObj){
	return STAT_OK;
}