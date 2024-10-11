#include "VIBuffer_Terrain_Basic.h"
#include "GameInstance.h"
#include "Transform.h"

CVIBuffer_Terrain_Basic::CVIBuffer_Terrain_Basic(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CVIBuffer_Terrain_Basic::CVIBuffer_Terrain_Basic(const CVIBuffer_Terrain_Basic& rhs)
	: CVIBuffer(rhs)
	, m_iNumVerticesX(rhs.m_iNumVerticesX)
	, m_iNumVerticesZ(rhs.m_iNumVerticesZ)
{
}

HRESULT CVIBuffer_Terrain_Basic::Initialize_Prototype(const _int _iSizeX, const _int _iSizeZ)
{
	m_iNumVerticesX = _iSizeX;
	m_iNumVerticesZ = _iSizeZ;

	m_iNumVertexBuffers = 1;
	m_iVertexStride = sizeof(VTXNORTEX);
	m_iNumVertices = m_iNumVerticesX * m_iNumVerticesZ;
	m_pVerticesPos = new _float3[m_iNumVertices];

	m_iIndexStride = 4;
	m_eIndexFormat = m_iIndexStride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
	m_iNumIndices = (m_iNumVerticesX - 1) * (m_iNumVerticesZ - 1) * 2 * 3;
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region VERTEX_BUFFER

	VTXNORTEX* pVertices = new VTXNORTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXNORTEX) * m_iNumVertices);

	for (size_t i = 0; i < m_iNumVerticesZ; i++)
	{
		for (size_t j = 0; j < m_iNumVerticesX; j++)
		{
			_uint iIndex = i * m_iNumVerticesX + j;

			/*		11111111 11111111 11111111 10110101

			&		00000000 00000000 00000000 11111111

					00000000 00000000 00000000 10110101*/

			pVertices[iIndex].vPosition = m_pVerticesPos[iIndex] = _float3(j, 0, i);
			pVertices[iIndex].vNormal = _float3(0.f, 0.f, 0.f);
			pVertices[iIndex].vTexcoord = _float2(j / (m_iNumVerticesX - 1.f), i / (m_iNumVerticesZ - 1.f));
		}
	}

#pragma endregion

#pragma region INDEX_BUFFER
	/* 인덱스버퍼를 만든다.*/

	_uint* pIndices = new _uint[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_uint) * m_iNumIndices);

	_uint		iNumIndices = { 0 };


	for (size_t i = 0; i < m_iNumVerticesZ - 1; i++)
	{
		for (size_t j = 0; j < m_iNumVerticesX - 1; j++)
		{
			_uint iIndex = j * m_iNumVerticesX + i;

			_uint	iIndices[4] = {
				iIndex + m_iNumVerticesX,
				iIndex + m_iNumVerticesX + 1,
				iIndex + 1,
				iIndex
			};

			_vector vSourDir, vDestDir, vNormal;

			pIndices[iNumIndices++] = iIndices[0];
			pIndices[iNumIndices++] = iIndices[1];
			pIndices[iNumIndices++] = iIndices[2];

			vSourDir = XMLoadFloat3(&pVertices[iIndices[1]].vPosition) - XMLoadFloat3(&pVertices[iIndices[0]].vPosition);
			vDestDir = XMLoadFloat3(&pVertices[iIndices[2]].vPosition) - XMLoadFloat3(&pVertices[iIndices[1]].vPosition);
			vNormal = XMVector3Normalize(XMVector3Cross(vSourDir, vDestDir));

			XMStoreFloat3(&pVertices[iIndices[0]].vNormal,
				XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[0]].vNormal) + vNormal));
			XMStoreFloat3(&pVertices[iIndices[1]].vNormal,
				XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[1]].vNormal) + vNormal));
			XMStoreFloat3(&pVertices[iIndices[2]].vNormal,
				XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[2]].vNormal) + vNormal));


			pIndices[iNumIndices++] = iIndices[0];
			pIndices[iNumIndices++] = iIndices[2];
			pIndices[iNumIndices++] = iIndices[3];

			vSourDir = XMLoadFloat3(&pVertices[iIndices[2]].vPosition) - XMLoadFloat3(&pVertices[iIndices[0]].vPosition);
			vDestDir = XMLoadFloat3(&pVertices[iIndices[3]].vPosition) - XMLoadFloat3(&pVertices[iIndices[2]].vPosition);
			vNormal = XMVector3Normalize(XMVector3Cross(vSourDir, vDestDir));

			XMStoreFloat3(&pVertices[iIndices[0]].vNormal,
				XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[0]].vNormal) + vNormal));
			XMStoreFloat3(&pVertices[iIndices[2]].vNormal,
				XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[2]].vNormal) + vNormal));
			XMStoreFloat3(&pVertices[iIndices[3]].vNormal,
				XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[3]].vNormal) + vNormal));
		}
	}

	/* 정점버퍼를 만든다.*/
	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	m_InitialData.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;


	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;

	m_InitialData.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);
	Safe_Delete_Array(pIndices);

