#include "stdafx.h"
#include "Mass.h"

CMass::CMass(_float _M)
{
	m_M = _M;
}

CMass::~CMass()
{
}

void CMass::ApplyForce(_vector force)
{
	m_Force += force;
}

void CMass::InitForce()
{
	m_Force = { 0.f, 0.f, 0.f };
}

void CMass::Simulate(_float _dt)
{
	m_Vel += (m_Force / m_M) * _dt;	// 속도의 변화.
	m_Pos += m_Vel * _dt;			// 위치의 변화.
}

void CMass::Free()
{

}
