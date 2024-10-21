#pragma once
#include "Simulation.h"
#include "Spring.h"

class CRope_Simulation : public CSimulation
{
public:
	CRope_Simulation(_int _iNum_Masses, _float _M);
	virtual ~CRope_Simulation();

private:
	vector<CSpring*> vecSprings;		// 스프링들

private:
	_vector m_Gravitation;				// 중력 가속도
	_vector m_RopeConnection_Pos;		// 첫 번째 Mass의 위치
	_vector m_RopeConnection_Vel;		// 첫 번째 Mass를 이동시키기 위한 변수

	_float	m_GroundRepulsionConstant;	// 지면 튕김 상수
	_float	m_GroundFrictionConstant;	// 지면 마찰 상수
	_float	m_GroundAbsorptionConstant;	// 지면 흡수 마찰 상수
	_float	m_GroundHeight;				// 지면의 높이

public:
	virtual void Free() override;
};

