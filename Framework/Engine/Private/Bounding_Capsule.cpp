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
	//// OBB�� �߽ɰ� ũ��
	//XMVECTOR vOBBCenter = XMLoadFloat3(&pTargetBounding->Get_MyDesc()->_Center);
	//XMVECTOR vOBBExtents = XMLoadFloat3(&pTargetBounding->Get_MyDesc()->_Extents);

	//// ĸ���� ��ܰ� �ϴ� ��
	//XMVECTOR vTop = XMVectorAdd(XMLoadFloat3(&m_pMyDesc.vCenter), XMVectorScale(XMLoadFloat3(&m_pMyDesc.vDir), m_pMyDesc.fHeight));
	//XMVECTOR vBot = XMVectorAdd(XMLoadFloat3(&m_pMyDesc.vCenter), -(XMVectorScale(XMLoadFloat3(&m_pMyDesc.vDir), m_pMyDesc.fHeight)));

	//// ĸ���� �߽� �� ����
	//XMVECTOR capsuleAxis = XMVectorSubtract(vBot, vTop);

	//// OBB�� �� ����
	//XMVECTOR obbXAxis, obbYAxis, obbZAxis;
	//pTargetBounding->GetOBBAxes(obbXAxis, obbYAxis, obbZAxis);

	//// �и��� ������ ���� ���
	//XMVECTOR Axes[7] = {
	//	obbXAxis, obbYAxis, obbZAxis,
	//	capsuleAxis,
	//	XMVector3Cross(capsuleAxis, obbXAxis),
	//	XMVector3Cross(capsuleAxis, obbYAxis),
	//	XMVector3Cross(capsuleAxis, obbZAxis)
	//};

	//// OBB�� ĸ���� �浹 �˻� (�и��� �˻�)
	//for (int i = 0; i < 7; ++i) {
	//	XMVECTOR Axis = Axes[i];

	//	// ĸ���� ���� �Ÿ� ���
	//	XMVECTOR capsuleStartProj = XMVector3Dot(vTop, Axis);
	//	XMVECTOR capsuleEndProj = XMVector3Dot(vBot, Axis);
	//	float capsuleMin = XMVectorGetX(XMVectorMin(capsuleStartProj, capsuleEndProj)) - m_pMyDesc.fRadius;
	//	float capsuleMax = XMVectorGetX(XMVectorMax(capsuleStartProj, capsuleEndProj)) + m_pMyDesc.fRadius;

	//	// OBB�� ���� �Ÿ� ���
	//	XMVECTOR obbCenterProj = XMVector3Dot(vOBBCenter, Axis);
	//	float obbRadius = fabsf(XMVectorGetX(XMVector3Dot(vOBBExtents, obbXAxis))) +
	//		fabsf(XMVectorGetX(XMVector3Dot(vOBBExtents, obbYAxis))) +
	//		fabsf(XMVectorGetX(XMVector3Dot(vOBBExtents, obbZAxis)));
	//	float obbMin = XMVectorGetX(obbCenterProj) - obbRadius;
	//	float obbMax = XMVectorGetX(obbCenterProj) + obbRadius;

	//	// �и��࿡�� �浹 ���� �Ǵ� (������ �����ϸ� �и���)
	//	if (capsuleMax < obbMin || obbMax < capsuleMin)
	//		return false; // �и��� �߰�, �浹 ����
	//}

	//// 2. ĸ���� ���κ�(��ü)�� OBB �浹 �˻�
	//// OBB�� �� �𼭸��� ĸ���� ���κ�(��ü) ������ �Ÿ��� ���
	//XMFLOAT3 obbCorners[8];
	//pTargetBounding->GetCorners(obbCorners);

	//// ĸ���� ���(��ü)
	//XMVECTOR sphereTopCenter = vTop;
	//// ĸ���� �ϴ�(��ü)
	//XMVECTOR sphereBotCenter = vBot;

	//// ��ü�� OBB�� �� �𼭸� ������ �Ÿ� ���
	//for (int i = 0; i < 8; ++i) {
	//	XMVECTOR obbCorner = XMLoadFloat3(&obbCorners[i]);

	//	// ��ü�� �𼭸� ���� �Ÿ� ��� (��ü�� �ݰ� + OBB �𼭸������� �Ÿ�)
	//	XMVECTOR distToTop = XMVectorSubtract(obbCorner, sphereTopCenter);
	//	float distToTopLength = XMVectorGetX(XMVector3Length(distToTop));

	//	XMVECTOR distToBot = XMVectorSubtract(obbCorner, sphereBotCenter);
	//	float distToBotLength = XMVectorGetX(XMVector3Length(distToBot));

	//	// ĸ���� �ݰ��� �Ÿ����� �۴ٸ� �浹 �߻�
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
	// ĸ���� �߽� �� ����
	XMVECTOR capsuleAxis = XMVectorSubtract(vBot, vTop);

	XMVECTOR Axes[7] = { 
		obbXAxis, obbYAxis, obbZAxis,
		capsuleAxis,
		XMVector3Cross(capsuleAxis, obbXAxis),
		XMVector3Cross(capsuleAxis, obbYAxis),
		XMVector3Cross(capsuleAxis, obbZAxis)
	};

	// OBB�� �� ��� ĸ�� �߽� �� ������ �и��� �˻�
	for (int i = 0; i < 7; ++i) {
		XMVECTOR Axis = XMVector3Normalize(Axes[i]);

		// ĸ���� ���� �Ÿ� ���
		XMVECTOR capsuleStartProj = XMVector3Dot(vTop, Axis);
		XMVECTOR capsuleEndProj = XMVector3Dot(vBot, Axis);
		float capsuleMin = XMVectorGetX(XMVectorMin(capsuleStartProj, capsuleEndProj)) - m_pMyDesc.fRadius;
		float capsuleMax = XMVectorGetX(XMVectorMax(capsuleStartProj, capsuleEndProj)) + m_pMyDesc.fRadius;

		// OBB�� ���� �Ÿ� ���
		XMVECTOR obbCenterProj = XMVector3Dot(vOBBCenter, Axis);
		float obbRadius = fabsf(XMVectorGetX(XMVector3Dot(obbXAxis * vOBBExtents.m128_f32[0], Axis))) +
							fabsf(XMVectorGetX(XMVector3Dot(obbYAxis * vOBBExtents.m128_f32[1], Axis))) +
							fabsf(XMVectorGetX(XMVector3Dot(obbZAxis * vOBBExtents.m128_f32[2], Axis)));
		float obbMin = XMVectorGetX(obbCenterProj) - obbRadius;
		float obbMax = XMVectorGetX(obbCenterProj) + obbRadius;

		// �и��࿡�� �浹 ���� �Ǵ� (������ �����ϸ� �и���)
		if (capsuleMax < obbMin || obbMax < capsuleMin)
			return false; // �и��� �߰�, �浹 ����	
	}
	return true;
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
