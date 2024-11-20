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
	_vector vOBBCenter = XMLoadFloat3(&pTargetBounding->Get_MyDesc()->_Center);
	_vector vOBBExtents = XMLoadFloat3(&pTargetBounding->Get_MyDesc()->_Extents);
	_vector vOBBOrientation = XMLoadFloat4(&pTargetBounding->Get_MyDesc()->_Orientation);

	// ĸ���� �� ���� OBB ��ǥ��� ��ȯ
	_vector	vTop = XMVectorAdd(XMLoadFloat3(&m_pMyDesc.vCenter), XMVectorScale(XMLoadFloat3(&m_pMyDesc.vDir), m_pMyDesc.fHeight));
	_vector	vBot = XMVectorAdd(XMLoadFloat3(&m_pMyDesc.vCenter), -(XMVectorScale(XMLoadFloat3(&m_pMyDesc.vDir), m_pMyDesc.fHeight)));

	_vector vOBBSpaceTop = XMVector3InverseRotate(XMVectorSubtract(vTop, vOBBCenter), vOBBOrientation);
	_vector vOBBSpaceBot = XMVector3InverseRotate(XMVectorSubtract(vBot, vOBBCenter), vOBBOrientation);

	// ĸ���� ���а� OBB�� �ִ� �Ÿ� ���
	_vector clampedTop = XMVectorClamp(vOBBSpaceTop, XMVectorNegate(vOBBExtents), vOBBExtents);
	_vector clampedBot = XMVectorClamp(vOBBSpaceBot, XMVectorNegate(vOBBExtents), vOBBExtents);

	// OBB���η� Ŭ������ Top�� Bot���� ĸ�� �������� ���� ����� �� ���
	_vector vClosestPoint1 = ClosestPointOnLineSegment(clampedTop, vOBBSpaceTop, vOBBSpaceBot);
	_vector vClosestPoint2 = ClosestPointOnLineSegment(clampedBot, vOBBSpaceTop, vOBBSpaceBot);

	//�ι��� Ŭ����
	_vector clampedTop2 = XMVectorClamp(vClosestPoint1, XMVectorNegate(vOBBExtents), vOBBExtents);
	_vector clampedBot2 = XMVectorClamp(vClosestPoint2, XMVectorNegate(vOBBExtents), vOBBExtents);

	// ù��° Ŭ������ �ι�° Ŭ������ �̿��� ���� ����
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

	// Ŭ���ε� ���� ���� �� ������ ���͸� ����Ͽ� �ִ� �Ÿ� ����
	_vector deltaTop = XMVectorSubtract(vClosestPoint1, clampedTop);
	_vector deltaBot = XMVectorSubtract(vClosestPoint2, clampedBot);

	// �� �� �� �ּ� �Ÿ��� ����
	float distSquaredTop = XMVectorGetX(XMVector3LengthSq(deltaTop));
	float distSquaredBot = XMVectorGetX(XMVector3LengthSq(deltaBot));
	float minDistSquared = min(distSquaredTop, distSquaredBot);

	// ĸ���� �������� ���Ͽ� �浹 ���� �Ǵ�
	return (minDistSquared <= (m_pMyDesc.fRadius * m_pMyDesc.fRadius));
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

_vector CBounding_Capsule::ClosestPointOnLineSegment(const _vector& point, const _vector& start, const _vector& end)
{
	XMVECTOR segDir = XMVectorSubtract(end, start);
	float segLengthSq = XMVectorGetX(XMVector3LengthSq(segDir));

	if (segLengthSq < FLT_EPSILON) {
		// ������ ���̰� 0�̸�, ������ ��ȯ
		return start;
	}

	// ���� ������ ���������� �̵�
	XMVECTOR d = XMVectorSubtract(point, start);
	float t = XMVectorGetX(XMVector3Dot(d, segDir)) / segLengthSq;

	// t�� 0 ~ 1�� Ŭ����
	t = std::clamp(t, 0.0f, 1.0f);

	// ���� ���� ���� ����� �� ���
	return XMVectorAdd(start, XMVectorScale(segDir, t));
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

bool CBounding_Capsule::GetIntersectionPoint(_vector A1, _vector d1, _vector A2, _vector d2, _vector& intersection)
{
	// ���� �� ���� ����
	XMVECTOR diff = A2 - A1;

	// �� ���� ������ ���� ���
	XMVECTOR cross_d1_d2 = XMVector3Cross(d1, d2);
	float det = XMVectorGetX(XMVector3Length(cross_d1_d2)); // ������ ũ��(���� ����)

	// ���� ���Ͱ� �����ϰų� ���� ������ �����ϴ� ���
	if (fabs(det) < 1e-6f) { // ����� ���� ��(��� ���� ����)
		return false;
	}

	// ũ�ν� ���� Ȱ���Ͽ� �Ű����� t�� u ���
	XMVECTOR cross_diff_d2 = XMVector3Cross(diff, d2);
	float t = XMVectorGetX(XMVector3Dot(cross_diff_d2, cross_d1_d2)) / det;

	XMVECTOR cross_diff_d1 = XMVector3Cross(diff, d1);
	float u = XMVectorGetX(XMVector3Dot(cross_diff_d1, cross_d1_d2)) / det;

	// t�� u�� [0, 1] ������ �־�� ������ ������ ����
	if (t < 0.0f || t > 1.0f || u < 0.0f || u > 1.0f) {
		return false;
	}

	// ������ ���
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
