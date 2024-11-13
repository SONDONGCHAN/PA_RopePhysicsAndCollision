#pragma once

#include "Transform.h"

/* 클라이언트에서 제작될 다양한객체들의 부모가 되는 클래스. */

BEGIN(Engine)

class ENGINE_DLL CGameObject abstract : public CBase
{
public:
	typedef struct : public CTransform::TRANSFORM_DESC
	{
		_uint		iGameObjectData = { 0 };
	}GAMEOBJECT_DESC;

	struct ColData
	{
		CGameObject* pGameObject = { nullptr };
		COL_TYPE	eMyColType;
		_uint		iTargetColType;
		_bool		isDead = { false };
		class CCollider* pCol{ nullptr };
	};

protected:
	CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject(const CGameObject& rhs);
	virtual ~CGameObject() = default;

public:
	class CComponent* Get_Component(const wstring& strComTag);
	CTransform* Get_TranformCom() { return m_pTransformCom; }

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Tick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual HRESULT Render_Shadow() { return S_OK; }

public:
	virtual void Event_CollisionEnter(ColData* _ColData) {};
	virtual void Event_CollisionStay(ColData* _ColData) {};
	virtual void Event_CollisionExit(ColData* _ColData) {};

public:
	_bool	Get_isEnable() { return m_isEnable; }
	void	Set_isEnable(_bool _isEnable) { m_isEnable = _isEnable; }

protected:
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };

	class CGameInstance*		m_pGameInstance = { nullptr };
	class CTransform*			m_pTransformCom = { nullptr };

protected:
	map<const wstring, class CComponent*>		m_Components;
	_uint	m_iGameObjectData = { 0 };

protected:
	_bool	m_isEnable{ true };

protected:
	HRESULT Add_Component(_uint iLevelIndex, const wstring& strPrototypeTag, const wstring& strComTag, CComponent** ppOut, void* pArg = nullptr);


public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END