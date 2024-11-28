#pragma once

#include "Bounding.h"

BEGIN(Engine)

class CBounding_Capsule final : public CBounding
{
public:
	enum class ContactPointType
	{
		VertexPlane,  // 꼭짓점과 평면 간 충돌
		EdgeEdge,     // 엣지와 엣지 간 충돌
		FaceFace,     // 면과 면 간 충돌
		Unknown       // 유형이 정의되지 않은 경우
	};

	struct ContactPoint
	{
		_vector ShapeContactPoints[2];	// 충돌한 두 물체에서의 접촉점 (월드 좌표)
		_vector ShapeContactNormal;		// 접촉 지점의 법선 벡터
		float Phi;						// 충돌 깊이 (음수일 경우 관통)
		ContactPointType ContactType{ ContactPointType::Unknown };	// 접촉점의 유형 (Vertex-Plane, Edge-Edge 등)
		_int FaceIndex;					// 충돌한 면의 인덱스 (삼각형 등)
		_bool isCol{ false };			// 실제 접촉 여부
	};


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
	_bool Intersect(class CBounding_Triangles* pTargetBounding);

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
	float SegmentSegmentDistanceSq(XMVECTOR p1, XMVECTOR q1, XMVECTOR p2, XMVECTOR q2, XMVECTOR& vNormal);

	void NearestPointsOnLineSegments(
		const XMVECTOR& P0, const XMVECTOR& P1,
		const XMVECTOR& EdgeP0, const XMVECTOR& EdgeP1,
		float& SegmentT, float& EdgeT,
		XMVECTOR& SegmentP, XMVECTOR& EdgeP);

	_bool AddCapsuleTriangleParallelEdgeManifoldContacts(
		const XMVECTOR& P0, const XMVECTOR& P1,
		const XMVECTOR& EdgeP0, const XMVECTOR& EdgeP1,
		const _float R, const _float RejectDistanceSq, const _float NormalToleranceSq,
		vector<vector<ContactPoint>>& _vvecPoints, _int _Index);

	inline bool InRangeClosed(float Value, float Min, float Max)
	{
		return (Value >= Min) && (Value <= Max);
	}
	inline bool InRangeOpen(float Value, float Min, float Max)
	{
		return (Value > Min) && (Value < Max);
	}
public:
	_int Get_State() { return iState; }
	inline _int Check_State(CBounding* pTargetBounding, _int Index) override;

	//0 : x, 1 : 내부에 있음, 2 : 내부에 없음
public:
	_vector Get_ColNormal() { return vColNormal; }
	_float	Get_ColDepth() { return fColDepth; }
	vector<vector<vector<ContactPoint>>>&	Get_Points() { return m_vecPoints; }

private:
	_int iState{ -1 };

private:
	CCollider::CAPSULE_DESC m_pOriginalMyDesc{  };
	CCollider::CAPSULE_DESC m_pMyDesc{  };

	vector<vector<vector<ContactPoint>>> m_vecPoints{	};
	//vector<여러 삼각형 세트vector<삼각형들vector <포인트2개 > >>
	 
	// 렌더링 용
	BoundingSphere* m_pSphere_1			= { nullptr };
	BoundingSphere* m_pSphere_2			= { nullptr };

	_vector vColNormal {0.f, 0.f, 0.f};
	_float	fColDepth{ 0.f };

public:
	static CBounding_Capsule* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	virtual void Free() override;
};

END