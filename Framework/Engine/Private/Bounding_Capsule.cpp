#include "Bounding_Capsule.h"
#include "Bounding_Sphere.h"
#include "Bounding_OBB.h"
#include "Bounding_Triangles.h"

#include "Collider.h"


CBounding_Capsule::CBounding_Capsule(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CBounding(pDevice, pContext)
{
}

HRESULT CBounding_Capsule::Initialize(void* pArg)
{
	CCollider::CAPSULE_DESC pCapsuleDesc = *(CCollider::CAPSULE_DESC*)pArg;

	m_pOriginalMyDesc = pCapsuleDesc;
	m_pMyDesc = pCapsuleDesc;

	/* 렌더링용 */
	m_pSphere_1 = new BoundingSphere(m_pMyDesc.vCenter, m_pMyDesc.fRadius);
	m_pSphere_2 = new BoundingSphere(m_pMyDesc.vCenter, m_pMyDesc.fRadius);
	/* 렌더링용 */

	return S_OK;
}

void CBounding_Capsule::Tick(_fmatrix WorldMatrix)
{
	Capsule_Tranform(WorldMatrix);
}

HRESULT CBounding_Capsule::Render(PrimitiveBatch<VertexPositionColor>* pBatch)
{
	_vector vColor_Red = XMVectorSet(1.f, 0.f, 0.f, 1.f);
	_vector vColor_Green = XMVectorSet(0.f, 1.f, 0.f, 1.f);
	_vector vColor = m_isColl == true ? vColor_Red : vColor_Green;

	DX::Draw(pBatch, *m_pSphere_1, vColor);
	DX::Draw(pBatch, *m_pSphere_2, vColor);
	
	const float radius = m_pSphere_1->Radius;
	XMVECTOR xaxis = g_XMIdentityR0 * radius;
	XMVECTOR yaxis = g_XMIdentityR1 * radius;
	XMVECTOR zaxis = g_XMIdentityR2 * radius;

	_vector vTop = XMLoadFloat3(&m_pSphere_1->Center);
	_vector vBot = XMLoadFloat3(&m_pSphere_2->Center);
	_vector vCen;

	_vector vX = { 1.f, 0.f, 0.f };
	_vector vZ = { 0.f, 0.f, 1.f };
	_vector vXZ1 = XMVector3Normalize(vX + vZ);
	_vector vXZ2 = XMVector3Normalize(vX - vZ);
	
	//_vector vXZ1axis = vXZ1 * radius;
	//_vector vXZ2axis = vXZ2 * radius;
	//DX::DrawRing(pBatch, vTop, vXZ1axis, yaxis, vColor);
	//DX::DrawRing(pBatch, vTop, vXZ2axis, yaxis, vColor);
	//DX::DrawRing(pBatch, vBot, vXZ1axis, yaxis, vColor);
	//DX::DrawRing(pBatch, vBot, vXZ2axis, yaxis, vColor);

	vCen = XMVectorLerp(vTop, vBot, 0.25f);
	DX::DrawRing(pBatch, vCen, xaxis, zaxis, vColor);
	vCen = XMVectorLerp(vTop, vBot, 0.5f);
	DX::DrawRing(pBatch, vCen, xaxis, zaxis, vColor);
	vCen = XMVectorLerp(vTop, vBot, 0.75f);
	DX::DrawRing(pBatch, vCen, xaxis, zaxis, vColor);

	
	_vector v1 = vTop - (vX * radius);
	_vector v2 = vTop + (vX * radius);
	_vector v3 = vBot + (vX * radius);
	_vector v4 = vBot - (vX * radius);
	DX::DrawQuad(pBatch, v1, v2, v3, v4, vColor);
	v1 = vTop - (vZ * radius);
	v2 = vTop + (vZ * radius);
	v3 = vBot + (vZ * radius);
	v4 = vBot - (vZ * radius);
	DX::DrawQuad(pBatch, v1, v2, v3, v4, vColor);
	v1 = vTop - (vXZ1 * radius);
	v2 = vTop + (vXZ1 * radius);
	v3 = vBot + (vXZ1 * radius);
	v4 = vBot - (vXZ1 * radius);
	DX::DrawQuad(pBatch, v1, v2, v3, v4, vColor);
	v1 = vTop - (vXZ2 * radius);
	v2 = vTop + (vXZ2 * radius);
	v3 = vBot + (vXZ2 * radius);
	v4 = vBot - (vXZ2 * radius);
	DX::DrawQuad(pBatch, v1, v2, v3, v4, vColor);

	return S_OK;
}

_bool CBounding_Capsule::Intersect(CCollider::TYPE eType, CBounding* pBounding)
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

	case CCollider::TYPE_CAPSULE:
		isColl = Intersect((CBounding_Capsule*)pBounding);
		break;

	case CCollider::TYPE_TRIANGLE:
		isColl = Intersect((CBounding_Triangles*)pBounding);
		break;
	}

	if (isColl && (!m_isColl))
		m_isColl = true;

	return isColl;
}

_bool CBounding_Capsule::Intersect(CBounding_AABB* pTargetBounding)
{
	return _bool();
}

