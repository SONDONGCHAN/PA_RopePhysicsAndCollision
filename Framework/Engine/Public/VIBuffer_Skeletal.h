#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

struct BoneIndexVertex 
{
	XMFLOAT3 Position;
	_int BoneIndex;
};

struct CB_BoneMatrices 
{
	XMFLOAT4X4 BoneMatrices[128];
};

class ENGINE_DLL CVIBuffer_Skeletal final : public CVIBuffer
{
private:
	CVIBuffer_Skeletal(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, vector<class CBone*>& _Bones);
	CVIBuffer_Skeletal(const CVIBuffer_Skeletal& rhs);
	virtual ~CVIBuffer_Skeletal() = default;

public:
	HRESULT Initialize_Prototype(vector<class CBone*>& _Bones) ;
	virtual HRESULT Initialize(void* pArg) override;

private:
	HRESULT Initialize_LineBuffer(vector<class CBone*>& _Bones);

public:
	virtual HRESULT Render() override;

public:
	static CVIBuffer_Skeletal* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, vector<class CBone*>& _Bones);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

private:
	_int			m_BoneCount { 0 };

};

END