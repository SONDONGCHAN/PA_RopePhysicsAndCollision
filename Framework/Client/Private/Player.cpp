#include "stdafx.h"
#include "..\Public\Player.h"

#include "Body_Player.h"
#include "Weapon.h"
#include "Projectile_Rope.h"
#include "Simulation_Pool.h"
#include "Simulation.h"

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

	pGameObjectDesc->fSpeedPerSec = 3.f;
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

	if(FAILED(Add_CrossHair()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _vector{ 250.f, 0.f, 250.f});
	SetUp_OnTerrain(m_pTransformCom);

	return S_OK;
}

void CPlayer::Priority_Tick(_float fTimeDelta)
{
	for (auto& Pair : m_PlayerParts)
		(Pair.second)->Priority_Tick(fTimeDelta);
}

void CPlayer::Tick(_float fTimeDelta)
{
	Move_Control();
	Root_Transform();

	KeyInput(fTimeDelta);

	/*디버깅*/
	Handle_Jump(fTimeDelta);

	for (auto& Pair : m_PlayerParts)
		(Pair.second)->Tick(fTimeDelta);

	m_pRigidColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());
	m_pTriggerColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());

	m_pSimulationPool->Tick(fTimeDelta);
	//m_pRopeSimulation->Operate(fTimeDelta);
}

void CPlayer::Late_Tick(_float fTimeDelta)
{
#ifdef _DEBUG
	//m_pGameInstance->Add_DebugComponent(m_pNavigationCom);
	m_pGameInstance->Add_DebugComponent(m_pRigidColliderCom);
	m_pGameInstance->Add_DebugComponent(m_pTriggerColliderCom);
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

	m_pSimulationPool->Render();
	
	return S_OK;
}