#pragma endregion

	return S_OK;
}

HRESULT CVIBuffer_Terrain_Basic::Initialize(void* pArg)
{
	return S_OK;
}

_float CVIBuffer_Terrain_Basic::Compute_Height(_fvector vLocalPos)
{
	_float3		vTargetPos;
	XMStoreFloat3(&vTargetPos, vLocalPos);

	_uint		iIndex = _uint(vTargetPos.z) * m_iNumVerticesX + _uint(vTargetPos.x);

	_uint		iIndices[4] = {
		iIndex + m_iNumVerticesX,
		iIndex + m_iNumVerticesX + 1,
		iIndex + 1,
		iIndex
	};

	_float		fWidth = vTargetPos.x - m_pVerticesPos[iIndices[0]].x;
	_float		fDepth = m_pVerticesPos[iIndices[0]].z - vTargetPos.z;

	_vector		vPlane;



	/* 오른쪽 위 삼각형 */
	if (fWidth > fDepth)
	{
		vPlane = XMPlaneFromPoints(XMLoadFloat3(&m_pVerticesPos[iIndices[0]]), XMLoadFloat3(&m_pVerticesPos[iIndices[1]]), XMLoadFloat3(&m_pVerticesPos[iIndices[2]]));
	}
	/* 왼쪽 하단 삼각형 */
	else
	{
		vPlane = XMPlaneFromPoints(XMLoadFloat3(&m_pVerticesPos[iIndices[0]]), XMLoadFloat3(&m_pVerticesPos[iIndices[2]]), XMLoadFloat3(&m_pVerticesPos[iIndices[3]]));
	}

	// ax + by + cz + d = 0

	// y = (-ax - cz - d) / b

	return (-vPlane.m128_f32[0] * vTargetPos.x - vPlane.m128_f32[2] * vTargetPos.z - vPlane.m128_f32[3]) / vPlane.m128_f32[1];
}

void CVIBuffer_Terrain_Basic::Culling(CTransform* pTransform)
{
	m_pGameInstance->Transform_ToLocalSpace(pTransform->Get_WorldMatrix());

	D3D11_MAPPED_SUBRESOURCE		SubResource{};

	_uint		iNumIndices = { 0 };

	m_pContext->Map(m_pIB, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource);

	for (size_t i = 0; i < m_iNumVerticesZ - 1; i++)
	{
		for (size_t j = 0; j < m_iNumVerticesX - 1; j++)
		{
			_uint iIndex = j * m_iNumVerticesX + i;

			_uint	iIndices[4] = {
				iIndex + m_iNumVerticesX,
				iIndex + m_iNumVerticesX + 1,
				iIndex + 1,
				iIndex
			};

			_bool		isIn[4] = {
				m_pGameInstance->isIn_Frustum_LocalSpace(XMLoadFloat3(&m_pVerticesPos[iIndices[0]])),
				m_pGameInstance->isIn_Frustum_LocalSpace(XMLoadFloat3(&m_pVerticesPos[iIndices[1]])),
				m_pGameInstance->isIn_Frustum_LocalSpace(XMLoadFloat3(&m_pVerticesPos[iIndices[2]])),
				m_pGameInstance->isIn_Frustum_LocalSpace(XMLoadFloat3(&m_pVerticesPos[iIndices[3]])),
			};

			if (true == isIn[0] &&
				true == isIn[1] &&
				true == isIn[2])
			{
				((_uint*)SubResource.pData)[iNumIndices++] = iIndices[0];
				((_uint*)SubResource.pData)[iNumIndices++] = iIndices[1];
				((_uint*)SubResource.pData)[iNumIndices++] = iIndices[2];
			}

			if (true == isIn[0] &&
				true == isIn[2] &&
				true == isIn[3])
			{
				((_uint*)SubResource.pData)[iNumIndices++] = iIndices[0];
				((_uint*)SubResource.pData)[iNumIndices++] = iIndices[2];
				((_uint*)SubResource.pData)[iNumIndices++] = iIndices[3];
			}
		}
	}

	m_pContext->Unmap(m_pIB, 0);

	m_iNumIndices = iNumIndices;
}

CVIBuffer_Terrain_Basic* CVIBuffer_Terrain_Basic::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _int _iSizeX, const _int _iSizeZ)
{
	CVIBuffer_Terrain_Basic* pInstance = new CVIBuffer_Terrain_Basic(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(_iSizeX, _iSizeZ)))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Terrain_Basic");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_Terrain_Basic::Clone(void* pArg)
{
	CVIBuffer_Terrain_Basic* pInstance = new CVIBuffer_Terrain_Basic(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_Terrain_Basic");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Terrain_Basic::Free()
{
	__super::Free();
}
