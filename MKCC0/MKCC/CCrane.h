#pragma once
#include "CSpec.h"
#include "CPlc.h"
#include "CFaults.h"


class CCrane
{
private:
	int crane_id;

public:
	CCrane(int _crane_id) {
		crane_id = _crane_id;
		pSpec = new CSpec(crane_id);
		pPlc = new CPlc(crane_id);
	};
	virtual ~CCrane() {
		if (pSpec) delete pSpec;
		if (pPlc) delete pPlc;
		pSpec = nullptr;
		pPlc = nullptr;
	};

	CSpec* pSpec;
	CPlc* pPlc;

	int get_id() { return crane_id; };

};

