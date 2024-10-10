#include "stdafx.h"
#include "..\Public\PartObject.h"

CPartObject::CPartObject(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CPartObject::CPartObject(const CPartObject & rhs)
	: CGameObject(rhs)
{
}

HRESULT CPartObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPartObject::Initialize(void * pArg)
{
	PARTOBJ_DESC*		pPartObjDesc = (PARTOBJ_DESC*)pArg;

	m_pParentTransform = pPartObjDesc->pParentTransform;
	Safe_AddRef(m_pParentTransform);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CPartObject::Priority_Tick(_float fTimeDelta)
{
}

void CPartObject::Tick(_float fTimeDelta)
{
}

void CPartObject::Late_Tick(_float fTimeDelta)
{
}

HRESULT CPartObject::Render()
{
	return S_OK;
}

void CPartObject::Free()
{
	__super::Free();

	Safe_Release(m_pParentTransform);
}
