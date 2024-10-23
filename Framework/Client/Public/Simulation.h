#pragma once
#include "Base.h"
#include "Client_Defines.h"

BEGIN(Client)

class CSimulation : public CBase
{
public:
	CSimulation(_int _iNum_Masses, _float _fM);
	virtual ~CSimulation();

	class CMass* Get_Mass(_int _index);

	virtual void Init();
	virtual void Solve() {};
	virtual void Simulate(_float fTimeDelta);
	virtual void Operate(_float fTimeDelta);

protected:
	_int				m_iNum_Masses { 0 };
	vector<CMass*>		m_vecMasses { };
	_float				m_fMaxPossible_dt = { 0.002f };
	_bool				m_bSimulating = { true };

public:
	virtual void Free() override;
};

END