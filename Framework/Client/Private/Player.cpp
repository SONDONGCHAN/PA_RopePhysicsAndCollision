#include "stdafx.h"
#include "..\Public\Player.h"

#include "Body_Player.h"
#include "Weapon.h"

CPlayer::CPlayer(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CLandObject(pDevice, pContext)
{
}

CPlayer::CPlayer(const CPlayer & rhs)
	: CLandObject(rhs)
{
}

CComponent * CPlayer::Get_PartObjectComponent(const wstring & strPartObjTag, const wstring & strComTag)
{
	auto	iter = m_PlayerParts.find(strPartObjTag);
	if (iter == m_PlayerParts.end())
		return nullptr;

	return iter->second->Get_Component(strComTag);		
}

HRESULT CPlayer::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayer::Initialize(void * pArg)
{
	LANDOBJ_DESC*	pGameObjectDesc = (LANDOBJ_DESC*)pArg;

	pGameObjectDesc->fSpeedPerSec = 10.f;
	pGameObjectDesc->fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Add_PartObjects()))
		return E_FAIL;

	return S_OK;
}

void CPlayer::Priority_Tick(_float fTimeDelta)
{
	for (auto& Pair : m_PlayerParts)
		(Pair.second)->Priority_Tick(fTimeDelta);
}

void CPlayer::Tick(_float fTimeDelta)
{
	if (GetKeyState(VK_LEFT) & 0x8000)
	{
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * -1.f);
	}

	if (GetKeyState(VK_RIGHT) & 0x8000)
	{
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * 1.f);
	}

	if (GetKeyState(VK_DOWN) & 0x8000)
	{
		m_pTransformCom->Go_Backward(fTimeDelta);
	}

	if (GetKeyState(VK_UP) & 0x8000)
	{
		m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigationCom);		
		m_iState |= STATE_RUN;
	}
	else
	{
		m_iState = 0x00000000;
		m_iState |= STATE_IDLE;
	}

	for (auto& Pair : m_PlayerParts)
		(Pair.second)->Tick(fTimeDelta);

	SetUp_OnTerrain(m_pTransformCom);

	m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());
}

void CPlayer::Late_Tick(_float fTimeDelta)
{
	for (auto& Pair : m_PlayerParts)
		(Pair.second)->Late_Tick(fTimeDelta);


#ifdef _DEBUG

	m_pGameInstance->Add_DebugComponent(m_pNavigationCom);
	m_pGameInstance->Add_DebugComponent(m_pColliderCom);

#endif

}

HRESULT CPlayer::Render()
{



	return S_OK;
}

HRESULT CPlayer::Add_Components()
{
	/* Com_Navigation */
	CNavigation::NAVI_DESC		NaviDesc{};

	NaviDesc.iStartCellIndex = 0;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"),
		TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &NaviDesc)))
		return E_FAIL;

	/* Com_Collider */
	CBounding_AABB::AABB_DESC		BoundingDesc{};

	BoundingDesc.vExtents = _float3(0.3f, 0.7f, 0.3f);
	BoundingDesc.vCenter = _float3(0.f, BoundingDesc.vExtents.y, 0.f);
	

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &BoundingDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer::Add_PartObjects()
{
	CBody_Player::BODY_PLAYER_DESC BodyPlayerDesc{};

	BodyPlayerDesc.pParentTransform = m_pTransformCom;
	BodyPlayerDesc.pPlayerState = &m_iState;

	CPartObject*		pBody_Player = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Body_Player"), &BodyPlayerDesc));
	if (nullptr == pBody_Player)
		return E_FAIL;
	m_PlayerParts.emplace(TEXT("Part_Body"), pBody_Player);

	CModel*				pBodyModel = dynamic_cast<CModel*>(pBody_Player->Get_Component(TEXT("Com_Model")));
	if (nullptr == pBodyModel)
		return E_FAIL;

	CWeapon::WEAPON_DESC			WeaponObjDesc{};
	WeaponObjDesc.pParentTransform = m_pTransformCom;
	WeaponObjDesc.pSocketMatrix = pBodyModel->Get_CombinedBoneMatrixPtr("SWORD");

	CPartObject*		pWeapon = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Weapon"), &WeaponObjDesc));
	if (nullptr == pWeapon)
		return E_FAIL;
	m_PlayerParts.emplace(TEXT("Part_Weapon"), pWeapon);

	return S_OK;
}

CPlayer * CPlayer::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPlayer*		pInstance = new CPlayer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPlayer");
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CPlayer::Clone(void * pArg)
{
	CPlayer*		pInstance = new CPlayer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPlayer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer::Free()
{
	__super::Free();

	for (auto& Pair : m_PlayerParts)
		Safe_Release(Pair.second);
	m_PlayerParts.clear();

	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pColliderCom);
}
