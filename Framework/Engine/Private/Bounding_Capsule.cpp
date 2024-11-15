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
	//// OBB의 중심과 크기
	//XMVECTOR vOBBCenter = XMLoadFloat3(&pTargetBounding->Get_MyDesc()->_Center);
	//XMVECTOR vOBBExtents = XMLoadFloat3(&pTargetBounding->Get_MyDesc()->_Extents);

	//// 캡슐의 상단과 하단 점
	//XMVECTOR vTop = XMVectorAdd(XMLoadFloat3(&m_pMyDesc.vCenter), XMVectorScale(XMLoadFloat3(&m_pMyDesc.vDir), m_pMyDesc.fHeight));
	//XMVECTOR vBot = XMVectorAdd(XMLoadFloat3(&m_pMyDesc.vCenter), -(XMVectorScale(XMLoadFloat3(&m_pMyDesc.vDir), m_pMyDesc.fHeight)));

	//// 캡슐의 중심 축 벡터
	//XMVECTOR capsuleAxis = XMVectorSubtract(vBot, vTop);

	//// OBB의 축 정보
	//XMVECTOR obbXAxis, obbYAxis, obbZAxis;
	//pTargetBounding->GetOBBAxes(obbXAxis, obbYAxis, obbZAxis);

	//// 분리축 정리를 위한 축들
	//XMVECTOR Axes[7] = {
	//	obbXAxis, obbYAxis, obbZAxis,
	//	capsuleAxis,
	//	XMVector3Cross(capsuleAxis, obbXAxis),
	//	XMVector3Cross(capsuleAxis, obbYAxis),
	//	XMVector3Cross(capsuleAxis, obbZAxis)
	//};

	//// OBB와 캡슐의 충돌 검사 (분리축 검사)
	//for (int i = 0; i < 7; ++i) {
	//	XMVECTOR Axis = Axes[i];

	//	// 캡슐의 투영 거리 계산
	//	XMVECTOR capsuleStartProj = XMVector3Dot(vTop, Axis);
	//	XMVECTOR capsuleEndProj = XMVector3Dot(vBot, Axis);
	//	float capsuleMin = XMVectorGetX(XMVectorMin(capsuleStartProj, capsuleEndProj)) - m_pMyDesc.fRadius;
	//	float capsuleMax = XMVectorGetX(XMVectorMax(capsuleStartProj, capsuleEndProj)) + m_pMyDesc.fRadius;

	//	// OBB의 투영 거리 계산
	//	XMVECTOR obbCenterProj = XMVector3Dot(vOBBCenter, Axis);
	//	float obbRadius = fabsf(XMVectorGetX(XMVector3Dot(vOBBExtents, obbXAxis))) +
	//		fabsf(XMVectorGetX(XMVector3Dot(vOBBExtents, obbYAxis))) +
	//		fabsf(XMVectorGetX(XMVector3Dot(vOBBExtents, obbZAxis)));
	//	float obbMin = XMVectorGetX(obbCenterProj) - obbRadius;
	//	float obbMax = XMVectorGetX(obbCenterProj) + obbRadius;

	//	// 분리축에서 충돌 여부 판단 (간격이 존재하면 분리됨)
	//	if (capsuleMax < obbMin || obbMax < capsuleMin)
	//		return false; // 분리축 발견, 충돌 없음
	//}

	//// 2. 캡슐의 끝부분(구체)과 OBB 충돌 검사
	//// OBB의 각 모서리와 캡슐의 끝부분(구체) 사이의 거리를 계산
	//XMFLOAT3 obbCorners[8];
	//pTargetBounding->GetCorners(obbCorners);

	//// 캡슐의 상단(구체)
	//XMVECTOR sphereTopCenter = vTop;
	//// 캡슐의 하단(구체)
	//XMVECTOR sphereBotCenter = vBot;

	//// 구체와 OBB의 각 모서리 사이의 거리 계산
	//for (int i = 0; i < 8; ++i) {
	//	XMVECTOR obbCorner = XMLoadFloat3(&obbCorners[i]);

	//	// 구체와 모서리 간의 거리 계산 (구체의 반경 + OBB 모서리까지의 거리)
	//	XMVECTOR distToTop = XMVectorSubtract(obbCorner, sphereTopCenter);
	//	float distToTopLength = XMVectorGetX(XMVector3Length(distToTop));

	//	XMVECTOR distToBot = XMVectorSubtract(obbCorner, sphereBotCenter);
	//	float distToBotLength = XMVectorGetX(XMVector3Length(distToBot));

	//	// 캡슐의 반경이 거리보다 작다면 충돌 발생
	//	if (distToTopLength <= m_pMyDesc.fRadius || distToBotLength <= m_pMyDesc.fRadius)
	//		return true;
	//}

	//return true;

	_vector obbXAxis, obbYAxis, obbZAxis;
	pTargetBounding->GetOBBAxes(obbXAxis, obbYAxis, obbZAxis);
	_vector vOBBCenter = XMLoadFloat3(&pTargetBounding->Get_MyDesc()->_Center);
	_vector vOBBExtents = XMLoadFloat3(&pTargetBounding->Get_MyDesc()->_Extents);
	
	XMVECTOR vTop = XMVectorAdd(XMLoadFloat3(&m_pMyDesc.vCenter), XMVectorScale(XMLoadFloat3(&m_pMyDesc.vDir), m_pMyDesc.fHeight));
	XMVECTOR vBot = XMVectorAdd(XMLoadFloat3(&m_pMyDesc.vCenter), -(XMVectorScale(XMLoadFloat3(&m_pMyDesc.vDir), m_pMyDesc.fHeight)));
	// 캡슐의 중심 축 벡터
	XMVECTOR capsuleAxis = XMVectorSubtract(vBot, vTop);

	XMVECTOR Axes[7] = { 
		obbXAxis, obbYAxis, obbZAxis,
		capsuleAxis,
		XMVector3Cross(capsuleAxis, obbXAxis),
		XMVector3Cross(capsuleAxis, obbYAxis),
		XMVector3Cross(capsuleAxis, obbZAxis)
	};

	// OBB의 각 축과 캡슐 중심 축 사이의 분리축 검사
	for (int i = 0; i < 7; ++i) {
		XMVECTOR Axis = XMVector3Normalize(Axes[i]);

		// 캡슐의 투영 거리 계산
		XMVECTOR capsuleStartProj = XMVector3Dot(vTop, Axis);
		XMVECTOR capsuleEndProj = XMVector3Dot(vBot, Axis);
		float capsuleMin = XMVectorGetX(XMVectorMin(capsuleStartProj, capsuleEndProj)) - m_pMyDesc.fRadius;
		float capsuleMax = XMVectorGetX(XMVectorMax(capsuleStartProj, capsuleEndProj)) + m_pMyDesc.fRadius;

		// OBB의 투영 거리 계산
		XMVECTOR obbCenterProj = XMVector3Dot(vOBBCenter, Axis);
		float obbRadius = fabsf(XMVectorGetX(XMVector3Dot(obbXAxis * vOBBExtents.m128_f32[0], Axis))) +
							fabsf(XMVectorGetX(XMVector3Dot(obbYAxis * vOBBExtents.m128_f32[1], Axis))) +
							fabsf(XMVectorGetX(XMVector3Dot(obbZAxis * vOBBExtents.m128_f32[2], Axis)));
		float obbMin = XMVectorGetX(obbCenterProj) - obbRadius;
		float obbMax = XMVectorGetX(obbCenterProj) + obbRadius;

		// 분리축에서 충돌 여부 판단 (간격이 존재하면 분리됨)
		if (capsuleMax < obbMin || obbMax < capsuleMin)
			return false; // 분리축 발견, 충돌 없음	
	}
	return true;
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

XMVECTOR CBounding_Capsule::ClosestPointOnLineSegment(const _vector& start, const _vector& end, const _vector& point)
{
	XMVECTOR lineVec = XMVectorSubtract(end, start);
	XMVECTOR pointVec = XMVectorSubtract(point, start);
	float t = XMVectorGetX(XMVector3Dot(pointVec, lineVec)) / XMVectorGetX(XMVector3LengthSq(lineVec));
	t = max(0.0f, min(1.0f, t));
	return XMVectorAdd(start, XMVectorScale(lineVec, t));
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