_bool CBounding_Capsule::Intersect(CBounding_OBB* pTargetBounding)
{
	_vector vOBBCenter = XMLoadFloat3(&pTargetBounding->Get_MyDesc()->_Center);
	_vector vOBBExtents = XMLoadFloat3(&pTargetBounding->Get_MyDesc()->_Extents);
	_vector vOBBOrientation = XMLoadFloat4(&pTargetBounding->Get_MyDesc()->_Orientation);

	_vector	vTop = XMVectorAdd(XMLoadFloat3(&m_pMyDesc.vCenter), XMVectorScale(XMLoadFloat3(&m_pMyDesc.vDir), m_pMyDesc.fHeight));
	_vector	vBot = XMVectorAdd(XMLoadFloat3(&m_pMyDesc.vCenter), -(XMVectorScale(XMLoadFloat3(&m_pMyDesc.vDir), m_pMyDesc.fHeight)));

	// 캡슐의 두 점을 OBB 좌표계로 변환
	_vector vOBBSpaceTop = XMVector3InverseRotate(XMVectorSubtract(vTop, vOBBCenter), vOBBOrientation);
	_vector vOBBSpaceBot = XMVector3InverseRotate(XMVectorSubtract(vBot, vOBBCenter), vOBBOrientation);

	// 캡슐내부 선분의 양 끝점과 OBB의 최단 거리 계산
	_vector clampedTop = XMVectorClamp(vOBBSpaceTop, XMVectorNegate(vOBBExtents), vOBBExtents);
	_vector clampedBot = XMVectorClamp(vOBBSpaceBot, XMVectorNegate(vOBBExtents), vOBBExtents);

	// 클램핑된 점과 원래 점 사이의 벡터를 계산하여 최단 거리 구함
	_vector deltaTop = XMVectorSubtract(vOBBSpaceTop, clampedTop);
	_vector deltaBot = XMVectorSubtract(vOBBSpaceBot, clampedBot);

	// 두 점 중 최소 거리를 선택
	float distSquaredTop = XMVectorGetX(XMVector3LengthSq(deltaTop));
	float distSquaredBot = XMVectorGetX(XMVector3LengthSq(deltaBot));
	float minDistSquared = min(distSquaredTop, distSquaredBot);

	// 캡슐의 반지름과 비교하여 충돌 여부 판단
	if (minDistSquared < (m_pMyDesc.fRadius * m_pMyDesc.fRadius))
	{
		XMVECTOR closestPoint = (distSquaredTop < distSquaredBot) ? clampedTop : clampedBot;
		XMVECTOR vNormal = XMVectorSubtract((distSquaredTop < distSquaredBot) ? vOBBSpaceTop : vOBBSpaceBot, closestPoint);
		fColDepth = m_pMyDesc.fRadius - sqrtf(minDistSquared);
		vColNormal = XMVector3Normalize(vNormal);
		vColNormal = XMVector3Rotate(vColNormal, vOBBOrientation);
		return true;
	}
	//========================

	// OBB의 8개 꼭짓점 계산
	XMVECTOR corners[8];
	corners[0] = XMVectorSet(-XMVectorGetX(vOBBExtents), -XMVectorGetY(vOBBExtents), -XMVectorGetZ(vOBBExtents), 0.0f);
	corners[1] = XMVectorSet(XMVectorGetX(vOBBExtents), -XMVectorGetY(vOBBExtents), -XMVectorGetZ(vOBBExtents), 0.0f);
	corners[2] = XMVectorSet(-XMVectorGetX(vOBBExtents), -XMVectorGetY(vOBBExtents), XMVectorGetZ(vOBBExtents), 0.0f);
	corners[3] = XMVectorSet(XMVectorGetX(vOBBExtents), -XMVectorGetY(vOBBExtents), XMVectorGetZ(vOBBExtents), 0.0f);
	corners[4] = XMVectorSet(-XMVectorGetX(vOBBExtents), XMVectorGetY(vOBBExtents), -XMVectorGetZ(vOBBExtents), 0.0f);
	corners[5] = XMVectorSet(XMVectorGetX(vOBBExtents), XMVectorGetY(vOBBExtents), -XMVectorGetZ(vOBBExtents), 0.0f);
	corners[6] = XMVectorSet(-XMVectorGetX(vOBBExtents), XMVectorGetY(vOBBExtents), XMVectorGetZ(vOBBExtents), 0.0f);
	corners[7] = XMVectorSet(XMVectorGetX(vOBBExtents), XMVectorGetY(vOBBExtents), XMVectorGetZ(vOBBExtents), 0.0f);


	// OBB의 12개 모서리 정의
	int edgePairs[12][2] =
	{
		{0, 1}, {1, 3}, {3, 2}, {2, 0}, // 아래 네 모서리
		{4, 5}, {5, 7}, {7, 6}, {6, 4}, // 위 네 모서리
		{0, 4}, {1, 5}, {3, 7}, {2, 6}  // 수직 모서리
	};

	vector<pair<XMVECTOR, XMVECTOR>> edges;
	// 모서리 추가
	for (int i = 0; i < 12; ++i)
		edges.emplace_back(corners[edgePairs[i][0]], corners[edgePairs[i][1]]);

	for (int i = 0; i < 12; ++i)
	{
		_vector vNormal;
		float distSq = SegmentSegmentDistanceSq(vOBBSpaceTop, vOBBSpaceBot, edges[i].first, edges[i].second, vNormal);

		if (distSq < (m_pMyDesc.fRadius * m_pMyDesc.fRadius))
		{
			fColDepth = m_pMyDesc.fRadius - sqrtf(distSq);
			fColDepth = XMVectorGetX(XMVector3Length(vNormal));
			vColNormal = XMVector3Normalize(vNormal);
			vColNormal = XMVector3Rotate(vColNormal, vOBBOrientation);
			return true; // 모서리 충돌
		}
	}

	return false;
}

_bool CBounding_Capsule::Intersect(CBounding_Sphere* pTargetBounding)
{
	_vector vSphereCenter = XMLoadFloat3(&static_cast<BoundingSphere*>(pTargetBounding->Get_Bounding())->Center);
	_float	fSphereRadius = static_cast<BoundingSphere*>(pTargetBounding->Get_Bounding())->Radius;

	_vector vCenterToSphere = XMVectorSubtract(vSphereCenter, XMLoadFloat3(&m_pMyDesc.vCenter));
	_float	fDotProj = XMVector3Dot(vCenterToSphere, XMLoadFloat3(&m_pMyDesc.vDir)).m128_f32[0];

	// 투영한 지점이 캡슐 몸통부분 내부에 있는지 확인
	
	// 내부에 있는 경우
	if ( -m_pMyDesc.fHeight <= fDotProj && fDotProj <= m_pMyDesc.fHeight)
	{
		_vector vDot = XMVectorAdd(XMLoadFloat3(&m_pMyDesc.vCenter), XMVectorScale(XMLoadFloat3(&m_pMyDesc.vDir), fDotProj));
		_vector vDotToSphere = XMVectorSubtract(vSphereCenter, vDot);

		_float	fDotToSphere = XMVector3Length(vDotToSphere).m128_f32[0];

		if (fDotToSphere <= m_pMyDesc.fRadius + fSphereRadius)
			return true;
		else
			return false;
	}
	// 내부에 없는경우
	else
	{
		_vector vCenter;

		if (m_pMyDesc.fHeight < fDotProj)
		{
			vCenter = XMVectorAdd(XMLoadFloat3(&m_pMyDesc.vCenter), XMVectorScale(XMLoadFloat3(&m_pMyDesc.vDir), m_pMyDesc.fHeight));
		}
		else
		{
			vCenter = XMVectorAdd(XMLoadFloat3(&m_pMyDesc.vCenter), XMVectorScale(XMLoadFloat3(&m_pMyDesc.vDir), -m_pMyDesc.fHeight));
		}

		_vector vCenterToSphere = XMVectorSubtract(vSphereCenter, vCenter);

		_float	fCenterToSphere = XMVector3Length(vCenterToSphere).m128_f32[0];

		if (fCenterToSphere <= m_pMyDesc.fRadius + fSphereRadius)
			return true;
		else
			return false;
	}
}

_bool CBounding_Capsule::Intersect(CBounding_Cylinder* pTargetBounding)
{
	return _bool();
}

