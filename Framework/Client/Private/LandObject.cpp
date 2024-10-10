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
	/* 지형을 타야하는 객체의 위치정보를 얻어온다.(In WorldSpace) */
	_vector		vTargetPos = pTargetTransform->Get_State(CTransform::STATE_POSITION);

	/* 지형객체가 그리고 있었던 지형정점들의 로컬스페이스 상에서 타겟이 존재하는 삼각형내에 높이를 계산해온다. */
	/* 지형 정점(로컬)과 타겟의 위치(월드)를 비교해야한다. */
	/* 타겟 위치(월드)를 지형 정점의 로컬스페이스 이동(지형의 로컬로가야하기때문에 지형월드행렬의 역행렬이 필요하다) 시켜서 비교해야겠다. */
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
