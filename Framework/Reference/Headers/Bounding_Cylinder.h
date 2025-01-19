#pragma once

#include "Bounding.h"

BEGIN(Engine)

class CBounding_Cylinder final : public CBounding
{

public:
	//typedef struct : public CBounding::BOUNDING_DESC
	//{
	//	_float3		vExtents;
	//	_float3		vRadians; /* x : x축 기준 회전 각도, y : y축 기준 회전 각도, z : z축 기준 회전 각도 */
	//}OBB_DESC;

private:
	CBounding_Cylinder(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CBounding_Cylinder() = default;

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
	_bool Intersect(CBounding_Cylinder* pTargetBounding);

private:
	CCollider::CYLINDER_DESC* m_pOriginalMyDesc{ nullptr };
	CCollider::CYLINDER_DESC* m_pMyDesc{ nullptr };

public:
	static CBounding_Cylinder* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	virtual void Free() override;
};

END