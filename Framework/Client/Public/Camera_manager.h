#pragma once

#include "Client_Defines.h"
#include "Camera.h"
#include "Camera_Free.h"
#include "Camera_ThirdPerson.h"

BEGIN(Client)

class CCamera_manager final : public CBase
{
	DECLARE_SINGLETON(CCamera_manager)

public:
	CCamera_manager();
	~CCamera_manager();

public:
	void Add_Camera(const string& _strCameraTag, CCamera* _pCamera);
	void Change_Camera(const string& _strCameraTag);

private:
	CCamera* FindCamera(const string& _strCameraTag);
	
private:
	CCamera* m_pCurrent_Camara = { nullptr };
	map<const string, CCamera* > m_Cameras;

public:
	virtual void Free() override;
};

END