void CPlayer::Event_CollisionEnter(ColData* _TargetColData, ColData* _MyColData)
{
	if (_TargetColData->eMyColType == COL_STATIC_OBJECT)
	{
		/*디버깅*/
		if (_MyColData->pCol == m_pRigidColliderCom)
		{		
			vector<vector<CBounding_Capsule::ContactPoint>>& vecPoints = dynamic_cast<CBounding_Capsule*>(m_pRigidColliderCom->Get_Bounding())->Get_Points()[0];
			CBounding_Capsule::ContactPoint ContactPoint;
			for (int i = 0; i < vecPoints.size(); ++i)
			{
				if (vecPoints[i][0].isCol)
				{
					ContactPoint = vecPoints[i][0];
					if (ContactPoint.ContactType == CBounding_Capsule::ContactPointType::VertexPlane)
						break;
				}
				if ((vecPoints[i].size() > 1) && (vecPoints[i][1].isCol))
				{
					ContactPoint = vecPoints[i][1];
					if (ContactPoint.ContactType == CBounding_Capsule::ContactPointType::VertexPlane)
						break;

				}			
			}

			_vector vNormal = ContactPoint.ShapeContactNormal;
			_vector vUp = { 0.0f, 1.0f, 0.0f };
			float dot = XMVectorGetX(XMVector3Dot(vNormal, vUp));
			float magnitude = XMVectorGetX(XMVector3Length(vNormal));	// 크기 계산
			float angleRad = acosf(dot / magnitude);					// 라디안 단위 기울기
			float angleDeg = XMConvertToDegrees(angleRad);				// 각도를 도 단위로 변환
			_float	fDepth	= (-ContactPoint.Phi) + (0.001f);			// 충돌 깊이

			if (m_eJumpState == JumpState::SWINGING)
			{
				Escape_Swing(JumpState::FALLING);
			}
			else
			{
				m_vVelocity = XMVectorSet(0.f, 0.f, 0.f, 0.f);
			}

			if (angleDeg <= 60.f)
			{
				XMStoreFloat3(&m_vCurrentNormal, vNormal);
				if (m_pCurrentLandCollider == nullptr)
				{
					m_eJumpState = JumpState::ONOBJECT;
					m_eCurrentState = PlayerState::STATE_IDLE;
					m_eNextState = PlayerState::STATE_IDLE;

					m_pCurrentLandCollider = _TargetColData->pCol;
					m_iCurrentLandIndex = ContactPoint.FaceIndex;
				}
				else
				{
					m_pCurrentLandCollider = _TargetColData->pCol;
					m_iCurrentLandIndex = ContactPoint.FaceIndex;
				}
				
			}
			else if ( 120.f<= angleDeg )
			{
				return;
			}
			else
			{
				if (m_eJumpState == JumpState::ONGROUND || m_eJumpState == JumpState::ONOBJECT)
				{
					
				}
				else
				{
					XMStoreFloat3(&m_vClimingNormal, vNormal);
					if (m_pCurrentClimbCollider == nullptr)
					{
						m_eJumpState = JumpState::CLIMING;
						m_eCurrentState = PlayerState::STATE_CLIMING;
						m_eNextState = PlayerState::STATE_CLIMING;

						m_pCurrentClimbCollider = _TargetColData->pCol;
						m_iCurrentClimbIndex = ContactPoint.FaceIndex;
						m_pTransformCom->Set_Look_ForLandObject(-vNormal);
					}
					else
					{
						m_pCurrentClimbCollider = _TargetColData->pCol;
						m_iCurrentClimbIndex = ContactPoint.FaceIndex;
					}
				}
				
			}
			m_pTransformCom->Move_Dir(vNormal, fDepth);
			m_pRigidColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());
			m_pTriggerColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());

			if (!m_pRigidColliderCom->Intersect(_TargetColData->pCol))
			{
				m_pRigidColliderCom->CollisionExit(_TargetColData->pCol);
			}			
			
		}

		if (_MyColData->pCol == m_pTriggerColliderCom)
		{
			m_CurrentLandIndeces.clear();
			m_CurrentClimbIndeces.clear();
			vector<vector<CBounding_Capsule::ContactPoint>>& vecPoints = dynamic_cast<CBounding_Capsule*>(m_pTriggerColliderCom->Get_Bounding())->Get_Points()[0];
			for (int i = 0; i < vecPoints.size(); ++i)
			{
				if (vecPoints[i][0].isCol)
				{
					CBounding_Capsule::ContactPoint ContactPoint = vecPoints[i][0];
					_vector vNormal = ContactPoint.ShapeContactNormal;
					_vector vUp = { 0.0f, 1.0f, 0.0f };
					float dot = XMVectorGetX(XMVector3Dot(vNormal, vUp));
					float magnitude = XMVectorGetX(XMVector3Length(vNormal));
					float angleRad = acosf(dot / magnitude);
					float angleDeg = XMConvertToDegrees(angleRad);
					_float	fDepth = (-ContactPoint.Phi) + (0.001f);

					if (angleDeg <= 60.f)
					{
						_float3 StoreNormal;
						XMStoreFloat3(&StoreNormal, vNormal);

						//if (m_CurrentLandIndeces.count(ContactPoint.FaceIndex) > 0)
						m_CurrentLandIndeces.emplace(ContactPoint.FaceIndex, StoreNormal);
						    
					}
					else if (120.f <= angleDeg)
					{
						return;
					}
					else
					{
						if (m_eJumpState == JumpState::ONGROUND || m_eJumpState == JumpState::ONOBJECT)
						{

						}
						else
						{
							_float3 StoreNormal;
							XMStoreFloat3(&StoreNormal, vNormal);
							m_CurrentClimbIndeces.emplace(ContactPoint.FaceIndex, StoreNormal);
						}
					}
				}
			}

		}
	}
}

void CPlayer::Event_CollisionStay(ColData* _ColData, ColData* _MyColData)
{
	if (_ColData->eMyColType == COL_STATIC_OBJECT)
	{
		if (_MyColData->pCol == m_pTriggerColliderCom)
		{
			m_CurrentLandIndeces.clear();
			m_CurrentClimbIndeces.clear();
			vector<vector<CBounding_Capsule::ContactPoint>>& vecPoints = dynamic_cast<CBounding_Capsule*>(m_pTriggerColliderCom->Get_Bounding())->Get_Points()[0];
			for (int i = 0; i < vecPoints.size(); ++i)
			{
				if (vecPoints[i][0].isCol)
				{
					CBounding_Capsule::ContactPoint ContactPoint = vecPoints[i][0];
					_vector vNormal = ContactPoint.ShapeContactNormal;
					_vector vUp = { 0.0f, 1.0f, 0.0f };
					float dot = XMVectorGetX(XMVector3Dot(vNormal, vUp));
					float magnitude = XMVectorGetX(XMVector3Length(vNormal));
					float angleRad = acosf(dot / magnitude);
					float angleDeg = XMConvertToDegrees(angleRad);
					_float	fDepth = (-ContactPoint.Phi) + (0.001f);

					if (angleDeg <= 60.f)
					{
						_float3 N = dynamic_cast<CBounding_Triangles*>(_ColData->pCol->Get_Bounding())->Get_Normals()[ContactPoint.FaceIndex];

						//if (m_CurrentLandIndeces.count(ContactPoint.FaceIndex) > 0)
						m_CurrentLandIndeces.emplace(ContactPoint.FaceIndex, N);
						
					}
					else if (120.f <= angleDeg)
					{
						return;
					}
					else
					{
						if (m_eJumpState == JumpState::ONGROUND || m_eJumpState == JumpState::ONOBJECT)
						{

						}
						else
						{
							_float3 N = dynamic_cast<CBounding_Triangles*>(_ColData->pCol->Get_Bounding())->Get_Normals()[ContactPoint.FaceIndex];
							m_CurrentClimbIndeces.emplace(ContactPoint.FaceIndex, N);
						}
					}
				}
			}
					
		}
	}
}

