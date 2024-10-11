#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)

class CMainApp final : public CBase
{
private:
	CMainApp();
	virtual ~CMainApp() = default;

public:
	HRESULT Initialize();
	void Tick(_float fTimeDelta);
	HRESULT Render();

private:
	CGameInstance*			m_pGameInstance = { nullptr };
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };

#ifdef _DEBUG
private:
	_tchar					m_szFPS[MAX_PATH] = TEXT("");
	_uint					m_iNumRender = { 0 };
	_float					m_fTimeAcc = { 0.0f };
#endif // _DEBUG

private:
	HRESULT Open_Level(LEVEL eStartLevel);
	HRESULT Ready_ETC();
	HRESULT Ready_Prototype_Component_Static();


public:
	static CMainApp* Create();
	virtual void Free() override;
};

END