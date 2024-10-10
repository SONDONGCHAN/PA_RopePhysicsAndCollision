#pragma once

#include "Base.h"

/* 화면에 그려져야할 객체들을 그리는 순서대로 보관한다. */
/* 보관하고 있는 순서대로 객체들의 Draw콜을 수행한다. */

BEGIN(Engine)

class CRenderer final : public CBase
{
public:
	enum RENDERGROUP { RENDER_PRIORITY, RENDER_SHADOW, RENDER_NONBLEND, RENDER_NONLIGHT, RENDER_BLEND, RENDER_UI, RENDER_END };

private:
	CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CRenderer() = default;

public:
	HRESULT Initialize();
	HRESULT Add_RenderGroup(RENDERGROUP eRenderGroup, class CGameObject* pGameObject);
	HRESULT Render();

#ifdef _DEBUG
public:
	HRESULT Add_DebugComponent(class CComponent* pComponent);
#endif // _DEBUG



private:
	ID3D11Device*						m_pDevice = { nullptr };
	ID3D11DeviceContext*				m_pContext = { nullptr };

private:
	
	list<class CGameObject*>			m_RenderObjects[RENDER_END];
	class CGameInstance*				m_pGameInstance = { nullptr };

private:
	class CShader*						m_pShader = { nullptr };
	class CVIBuffer_Rect*				m_pVIBuffer = { nullptr };

	_float4x4							m_WorldMatrix, m_ViewMatrix, m_ProjMatrix;

	ID3D11DepthStencilView*				m_pShadowDepthStencilView = { nullptr };

#ifdef _DEBUG
private:
	list<class CComponent*>				m_DebugCom;
#endif // _DEBUG

private:
	HRESULT Render_Priority();
	HRESULT Render_Shadow();
	HRESULT Render_NonBlend();
	HRESULT Render_Lights();
	HRESULT Render_Final();
	HRESULT Render_NonLight();
	HRESULT Render_Blend();
	HRESULT Render_UI();

#ifdef _DEBUG
	HRESULT Render_Debug();
#endif // _DEBUG



public:
	static CRenderer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END