#pragma once
#include "Base.h"
#include "Client_Defines.h"


class CSpring : public CBase
{
public:
	CSpring(class CMass* _pMass1, CMass* _pMass2, _float _fSpringConstant, _float _fSpringLength, _float _fFrictionConstant);
	virtual ~CSpring();

public:
	void	Solve();

private:
	CMass* m_pMass1{ nullptr };
	CMass* m_pMass2{ nullptr };

	_float m_fSpringConstant;             
	_float m_fSpringLength;               
	_float m_fFrictionConstant;

public:
	virtual void Free() override;
};

