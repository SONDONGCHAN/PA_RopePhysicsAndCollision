#include "stdafx.h"
#include "..\Public\Player.h"

#include "Body_Player.h"
#include "Weapon.h"
#include "Collider.h"
#include "Projectile_Rope.h"
#include "Mass.h"


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

	if (FAILED(Add_Simulation()))
		return E_FAIL;

	if (FAILED(Add_Projectile()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _vector{ 250.f, 0.f, 250.f});

	return S_OK;
}

void CPlayer::Priority_Tick(_float fTimeDelta)
{
	for (auto& Pair : m_PlayerParts)
		(Pair.second)->Priority_Tick(fTimeDelta);
}

void CPlayer::Tick(_float fTimeDelta)
{
	KeyInput(fTimeDelta);

	Handle_Jump(fTimeDelta);

	for (auto& Pair : m_PlayerParts)
		(Pair.second)->Tick(fTimeDelta);

	Root_Transform();
	m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());

	m_pRopeSimulation->Operate(fTimeDelta);
}

void CPlayer::Late_Tick(_float fTimeDelta)
{
#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponent(m_pNavigationCom);
	m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif

	for (auto& Pair : m_PlayerParts)
		(Pair.second)->Late_Tick(fTimeDelta);

	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this)))
		return;
}

HRESULT CPlayer::Render()
{
	if (KEYINPUT(DIK_LSHIFT))
	{
		if (m_eJumpState == JumpState::SWINGING)
			m_pGameInstance->Render_Font(TEXT("Font_Default"), TEXT("Accelerating"), _float2(g_iWinSizeX * 0.8f, g_iWinSizeY * 0.5f), XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f, _float2(0.f, 0.f), 1.f);
	}
	m_pRopeSimulation->Render();
	return S_OK;
}

void CPlayer::Start_Soft_Simulating(_vector _vDir, _vector _vPos, _float _fM, _float _fLastM)
{
	if (nullptr == m_pRopeSimulation)
		return;

	m_pRopeSimulation->Start_Soft_Simulating(_vDir, _vPos, _fM, _fLastM);
}

void CPlayer::Start_Stiff_Simulating(_vector _vDir, _vector _vPos, _float _fM, _float _fLastM)
{
	if (nullptr == m_pRopeSimulation)
		return;

	m_pRopeSimulation->Start_Stiff_Simulating(_vDir, _vPos, _fM, _fLastM, m_vVelocity);
	Start_Swing();

}

void CPlayer::End_Simulating()
{
	m_pRopeSimulation->End_Simulating();
}

void CPlayer::KeyInput(_float fTimeDelta)
{
	// 점프
	if (KEYDOWN(DIK_SPACE))
		Start_Jump();

	if (KEYDOWN(DIK_Q))
	{
		_vector vVel = m_pRopeSimulation->Get_FinalMass()->Get_Vel();
		m_vVelocity = vVel * 1.3f;
		m_pRopeSimulation->Switch_Soft_Simulating(vVel);

		if (m_eJumpState == JumpState::SWINGING)
			m_eJumpState = JumpState::FALLING;
	}

	// 로프	
	if (m_pGameInstance->MouseDown(DIM_LB))
	{	
		m_pProjectile_Rope->Enable_Projectile(
			m_pTransformCom->Get_State(CTransform::STATE_POSITION),
			XMLoadFloat4(&CGameInstance::GetInstance()->Get_CamLook()));

		//디버깅
		//m_pProjectile_Rope->Enable_Projectile(
		//	_vector{6.f, 1.f, 2.f},
		//	XMVector3Normalize(_vector{0.f, 1.f, 1.f}));
	}
	if (KEYDOWN(DIK_LSHIFT))
	{
		if (m_eJumpState == JumpState::SWINGING)
			m_pRopeSimulation->Set_Accelerating(true, 50.f);
	}
	else if (KEYUP(DIK_LSHIFT))
	{
		m_pRopeSimulation->Set_Accelerating(false);
	}

	// 상태
	Update_State(fTimeDelta);
}

