#pragma once
#include "Base.h"

BEGIN(Engine)

class CCollider_Layer : public CBase
{
private:
	CCollider_Layer();
	virtual ~CCollider_Layer() = default;

public:
	HRESULT Initialize();


public:
	_bool	Add_Collider(class CCollider* pCollider);
	unordered_set<CCollider*>* Get_Colliders() { return &m_Colliders; }

private:
	unordered_set<CCollider*>	m_Colliders;

public:
	static CCollider_Layer* Create();
	virtual void	Free() override;
};

END