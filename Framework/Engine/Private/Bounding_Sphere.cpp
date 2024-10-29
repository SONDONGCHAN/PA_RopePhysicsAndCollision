#include "..\Public\Bounding_Sphere.h"
#include "Collider.h"

CBounding_Sphere::CBounding_Sphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CBounding(pDevice, pContext)
{
}


HRESULT CBounding_Sphere::Initialize(void * pArg)
{
	/* 초기상태는 로컬상태 */
	CCollider::SPHERE_DESC*	pSphereDesc = (CCollider::SPHERE_DESC*)pArg;

	m_pOriginalSphere = new BoundingSphere(pSphereDesc->vCenter, pSphereDesc->fRadius);
	m_pSphere = new BoundingSphere(*m_pOriginalSphere);
	
	return S_OK;
}

void CBounding_Sphere::Tick(_fmatrix WorldMatrix)
{
	m_pOriginalSphere->Transform(*m_pSphere, WorldMatrix);
}

HRESULT CBounding_Sphere::Render(PrimitiveBatch<VertexPositionColor>* pBatch)
{
	DX::Draw(pBatch, *m_pSphere, m_isColl == true ? XMVectorSet(1.f, 0.f, 0.f, 1.f) : XMVectorSet(0.f, 1.f, 0.f, 1.f));

	return S_OK;
}

_bool CBounding_Sphere::Intersect(CCollider::TYPE eType, CBounding * pBounding)
{
	_bool isColl = false;
	
	void*		pTargetBounding = pBounding->Get_Bounding();

	switch (eType)
	{
	case CCollider::TYPE_AABB:
		isColl = m_pSphere->Intersects(*(BoundingBox*)pTargetBounding);
		break;
	case CCollider::TYPE_OBB:
		isColl = m_pSphere->Intersects(*(BoundingOrientedBox*)pTargetBounding);
		break;
	case CCollider::TYPE_SPHERE:
		isColl = m_pSphere->Intersects(*(BoundingSphere*)pTargetBounding);
		break;
	}

	if (isColl && (!m_isColl))
		m_isColl = true;

	return isColl;
}

CBounding_Sphere * CBounding_Sphere::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, void * pArg)
{
	CBounding_Sphere*		pInstance = new CBounding_Sphere(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CBounding_Sphere");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBounding_Sphere::Free()
{
	__super::Free();

	Safe_Delete(m_pOriginalSphere);
	Safe_Delete(m_pSphere);
}
