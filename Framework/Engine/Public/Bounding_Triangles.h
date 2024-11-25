#pragma once

#include "Bounding.h"

BEGIN(Engine)

class CBounding_Triangles final : public CBounding
{
private:
	CBounding_Triangles(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CBounding_Triangles() = default;

public:
	virtual void* Get_Bounding() override { return nullptr; }
	CCollider::TRIANGLE_DESC* Get_Desc() { return &m_MyDesc;}

public:
	HRESULT Initialize(void* pArg);
	virtual void Tick(_fmatrix WorldMatrix) override;
	virtual HRESULT Render(PrimitiveBatch<VertexPositionColor>* pBatch) override;
	virtual _bool Intersect(CCollider::TYPE eType, CBounding * pBounding) override;

private:
	_bool Intersect(class CBounding_AABB* pTargetBounding);
	_bool Intersect(class CBounding_OBB* pTargetBounding);
	_bool Intersect(class CBounding_Sphere* pTargetBounding);
	_bool Intersect(class CBounding_Cylinder* pTargetBounding);
	_bool Intersect(class CBounding_Capsule* pTargetBounding);
	_bool Intersect(CBounding_Triangles* pTargetBounding);


private:
	void	Triangle_Tranform(_fmatrix WorldMatrix);

private:
	CCollider::TRIANGLE_DESC  m_OriginalMyDesc;
	CCollider::TRIANGLE_DESC  m_MyDesc;

public:
	static CBounding_Triangles* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	virtual void Free() override;
};

END