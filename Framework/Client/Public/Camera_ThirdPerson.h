#pragma once

#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Client)

class CCamera_ThirdPerson : public CCamera
{

public:
	struct CAMERA_THIRDPERSON_DESC : CCamera::CAMERA_DESC
	{
		_float		fMouseSensor = { 0.f };
	} ;


protected:
	CCamera_ThirdPerson(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera_ThirdPerson(const CCamera_ThirdPerson& rhs);
	virtual ~CCamera_ThirdPerson() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	void CursorFixCenter();
	void CamMove(_float fTimeDelta);
	_float3	Cal_Pos(_float radius, _float Horizontal_degree, _float column_degree);
	void KeyInput(_float fTimeDelta);
	
private:
	_float			m_fMouseSensor = { 0.f };		// ���콺 ����

	//_float			m_Target_radius = { 4.f };		// ���� ��ǥ ������ (Target)
	_float			m_radius = { 4.f };				// ���� ��ǥ ������
	_float			m_degree_Horizontal = { 0.f };	// ���� ��ǥ ���� ����
	_float			m_degree_Vertical = { 0.f };	// ���� ��ǥ ���� ����
	_float			Column_Max = { 85.f };			// ���� ���� �ִ�
	_float			Column_Min = { -10.f };			// ���� ���� �ּ�

	CTransform*		pTarget_Transform= { nullptr };

public:
	static CCamera_ThirdPerson* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

END