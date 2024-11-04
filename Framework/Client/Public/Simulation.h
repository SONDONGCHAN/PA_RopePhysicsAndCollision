#pragma once
#include "Base.h"
#include "Client_Defines.h"

BEGIN(Client)

class CSimulation : public CBase
{
public:
	enum class Simulation_Type
	{
		SIMULATION_ROPE,

		SIMULATION_END
	};

public:
	CSimulation();
	virtual ~CSimulation();

	class CMass* Get_Mass(_int _index);

	virtual void Init();
	virtual void Solve() {};
	virtual void Simulate(_float fTimeDelta);
	virtual void Operate(_float fTimeDelta);

public:
	virtual void Render() {};

protected:
	void	Make_Mass(_float _fM, _float _fLastM);
	void	Clear_Masses();

public:
	virtual void	Start_Simulating(void* _Datas) {};

public:
	void	Set_Simulating(_bool _isSimulating) { m_bSimulating = _isSimulating; }
	_bool	Get_Simulating() {return  m_bSimulating ; }

	void	Set_Type(_uint _Type) { m_eSimulationType = static_cast<Simulation_Type>(_Type); }


public:
	class CMass*	Get_FinalMass() { return m_pFinalMass; }

protected:
	_int				m_iNum_Masses { 0 };
	vector<CMass*>		m_vecMasses { };
	CMass*				m_pFinalMass{ nullptr };
	_float				m_fMaxPossible_dt = { 0.002f };
	_bool				m_bSimulating = { false };
	Simulation_Type		m_eSimulationType{};


public:
	virtual void Free() override;
};

END