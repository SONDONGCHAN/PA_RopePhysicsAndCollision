#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Instancing abstract : public CVIBuffer
{
public:
	typedef struct
	{
		_float3			vPivot;
		_float3			vCenter;
		_float3			vRange;
		_float2			vSize;
		_float2			vSpeed;
		_float2			vLifeTime;
		_bool			isLoop;
		_float4			vColor;
		_float			fDuration;
	}INSTANCE_DESC;

protected:
	CVIBuffer_Instancing(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Instancing(const CVIBuffer_Instancing& rhs);
	virtual ~CVIBuffer_Instancing() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;	
	virtual HRESULT Render();
	virtual HRESULT Bind_Buffers();
public:
	void Tick_Drop(_float fTimeDelta);
	void Tick_Spread(_float fTimeDelta);

protected:
	ID3D11Buffer*				m_pVBInstance = { nullptr };
	_uint						m_iInstanceStride = { 0 };
	_uint						m_iNumInstance = { 0 };
	_uint						m_iIndexCountPerInstance = { 0 };

	random_device				m_RandomDevice;
	mt19937_64					m_RandomNumber;
	INSTANCE_DESC				m_InstanceData;

	_float*						m_pSpeed = { nullptr };
	_float*						m_pLifeTime = { nullptr };


	_float						m_fTimeAcc = { 0.f };
	_bool						m_isFinished = { false };


public:
	virtual CComponent* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END