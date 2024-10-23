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
	virtual void Init() override;
	virtual void Solve() override;
	virtual void Simulate(_float fTimeDelta) override;
	virtual void Operate(_float fTimeDelta) override;
	void Render() ;


public:
	void	Set_RopeConnection_Vel(_vector _vRopeConnection_Vel) {
		m_vRopeConnection_Vel = _vRopeConnection_Vel ; }

	void	Set_RopeConnection_Pos(_vector _vRopeConnection_Pos) {
		m_vRopeConnection_Pos = _vRopeConnection_Pos;
	}

private:
	vector<CSpring*> vecSprings;		// 스프링들

private:
	_vector m_vRopeConnection_Pos{ 1.f, 6.f, 4.f };		// 첫 번째 Mass의 위치
	_vector m_vRopeConnection_Vel{ 0.f, 0.f, 0.f };		// 첫 번째 Mass를 이동시키기 위한 변수

private:
	_vector m_vGravitation;				// 중력 가속도
	_float	m_fAirFrictionConstant;		// 공기 마찰 상수
	_float	m_fGroundRepulsionConstant;	// 지면 튕김 상수
	_float	m_fGroundFrictionConstant;	// 지면 마찰 상수
	_float	m_fGroundAbsorptionConstant;	// 지면 흡수 마찰 상수
	_float	m_fGroundHeight;				// 지면의 높이

public:
	virtual void Free() override;
};

END