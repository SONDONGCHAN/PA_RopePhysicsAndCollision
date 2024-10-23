#pragma once
#include "Projectile.h"

BEGIN(Engine)
class CCollider;
END

BEGIN(Client)

class CProjectile_Rope : public CProjectile
{
private:
	CProjectile_Rope(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CProjectile_Rope(const CProjectile& rhs);
	virtual ~CProjectile_Rope() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void	Enable_Projectile(_vector _vMyStartPos, _vector _vTargetDir) override;
	virtual void	Disable_Projectile() override;

public:
	virtual void Event_CollisionEnter(ColData* _ColData) override;
	virtual void Event_CollisionStay(ColData* _ColData) override;
	virtual void Event_CollisionExit(ColData* _ColData) override;

private:
	HRESULT Add_Component();

private:
	CCollider* m_pColliderCom = { nullptr };

public:
	static CProjectile_Rope* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END