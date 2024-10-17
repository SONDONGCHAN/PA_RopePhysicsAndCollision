#include "..\Public\Camera.h"

#include "GameInstance.h"


CCamera::CCamera(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CCamera::CCamera(const CCamera & rhs)
	: CGameObject(rhs)
{
}

HRESULT CCamera::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCamera::Initialize(void * pArg)
{
	CAMERA_DESC*		pCameraDesc = (CAMERA_DESC*)pArg;

	m_fFovy = pCameraDesc->fFovy;
	m_fNear = pCameraDesc->fNear;
	m_fFar = pCameraDesc->fFar;
	m_fAspect = pCameraDesc->fAspect;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&pCameraDesc->vEye));
	m_pTransformCom->LookAt(XMLoadFloat4(&pCameraDesc->vAt));

	return S_OK;
}

void CCamera::Priority_Tick(_float fTimeDelta)
{

}

void CCamera::Tick(_float fTimeDelta)
{
}

void CCamera::Late_Tick(_float fTimeDelta)
{
	if (m_isRecording)
	{
		if (FAILED(SetUp_TransformMatices()))
			return;
	}
}

HRESULT CCamera::Render()
{
	return S_OK;
}

HRESULT CCamera::SetUp_TransformMatices()
{
	m_pGameInstance->Set_Transform(CPipeLine::D3DTS_VIEW, m_pTransformCom->Get_WorldMatrix_Inverse());
	m_pGameInstance->Set_Transform(CPipeLine::D3DTS_PROJ, XMMatrixPerspectiveFovLH(m_fFovy, m_fAspect, m_fNear, m_fFar));

	return S_OK;
}

void CCamera::Free()
{
	__super::Free();

}
