#include "stdafx.h"
#include "..\Public\LandObject.h"

CLandObject::CLandObject(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CLandObject::CLandObject(const CLandObject & rhs)
	: CGameObject(rhs)
{
}

HRESULT CLandObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLandObject::Initialize(void * pArg)
{
	LANDOBJ_DESC*	pGameObjectDesc = (LANDOBJ_DESC*)pArg;

	m_pTerrainTransform = pGameObjectDesc->pTerrainTransform;
	m_pTerrainVIBuffer = pGameObjectDesc->pTerrainVIBuffer;

	Safe_AddRef(m_pTerrainTransform);
	Safe_AddRef(m_pTerrainVIBuffer);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CLandObject::Priority_Tick(_float fTimeDelta)
{
}

void CLandObject::Tick(_float fTimeDelta)
{
}

void CLandObject::Late_Tick(_float fTimeDelta)
{
}

HRESULT CLandObject::Render()
{
	return S_OK;
}


HRESULT CLandObject::SetUp_OnTerrain(CTransform * pTargetTransform)
{
	/* ������ Ÿ���ϴ� ��ü�� ��ġ������ ���´�.(In WorldSpace) */
	_vector		vTargetPos = pTargetTransform->Get_State(CTransform::STATE_POSITION);

	/* ������ü�� �׸��� �־��� ������������ ���ý����̽� �󿡼� Ÿ���� �����ϴ� �ﰢ������ ���̸� ����ؿ´�. */
	/* ���� ����(����)�� Ÿ���� ��ġ(����)�� ���ؾ��Ѵ�. */
	/* Ÿ�� ��ġ(����)�� ���� ������ ���ý����̽� �̵�(������ ���÷ΰ����ϱ⶧���� ������������� ������� �ʿ��ϴ�) ���Ѽ� ���ؾ߰ڴ�. */
	vTargetPos = XMVector3TransformCoord(vTargetPos, m_pTerrainTransform->Get_WorldMatrix_Inverse());

	vTargetPos = XMVectorSetY(vTargetPos, m_pTerrainVIBuffer->Compute_Height(vTargetPos));

	pTargetTransform->Set_State(CTransform::STATE_POSITION, XMVector3TransformCoord(vTargetPos, m_pTerrainTransform->Get_WorldMatrix()));

	return S_OK;
}

void CLandObject::Free()
{
	__super::Free();

	Safe_Release(m_pTerrainTransform);
	Safe_Release(m_pTerrainVIBuffer);
}
