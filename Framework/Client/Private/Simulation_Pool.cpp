#include "stdafx.h"
#include "Simulation_Pool.h"
#include "Simulation.h"
#include "Rope_Simulation.h"


CSimulation_Pool::CSimulation_Pool()
{
	m_iSimulationNum[static_cast<_uint>(CSimulation::Simulation_Type::SIMULATION_ROPE)] = 10;


	for (_uint i = 0; i < static_cast<_uint>(CSimulation::Simulation_Type::SIMULATION_END); ++i)
	{
		for (_uint j = 0; j < m_iSimulationNum[i]; ++j)
		{
			CSimulation* pSimulation = nullptr;

			switch (i)
			{
			case static_cast<_uint>(CSimulation::Simulation_Type::SIMULATION_ROPE):
				pSimulation = new CRope_Simulation
				(10000.f,
					10.f,
					_vector{ 0.f, -9.81f, 0 },
					0.02f,
					100.f,
					0.2f,
					2.f,
					-50.f);

				break;

			case static_cast<_uint>(CSimulation::Simulation_Type::SIMULATION_END):
				break;

			default:
				break;
			}

			if (pSimulation == nullptr)
				continue;

			pSimulation->Set_Type(i);
			m_pSimulations[i].push_back(pSimulation);		
		}
	}
}

void CSimulation_Pool::Tick(_float fTimeDelta)
{
	for (int i = 0; i < static_cast<_uint>(CSimulation::Simulation_Type::SIMULATION_END); ++i)
	{
		for (auto& iter : m_pSimulations[i])
		{
			iter->Operate(fTimeDelta);
		}
	}
}

void CSimulation_Pool::Render()
{
	for (int i = 0; i < static_cast<_uint>(CSimulation::Simulation_Type::SIMULATION_END); ++i)
	{
		for (auto& iter : m_pSimulations[i])
		{
			iter->Render();
		}
	}
}

CSimulation* CSimulation_Pool::Awake_Simulation(CSimulation::Simulation_Type _eSimulationType, void* _Datas)
{
	for (auto pSimulation : m_pSimulations[static_cast<_uint>(_eSimulationType)])
	{
		if (!pSimulation->Get_Simulating())
		{
			pSimulation->Start_Simulating(_Datas);
			return pSimulation;
		}
	}
	return nullptr;
}

void CSimulation_Pool::Free()
{
	for (int i = 0; i < static_cast<_uint>(CSimulation::Simulation_Type::SIMULATION_END); ++i)
	{
		for (auto& iter : m_pSimulations[i])
		{
			Safe_Release(iter);
		}
		m_pSimulations[i].clear();
	}

}
