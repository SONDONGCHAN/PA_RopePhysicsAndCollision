#pragma once
#include "Base.h"
#include "Client_Defines.h"

BEGIN(Client)

class CSimulation : public CBase
{
public:
	CSimulation();
	virtual ~CSimulation();

	class CMass* Get_Mass(_int _index);

	virtual void Init();
	virtual void Solve() {};
	virtual void Simulate(_float fTimeDelta);
	virtual void Operate(_float fTimeDelta);

protected:
	void	Make_Mass(_float _fM, _float _fLastM);
	void	Clear_Masses();
	void	Set_Simulating(_bool _isSimulating) { m_bSimulating = _isSimulating; }

public:
	class CMass*	Get_FinalMass() { return m_pFinalMass; }

protected:
	_int				m_iNum_Masses { 0 };
	vector<CMass*>		m_vecMasses { };
	CMass*				m_pFinalMass{ nullptr };
	_float				m_fMaxPossible_dt = { 0.002f };
	_bool				m_bSimulating = { false };

public:
	virtual void Free() override;
};

END