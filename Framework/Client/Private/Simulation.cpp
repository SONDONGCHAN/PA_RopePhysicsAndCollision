#include "stdafx.h"
#include "Simulation.h"
#include "Mass.h"

CSimulation::CSimulation(_int _iNum_Masses, _float _fM)
{
	m_iNum_Masses = _iNum_Masses;
	m_vecMasses.reserve(m_iNum_Masses);

	for (_int i = 0; i < m_iNum_Masses; ++i)
	{
		CMass* pMass = new CMass(_fM);
		m_vecMasses.push_back(pMass);
	}
}

CSimulation::~CSimulation()
{

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
	Init();
	Solve();
	Simulate(fTimeDelta);
}

void CSimulation::Free()
{
	for (auto iter : m_vecMasses)
		Safe_Release(iter);

	m_vecMasses.clear();
}