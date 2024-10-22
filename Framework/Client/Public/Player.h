#pragma once

#include "Client_Defines.h"
#include "LandObject.h"
#include "Rope_Simulation.h"

BEGIN(Engine)

class CCollider;
class CNavigation;

END


BEGIN(Client)

class CPlayer final : public CLandObject
{
public:
	enum class PlayerState 
	{
		STATE_IDLE,           // 대기 상태
		STATE_MOVE,           // 이동
		STATE_ATTACK,         // 공격
		STATE_JUMP,           // 점프
	};
	enum PlayerAnim
	{
		IDLE			= 0,	// 대기
		WALK			= 1,	// 걷기
		WALK_STOP		= 2,	// 걷기 중지
		RUN_START		= 3,	// 달리기 시작
		RUN_CYCLE		= 4,	// 달리기 반복
		RUN_STOP		= 5,	// 달리기 중지
		RUN_TO_WALK		= 7,	// 달리기 -> 걷기
		SPRINT_START	= 8,	// 질주 시작
		SPRINT_CYCLE	= 9,	// 질주 반복
		SPRINT_STOP		= 10,	// 질주 중지
		SPRINT_TO_RUN	= 11,	// 질주 -> 달리기
	};

private:
	CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer(const CPlayer& rhs);
	virtual ~CPlayer() = default;

public:
	class CComponent* Get_PartObjectComponent(const wstring& strPartObjTag, const wstring& strComTag);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	void	KeyInput(_float fTimeDelta);
	void	Root_Transform();

private:
	void	Change_State(PlayerState eNewState);		// 상태 전환 처리
	void	Update_State(_float fTimeDelta);			// 상태에 따른 동작 처리

	void	Change_Anim(PlayerAnim eNewAnim);			// 상태에 따른 동작 처리

	// 상태별 처리 함수들
	void	Handle_IdleState(_float fTimeDelta);
	void	Handle_MoveState(_float fTimeDelta);
	void	Handle_AttackState(_float fTimeDelta);
	void	Handle_JumpState(_float fTimeDelta);

private:
	void	Set_Dir_From_Cam(_float fTimeDelta, Direction _DIRType);


private:
	PlayerState	m_eCurrentState = PlayerState :: STATE_IDLE;	// 현재 상태
	PlayerState	m_eNextState	= PlayerState :: STATE_IDLE;	// 다음 상태
	_float		m_fStateTimer	= 0.0f;						// 상태 유지 시간
	PlayerAnim	m_iAnimation = { IDLE };

private:
	CNavigation*	m_pNavigationCom = { nullptr };
	CCollider*		m_pColliderCom = { nullptr };

private:
	CRope_Simulation* m_pRopeSimulation = { nullptr };

private:
	map<const wstring, class CPartObject*>		m_PlayerParts;

private:
	HRESULT Add_Components();
	HRESULT Add_PartObjects();
	HRESULT Add_Simulation();

	_int iRend = 0;

public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END