#include "stdafx.h"
#include "Camera_ThirdPerson.h"
#include "Gameobject.h"

CCamera_ThirdPerson::CCamera_ThirdPerson(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamera(pDevice, pContext)
{
}

CCamera_ThirdPerson::CCamera_ThirdPerson(const CCamera_ThirdPerson& rhs)
	: CCamera(rhs)
{
}

HRESULT CCamera_ThirdPerson::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCamera_ThirdPerson::Initialize(void* pArg)
{
	CAMERA_THIRDPERSON_DESC* pCameraDesc = (CAMERA_THIRDPERSON_DESC*)pArg;
	
	m_fMouseSensor = pCameraDesc->fMouseSensor;
	
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	pTarget_Transform = m_pGameInstance->Get_GameObject(CGameInstance::GetInstance()->Get_CurrentLevel(), TEXT("Layer_Player"))->Get_TranformCom();
	Safe_AddRef(pTarget_Transform);

	return S_OK;
}

void CCamera_ThirdPerson::Priority_Tick(_float fTimeDelta)
{
	if (m_isRecording)
	{
		KeyInput(fTimeDelta);
		CamMove(fTimeDelta);
	}
	__super::Priority_Tick(fTimeDelta);
}

void CCamera_ThirdPerson::Tick(_float fTimeDelta)
{
}

void CCamera_ThirdPerson::Late_Tick(_float fTimeDelta)
{
}

HRESULT CCamera_ThirdPerson::Render()
{
	return S_OK;
}

void CCamera_ThirdPerson::CursorFixCenter()
{
	POINT	ptMouse{ g_iWinSizeX >> 1, g_iWinSizeY >> 1 };

	ClientToScreen(g_hWnd, &ptMouse);
	SetCursorPos(ptMouse.x, ptMouse.y);
}

void CCamera_ThirdPerson::CamMove(_float fTimeDelta)
{
	// 구면 좌표계를 이용하여 카메라의 위치를 잡아주자
	_float3 camPos = Cal_Pos(m_radius, m_degree_Horizontal, m_degree_Vertical);

	// 타켓의 중심으로 회전 할 수 있도록 타겟의 위치를 받아오자
	_float4 revisePos;
	XMStoreFloat4(&revisePos, pTarget_Transform->Get_State(CTransform::STATE_POSITION));

	// 플레이어의 위치보다 살짝 높은곳을 기준으로 잡자
	revisePos.y += 1.5f;

	// 플레이어의 위치를 더해줘서 최종 카메라의 위치를 잡아주자
	_vector TargetPos = XMLoadFloat3(&camPos) + XMLoadFloat4(&revisePos);

	// 카메라의 현재 위치와 목표위치를 선형보간하여 최종 위치를 잡아주자.
	//_vector CurrentCamPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	//_vector Lerp_Pos = XMVectorLerp(CurrentCamPos, TargetPos, 0.2f);

	//  카메라의 현재 방향와 목표방향를 선형보간하여 최종 방향을 잡아주자.
	_vector CamDir = XMVector3Normalize(XMLoadFloat4(&revisePos) - TargetPos);
	//_vector CurrentCamDir = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	//_vector Lerp_Dir = XMVectorLerp(CurrentCamDir, CamDir, 0.2f);

	//  Eye와 At을 설정해주자
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, TargetPos);
	m_pTransformCom->Set_Look(CamDir);
}

_float3 CCamera_ThirdPerson::Cal_Pos(_float radius, _float Horizontal_degree, _float column_degree)
{
	_float3 CameraPos;
	
	CameraPos.x = radius * cos(XMConvertToRadians(column_degree)) * cos(XMConvertToRadians(Horizontal_degree));
	CameraPos.y = radius * sin(XMConvertToRadians(column_degree));
	CameraPos.z = radius * cos(XMConvertToRadians(column_degree)) * sin(XMConvertToRadians(Horizontal_degree));
	
	return CameraPos;
}

void CCamera_ThirdPerson::KeyInput(_float fTimeDelta)
{
	_long	MouseMove = { 0 };
	
	if (MouseMove = m_pGameInstance->Get_DIMouseMove(DIMS_X))
	{
		m_degree_Horizontal -= m_fMouseSensor * MouseMove;
	}
	
	if (MouseMove = m_pGameInstance->Get_DIMouseMove(DIMS_Y))
	{
		m_degree_Vertical += m_fMouseSensor * MouseMove;
	
		if (m_degree_Vertical > Column_Max)
			m_degree_Vertical = Column_Max;
	
		else if (m_degree_Vertical < Column_Min)
			m_degree_Vertical = Column_Min;
	}

	if(m_isFixed)
		CursorFixCenter();
}

CCamera_ThirdPerson* CCamera_ThirdPerson::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCamera_ThirdPerson* pInstance = new CCamera_ThirdPerson(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCamera_ThirdPerson");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCamera_ThirdPerson::Clone(void* pArg)
{
	CCamera_ThirdPerson* pInstance = new CCamera_ThirdPerson(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCamera_ThirdPerson");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera_ThirdPerson::Free()
{
	Safe_Release(pTarget_Transform);

	__super::Free();
}
