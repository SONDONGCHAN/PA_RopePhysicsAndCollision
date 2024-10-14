#include "stdafx.h"
#include "Camera.h"
#include "Camera_manager.h"

IMPLEMENT_SINGLETON(CCamera_manager)

CCamera_manager::CCamera_manager()
{
}

CCamera_manager::~CCamera_manager()
{
}

void CCamera_manager::Tick(_float fTimeDelta)
{
	if (CGameInstance::GetInstance()->KeyDown(DIK_P))
	{
		Change_Camera("Camera_ThirdPerson");
	}
	else if (CGameInstance::GetInstance()->KeyDown(DIK_O))
	{
		Change_Camera("Camera_Free");
	}
}

void CCamera_manager::Add_Camera(const string& _strCameraTag, CCamera* _pCamera)
{
	CCamera* pCamera = FindCamera(_strCameraTag);

	if (!pCamera && _pCamera)
	{
		m_Cameras.insert({ _strCameraTag, _pCamera });
		Safe_AddRef(pCamera);
	}
}

void CCamera_manager::Change_Camera(const string& _strCameraTag)
{
	Set_Camera(FindCamera(_strCameraTag));
}

void CCamera_manager::Set_Camera(CCamera* _pCamera)
{
	if (_pCamera == m_pCurrent_Camara)
		return;

	if (m_pCurrent_Camara)
	{
		m_pCurrent_Camara->ChangeRecording();
		Safe_Release(m_pCurrent_Camara);
	}

	_pCamera->ChangeRecording();
	m_pCurrent_Camara = _pCamera;
	Safe_AddRef(_pCamera);
}

void CCamera_manager::Next_Camera()
{

}

void CCamera_manager::Release_Manager()
{
	DestroyInstance();
}

CCamera* CCamera_manager::FindCamera(const string& _strCameraTag)
{
	auto iter = m_Cameras.find(_strCameraTag);
	if (m_Cameras.end() == iter)
		return nullptr;

	return iter->second;
}

void CCamera_manager::Free()
{
	Safe_Release(m_pCurrent_Camara);

	for (auto& Pair : m_Cameras)
		Safe_Release(Pair.second);
	m_Cameras.clear();
}