_bool CBounding_Capsule::Intersect(CBounding_Capsule* pTargetBounding)
{
	return _bool();
}

_bool CBounding_Capsule::Intersect(CBounding_Triangles* pTargetBounding)
{
	m_vecPoints.clear();
	// 캡슐정보
	const _vector P0 = XMVectorAdd(XMLoadFloat3(&m_pMyDesc.vCenter), XMVectorScale(XMLoadFloat3(&m_pMyDesc.vDir), m_pMyDesc.fHeight));
	const _vector P1 = XMVectorAdd(XMLoadFloat3(&m_pMyDesc.vCenter), -(XMVectorScale(XMLoadFloat3(&m_pMyDesc.vDir), m_pMyDesc.fHeight)));
	const _float R = m_pMyDesc.fRadius;
	const _vector Axis = XMLoadFloat3(&m_pMyDesc.vDir);
	const _float L = m_pMyDesc.fHeight;
	const _float RejectDistance = R + 50.f;
	const _float RejectDistanceSq = RejectDistance * RejectDistance;

	// 삼각형 정보
	const _vector V0 = XMLoadFloat3(&pTargetBounding->Get_Desc()->vVertex1);
	const _vector V1 = XMLoadFloat3(&pTargetBounding->Get_Desc()->vVertex2);
	const _vector V2 = XMLoadFloat3(&pTargetBounding->Get_Desc()->vVertex3);
	const _vector vVertex[3] = {V0, V1, V2};

	const _vector Centroid = XMLoadFloat3(&pTargetBounding->Get_Desc()->vCenter);

	// 오차허용범위
	const _float DistanceTolerance = _float(1.e-5) * L;
	const _float NormalTolerance = _float(1.e-5);
	const _float NormalToleranceSq = NormalTolerance * NormalTolerance;
	const _float FaceContactSinAngleThreshold = _float(0.34);	// ~Sin(20deg)

	// 삼각형 노말
	const _vector& FaceP = V0;
	_vector FaceN = XMVector3Cross(V1 - V0, V2 - V0);
	//if (!FaceN.Normalize(NormalToleranceSq))
	//{
	//	// Degenerate triangle
	//	return;
	//}
	FaceN = XMVector3Normalize(FaceN);

	//캡슐과 삼각형의 거리
	const _float FaceD0 = XMVectorGetX(XMVector3Dot(P0 - V0, FaceN));
	const _float FaceD1 = XMVectorGetX(XMVector3Dot(P1 - V0, FaceN));
	const bool bIsParallelFace = XMScalarNearEqual(FaceD0, FaceD1, DistanceTolerance);

	// 거리기반 컬링
	if ((FaceD0 > RejectDistance) && (FaceD1 > RejectDistance))
		return false;
	
	// Reject if the middle of the capsule is inside the face (single-sided collision)
	// 백페이스 컬링
	const _float FaceDMid = 0.5f * (FaceD0 + FaceD1);
	if (FaceDMid < -DistanceTolerance)
	{
		// Far inside triangle
		return false;
	}


	// 삼각형의 각 엣지와 캡슐축과의 거리
	// Edge plane normals and signed distances to each segment point
	_vector EdgeNs[3];	// 엣지 노말
	_float	EdgeD0s[3]; // 엣지와 축의 한쪽 끝과의 거리
	_float	EdgeD1s[3];	// 엣지와 축의 다른 한쪽 끝과의 거리

	EdgeNs[0] = XMVector3Cross(V0 - V2, FaceN);
	EdgeNs[0] = XMVector3Normalize(EdgeNs[0]);
	EdgeD0s[0] = XMVectorGetX(XMVector3Dot(P0 - V0, EdgeNs[0]));
	EdgeD1s[0] = XMVectorGetX(XMVector3Dot(P1 - V0, EdgeNs[0]));
	if (((EdgeD0s[0] > RejectDistance) && (EdgeD1s[0] > RejectDistance)))
		return false;

	EdgeNs[1] = XMVector3Cross(V1 - V0, FaceN);
	EdgeNs[1] = XMVector3Normalize(EdgeNs[1]);
	EdgeD0s[1] = XMVectorGetX(XMVector3Dot(P0 - V1, EdgeNs[1]));
	EdgeD1s[1] = XMVectorGetX(XMVector3Dot(P1 - V1, EdgeNs[1]));
	if (((EdgeD0s[1] > RejectDistance) & (EdgeD1s[1] > RejectDistance)) != 0)
		return false;

	EdgeNs[2] = XMVector3Cross(V2 - V1, FaceN);
	EdgeNs[2] = XMVector3Normalize(EdgeNs[2]);
	EdgeD0s[2] = XMVectorGetX(XMVector3Dot(P0 - V2, EdgeNs[2]));
	EdgeD1s[2] = XMVectorGetX(XMVector3Dot(P1 - V2, EdgeNs[2]));
	if (((EdgeD0s[2] > RejectDistance) & (EdgeD1s[2] > RejectDistance)) != 0)
		return false;


	_vector	EdgeSegmentDeltas[3];	//캡슐의 축과 삼각형의 엣지 간의 벡터 차이
	_vector	EdgeEdgePs[3];			//삼각형 엣지와 캡슐 축에서 가장 가까운 점의 좌표
	_vector EdgeSegmentPs[3];
	_float	EdgeEdgeTs[3];			//선분의 파라메트릭 값
	_float	EdgeSegmentTs[3];
	_float	EdgeDistSqs[3];			//두 선분 사이 거리의 제곱
	_float	EdgeDistSigns[3];		//거리의 방향(양수 또는 음수)을 저장
	_float	EdgeDotFace[3];			//삼각형의 법선(Face Normal)과 엣지 간의 관계를 저장
	_int	EdgeVertexIndex0 = 2;	//삼각형 엣지의 첫 번째 꼭짓점 인덱스 (2 -> 0 -> 1 -> 순환).

	for (int EdgeIndex = 0; EdgeIndex < 3; ++EdgeIndex)
	{
		const int EdgeVertexIndex1 = EdgeIndex;
		const _vector& EdgeP0 = vVertex[EdgeVertexIndex0];
		const _vector& EdgeP1 = vVertex[EdgeVertexIndex1];
		EdgeVertexIndex0 = EdgeVertexIndex1;

		// Find the nearest point on the capsule segment to the edge segment
		_float	SegmentT, EdgeT;
		_vector	SegmentP, EdgeP;
		NearestPointsOnLineSegments(P0, P1, EdgeP0, EdgeP1, SegmentT, EdgeT, SegmentP, EdgeP);

		// Calculate the separation vector (from triangle to capsule)
		_vector SegmentEdgeN = SegmentP - EdgeP;
		_float SegmentEdgeDistSign = 1.f;
		const _float SegmentEdgeDistSq = XMVectorGetX(XMVector3LengthSq(SegmentEdgeN));

		// If the near point on the capsule axis is inside the triangle, fix the normal
		const _float DotEdge = XMVectorGetX(XMVector3Dot(SegmentEdgeN, EdgeNs[EdgeIndex]));
		if (DotEdge < (-NormalTolerance))
		{
			SegmentEdgeN = -SegmentEdgeN;
			SegmentEdgeDistSign = -1.f;
		}
		const _float DotFace = XMVectorGetX(XMVector3Dot(SegmentEdgeN, FaceN));

		// If the near point on the capsule axis is outside the triangle check for cull distance
		if (SegmentEdgeDistSign > 0.f)
		{
			_float SeparationAxisCullDistance = RejectDistance;
			if (DotFace < -NormalTolerance)
			{
				SeparationAxisCullDistance = R;
			}

			if (SegmentEdgeDistSq > (SeparationAxisCullDistance * SeparationAxisCullDistance))
			{
				return false;
			}
		}

		EdgeSegmentDeltas[EdgeIndex] = SegmentEdgeN;
		EdgeEdgePs[EdgeIndex] = EdgeP;
		EdgeSegmentPs[EdgeIndex] = SegmentP;
		EdgeEdgeTs[EdgeIndex] = EdgeT;
		EdgeSegmentTs[EdgeIndex] = SegmentT;
		EdgeDistSqs[EdgeIndex] = SegmentEdgeDistSq;
		EdgeDistSigns[EdgeIndex] = SegmentEdgeDistSign;
		EdgeDotFace[EdgeIndex] = DotFace;
	}

	bool bCollided0 = false;
	bool bCollided1 = false;
	// 음수면 내부에 있다는 뜻
	const bool bInsideAll0 = ((EdgeD0s[0] <= DistanceTolerance) & (EdgeD0s[1] <= DistanceTolerance) & (EdgeD0s[2] <= DistanceTolerance)) != 0;
	const bool bInsideAll1 = ((EdgeD1s[0] <= DistanceTolerance) & (EdgeD1s[1] <= DistanceTolerance) & (EdgeD1s[2] <= DistanceTolerance)) != 0;
	bool isCol = false;

	if ((bInsideAll0 & (FaceD0 < RejectDistance) & (FaceD0 < FaceD1 + DistanceTolerance)) != 0)
	{
		ContactPoint stContactPoint;
		stContactPoint.ShapeContactPoints[0] = P0 - R * FaceN;
		stContactPoint.ShapeContactPoints[1] = P0 - FaceD0 * FaceN;
		stContactPoint.ShapeContactNormal = FaceN;
		stContactPoint.Phi = FaceD0 - R;
		stContactPoint.ContactType = ContactPointType::VertexPlane;
		stContactPoint.FaceIndex = 0;
		bCollided0 = true;
		m_vecPoints.push_back(stContactPoint);

		if (stContactPoint.Phi < 0) isCol = true;
	}
	if ((bInsideAll1 & (FaceD1 < RejectDistance) & (FaceD1 < FaceD0 + DistanceTolerance)) != 0)
	{
		ContactPoint stContactPoint;
		stContactPoint.ShapeContactPoints[0] = P1 - R * FaceN;
		stContactPoint.ShapeContactPoints[1] = P1 - FaceD1 * FaceN;
		stContactPoint.ShapeContactNormal = FaceN;
		stContactPoint.Phi = FaceD1 - R;
		stContactPoint.ContactType = ContactPointType::VertexPlane;
		stContactPoint.FaceIndex = 0;
		bCollided1 = true;
		m_vecPoints.push_back(stContactPoint);

		if (stContactPoint.Phi < 0) isCol = true;
	}
	if (m_vecPoints.size() == 2)
	{
		return isCol;
	}

	// Handle the parallel segment-edge case by clipping the segment to the edge
	// NOTE: If we are parallel to an edge (and outside the edge prism) we can only collide with the one edge
	bool bIsParallelEdge[3];
	const bool bEqualEdgeDist0 = XMScalarNearEqual(EdgeD0s[0], EdgeD1s[0], DistanceTolerance);
	const bool bEqualEdgeDist1 = XMScalarNearEqual(EdgeD0s[1], EdgeD1s[1], DistanceTolerance);
	const bool bEqualEdgeDist2 = XMScalarNearEqual(EdgeD0s[2], EdgeD1s[2], DistanceTolerance);
	bIsParallelEdge[0] = ((EdgeD0s[0] >= (0.f)) & (EdgeD1s[0] >= (0.f)) & bEqualEdgeDist0) != 0;
	bIsParallelEdge[1] = ((EdgeD0s[1] >= (0.f)) & (EdgeD1s[1] >= (0.f)) & bEqualEdgeDist1) != 0;
	bIsParallelEdge[2] = ((EdgeD0s[2] >= (0.f)) & (EdgeD1s[2] >= (0.f)) & bEqualEdgeDist2) != 0;
	EdgeVertexIndex0 = 2;
	if ((bIsParallelFace & (bIsParallelEdge[0] | bIsParallelEdge[1] | bIsParallelEdge[2])) != 0)
	{
		for (int  EdgeIndex = 0; EdgeIndex < 3; ++EdgeIndex)
		{
			const int EdgeVertexIndex1 = EdgeIndex;
			const _vector& EdgeP0 = vVertex[EdgeVertexIndex0];
			const _vector& EdgeP1 = vVertex[EdgeVertexIndex1];
			EdgeVertexIndex0 = EdgeVertexIndex1;
			if (bIsParallelEdge[EdgeIndex])
			{
				return AddCapsuleTriangleParallelEdgeManifoldContacts(P0, P1, EdgeP0, EdgeP1, R, RejectDistanceSq, NormalToleranceSq);
			}
		}
	}
	
	// Sine of angle between segment axis and the triangle face
	const _float AxisDotNormal = XMVectorGetX(XMVector3Dot(Axis, FaceN));
	const _float SinAxisFaceAngle = AxisDotNormal;
	const bool bPreferFaceContact = (abs(SinAxisFaceAngle) < FaceContactSinAngleThreshold);

	// Generate contacts for the cylinder ends near the face. Only consider the ends where we did not generate an end-cap contact, 
	// and only when we are at a low angle to the face. 
	// The cylinder points can only be inside the edge planes if the segment points are within Radius of the edge planes
	const bool bNearAll0 = ((EdgeD0s[0] <= R + DistanceTolerance) & (EdgeD0s[1] <= R + DistanceTolerance) & (EdgeD0s[2] <= R + DistanceTolerance)) != 0;
	const bool bNearAll1 = ((EdgeD1s[0] <= R + DistanceTolerance) & (EdgeD1s[1] <= R + DistanceTolerance) & (EdgeD1s[2] <= R + DistanceTolerance)) != 0;
	const bool bCheckCylinder0 = ((!bCollided0) & bPreferFaceContact & bNearAll0) != 0;
	const bool bCheckCylinder1 = ((!bCollided1) & bPreferFaceContact & bNearAll1) != 0;

	if ((bCheckCylinder0 | bCheckCylinder1) != 0)
	{
		_vector RadialAxis = XMVector3Cross(XMVector3Cross(Axis, FaceN), Axis);
		if (XMVectorGetX(XMVector3Length(RadialAxis)) >= NormalTolerance)
		{
			RadialAxis = XMVector3Normalize(RadialAxis);

			// We want Radial axis to point against the normal
			if (XMVectorGetX(XMVector3Dot(RadialAxis, FaceN)) > (0.f))
			{
				RadialAxis = -RadialAxis;
			}

			// Utility to add a cylinder contact point, if it is within the edge planes
			const auto& TryAddCylinderContact = [R, &RadialAxis, &V0, &V1, &V2, &FaceN, &FaceP, &EdgeNs, DistanceTolerance](const _vector& P, vector<ContactPoint>& vecPoints) -> void
			{
				const _vector CylinderP = P + R * RadialAxis;
				const _float CylinderEdgeD0 = XMVectorGetX(XMVector3Dot(CylinderP - V0, EdgeNs[0]));
				const _float CylinderEdgeD1 = XMVectorGetX(XMVector3Dot(CylinderP - V1, EdgeNs[1]));
				const _float CylinderEdgeD2 = XMVectorGetX(XMVector3Dot(CylinderP - V2, EdgeNs[2]));
				const bool bCylinderInsideAll = ((CylinderEdgeD0 <= DistanceTolerance) & (CylinderEdgeD1 <= DistanceTolerance) & (CylinderEdgeD2 <= DistanceTolerance)) != 0;
				if (bCylinderInsideAll)
				{
					const _float CylinderFaceD = XMVectorGetX(XMVector3Dot(CylinderP - FaceP, FaceN));

					ContactPoint stContactPoint;
					stContactPoint.ShapeContactPoints[0] = CylinderP;
					stContactPoint.ShapeContactPoints[1] = CylinderP - CylinderFaceD * FaceN;
					stContactPoint.ShapeContactNormal = FaceN;
					stContactPoint.Phi = CylinderFaceD;
					stContactPoint.ContactType = ContactPointType::VertexPlane;
					stContactPoint.FaceIndex = 0;
					vecPoints.push_back(stContactPoint);
				}
			};

			if (bCheckCylinder0)
			{
				int iSize = m_vecPoints.size();
				TryAddCylinderContact(P0, m_vecPoints);
				if ((m_vecPoints.size() > iSize) && (m_vecPoints[iSize].Phi < 0))
					isCol = true;
			}
			if (bCheckCylinder1)
			{
				int iSize = m_vecPoints.size();
				TryAddCylinderContact(P1, m_vecPoints);
				if ((m_vecPoints.size() > iSize) && (m_vecPoints[iSize].Phi < 0))
					isCol = true;
			}
		}
	}
	if (m_vecPoints.size() == 2)
	{
		return isCol;
	}


	// Add edge contacts to the manifold
	EdgeVertexIndex0 = 2;
	for (int EdgeIndex = 0; EdgeIndex < 3; ++EdgeIndex)
	{
		const int EdgeVertexIndex1 = EdgeIndex;
		const _vector& EdgeP0 = vVertex[EdgeVertexIndex0];
		const _vector& EdgeP1 = vVertex[EdgeVertexIndex1];
		EdgeVertexIndex0 = EdgeVertexIndex1;

		// Reuse edge-segment data calculated in cull check above
		_vector SegmentEdgeN = EdgeSegmentDeltas[EdgeIndex];
		const _vector& EdgeP = EdgeEdgePs[EdgeIndex];
		const _vector& SegmentP = EdgeSegmentPs[EdgeIndex];
		const _float EdgeT = EdgeEdgeTs[EdgeIndex];
		const _float SegmentT = EdgeSegmentTs[EdgeIndex];
		const _float SegmentEdgeDistSq = EdgeDistSqs[EdgeIndex];
		const _float SegmentEdgeDistSign = EdgeDistSigns[EdgeIndex];
		const _float DotFace = EdgeDotFace[EdgeIndex];
	
		// We only care about edges if at least one capsule segment point is outside the edge plane
			// (internal points were handled already)
		if (((EdgeD0s[EdgeIndex] > -DistanceTolerance) | (EdgeD1s[EdgeIndex] > -DistanceTolerance)) != 0)
		{
			// Don't collide with inside face
			if (DotFace < (-NormalTolerance))
			{
				continue;
			}

			// We will create a face contact rather than an edge contact where possible.
			// When the angle between the axis and the face is below a threshold.
			const bool bInEdgeRange = InRangeOpen(EdgeT, (0.f), (1.f));
			const bool bInSegmentRange = InRangeOpen(SegmentT, (0.f), (1.f));
			const bool bCrossedEdgeSegment = ((bInEdgeRange & bInSegmentRange) != 0);

			// Calculate separation distance and normal
				// If we have zero separation, we cannot renormalize the separation vector so we must calculate the normal
			//캡슐 축이 엣지와 접촉하지 않은 경우
			//캡슐 축과 삼각형 엣지가 서로 분리된 상태라면, 두 선분 간의 가장 가까운 점을 기준으로 충돌 정보를 계산.
			_float SegmentEdgeDist;
			if (SegmentEdgeDistSq > NormalToleranceSq)
			{
				// Get the signed distance and separating axis
				SegmentEdgeDist = sqrt(SegmentEdgeDistSq);
				SegmentEdgeN = SegmentEdgeN / SegmentEdgeDist;
				SegmentEdgeDist *= SegmentEdgeDistSign;
			}
			// 캡슐 축이 엣지와 거의 접촉한 경우
			//축(segment)과 엣지가 거의 평행한 경우, 법선 벡터를 계산하여 충돌을 처리.
			else
			{
				// Segment passes right through edge - calculate normal
				SegmentEdgeDist = (0.f);
				SegmentEdgeN = XMVector3Cross(Axis, EdgeP1 - EdgeP0);
				if (XMVectorGetX(XMVector3Length(SegmentEdgeN)) < NormalToleranceSq)
					continue;
				
				if (XMVectorGetX(XMVector3Dot(SegmentEdgeN, FaceN)) < (0.f))
				{
					SegmentEdgeN = -SegmentEdgeN;
				}
				const _float DotEdge = XMVectorGetX(XMVector3Dot(EdgeNs[EdgeIndex], SegmentEdgeN));
				if (DotEdge < -NormalTolerance)
				{
					continue;
				}
			}
			
			if ((bCrossedEdgeSegment & bPreferFaceContact) == 0)
			{
				// We cannot collide with the inside of the edge
				// 엣지 내부충돌이면 무시
				const _float DotEdge = XMVectorGetX(XMVector3Dot(EdgeNs[EdgeIndex], SegmentEdgeN));
				if (DotEdge < -NormalTolerance)
				{
					continue;
				}

				// For Vertex contacts, check that the normal is in the valid range
				// it must point away from the edge vectors that share the vertex
				if (EdgeT == (0.f))
				{
					const int PrevEdgeVertexIndex0 = (EdgeIndex >= 2) ? (EdgeIndex - 2) : (EdgeIndex - 2 + 3);
					const _vector& PrevEdgeP0 = vVertex[PrevEdgeVertexIndex0];
					const _vector& PrevEdgeP1 = EdgeP0;

					// NOTE: both edge vetors here point towards the shared vertex (at EdgeP0)
					const _float PrevEdgeDotNormal = XMVectorGetX(XMVector3Dot(PrevEdgeP1 - PrevEdgeP0, SegmentEdgeN));
					const _float EdgeDotNormal = XMVectorGetX(XMVector3Dot(EdgeP0 - EdgeP1, SegmentEdgeN));
					if ((PrevEdgeDotNormal < -NormalTolerance) || (EdgeDotNormal < -NormalTolerance))
					{
						continue;
					}
				}
				if (EdgeT == (1.f))
				{
					const int NextEdgeVertexIndex1 = (EdgeIndex < 2) ? EdgeIndex + 1 : 0;
					const _vector& NextEdgeP0 = EdgeP1;
					const _vector& NextEdgeP1 = vVertex[NextEdgeVertexIndex1];

					// NOTE: both edge vetors here point towards the shared vertex (at EgdeP1)
					const _float EdgeDotNormal = XMVectorGetX(XMVector3Dot(EdgeP1 - EdgeP0, SegmentEdgeN));
					const _float NextEdgeDotNormal = XMVectorGetX(XMVector3Dot(NextEdgeP0 - NextEdgeP1, SegmentEdgeN));
					if ((EdgeDotNormal < -NormalTolerance) || (NextEdgeDotNormal < -NormalTolerance))
					{
						continue;
					}
				}
			}

			// If we are within the face angle tolerance, generate a face contact rather than an edge one
			// (but only if we have an edge contact and not a vertex one)
			// NOTE: we rely on the fact that the Ts will be exactly 0 or 1 when the near point is outside the segment or edge
			if ((bCrossedEdgeSegment & bPreferFaceContact) != 0)
			{
				const _vector CapsuleP = SegmentP - R * SegmentEdgeN;
				const _float CapsuleDist = XMVectorGetX(XMVector3Dot(CapsuleP - FaceP, FaceN));

				ContactPoint stContactPoint;
				stContactPoint.ShapeContactPoints[0] = CapsuleP;
				stContactPoint.ShapeContactPoints[1] = CapsuleP - CapsuleDist * FaceN;
				stContactPoint.ShapeContactNormal = FaceN;
				stContactPoint.Phi = CapsuleDist;
				stContactPoint.ContactType = ContactPointType::VertexPlane;
				stContactPoint.FaceIndex = 0;
				m_vecPoints.push_back(stContactPoint);

				if(stContactPoint.Phi < 0) isCol = true;				
			}
			else
			{
				ContactPoint stContactPoint;
				stContactPoint.ShapeContactPoints[0] = SegmentP - R * SegmentEdgeN;
				stContactPoint.ShapeContactPoints[1] = EdgeP;
				stContactPoint.ShapeContactNormal = SegmentEdgeN;
				stContactPoint.Phi = SegmentEdgeDist - R;
				stContactPoint.ContactType = ContactPointType::EdgeEdge;
				stContactPoint.FaceIndex = 0;
				m_vecPoints.push_back(stContactPoint);

				if (stContactPoint.Phi < 0) isCol = true;
			}
		}
	}

	return isCol;
}

