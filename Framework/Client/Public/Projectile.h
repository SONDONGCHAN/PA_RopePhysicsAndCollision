#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CProjectile : public CGameObject
{
public:
	struct PROJECTILE_DESC : public GAMEOBJECT_DESC
	{
		CGameObject* pOwnerObject;
	};

protected:
	CProjectile(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CProjectile(const CProjectile& rhs);
	virtual ~CProjectile() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void	Enable_Projectile(_vector _vMyStartPos, _vector _vTargetDir ) {};
	virtual void	Disable_Projectile() {};

protected:
	CGameObject* m_pOwnerObject{ nullptr };

	_float	m_fMaxLifeTime = 1.f;
	_float	m_fCurrentLifeTime = 0.f;


public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END