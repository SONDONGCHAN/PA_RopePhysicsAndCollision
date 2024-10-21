#include "stdafx.h"
#include "..\Public\Weapon.h"

#include "GameInstance.h"

CWeapon::CWeapon(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CPartObject(pDevice, pContext)
{
}

CWeapon::CWeapon(const CWeapon & rhs)
	: CPartObject(rhs)
{
}

HRESULT CWeapon::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CWeapon::Initialize(void * pArg)
{		
	WEAPON_DESC*		pWeaponDesc = (WEAPON_DESC*)pArg;

	m_pSocketMatrix = pWeaponDesc->pSocketMatrix;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;	

	if (FAILED(Add_Component()))
		return E_FAIL;	

	m_pTransformCom->Set_Scaling(0.1f, 0.1f, 0.1f);
	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(90.0f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.5f, 0.f, 0.f, 1.f));
	
	return S_OK;
}

void CWeapon::Priority_Tick(_float fTimeDelta)
{
	
}

void CWeapon::Tick(_float fTimeDelta)
{
	
}

void CWeapon::Late_Tick(_float fTimeDelta)
{
	_matrix	BoneMatrix = XMLoadFloat4x4(m_pSocketMatrix);

	for (size_t i = 0; i < 3; i++)
	{
		BoneMatrix.r[i] = XMVector3Normalize(BoneMatrix.r[i]);
	}
	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * BoneMatrix * m_pParentTransform->Get_WorldMatrix());

	m_pColliderCom->Tick(XMLoadFloat4x4(&m_WorldMatrix));

	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this)))
		return;

#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif
}

HRESULT CWeapon::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;	

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		//if (FAILED(m_pModelCom->Bind_Material_ShaderResource(m_pShaderCom, i, aiTextureType::aiTextureType_DIFFUSE, "g_DiffuseTexture")))
		//	return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;		

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}



	return S_OK;
}

HRESULT CWeapon::Add_Component()
{
	/* Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;	

	/* Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_ForkLift"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;	


	/* Com_Collider */
	//CBounding_OBB::OBB_DESC		BoundingDesc{};

	//BoundingDesc.vExtents = _float3(1.0f, 1.3f, 2.f);
	//BoundingDesc.vCenter = _float3(0.f, BoundingDesc.vExtents.y, 0.f);
	//BoundingDesc.vRadians = _float3(0.f, 0.f, 0.f);

	//if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"),
	//	TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &BoundingDesc)))
	//	return E_FAIL;


	

	return S_OK;
}

HRESULT CWeapon::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;	

	return S_OK;
}

CWeapon * CWeapon::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CWeapon*		pInstance = new CWeapon(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CWeapon");
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CWeapon::Clone(void * pArg)
{
	CWeapon*		pInstance = new CWeapon(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CWeapon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWeapon::Free()
{
	__super::Free();	

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
