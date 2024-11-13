#pragma once

#include "Component.h"
#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CCollider final : public CComponent
{
public:
	enum TYPE { TYPE_SPHERE, TYPE_AABB, TYPE_OBB, TYPE_CYLINDER, TYPE_END };

	struct BOUNDING_DESC
	{
		_float3		vCenter;
	};

	struct SPHERE_DESC : public BOUNDING_DESC
	{
		_float		fRadius;
	};

	 struct OBB_DESC : public BOUNDING_DESC
	{
		_float3		vExtents;
		_float3		vRadians; /* x : x축 기준 회전 각도, y : y축 기준 회전 각도, z : z축 기준 회전 각도 */
	};

	struct AABB_DESC : public BOUNDING_DESC
	{
		_float3		vExtents;
	};

	struct CYLINDER_DESC : public BOUNDING_DESC
	{
		_float		fRadius;
		_float		fHeight;
		_float3		vDir;
	};

	struct ColliderInitData
	{
		CGameObject::ColData ColData;

		// 각 타입별 초기화 데이터를 위한 유니온
		union
		{
			AABB_DESC AABBDesc;
			OBB_DESC OBBDesc;
			SPHERE_DESC SphereDesc;
			CYLINDER_DESC SylinderDesc;
		};
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

	void	CollisionEnter(CCollider* pTarget_Collider);
	void	CollisionStay(CCollider* pTarget_Collider);
	void	CollisionExit(CCollider* pTarget_Collider);

public:
	CGameObject::ColData* Get_ColData() { return &m_ColData;};

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

public:
	void	Reset_Iscoll();
	class CBounding* Get_Bounding() { return m_pBounding; }


private:
	TYPE			m_eType = { TYPE_END };
	CBounding* m_pBounding = { nullptr };

	unordered_set<CCollider*> m_CurrentCollisions;
	CGameObject:: ColData m_ColData;
};

END