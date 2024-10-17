#include "stdafx.h"
#include "..\Public\Camera_Free.h"

CCamera_Free::CCamera_Free(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CCamera(pDevice, pContext)
{

}

CCamera_Free::CCamera_Free(const CCamera_Free & rhs)
	: CCamera(rhs)
{
}

HRESULT CCamera_Free::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCamera_Free::Initialize(void * pArg)
{
	CAMERA_FREE_DESC*		pCameraDesc = (CAMERA_FREE_DESC*)pArg;
	
	m_fMouseSensor = pCameraDesc->fMouseSensor;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CCamera_Free::Priority_Tick(_float fTimeDelta)
{

	__super::Priority_Tick(fTimeDelta);
}

void CCamera_Free::Tick(_float fTimeDelta)
{

	__super::Tick(fTimeDelta);
}

void CCamera_Free::Late_Tick(_float fTimeDelta)
{

	if (m_isRecording && m_isFixed)
	{
		KeyInput(fTimeDelta);
	}
	__super::Late_Tick(fTimeDelta);

}

HRESULT CCamera_Free::Render()
{
	return S_OK;
}

void CCamera_Free::CursorFixCenter()
{
	POINT	ptMouse{ g_iWinSizeX >> 1, g_iWinSizeY >> 1 };

	ClientToScreen(g_hWnd, &ptMouse);
	SetCursorPos(ptMouse.x, ptMouse.y);
}

void CCamera_Free::KeyInput(_float fTimeDelta)
{
	if (CGameInstance::GetInstance()->KeyPressing(DIK_UP))
	{
		m_pTransformCom->Go_Straight(fTimeDelta);
	}
	if (CGameInstance::GetInstance()->KeyPressing(DIK_DOWN))
	{
		m_pTransformCom->Go_Backward(fTimeDelta);
	}
	if (CGameInstance::GetInstance()->KeyPressing(DIK_LEFT))
	{
		m_pTransformCom->Go_Left(fTimeDelta);
	}
	if (CGameInstance::GetInstance()->KeyPressing(DIK_RIGHT))
	{
		m_pTransformCom->Go_Right(fTimeDelta);
	}

	_long	MouseMove = { 0 };

	if (MouseMove = m_pGameInstance->Get_DIMouseMove(DIMS_X))
	{
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * MouseMove * m_fMouseSensor);
	}


	if (MouseMove = m_pGameInstance->Get_DIMouseMove(DIMS_Y))
	{
		m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), fTimeDelta * MouseMove * m_fMouseSensor);
	}

	if (m_isFixed)
		CursorFixCenter();
}

CCamera_Free * CCamera_Free::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCamera_Free*		pInstance = new CCamera_Free(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCamera_Free");
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CCamera_Free::Clone(void * pArg)
{
	CCamera_Free*		pInstance = new CCamera_Free(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCamera_Free");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera_Free::Free()
{
	__super::Free();
}