void CBounding_Capsule::Capsule_Tranform(_fmatrix WorldMatrix)
{
	/* 캡슐 위치 갱신*/ 
	XMVECTOR vCenter = XMLoadFloat3(&m_pOriginalMyDesc.vCenter);
	XMVECTOR C = XMVector3Transform(vCenter, WorldMatrix);
	XMStoreFloat3(&m_pMyDesc.vCenter, C);

	_vector vUp = WorldMatrix.r[1];
	XMStoreFloat3(&m_pMyDesc.vDir, vUp);


	/* 렌더링용 구체 갱신*/
	XMVECTOR vTop = XMVectorAdd(XMLoadFloat3(&m_pMyDesc.vCenter), XMVectorScale(XMLoadFloat3(&m_pMyDesc.vDir), m_pMyDesc.fHeight));
	XMStoreFloat3(&m_pSphere_1->Center, vTop);
	XMVECTOR vBot = XMVectorAdd(XMLoadFloat3(&m_pMyDesc.vCenter), -(XMVectorScale(XMLoadFloat3(&m_pMyDesc.vDir), m_pMyDesc.fHeight)));
	XMStoreFloat3(&m_pSphere_2->Center, vBot);
}

_vector CBounding_Capsule::ClosestPointOnLineSegment(const _vector& point, const _vector& start, const _vector& end)
{
	XMVECTOR segDir = XMVectorSubtract(end, start);
	float segLengthSq = XMVectorGetX(XMVector3LengthSq(segDir));

	if (segLengthSq < FLT_EPSILON) {
		// 선분의 길이가 0이면, 시작점 반환
		return start;
	}

	// 점을 선분의 시작점으로 이동
	XMVECTOR d = XMVectorSubtract(point, start);
	float t = XMVectorGetX(XMVector3Dot(d, segDir)) / segLengthSq;

	// t를 0 ~ 1로 클램핑
	t = std::clamp(t, 0.0f, 1.0f);

	// 선분 상의 가장 가까운 점 계산
	return XMVectorAdd(start, XMVectorScale(segDir, t));
}

