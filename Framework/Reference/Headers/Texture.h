#pragma once

#include "Component.h"

/* 텍스쳐들을 로드하고 저장한다. */

BEGIN(Engine)

class ENGINE_DLL CTexture final : public CComponent
{
private:
	CTexture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTexture(const CTexture& rhs);
	virtual ~CTexture() = default;

public:
	virtual HRESULT Initialize_Prototype(const wstring& strTextureFilePath, _uint iNumTextures);
	virtual HRESULT Initialize(void* pArg) override;

public:
	HRESULT Bind_ShaderResource(class CShader* pShader, const _char* pConstantName, _uint iIndex);
	HRESULT Bind_ShaderResources(class CShader* pShader, const _char* pConstantName);

private:
	_uint												m_iNumTextures = { 0 };
	vector<ID3D11ShaderResourceView*>					m_SRVs;


public:
	static CTexture* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strTextureFilePath, _uint iNumTextures);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END