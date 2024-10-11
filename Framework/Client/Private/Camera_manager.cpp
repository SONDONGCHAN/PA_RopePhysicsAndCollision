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