_vector CBounding_Capsule::ClosestPointBetweenSegments(const _vector& A1, const _vector& A2, const _vector& B1, const _vector& B2)
{
	_vector dA = A2 - A1; // 선분 A의 방향 벡터
	_vector dB = B2 - B1; // 선분 B의 방향 벡터
	_vector r = A1 - B1;  // 두 선분의 시작점 차이

	float a = XMVectorGetX(XMVector3Dot(dA, dA)); // 선분 A의 길이 제곱
	float b = XMVectorGetX(XMVector3Dot(dA, dB)); // 선분 A와 B의 내적
	float c = XMVectorGetX(XMVector3Dot(dB, dB)); // 선분 B의 길이 제곱
	float d = XMVectorGetX(XMVector3Dot(dA, r));  // 선분 A와 r의 내적
	float e = XMVectorGetX(XMVector3Dot(dB, r));  // 선분 B와 r의 내적

	float denominator = a * c - b * b; // 선분 간의 교차 여부를 확인하는 분모

	float t, s;

	// 분모가 0인 경우 선분이 평행
	if (fabs(denominator) < 1e-6f) {
		t = 0.0f; // 선분 A의 임의 점
		s = e / c; // 선분 B의 가장 가까운 점
		s = std::clamp(s, 0.0f, 1.0f); // s를 [0, 1]로 클램프
	}
	else {
		// 일반적인 경우 (평행하지 않음)
		t = (b * e - c * d) / denominator;
		s = (a * e - b * d) / denominator;

		// t와 s를 [0, 1]로 클램프
		t = std::clamp(t, 0.0f, 1.0f);
		s = std::clamp(s, 0.0f, 1.0f);
	}

	// 두 선분에서 가장 가까운 점
	_vector closestPointA = A1 + t * dA; // 선분 A의 가장 가까운 점
	_vector closestPointB = B1 + s * dB; // 선분 B의 가장 가까운 점

	// 최단 거리 벡터 반환
	return closestPointA - closestPointB;
}

