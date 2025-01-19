#pragma once

#include "Bounding.h"

BEGIN(Engine)

class CBounding_Capsule final : public CBounding
{
public:
	enum class ContactPointType
	{
		VertexPlane,  // �������� ��� �� �浹
		EdgeEdge,     // ������ ���� �� �浹
		FaceFace,     // ��� �� �� �浹
		Unknown       // ������ ���ǵ��� ���� ���
	};

	struct ContactPoint
	{
		_vector ShapeContactPoints[2];	// �浹�� �� ��ü������ ������ (���� ��ǥ)
		_vector ShapeContactNormal;		// ���� ������ ���� ����
		float Phi;						// �浹 ���� (������ ��� ����)
		ContactPointType ContactType{ ContactPointType::Unknown };	// �������� ���� (Vertex-Plane, Edge-Edge ��)
		_int FaceIndex;					// �浹�� ���� �ε��� (�ﰢ�� ��)
		_bool isCol{ false };			// ���� ���� ����
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
		_vector A1, _vector d1, // ���� 1�� �������� ���� ����
		_vector A2, _vector d2, // ���� 2�� �������� ���� ����
		_vector& intersection    // ������ (���)
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

	//0 : x, 1 : ���ο� ����, 2 : ���ο� ����
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
	//vector<���� �ﰢ�� ��Ʈvector<�ﰢ����vector <����Ʈ2�� > >>
	 
	// ������ ��
	BoundingSphere* m_pSphere_1			= { nullptr };
	BoundingSphere* m_pSphere_2			= { nullptr };

	_vector vColNormal {0.f, 0.f, 0.f};
	_float	fColDepth{ 0.f };

public:
	static CBounding_Capsule* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	virtual void Free() override;
};

END