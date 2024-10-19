#pragma once

#include "Component.h"
#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CCollider final : public CComponent
{
public:
	enum TYPE { TYPE_SPHERE, TYPE_AABB, TYPE_OBB, TYPE_END };
	enum COL_TYPE { COL_PLAYER, COL_STATIC_OBJECT, COL_PROJECTILE, COL_END };

	struct ColData
	{
		CGameObject* pGameObject = { nullptr };
		COL_TYPE	eMyColType;
		COL_TYPE	eTargetColType;
		_bool		isDead = { false };
	};

private:
	CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCollider(const CCollider& rhs);
	virtual ~CCollider() = default;

public:
	virtual HRESULT Initialize_Prototype(TYPE eType);
	virtual HRESULT Initialize(void* pArg) override;

public:
	void Tick(_fmatrix WorldMatrix);

public:
	_bool	Find_CurrentCollision(CCollider* pTarget_Collider);
	_bool	Delete_CurrentCollision(CCollider* pTarget_Collider);
	void	Clear_Collisions();

	void	CollisionEnter();
	void	CollisionStay();
	void	CollisionExit();

public:
	ColData* Get_ColData() { return &m_ColData;};

#ifdef _DEBUG
public:
	virtual HRESULT Render() override;
#endif // _DEBUG

public:
	_bool Intersect(CCollider* pTargetCollider);

#ifdef _DEBUG
private:
	PrimitiveBatch<VertexPositionColor>*			m_pBatch = { nullptr };
	BasicEffect*									m_pEffect = { nullptr };
	ID3D11InputLayout*								m_pInputLayout = { nullptr };
#endif

public:
	static CCollider* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eType);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

private:
	TYPE			m_eType = { TYPE_END };
	class CBounding* m_pBounding = { nullptr };

	unordered_set<CCollider*> m_CurrentCollisions;
	ColData m_ColData;
};

END