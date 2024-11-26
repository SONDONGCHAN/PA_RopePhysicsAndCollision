#include "stdafx.h"
#include "Obstacle.h"
#include "GameInstance.h"

CObstacle::CObstacle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CObstacle::CObstacle(const CObstacle& rhs)
	: CGameObject(rhs)
{
}

HRESULT CObstacle::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CObstacle::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	OBSTACLE_DESC* pObstacleDesc = (OBSTACLE_DESC*)pArg;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&pObstacleDesc->vStartpos));
	m_eMyShape = pObstacleDesc->eShape;

	if (FAILED(Add_Component()))
		return E_FAIL;

	
	return S_OK;
}

void CObstacle::Priority_Tick(_float fTimeDelta)
{
}

void CObstacle::Tick(_float fTimeDelta)
{
	m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());

}

void CObstacle::Late_Tick(_float fTimeDelta)
{
	m_pGameInstance->Add_DebugComponent(m_pColliderCom);
}

HRESULT CObstacle::Render()
{
	return S_OK;
}

void CObstacle::Event_CollisionEnter(ColData* _ColData, ColData* _MyColData)
{
}

void CObstacle::Event_CollisionStay(ColData* _ColData, ColData* _MyColData)
{
}

void CObstacle::Event_CollisionExit(ColData* _ColData, ColData* _MyColData)
{
}

HRESULT CObstacle::Add_Component()
{
	CCollider::ColliderInitData	ColliderDesc{};

	CGameObject::ColData ColData{};
	ColData.pGameObject = this;
	ColData.eMyColType = COL_STATIC_OBJECT;
	ColData.iTargetColType = COL_NONE;
	ColData.isDead = false;

	/*OBB*/
	//CCollider::OBB_DESC	BoundingDesc{};

	//BoundingDesc.vExtents = _float3{4.f, 2.f, 4.f};
	////BoundingDesc.vRadians = _float3(XMConvertToRadians(45.f), 0.f, XMConvertToRadians(45.f));;
	//BoundingDesc.vRadians = _float3(0.f, 0.f, 0.f);;
	//BoundingDesc.vCenter = _float3(0.f, BoundingDesc.vExtents.y, 0.f);
	//ColliderDesc.ColData = ColData;
	//ColliderDesc.OBBDesc = BoundingDesc;

	/*±¸*/
	//CCollider::SPHERE_DESC	BoundingDesc{};

	//BoundingDesc.fRadius = 4.f;
	//BoundingDesc.vCenter = _float3(0.f, BoundingDesc.fRadius, 0.f);
	//ColliderDesc.ColData = ColData;
	//ColliderDesc.SphereDesc = BoundingDesc;

	/*»ï°¢Çü*/
	vector<CCollider::TRIANGLE_DESC> BoundingDesc{};
	CCollider::TRIANGLE_DESC desc;
	vector<_float3> vecPoints{};
	vector<array<int, 3>> vecIndices{};

	switch (m_eMyShape)
	{
	case Client::OBSTACLE_SHAPE::SHAPE_BOX:
		vecPoints =
		{
			{0.f, 0.f, 0.f},
			{8.f, 0.f, 0.f},
			{8.f, 0.f, 8.f},
			{0.f, 0.f, 8.f},
			{0.f, 8.f, 0.f},
			{8.f, 8.f, 0.f},
			{8.f, 8.f, 8.f},
			{0.f, 8.f, 8.f}
		};
		vecIndices =
		{
			{0, 4, 5},
			{0, 5, 1},
			{2, 6, 7},
			{2, 7, 3},
			{1, 5, 6},
			{1, 6, 2},
			{3, 7, 4},
			{3, 4, 0},
			{4, 7, 6},
			{4, 6, 5},
			{3, 0, 1},
			{3, 1, 2}
		};

		for (int i = 0; i < 12; ++i)
		{
			desc.vVertex1 = vecPoints[vecIndices[i][0]];
			desc.vVertex2 = vecPoints[vecIndices[i][1]];
			desc.vVertex3 = vecPoints[vecIndices[i][2]];
			BoundingDesc.push_back(desc);
		}
		break;

	case Client::OBSTACLE_SHAPE::SHAPE_PYRAMID:
		break;

	case Client::OBSTACLE_SHAPE::SHAPE_PLANE:
		CCollider::TRIANGLE_DESC desc;
		desc.vVertex1 = { 0.f, 2.f, 0.f };
		desc.vVertex2 = { 0.f, 0.f, 10.f };
		desc.vVertex3 = { 10.f, 0.f, 0.f };
		BoundingDesc.push_back(desc);
		break;

	case Client::OBSTACLE_SHAPE::SHAPE_CUSTOM_1:
		break;

	case Client::OBSTACLE_SHAPE::SHAPE_END:
		break;

	default:
		break;
	}
	
	ColliderDesc.ColData = ColData;
	ColliderDesc.ColliderDesc = BoundingDesc;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Triangle"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;
	CGameInstance::GetInstance()->Add_Collider(m_pColliderCom);

	return S_OK;
}

CObstacle* CObstacle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CObstacle* pInstance = new CObstacle(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CObstacle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CObstacle::Clone(void* pArg)
{
	CObstacle* pInstance = new CObstacle(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CObstacle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CObstacle::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
}
