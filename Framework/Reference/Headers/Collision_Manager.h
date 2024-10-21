#pragma once
#include "Base.h"
#include "Collider.h"

BEGIN(Engine)

class CCollision_Manager : public CBase
{
private:
	CCollision_Manager();
	virtual ~CCollision_Manager() = default;

public:
	HRESULT Initialize();

public:
	_bool	Add_Collider(CCollider* _pCollider);
	void	Check_Collision();

private:
	class CCollider_Layer* Find_Collider_Layer(CCollider* _pCollider);


	unordered_map<_uint, CCollider_Layer*> m_ColliderLayers;

public:
	static CCollision_Manager* Create();
	virtual void Free() override;
};

END