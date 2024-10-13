#include "stdafx.h"
#include "Camera.h"
#include "Camera_manager.h"

IMPLEMENT_SINGLETON(CCamera_manager)

CCamera_manager::CCamera_manager()
{
}

CCamera_manager::~CCamera_manager()
{
	Free();
}

void CCamera_manager::Tick(_float fTimeDelta)
{
	if (GetKeyState('P') & 0x8000)
	{
		Change_Camera("Camera_ThirdPerson");
	}

	else if (GetKeyState('O') & 0x8000)
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

CCamera* CCamera_manager::FindCamera(const string& _strCameraTag)
{
	auto iter = m_Cameras.find(_strCameraTag);
	if (m_Cameras.end() == iter)
		return nullptr;

	return iter->second;
}

void CCamera_manager::Free()
{
	for (auto& Pair : m_Cameras)
		Safe_Release(Pair.second);
	m_Cameras.clear();

	DestroyInstance();
}
