#include "CBasicControl.h"

class CClientService : public CBasicControl
{
public:
	CClientService() {};
	~CClientService() {};

	virtual unsigned initialize(void* pObj) override {
		return STAT_OK;
	}
	virtual int routine_work(void* pObj) override {
		return STAT_OK;
	}

};