void CPlayer::Event_CollisionExit(ColData* _ColData, ColData* _MyColData)
{
	if (_ColData->eMyColType == COL_STATIC_OBJECT)
	{
		if (_MyColData->pCol == m_pTriggerColliderCom)
		{
			if (_ColData->pCol == m_pCurrentLandCollider)
			{
				//_bool isFind = false;
				//for (auto pair : m_CurrentLandIndeces)
				//{
				//	if (pair.first == m_iCurrentLandIndex)
				//		continue;

				//	m_iCurrentLandIndex = pair.first;
				//	m_vCurrentNormal = pair.second;
				//	isFind = true;
				//	break;
				//}

				//if (!isFind)
				//{
				//	m_eJumpState = JumpState::FALLING;
				//	m_eCurrentState = PlayerState::STATE_FALLING;
				//	m_eNextState = PlayerState::STATE_FALLING;
				//	m_iCurrentLandIndex = -1;
				//}

				m_CurrentLandIndeces.clear();
				m_pCurrentLandCollider = nullptr;
			}
			else if (_ColData->pCol == m_pCurrentClimbCollider)
			{
				//_bool isFind = false;
				//for (auto pair : m_CurrentClimbIndeces)
				//{
				//	if (pair.first == m_iCurrentClimbIndex)
				//		continue;

				//	m_iCurrentClimbIndex = pair.first;
				//	m_vClimingNormal = pair.second;
				//	isFind = true;
				//	break;
				//}

				//if (!isFind)
				//{
				//	m_eJumpState = JumpState::FALLING;
				//	m_eCurrentState = PlayerState::STATE_FALLING;
				//	m_eNextState = PlayerState::STATE_FALLING;
				//	m_iCurrentClimbIndex = -1;
				//}
				m_CurrentClimbIndeces.clear();
				m_pCurrentClimbCollider = nullptr;
			}
		}
	}
}


void CPlayer::Start_Stiff_Simulating(_vector _vDir, _vector _vPos, _float _fM, _float _fLastM)
{
	if (nullptr == m_pSimulationPool)
		return; 

	if (m_eJumpState == JumpState::ONGROUND || m_eJumpState == JumpState::ONOBJECT)
		return;

	CRope_Simulation::Rope_Simulation_Data pData;
	
	pData.vDir		= _vDir;
	pData.vPos		= _vPos;
	pData.fM		= _fM;
	pData.fLastM	= _fLastM;
	pData.vStartVel = m_vVelocity;

	m_pCurrentSimulation = m_pSimulationPool->Awake_Simulation(CSimulation::Simulation_Type::SIMULATION_ROPE, &pData);

	if (nullptr == m_pCurrentSimulation)
		return;

	m_pTransformCom->Set_Look_ForLandObject((-_vDir));
	// 스윙이 시작 될때, 스윙 진행방향으로 캐릭터 Look 변경해주기

	Start_Swing();
}

void CPlayer::End_Simulating()
{
	dynamic_cast<CRope_Simulation*>(m_pCurrentSimulation)->End_Simulating();
}

