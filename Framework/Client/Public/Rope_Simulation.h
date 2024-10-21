#pragma once
#include "Simulation.h"
#include "Spring.h"

class CRope_Simulation : public CSimulation
{
public:
	CRope_Simulation(_int _iNum_Masses, _float _M);
	virtual ~CRope_Simulation();

private:
	vector<CSpring*> vecSprings;		// ��������

private:
	_vector m_Gravitation;				// �߷� ���ӵ�
	_vector m_RopeConnection_Pos;		// ù ��° Mass�� ��ġ
	_vector m_RopeConnection_Vel;		// ù ��° Mass�� �̵���Ű�� ���� ����

	_float	m_GroundRepulsionConstant;	// ���� ƨ�� ���
	_float	m_GroundFrictionConstant;	// ���� ���� ���
	_float	m_GroundAbsorptionConstant;	// ���� ��� ���� ���
	_float	m_GroundHeight;				// ������ ����

public:
	virtual void Free() override;
};

