#include "..\Public\VIBuffer_Skeletal.h"

CVIBuffer_Skeletal::CVIBuffer_Skeletal(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _int _BoneCount)
	: CVIBuffer(pDevice, pContext), m_BoneCount(_BoneCount)
{
}

CVIBuffer_Skeletal::CVIBuffer_Skeletal(const CVIBuffer_Skeletal& rhs)
	: CVIBuffer(rhs), m_BoneCount(rhs.m_BoneCount)
{
}

HRESULT CVIBuffer_Skeletal::Initialize_Prototype()
{
    if (FAILED(Initialize_LineBuffer()))
        return E_FAIL;

    return S_OK;
}

HRESULT CVIBuffer_Skeletal::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CVIBuffer_Skeletal::Initialize_LineBuffer()
{
    BoneIndexVertex* pVertices = new BoneIndexVertex[(m_BoneCount - 1) * 2];

    for (_int i = 0; i < m_BoneCount - 1; ++i) {
        // 첫 번째 정점 (본 i의 위치)
        pVertices[i * 2] = { XMFLOAT3(0.0f, 0.0f, 0.0f), i };

        // 두 번째 정점 (본 i+1의 위치)
        pVertices[i * 2 + 1] = { XMFLOAT3(0.0f, 0.0f, 0.0f), i + 1 };
    }

    D3D11_BUFFER_DESC bufferDesc = {};
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.ByteWidth = sizeof(BoneIndexVertex) * ((m_BoneCount - 1) * 2);
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = pVertices;

    HRESULT hr = m_pDevice->CreateBuffer(&bufferDesc, &initData, &m_pVB);
    Safe_Delete_Array(pVertices);

    m_iVertexStride = sizeof(BoneIndexVertex);

    return hr;
}

HRESULT CVIBuffer_Skeletal::Render()
{
    UINT stride = sizeof(BoneIndexVertex);
    UINT offset = 0;
    
    m_pContext->IASetVertexBuffers(0, 1, &m_pVB, &stride, &offset);
    //m_pContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);
    m_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
    
    // Draw lines connecting bones
      
    m_pContext->Draw((m_BoneCount - 1) * 2, 0);
    
    return S_OK;
}

CVIBuffer_Skeletal* CVIBuffer_Skeletal::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _int _BoneCount)
{
    CVIBuffer_Skeletal* pInstance = new CVIBuffer_Skeletal(pDevice, pContext, _BoneCount);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CVIBuffer_Skeletal");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CComponent* CVIBuffer_Skeletal::Clone(void* pArg)
{
    CVIBuffer_Skeletal* pInstance = new CVIBuffer_Skeletal(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CVIBuffer_Skeletal");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CVIBuffer_Skeletal::Free()
{
    __super::Free();
}