void CPlayer::KeyInput(_float fTimeDelta)
{
	/*디버깅*/
	//if (KEYPRESSING(DIK_UP))
	//{
	//	m_pTransformCom->Go_Dir(Direction::DIR_UP, fTimeDelta);
	//}
	//if (KEYPRESSING(DIK_DOWN))
	//{
	//	m_pTransformCom->Go_Dir(Direction::DIR_DOWN, fTimeDelta);
	//}
	//if (KEYDOWN(DIK_LCONTROL))
	//{
	//	m_pTransformCom->Set_SpeedPerSec(10.f);
	//}
	//else if (KEYUP(DIK_LCONTROL))
	//{
	//	m_pTransformCom->Set_SpeedPerSec(1.f);
	//}
	// 점프
	if (KEYDOWN(DIK_SPACE))
		Start_Jump();

	if (KEYDOWN(DIK_Q))
	{
		if (m_eJumpState == JumpState::SWINGING)
		{
			_vector vVel = m_pCurrentSimulation->Get_FinalMass()->Get_Vel();
			m_vVelocity = vVel * 1.3f;
			dynamic_cast<CRope_Simulation*>(m_pCurrentSimulation)->Switch_Soft_Simulating(vVel);
			dynamic_cast<CRope_Simulation*>(m_pCurrentSimulation)->Set_Accelerating(false);

			m_eJumpState = JumpState::FALLING;
			m_eCurrentState = PlayerState::STATE_FALLING;
			m_eNextState = PlayerState::STATE_FALLING;

			m_pCurrentSimulation = nullptr;

		}
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
		if (m_eJumpState == JumpState::SWINGING && m_pCurrentSimulation)
			dynamic_cast<CRope_Simulation*>(m_pCurrentSimulation)->Set_Accelerating(true, 50.f);
	}
	else if (KEYUP(DIK_LSHIFT))
	{
		if(m_pCurrentSimulation)
			dynamic_cast<CRope_Simulation*>(m_pCurrentSimulation)->Set_Accelerating(false);
	}

	// 상태
	Update_State(fTimeDelta);
}

void CPlayer::Root_Transform()
{
	CBody_Player* pBody = dynamic_cast<CBody_Player*>(m_PlayerParts[TEXT("Part_Body")]);
	_float3 vRootTransform = pBody->Get_RootTranform();

	if (m_eJumpState == JumpState::ONOBJECT)
	{
		_vector vMove	= XMLoadFloat3(&vRootTransform);
		_vector	vLook	= m_pTransformCom->Get_State(CTransform::STATE_LOOK);
		_vector	vRight	= m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
		_vector vFinal  = XMVector3Normalize(vLook) * XMVectorGetZ(vMove);
		vFinal += (XMVector3Normalize(vRight) * XMVectorGetX(vMove));

		_float	fMoveScale = XMVectorGetX(XMVector3Length(vFinal));
		_vector vNormal = XMLoadFloat3(&m_vCurrentNormal);
		_vector vSurface = vFinal - (XMVectorGetX(XMVector3Dot(vFinal, vNormal)) * vNormal);
		vSurface = XMVector3Normalize(vSurface);
		m_pTransformCom->Move_Dir(vSurface, fMoveScale);
	}
	else
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

	case PlayerState::STATE_FALLING:
		Handle_FallingState(fTimeDelta);
		break;

	case PlayerState::STATE_CLIMING:
		Handle_ClimingState(fTimeDelta);
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
		if(m_eJumpState == JumpState::ONGROUND || m_eJumpState == JumpState::ONOBJECT)
			Change_State(PlayerState::STATE_MOVE);
		else
			Change_State(PlayerState::STATE_FALLING);
	}
}

