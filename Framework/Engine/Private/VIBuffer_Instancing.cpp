#include "..\Public\VIBuffer_Instancing.h"

CVIBuffer_Instancing::CVIBuffer_Instancing(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CVIBuffer_Instancing::CVIBuffer_Instancing(const CVIBuffer_Instancing & rhs)
	: CVIBuffer(rhs)
	, m_pVBInstance(rhs.m_pVBInstance)
	, m_iInstanceStride(rhs.m_iInstanceStride)
	, m_iNumInstance(rhs.m_iNumInstance)
	, m_iIndexCountPerInstance(rhs.m_iIndexCountPerInstance)
	, m_RandomNumber(rhs.m_RandomNumber)
	, m_pSpeed(rhs.m_pSpeed)
{
	Safe_AddRef(m_pVBInstance);
}

HRESULT CVIBuffer_Instancing::Initialize_Prototype()
{
	m_RandomNumber = mt19937_64(m_RandomDevice());

	return S_OK;
}

HRESULT CVIBuffer_Instancing::Initialize(void * pArg)
{
	m_InstanceData = *(INSTANCE_DESC*)pArg;

	m_pSpeed = new _float[m_iNumInstance];
	m_pLifeTime = new _float[m_iNumInstance];

	uniform_real_distribution<float>	SpeedRange(m_InstanceData.vSpeed.x, m_InstanceData.vSpeed.y);
	uniform_real_distribution<float>	TimeRange(m_InstanceData.vLifeTime.x, m_InstanceData.vLifeTime.y);

	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		m_pSpeed[i] = SpeedRange(m_RandomNumber);
		m_pLifeTime[i] = TimeRange(m_RandomNumber);
	}

	return S_OK;
}

HRESULT CVIBuffer_Instancing::Render()
{
	m_pContext->DrawIndexedInstanced(m_iIndexCountPerInstance, m_iNumInstance, 0, 0, 0);

	return S_OK;
}

HRESULT CVIBuffer_Instancing::Bind_Buffers()
{
	ID3D11Buffer*		pVertexBuffers[] = {
		m_pVB,
		m_pVBInstance
	};

	_uint				iVertexStrides[] = {
		m_iVertexStride,
		m_iInstanceStride
	};

	_uint				iOffsets[] = {
		0,
		0
	};

	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iVertexStrides, iOffsets);
	m_pContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);
	m_pContext->IASetPrimitiveTopology(m_ePrimitiveTopology);

	return S_OK;
}

void CVIBuffer_Instancing::Tick_Drop(_float fTimeDelta)
{
	m_fTimeAcc += fTimeDelta;

	if (m_InstanceData.fDuration <= m_fTimeAcc)
		m_isFinished = true;

	D3D11_MAPPED_SUBRESOURCE			SubResource{};

	uniform_real_distribution<float>	TimeRange(m_InstanceData.vLifeTime.x, m_InstanceData.vLifeTime.y);
	uniform_real_distribution<float>	WidthRange(m_InstanceData.vRange.x * -0.5f, m_InstanceData.vRange.x * 0.5f);
	uniform_real_distribution<float>	HeightRange(m_InstanceData.vRange.y * -0.5f, m_InstanceData.vRange.y * 0.5f);
	uniform_real_distribution<float>	DepthRange(m_InstanceData.vRange.z * -0.5f, m_InstanceData.vRange.z * 0.5f);

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		((VTXINSTANCE*)SubResource.pData)[i].vTranslation.y -= m_pSpeed[i] * fTimeDelta;

		m_pLifeTime[i] -= fTimeDelta;
		if (0.0f >= m_pLifeTime[i])
		{
			if (true == m_InstanceData.isLoop)
			{
				m_pLifeTime[i] = TimeRange(m_RandomNumber);
				((VTXINSTANCE*)SubResource.pData)[i].vTranslation = _float4(
					m_InstanceData.vCenter.x + WidthRange(m_RandomNumber),
					m_InstanceData.vCenter.y + HeightRange(m_RandomNumber),
					m_InstanceData.vCenter.z + DepthRange(m_RandomNumber),
					1.f);
			}			
			else
			{
				((VTXINSTANCE*)SubResource.pData)[i].vColor.w = 0.f;
			}
		}
	}

	m_pContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_Instancing::Tick_Spread(_float fTimeDelta)
{
	m_fTimeAcc += fTimeDelta;

	if (m_InstanceData.fDuration <= m_fTimeAcc)
		m_isFinished = true;

	D3D11_MAPPED_SUBRESOURCE			SubResource{};

	uniform_real_distribution<float>	TimeRange(m_InstanceData.vLifeTime.x, m_InstanceData.vLifeTime.y);
	uniform_real_distribution<float>	WidthRange(m_InstanceData.vRange.x * -0.5f, m_InstanceData.vRange.x * 0.5f);
	uniform_real_distribution<float>	HeightRange(m_InstanceData.vRange.y * -0.5f, m_InstanceData.vRange.y * 0.5f);
	uniform_real_distribution<float>	DepthRange(m_InstanceData.vRange.z * -0.5f, m_InstanceData.vRange.z * 0.5f);

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		VTXINSTANCE*	pVertices = ((VTXINSTANCE*)SubResource.pData);

		_vector		vDir = XMLoadFloat4(&pVertices[i].vTranslation) - XMLoadFloat3(&m_InstanceData.vPivot);
		vDir = XMVectorSetW(vDir, 0.f);

		XMStoreFloat4(&pVertices[i].vTranslation, XMLoadFloat4(&pVertices[i].vTranslation) + XMVector3Normalize(vDir) * m_pSpeed[i] * fTimeDelta);

		m_pLifeTime[i] -= fTimeDelta;
		if (0.0f >= m_pLifeTime[i])
		{
			if (true == m_InstanceData.isLoop)
			{
				m_pLifeTime[i] = TimeRange(m_RandomNumber);
				((VTXINSTANCE*)SubResource.pData)[i].vTranslation = _float4(
					m_InstanceData.vCenter.x + WidthRange(m_RandomNumber),
					m_InstanceData.vCenter.y + HeightRange(m_RandomNumber),
					m_InstanceData.vCenter.z + DepthRange(m_RandomNumber),
					1.f);
			}
			else
			{
				((VTXINSTANCE*)SubResource.pData)[i].vColor.w = 0.f;
			}
		}
	}

	m_pContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_Instancing::Free()
{
	__super::Free();

	
	Safe_Delete_Array(m_pLifeTime);
	Safe_Delete_Array(m_pSpeed);

	Safe_Release(m_pVBInstance);
}
