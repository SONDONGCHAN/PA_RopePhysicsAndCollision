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
}
