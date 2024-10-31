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


	void Set_M(_float _fM) { m_fM = _fM; }
	void Set_Pos(_vector _vPos) { m_vPos = _vPos; }
	void Set_Vel(_vector _vVel) { m_vVel = _vVel; }
	void Set_Force(_vector _vForce) { m_vForce = _vForce; }

	void Add_Pos(_vector _vDir) { m_vPos += _vDir;}

	void	Set_Root() { m_isRoot = true; }
	_bool	Get_IsRoot() { return m_isRoot; }

private:
	_float	m_fM		{ 0.f };			// ���� ��.
	_vector	m_vPos	{ 0.f, 0.f, 0.f };  // ���� �� ��ġ.
	_vector	m_vVel	{ 0.f, 0.f, 0.f };  // �ӵ�.
	_vector	m_vForce { 0.f, 0.f, 0.f };  // Ư�� ������ ����� ��.

	_bool	m_isRoot{ false };

public:
	virtual void Free() override;
};

END