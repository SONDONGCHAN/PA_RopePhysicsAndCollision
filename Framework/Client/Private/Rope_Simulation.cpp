#include "stdafx.h"
#include "Rope_Simulation.h"
#include "Simulation.h"
#include "Mass.h"

CRope_Simulation::CRope_Simulation(
	_int _iNum_Masses,
	_float _fM,
	_float _fSpringConstant,
	_float _fSpringLength,
	_float _fSpringFrictionConstant,
	_vector _vGravitation,
	_float _fAirFrictionConstant,
	_float _fGroundRepulsionConstant,
	_float _fGroundFrictionConstant,
	_float _fGroundAbsorptionConstant,
	_float _fGroundHeight
	) : CSimulation(_iNum_Masses, _fM)
{
	m_vGravitation = _vGravitation;
	m_fAirFrictionConstant = _fAirFrictionConstant;
	m_fGroundRepulsionConstant = _fGroundRepulsionConstant;
	m_fGroundFrictionConstant = _fGroundFrictionConstant;
	m_fGroundAbsorptionConstant = _fGroundAbsorptionConstant;
	m_fGroundHeight = _fGroundHeight;

	for (_int i = 0; i < _iNum_Masses; ++i)
	{
		m_vecMasses[i]->Set_Pos(_vector{ i * _fSpringLength, 0, 0 });
	}

	vecSprings.reserve(_iNum_Masses - 1);
	for (_int i = 0; i < _iNum_Masses - 1; ++i)
	{
		CSpring* pSpring = new CSpring(m_vecMasses[i], m_vecMasses[i + 1], _fSpringConstant, _fSpringLength, _fSpringFrictionConstant);
		vecSprings.push_back(pSpring);
	}

	
}

CRope_Simulation::~CRope_Simulation()
{

}

void CRope_Simulation::Solve()
{
	for (_int i = 0; i < m_iNum_Masses - 1; ++i)
	{
		vecSprings[i]->Solve();
	}

	for (_int i = 0; i < m_iNum_Masses; ++i)
	{
		m_vecMasses[i]->ApplyForce(m_vGravitation * m_vecMasses[i]->Get_M());
		m_vecMasses[i]->ApplyForce( (-(m_vecMasses[i]->Get_Vel())) * m_fAirFrictionConstant );

		if (m_vecMasses[i]->Get_Pos().m128_f32[1] < m_fGroundHeight)
		{
			_vector v = m_vecMasses[i]->Get_Vel();
			v.m128_f32[1] = 0;
			m_vecMasses[i]->ApplyForce( (-v) * m_fGroundFrictionConstant);

			v = m_vecMasses[i]->Get_Vel();
			v.m128_f32[0] = 0;
			v.m128_f32[2] = 0;

			if (v.m128_f32[1] < 0)
			{
				m_vecMasses[i]->ApplyForce((-v) * m_fGroundAbsorptionConstant);
			}

			_vector vForce = _vector{ 0.f, m_fGroundRepulsionConstant, 0 } * (m_fGroundHeight - m_vecMasses[i]->Get_Pos().m128_f32[1]);
			m_vecMasses[i]->ApplyForce(vForce);
		}
	}
}

void CRope_Simulation::Simulate(_float fTimeDelta)
{
	__super::Simulate(fTimeDelta);

	m_vRopeConnection_Pos += m_vRopeConnection_Vel * fTimeDelta;

	if (m_vRopeConnection_Pos.m128_f32[1] < m_fGroundHeight)
	{
		m_vRopeConnection_Pos.m128_f32[1] = m_fGroundHeight;
		m_vRopeConnection_Vel.m128_f32[1] = 0.f;
	}

	m_vecMasses[0]->Set_Pos(m_vRopeConnection_Pos);
	m_vecMasses[0]->Set_Vel(m_vRopeConnection_Vel);
}

void CRope_Simulation::Render()
{
	for (auto iter : vecSprings)
		iter->Render();
}

void CRope_Simulation::Free()
{
	__super::Free();

	for (auto iter : vecSprings)
		Safe_Release(iter);

	vecSprings.clear();
}
