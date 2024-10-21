#pragma once

#include "Bounding.h"

BEGIN(Engine)

class CBounding_AABB final : public CBounding
{
public:
	//typedef struct : public CBounding::BOUNDING_DESC
	//{
	//	_float3		vExtents;
	//}AABB_DESC;
private:
	CBounding_AABB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CBounding_AABB() = default;

public:
	virtual void* Get_Bounding() override {
		return m_pAABB;
	}

public:
	HRESULT Initialize(void* pArg);
	virtual void Tick(_fmatrix WorldMatrix) override;
	virtual HRESULT Render(PrimitiveBatch<VertexPositionColor>* pBatch) override;
	virtual _bool Intersect(CCollider::TYPE eType, CBounding* pBounding) override;

private:
	BoundingBox*				m_pOriginalAABB = { nullptr };
	BoundingBox*				m_pAABB = { nullptr };

private:
	_bool Intersect(BoundingBox* pTargetAABB);


public:
	static CBounding_AABB* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	virtual void Free() override;
};

END