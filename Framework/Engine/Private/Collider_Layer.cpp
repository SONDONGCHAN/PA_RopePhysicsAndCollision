#include "Collider_Layer.h"
#include "Collider.h"

CCollider_Layer::CCollider_Layer()
{
}

HRESULT CCollider_Layer::Initialize()
{
	return S_OK;
}

_bool CCollider_Layer::Add_Collider(CCollider* pCollider)
{
	if (nullptr == pCollider || (m_Colliders.find(pCollider) != m_Colliders.end()) )
		return false;

	m_Colliders.insert(pCollider);
	Safe_AddRef(pCollider);

	return true;
}

CCollider_Layer* CCollider_Layer::Create()
{
	CCollider_Layer* pInstance = new CCollider_Layer();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CCollider_Layer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCollider_Layer::Free()
{
	for (auto pCollider : m_Colliders)
		Safe_Release(pCollider);

	m_Colliders.clear();
}
