#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Terrain_Basic;
END

BEGIN(Client)

class CTerrain_Basic final : public CGameObject
{
public:
	enum TEXTURETYPE { TYPE_DIFFUSE, TYPE_MASK, TYPE_BRUSH, TYPE_END };
private:
	CTerrain_Basic(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTerrain_Basic(const CTerrain_Basic& rhs);
	virtual ~CTerrain_Basic() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom[TYPE_END] = { nullptr };
	CNavigation* m_pNavigationCom = { nullptr };
	CVIBuffer_Terrain_Basic* m_pVIBufferCom = { nullptr };


private:
	HRESULT Add_Component();
	HRESULT Bind_ShaderResources();

public:
	static CTerrain_Basic* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END