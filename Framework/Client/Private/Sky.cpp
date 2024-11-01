#include "stdafx.h"
#include "..\Public\Sky.h"

#include "GameInstance.h"

CSky::CSky(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CSky::CSky(const CSky & rhs)
	: CGameObject(rhs)
{
}

HRESULT CSky::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSky::Initialize(void * pArg)
{
	if (nullptr == pArg)
	{
		CGameObject::GAMEOBJECT_DESC			GameObjectDesc = {};

		GameObjectDesc.iGameObjectData = 10;
		GameObjectDesc.fSpeedPerSec = 10.0f;
		GameObjectDesc.fRotationPerSec = XMConvertToRadians(60.0f);

		if (FAILED(__super::Initialize(&GameObjectDesc)))
			return E_FAIL;	
	}	

	if (FAILED(Add_Component()))
		return E_FAIL;

	return S_OK;
}

void CSky::Priority_Tick(_float fTimeDelta)
{

}

void CSky::Tick(_float fTimeDelta)
{

}

void CSky::Late_Tick(_float fTimeDelta)
{

	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_PRIORITY, this)))
		return;
}

HRESULT CSky::Render()
{	
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_pGameInstance->Get_CamPosition()));


	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
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

HRESULT CSky::Add_Component()
{
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Sky"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;	

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxCube"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;	

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Cube"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;	

	return S_OK;
}

CSky * CSky::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CSky*		pInstance = new CSky(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSky");
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CSky::Clone(void * pArg)
{
	CSky*		pInstance = new CSky(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSky");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSky::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
