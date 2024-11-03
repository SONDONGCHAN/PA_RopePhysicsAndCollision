#pragma once
#include "Client_Defines.h"
#include "Simulation.h"

BEGIN(Client)


class CSimulation_Pool : public CBase
{
public:
	enum Simulation_Type
	{
		SIMULATION_ROPE,

		SIMULATION_END
	};

public:
	CSimulation_Pool();
	virtual ~CSimulation_Pool() = default;

public:
	void	Tick(_float fTimeDelta);
	
public:
	void	Awake_Simulation(Simulation_Type _eSimulationType);

private:
	vector<class CSimulation*> m_pSimulations[Simulation_Type::SIMULATION_END];
	_uint	m_iSimulationNum[Simulation_Type::SIMULATION_END];


public:
	virtual void Free() override;

};

END