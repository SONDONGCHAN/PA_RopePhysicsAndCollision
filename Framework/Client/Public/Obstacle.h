#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CCollider;
END

BEGIN(Client)

class CObstacle final : public CGameObject
{
public:
	struct OBSTACLE_DESC : public GAMEOBJECT_DESC
	{
		_float3 vStartpos;
	};

private:
	CObstacle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CObstacle(const CObstacle& rhs);
	virtual ~CObstacle() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void Event_CollisionEnter(ColData* _ColData, ColData* _MyColData) override;
	virtual void Event_CollisionStay(ColData* _ColData, ColData* _MyColData) override;
	virtual void Event_CollisionExit(ColData* _ColData, ColData* _MyColData) override;

private:
	CCollider* m_pColliderCom = { nullptr };

private:
	HRESULT Add_Component();

public:
	static CObstacle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END