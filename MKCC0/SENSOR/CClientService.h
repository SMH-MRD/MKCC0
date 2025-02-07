#include "CBasicControl.h"

class CClientService : public CBasicControl
{
public:
	CClientService() {};
	~CClientService() {};

	virtual HRESULT initialize(void* pObj) override {
		return S_OK;
	}
	virtual HRESULT routine_work(void* pObj) override {
		return S_OK;
	}

};