void CPlayer::Handle_MoveState(_float fTimeDelta)
{
	if (m_eJumpState == JumpState::SWINGING)
		return;

	if (KEYPRESSING(DIK_W) || KEYPRESSING(DIK_A) || KEYPRESSING(DIK_S) || KEYPRESSING(DIK_D))
	{
		Change_Anim(RUN_CYCLE);

		if (KEYPRESSING(DIK_W))
		{
			if (KEYPRESSING(DIK_A))
				Set_Dir_From_Cam(fTimeDelta, Direction::DIR_FRONT_LEFT);

			else if (KEYPRESSING(DIK_D))
				Set_Dir_From_Cam(fTimeDelta, Direction::DIR_FRONT_RIGHT);

			else
				Set_Dir_From_Cam(fTimeDelta, Direction::DIR_FRONT);
		}
		else if (KEYPRESSING(DIK_S))
		{
			if (KEYPRESSING(DIK_A))
				Set_Dir_From_Cam(fTimeDelta, Direction::DIR_BACK_LEFT);

			else if (KEYPRESSING(DIK_D))
				Set_Dir_From_Cam(fTimeDelta, Direction::DIR_BACK_RIGHT);

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

	/*디버깅*/
	/*if (KEYPRESSING(DIK_W) || KEYPRESSING(DIK_A) || KEYPRESSING(DIK_S) || KEYPRESSING(DIK_D))
	{
		if (KEYPRESSING(DIK_W))
		{
			if (KEYPRESSING(DIK_A))
				m_pTransformCom->Go_Dir(Direction::DIR_FRONT_LEFT, fTimeDelta);

			else if (KEYPRESSING(DIK_D))
				m_pTransformCom->Go_Dir(Direction::DIR_FRONT_RIGHT, fTimeDelta);
			else
				m_pTransformCom->Go_Dir(Direction::DIR_FRONT, fTimeDelta);
		}
		else if (KEYPRESSING(DIK_S))
		{
			if (KEYPRESSING(DIK_A))
				m_pTransformCom->Go_Dir(Direction::DIR_BACK_LEFT, fTimeDelta);

			else if (KEYPRESSING(DIK_D))
				m_pTransformCom->Go_Dir(Direction::DIR_BACK_RIGHT, fTimeDelta);

			else
				m_pTransformCom->Go_Dir(Direction::DIR_BACK, fTimeDelta);
		}
		else if (KEYPRESSING(DIK_A))
		{
			m_pTransformCom->Go_Dir(Direction::DIR_LEFT, fTimeDelta);
		}
		else if (KEYPRESSING(DIK_D))
		{
			m_pTransformCom->Go_Dir(Direction::DIR_RIGHT, fTimeDelta);
		}
	}*/
	else
	{ 
		Change_State(PlayerState::STATE_IDLE);
	}
}

void CPlayer::Handle_FallingState(_float fTimeDelta)
{
	if (m_eJumpState == JumpState::SWINGING)
		return;

	Change_Anim(FLY_NORMAL);

	if ((KEYPRESSING(DIK_W) || KEYPRESSING(DIK_A) || KEYPRESSING(DIK_S) || KEYPRESSING(DIK_D)))
	{
		_vector vVel;

		if(m_pCurrentSimulation )
			vVel = m_pCurrentSimulation->Get_FinalMass()->Get_Vel();

		_vector vDir = {0.f, 0.f, 1.f, 0.f};

		if (KEYPRESSING(DIK_W))
		{
			if (KEYPRESSING(DIK_A))
			{ 
				vDir = Get_Dir_From_Cam(Direction::DIR_FRONT_LEFT);
			}
			else if (KEYPRESSING(DIK_D))
			{
				vDir = Get_Dir_From_Cam(Direction::DIR_FRONT_RIGHT);
			}
			else
			{
				vDir = Get_Dir_From_Cam(Direction::DIR_FRONT);
			}
		}
		else if (KEYPRESSING(DIK_S))
		{
			if (KEYPRESSING(DIK_A))
			{
				vDir = Get_Dir_From_Cam(Direction::DIR_BACK_LEFT);
			}
			else if (KEYPRESSING(DIK_D))
			{
				vDir = Get_Dir_From_Cam(Direction::DIR_BACK_RIGHT);
			}
			else
			{
				vDir = Get_Dir_From_Cam(Direction::DIR_BACK);
			}
		}
		else if (KEYPRESSING(DIK_A))
		{
			vDir = Get_Dir_From_Cam(Direction::DIR_LEFT);
		}
		else if (KEYPRESSING(DIK_D))
		{
			vDir = Get_Dir_From_Cam(Direction::DIR_RIGHT);
		}

		if (m_pCurrentSimulation)
		{
			_float fDirScale = XMVectorGetX(XMVector3Dot(vVel, vDir));

			if (fDirScale < 5.f)
			{
				m_vVelocity += vDir * fTimeDelta * 3.f;
			}
		}
		else
		{
			m_vVelocity += vDir * fTimeDelta * 3.f;
		}
	}
	Set_Dir_From_Velocity(fTimeDelta);

}

void CPlayer::Handle_ClimingState(_float fTimeDelta)
{
	if (m_eJumpState == JumpState::SWINGING)
		return;

	Change_Anim(CLIMB_IDLE);

	
	if (KEYPRESSING(DIK_W) || KEYPRESSING(DIK_A) || KEYPRESSING(DIK_S) || KEYPRESSING(DIK_D))
	{
		_vector vDir = {0.f, 1.f, 0.f};

		if (KEYPRESSING(DIK_W))
		{
			vDir = { 0.f, 1.f, 0.f };

			if (KEYPRESSING(DIK_A))
			{
				vDir -= (m_pTransformCom->Get_State(CTransform::STATE_RIGHT));
			}
			else if (KEYPRESSING(DIK_D))
			{
				vDir += (m_pTransformCom->Get_State(CTransform::STATE_RIGHT));
			}
			else
			{				
			}
		}
		else if (KEYPRESSING(DIK_S))
		{
			vDir = { 0.f, -1.f, 0.f };

			if (KEYPRESSING(DIK_A))
			{
				vDir -= (m_pTransformCom->Get_State(CTransform::STATE_RIGHT));
			}
			else if (KEYPRESSING(DIK_D))
			{
				vDir += (m_pTransformCom->Get_State(CTransform::STATE_RIGHT));
			}
			else
			{
			}
		}
		else if (KEYPRESSING(DIK_A))
		{
			vDir = -(m_pTransformCom->Get_State(CTransform::STATE_RIGHT));
		}
		else if (KEYPRESSING(DIK_D))
		{
			vDir = (m_pTransformCom->Get_State(CTransform::STATE_RIGHT));
		}
		
		vDir = XMVector3Normalize(vDir);
		_vector vNormal = XMLoadFloat3(&m_vClimingNormal);
		_vector vSurface = vDir - (XMVectorGetX(XMVector3Dot(vDir, vNormal)) * vNormal);
		vSurface = XMVector3Normalize(vSurface);
		m_pTransformCom->Move_Dir(vSurface, 3 * fTimeDelta);
	}
	
	//if (KEYPRESSING(DIK_W) || KEYPRESSING(DIK_A) || KEYPRESSING(DIK_S) || KEYPRESSING(DIK_D))
	//{

	//	if (KEYPRESSING(DIK_W))
	//	{
	//		if (KEYPRESSING(DIK_A))
	//		{
	//			m_pTransformCom->Go_Dir(Direction::DIR_UP_LEFT, fTimeDelta);
	//		}
	//		else if (KEYPRESSING(DIK_D))
	//		{
	//			m_pTransformCom->Go_Dir(Direction::DIR_UP_RIGHT, fTimeDelta);
	//		}
	//		else
	//		{
	//			m_pTransformCom->Go_Dir(Direction::DIR_UP, fTimeDelta);
	//		}
	//	}
	//	else if (KEYPRESSING(DIK_S))
	//	{
	//		if (KEYPRESSING(DIK_A))
	//		{
	//			m_pTransformCom->Go_Dir(Direction::DIR_DOWN_LEFT, fTimeDelta);
	//		}
	//		else if (KEYPRESSING(DIK_D))
	//		{
	//			m_pTransformCom->Go_Dir(Direction::DIR_DOWN_RIGHT, fTimeDelta);
	//		}
	//		else
	//		{
	//			m_pTransformCom->Go_Dir(Direction::DIR_DOWN, fTimeDelta);
	//		}
	//	}
	//	else if (KEYPRESSING(DIK_A))
	//	{
	//		m_pTransformCom->Go_Dir(Direction::DIR_LEFT, fTimeDelta);
	//	}
	//	else if (KEYPRESSING(DIK_D))
	//	{
	//		m_pTransformCom->Go_Dir(Direction::DIR_RIGHT, fTimeDelta);
	//	}
	//}

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
			m_vVelocity		= XMVectorSet(0.f, 0.f, 0.f, 0.f);
			m_eJumpState	= JumpState::ONGROUND;
			m_eCurrentState = PlayerState::STATE_IDLE;
			m_eNextState	= PlayerState::STATE_IDLE;
		}
		
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
	}
	else if(m_eJumpState == JumpState::ONGROUND)
	{
		XMStoreFloat3(&m_vCurrentNormal, _vector{ 0.f, 1.f, 0.f });
		SetUp_OnTerrain(m_pTransformCom);
	}
	else if (m_eJumpState == JumpState::SWINGING)
	{
		Handle_Swing(fTimeDelta);
	}
	else if (m_eJumpState == JumpState::ONOBJECT)
	{

	}
}

