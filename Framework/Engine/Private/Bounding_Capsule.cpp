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
	_vector vOBBCenter = XMLoadFloat3(&pTargetBounding->Get_MyDesc()->_Center);
	_vector vOBBExtents = XMLoadFloat3(&pTargetBounding->Get_MyDesc()->_Extents);
	_vector vOBBOrientation = XMLoadFloat4(&pTargetBounding->Get_MyDesc()->_Orientation);

	// 캡슐의 두 점을 OBB 좌표계로 변환
	_vector	vTop = XMVectorAdd(XMLoadFloat3(&m_pMyDesc.vCenter), XMVectorScale(XMLoadFloat3(&m_pMyDesc.vDir), m_pMyDesc.fHeight));
	_vector	vBot = XMVectorAdd(XMLoadFloat3(&m_pMyDesc.vCenter), -(XMVectorScale(XMLoadFloat3(&m_pMyDesc.vDir), m_pMyDesc.fHeight)));

	_vector vOBBSpaceTop = XMVector3InverseRotate(XMVectorSubtract(vTop, vOBBCenter), vOBBOrientation);
	_vector vOBBSpaceBot = XMVector3InverseRotate(XMVectorSubtract(vBot, vOBBCenter), vOBBOrientation);

	// 캡슐의 선분과 OBB의 최단 거리 계산
	_vector clampedTop = XMVectorClamp(vOBBSpaceTop, XMVectorNegate(vOBBExtents), vOBBExtents);
	_vector clampedBot = XMVectorClamp(vOBBSpaceBot, XMVectorNegate(vOBBExtents), vOBBExtents);

	// OBB내부로 클램프한 Top과 Bot에서 캡슐 선분위의 가장 가까운 점 계산
	_vector vClosestPoint1 = ClosestPointOnLineSegment(clampedTop, vOBBSpaceTop, vOBBSpaceBot);
	_vector vClosestPoint2 = ClosestPointOnLineSegment(clampedBot, vOBBSpaceTop, vOBBSpaceBot);

	//두번쨰 클램프
	_vector clampedTop2 = XMVectorClamp(vClosestPoint1, XMVectorNegate(vOBBExtents), vOBBExtents);
	_vector clampedBot2 = XMVectorClamp(vClosestPoint2, XMVectorNegate(vOBBExtents), vOBBExtents);

	// 첫번째 클램프와 두번째 클램프를 이용한 방향 벡터
	_vector DirClampedTop = XMVectorSubtract(clampedTop2, clampedTop);
	_vector DirClampedBot = XMVectorSubtract(clampedBot2, clampedBot);

	_vector vPoint = XMVectorZero();

	if (GetIntersectionPoint(clampedTop, DirClampedTop, clampedBot, DirClampedBot, vPoint))
	{
		_vector vClosestPoint = ClosestPointOnLineSegment(vPoint, vOBBSpaceTop, vOBBSpaceBot);
		_vector delta = XMVectorSubtract(vClosestPoint, vPoint);
		float distSquared = XMVectorGetX(XMVector3LengthSq(delta));

		if (distSquared <= (m_pMyDesc.fRadius * m_pMyDesc.fRadius))
			return true;
		else
			return false;
	}

	// 클램핑된 점과 원래 점 사이의 벡터를 계산하여 최단 거리 구함
	_vector deltaTop = XMVectorSubtract(vClosestPoint1, clampedTop);
	_vector deltaBot = XMVectorSubtract(vClosestPoint2, clampedBot);

	// 두 점 중 최소 거리를 선택
	float distSquaredTop = XMVectorGetX(XMVector3LengthSq(deltaTop));
	float distSquaredBot = XMVectorGetX(XMVector3LengthSq(deltaBot));
	float minDistSquared = min(distSquaredTop, distSquaredBot);

	// 캡슐의 반지름과 비교하여 충돌 여부 판단
	return (minDistSquared <= (m_pMyDesc.fRadius * m_pMyDesc.fRadius));
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
