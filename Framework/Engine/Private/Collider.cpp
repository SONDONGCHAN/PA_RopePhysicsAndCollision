#include "..\Public\Collider.h"
#include "GameInstance.h"

CCollider::CCollider(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)
{
}

CCollider::CCollider(const CCollider & rhs)
	: CComponent(rhs)
	, m_eType(rhs.m_eType)
	, m_pBounding(rhs.m_pBounding)
#ifdef _DEBUG
	, m_pBatch(rhs.m_pBatch)
	, m_pEffect(rhs.m_pEffect)
	, m_pInputLayout(rhs.m_pInputLayout)
#endif
{
	Safe_AddRef(m_pBounding);

#ifdef _DEBUG
	Safe_AddRef(m_pInputLayout);
#endif
}

HRESULT CCollider::Initialize_Prototype(TYPE eType)
{
	m_eType = eType;	

#ifdef _DEBUG
	m_pBatch = new PrimitiveBatch<VertexPositionColor>(m_pContext);
	m_pEffect = new BasicEffect(m_pDevice);

	m_pEffect->SetVertexColorEnabled(true);

	const void*	pShaderByteCode = { nullptr };
	size_t		iByteCodeLength = { 0 };

	m_pEffect->GetVertexShaderBytecode(&pShaderByteCode, &iByteCodeLength);

	if (FAILED(m_pDevice->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount, 
		pShaderByteCode, iByteCodeLength, &m_pInputLayout)))
		return E_FAIL;
#endif

	return S_OK;
}

HRESULT CCollider::Initialize(void * pArg)
{
	ColliderInitData* pInitData = ((ColliderInitData*)pArg);
	m_ColData = pInitData->ColData;
	m_ColData.pCol = this;

	switch (m_eType)
	{
	case TYPE_AABB:
		m_pBounding = CBounding_AABB::Create(m_pDevice, m_pContext, &pInitData->ColliderDesc);
		break;
	case TYPE_OBB:
		m_pBounding = CBounding_OBB::Create(m_pDevice, m_pContext, &pInitData->ColliderDesc);
		break;
	case TYPE_SPHERE:
		m_pBounding = CBounding_Sphere::Create(m_pDevice, m_pContext, &pInitData->ColliderDesc);
		break;
	case TYPE_CAPSULE:
		m_pBounding = CBounding_Capsule::Create(m_pDevice, m_pContext, &pInitData->ColliderDesc);
		break;
	case TYPE_TRIANGLE:
		m_pBounding = CBounding_Triangles::Create(m_pDevice, m_pContext, &pInitData->ColliderDesc);
		break;
	}

	return S_OK;
}

void CCollider::Tick(_fmatrix WorldMatrix)
{
	m_pBounding->Tick(WorldMatrix);
}

_bool CCollider::Find_CurrentCollision(CCollider* pTarget_Collider)
{
	if (m_CurrentCollisions.find(pTarget_Collider) == m_CurrentCollisions.end())
		return false;

	return true;
}

_bool CCollider::Delete_CurrentCollision(CCollider* pTarget_Collider)
{
	if (m_CurrentCollisions.find(pTarget_Collider) == m_CurrentCollisions.end())
		return false;

	Safe_Release(pTarget_Collider);
	m_CurrentCollisions.erase(pTarget_Collider);
	return true;
}

void CCollider::Clear_Collisions()
{
	for (auto iter : m_CurrentCollisions)
	{
		Safe_Release(iter);
	}
	m_CurrentCollisions.clear();
}

void CCollider::CollisionEnter(CCollider* pTarget_Collider)
{
	m_CurrentCollisions.insert(pTarget_Collider);
	Safe_AddRef(pTarget_Collider);

	m_ColData.pGameObject->Event_CollisionEnter(pTarget_Collider->Get_ColData(), &m_ColData);
}

void CCollider::CollisionStay(CCollider* pTarget_Collider)
{
	m_ColData.pGameObject->Event_CollisionStay(pTarget_Collider->Get_ColData(), &m_ColData);
}

void CCollider::CollisionExit(CCollider* pTarget_Collider)
{
	m_CurrentCollisions.erase(pTarget_Collider);
	Safe_Release(pTarget_Collider);
	m_ColData.pGameObject->Event_CollisionExit(pTarget_Collider->Get_ColData(), &m_ColData);
}

#ifdef _DEBUG

HRESULT CCollider::Render()
{
	m_pContext->GSSetShader(nullptr, nullptr, 0);

	m_pEffect->SetWorld(XMMatrixIdentity());
	m_pEffect->SetView(m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
	m_pEffect->SetProjection(m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));

	m_pContext->IASetInputLayout(m_pInputLayout);

	m_pEffect->Apply(m_pContext);

	m_pBatch->Begin();

	m_pBounding->Render(m_pBatch);

	m_pBatch->End();

	return S_OK;
}
#endif // _DEBUG

_bool CCollider::Intersect(CCollider * pTargetCollider)
{
	return m_pBounding->Intersect(pTargetCollider->m_eType, pTargetCollider->m_pBounding);
}

CCollider * CCollider::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, TYPE eType)
{
	CCollider*		pInstance = new CCollider(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eType)))
	{
		MSG_BOX("Failed to Created : CCollider");
		Safe_Release(pInstance);
	}

	return pInstance;
}


CComponent * CCollider::Clone(void * pArg)
{
	CCollider*		pInstance = new CCollider(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCollider");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CCollider::Free()
{
	__super::Free();

	Clear_Collisions();
#ifdef _DEBUG

	if (false == m_isCloned)
	{
		Safe_Delete(m_pBatch);
		Safe_Delete(m_pEffect);
	}

	Safe_Release(m_pInputLayout);

#endif

	Safe_Release(m_pBounding);
}

void CCollider::Reset_Iscoll()
{
	m_pBounding->Reset_Iscoll();
}