void CPlayer::Handle_Swing(_float fTimeDelta)
{
	_vector vPos = dynamic_cast<CRope_Simulation*>(m_pCurrentSimulation)->Get_FinalMass()->Get_Pos();
	vPos += _vector{ 0.f, -1.f, 0.f };
	
	if (vPos.m128_f32[1] < 0.f)
	{
		vPos.m128_f32[1] = 0.f;

		m_eJumpState = JumpState::ONGROUND;
		m_eCurrentState = PlayerState::STATE_IDLE;
		m_eNextState = PlayerState::STATE_IDLE;

		m_vVelocity = XMVectorSet(0.f, 0.f, 0.f, 0.f);	
		
		_vector vVel = m_pCurrentSimulation->Get_FinalMass()->Get_Vel();
		dynamic_cast<CRope_Simulation*>(m_pCurrentSimulation)->Switch_Soft_Simulating(vVel);
		dynamic_cast<CRope_Simulation*>(m_pCurrentSimulation)->Set_Accelerating(false);

		m_pCurrentSimulation = nullptr;	
	}
	
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
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

	case Direction::DIR_FRONT_LEFT:
		vTargetDir = vLook - vRight;
		break;

	case Direction::DIR_FRONT_RIGHT:
		vTargetDir = vLook + vRight;
		break;

	case Direction::DIR_BACK_LEFT:
		vTargetDir = (-vLook) - vRight;
		break;

	case Direction::DIR_BACK_RIGHT:
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

_vector CPlayer::Get_Dir_From_Cam(Direction _DIRType)
{
	_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_POSITION) - XMLoadFloat4(&CGameInstance::GetInstance()->Get_CamPosition());
	vLook = XMVector4Normalize(XMVectorSetY(vLook, 0.f));
	_vector vRight = XMVector4Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook));
	_vector vTargetDir = { 1.f, 1.f, 1.f, 0.f };

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

	case Direction::DIR_FRONT_LEFT:
		vTargetDir = vLook - vRight;
		break;

	case Direction::DIR_FRONT_RIGHT:
		vTargetDir = vLook + vRight;
		break;

	case Direction::DIR_BACK_LEFT:
		vTargetDir = (-vLook) - vRight;
		break;

	case Direction::DIR_BACK_RIGHT:
		vTargetDir = (-vLook) + vRight;
		break;
	}

	vTargetDir = XMVector4Normalize(vTargetDir);

	return vTargetDir;
}

