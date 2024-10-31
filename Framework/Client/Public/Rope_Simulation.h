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

	void Start_Soft_Simulating(_vector _vDir, _vector _vPos, _float _fM, _float _fLastM);
	void Switch_Soft_Simulating(_vector _vVel);
	void Start_Stiff_Simulating(_vector _vDir, _vector _vPos, _float _fM, _float _fLastM, _vector _vStartVel);
	void End_Simulating();

	void Make_Spring(_vector _vDir);

	void Render() ;



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
	vector<class CSpring*> vecSprings;		// ��������

private:
	_vector m_vRopeConnection_Pos{ };		// ù ��° Mass�� ��ġ
	_vector m_vRopeConnection_Vel{ 0.f, 0.f, 0.f };		// ù ��° Mass�� �̵���Ű�� ���� ����

private:
	_float			m_fSpringLength{ 0.2f };		// ������ ����
	const _float	m_fMaxSpringLength{ 0.2f };		// ������ ���� �ִ�
	_float			m_fRatio{ 1.f };
	const _float	m_fMinRatio{ 0.5f };
	_float			m_fCurTime{ 0.f };
	const _float	m_fDurTime{ 0.6f };

	const _float	m_fEpsilon{ 0.001f };

	_float	m_fSpringConstant;				// ������ ���
	_float  m_fSpringFrictionConstant;		// ������ ���� ���

	_vector m_vGravitation;					// �߷� ���ӵ�
	_float	m_fAirFrictionConstant;			// ���� ���� ���
	_float	m_fGroundRepulsionConstant;		// ���� ƨ�� ���
	_float	m_fGroundFrictionConstant;		// ���� ���� ���
	_float	m_fGroundAbsorptionConstant;	// ���� ��� ���� ���
	_float	m_fGroundHeight;				// ������ ����

private:
	_bool	m_isAccelerating{ false };		// ���� �� ����
	_float	m_fAccelerate_Force { 1.f };	// ���� ��
	_vector m_vbaseDir{ 0.f, 1.f, 0.f };	// ���� ���� ����

private:
	SimulateMode m_eSimulateMode{ SimulateMode:: MODE_STIFF };

public:
	virtual void Free() override;
};

END