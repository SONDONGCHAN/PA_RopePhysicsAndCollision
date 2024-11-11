#pragma once

#include "Client_Defines.h"
#include "LandObject.h"
#include "Projectile_Rope.h"
#include "Rope_Simulation.h"
#include "Simulation_Pool.h"
#include "CrossHair.h"

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
		STATE_IDLE,		// ��� ����
		STATE_MOVE,		// �̵�
		STATE_FALLING,	// ����
		STATE_ATTACK,	// ����
		STATE_CLIMING,	// Ŭ���̹�
	};

	enum class JumpState
	{
		ONGROUND,		// ���� ��
		JUMPING,		// ���� ��
		SWINGING,		// ���� ��
		FALLING,		// ���� ��
		ONOBJECT,		// ������Ʈ ��

	};

	enum PlayerAnim
	{
		IDLE			= 0,	// ���
		WALK			= 1,	// �ȱ�
		WALK_STOP		= 2,	// �ȱ� ����
		RUN_START		= 3,	// �޸��� ����
		RUN_CYCLE		= 4,	// �޸��� �ݺ�
		RUN_STOP		= 5,	// �޸��� ����
		RUN_TO_WALK		= 7,	// �޸��� -> �ȱ�
		SPRINT_START	= 8,	// ���� ����
		SPRINT_CYCLE	= 9,	// ���� �ݺ�
		SPRINT_STOP		= 10,	// ���� ����
		SPRINT_TO_RUN	= 11,	// ���� -> �޸���

		FLY_NORMAL		= 32,	// ü�� �ݺ�	
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

public:
	virtual void Event_CollisionEnter(ColData* _ColData) override;
	virtual void Event_CollisionStay(ColData* _ColData) override;
	virtual void Event_CollisionExit(ColData* _ColData) override;

public:
	void	Start_Stiff_Simulating(_vector _vDir, _vector _vPos, _float _fM, _float _fLastM);
	void	End_Simulating();
	
private:
	void	KeyInput(_float fTimeDelta);
	void	Root_Transform();

private:
	void	Change_State(PlayerState eNewState);		// ���� ��ȯ ó��
	void	Update_State(_float fTimeDelta);			// ���¿� ���� ���� ó��

	void	Change_Anim(PlayerAnim eNewAnim);			// ���¿� ���� ���� ó��

	// ���º� ó�� �Լ���
	void	Handle_IdleState(_float fTimeDelta);
	void	Handle_MoveState(_float fTimeDelta);
	void	Handle_FallingState(_float fTimeDelta);
	void	Handle_AttackState(_float fTimeDelta);

	//
	void	Handle_Jump(_float fTimeDelta);
	void	Handle_Swing(_float fTimeDelta);

private:
	void	Set_Dir_From_Cam(_float fTimeDelta, Direction _DIRType);
	_vector	Get_Dir_From_Cam(Direction _DIRType);

	void	Set_Dir_From_Velocity(_float fTimeDelta);

private:
	void	Start_Jump();
	void	Start_Swing();
	void	Escape_Swing(JumpState _eJumpState);

private:
	PlayerState	m_eCurrentState = PlayerState :: STATE_IDLE;	// ���� ����
	PlayerState	m_eNextState	= PlayerState :: STATE_IDLE;	// ���� ����
	_float		m_fStateTimer	= 0.0f;							// ���� ���� �ð�
	PlayerAnim	m_iAnimation	= { IDLE };						// ���� �ִϸ��̼�
	JumpState	m_eJumpState	= JumpState::ONGROUND;			// ���� ���� ����

private:
	CNavigation*		m_pNavigationCom = { nullptr };
	CCollider*			m_pColliderCom = { nullptr };
	CProjectile_Rope*	m_pProjectile_Rope = { nullptr };
	CCrossHair*			m_pCrossHair = { nullptr };

private:
	CSimulation*		m_pCurrentSimulation = { nullptr };
	CSimulation_Pool*	m_pSimulationPool = { nullptr };

private:
	const	_float m_fGravity{ -9.81f };		// �߷� ���ӵ�
	const	_float m_fJumpforce{ 20.f };		// �ʱ� ���� �ӵ�
	const	_float m_fTerminalVelocity = -25.f; // �ִ� ���� �ӵ�
	const	_float m_fDragCoefficient = 0.1f;	// ���� ���� ���

private:
	_vector	m_vVelocity{0.f, 0.f, 0.f, 0.f};// �÷��̾� �ӵ�


private:
	map<const wstring, class CPartObject*>		m_PlayerParts;

private:
	HRESULT Add_Components();
	HRESULT Add_PartObjects();
	HRESULT Add_Simulation();
	HRESULT Add_Projectile();
	HRESULT Add_CrossHair();

public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END