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

	/* �������� */
	m_pSphere_1 = new BoundingSphere(m_pMyDesc.vCenter, m_pMyDesc.fRadius);
	m_pSphere_2 = new BoundingSphere(m_pMyDesc.vCenter, m_pMyDesc.fRadius);
	/* �������� */

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
	
	// 1. ĸ�� ��� OBB �� �� �Ÿ� ���
	for (int i = 0; i < 3; ++i) 
	{
		_vector planeNormal = obbAxes[i]; // OBB �� ����(���� ����)
	
		// ���� ��ġ�� OBB�� Extents�� �ݿ��Ͽ� ���
		float planeDistancePos = XMVectorGetX(XMVector3Dot(vOBBCenter, planeNormal)) + vOBBExtents.m128_f32[i];
		float planeDistanceNeg = XMVectorGetX(XMVector3Dot(vOBBCenter, planeNormal)) - vOBBExtents.m128_f32[i];
	
		// ĸ�� �� ����(t1: vTop, t2: vBot)�� OBB�� +Extents ������� �Ÿ�
		float t1Pos = XMVectorGetX(XMVector3Dot(vTop, planeNormal)) - planeDistancePos;
		float t2Pos = XMVectorGetX(XMVector3Dot(vBot, planeNormal)) - planeDistancePos;
	
		// ĸ�� �� ����(t1: vTop, t2: vBot)�� OBB�� -Extents ������� �Ÿ�
		float t1Neg = XMVectorGetX(XMVector3Dot(vTop, planeNormal)) - planeDistanceNeg;
		float t2Neg = XMVectorGetX(XMVector3Dot(vBot, planeNormal)) - planeDistanceNeg;
		
		// �� ������ OBB �� ��� �ȿ� �ִ��� Ȯ��
		bool outsideBoundary = false;

		for (int j = 0; j < 3; ++j) 
		{
			if (j == i) 
				continue; // ���� ��(i)�� ���� ������ �ุ Ȯ��

			// ĸ�� ������ ���� ��
			float extentProjTop = XMVectorGetX(XMVector3Dot(vTop, obbAxes[j])) - XMVectorGetX(XMVector3Dot(vOBBCenter, obbAxes[j]));
			float extentProjBot = XMVectorGetX(XMVector3Dot(vBot, obbAxes[j])) - XMVectorGetX(XMVector3Dot(vOBBCenter, obbAxes[j]));

			// OBB ���� ���(Extents)�� ��
			float extentLimit = vOBBExtents.m128_f32[j];
			if (fabs(extentProjTop) > extentLimit || fabs(extentProjBot) > extentLimit) 
			{
				outsideBoundary = true; // ��� �ۿ� ����
				break; // ���� ���� ����
			}
		}

		if (outsideBoundary) {
			continue; // �ܺ� ������ �ǳʶ�
		}
		// �� �鿡�� ���� ����� �Ÿ� ���
		float closestPlaneDistancePos = min(fabs(t1Pos), fabs(t2Pos));
		float closestPlaneDistanceNeg = min(fabs(t1Neg), fabs(t2Neg));
	
		// ���� ����� �Ÿ� ����
		float closestPlaneDistance = min(closestPlaneDistancePos, closestPlaneDistanceNeg);
	
		// �ִ� �Ÿ� ���� ������Ʈ
		closestDistanceSquared = min(closestDistanceSquared, closestPlaneDistance * closestPlaneDistance);
	}
	
	// 2. ĸ�� ��� OBB �𼭸� �� �Ÿ� ���
	for (int i = 0; i < 3; ++i) 
	{
		for (int j = -1; j <= 1; j += 2) 
		{
			for (int k = -1; k <= 1; k += 2)
			{
				// OBB �𼭸��� �������� ���� ���
				_vector edgeStart = vOBBCenter + obbAxes[i] * vOBBExtents.m128_f32[i] * j +
					obbAxes[(i + 1) % 3] * vOBBExtents.m128_f32[(i + 1) % 3] * k;
	
				_vector edgeDirection = obbAxes[(i + 2) % 3] * (2 * vOBBExtents.m128_f32[(i + 2) % 3]);
	
				// ���� �� �ִ� �Ÿ� ���
				_vector closestPointBetweenEdges = ClosestPointBetweenSegments(vTop, vBot, edgeStart, edgeStart + edgeDirection);
				closestDistanceSquared = min(closestDistanceSquared, XMVectorGetX(XMVector3LengthSq(closestPointBetweenEdges)));
			}
		}
	}
	
	// 3. ĸ�� ��� OBB ������ �� �Ÿ� ���
	for (int i = -1; i <= 1; i += 2) 
	{
		for (int j = -1; j <= 1; j += 2) 
		{
			for (int k = -1; k <= 1; k += 2) 
			{
				// OBB ������ ���
				_vector corner = vOBBCenter + obbAxes[0] * vOBBExtents.m128_f32[0] * i +
					obbAxes[1] * vOBBExtents.m128_f32[1] * j +
					obbAxes[2] * vOBBExtents.m128_f32[2] * k;
	
				// ���� ���� �� �ִ� �Ÿ� ���
				_vector closestPointToCorner = ClosestPointOnLineSegment(vTop, vBot, corner);
				closestDistanceSquared = min(closestDistanceSquared, XMVectorGetX(XMVector3LengthSq(closestPointToCorner)));
			}
		}
	}
	
	
	if (closestDistanceSquared <= m_pMyDesc.fRadius * m_pMyDesc.fRadius) {
		return true; // �浹 �߻�
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

	// ������ ������ ĸ�� ����κ� ���ο� �ִ��� Ȯ��
	
	// ���ο� �ִ� ���
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
	// ���ο� ���°��
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
	/* ĸ�� ��ġ ����*/ 
	XMVECTOR vCenter = XMLoadFloat3(&m_pOriginalMyDesc.vCenter);
	XMVECTOR C = XMVector3Transform(vCenter, WorldMatrix);
	XMStoreFloat3(&m_pMyDesc.vCenter, C);

	_vector vUp = WorldMatrix.r[1];
	XMStoreFloat3(&m_pMyDesc.vDir, vUp);


	/* �������� ��ü ����*/
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
	_vector dA = A2 - A1; // ���� A�� ���� ����
	_vector dB = B2 - B1; // ���� B�� ���� ����
	_vector r = A1 - B1;  // �� ������ ������ ����

	float a = XMVectorGetX(XMVector3Dot(dA, dA)); // ���� A�� ���� ����
	float b = XMVectorGetX(XMVector3Dot(dA, dB)); // ���� A�� B�� ����
	float c = XMVectorGetX(XMVector3Dot(dB, dB)); // ���� B�� ���� ����
	float d = XMVectorGetX(XMVector3Dot(dA, r));  // ���� A�� r�� ����
	float e = XMVectorGetX(XMVector3Dot(dB, r));  // ���� B�� r�� ����

	float denominator = a * c - b * b; // ���� ���� ���� ���θ� Ȯ���ϴ� �и�

	float t, s;

	// �и� 0�� ��� ������ ����
	if (fabs(denominator) < 1e-6f) {
		t = 0.0f; // ���� A�� ���� ��
		s = e / c; // ���� B�� ���� ����� ��
		s = std::clamp(s, 0.0f, 1.0f); // s�� [0, 1]�� Ŭ����
	}
	else {
		// �Ϲ����� ��� (�������� ����)
		t = (b * e - c * d) / denominator;
		s = (a * e - b * d) / denominator;

		// t�� s�� [0, 1]�� Ŭ����
		t = std::clamp(t, 0.0f, 1.0f);
		s = std::clamp(s, 0.0f, 1.0f);
	}

	// �� ���п��� ���� ����� ��
	_vector closestPointA = A1 + t * dA; // ���� A�� ���� ����� ��
	_vector closestPointB = B1 + s * dB; // ���� B�� ���� ����� ��

	// �ִ� �Ÿ� ���� ��ȯ
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
