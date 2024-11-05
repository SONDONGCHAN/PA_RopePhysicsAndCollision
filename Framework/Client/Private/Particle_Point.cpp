#include "stdafx.h"
#include "..\Public\Particle_Point.h"

#include "GameInstance.h"

CParticle_Point::CParticle_Point(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CParticle_Point::CParticle_Point(const CParticle_Point & rhs)
	: CGameObject(rhs)
{
}

HRESULT CParticle_Point::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CParticle_Point::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECT_DESC			GameObjectDesc = {};

	GameObjectDesc.iGameObjectData = 10;
	GameObjectDesc.fSpeedPerSec = 10.0f;
	GameObjectDesc.fRotationPerSec = XMConvertToRadians(60.0f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;		

	if (FAILED(Add_Component()))
		return E_FAIL;	

	return S_OK;
}

void CParticle_Point::Priority_Tick(_float fTimeDelta)
{
	int a = 10;
}

void CParticle_Point::Tick(_float fTimeDelta)
{
	m_pVIBufferCom->Tick_Drop(fTimeDelta);
	// m_pVIBufferCom->Tick_Spread(fTimeDelta);
}

void CParticle_Point::Late_Tick(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this)))
		return;
}

HRESULT CParticle_Point::Render()
{	
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CParticle_Point::Add_Component()
{
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY , TEXT("Prototype_Component_Texture_Snow"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;	

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxPointInstance"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;	

	CVIBuffer_Instancing::INSTANCE_DESC			InstanceDesc{};
	InstanceDesc.vPivot = _float3(0.f, 9.f, 0.f);
	InstanceDesc.vCenter = _float3(0.0f, 50.f, 0.f);
	InstanceDesc.vRange = _float3(300.0f, 5.0f, 300.0f);
	InstanceDesc.vSize = _float2(0.2f, 0.7f);
	InstanceDesc.vSpeed = _float2(1.f, 3.f);
	InstanceDesc.vLifeTime = _float2(20.f, 50.f);
	InstanceDesc.isLoop = true;
	InstanceDesc.vColor = _float4(1.f, 1.f, 1.f, 1.f);

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Particle_Point"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), &InstanceDesc)))
		return E_FAIL;	

	return S_OK;
}

CParticle_Point * CParticle_Point::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CParticle_Point*		pInstance = new CParticle_Point(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CParticle_Point");
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CParticle_Point::Clone(void * pArg)
{
	CParticle_Point*		pInstance = new CParticle_Point(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CParticle_Point");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CParticle_Point::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