bool CBounding_Capsule::GetIntersectionPoint(_vector A1, _vector d1, _vector A2, _vector d2, _vector& intersection)
{
	// 선분 간 차이 벡터
	XMVECTOR diff = A2 - A1;

	// 두 방향 벡터의 외적 계산
	XMVECTOR cross_d1_d2 = XMVector3Cross(d1, d2);
	float det = XMVectorGetX(XMVector3Length(cross_d1_d2)); // 외적의 크기(결정 요인)

	// 방향 벡터가 평행하거나 같은 직선에 존재하는 경우
	if (fabs(det) < 1e-6f) { // 충분히 작은 값(기계 오차 보정)
		return false;
	}

	// 크로스 곱을 활용하여 매개변수 t와 u 계산
	XMVECTOR cross_diff_d2 = XMVector3Cross(diff, d2);
	float t = XMVectorGetX(XMVector3Dot(cross_diff_d2, cross_d1_d2)) / det;

	XMVECTOR cross_diff_d1 = XMVector3Cross(diff, d1);
	float u = XMVectorGetX(XMVector3Dot(cross_diff_d1, cross_d1_d2)) / det;

	// t와 u가 [0, 1] 범위에 있어야 선분이 실제로 교차
	if (t < 0.0f || t > 1.0f || u < 0.0f || u > 1.0f) {
		return false;
	}

	// 교차점 계산
	intersection = A1 + t * d1;
	return true;
}

