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
	_vector ClosestPointOnLineSegment(const _vector& point, const _vector& start, const _vector& end);
	_vector ClosestPointBetweenSegments(const _vector& A1, const _vector& A2, const _vector& B1, const _vector& B2);
	bool GetIntersectionPoint(
		_vector A1, _vector d1, // 선분 1의 시작점과 방향 벡터
		_vector A2, _vector d2, // 선분 2의 시작점과 방향 벡터
		_vector& intersection    // 교차점 (출력)
	);

private:
	CCollider::CAPSULE_DESC m_pOriginalMyDesc{  };
	CCollider::CAPSULE_DESC m_pMyDesc{  };

	// 렌더링 용
	BoundingSphere* m_pSphere_1			= { nullptr };
	BoundingSphere* m_pSphere_2			= { nullptr };


public:
	static CBounding_Capsule* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	virtual void Free() override;
};

END