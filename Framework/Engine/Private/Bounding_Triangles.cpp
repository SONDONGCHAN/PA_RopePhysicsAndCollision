#include "Bounding_Triangles.h"
#include "Collider.h"

CBounding_Triangles::CBounding_Triangles(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CBounding(pDevice, pContext)
{

}

HRESULT CBounding_Triangles::Initialize(void* pArg)
{
	vector<CCollider::TRIANGLE_DESC> vecCapsuleDesc = *(vector<CCollider::TRIANGLE_DESC>*)pArg;
	
	m_vecOriginalMyDesc = vecCapsuleDesc;
	m_vecMyDesc = vecCapsuleDesc;
	
	return S_OK;
}

void CBounding_Triangles::Tick(_fmatrix WorldMatrix)
{
	Triangle_Tranform(WorldMatrix);
}

HRESULT CBounding_Triangles::Render(PrimitiveBatch<VertexPositionColor>* pBatch)
{
	_vector vColor_Red = XMVectorSet(1.f, 0.f, 0.f, 1.f);
	_vector vColor_Green = XMVectorSet(0.f, 1.f, 0.f, 1.f);
	_vector vColor = m_isColl == true ? vColor_Red : vColor_Green;

	for (int i = 0; i < m_vecMyDesc.size(); ++i)
	{
		_vector vVertex1 = XMLoadFloat3(&m_vecMyDesc[i].vVertex1);
		_vector vVertex2 = XMLoadFloat3(&m_vecMyDesc[i].vVertex2);
		_vector vVertex3 = XMLoadFloat3(&m_vecMyDesc[i].vVertex3);

		DX::DrawTriangle(pBatch, vVertex1, vVertex2, vVertex3, vColor);
	}
	return S_OK;
}

_bool CBounding_Triangles::Intersect(CCollider::TYPE eType, CBounding* pBounding)
{
	_bool isColl = false;

	switch (eType)
	{
	case CCollider::TYPE_AABB:
		isColl = Intersect((CBounding_AABB*)pBounding);
		break;

	case CCollider::TYPE_OBB:
		isColl = Intersect((CBounding_OBB*)pBounding);
		break;

	case CCollider::TYPE_SPHERE:
		isColl = Intersect((CBounding_Sphere*)pBounding);
		break;

	case CCollider::TYPE_CYLINDER:
		isColl = Intersect((CBounding_Cylinder*)pBounding);
		break;

	case CCollider::TYPE_CAPSULE:
		isColl = Intersect((CBounding_Capsule*)pBounding);
		break;

	case CCollider::TYPE_TRIANGLE:
		isColl = Intersect((CBounding_Triangles*)pBounding);
		break;
	}

	if (isColl && (!m_isColl))
		m_isColl = true;

	return isColl;
}

_bool CBounding_Triangles::Intersect(CBounding_AABB* pTargetBounding)
{
	return false;
}

_bool CBounding_Triangles::Intersect(CBounding_OBB* pTargetBounding)
{
	return false;
}

_bool CBounding_Triangles::Intersect(CBounding_Sphere* pTargetBounding)
{
	return false;
}

_bool CBounding_Triangles::Intersect(CBounding_Cylinder* pTargetBounding)
{
	return false;
}

_bool CBounding_Triangles::Intersect(CBounding_Capsule* pTargetBounding)
{
	return false;
}

_bool CBounding_Triangles::Intersect(CBounding_Triangles* pTargetBounding)
{
	return false;
}

void CBounding_Triangles::Triangle_Tranform(_fmatrix WorldMatrix)
{
	/* 삼각형 위치 갱신*/
	for (int i = 0; i < m_vecMyDesc.size(); ++i)
	{
		// 원본의 점들을 로드
		XMVECTOR v1 = XMLoadFloat3(&m_vecOriginalMyDesc[i].vVertex1);
		XMVECTOR v2 = XMLoadFloat3(&m_vecOriginalMyDesc[i].vVertex2);
		XMVECTOR v3 = XMLoadFloat3(&m_vecOriginalMyDesc[i].vVertex3);

		// 월드 매트릭스를 이용해 변환
		v1 = XMVector3Transform(v1, WorldMatrix);
		v2 = XMVector3Transform(v2, WorldMatrix);
		v3 = XMVector3Transform(v3, WorldMatrix);

		// 변환된 값을 현재 구조체에 저장
		XMStoreFloat3(&m_vecMyDesc[i].vVertex1, v1);
		XMStoreFloat3(&m_vecMyDesc[i].vVertex2, v2);
		XMStoreFloat3(&m_vecMyDesc[i].vVertex3, v3);

		// 중앙 좌표 계산 (vCenter)
		XMVECTOR center = (v1 + v2 + v3) / 3.0f;
		XMStoreFloat3(&m_vecMyDesc[i].vCenter, center);
	}
}

CBounding_Triangles* CBounding_Triangles::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CBounding_Triangles* pInstance = new CBounding_Triangles(pDevice, pContext);
	
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CBounding_Triangles");
		Safe_Release(pInstance);
	}
	
	return pInstance;
}

void CBounding_Triangles::Free()
{
	__super::Free();
}
