#include "..\Public\Bounding_OBB.h"
#include "Collider.h"

CBounding_OBB::CBounding_OBB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CBounding(pDevice, pContext)
{
}


HRESULT CBounding_OBB::Initialize(void * pArg)
{
	/* �ʱ���´� ���û��� */
	CCollider::OBB_DESC*	pDesc = (CCollider::OBB_DESC*)pArg;

	_float4		vRotation;
	XMStoreFloat4(&vRotation, XMQuaternionRotationRollPitchYaw(pDesc->vRadians.x, pDesc->vRadians.y, pDesc->vRadians.z));

	m_pOriginalOBB = new BoundingOrientedBox(pDesc->vCenter, pDesc->vExtents, vRotation);
	m_pOBB = new BoundingOrientedBox(*m_pOriginalOBB);

	return S_OK;
}

void CBounding_OBB::Tick(_fmatrix WorldMatrix)
{
	m_pOriginalOBB->Transform(*m_pOBB, WorldMatrix);
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
		break;
	case CCollider::TYPE_SPHERE:
		isColl = m_pOBB->Intersects(*(BoundingSphere*)pTargetBounding);
		break;
	}

	if (isColl && (!m_isColl))
		m_isColl = true;

	return isColl;
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
