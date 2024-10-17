#pragma once

#include "Client_Defines.h"
#include "LandObject.h"

BEGIN(Engine)

class CCollider;
class CNavigation;

END


BEGIN(Client)

class CPlayer final : public CLandObject
{
public:


private:
	CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer(const CPlayer& rhs);
	virtual ~CPlayer() = default;

public:
	class CComponent* Get_PartObjectComponent(const wstring& strPartObjTag, const wstring& strComTag);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	void	KeyInput(_float fTimeDelta);
	void	Root_Transform(_float fTimeDelta);

private:
	CNavigation*				m_pNavigationCom = { nullptr };
	CCollider*					m_pColliderCom = { nullptr };
	_uint						m_iState = { 0 };

private:
	map<const wstring, class CPartObject*>		m_PlayerParts;

private:
	HRESULT Add_Components();
	HRESULT Add_PartObjects();

public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END