float CBounding_Capsule::SegmentSegmentDistanceSq(XMVECTOR p1, XMVECTOR q1, XMVECTOR p2, XMVECTOR q2, XMVECTOR& vNormal)
{
	XMVECTOR d1 = XMVectorSubtract(q1, p1); // 선분 1의 방향
	XMVECTOR d2 = XMVectorSubtract(q2, p2); // 선분 2의 방향
	XMVECTOR r = XMVectorSubtract(p1, p2);

	float a = XMVectorGetX(XMVector3Dot(d1, d1)); // d1·d1
	float e = XMVectorGetX(XMVector3Dot(d2, d2)); // d2·d2
	float f = XMVectorGetX(XMVector3Dot(d2, r));  // d2·r

	float s = 0.0f, t = 0.0f;

	// 두 선분 모두 점인 경우
	if (a <= FLT_EPSILON && e <= FLT_EPSILON) {
		return XMVectorGetX(XMVector3Length(r));
	}

	// 선분 1이 점인 경우
	if (a <= FLT_EPSILON) {
		t = std::clamp(f / e, 0.0f, 1.0f);
	}
	// 선분 2가 점인 경우
	else if (e <= FLT_EPSILON) {
		s = std::clamp(XMVectorGetX(XMVector3Dot(d1, r)) / a, 0.0f, 1.0f);
	}
	// 일반적인 경우
	else {
		float c = XMVectorGetX(XMVector3Dot(d1, r)); // d1·r
		float b = XMVectorGetX(XMVector3Dot(d1, d2)); // d1·d2

		float denom = a * e - b * b;

		if (denom != 0.0f) {
			s = std::clamp((b * f - c * e) / denom, 0.0f, 1.0f);
		}

		t = (b * s + f) / e;

		// t가 범위를 벗어나면 다시 s를 조정
		if (t < 0.0f) {
			t = 0.0f;
			s = std::clamp(-c / a, 0.0f, 1.0f);
		}
		else if (t > 1.0f) {
			t = 1.0f;
			s = std::clamp((b - c) / a, 0.0f, 1.0f);
		}
	}

	XMVECTOR closestPoint1 = XMVectorAdd(p1, XMVectorScale(d1, s));
	XMVECTOR closestPoint2 = XMVectorAdd(p2, XMVectorScale(d2, t));

	vNormal = XMVectorSubtract(closestPoint1, closestPoint2);
	return XMVectorGetX(XMVector3LengthSq(XMVectorSubtract(closestPoint1, closestPoint2)));
}

