#pragma once
#include "Base.h"
#include "Client_Defines.h"

BEGIN(Client)

class CMass final : public CBase
{
public:
	CMass(_float _M);
	virtual ~CMass();

public:
	void ApplyForce(_vector force);
	void InitForce();
	void Simulate(_float fTimeDelta);

public:
	_float	Get_M() { return m_fM; }
	_vector Get_Pos() { return m_vPos; }
	_vector Get_Vel() { return m_vVel; }

	void Set_Pos(_vector _vPos) { m_vPos = _vPos; }
	void Set_Vel(_vector _vVel) { m_vVel = _vVel; }
	void Set_Force(_vector _vForce) { m_vForce = _vForce; }


private:
	_float	m_fM		{ 0.f };			// 질량 값.
	_vector	m_vPos	{ 0.f, 0.f, 0.f };  // 공간 내 위치.
	_vector	m_vVel	{ 0.f, 0.f, 0.f };  // 속도.
	_vector	m_vForce { 0.f, 0.f, 0.f };  // 특정 시점에 적용된 힘.

public:
	virtual void Free() override;
};

END