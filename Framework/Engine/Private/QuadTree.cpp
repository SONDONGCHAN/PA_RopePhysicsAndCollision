#include "..\Public\QuadTree.h"
#include "GameInstance.h"

CQuadTree::CQuadTree()
	: m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CQuadTree::Initialize(_uint iLT, _uint iRT, _uint iRB, _uint iLB)
{
	m_iCorners[CORNER_LT] = iLT;
	m_iCorners[CORNER_RT] = iRT;
	m_iCorners[CORNER_RB] = iRB;
	m_iCorners[CORNER_LB] = iLB;

	if (1 == m_iCorners[CORNER_RT] - m_iCorners[CORNER_LT])
		return S_OK;

	m_iCenter = (iLT + iRB) >> 1;

	_uint		iLC, iTC, iRC, iBC;

	iLC = (m_iCorners[CORNER_LT] + m_iCorners[CORNER_LB]) >> 1;
	iTC = (m_iCorners[CORNER_LT] + m_iCorners[CORNER_RT]) >> 1;
	iRC = (m_iCorners[CORNER_RT] + m_iCorners[CORNER_RB]) >> 1;
	iBC = (m_iCorners[CORNER_LB] + m_iCorners[CORNER_RB]) >> 1;

	m_pChilds[CORNER_LT] = CQuadTree::Create(m_iCorners[CORNER_LT], iTC, m_iCenter, iLC);
	m_pChilds[CORNER_RT] = CQuadTree::Create(iTC, m_iCorners[CORNER_RT], iRC, m_iCenter);
	m_pChilds[CORNER_RB] = CQuadTree::Create(m_iCenter, iRC, m_iCorners[CORNER_RB], iBC);
	m_pChilds[CORNER_LB] = CQuadTree::Create(iLC, m_iCenter, iBC, m_iCorners[CORNER_LB]);
	
	return S_OK;
}

