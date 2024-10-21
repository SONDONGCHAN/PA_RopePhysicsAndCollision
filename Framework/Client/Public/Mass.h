#pragma once
#include "Base.h"
#include "Client_Defines.h"

class CMass final : public CBase
{
public:
	CMass(_float _M);
	virtual ~CMass();

public:
	void ApplyForce(_vector force);
	void InitForce();
	void Simulate(_float _dt);

public:
	_vector Get_Pos() { return m_Pos; }
	_vector Get_Vel() { return m_Vel; }

private:
	_float	m_M		{ 0.f };			// ���� ��.
	_vector	m_Pos	{ 0.f, 0.f, 0.f };  // ���� �� ��ġ.
	_vector	m_Vel	{ 0.f, 0.f, 0.f };  // �ӵ�.
	_vector	m_Force { 0.f, 0.f, 0.f };  // Ư�� ������ ����� ��.

public:
	virtual void Free() override;
};

