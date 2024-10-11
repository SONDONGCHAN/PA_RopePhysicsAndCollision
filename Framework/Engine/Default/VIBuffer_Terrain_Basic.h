#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Terrain_Basic final : public CVIBuffer
{
private:
	CVIBuffer_Terrain_Basic(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Terrain_Basic(const CVIBuffer_Terrain_Basic& rhs);
	virtual ~CVIBuffer_Terrain_Basic() = default;

public:
	virtual HRESULT Initialize_Prototype(const _int _iSizeX, const _int _iSizeZ);
	virtual HRESULT Initialize(void* pArg) override;

public:
	_float Compute_Height(_fvector vLocalPos);
	void Culling(class CTransform* pTransform);

private:
	_uint			m_iNumVerticesX = { 0 };
	_uint			m_iNumVerticesZ = { 0 };

public:
	static CVIBuffer_Terrain_Basic* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _int _iSizeX, const _int _iSizeZ);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END