void CPlayer::Root_Transform()
{
	CBody_Player* pBody = dynamic_cast<CBody_Player*>(m_PlayerParts[TEXT("Part_Body")]);
	_float3 vRootTransform = pBody->Get_RootTranform();

	m_pTransformCom->Move(XMLoadFloat3(&vRootTransform), m_pNavigationCom);
}

void CPlayer::Change_State(PlayerState eNewState)
{
	if (m_eCurrentState == eNewState) return;

	m_eNextState = eNewState;  // 다음 상태 설정
	m_fStateTimer = 0.0f;      // 타이머 초기화
}

void CPlayer::Update_State(_float fTimeDelta)
{	
	m_fStateTimer += fTimeDelta;

	switch (m_eCurrentState) 
	{
	case PlayerState::STATE_IDLE:
		Handle_IdleState(fTimeDelta);
		break;

	case PlayerState::STATE_MOVE:
		Handle_MoveState(fTimeDelta);
		break;

	case PlayerState::STATE_ATTACK:
		Handle_AttackState(fTimeDelta);
		break;

	}

	// 상태 전환이 발생했을 경우, 다음 상태로 변경
	if (m_eCurrentState != m_eNextState)
	{
		m_eCurrentState = m_eNextState;
	}	
}

void CPlayer::Change_Anim(PlayerAnim eNewAnim)
{
	if (m_iAnimation == eNewAnim)
		return;

	m_iAnimation = eNewAnim;
}

void CPlayer::Handle_IdleState(_float fTimeDelta)
{
	Change_Anim(IDLE);

	if (m_eJumpState == JumpState::SWINGING)
		return;

	if (KEYINPUT(DIK_W) || KEYINPUT(DIK_A) || KEYINPUT(DIK_S) || KEYINPUT(DIK_D))
	{	
		Change_State(PlayerState::STATE_MOVE);
	}
}

void CPlayer::Handle_MoveState(_float fTimeDelta)
{
	if (m_eJumpState == JumpState::SWINGING)
		return;

	if (KEYPRESSING(DIK_W) || KEYPRESSING(DIK_A) || KEYPRESSING(DIK_S) || KEYPRESSING(DIK_D))
	{
		Change_Anim(RUN_CYCLE);

		_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_POSITION) - XMLoadFloat4(&CGameInstance::GetInstance()->Get_CamPosition());
		vLook = XMVector4Normalize(XMVectorSetY(vLook, 0.f));
		_vector vRight = XMVector4Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook));
		_vector vMyLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
		_vector vTargetDir;

		if (KEYPRESSING(DIK_W))
		{
			if (KEYPRESSING(DIK_A))
				Set_Dir_From_Cam(fTimeDelta, Direction::DIR_FLEFT);
			else if (KEYPRESSING(DIK_D))
				Set_Dir_From_Cam(fTimeDelta, Direction::DIR_FRIGHT);
			else
				Set_Dir_From_Cam(fTimeDelta, Direction::DIR_FRONT);
		}
		else if (KEYPRESSING(DIK_S))
		{
			if (KEYPRESSING(DIK_A))
				Set_Dir_From_Cam(fTimeDelta, Direction::DIR_BLEFT);
			else if (KEYPRESSING(DIK_D))
				Set_Dir_From_Cam(fTimeDelta, Direction::DIR_BRIGHT);
			else
				Set_Dir_From_Cam(fTimeDelta, Direction::DIR_BACK);
		}
		else if (KEYPRESSING(DIK_A))
		{
			Set_Dir_From_Cam(fTimeDelta, Direction::DIR_LEFT);
		}
		else if (KEYPRESSING(DIK_D))
		{
			Set_Dir_From_Cam(fTimeDelta, Direction::DIR_RIGHT);
		}
	}
	else
	{ 
		Change_State(PlayerState::STATE_IDLE);
	}
}