void CQuadTree::Culling(const _float3 * pVerticesPos, _uint * pIndices, _uint * pNumIndices)
{
	if (nullptr == m_pChilds[CORNER_LT] || 
		true == isDraw(pVerticesPos))
	{
		_bool		isIn[4] = {
			m_pGameInstance->isIn_Frustum_LocalSpace(XMLoadFloat3(&pVerticesPos[m_iCorners[CORNER_LT]])),
			m_pGameInstance->isIn_Frustum_LocalSpace(XMLoadFloat3(&pVerticesPos[m_iCorners[CORNER_RT]])),
			m_pGameInstance->isIn_Frustum_LocalSpace(XMLoadFloat3(&pVerticesPos[m_iCorners[CORNER_RB]])),
			m_pGameInstance->isIn_Frustum_LocalSpace(XMLoadFloat3(&pVerticesPos[m_iCorners[CORNER_LB]])),
		};

		_bool		isDraw[NEIGHBOR_END] = { true, true, true, true };

		for (size_t i = 0; i < NEIGHBOR_END; i++)
		{
			if (nullptr != m_pNeighbors[i])
				isDraw[i] = m_pNeighbors[i]->isDraw(pVerticesPos);
		}

		if (true == isDraw[NEIGHBOR_LEFT] &&
			true == isDraw[NEIGHBOR_TOP] &&
			true == isDraw[NEIGHBOR_RIGHT] &&
			true == isDraw[NEIGHBOR_BOTTOM])
		{
			/*오른쪽 위 삼각형이 절두체 안에 있냐 ? */
			if (true == isIn[0] ||
				true == isIn[1] ||
				true == isIn[2])
			{
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_LT];
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_RT];
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_RB];
			}

			/*왼쪽 아래 삼각형이 절두체 안에 있냐 ? */
			if (true == isIn[0] ||
				true == isIn[2] ||
				true == isIn[3])
			{
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_LT];
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_RB];
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_LB];
			}
			return;
		}

		_uint		iLC, iTC, iRC, iBC;

		iLC = (m_iCorners[CORNER_LT] + m_iCorners[CORNER_LB]) >> 1;
		iTC = (m_iCorners[CORNER_LT] + m_iCorners[CORNER_RT]) >> 1;
		iRC = (m_iCorners[CORNER_RT] + m_iCorners[CORNER_RB]) >> 1;
		iBC = (m_iCorners[CORNER_LB] + m_iCorners[CORNER_RB]) >> 1;

		if (true == isIn[0] ||
			true == isIn[2] ||
			true == isIn[3])
		{

			if (false == isDraw[NEIGHBOR_LEFT])
			{
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_LT];
				pIndices[(*pNumIndices)++] = m_iCenter;
				pIndices[(*pNumIndices)++] = iLC;

				pIndices[(*pNumIndices)++] = iLC;
				pIndices[(*pNumIndices)++] = m_iCenter;
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_LB];
			}
			else
			{
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_LT];
				pIndices[(*pNumIndices)++] = m_iCenter;
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_LB];
			}

			if (false == isDraw[NEIGHBOR_BOTTOM])
			{
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_LB];
				pIndices[(*pNumIndices)++] = m_iCenter;
				pIndices[(*pNumIndices)++] = iBC;

				pIndices[(*pNumIndices)++] = iBC;
				pIndices[(*pNumIndices)++] = m_iCenter;
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_RB];
			}
			else
			{
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_LB];
				pIndices[(*pNumIndices)++] = m_iCenter;
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_RB];
			}
		}

		if (true == isIn[0] ||
			true == isIn[1] ||
			true == isIn[2])
		{
			if (false == isDraw[NEIGHBOR_TOP])
			{
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_LT];
				pIndices[(*pNumIndices)++] = iTC;
				pIndices[(*pNumIndices)++] = m_iCenter;

				pIndices[(*pNumIndices)++] = m_iCenter;
				pIndices[(*pNumIndices)++] = iTC;
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_RT];
			}
			else
			{
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_LT];
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_RT];
				pIndices[(*pNumIndices)++] = m_iCenter;
			}

			if (false == isDraw[NEIGHBOR_RIGHT])
			{
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_RT];
				pIndices[(*pNumIndices)++] = iRC;
				pIndices[(*pNumIndices)++] = m_iCenter;

				pIndices[(*pNumIndices)++] = m_iCenter;
				pIndices[(*pNumIndices)++] = iRC;
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_RB];
			}
			else
			{
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_RT];
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_RB];
				pIndices[(*pNumIndices)++] = m_iCenter;
			}
		}

		return;
	}
	
	_float		fRadius = { XMVectorGetX(XMVector3Length(XMLoadFloat3(&pVerticesPos[m_iCenter]) - XMLoadFloat3(&pVerticesPos[m_iCorners[CORNER_LT]]))) };

	if (true == m_pGameInstance->isIn_Frustum_LocalSpace(XMLoadFloat3(&pVerticesPos[m_iCenter]), fRadius))
	{
		for (size_t i = 0; i < CORNER_END; i++)
		{
			if (nullptr != m_pChilds[i])
				m_pChilds[i]->Culling(pVerticesPos, pIndices, pNumIndices);
		}
	}
}

