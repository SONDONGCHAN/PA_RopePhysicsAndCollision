#include "Bounding_Cylinder.h"
#include "Collider.h"

CBounding_Cylinder::CBounding_Cylinder(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CBounding(pDevice, pContext)
{
}

HRESULT CBounding_Cylinder::Initialize(void* pArg)
{
	return S_OK;
}

void CBounding_Cylinder::Tick(_fmatrix WorldMatrix)
{
}

HRESULT CBounding_Cylinder::Render(PrimitiveBatch<VertexPositionColor>* pBatch)
{
	return S_OK;
}

_bool CBounding_Cylinder::Intersect(CCollider::TYPE eType, CBounding* pBounding)
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
	}
	
	if (isColl && (!m_isColl))
		m_isColl = true;
	
	return isColl;
}

_bool CBounding_Cylinder::Intersect(CBounding_AABB* pTargetBounding)
{
	return _bool();
}

_bool CBounding_Cylinder::Intersect(CBounding_OBB* pTargetBounding)
{
	return _bool();
}

_bool CBounding_Cylinder::Intersect(CBounding_Sphere* pTargetBounding)
{
	return _bool();
}

_bool CBounding_Cylinder::Intersect(CBounding_Cylinder* pTargetBounding)
{
	return _bool();
}

CBounding_Cylinder* CBounding_Cylinder::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	return nullptr;
}

void CBounding_Cylinder::Free()
{
	__super::Free();
}
