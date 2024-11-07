#pragma once

#include "Bounding.h"

BEGIN(Engine)

class CBounding_OBB final : public CBounding
{
public:
	struct OBB_COL_DESC
	{
		_float3		vCenter;
		_float3		vCenterAxis[3];
		_float3		vAlignAxis[3];
	};

	struct My_Desc
	{
		_float3 _Center;
		_float3 _Extents;
		_float4 _Orientation;
	};

public:
	//typedef struct : public CBounding::BOUNDING_DESC
	//{
	//	_float3		vExtents;
	//	_float3		vRadians; /* x : x축 기준 회전 각도, y : y축 기준 회전 각도, z : z축 기준 회전 각도 */
	//}OBB_DESC;

private:
	CBounding_OBB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CBounding_OBB() = default;

public:
	virtual void* Get_Bounding() override {
		return m_pOBB;
	}

public:
	HRESULT Initialize(void* pArg);
	virtual void Tick(_fmatrix WorldMatrix) override;
	virtual HRESULT Render(PrimitiveBatch<VertexPositionColor>* pBatch) override;
	virtual _bool Intersect(CCollider::TYPE eType, CBounding * pBounding) override;
	_vector CalColFace(My_Desc* _Target_Desc);

public:
	OBB_COL_DESC Compute_OBBColDesc();

private:
	void	TransformMyDesc(_fmatrix WorldMatrix);

public:
	My_Desc*	Get_MyDesc() { return &m_MyDesc; }
	_vector		Get_ColFace() { return vColFace; }

private:
	BoundingOrientedBox*				m_pOriginalOBB = { nullptr };
	BoundingOrientedBox*				m_pOBB = { nullptr };

	My_Desc m_MyOriginalDesc;
	My_Desc m_MyDesc;

	_vector vColFace{};


private:
	_bool Intersect(CBounding_OBB* pTargetBounding);
	

public:
	static CBounding_OBB* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	virtual void Free() override;
};

END