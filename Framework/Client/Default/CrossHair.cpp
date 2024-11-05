#include "stdafx.h"
#include "CrossHair.h"
#include "GameInstance.h"

CCrossHair::CCrossHair(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CCrossHair::CCrossHair(const CCrossHair& rhs)
	: CGameObject(rhs)

{
}

HRESULT CCrossHair::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCrossHair::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(&pArg)))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	m_fSizeX = g_iWinSizeX;
	m_fSizeY = g_iWinSizeY;
	m_fX = g_iWinSizeX * 0.5f;
	m_fY = g_iWinSizeY * 0.5f;

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.f, 1.f));

	m_pTransformCom->Set_Scaling(1.f, 1.f, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f, 1.f));

	m_vColor		= { 1.f, 0.f, 0.f, 1.f };
	m_fThickness	= 4.f;
	m_fSize			= 20.f;

	return S_OK;
}

void CCrossHair::Priority_Tick(_float fTimeDelta)
{
}

void CCrossHair::Tick(_float fTimeDelta)
{

}

void CCrossHair::Late_Tick(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this)))
		return;
}

HRESULT CCrossHair::Render()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_pTransformCom->Get_WorldMatrix4x4())))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof(_float4));
	m_pShaderCom->Bind_RawValue("g_fThickness", &m_fThickness, sizeof(_float));
	m_pShaderCom->Bind_RawValue("g_fSize", &m_fSize, sizeof(_float));

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCrossHair::Add_Component()
{
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxPoint_CrossHair"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Point"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

CCrossHair* CCrossHair::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCrossHair* pInstance = new CCrossHair(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCrossHair");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCrossHair::Clone(void* pArg)
{
	CCrossHair* pInstance = new CCrossHair(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCrossHair");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCrossHair::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
