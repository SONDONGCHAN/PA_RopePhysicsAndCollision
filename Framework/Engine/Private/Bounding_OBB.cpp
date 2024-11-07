#include "..\Public\Bounding_OBB.h"
#include "Collider.h"

CBounding_OBB::CBounding_OBB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CBounding(pDevice, pContext)
{
}


HRESULT CBounding_OBB::Initialize(void * pArg)
{
	/* 초기상태는 로컬상태 */
	CCollider::OBB_DESC*	pDesc = (CCollider::OBB_DESC*)pArg;

	_float4		vRotation;
	XMStoreFloat4(&vRotation, XMQuaternionRotationRollPitchYaw(pDesc->vRadians.x, pDesc->vRadians.y, pDesc->vRadians.z));

	m_pOriginalOBB = new BoundingOrientedBox(pDesc->vCenter, pDesc->vExtents, vRotation);
	m_pOBB = new BoundingOrientedBox(*m_pOriginalOBB);

	m_MyOriginalDesc._Center		= pDesc->vCenter;
	m_MyOriginalDesc._Extents		= pDesc->vExtents;
	m_MyOriginalDesc._Orientation	= vRotation;
	m_MyDesc = m_MyOriginalDesc;

	return S_OK;
}

void CBounding_OBB::Tick(_fmatrix WorldMatrix)
{
	m_pOriginalOBB->Transform(*m_pOBB, WorldMatrix);
	TransformMyDesc(WorldMatrix);
}

HRESULT CBounding_OBB::Render(PrimitiveBatch<VertexPositionColor>* pBatch)
{
	DX::Draw(pBatch, *m_pOBB, m_isColl == true ? XMVectorSet(1.f, 0.f, 0.f, 1.f) : XMVectorSet(0.f, 1.f, 0.f, 1.f));

	return S_OK;
}

_bool CBounding_OBB::Intersect(CCollider::TYPE eType, CBounding * pBounding)
{
	_bool isColl = false;

	void*		pTargetBounding = pBounding->Get_Bounding();

	switch (eType)
	{
	case CCollider::TYPE_AABB:
		isColl = m_pOBB->Intersects(*(BoundingBox*)pTargetBounding);
		break;
	case CCollider::TYPE_OBB:
		isColl = m_pOBB->Intersects(*(BoundingOrientedBox*)pTargetBounding); /* Intersect((CBounding_OBB*)pBounding);*/
		if(isColl)
			vColFace = CalColFace(dynamic_cast<CBounding_OBB*>(pBounding)->Get_MyDesc());
		break;
	case CCollider::TYPE_SPHERE:
		isColl = m_pOBB->Intersects(*(BoundingSphere*)pTargetBounding);
		break;
	}

	if (isColl && (!m_isColl))
		m_isColl = true;

	return isColl;
}

_vector CBounding_OBB::CalColFace(My_Desc* _Target_Desc)
{
	// Build the 3x3 rotation matrix that defines the orientation of B relative to A.
	XMVECTOR A_quat = XMLoadFloat4(&m_MyDesc._Orientation);
	XMVECTOR B_quat = XMLoadFloat4(&_Target_Desc->_Orientation);

	XMVECTOR Q = XMQuaternionMultiply(A_quat, XMQuaternionConjugate(B_quat));
	XMMATRIX R = XMMatrixRotationQuaternion(Q);

	// Compute the translation of B relative to A.
	XMVECTOR A_cent = XMLoadFloat3(&m_MyDesc._Center);
	XMVECTOR B_cent = XMLoadFloat3(&_Target_Desc->_Center);
	XMVECTOR t = XMVector3InverseRotate(XMVectorSubtract(B_cent, A_cent), A_quat);


	// Load extents of A and B.
	XMVECTOR h_A = XMLoadFloat3(&m_MyDesc._Extents);
	XMVECTOR h_B = XMLoadFloat3(&_Target_Desc->_Extents);

	// Rows. Note R[0,1,2]X.w = 0.
	XMVECTOR R0X = R.r[0];
	XMVECTOR R1X = R.r[1];
	XMVECTOR R2X = R.r[2];

	// Absolute value of rows.
	XMVECTOR AR0X = XMVectorAbs(R0X);
	XMVECTOR AR1X = XMVectorAbs(R1X);
	XMVECTOR AR2X = XMVectorAbs(R2X);


	XMVECTOR minOverlap = XMVectorReplicate(FLT_MAX);
	XMVECTOR collisionAxis = XMVectorZero();

	// Helper lambda to check overlap and update minimum overlap axis
	auto TestAxis = [&](XMVECTOR _d, XMVECTOR _d_A, XMVECTOR _d_B, XMVECTOR _axis) -> void
	{
		XMVECTOR overlap = XMVectorSubtract(XMVectorAdd(_d_A, _d_B), XMVectorAbs(_d));
		if (XMVector3Less(overlap, minOverlap))
		{
			minOverlap = overlap;
			collisionAxis = _axis;
		}
	};


	XMVECTOR d, d_A, d_B;

	d = XMVectorSplatX(t);
	d_A = XMVectorSplatX(h_A);
	d_B = XMVector3Dot(h_B, AR0X);
	TestAxis(d, d_A, d_B, XMVectorSet(1, 0, 0, 0));

	d = XMVectorSplatY(t);
	d_A = XMVectorSplatY(h_A);
	d_B = XMVector3Dot(h_B, AR1X);
	TestAxis(d, d_A, d_B, XMVectorSet(0, 1, 0, 0));

	d = XMVectorSplatZ(t);
	d_A = XMVectorSplatZ(h_A);
	d_B = XMVector3Dot(h_B, AR2X);
	TestAxis(d, d_A, d_B, XMVectorSet(0, 0, 1, 0));


	return collisionAxis;
}

