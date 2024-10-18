#pragma once

#include "Client_Defines.h"
#include "PartObject.h"
#include "Player.h"

/* 플레이어의 각 파츠를 모아서 가지고 있는 객체. */

BEGIN(Engine)
class CCollider;
class CShader;
class CModel;
END

BEGIN(Client)

class CBody_Player final : public CPartObject
{
public:
	typedef struct : public CPartObject::PARTOBJ_DESC
	{

		CPlayer::PlayerAnim*		pPlayerAnimation;

	}BODY_PLAYER_DESC;


private:
	CBody_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBody_Player(const CBody_Player& rhs);
	virtual ~CBody_Player() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Shadow() override;

public:
	_float3 Get_RootTranform() { return m_pSkeletalModelCom->Get_Root_Transform(); }

private:
	CShader*			m_pShaderCom = { nullptr };	
	CShader*			m_pSkeletalShaderCom = { nullptr };
	CModel*				m_pModelCom = { nullptr };
	CModel*				m_pSkeletalModelCom = { nullptr };
	CCollider*			m_pColliderCom = { nullptr };


private:
	CPlayer::PlayerAnim*		m_pPlayerAnimation = { nullptr };

private:
	HRESULT Add_Component();
	HRESULT Bind_ShaderResources();

public:
	static CBody_Player* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END