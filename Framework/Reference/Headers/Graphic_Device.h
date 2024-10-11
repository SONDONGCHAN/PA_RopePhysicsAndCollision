#pragma once

#include "Base.h"

BEGIN(Engine)

class CGraphic_Device final : public CBase
{
public:
	CGraphic_Device();
	virtual ~CGraphic_Device() = default;

public:

	HRESULT Ready_Graphic_Device(const GRAPHIC_DESC& GraphicDesc, _Inout_ ID3D11Device** ppDevice,
		_Inout_ ID3D11DeviceContext** ppDeviceContextOut);

	HRESULT Clear_BackBuffer_View(_float4 vClearColor);

	HRESULT Clear_DepthStencil_View();

	HRESULT Present();

private:	

	ID3D11Device*				m_pDevice = nullptr;
		
	ID3D11DeviceContext*		m_pDeviceContext = nullptr;	

	IDXGISwapChain*				m_pSwapChain = nullptr;		

	ID3D11RenderTargetView*		m_pBackBufferRTV = nullptr;
	ID3D11DepthStencilView*		m_pDepthStencilView = nullptr;

	


private:
	HRESULT Ready_SwapChain(HWND hWnd, _bool isWindowed, _uint iWinCX, _uint iWinCY);
	HRESULT Ready_BackBufferRenderTargetView();
	HRESULT Ready_DepthStencilRenderTargetView(_uint iWinCX, _uint iWinCY);

public:
	static CGraphic_Device* Create(const GRAPHIC_DESC& GraphicDesc, _Out_ ID3D11Device** ppDevice,
		_Out_ ID3D11DeviceContext** ppDeviceContextOut);
	virtual void Free() override;
};

END