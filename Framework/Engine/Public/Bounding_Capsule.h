#pragma once

#include "Bounding.h"

BEGIN(Engine)

class CBounding_Capsule final : public CBounding
{
private:
	CBounding_Capsule(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CBounding_Capsule() = default;

public:
	virtual void* Get_Bounding() override { return nullptr; }

public:
	HRESULT Initialize(void* pArg);
	virtual void Tick(_fmatrix WorldMatrix) override;
	virtual HRESULT Render(PrimitiveBatch<VertexPositionColor>* pBatch) override;
	virtual _bool Intersect(CCollider::TYPE eType, CBounding * pBounding) override;

private:
	//BoundingOrientedBox*				m_pOriginalSylinder = { nullptr };
	//BoundingOrientedBox*				m_pSylinder = { nullptr };

private:
	_bool Intersect(class CBounding_AABB* pTargetBounding);
	_bool Intersect(class CBounding_OBB* pTargetBounding);
	_bool Intersect(class CBounding_Sphere* pTargetBounding);
	_bool Intersect(class CBounding_Cylinder* pTargetBounding);
	_bool Intersect(CBounding_Capsule* pTargetBounding);

private:
	void	Capsule_Tranform(_fmatrix WorldMatrix);

private:
	CCollider::CAPSULE_DESC m_pOriginalMyDesc{  };
	CCollider::CAPSULE_DESC m_pMyDesc{  };

	// ·»´õ¸µ ¿ë
	BoundingSphere* m_pSphere_1			= { nullptr };
	BoundingSphere* m_pSphere_2			= { nullptr };


public:
	static CBounding_Capsule* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	virtual void Free() override;
};

END