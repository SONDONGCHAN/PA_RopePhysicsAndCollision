#pragma once
#include "Base.h"
#include "Client_Defines.h"

class CSimulation : public CBase
{
public:
	CSimulation(_int _iNum_Masses, _float _M);
	virtual ~CSimulation();

	class CMass* Get_Mass(_int _index);

	virtual void Init();
	virtual void Solve();
	virtual void Simulate(_float dt);
	virtual void Operate(_float _dt, _vector force);

private:
	_int				m_iNum_Masses { 0 };
	vector<CMass*>		m_vecMasses { };

public:
	virtual void Free() override;
};

