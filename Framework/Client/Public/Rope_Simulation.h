#pragma once
#include "Simulation.h"

BEGIN(Client)

class CRope_Simulation : public CSimulation
{

public:
	enum class SimulateMode
	{
		MODE_STIFF,
		MODE_SOFT,
		MODE_END
	};

	struct Rope_Simulation_Data
	{
		_vector			vDir;
		_vector			vPos;
		_float			fM;
		_float			fLastM;
		_vector			vStartVel{0.f, 0.f, 0.f, 0.f};
	};

public:
	CRope_Simulation(
		_float _fSpringConstant,
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

	virtual void	Start_Simulating(void* _Datas) override;

public:
	void Switch_Soft_Simulating(_vector _vVel);
	void End_Simulating();

	void Make_Spring(_vector _vDir);

	virtual void Render() override;



public:
	void	Set_RopeConnection_Vel(_vector _vRopeConnection_Vel) {
		m_vRopeConnection_Vel = _vRopeConnection_Vel ; }

	void	Set_RopeConnection_Pos(_vector _vRopeConnection_Pos) {
		m_vRopeConnection_Pos = _vRopeConnection_Pos;
	}
	void	Set_Accelerating(_bool _isAccelerating, _float _fAccelerate_Force = 200.f);

private: 
	void	Clear_Springs();
	void	Accelerator(class CMass* _pMass);
	void	Set_SpringLength();

private:
	vector<class CSpring*> vecSprings;		// 스프링들

private:
	_vector m_vRopeConnection_Pos{ };		// 첫 번째 Mass의 위치
	_vector m_vRopeConnection_Vel{ 0.f, 0.f, 0.f };		// 첫 번째 Mass를 이동시키기 위한 변수

private:
	_float			m_fSpringLength{ 0.2f };		// 스프링 길이
	const _float	m_fMaxSpringLength{ 0.2f };		// 스프링 길이 최대
	_float			m_fRatio{ 1.f };
	const _float	m_fMinRatio{ 0.5f };
	_float			m_fCurTime{ 0.f };
	const _float	m_fDurTime{ 0.6f };

	const _float	m_fEpsilon{ 0.001f };

private:
	_float	m_fHoldingTime{ 30.f };
	_float	m_fHoldingCurrentTime{ 0.f };
	_bool	m_isHolding{ false };

	_float	m_fSpringConstant;				// 스프링 계수
	_float  m_fSpringFrictionConstant;		// 스프링 마찰 계수

	_vector m_vGravitation;					// 중력 가속도
	_float	m_fAirFrictionConstant;			// 공기 마찰 계수
	_float	m_fGroundRepulsionConstant;		// 지면 튕김 계수
	_float	m_fGroundFrictionConstant;		// 지면 마찰 계수
	_float	m_fGroundAbsorptionConstant;	// 지면 흡수 마찰 계수
	_float	m_fGroundHeight;				// 지면의 높이

private:
	_bool	m_isAccelerating{ false };		// 가속 중 여부
	_float	m_fAccelerate_Force { 1.f };	// 가속 힘
	_vector m_vbaseDir{ 0.f, 1.f, 0.f };	// 가속 기준 벡터

private:
	SimulateMode m_eSimulateMode{ SimulateMode:: MODE_STIFF };

public:
	virtual void Free() override;
};

END