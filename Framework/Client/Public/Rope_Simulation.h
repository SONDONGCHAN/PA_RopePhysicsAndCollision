#pragma once
#include "Simulation.h"
#include "Spring.h"

BEGIN(Client)

class CRope_Simulation : public CSimulation
{
public:
	CRope_Simulation(
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
		);
	virtual ~CRope_Simulation();

public:
	virtual void Solve() override;
	virtual void Simulate(_float fTimeDelta) override;
	void Render() ;


public:
	void	Set_RopeConnection_Vel(_vector _vRopeConnection_Vel) {
		m_vRopeConnection_Vel = _vRopeConnection_Vel ; }

private:
	vector<CSpring*> vecSprings;		// ��������

private:
	_vector m_vRopeConnection_Pos{ 0.f, 0.f, 0.f };		// ù ��° Mass�� ��ġ
	_vector m_vRopeConnection_Vel{ 0.f, 0.f, 0.f };		// ù ��° Mass�� �̵���Ű�� ���� ����

private:
	_vector m_vGravitation;				// �߷� ���ӵ�
	_float	m_fAirFrictionConstant;		// ���� ���� ���
	_float	m_fGroundRepulsionConstant;	// ���� ƨ�� ���
	_float	m_fGroundFrictionConstant;	// ���� ���� ���
	_float	m_fGroundAbsorptionConstant;	// ���� ��� ���� ���
	_float	m_fGroundHeight;				// ������ ����

public:
	virtual void Free() override;
};

END