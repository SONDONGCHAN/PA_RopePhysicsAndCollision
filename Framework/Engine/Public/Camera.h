#pragma once

#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CCamera abstract : public CGameObject
{
public:
	typedef struct  : public CGameObject::GAMEOBJECT_DESC
	{
		_float4		vEye, vAt;
		_float		fFovy = { 0.0f };
		_float		fNear = { 0.0f };
		_float		fFar = { 0.0f };
		_float		fAspect = { 0.0f };
	}CAMERA_DESC;

protected:
	CCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera(const CCamera& rhs);
	virtual ~CCamera() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void ChangeRecording()	{ m_isRecording = !m_isRecording; }
	void ChangeFixed()		{ m_isFixed		= !m_isFixed; }

protected:
	_float				m_fFovy = { 0.0f };
	_float				m_fNear = { 0.0f };
	_float				m_fFar = { 0.0f };
	_float				m_fAspect = { 0.0f };
	
protected:
	_bool	m_isRecording	= { false };
	_bool	m_isFixed		= { true };

protected:
	HRESULT SetUp_TransformMatices();

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END