void CQuadTree::Make_Neighbors()
{
	if (nullptr == m_pChilds[CORNER_LT]->m_pChilds[CORNER_LT])
		return;

	m_pChilds[CORNER_LT]->m_pNeighbors[NEIGHBOR_RIGHT] = m_pChilds[CORNER_RT];
	m_pChilds[CORNER_LT]->m_pNeighbors[NEIGHBOR_BOTTOM] = m_pChilds[CORNER_LB];

	m_pChilds[CORNER_RT]->m_pNeighbors[NEIGHBOR_LEFT] = m_pChilds[CORNER_LT];
	m_pChilds[CORNER_RT]->m_pNeighbors[NEIGHBOR_BOTTOM] = m_pChilds[CORNER_RB];

	m_pChilds[CORNER_RB]->m_pNeighbors[NEIGHBOR_LEFT] = m_pChilds[CORNER_LB];
	m_pChilds[CORNER_RB]->m_pNeighbors[NEIGHBOR_TOP] = m_pChilds[CORNER_RT];

	m_pChilds[CORNER_LB]->m_pNeighbors[NEIGHBOR_RIGHT] = m_pChilds[CORNER_RB];
	m_pChilds[CORNER_LB]->m_pNeighbors[NEIGHBOR_TOP] = m_pChilds[CORNER_LT];

	if (nullptr != m_pNeighbors[NEIGHBOR_RIGHT])
	{
		m_pChilds[CORNER_RT]->m_pNeighbors[NEIGHBOR_RIGHT] = m_pNeighbors[NEIGHBOR_RIGHT]->m_pChilds[CORNER_LT];
		m_pChilds[CORNER_RB]->m_pNeighbors[NEIGHBOR_RIGHT] = m_pNeighbors[NEIGHBOR_RIGHT]->m_pChilds[CORNER_LB];
	}

	if (nullptr != m_pNeighbors[NEIGHBOR_BOTTOM])
	{
		m_pChilds[CORNER_LB]->m_pNeighbors[NEIGHBOR_BOTTOM] = m_pNeighbors[NEIGHBOR_BOTTOM]->m_pChilds[CORNER_LT];
		m_pChilds[CORNER_RB]->m_pNeighbors[NEIGHBOR_BOTTOM] = m_pNeighbors[NEIGHBOR_BOTTOM]->m_pChilds[CORNER_RT];
	}

	if (nullptr != m_pNeighbors[NEIGHBOR_LEFT])
	{
		m_pChilds[CORNER_LT]->m_pNeighbors[NEIGHBOR_LEFT] = m_pNeighbors[NEIGHBOR_LEFT]->m_pChilds[CORNER_RT];
		m_pChilds[CORNER_LB]->m_pNeighbors[NEIGHBOR_LEFT] = m_pNeighbors[NEIGHBOR_LEFT]->m_pChilds[CORNER_RB];
	}

	if (nullptr != m_pNeighbors[NEIGHBOR_TOP])
	{
		m_pChilds[CORNER_LT]->m_pNeighbors[NEIGHBOR_TOP] = m_pNeighbors[NEIGHBOR_TOP]->m_pChilds[CORNER_LB];
		m_pChilds[CORNER_RT]->m_pNeighbors[NEIGHBOR_TOP] = m_pNeighbors[NEIGHBOR_TOP]->m_pChilds[CORNER_RB];
	}

	for (_uint i = 0; i < CORNER_END; i++)
	{
		if(nullptr != m_pChilds[i])
			m_pChilds[i]->Make_Neighbors();
	}
}


_bool CQuadTree::isDraw(const _float3 * pVertices)
{
	_float		fCamDistance = XMVector3Length(XMLoadFloat4(&m_pGameInstance->Get_CamPosition()) - XMLoadFloat3(&pVertices[m_iCenter])).m128_f32[0];
	_float		fWidth = m_iCorners[CORNER_RT] - m_iCorners[CORNER_LT];

	if (fCamDistance * 0.1f > fWidth)
		return true;

	return false;
}

CQuadTree * CQuadTree::Create(_uint iLT, _uint iRT, _uint iRB, _uint iLB)
{
	CQuadTree*		pInstance = new CQuadTree();

	if (FAILED(pInstance->Initialize(iLT, iRT, iRB, iLB)))
	{
		MSG_BOX("Failed to Created : CQuadTree");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CQuadTree::Free()
{
	for (auto& pQuadTree : m_pChilds)
		Safe_Release(pQuadTree);

	Safe_Release(m_pGameInstance);
}
