#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

struct BoneIndexVertex 
{
	_int BoneIndex;
};

struct CB_BoneMatrices 
{
	XMFLOAT4X4 BoneMatrices[128];
};

class ENGINE_DLL CVIBuffer_Skeletal final : public CVIBuffer
{
private:
	CVIBuffer_Skeletal(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _int _BoneCount);
	CVIBuffer_Skeletal(const CVIBuffer_Skeletal& rhs);
	virtual ~CVIBuffer_Skeletal() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

private:
	HRESULT Initialize_LineBuffer();
	HRESULT Initialize_ConstantBuffer();

public:
	void UpdateBoneMatrices(const std::vector<XMFLOAT4X4*>& boneMatrices);
	virtual HRESULT Render() override;

public:
	static CVIBuffer_Skeletal* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _int _BoneCount);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

private:
	_int			m_BoneCount { 0 };
	ID3D11Buffer* m_pConstantBuffer { nullptr };
};

END