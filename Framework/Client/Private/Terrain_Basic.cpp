#include "stdafx.h"
#include "Terrain_Basic.h"
#include "GameInstance.h"

CTerrain_Basic::CTerrain_Basic(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CTerrain_Basic::CTerrain_Basic(const CTerrain_Basic& rhs)
	: CGameObject(rhs)
{
}

HRESULT CTerrain_Basic::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTerrain_Basic::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	return S_OK;
}

void CTerrain_Basic::Priority_Tick(_float fTimeDelta)
{
}

void CTerrain_Basic::Tick(_float fTimeDelta)
{
	m_pNavigationCom->Tick(m_pTransformCom->Get_WorldMatrix());
}

void CTerrain_Basic::Late_Tick(_float fTimeDelta)
{
	//m_pVIBufferCom->Culling(m_pTransformCom);

	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this)))
		return;

#ifdef _DEBUG
	//m_pGameInstance->Add_DebugComponent(m_pNavigationCom);
#endif
}

HRESULT CTerrain_Basic::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTerrain_Basic::Add_Component()
{
	/* Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Terrain"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom[TYPE_DIFFUSE]))))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxNorTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Terrain_Basic"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* Com_Navigatiom */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"),
		TEXT("Com_Navigatiom"), reinterpret_cast<CComponent**>(&m_pNavigationCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CTerrain_Basic::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pTextureCom[TYPE_DIFFUSE]->Bind_ShaderResources(m_pShaderCom, "g_DiffuseTexture")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	return S_OK;
}

CTerrain_Basic* CTerrain_Basic::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTerrain_Basic* pInstance = new CTerrain_Basic(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTerrain_Basic");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTerrain_Basic::Clone(void* pArg)
{
	CTerrain_Basic* pInstance = new CTerrain_Basic(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTerrain_Basic");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTerrain_Basic::Free()
{
	__super::Free();

	for (auto& pTexture : m_pTextureCom)
		Safe_Release(pTexture);

	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
