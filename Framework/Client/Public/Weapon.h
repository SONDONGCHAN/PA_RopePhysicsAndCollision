#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

/* 플레이어의 각 파츠를 모아서 가지고 있는 객체. */

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CWeapon final : public CPartObject
{
public:
	typedef struct : public CPartObject::PARTOBJ_DESC
	{
		_float4x4*	pSocketMatrix;
	}WEAPON_DESC;
private:
	CWeapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWeapon(const CWeapon& rhs);
	virtual ~CWeapon() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader*			m_pShaderCom = { nullptr };	
	CModel*				m_pModelCom = { nullptr };
	CCollider*			m_pColliderCom = { nullptr };

private:
	_float4x4*			m_pSocketMatrix = { nullptr };

private:
	HRESULT Add_Component();
	HRESULT Bind_ShaderResources();

public:
	static CWeapon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END