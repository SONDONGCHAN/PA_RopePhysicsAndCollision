#pragma once

#include "Component.h"

/* 1. 월드스페이스 변환을 위한 행렬을 보유. */
/* 2. 월드상에서의 상태변환을 위한 기능 (움직인다.) */
/* 3. 객체를 생성하게되면 기본적으로 항상 추가되는 방식. */

BEGIN(Engine)

class ENGINE_DLL CTransform final : public CComponent
{
public:
	enum STATE { STATE_RIGHT, STATE_UP, STATE_LOOK, STATE_POSITION, STATE_END };

	typedef struct
	{
		_float		fSpeedPerSec = { 0.0f };
		_float		fRotationPerSec = { 0.0f };
	}TRANSFORM_DESC;

private:
	CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);	
	virtual ~CTransform() = default;

public:
	_vector Get_State(STATE eState) {
		return XMLoadFloat4x4(&m_WorldMatrix).r[eState];
	}

	_float3 Get_Scaled() {
		return _float3(
			XMVectorGetX(XMVector3Length(Get_State(STATE_RIGHT))), 
			XMVector3Length(Get_State(STATE_UP)).m128_f32[0],
			XMVector3Length(Get_State(STATE_LOOK)).m128_f32[0]
		);
	}

	_matrix Get_WorldMatrix() const {

		return XMLoadFloat4x4(&m_WorldMatrix);
	}

	_float4x4 Get_WorldMatrix4x4() const {

		return m_WorldMatrix;
	}
	void Set_WorldMatrix(const _float4x4& _matrix) { m_WorldMatrix = _matrix; }
	void Set_WorldMatrix(const XMMATRIX& _matrix) { XMStoreFloat4x4(&m_WorldMatrix, _matrix);}

	_matrix Get_WorldMatrix_Inverse() {
		return XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix));
	}

	void Set_Scaling(_float fX, _float fY, _float fZ);

	void Set_State(STATE eState, _fvector vState)
	{
		_float4		vTmp;
		XMStoreFloat4(&vTmp, vState);
		memcpy(&m_WorldMatrix.m[eState], &vTmp, sizeof(_float4));
	}

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	HRESULT Bind_ShaderResource(class CShader*, const _char* pContantName);


public:
	void Go_Straight(_float fTimeDelta, class CNavigation* pNavigation = nullptr);
	void Go_Backward(_float fTimeDelta);
	void Go_Left(_float fTimeDelta);
	void Go_Right(_float fTimeDelta);
	void Go_Dir(Direction _eDirection, _float fTimeDelta);
	void Move(XMVECTOR _Movement, CNavigation* pNavigation);
	void Move_Dir(_vector _Dir, _float _fScale);


	void Turn(_fvector vAxis, _float fTimeDelta);
	void Rotation(_fvector vAxis, _float fRadian);

	void MoveTo(_fvector vPoint, _float fLimit, _float fTimeDelta);
	void LookAt(_fvector vPoint);
	void LookAt_ForLandObject(_fvector vPoint);

	void Set_Look(_fvector direction);
	void Set_Look_ForLandObject(_fvector direction);

public:
	void Set_SpeedPerSec(_float _fSpeed) { m_fSpeedPerSec = _fSpeed; }

private:
	_float4x4			m_WorldMatrix = {};

	_float				m_fSpeedPerSec = { 0.0f };
	_float				m_fRotationPerSec = { 0.0f };

public:
	static CTransform* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END