#include "stdafx.h"
#include "Projectile.h"

CProjectile::CProjectile(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CProjectile::CProjectile(const CProjectile& rhs)
	: CGameObject(rhs)
{
}

HRESULT CProjectile::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CProjectile::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CProjectile::Priority_Tick(_float fTimeDelta)
{
}

void CProjectile::Tick(_float fTimeDelta)
{
}

void CProjectile::Late_Tick(_float fTimeDelta)
{
}

HRESULT CProjectile::Render()
{
	return S_OK;
}


void CProjectile::Free()
{
	__super::Free();
}
