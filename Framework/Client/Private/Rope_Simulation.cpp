#include "stdafx.h"
#include "Rope_Simulation.h"
#include "Simulation.h"
#include "Mass.h"

CRope_Simulation::CRope_Simulation(
	_float _fSpringConstant,
	_float _fSpringFrictionConstant,
	_vector _vGravitation,
	_float _fAirFrictionConstant,
	_float _fGroundRepulsionConstant,
	_float _fGroundFrictionConstant,
	_float _fGroundAbsorptionConstant,
	_float _fGroundHeight
	) : CSimulation()
{
	m_fSpringConstant = _fSpringConstant;
	m_fSpringFrictionConstant = _fSpringFrictionConstant;
	m_vGravitation = _vGravitation;
	m_fAirFrictionConstant = _fAirFrictionConstant;
	m_fGroundRepulsionConstant = _fGroundRepulsionConstant;
	m_fGroundFrictionConstant = _fGroundFrictionConstant;
	m_fGroundAbsorptionConstant = _fGroundAbsorptionConstant;
	m_fGroundHeight = _fGroundHeight;
}

CRope_Simulation::~CRope_Simulation()
{

}

void CRope_Simulation::Init()
{
	__super::Init();
}

void CRope_Simulation::Solve()
{
	Set_SpringLength();

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

		if (i == m_iNum_Masses - 1)
		{
			Accelerator(m_vecMasses[i]);
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

	//¼±Çü °¨¼è
	//m_fCurTime += fTimeDelta;

	//if (m_fCurTime > m_fDurTime)
	//{
	//	m_fCurTime = m_fDurTime;
	//}
	//m_fRatio = 1.f - ((m_fCurTime / m_fDurTime) * (1.f - m_fMinRatio));

	//Áö¼ö °¨¼è
	_float fAlpha = 1 - exp(-fTimeDelta / m_fDurTime);

	if (m_fEpsilon < abs(m_fRatio - m_fMinRatio))
		m_fRatio = ((1.f - fAlpha) * m_fRatio) + (fAlpha * m_fMinRatio);
}

void CRope_Simulation::Operate(_float fTimeDelta)
{
	__super::Operate(fTimeDelta);
}

void CRope_Simulation::Start_Soft_Simulating(_vector _vDir, _vector _vPos, _float _fM, _float _fLastM)
{
	End_Simulating();

	m_vRopeConnection_Pos = _vPos;

	_float fTotalLength = XMVectorGetX(XMVector3Length(_vDir));

	m_iNum_Masses = (fTotalLength / m_fMaxSpringLength) + 2;
	m_fSpringLength = fTotalLength / (m_iNum_Masses - 1);

	Make_Mass(_fM, _fLastM);
	Make_Spring(XMVector3Normalize(_vDir));

	m_fRatio = 1.f;
	m_fCurTime = 0.f;

	Set_Simulating(true);
}

void CRope_Simulation::Switch_Soft_Simulating()
{
	_float fM = m_vecMasses[0]->Get_M();
	_vector vDir = m_vecMasses[1]->Get_Pos() - m_vecMasses[0]->Get_Pos();

	End_Simulating();

	_float fTotalLength = XMVectorGetX(XMVector3Length(vDir));

	m_iNum_Masses = (fTotalLength / m_fMaxSpringLength) + 2;
	m_fSpringLength = fTotalLength / (m_iNum_Masses - 1);

	Make_Mass(fM, fM);
	Make_Spring(XMVector3Normalize(vDir));

	m_fSpringLength *= (1.f / m_fRatio);

	Set_Simulating(true);
}

void CRope_Simulation::Start_Stiff_Simulating(_vector _vDir, _vector _vPos, _float _fM, _float _fLastM)
{
	End_Simulating();

	m_vRopeConnection_Pos = _vPos;
	m_iNum_Masses = 2;
	m_fSpringLength = XMVectorGetX(XMVector3Length(_vDir));

	Make_Mass(_fM, _fLastM);
	Make_Spring(XMVector3Normalize(_vDir));

	m_fRatio = 1.f;
	m_fCurTime = 0.f;

	Set_Simulating(true);
}

void CRope_Simulation::End_Simulating()
{
	Clear_Springs();
	Clear_Masses();

	Set_Simulating(false);
}

void CRope_Simulation::Make_Spring(_vector _vDir)
{
	for (_int i = 0; i < m_iNum_Masses; ++i)
	{
		m_vecMasses[i]->Set_Pos(m_vRopeConnection_Pos + (i * _vDir * m_fSpringLength * 1.f ));
	}

	vecSprings.reserve(m_iNum_Masses - 1);
	for (_int i = 0; i < m_iNum_Masses - 1; ++i)
	{
		CSpring* pSpring = new CSpring(m_vecMasses[i], m_vecMasses[i + 1], m_fSpringConstant, m_fSpringLength, m_fSpringFrictionConstant);
		vecSprings.push_back(pSpring);
	}
}

void CRope_Simulation::Render()
{
	if (!m_bSimulating)
		return;

	for (auto iter : vecSprings)
		iter->Render();
}

void CRope_Simulation::Set_Accelerating(_bool _isAccelerating, _float _fAccelerate_Force)
{
	m_isAccelerating = _isAccelerating;

	if (m_isAccelerating)
	{
		m_fAccelerate_Force = _fAccelerate_Force;
		_vector FinalMassPos = m_pFinalMass->Get_Pos();
		m_vbaseDir = XMVector3Normalize(m_vRopeConnection_Pos - FinalMassPos);
	}
}

void CRope_Simulation::Clear_Springs()
{
	for (auto iter : vecSprings)
		Safe_Release(iter);

	vecSprings.clear();
}

void CRope_Simulation::Accelerator(CMass* _pMass)
{
	if (!m_isAccelerating)
		return;
	
	_vector FinalMassPos = m_pFinalMass->Get_Pos();

	_vector vDir = XMVector3Normalize(m_vRopeConnection_Pos - FinalMassPos);
	_vector	vRight = XMVector3Normalize(XMVector3Cross(_vector{ 0.f, 1.f, 0.f }, m_vbaseDir));
	_vector	vLook = XMVector3Normalize(XMVector3Cross(vRight, vDir));

	_pMass->ApplyForce(XMVector3Normalize(2 * vLook - vDir) * m_fAccelerate_Force);

	//_vector FinalMassPos = m_pFinalMass->Get_Pos();

	//_vector vDir = XMVector3Normalize(m_vRopeConnection_Pos - FinalMassPos);
	//_vector	vRight = XMVector3Normalize(XMVector3Cross(_vector{0.f, 1.f, 0.f}, vDir));
	//_vector	vLook = XMVector3Normalize(XMVector3Cross(vRight, vDir));

	//_pMass->ApplyForce(XMVector3Normalize(2 * vLook - vDir) * m_fAccelerate_Force);
}

void CRope_Simulation::Set_SpringLength()
{
	for (auto iter : vecSprings)
		iter->Set_SpringLength(m_fSpringLength * m_fRatio);
}
 
void CRope_Simulation::Free()
{
	__super::Free();

	for (auto iter : vecSprings)
		Safe_Release(iter);

	vecSprings.clear();
}
