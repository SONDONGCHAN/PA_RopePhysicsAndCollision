#include "stdafx.h"
#include "Spring.h"
#include "Mass.h"
#include "GameInstance.h"

CSpring::CSpring(CMass* _pMass1, CMass* _pMass2, _float _fSpringConstant, _float _fSpringLength, _float _fFrictionConstant)
{
	m_pMass1 = _pMass1;
	m_pMass2 = _pMass2;

	m_fSpringConstant = _fSpringConstant;
	m_fSpringLength = _fSpringLength;
	m_fFrictionConstant = _fFrictionConstant;

	m_pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(m_pGameInstance);

	m_vColor = { 0.f, 0.f, 0.f, 1.f };
	m_fThickness = 0.05f;

	Add_Component();
}

CSpring::~CSpring()
{
}

void CSpring::Solve()
{
	_vector vSpringVector = m_pMass1->Get_Pos() - m_pMass2->Get_Pos();
	
	_float fLength = XMVectorGetX(XMVector3Length(vSpringVector));

	_vector vForce {0.f, 0.f, 0.f};

	if (fLength > 0)
	{
		vForce += -(vSpringVector / fLength) * (fLength - m_fSpringLength) * m_fSpringConstant;
	}

	vForce += -(m_pMass1->Get_Vel() - m_pMass2->Get_Vel()) * m_fFrictionConstant;

	m_pMass1->ApplyForce(vForce);
	m_pMass2->ApplyForce(-vForce);
}

void CSpring::Render()
{
	_float4x4	ViewMatrix = m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTRANSFORMSTATE::D3DTS_VIEW);
	_float4x4	ProjMatrix = m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTRANSFORMSTATE::D3DTS_PROJ);

	_float4x4	WorldMatrix1;
	_float4x4	WorldMatrix2;
	
	XMStoreFloat4x4(&WorldMatrix1, XMMatrixTranslationFromVector(m_pMass1->Get_Pos()));
	XMStoreFloat4x4(&WorldMatrix2, XMMatrixTranslationFromVector(m_pMass2->Get_Pos()));

	m_pShaderCom->Bind_Matrix("g_WorldMatrix1", &WorldMatrix1);
	m_pShaderCom->Bind_Matrix("g_WorldMatrix2", &WorldMatrix2);
	m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix);
	m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMatrix);

	m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof(_float4));

	m_pShaderCom->Bind_RawValue("g_fThickness", &m_fThickness, sizeof(_float));

	m_pShaderCom->Begin(0);

	m_pVIBufferCom->Bind_Buffers();

	m_pVIBufferCom->Render();
}

void CSpring::Set_SpringLength(_float _fSpringLength)
{
	_vector vSpringVector = m_pMass1->Get_Pos() - m_pMass2->Get_Pos();
	_float fLength = XMVectorGetX(XMVector3Length(vSpringVector));
	_float fStretchRatio = fLength / m_fSpringLength;

	if (fStretchRatio <= 1.f)
		return;

	m_pMass2->Add_Pos ((vSpringVector / fLength)* fStretchRatio * (m_fSpringLength - _fSpringLength));

	m_fSpringLength = _fSpringLength;
}

void CSpring::Add_Component()
{
	m_pVIBufferCom = dynamic_cast<CVIBuffer_Point_Double*>(m_pGameInstance->Clone_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Point_Double")));

	m_pShaderCom = dynamic_cast<CShader*>(m_pGameInstance->Clone_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxPoint_Rope")));
}

void CSpring::Free()
{
	Safe_Release(m_pGameInstance);

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);

	Safe_Release(m_pMass1);
	Safe_Release(m_pMass2);
}
