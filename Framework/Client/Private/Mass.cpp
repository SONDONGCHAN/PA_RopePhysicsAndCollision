#include "stdafx.h"
#include "Mass.h"

CMass::CMass(_float _M)
{
	m_fM = _M;
}

CMass::~CMass()
{
}

void CMass::ApplyForce(_vector force)
{
	m_vForce += force;
}

void CMass::InitForce()
{
	m_vForce = { 0.f, 0.f, 0.f };
}

void CMass::Simulate(_float fTimeDelta)
{
	m_vVel += (m_vForce / m_fM) * fTimeDelta;	// �ӵ��� ��ȭ.
	m_vPos += m_vVel * fTimeDelta;			// ��ġ�� ��ȭ.
}

void CMass::Free()
{

}
