#include "stdafx.h"
#include "Simulation.h"
#include "Mass.h"

CSimulation::CSimulation()
{

}

CSimulation::~CSimulation()
{

}

void CSimulation::Make_Mass(_float _fM, _float _fLastM)
{
	m_vecMasses.reserve(m_iNum_Masses);

	for (_int i = 0; i < m_iNum_Masses; ++i)
	{
		CMass* pMass;

		if (i == m_iNum_Masses - 1)
		{
			pMass = new CMass(_fLastM);
			m_pFinalMass = pMass;
			Safe_AddRef(pMass);
		}
		else
			pMass = new CMass(_fM);

		m_vecMasses.push_back(pMass);
	}
}

CMass* CSimulation::Get_Mass(_int _index)
{
	if (_index < 0 || _index >= m_iNum_Masses)
		return nullptr;

	return m_vecMasses[_index];
}

void CSimulation::Init()
{
	for (_int i = 0; i < m_iNum_Masses; ++i) 
	{
		m_vecMasses[i]->InitForce();
	}
}

void CSimulation::Simulate(_float fTimeDelta)
{
	for (_int i = 0; i < m_iNum_Masses; ++i)
	{
		m_vecMasses[i]->Simulate(fTimeDelta);
	}
}

void CSimulation::Operate(_float fTimeDelta)
{
	if (!m_bSimulating)
		return;

	if (fTimeDelta > m_fMaxPossible_dt)
	{
		_uint iRepeatCount = (fTimeDelta / m_fMaxPossible_dt) + 1;
	
		_float Subdivided_TimeDelta = fTimeDelta / iRepeatCount;
	
		for (_int j = 0; j < iRepeatCount; ++j)
		{
			Init();
			Solve();
			Simulate(Subdivided_TimeDelta);		
		}
	}
	
	else
	{
		Init();
		Solve();
		Simulate(fTimeDelta);	
	}
}

void CSimulation::Clear_Masses()
{
	for (auto iter : m_vecMasses)
		Safe_Release(iter);
	
	m_vecMasses.clear();
}

void CSimulation::Free()
{
	for (auto iter : m_vecMasses)
		Safe_Release(iter);

	m_vecMasses.clear();

	Safe_Release(m_pFinalMass);
}