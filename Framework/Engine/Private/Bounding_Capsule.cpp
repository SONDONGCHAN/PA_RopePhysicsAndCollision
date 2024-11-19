#include "Bounding_Capsule.h"
#include "Bounding_Sphere.h"
#include "Bounding_OBB.h"
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
	_vector vOBBCenter		= XMLoadFloat3(&pTargetBounding->Get_MyDesc()->_Center);
	_vector vOBBExtents		= XMLoadFloat3(&pTargetBounding->Get_MyDesc()->_Extents);
	_vector vOBBOrientation = XMLoadFloat4(&pTargetBounding->Get_MyDesc()->_Orientation);

	_vector	vTop = XMVectorAdd(XMLoadFloat3(&m_pMyDesc.vCenter), XMVectorScale(XMLoadFloat3(&m_pMyDesc.vDir), m_pMyDesc.fHeight));
	_vector	vBot = XMVectorAdd(XMLoadFloat3(&m_pMyDesc.vCenter), -(XMVectorScale(XMLoadFloat3(&m_pMyDesc.vDir), m_pMyDesc.fHeight)));

	_vector obbAxes[3];
	pTargetBounding->GetOBBAxes(obbAxes[0], obbAxes[1], obbAxes[2]);
	_float closestDistanceSquared = FLT_MAX;
	
	// 1. 캡슐 축과 OBB 면 간 거리 계산
	for (int i = 0; i < 3; ++i) 
	{
		_vector planeNormal = obbAxes[i]; // OBB 축 방향(법선 벡터)
	
		// 면의 위치를 OBB의 Extents를 반영하여 계산
		float planeDistancePos = XMVectorGetX(XMVector3Dot(vOBBCenter, planeNormal)) + vOBBExtents.m128_f32[i];
		float planeDistanceNeg = XMVectorGetX(XMVector3Dot(vOBBCenter, planeNormal)) - vOBBExtents.m128_f32[i];
	
		// 캡슐 축 끝점(t1: vTop, t2: vBot)과 OBB의 +Extents 면까지의 거리
		float t1Pos = XMVectorGetX(XMVector3Dot(vTop, planeNormal)) - planeDistancePos;
		float t2Pos = XMVectorGetX(XMVector3Dot(vBot, planeNormal)) - planeDistancePos;
	
		// 캡슐 축 끝점(t1: vTop, t2: vBot)과 OBB의 -Extents 면까지의 거리
		float t1Neg = XMVectorGetX(XMVector3Dot(vTop, planeNormal)) - planeDistanceNeg;
		float t2Neg = XMVectorGetX(XMVector3Dot(vBot, planeNormal)) - planeDistanceNeg;
		
		// 각 끝점이 OBB 면 경계 안에 있는지 확인
		bool outsideBoundary = false;

		for (int j = 0; j < 3; ++j) 
		{
			if (j == i) 
				continue; // 현재 축(i)에 대해 수직인 축만 확인

			// 캡슐 끝점의 투영 값
			float extentProjTop = XMVectorGetX(XMVector3Dot(vTop, obbAxes[j])) - XMVectorGetX(XMVector3Dot(vOBBCenter, obbAxes[j]));
			float extentProjBot = XMVectorGetX(XMVector3Dot(vBot, obbAxes[j])) - XMVectorGetX(XMVector3Dot(vOBBCenter, obbAxes[j]));

			// OBB 면의 경계(Extents)와 비교
			float extentLimit = vOBBExtents.m128_f32[j];
			if (fabs(extentProjTop) > extentLimit || fabs(extentProjBot) > extentLimit) 
			{
				outsideBoundary = true; // 경계 밖에 있음
				break; // 내부 루프 종료
			}
		}

		if (outsideBoundary) {
			continue; // 외부 루프를 건너뜀
		}
		// 각 면에서 가장 가까운 거리 계산
		float closestPlaneDistancePos = min(fabs(t1Pos), fabs(t2Pos));
		float closestPlaneDistanceNeg = min(fabs(t1Neg), fabs(t2Neg));
	
		// 가장 가까운 거리 선택
		float closestPlaneDistance = min(closestPlaneDistancePos, closestPlaneDistanceNeg);
	
		// 최단 거리 제곱 업데이트
		closestDistanceSquared = min(closestDistanceSquared, closestPlaneDistance * closestPlaneDistance);
	}
	
	// 2. 캡슐 축과 OBB 모서리 간 거리 계산
	for (int i = 0; i < 3; ++i) 
	{
		for (int j = -1; j <= 1; j += 2) 
		{
			for (int k = -1; k <= 1; k += 2)
			{
				// OBB 모서리의 시작점과 끝점 계산
				_vector edgeStart = vOBBCenter + obbAxes[i] * vOBBExtents.m128_f32[i] * j +
					obbAxes[(i + 1) % 3] * vOBBExtents.m128_f32[(i + 1) % 3] * k;
	
				_vector edgeDirection = obbAxes[(i + 2) % 3] * (2 * vOBBExtents.m128_f32[(i + 2) % 3]);
	
				// 선분 간 최단 거리 계산
				_vector closestPointBetweenEdges = ClosestPointBetweenSegments(vTop, vBot, edgeStart, edgeStart + edgeDirection);
				closestDistanceSquared = min(closestDistanceSquared, XMVectorGetX(XMVector3LengthSq(closestPointBetweenEdges)));
			}
		}
	}
	
	// 3. 캡슐 축과 OBB 꼭짓점 간 거리 계산
	for (int i = -1; i <= 1; i += 2) 
	{
		for (int j = -1; j <= 1; j += 2) 
		{
			for (int k = -1; k <= 1; k += 2) 
			{
				// OBB 꼭짓점 계산
				_vector corner = vOBBCenter + obbAxes[0] * vOBBExtents.m128_f32[0] * i +
					obbAxes[1] * vOBBExtents.m128_f32[1] * j +
					obbAxes[2] * vOBBExtents.m128_f32[2] * k;
	
				// 점과 선분 간 최단 거리 계산
				_vector closestPointToCorner = ClosestPointOnLineSegment(vTop, vBot, corner);
				closestDistanceSquared = min(closestDistanceSquared, XMVectorGetX(XMVector3LengthSq(closestPointToCorner)));
			}
		}
	}
	
	
	if (closestDistanceSquared <= m_pMyDesc.fRadius * m_pMyDesc.fRadius) {
		return true; // 충돌 발생
	}


	//_vector vOBBSpaceTop = XMVector3InverseRotate(XMVectorSubtract(vTop, vOBBCenter), vOBBOrientation);
	//_vector vOBBSpaceBot = XMVector3InverseRotate(XMVectorSubtract(vBot, vOBBCenter), vOBBOrientation);
	//_vector	vOBBSpaceCapsuleAxis = XMVectorSubtract(vOBBSpaceBot, vOBBSpaceTop);

	//_float t = XMVectorGetX(XMVector3Dot(-vOBBSpaceTop, vOBBSpaceCapsuleAxis)) /
	//	XMVectorGetX(XMVector3Dot(vOBBSpaceCapsuleAxis, vOBBSpaceCapsuleAxis));
	//t = std::clamp(t, 0.0f, 1.0f); 

	//_vector closestPointOnCapsule = XMVectorAdd(vOBBSpaceTop, XMVectorScale(vOBBSpaceCapsuleAxis, t));

	//_vector closestPointOnOBB = XMVectorClamp(
	//	closestPointOnCapsule,
	//	-vOBBExtents,
	//	+vOBBExtents
	//);

	//XMVECTOR distanceVec = XMVectorSubtract(closestPointOnCapsule, closestPointOnOBB);
	//float distanceSquared = XMVectorGetX(XMVector3LengthSq(distanceVec));

	//if (distanceSquared > m_pMyDesc.fRadius * m_pMyDesc.fRadius) 
	//{
	//	return false;
	//}

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

float CBounding_Capsule::SquaredDistanceBetweenPoints(const _vector& point1, const _vector& point2)
{
	XMVECTOR diff = XMVectorSubtract(point1, point2);
	return XMVectorGetX(XMVector3LengthSq(diff));
}

_vector CBounding_Capsule::ClosestPointOnLineSegment(const _vector& start, const _vector& end, const _vector& point)
{
	XMVECTOR lineVec = XMVectorSubtract(end, start);
	XMVECTOR pointVec = XMVectorSubtract(point, start);
	float t = XMVectorGetX(XMVector3Dot(pointVec, lineVec)) / XMVectorGetX(XMVector3LengthSq(lineVec));
	t = clamp(t, 0.0f, 1.0f);

	return XMVectorAdd(start, XMVectorScale(lineVec, t));
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
