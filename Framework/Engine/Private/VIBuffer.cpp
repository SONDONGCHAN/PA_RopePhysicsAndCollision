#include "..\Public\VIBuffer.h"

CVIBuffer::CVIBuffer(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)
{
}

CVIBuffer::CVIBuffer(const CVIBuffer & rhs)
	: CComponent(rhs)
	, m_pVB(rhs.m_pVB)
	, m_pIB(rhs.m_pIB)
	, m_iVertexStride(rhs.m_iVertexStride)
	, m_iNumVertices(rhs.m_iNumVertices)
	, m_iIndexStride(rhs.m_iIndexStride)
	, m_iNumIndices(rhs.m_iNumIndices)
	, m_iNumVertexBuffers(rhs.m_iNumVertexBuffers)
	, m_eIndexFormat(rhs.m_eIndexFormat)
	, m_ePrimitiveTopology(rhs.m_ePrimitiveTopology)
	, m_pVerticesPos(rhs.m_pVerticesPos)
{
	Safe_AddRef(m_pVB);
	Safe_AddRef(m_pIB);
}

HRESULT CVIBuffer::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CVIBuffer::Initialize(void * pArg)
{
	return S_OK;
}

HRESULT CVIBuffer::Render()
{	
	m_pContext->DrawIndexed(m_iNumIndices, 0, 0);

	return S_OK;
}

HRESULT CVIBuffer::Bind_Buffers()
{
	ID3D11Buffer*		pVertexBuffers[] = {
		m_pVB,				
	};

	_uint				iVertexStrides[] = {
		m_iVertexStride,	
		
	};

	_uint				iOffsets[] = {
		0,
	};

	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iVertexStrides, iOffsets);
	m_pContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);
	m_pContext->IASetPrimitiveTopology(m_ePrimitiveTopology);

	return S_OK;
}

HRESULT CVIBuffer::Create_Buffer(_Out_ ID3D11Buffer** ppBuffer)
{
	return m_pDevice->CreateBuffer(&m_BufferDesc, &m_InitialData, ppBuffer);	
}


void CVIBuffer::Free()
{
	__super::Free();

	if(false == m_isCloned)
		Safe_Delete_Array(m_pVerticesPos);

	Safe_Release(m_pIB);
	Safe_Release(m_pVB);
}
