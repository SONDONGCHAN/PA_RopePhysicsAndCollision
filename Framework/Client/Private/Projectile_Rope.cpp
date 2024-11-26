#include "stdafx.h"
#include "Projectile_Rope.h"
#include "GameInstance.h"
#include "Player.h"

CProjectile_Rope::CProjectile_Rope(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CProjectile(pDevice, pContext)
{
}

CProjectile_Rope::CProjectile_Rope(const CProjectile& rhs)
	: CProjectile(rhs)
{
}

HRESULT CProjectile_Rope::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CProjectile_Rope::Initialize(void* pArg)
{
	PROJECTILE_DESC* pProjectileDesc = (PROJECTILE_DESC*)pArg;
	pProjectileDesc->fSpeedPerSec = 70.f;
	m_pOwnerObject = pProjectileDesc->pOwnerObject;
	
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	m_isEnable = false;

	m_vColor = { 0.f, 0.f, 0.f, 1.f };
	m_fThickness = 0.05f;

	return S_OK;
}

void CProjectile_Rope::Priority_Tick(_float fTimeDelta)
{
	m_fCurrentLifeTime -= fTimeDelta;
	if (m_fCurrentLifeTime <= 0.f)
		Disable_Projectile();
}

void CProjectile_Rope::Tick(_float fTimeDelta)
{
	m_pTransformCom->Go_Straight(fTimeDelta);
	m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());
}

void CProjectile_Rope::Late_Tick(_float fTimeDelta)
{
	m_pGameInstance->Add_DebugComponent(m_pColliderCom);

	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this)))
		return;
}

HRESULT CProjectile_Rope::Render()
{
	_float4x4	ViewMatrix = m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTRANSFORMSTATE::D3DTS_VIEW);
	_float4x4	ProjMatrix = m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTRANSFORMSTATE::D3DTS_PROJ);

	_float4x4	WorldMatrix1;
	_float4x4	WorldMatrix2;

	XMStoreFloat4x4(&WorldMatrix1, XMMatrixTranslationFromVector(m_pTransformCom->Get_State(CTransform::STATE_POSITION)));

	_vector vOwnerPos = dynamic_cast<CPlayer*>(m_pOwnerObject)->Get_TranformCom()->Get_State(CTransform::STATE_POSITION);
	vOwnerPos += _vector{ 0.f, 1.f, 0.f };
	XMStoreFloat4x4(&WorldMatrix2, XMMatrixTranslationFromVector(vOwnerPos));

	m_pShaderCom->Bind_Matrix("g_WorldMatrix1", &WorldMatrix1);
	m_pShaderCom->Bind_Matrix("g_WorldMatrix2", &WorldMatrix2);
	m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix);
	m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMatrix);

	m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof(_float4));

	m_pShaderCom->Bind_RawValue("g_fThickness", &m_fThickness, sizeof(_float));

	m_pShaderCom->Begin(0);

	m_pVIBufferCom->Bind_Buffers();

	m_pVIBufferCom->Render();
	return S_OK;
}

void CProjectile_Rope::Enable_Projectile(_vector _vMyStartPos, _vector _vTargetDir)
{
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, _vTargetDir);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _vMyStartPos + _vector{0.f, 1.f, 0.f, 0.f});
	m_fCurrentLifeTime = m_fMaxLifeTime;
	m_isEnable = true;
}

void CProjectile_Rope::Disable_Projectile()
{
	m_isEnable = false;
}

