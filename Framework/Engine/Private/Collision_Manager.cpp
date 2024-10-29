#include "Collision_Manager.h"
#include "Collider_Layer.h"
#include "Collider.h"

CCollision_Manager::CCollision_Manager()
{
}

HRESULT CCollision_Manager::Initialize()
{
	return S_OK;
}

_bool CCollision_Manager::Add_Collider(CCollider* _pCollider)
{
	if (_pCollider == nullptr)
		return false;
	
	CCollider_Layer* pCollider_Layer = Find_Collider_Layer(_pCollider);

	if (nullptr == pCollider_Layer)
	{
		pCollider_Layer = CCollider_Layer::Create();

		if (nullptr == pCollider_Layer)
			return false;

		pCollider_Layer->Add_Collider(_pCollider);

		m_ColliderLayers.emplace(_pCollider->Get_ColData()->eMyColType, pCollider_Layer);
	}
	else
	{
		pCollider_Layer->Add_Collider(_pCollider);
	}

	return true;
}

void CCollision_Manager::Check_Collision()
{
	for (auto& pair : m_ColliderLayers)
	{
		unordered_set<CCollider*>* pColliders = pair.second->Get_Colliders();

		for (auto& pMy : (*pColliders))
		{
			pMy->Reset_Iscoll();
		}
	}

	for (auto& pair : m_ColliderLayers)
	{
		unordered_set<CCollider*>*	pColliders = pair.second->Get_Colliders();

		for (auto&  pMy : (*pColliders) )
		{
			CGameObject::ColData* pData = pMy->Get_ColData();
			
			if (pData->isDead)
				continue;

			_uint iTargetColType = pData->iTargetColType;
			_uint iTemp = 1;
			
			for (_uint i = 0; i < 32; ++i)
			{
				_uint eTargetColType = iTemp << i;

				if ((iTargetColType & eTargetColType) != eTargetColType)
					continue;

				if (m_ColliderLayers.find(eTargetColType) == m_ColliderLayers.end())
					continue;				

				unordered_set<CCollider*>* pTargetColliders = m_ColliderLayers[eTargetColType]->Get_Colliders();

				int count = 0;
				for (auto pTarget : (*pTargetColliders))
				{
					if (pMy->Intersect(pTarget))
					{
						if (pMy->Find_CurrentCollision(pTarget))
						{
							pMy->CollisionStay(pTarget);
						}
						else
						{
							pMy->CollisionEnter(pTarget);
						}
					}
					else
					{
						if (pMy->Find_CurrentCollision(pTarget))
						{
							pMy->CollisionExit(pTarget);
						}
						else
						{

						}
					}
					++count;
				}
			}

		}	
	}
}

CCollider_Layer* CCollision_Manager::Find_Collider_Layer(CCollider* _pCollider)
{
	auto	iter = (m_ColliderLayers.find(_pCollider->Get_ColData()->eMyColType));

	if (iter == m_ColliderLayers.end())
		return nullptr;

	return iter->second;
}

CCollision_Manager* CCollision_Manager::Create()
{
	CCollision_Manager* pInstance = new CCollision_Manager();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CCollision_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCollision_Manager::Free()
{
	for (auto& Pair : m_ColliderLayers)
		Safe_Release(Pair.second);

	m_ColliderLayers.clear();
}