void CPlayer::Set_Dir_From_Velocity(_float fTimeDelta)
{
	_vector vDir = XMVector3Normalize(m_vVelocity);
	vDir.m128_f32[1] = 0.f;
	if (XMVectorGetX(XMVector3Length(vDir)) == 0.f)
		return;
	m_pTransformCom->Set_Look_ForLandObject(vDir);
}


void CPlayer::Start_Jump()
{
	if (m_eJumpState == JumpState::ONGROUND || m_eJumpState == JumpState::ONOBJECT)
	{
		m_vVelocity = XMVectorSetY(m_vVelocity, m_fJumpforce); // 초기 점프 속도 설정
		m_eJumpState = JumpState::JUMPING;
		m_eCurrentState = PlayerState::STATE_FALLING;
		m_eNextState = PlayerState::STATE_FALLING;
	}
	else if (m_eJumpState == JumpState::CLIMING)
	{
		m_vVelocity = XMVectorSetY(m_vVelocity, 7.f);
		_vector v = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
		m_vVelocity -= (5 * v);
		m_eJumpState = JumpState::JUMPING;
		m_eCurrentState = PlayerState::STATE_FALLING;
		m_eNextState = PlayerState::STATE_FALLING;
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

void CPlayer::Escape_Swing(JumpState _eJumpState)
{
	m_eJumpState = _eJumpState;
	m_eCurrentState = PlayerState::STATE_FALLING;
	m_eNextState = PlayerState::STATE_FALLING;

	m_vVelocity = XMVectorSet(0.f, 0.f, 0.f, 0.f);

	_vector vVel = m_pCurrentSimulation->Get_FinalMass()->Get_Vel();
	dynamic_cast<CRope_Simulation*>(m_pCurrentSimulation)->Switch_Soft_Simulating(vVel);
	dynamic_cast<CRope_Simulation*>(m_pCurrentSimulation)->Set_Accelerating(false);

	m_pCurrentSimulation = nullptr;
}

_bool CPlayer::Check_TriangleState(JumpState _eJumpState)
{
	int iState = 1;

	if ((_eJumpState == JumpState::ONOBJECT) && m_pCurrentLandCollider != nullptr)
	{
		int iState = m_pRigidColliderCom->Get_Bounding()
			->Check_State(dynamic_cast<CBounding_Triangles*>(m_pCurrentLandCollider->Get_Bounding()), m_iCurrentLandIndex);		
	}
	else if ((_eJumpState == JumpState::CLIMING) && m_pCurrentClimbCollider != nullptr)
	{
		int iState = m_pRigidColliderCom->Get_Bounding()
			->Check_State(dynamic_cast<CBounding_Triangles*>(m_pCurrentClimbCollider->Get_Bounding()), m_iCurrentClimbIndex);
	}

	if (iState == 1)
		return true;
	else
		return false;
}

void CPlayer::Move_Control()
{
	if (m_eJumpState == JumpState::ONOBJECT)
	{
		if(!Check_TriangleState(JumpState::ONOBJECT))
		{
			_bool isFind = false;
			for (auto pair : m_CurrentLandIndeces)
			{
				if (pair.first == m_iCurrentLandIndex)
					continue;

				m_iCurrentLandIndex = pair.first;
				m_vCurrentNormal = pair.second;
				isFind = true;
				break;
			}

			if (!isFind)
			{
				m_eJumpState = JumpState::FALLING;
				m_eCurrentState = PlayerState::STATE_FALLING;
				m_eNextState = PlayerState::STATE_FALLING;
				m_iCurrentLandIndex = -1;
			}
		}
	}

	else if (m_eJumpState == JumpState::CLIMING)
	{
		if (!Check_TriangleState(JumpState::CLIMING)) 
		{
			_bool isFind = false;
			for (auto pair : m_CurrentClimbIndeces)
			{
				if (pair.first == m_iCurrentClimbIndex)
					continue;
			
				m_iCurrentClimbIndex = pair.first;
				m_vClimingNormal = pair.second;
				isFind = true;
				break;
			}
			
			if (!isFind)
			{
				m_eJumpState = JumpState::FALLING;
				m_eCurrentState = PlayerState::STATE_FALLING;
				m_eNextState = PlayerState::STATE_FALLING;
				m_iCurrentClimbIndex = -1;
			}
		}
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

	CCollider::ColliderInitData	ColliderDesc{};
	 
	CGameObject::ColData ColData{};
	ColData.pGameObject = this;
	ColData.eMyColType = COL_PLAYER;
	ColData.iTargetColType = COL_STATIC_OBJECT;
	ColData.isDead = false;

	
	//CCollider::OBB_DESC	BoundingDesc{};
	//BoundingDesc.vExtents = { 0.2f, 0.8f, 0.2f };
	//BoundingDesc.vCenter = {0.f, BoundingDesc.vExtents.y, 0.f};
	//BoundingDesc.vRadians = {0.f, 0.f, 0.f };

	//ColliderDesc.ColData = ColData;
	//ColliderDesc.ColliderDesc = BoundingDesc;

	//if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"),
	//	TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
	//	return E_FAIL;

	CCollider::CAPSULE_DESC	BoundingDesc{};
	BoundingDesc.fHeight = 0.6f;
	BoundingDesc.fRadius = 0.3f;
	BoundingDesc.vCenter = { 0.f, BoundingDesc.fHeight + BoundingDesc.fRadius, 0.f };
	BoundingDesc.vDir = { 0.f, 1.f, 0.f };

	ColliderDesc.ColData = ColData;
	ColliderDesc.ColliderDesc = BoundingDesc;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Capsule"),
		TEXT("Com_RigidCollider"), reinterpret_cast<CComponent**>(&m_pRigidColliderCom), &ColliderDesc)))
		return E_FAIL;
	CGameInstance::GetInstance()->Add_Collider(m_pRigidColliderCom);

	BoundingDesc.fRadius = 0.4f;
	ColliderDesc.ColliderDesc = BoundingDesc;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Capsule"),
		TEXT("Com_TrigerCollider"), reinterpret_cast<CComponent**>(&m_pTriggerColliderCom), &ColliderDesc)))
		return E_FAIL;
	CGameInstance::GetInstance()->Add_Collider(m_pTriggerColliderCom);

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
	m_pSimulationPool = new CSimulation_Pool();
	if (nullptr == m_pSimulationPool)
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

HRESULT CPlayer::Add_CrossHair()
{
	m_pCrossHair = dynamic_cast<CCrossHair*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_CrossHair")));
	Safe_AddRef(m_pCrossHair);

	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_CrossHair"), m_pCrossHair)))
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

	//for (auto iter : m_pLandColliders)
	//	Safe_Release(iter);
	//m_pLandColliders.clear();

	//for (auto iter : m_pClimbColliders)
	//	Safe_Release(iter);
	//m_pClimbColliders.clear();

	Safe_Release(m_pCurrentLandCollider);
	Safe_Release(m_pCurrentClimbCollider);

	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pRigidColliderCom);
	Safe_Release(m_pTriggerColliderCom);

	Safe_Release(m_pCurrentSimulation);
	Safe_Release(m_pSimulationPool);
	Safe_Release(m_pProjectile_Rope);
	Safe_Release(m_pCrossHair);
}
