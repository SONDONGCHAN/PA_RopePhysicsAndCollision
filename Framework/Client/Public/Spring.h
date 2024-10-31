#pragma once
#include "Base.h"
#include "Client_Defines.h"
#include "Rope_Simulation.h"

BEGIN(Engine)
class CShader;
class CVIBuffer_Point_Double;
END

BEGIN(Client)

class CSpring : public CBase
{
public:
	CSpring(class CMass* _pMass1, CMass* _pMass2, _float _fSpringConstant, _float _fSpringLength, _float _fFrictionConstant);
	virtual ~CSpring();

public:
	void	Solve(CRope_Simulation::SimulateMode _eSimulateMode);
	void	Render();

public:
	void	Set_SpringLength(_float _fSpringLength);
	void	MainTain_Length();


private:
	void	Add_Component();

private:
	CShader*				m_pShaderCom = { nullptr };
	CVIBuffer_Point_Double* m_pVIBufferCom = { nullptr };

private:
	CGameInstance*	m_pGameInstance { nullptr };

private:
	CMass* m_pMass1{ nullptr };
	CMass* m_pMass2{ nullptr };

	_float m_fSpringLength;

	_float m_fSpringConstant;                     
	_float m_fFrictionConstant;

private:
	_float	m_fCurrentLength;
	_vector	m_vCurrentDir;
	_float4 m_vColor{};
	_float	m_fThickness{};


public:
	virtual void Free() override;
};

END