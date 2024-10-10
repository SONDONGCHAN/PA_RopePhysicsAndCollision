#include "..\Public\Cell.h"
#include "VIBuffer_Cell.h"

CCell::CCell(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CCell::Initialize(const _float3 * pPoints, _uint iCellIndex)
{
	for (size_t i = 0; i < POINT_END; i++)
		m_vPoints[i] = pPoints[i];	

	m_iIndex = iCellIndex;

	XMStoreFloat3(&m_vNormals[LINE_AB], XMLoadFloat3(&m_vPoints[POINT_B]) - XMLoadFloat3(&m_vPoints[POINT_A]));
	XMStoreFloat3(&m_vNormals[LINE_AB], XMVector3Normalize(XMLoadFloat3(&_float3(m_vNormals[LINE_AB].z * -1.f, 0.f, m_vNormals[LINE_AB].x))));

	XMStoreFloat3(&m_vNormals[LINE_BC], XMLoadFloat3(&m_vPoints[POINT_C]) - XMLoadFloat3(&m_vPoints[POINT_B]));
	XMStoreFloat3(&m_vNormals[LINE_BC], XMVector3Normalize(XMLoadFloat3(&_float3(m_vNormals[LINE_BC].z * -1.f, 0.f, m_vNormals[LINE_BC].x))));

	XMStoreFloat3(&m_vNormals[LINE_CA], XMLoadFloat3(&m_vPoints[POINT_A]) - XMLoadFloat3(&m_vPoints[POINT_C]));
	XMStoreFloat3(&m_vNormals[LINE_CA], XMVector3Normalize(XMLoadFloat3(&_float3(m_vNormals[LINE_CA].z * -1.f, 0.f, m_vNormals[LINE_CA].x))));

	

#ifdef _DEBUG
	m_pVIBuffer = CVIBuffer_Cell::Create(m_pDevice, m_pContext, m_vPoints);
	if (nullptr == m_pVIBuffer)
		return E_FAIL;
#endif

	return S_OK;
}

_bool CCell::Compare_Points(const _float3 & vSourPoint, const _float3 & vDestPoint)
{
	if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_A]), XMLoadFloat3(&vSourPoint)))
	{
		if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_B]), XMLoadFloat3(&vDestPoint)))
			return true;		
		if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_C]), XMLoadFloat3(&vDestPoint)))
			return true;
	}
	if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_B]), XMLoadFloat3(&vSourPoint)))
	{
		if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_C]), XMLoadFloat3(&vDestPoint)))
			return true;
		if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_A]), XMLoadFloat3(&vDestPoint)))
			return true;
	}
	if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_C]), XMLoadFloat3(&vSourPoint)))
	{
		if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_A]), XMLoadFloat3(&vDestPoint)))
			return true;
		if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_B]), XMLoadFloat3(&vDestPoint)))
			return true;
	}

	return false;
}

_bool CCell::isIn(_fvector vLocalPos, _int* pNeighborIndex)
{
	for (size_t i = 0; i < LINE_END; i++)
	{
		_vector	vDir = XMVector3Normalize(vLocalPos - XMLoadFloat3(&m_vPoints[i]));

		if (0 < XMVectorGetX(XMVector3Dot(XMLoadFloat3(&m_vNormals[i]), vDir)))
		{
			*pNeighborIndex = m_iNeighborIndices[i];
			return false;
		}
	}	

	return true;
}

#ifdef _DEBUG

HRESULT CCell::Render()
{


	m_pVIBuffer->Bind_Buffers();
	m_pVIBuffer->Render();

	return S_OK;
}

#endif

CCell * CCell::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _float3 * pPoints, _uint iCellIndex)
{
	CCell*		pInstance = new CCell(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pPoints, iCellIndex)))
	{
		MSG_BOX("Failed to Created : CCell");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CCell::Free()
{
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);

#ifdef _DEBUG
	Safe_Release(m_pVIBuffer);
#endif
}
