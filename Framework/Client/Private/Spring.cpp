#include "stdafx.h"
#include "Spring.h"
#include "Mass.h"

CSpring::CSpring(CMass* _pMass1, CMass* _pMass2, _float _fSpringConstant, _float _fSpringLength, _float _fFrictionConstant)
{
	m_pMass1 = _pMass1;
	m_pMass2 = _pMass2;

	m_fSpringConstant;
	m_fSpringLength;
	m_fFrictionConstant;
}

CSpring::~CSpring()
{
}

void CSpring::Solve()
{
	_vector vSpringVector = m_pMass1->Get_Pos() - m_pMass2->Get_Pos();
	
	_float fLength = XMVectorGetX(XMVector3Length(vSpringVector));

	_vector vForce {0.f, 0.f, 0.f};

	if (fLength > 0)
	{
		vForce += -(vSpringVector / fLength) * (fLength - m_fSpringLength) * m_fSpringConstant;
	}

	vForce += -(m_pMass1->Get_Vel() - m_pMass2->Get_Vel()) * m_fFrictionConstant;

	m_pMass1->ApplyForce(vForce);
	m_pMass2->ApplyForce(-vForce);
}

void CSpring::Free()
{
	Safe_Release(m_pMass1);
	Safe_Release(m_pMass2);
}
