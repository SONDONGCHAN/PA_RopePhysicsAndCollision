#include "stdafx.h"
#include "..\Public\Monster.h"

#include "GameInstance.h"
#include "Player.h"

CMonster::CMonster(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CLandObject(pDevice, pContext)
{
}

CMonster::CMonster(const CMonster & rhs)
	: CLandObject(rhs)
{
}

HRESULT CMonster::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMonster::Initialize(void * pArg)
{	
	LANDOBJ_DESC*	pGameObjectDesc = (LANDOBJ_DESC*)pArg;

	pGameObjectDesc->fSpeedPerSec = 10.f;
	pGameObjectDesc->fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(rand() % 10, 3.f, rand() % 10, 1.f));
	m_pModelCom->Set_Animation(0);
	
	return S_OK;
}

void CMonster::Priority_Tick(_float fTimeDelta)
{
	
}

void CMonster::Tick(_float fTimeDelta)
{
	m_pModelCom->Play_Animation(fTimeDelta, true);
	

	SetUp_OnTerrain(m_pTransformCom);	

	for (size_t i = 0; i < TYPE_END; i++)
		m_pColliderCom[i]->Tick(m_pTransformCom->Get_WorldMatrix());	
}

void CMonster::Late_Tick(_float fTimeDelta)
{
	Collision_ToPlayer();

	if (true == m_pGameInstance->isIn_Frustum_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 2.f))
	{
		if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this)))
			return;

#ifdef _DEBUG
		for (size_t i = 0; i < TYPE_END; i++)
			m_pGameInstance->Add_DebugComponent(m_pColliderCom[i]);
#endif
	}



}

HRESULT CMonster::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;	

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material_ShaderResource(m_pShaderCom, i, aiTextureType::aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;		

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}



	return S_OK;
}

HRESULT CMonster::Add_Component()
{
	/* Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;	

	/* Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Fiona"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;	

	return S_OK;
}

HRESULT CMonster::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;		

	return S_OK;
}

void CMonster::Collision_ToPlayer()
{
	/*
	CPlayer*	pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Player")));
	CCollider*		pPlayerCollider = dynamic_cast<CCollider*>(pPlayer->Get_PartObjectComponent(TEXT("Part_Body"), TEXT("Com_Collider")));
	*/

	CPlayer*	pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Player")));
	CCollider*		pPlayerCollider = dynamic_cast<CCollider*>(pPlayer->Get_PartObjectComponent(TEXT("Part_Weapon"), TEXT("Com_Collider")));

	/*CCollider*		pPlayerCollider = dynamic_cast<CCollider*>(m_pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_Collider")));
	if (nullptr == pPlayerCollider)
		return;*/

	
	m_pColliderCom[TYPE_OBB]->Intersect(pPlayerCollider);
	
}

CMonster * CMonster::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMonster*		pInstance = new CMonster(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMonster");
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CMonster::Clone(void * pArg)
{
	CMonster*		pInstance = new CMonster(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMonster");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMonster::Free()
{
	__super::Free();
	
	for (size_t i = 0; i < TYPE_END; i++)
		Safe_Release(m_pColliderCom[i]);
	
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
