#include "stdafx.h"
#include "Rope_Simulation.h"
#include "Simulation.h"
#include "Mass.h"
#include "Spring.h"


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
	m_fSpringConstant			= _fSpringConstant;
	m_fSpringFrictionConstant	= _fSpringFrictionConstant;
	m_vGravitation				= _vGravitation;
	m_fAirFrictionConstant		= _fAirFrictionConstant;
	m_fGroundRepulsionConstant	= _fGroundRepulsionConstant;
	m_fGroundFrictionConstant	= _fGroundFrictionConstant;
	m_fGroundAbsorptionConstant = _fGroundAbsorptionConstant;
	m_fGroundHeight				= _fGroundHeight;
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
	//Set_SpringLength();

	for (_int i = 0; i < m_iNum_Masses - 1; ++i)
	{
		vecSprings[i]->Solve(m_eSimulateMode);
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



	//선형 감쇠
	//m_fCurTime += fTimeDelta;

	//if (m_fCurTime > m_fDurTime)
	//{
	//	m_fCurTime = m_fDurTime;
	//}
	//m_fRatio = 1.f - ((m_fCurTime / m_fDurTime) * (1.f - m_fMinRatio));

	//지수 감쇠
	//_float fAlpha = 1 - exp(-fTimeDelta / m_fDurTime);

	//if (m_fEpsilon < abs(m_fRatio - m_fMinRatio))
	//	m_fRatio = ((1.f - fAlpha) * m_fRatio) + (fAlpha * m_fMinRatio);
}

void CRope_Simulation::Operate(_float fTimeDelta)
{
	__super::Operate(fTimeDelta);

	if (m_isHolding)
	{
		m_fHoldingCurrentTime -= fTimeDelta;
		if (m_fHoldingCurrentTime < 0.f)
		{
			m_isHolding = false;
			m_bSimulating = false;
		}
	}
}

void CRope_Simulation::Start_Simulating(void* _Datas)
{
	Rope_Simulation_Data* pData = (Rope_Simulation_Data*)_Datas;

	End_Simulating();
	m_eSimulateMode = SimulateMode::MODE_STIFF;

	m_vRopeConnection_Pos = pData->vPos;
	m_iNum_Masses = 2;
	m_fSpringLength = XMVectorGetX(XMVector3Length(pData->vDir));

	Make_Mass(pData->fM, pData->fLastM);
	Make_Spring(XMVector3Normalize(pData->vDir));

	m_pFinalMass->Set_Vel(pData->vStartVel);

	m_fRatio = 1.f;
	m_fCurTime = 0.f;

	Set_Simulating(true);
}

void CRope_Simulation::Switch_Soft_Simulating(_vector _vVel)
{
	_float fM = m_vecMasses[0]->Get_M();
	_vector vDir = m_vecMasses[1]->Get_Pos() - m_vecMasses[0]->Get_Pos();

	End_Simulating();
	m_eSimulateMode = SimulateMode::MODE_SOFT;

	_float fTotalLength = XMVectorGetX(XMVector3Length(vDir));

	m_iNum_Masses = (fTotalLength / m_fMaxSpringLength) + 2;
	m_fSpringLength = fTotalLength / (m_iNum_Masses - 1);

	Make_Mass(fM, fM);
	Make_Spring(XMVector3Normalize(vDir));

	m_fSpringLength *= (1.f / m_fRatio);

	// 속도 전달
	for (_int i = 0; i < m_iNum_Masses; ++i)
	{
		_vector vVel = _vVel * (i / static_cast<float>(m_iNum_Masses - 1));
		m_vecMasses[i]->Set_Vel(vVel);
	}

	Set_Simulating(true);

	m_isHolding = true;
	m_fHoldingCurrentTime = m_fHoldingTime;
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

	//_pMass->ApplyForce(XMVector3Normalize(2 * vLook - vDir) * m_fAccelerate_Force);

	_pMass->ApplyForce(XMVector3Normalize(vLook) * m_fAccelerate_Force);

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
