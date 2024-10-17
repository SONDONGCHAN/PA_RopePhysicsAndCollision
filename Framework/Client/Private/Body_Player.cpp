#include "stdafx.h"
#include "..\Public\Body_Player.h"

#include "GameInstance.h"
#include "Player.h"

CBody_Player::CBody_Player(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CPartObject(pDevice, pContext)
{
}

CBody_Player::CBody_Player(const CBody_Player & rhs)
	: CPartObject(rhs)
{
}

HRESULT CBody_Player::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBody_Player::Initialize(void * pArg)
{	
	BODY_PLAYER_DESC*		pBodyPlayerDesc = (BODY_PLAYER_DESC*)pArg;

	m_pPlayerState = pBodyPlayerDesc->pPlayerState;
		 
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;	

	if (FAILED(Add_Component()))
		return E_FAIL;

	//m_pModelCom->Set_Animation(0);
	m_pSkeletalModelCom->Set_Animation(0);
	
	return S_OK;
}

void CBody_Player::Priority_Tick(_float fTimeDelta)
{
	
}

void CBody_Player::Tick(_float fTimeDelta)
{
	//if (*m_pPlayerState == STATE_IDLE)
	//{
	//	m_pModelCom->Set_Animation(3);
	//	m_pSkeletalModelCom->Set_Animation(3);
	//}
	//if (*m_pPlayerState == STATE_RUN)
	//{
	//	m_pModelCom->Set_Animation(4);
	//	m_pSkeletalModelCom->Set_Animation(4);
	//}
	
	//m_pModelCom->Set_Animation(*m_pPlayerState);
	//m_pModelCom->Play_Animation(fTimeDelta, true);	

	m_pSkeletalModelCom->Set_Animation(*m_pPlayerState);
	m_pSkeletalModelCom->Play_Animation(fTimeDelta, true);
	
}

void CBody_Player::Late_Tick(_float fTimeDelta)
{
	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * m_pParentTransform->Get_WorldMatrix());

	m_pColliderCom->Tick(XMLoadFloat4x4(&m_WorldMatrix));

	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this)))
		return;
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW, this)))
		return;

#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif
}

HRESULT CBody_Player::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	//AnimMesh
	
	//_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	//for (size_t i = 0; i < iNumMeshes; i++)
	//{
	//	if (FAILED(m_pModelCom->Bind_Material_ShaderResource(m_pShaderCom, i, aiTextureType::aiTextureType_DIFFUSE, "g_DiffuseTexture")))
	//		return E_FAIL;

	//	if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
	//		return E_FAIL;

	//	if (FAILED(m_pShaderCom->Begin(0)))
	//		return E_FAIL;		

	//	if (FAILED(m_pModelCom->Render(i)))
	//		return E_FAIL;
	//}

	// Skeletal

	if (FAILED(m_pSkeletalModelCom->Update_BoneMatrices(m_pSkeletalShaderCom, "g_BoneMatrices")))
		return E_FAIL;

	if (FAILED(m_pSkeletalShaderCom->Begin(0)))
		return E_FAIL;		

	if (FAILED(m_pSkeletalModelCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBody_Player::Render_Shadow()
{
	//if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
	//	return E_FAIL;

	//_float4x4		ViewMatrix, ProjMatrix;

	//XMStoreFloat4x4(&ViewMatrix, XMMatrixLookAtLH(XMVectorSet(20.f, 20.f, -20.f, 1.f), XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 1.f, 0.f, 0.f)));
	//XMStoreFloat4x4(&ProjMatrix, XMMatrixPerspectiveFovLH(XMConvertToRadians(90.0f), (_float)g_iWinSizeX / g_iWinSizeY, 0.1f, 2000.f));

	//if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
	//	return E_FAIL;

	//if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))
	//	return E_FAIL;


	//_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	//for (size_t i = 0; i < iNumMeshes; i++)
	//{
	//	if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
	//		return E_FAIL;

	//	if (FAILED(m_pShaderCom->Begin(1)))
	//		return E_FAIL;

	//	if (FAILED(m_pModelCom->Render(i)))
	//		return E_FAIL;
	//}

	return S_OK;
}

HRESULT CBody_Player::Add_Component()
{
	/* Com_Shader */
	//if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
	//	TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	//	return E_FAIL;	
	
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimSkeletal"),
		TEXT("Com_Shader_Skel"), reinterpret_cast<CComponent**>(&m_pSkeletalShaderCom))))
		return E_FAIL;
	
	/* Com_Model */
	//if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Fiona"),
	//	TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
	//	return E_FAIL;	
	
	//if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Fiona_Skeletal"),
	//	TEXT("Com_Model_Skel"), reinterpret_cast<CComponent**>(&m_pSkeletalModelCom))))
	//	return E_FAIL;
	
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Human_Skeletal"),
		TEXT("Com_Model_Skel"), reinterpret_cast<CComponent**>(&m_pSkeletalModelCom))))
		return E_FAIL;

	
	/* Com_Collider */
	CBounding_Sphere::SPHERE_DESC		BoundingDesc{};

	BoundingDesc.fRadius = 0.5f;
	BoundingDesc.vCenter = _float3(0.f, BoundingDesc.fRadius, 0.f);	

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &BoundingDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBody_Player::Bind_ShaderResources()
{
	//if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
	//	return E_FAIL;

	//if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
	//	return E_FAIL;

	//if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
	//	return E_FAIL;	

	if (FAILED(m_pSkeletalShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;

	if (FAILED(m_pSkeletalShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pSkeletalShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

CBody_Player * CBody_Player::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBody_Player*		pInstance = new CBody_Player(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBody_Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CBody_Player::Clone(void * pArg)
{
	CBody_Player*		pInstance = new CBody_Player(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBody_Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBody_Player::Free()
{
	__super::Free();
		
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pSkeletalShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pSkeletalModelCom);
}
