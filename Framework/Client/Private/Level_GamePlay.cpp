#include "stdafx.h"
#include "Level_GamePlay.h"

#include "LandObject.h"
#include "Obstacle.h"
#include "Camera_manager.h"



CLevel_GamePlay::CLevel_GamePlay(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_GamePlay::Initialize()
{
	if (FAILED(Ready_Lights()))
		return E_FAIL;
	
	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	if (FAILED(Ready_LandObjects()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Obstacle(TEXT("Layer_Obstacle"))))
		return E_FAIL;
	

	ShowCursor(FALSE);

	return S_OK;
}

void CLevel_GamePlay::Tick(_float fTimeDelta)
{
	CCamera_manager::GetInstance()->Tick(fTimeDelta);
}

HRESULT CLevel_GamePlay::Render()
{
#ifdef _DEBUG
	SetWindowText(g_hWnd, TEXT("Playing"));
#endif

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Lights()
{
	LIGHT_DESC		LightDesc = {};

	LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
	LightDesc.vDiffuse = _float4(1.0f, 1.0f, 1.0f, 1.f);
	LightDesc.vAmbient = _float4(0.1f, 0.1f, 0.1f, 1.f);
	LightDesc.vSpecular = LightDesc.vDiffuse;

	if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Camera(const wstring & strLayerTag)
{
	CCamera_Free::CAMERA_FREE_DESC		CameraDesc_Free = {};

	CameraDesc_Free.fMouseSensor = 0.05f;
	CameraDesc_Free.vEye = _float4(0.0f, 30.f, -25.f, 1.f);
	CameraDesc_Free.vAt = _float4(0.0f, 0.0f, 0.f, 1.f);
	CameraDesc_Free.fFovy = XMConvertToRadians(60.0f);
	CameraDesc_Free.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	CameraDesc_Free.fNear = 0.2f;
	CameraDesc_Free.fFar = 1000.f;
	CameraDesc_Free.fSpeedPerSec = 50.f;
	CameraDesc_Free.fRotationPerSec = XMConvertToRadians(90.0f);

	CGameObject* pCamera_Free = m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Camera_Free"), &CameraDesc_Free);
	CCamera_manager::GetInstance()->Add_Camera("Camera_Free", dynamic_cast<CCamera*>(pCamera_Free));

	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, strLayerTag, pCamera_Free)))
		return E_FAIL;


	CCamera_ThirdPerson::CAMERA_THIRDPERSON_DESC CameraDesc_ThirdPerson = {};

	CameraDesc_ThirdPerson.fMouseSensor = 0.05f;
	CameraDesc_ThirdPerson.vEye = _float4(0.0f, 30.f, -25.f, 1.f);
	CameraDesc_ThirdPerson.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	CameraDesc_ThirdPerson.fFovy = XMConvertToRadians(60.0f);
	CameraDesc_ThirdPerson.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	CameraDesc_ThirdPerson.fNear = 0.2f;
	CameraDesc_ThirdPerson.fFar = 1000.f;
	CameraDesc_ThirdPerson.fSpeedPerSec = 20.f;
	CameraDesc_ThirdPerson.fRotationPerSec = XMConvertToRadians(90.f);

	CGameObject* pCamera_ThirdPerson = m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Camera_ThirdPerson"), &CameraDesc_ThirdPerson);
	CCamera_manager::GetInstance()->Add_Camera("Camera_ThirdPerson", dynamic_cast<CCamera*>(pCamera_ThirdPerson));
	CCamera_manager::GetInstance()->Change_Camera("Camera_ThirdPerson");

	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, strLayerTag, pCamera_ThirdPerson)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Player(const wstring & strLayerTag, CLandObject::LANDOBJ_DESC& LandObjDesc)
{
	if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Player"), &LandObjDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Static_Object(const wstring & strLayerTag, CLandObject::LANDOBJ_DESC& LandObjDesc)
{
	
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Obstacle(const wstring& strLayerTag)
{
	CObstacle::OBSTACLE_DESC  Obstacle_Desc{};

	_float	fTemp = 10.f;
	_float	fHeight = 4.f;
	_int	iHeightCount = 0;
	for (_int i = 0; i < 15; ++i)
	{
		for (_int j = 0; j < 15; ++j)
		{
			Obstacle_Desc.vStartpos = _float3(5.f + fTemp * i, 6.f + fHeight* iHeightCount, 5.f + fTemp * j);
			if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Obstacle"), &Obstacle_Desc)))
				return E_FAIL;
			++iHeightCount;
			if (iHeightCount >= 6)
				iHeightCount = 0;
		}
	}

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_BackGround(const wstring & strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Terrain_Basic"))))
		return E_FAIL;

	//if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Sky"))))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Effect(const wstring & strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Particle_Rect"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Particle_Point"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_LandObjects()
{
	CLandObject::LANDOBJ_DESC			LandObjDesc{};	

	LandObjDesc.pTerrainTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_BackGround"), g_strTransformTag));
	LandObjDesc.pTerrainVIBuffer = dynamic_cast<CVIBuffer_Terrain_Basic*>(m_pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_BackGround"), TEXT("Com_VIBuffer")));

	if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"), LandObjDesc)))
		return E_FAIL;

	if (FAILED(Ready_Layer_Static_Object(TEXT("Layer_LandObject"), LandObjDesc)))
		return E_FAIL;

	return S_OK;
}

CLevel_GamePlay * CLevel_GamePlay::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CLevel_GamePlay*		pInstance = new CLevel_GamePlay(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_GamePlay");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_GamePlay::Free()
{
	CCamera_manager::GetInstance()->Release_Manager();
	__super::Free();

}