CBounding_OBB::OBB_COL_DESC CBounding_OBB::Compute_OBBColDesc()
{
	OBB_COL_DESC			OBBDesc{};

	_float3		vPoints[8];

	m_pOBB->GetCorners(vPoints);

	OBBDesc.vCenter = m_pOBB->Center;
	XMStoreFloat3(&OBBDesc.vCenterAxis[0], (XMLoadFloat3(&vPoints[5]) - XMLoadFloat3(&vPoints[4])) * 0.5f);
	XMStoreFloat3(&OBBDesc.vCenterAxis[1], (XMLoadFloat3(&vPoints[7]) - XMLoadFloat3(&vPoints[4])) * 0.5f);
	XMStoreFloat3(&OBBDesc.vCenterAxis[2], (XMLoadFloat3(&vPoints[0]) - XMLoadFloat3(&vPoints[4])) * 0.5f);

	for (size_t i = 0; i < 3; i++)
	{
		XMStoreFloat3(&OBBDesc.vAlignAxis[i], XMVector3Normalize(XMLoadFloat3(&OBBDesc.vCenterAxis[i])));
	}

	return OBBDesc;
}

void CBounding_OBB::TransformMyDesc(_fmatrix WorldMatrix)
{
	// Load the box.
	XMVECTOR vCenter = XMLoadFloat3(&m_MyOriginalDesc._Center);
	XMVECTOR vExtents = XMLoadFloat3(&m_MyOriginalDesc._Extents);
	XMVECTOR vOrientation = XMLoadFloat4(&m_MyOriginalDesc._Orientation);

	assert(DirectX::Internal::XMQuaternionIsUnit(vOrientation));

	// Composite the box rotation and the transform rotation.
	XMMATRIX nM;
	nM.r[0] = XMVector3Normalize(WorldMatrix.r[0]);
	nM.r[1] = XMVector3Normalize(WorldMatrix.r[1]);
	nM.r[2] = XMVector3Normalize(WorldMatrix.r[2]);
	nM.r[3] = g_XMIdentityR3;
	XMVECTOR Rotation = XMQuaternionRotationMatrix(nM);
	vOrientation = XMQuaternionMultiply(vOrientation, Rotation);

	// Transform the center.
	vCenter = XMVector3Transform(vCenter, WorldMatrix);

	// Scale the box extents.
	XMVECTOR dX = XMVector3Length(WorldMatrix.r[0]);
	XMVECTOR dY = XMVector3Length(WorldMatrix.r[1]);
	XMVECTOR dZ = XMVector3Length(WorldMatrix.r[2]);

	XMVECTOR VectorScale = XMVectorSelect(dY, dX, g_XMSelect1000);
	VectorScale = XMVectorSelect(dZ, VectorScale, g_XMSelect1100);
	vExtents = XMVectorMultiply(vExtents, VectorScale);

	// Store the box.
	XMStoreFloat3(&m_MyDesc._Center, vCenter);
	XMStoreFloat3(&m_MyDesc._Extents, vExtents);
	XMStoreFloat4(&m_MyDesc._Orientation, vOrientation);
}

_bool CBounding_OBB::Intersect(CBounding_OBB * pTargetBounding)
{
	OBB_COL_DESC		OBBDesc[2];

	OBBDesc[0] = Compute_OBBColDesc();
	OBBDesc[1] = pTargetBounding->Compute_OBBColDesc();

	_float			fDistance[3];

	for (size_t i = 0; i < 2; i++)	
	{
		for (size_t j = 0; j < 3; j++)
		{
			fDistance[0] = fabs(XMVector3Dot(XMLoadFloat3(&OBBDesc[1].vCenter) - XMLoadFloat3(&OBBDesc[0].vCenter),
				XMLoadFloat3(&OBBDesc[i].vAlignAxis[j])).m128_f32[0]);

			fDistance[1] = fabs(XMVector3Dot(XMLoadFloat3(&OBBDesc[0].vCenterAxis[0]), XMLoadFloat3(&OBBDesc[i].vAlignAxis[j])).m128_f32[0]) +
				fabs(XMVector3Dot(XMLoadFloat3(&OBBDesc[0].vCenterAxis[1]), XMLoadFloat3(&OBBDesc[i].vAlignAxis[j])).m128_f32[0]) +
				fabs(XMVector3Dot(XMLoadFloat3(&OBBDesc[0].vCenterAxis[2]), XMLoadFloat3(&OBBDesc[i].vAlignAxis[j])).m128_f32[0]);

			fDistance[2] = fabs(XMVector3Dot(XMLoadFloat3(&OBBDesc[1].vCenterAxis[0]), XMLoadFloat3(&OBBDesc[i].vAlignAxis[j])).m128_f32[0]) +
				fabs(XMVector3Dot(XMLoadFloat3(&OBBDesc[1].vCenterAxis[1]), XMLoadFloat3(&OBBDesc[i].vAlignAxis[j])).m128_f32[0]) +
				fabs(XMVector3Dot(XMLoadFloat3(&OBBDesc[1].vCenterAxis[2]), XMLoadFloat3(&OBBDesc[i].vAlignAxis[j])).m128_f32[0]);

			if (fDistance[0] > fDistance[1] + fDistance[2])
				return false;
		}
	}

	return true;
}

CBounding_OBB * CBounding_OBB::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, void * pArg)
{
	CBounding_OBB*		pInstance = new CBounding_OBB(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CBounding_OBB");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBounding_OBB::Free()
{
	__super::Free();
	Safe_Delete(m_pOriginalOBB);
	Safe_Delete(m_pOBB);
}