void CPlayer::Handle_AttackState(_float fTimeDelta)
{

}

void CPlayer::Handle_Jump(_float fTimeDelta)
{
	if (m_eJumpState == JumpState::JUMPING || m_eJumpState == JumpState::FALLING)
	{
		_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

		m_vVelocity = XMVectorSetY(m_vVelocity, XMVectorGetY(m_vVelocity) + (m_fGravity * fTimeDelta));
		_vector vDragForce = m_fDragCoefficient * m_vVelocity * fTimeDelta;
		m_vVelocity -= vDragForce;

		if (XMVectorGetY(m_vVelocity) < m_fTerminalVelocity)
		{
			XMVectorSetY(m_vVelocity, m_fTerminalVelocity);
		}

		vPos = XMVectorAdd(vPos, m_vVelocity * fTimeDelta);

		_float fTerrainHeight = Get_Height(m_pTransformCom);

		if (vPos.m128_f32[1] <= fTerrainHeight)
		{
			vPos = XMVectorSetY(vPos, fTerrainHeight);
			m_vVelocity = XMVectorSet(0.f, 0.f, 0.f, 0.f);
			m_eJumpState = JumpState::ONGROUND;
		}
		
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
	}
	else if(m_eJumpState == JumpState::ONGROUND)
	{
		SetUp_OnTerrain(m_pTransformCom);
	}
	else if (m_eJumpState == JumpState::SWINGING)
	{
		Handle_Swing(fTimeDelta);
	}
}

void CPlayer::Handle_Swing(_float fTimeDelta)
{
	_vector vPos = m_pRopeSimulation->Get_FinalMass()->Get_Pos();
	
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos + _vector{ 0.f, -1.f, 0.f });
}

void CPlayer::Set_Dir_From_Cam(_float fTimeDelta, Direction _DIRType)
{
	_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_POSITION) - XMLoadFloat4(&CGameInstance::GetInstance()->Get_CamPosition());
	vLook = XMVector4Normalize(XMVectorSetY(vLook, 0.f));
	_vector vRight = XMVector4Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook));
	_vector vTargetDir = {1.f, 1.f, 1.f, 0.f};

	switch (_DIRType)
	{
	case Direction::DIR_FRONT:
		vTargetDir = vLook;
		break;

	case Direction::DIR_BACK:
		vTargetDir = -vLook;
		break;

	case Direction::DIR_LEFT:
		vTargetDir = -vRight;
		break;

	case Direction::DIR_RIGHT:
		vTargetDir = vRight;
		break;

	case Direction::DIR_FLEFT:
		vTargetDir = vLook - vRight;
		break;

	case Direction::DIR_FRIGHT:
		vTargetDir = vLook + vRight;
		break;

	case Direction::DIR_BLEFT:
		vTargetDir = (-vLook) - vRight;
		break;

	case Direction::DIR_BRIGHT:
		vTargetDir = (- vLook) + vRight;
		break;
	}

	vTargetDir = XMVector4Normalize(vTargetDir);

	_float fLerpSpeed = 10.0f * fTimeDelta;

	XMMATRIX myWorld = m_pTransformCom->Get_WorldMatrix();
	XMVECTOR qCurrent = XMQuaternionRotationMatrix(myWorld);
	XMVECTOR qTarget = XMQuaternionInverse(XMQuaternionRotationMatrix(XMMatrixLookToLH(
		m_pTransformCom->Get_State(CTransform::STATE_POSITION), vTargetDir, XMVectorSet(0.f, 1.f, 0.f, 0.f))));

	XMVECTOR qNewRotation = XMQuaternionSlerp(qCurrent, qTarget, fLerpSpeed);

	XMVECTOR vScale, vTranslation, vRotation;
	XMMatrixDecompose(&vScale, &vRotation, &vTranslation, myWorld);

	XMMATRIX mNewWorld = XMMatrixScalingFromVector(vScale) *
						 XMMatrixRotationQuaternion(qNewRotation) *
						 XMMatrixTranslationFromVector(vTranslation);

	m_pTransformCom->Set_WorldMatrix(mNewWorld);
}