void CBounding_Capsule::NearestPointsOnLineSegments(const XMVECTOR& P0, const XMVECTOR& P1, const XMVECTOR& EdgeP0, const XMVECTOR& EdgeP1, float& SegmentT, float& EdgeT, XMVECTOR& SegmentP, XMVECTOR& EdgeP)
{
	// 두 선분의 방향 벡터
	XMVECTOR d1 = P1 - P0; // 첫 번째 선분의 방향
	XMVECTOR d2 = EdgeP1 - EdgeP0; // 두 번째 선분의 방향
	XMVECTOR r = P0 - EdgeP0;

	// 길이 제곱 (normalize 하지 않음)
	float a = XMVectorGetX(XMVector3Dot(d1, d1)); // |d1|^2
	float e = XMVectorGetX(XMVector3Dot(d2, d2)); // |d2|^2
	float f = XMVectorGetX(XMVector3Dot(d2, r));

	// s와 t를 계산하기 위한 초기화
	float s = 0.0f; // 첫 번째 선분의 매개변수
	float t = 0.0f; // 두 번째 선분의 매개변수

	// 선분들이 거의 점에 가까운 경우
	if (a <= 1.e-4f && e <= 1.e-4f)
	{
		// 두 선분이 사실상 점에 가까움
		SegmentT = 0.0f;
		EdgeT = 0.0f;
		SegmentP = P0;
		EdgeP = EdgeP0;
		return;
	}

	// 첫 번째 선분이 점인 경우
	if (a <= 1.e-4f)
	{
		SegmentT = 0.0f;
		t =  clamp(f / e, 0.0f, 1.0f);
	}
	else
	{
		float c = XMVectorGetX(XMVector3Dot(d1, r));
		if (e <= 1.e-4f)
		{
			// 두 번째 선분이 점인 경우
			t = 0.0f;
			s = clamp(-c / a, 0.0f, 1.0f);
		}
		else
		{
			// 두 선분 모두 유효한 길이를 가질 경우
			float b = XMVectorGetX(XMVector3Dot(d1, d2));
			float denom = a * e - b * b; // 분모

			// 분모가 0에 가까운 경우, 두 선분이 거의 평행함
			if (denom != 0.0f)
			{
				s =clamp((b * f - c * e) / denom, 0.0f, 1.0f);
			}
			else
			{
				s = 0.0f;
			}

			t = (b * s + f) / e;

			// t를 [0, 1] 범위로 클램핑
			if (t < 0.0f)
			{
				t = 0.0f;
				s = clamp(-c / a, 0.0f, 1.0f);
			}
			else if (t > 1.0f)
			{
				t = 1.0f;
				s = clamp((b - c) / a, 0.0f, 1.0f);
			}
		}
	}

	// 최종적으로 계산된 s와 t를 사용해 점을 계산
	SegmentT = s;
	EdgeT = t;

	SegmentP = P0 + s * d1;
	EdgeP = EdgeP0 + t * d2;
}

_bool CBounding_Capsule::AddCapsuleTriangleParallelEdgeManifoldContacts(const XMVECTOR& P0, const XMVECTOR& P1, const XMVECTOR& EdgeP0, const XMVECTOR& EdgeP1, const _float R, const _float RejectDistanceSq, const _float NormalToleranceSq)
{
	// Utility to add a contact to the array if it is within cull distance
	const auto& AddContact = [](const _vector& SegmentEdgeC, const _vector& SegmentEdgeDelta, const _float R, const _float RejectDistanceSq, const _float NormalToleranceSq, vector<ContactPoint>& vecPoints) -> void
	{
		const _float SegmentEdgeDistSq = XMVectorGetX(XMVector3LengthSq(SegmentEdgeDelta));
		if ((SegmentEdgeDistSq < RejectDistanceSq) && (SegmentEdgeDistSq > NormalToleranceSq))
		{
			const _float	SegmentEdgeDist = sqrt(SegmentEdgeDistSq);
			const _vector	SegmentEdgeN = SegmentEdgeDelta / SegmentEdgeDist;

			ContactPoint stContactPoint;
			stContactPoint.ShapeContactPoints[0] = SegmentEdgeC + (SegmentEdgeDist - R) * SegmentEdgeN;
			stContactPoint.ShapeContactPoints[1] = SegmentEdgeC;
			stContactPoint.ShapeContactNormal = SegmentEdgeN;
			stContactPoint.Phi = SegmentEdgeDist - R;
			stContactPoint.ContactType = ContactPointType::EdgeEdge;
			stContactPoint.FaceIndex = 0;
			vecPoints.push_back(stContactPoint);
		}
	};

	const _vector EdgeDelta = EdgeP1 - EdgeP0;
	const _float EdgeLenSq = XMVectorGetX(XMVector3LengthSq(EdgeDelta));
	bool isCol = false;
	if (EdgeLenSq > NormalToleranceSq)
	{
		// Project the first segment point onto the edge
		const _float T0 = XMVectorGetX(XMVector3Dot(P0 - EdgeP0, EdgeDelta)) / EdgeLenSq;
		const _float T1 = XMVectorGetX(XMVector3Dot(P1 - EdgeP0, EdgeDelta)) / EdgeLenSq;

		// If both points have T < 0, only use the point with highest T
		// If both points have T > 1, only use the point with lowest T
		// Otherwise use both points with T clamped to [0,1]
		const bool bInRange0 = InRangeClosed(T0, (0.f), (1.f));
		const bool bInRange1 = InRangeClosed(T1, (0.f), (1.f));
		if ((bInRange0 | bInRange1) != 0)
		{
			// At least one segment point projects to a point inside the edge extents.
			// Clip the edge-mapped segement and use the clipped verts as contacts.

			const _vector SegmentEdgeDelta0 = P0 - (EdgeP0 + T0 * EdgeDelta);
			const _vector SegmentEdgeC0 = EdgeP0 + clamp(T0, (0.f), (1.f)) * EdgeDelta;
			AddContact(SegmentEdgeC0, SegmentEdgeDelta0, R, RejectDistanceSq, NormalToleranceSq, m_vecPoints);
			if (m_vecPoints[m_vecPoints.size() - 1].Phi < 0) isCol = true;

			const _vector SegmentEdgeDelta1 = P1 - (EdgeP0 + T1 * EdgeDelta);
			const _vector SegmentEdgeC1 = EdgeP0 + clamp(T1, (0.f), (1.f)) * EdgeDelta;
			AddContact(SegmentEdgeC1, SegmentEdgeDelta1, R, RejectDistanceSq, NormalToleranceSq, m_vecPoints);
			if (m_vecPoints[m_vecPoints.size() - 1].Phi < 0) isCol = true;
		}
		else
		{
			// Both segment points projected to the edge are outside the edge extents.
			// Use the segment point nearest the edge as the contact.

			_float SegmentEdgeT;
			_vector SegmentEdgeP;
			if ((T0 < (0.f)) && (T1 < (0.f)))
			{
				SegmentEdgeT = (T0 > T1) ? T0 : T1;
				SegmentEdgeP = (T0 > T1) ? P0 : P1;
			}
			else
			{
				SegmentEdgeT = (T0 < T1) ? T0 : T1;
				SegmentEdgeP = (T0 < T1) ? P0 : P1;
			}
			const _vector SegmentEdgeC = EdgeP0 + clamp(SegmentEdgeT, (0.f), (1.f)) * EdgeDelta;
			const _vector SegmentEdgeDelta = (SegmentEdgeP - SegmentEdgeC);
			AddContact(SegmentEdgeC, SegmentEdgeDelta, R, RejectDistanceSq, NormalToleranceSq, m_vecPoints);
			if (m_vecPoints[m_vecPoints.size() - 1].Phi < 0) isCol = true;
		}
	}
	return isCol;
}

CBounding_Capsule* CBounding_Capsule::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CBounding_Capsule* pInstance = new CBounding_Capsule(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CBounding_Capsule");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBounding_Capsule::Free()
{
	__super::Free();

	Safe_Delete(m_pSphere_1);
	Safe_Delete(m_pSphere_2);
}
