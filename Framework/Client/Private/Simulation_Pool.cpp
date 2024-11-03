#include "Simulation_Pool.h"
#include "Simulation.h"
#include "Rope_Simulation.h"


CSimulation_Pool::CSimulation_Pool()
{
	m_iSimulationNum[Simulation_Type::SIMULATION_ROPE] = 10;

	for (_uint i = Simulation_Type::SIMULATION_ROPE; i < Simulation_Type::SIMULATION_END; ++i)
	{
		for (_uint j = 0; j < m_iSimulationNum[i]; ++j)
		{
			CSimulation* pSimulation;
			switch (i)
			{
			case SIMULATION_ROPE:
				pSimulation = new CRope_Simulation
				(10000.f,
					1.f,
					_vector{ 0.f, -9.81f, 0 },
					0.02f,
					100.f,
					0.2f,
					2.f,
					-50.f);

				break;

			case SIMULATION_END:
				break;

			default:
				break;

			}

			if (pSimulation)
			{
				m_pSimulations[i].push_back(pSimulation);
			}
		}
	}
}

void CSimulation_Pool::Tick(_float fTimeDelta)
{

	for (int i = 0; i < Simulation_Type::SIMULATION_END; ++i)
	{
		for (auto& iter : m_pSimulations[i])
		{
			iter->Operate(fTimeDelta);
		}
	}
}

void CSimulation_Pool::Awake_Simulation(Simulation_Type _eSimulationType)
{
	for (auto pSimulation : m_pSimulations[_eSimulationType])
	{
		if (!pSimulation->Get_Simulating())
		{
			pSimulation->Set_Simulating(true);
			break;
		}
	}
}

void CSimulation_Pool::Free()
{
	for (int i = 0; i < Simulation_Type::SIMULATION_END; ++i)
	{
		for (auto& iter : m_pSimulations[i])
		{
			Safe_Release(iter);
		}
		m_pSimulations[i].clear();
	}

}