void CPlayer::Start_Jump()
{
	if (m_eJumpState == JumpState::ONGROUND) 
	{
		m_vVelocity = XMVectorSetY(m_vVelocity, m_fJumpforce); // 초기 점프 속도 설정
		m_eJumpState = JumpState::JUMPING;
	}
}

void CPlayer::Start_Swing()
{
	if (m_eJumpState != JumpState::SWINGING)
	{
		m_eJumpState	= JumpState::SWINGING;
		m_eCurrentState = PlayerState::STATE_IDLE;
		m_eNextState	= PlayerState::STATE_IDLE;

		m_vVelocity = XMVectorSet(0.f, 0.f, 0.f, 0.f);
	}
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

	CCollider::ColliderInitData	ColliderDesc {};
	 
	CGameObject::ColData ColData{};
	ColData.pGameObject = this;
	ColData.eMyColType = COL_PLAYER;
	ColData.iTargetColType = COL_STATIC_OBJECT;
	ColData.isDead = false;

	
	CCollider::SPHERE_DESC	BoundingDesc{};
	BoundingDesc.fRadius = 0.5f;
	BoundingDesc.vCenter = _float3(0.f, BoundingDesc.fRadius, 0.f);

	ColliderDesc.ColData = ColData;
	ColliderDesc.SphereDesc = BoundingDesc;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;

	CGameInstance::GetInstance()->Add_Collider(m_pColliderCom);
	
	return S_OK;
}

HRESULT CPlayer::Add_PartObjects()
{
	CBody_Player::BODY_PLAYER_DESC BodyPlayerDesc{};

	BodyPlayerDesc.pParentTransform = m_pTransformCom;
	BodyPlayerDesc.pPlayerAnimation = &m_iAnimation;

	CPartObject*		pBody_Player = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Body_Player"), &BodyPlayerDesc));
	if (nullptr == pBody_Player)
		return E_FAIL;
	m_PlayerParts.emplace(TEXT("Part_Body"), pBody_Player);

	//CModel*				pBodyModel = dynamic_cast<CModel*>(pBody_Player->Get_Component(TEXT("Com_Model")));
	//if (nullptr == pBodyModel)
	//	return E_FAIL;

	//CWeapon::WEAPON_DESC			WeaponObjDesc{};
	//WeaponObjDesc.pParentTransform = m_pTransformCom;
	//WeaponObjDesc.pSocketMatrix = pBodyModel->Get_CombinedBoneMatrixPtr("SWORD");

	//CPartObject*		pWeapon = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Weapon"), &WeaponObjDesc));
	//if (nullptr == pWeapon)
	//	return E_FAIL;
	//m_PlayerParts.emplace(TEXT("Part_Weapon"), pWeapon);

	return S_OK;
}

HRESULT CPlayer::Add_Simulation()
{
	m_pRopeSimulation = new CRope_Simulation
	(	10000.f,
		1.f,
		_vector{0.f, -9.81f, 0},
		0.02f,
		100.f,
		0.2f,
		2.f,
		-50.f);
	
	if (nullptr == m_pRopeSimulation)
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer::Add_Projectile()
{
	CProjectile::PROJECTILE_DESC Projectile_Desc{};
	Projectile_Desc.pOwnerObject = this;
 
	m_pProjectile_Rope = dynamic_cast<CProjectile_Rope*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Projectile_Rope"), &Projectile_Desc));
	Safe_AddRef(m_pProjectile_Rope);

	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Projectile"), m_pProjectile_Rope)))
		return E_FAIL;

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

	Safe_Release(m_pRopeSimulation);
	Safe_Release(m_pProjectile_Rope);
}
