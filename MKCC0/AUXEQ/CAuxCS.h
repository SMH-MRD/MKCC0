#include "CBasicControl.h"

class CAuxCS : public CBasicControl
{
public:
	CAuxCS() {};
	~CAuxCS() {};

	virtual HRESULT initialize(void* pObj) override {
		return S_OK;
	}
	virtual HRESULT routine_work(void* pObj) override {
		return S_OK;
	}

};