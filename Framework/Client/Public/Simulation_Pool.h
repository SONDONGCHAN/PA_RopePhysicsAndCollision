#pragma once
#include "Client_Defines.h"
#include "Simulation.h"

BEGIN(Client)


class CSimulation_Pool : public CBase
{


public:
	CSimulation_Pool();
	virtual ~CSimulation_Pool() = default;

public:
	void	Tick(_float fTimeDelta);
	void	Render();
	
public:
	CSimulation*	Awake_Simulation(CSimulation::Simulation_Type _eSimulationType, void* _Datas = nullptr);

private:
	vector<CSimulation*> m_pSimulations[static_cast<_uint>(CSimulation::Simulation_Type::SIMULATION_END)];
	_uint	m_iSimulationNum[static_cast<_uint>(CSimulation::Simulation_Type::SIMULATION_END)];


public:
	virtual void Free() override;

};

END