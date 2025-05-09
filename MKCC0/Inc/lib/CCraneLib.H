#pragma once

#include <winsock.h>
#include <windows.h>

#include ".\spec\SPEC_DEF.h"
#include "COMMON_DEF.h"
#include "NET_DEF.h"
#include "PLC_DEF.h"
#include "OTE_DEF.h"
#include "CPlcIo.h"
#include "CCraneValue.h"

using namespace std;

typedef struct _ST_CRANE_OBJ {
	CBitSignal* estop;
	CBitSignal* estop_req;
	CBitSignal* csource;
	CBitSignal* csource_req;
	CBitSignal* remote_mode;
	CBitSignal* remote_mode_req;

}ST_CRANE_OBJ, * LPST_CRANE_OBJ;


#define N_REMOTE_CAMERA		16

class  CCraneBase {

private:
	static int _crane_id;
	static ST_SPEC _spec;

	static LPST_PC_U_MSG pbuf_r;
	static LPST_OTE_U_MSG pbuf_w;

public:
	static bool is_setup_ok;

	CCraneBase(int crane_id);
	virtual ~CCraneBase(void);

	static ST_CRANE_OBJ objs;
	static SOCKADDR_IN addr_pc;
	static SOCKADDR_IN addr_camera[N_REMOTE_CAMERA];

	static int get_id() { return _crane_id; }
	static int set_id(int crane_id) { _crane_id = crane_id; return _crane_id; }
	static ST_SPEC get_spec() { return _spec; }

	static HRESULT setup_crane(int crane_id);

};