void CProjectile_Rope::Event_CollisionEnter(ColData* _ColData, ColData* _MyColData)
{
	if (_ColData->eMyColType == COL_STATIC_OBJECT)
	{
		Disable_Projectile();

		vector<vector<CBounding_Capsule::ContactPoint>>& vecPoints = dynamic_cast<CBounding_Capsule*>(m_pColliderCom->Get_Bounding())->Get_Points()[0];
		CBounding_Capsule::ContactPoint ContactPoint;
		for (int i = 0; i < vecPoints.size(); ++i)
		{
			if (vecPoints[i][0].isCol)
			{
				ContactPoint = vecPoints[i][0];
				if (ContactPoint.ContactType == CBounding_Capsule::ContactPointType::VertexPlane)
					break;
			}
			if ((vecPoints[i].size() > 1) && (vecPoints[i][1].isCol))
			{
				ContactPoint = vecPoints[i][1];
				if (ContactPoint.ContactType == CBounding_Capsule::ContactPointType::VertexPlane)
					break;
		
			}
		}

		_vector vOwnerPos = dynamic_cast<CPlayer*>(m_pOwnerObject)->Get_TranformCom()->Get_State(CTransform::STATE_POSITION);
		_vector vMyPos = ContactPoint.ShapeContactPoints[1];
		vMyPos.m128_f32[3] = 1.f;
		//_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_vector vDir = (vOwnerPos + _vector{ 0.f, 1.f, 0.f }) - vMyPos;

		//�����
		//_vector vOwnerPos = _vector{ 6.f, 2.f, 2.f };
		//_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		//_vector vDir = vOwnerPos - vMyPos;

		dynamic_cast<CPlayer*>(m_pOwnerObject)->Start_Stiff_Simulating(vDir, vMyPos, 0.5f, 10.f);
	}
}

void CProjectile_Rope::Event_CollisionStay(ColData* _ColData, ColData* _MyColData)
{

}

void CProjectile_Rope::Event_CollisionExit(ColData* _ColData, ColData* _MyColData)
{
	int i = 0;
}

HRESULT CProjectile_Rope::Add_Component()
{
	CCollider::ColliderInitData	ColliderDesc{};

	CGameObject::ColData ColData{};
	ColData.pGameObject = this;
	ColData.eMyColType = COL_PROJECTILE;
	ColData.iTargetColType = COL_STATIC_OBJECT;
	ColData.isDead = false;

	//CCollider::SPHERE_DESC	BoundingDesc{};
	//BoundingDesc.fRadius = 0.1f;
	//BoundingDesc.vCenter = _float3(0.f, 0.f, 0.f);

	//ColliderDesc.ColData = ColData;
	//ColliderDesc.ColliderDesc = BoundingDesc;

	//if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Sphere"),
	//	TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
	//	return E_FAIL;

	CCollider::CAPSULE_DESC	BoundingDesc{};
	BoundingDesc.fHeight = 0.01f;
	BoundingDesc.fRadius = 0.6f;
	BoundingDesc.vCenter = { 0.f, 0.f, 0.f };
	BoundingDesc.vDir = { 0.f, 1.f, 0.f };
	
	ColliderDesc.ColData = ColData;
	ColliderDesc.ColliderDesc = BoundingDesc;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Capsule"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;

	CGameInstance::GetInstance()->Add_Collider(m_pColliderCom);


	m_pVIBufferCom = dynamic_cast<CVIBuffer_Point_Double*>(m_pGameInstance->Clone_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Point_Double")));

	m_pShaderCom = dynamic_cast<CShader*>(m_pGameInstance->Clone_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxPoint_Rope")));


	//CCollider::OBB_DESC	BoundingDesc{};
	//BoundingDesc.vExtents = { 0.2f, 0.8f, 0.2f };
	//BoundingDesc.vCenter = {0.f, BoundingDesc.vExtents.y, 0.f};
	//BoundingDesc.vRadians = {0.f, 0.f, 0.f };

	//ColliderDesc.ColData = ColData;
	//ColliderDesc.ColliderDesc = BoundingDesc;

	//if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"),
	//	TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
	//	return E_FAIL;

	return S_OK;
}

CProjectile_Rope* CProjectile_Rope::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CProjectile_Rope* pInstance = new CProjectile_Rope(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CProjectile_Rope");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CProjectile_Rope::Clone(void* pArg)
{
	CProjectile_Rope* pInstance = new CProjectile_Rope(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CProjectile_Rope");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CProjectile_Rope::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
}
