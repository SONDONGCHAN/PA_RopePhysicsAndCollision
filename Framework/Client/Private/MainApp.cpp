#include "stdafx.h"
#include "..\Public\MainApp.h"

#include "GameInstance.h"

#include "Level_Loading.h"


CMainApp::CMainApp()
	: m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);		
}

HRESULT CMainApp::Initialize()
{
	GRAPHIC_DESC			GraphicDesc = {};

	GraphicDesc.hWnd = g_hWnd;
	GraphicDesc.isWindowed = true;
	GraphicDesc.iWinSizeX = g_iWinSizeX;
	GraphicDesc.iWinSizeY = g_iWinSizeY;

	/* 내 게임에 필요한 기초 초기화작업을 거치낟. */
	if (FAILED(m_pGameInstance->Initialize_Engine(g_hInst, LEVEL_END, GraphicDesc, &m_pDevice, &m_pContext)))
		return E_FAIL;	

	if (FAILED(Ready_ETC()))
		return E_FAIL;

	if (FAILED(Ready_Prototype_Component_Static()))
		return E_FAIL;

	/* 내가 시작하고자 하는 레벨을 할당한다. */
	if (FAILED(Open_Level(LEVEL_LOGO)))
		return E_FAIL;

	return S_OK;
}

void CMainApp::Tick(_float fTimeDelta)
{
	m_pGameInstance->Tick_Engine(fTimeDelta);

#ifdef _DEBUG
	m_fTimeAcc += fTimeDelta;
#endif // _DEBUG
}

HRESULT CMainApp::Render()
{
	if (FAILED(m_pGameInstance->Begin_Draw(_float4(0.f, 0.5f, 0.5f, 1.f))))
		return E_FAIL;	

	if (FAILED(m_pGameInstance->Draw()))
		return E_FAIL;

#ifdef _DEBUG
	++m_iNumRender;

	if (m_fTimeAcc >= 1.f)
	{
		wsprintf(m_szFPS, TEXT("FPS:%d"), m_iNumRender);

		m_iNumRender = 0;
		m_fTimeAcc = 0.f;
	}

	// MakeSpriteFont "넥슨lv1고딕 Bold" /FontSize:30 /FastPack /CharacterRegion:0x0020-0x00FF /CharacterRegion:0x3131-0x3163 /CharacterRegion:0xAC00-0xD800 /DefaultCharacter:0xAC00 140.spritefont
	if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_Default"), m_szFPS, _float2(0.f, 0.f), XMVectorSet(0.f, 0.f, 0.f, 1.f), 0.f, _float2(0.f, 0.f), 0.5f)))
		return E_FAIL;

#endif // _DEBUG
	

	if (FAILED(m_pGameInstance->End_Draw()))
		return E_FAIL;


	return S_OK;
}

HRESULT CMainApp::Open_Level(LEVEL eStartLevel)
{
	if (LEVEL_LOADING == eStartLevel)
		return E_FAIL;

	return m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, eStartLevel));
	
}

HRESULT CMainApp::Ready_ETC()
{
	if (FAILED(m_pGameInstance->Add_Font(TEXT("Font_Default"), TEXT("../Bin/Resources/Fonts/140.spritefont"))))
		return E_FAIL;

	_ulong		dwByte = { 0 };
	HANDLE		hFile = CreateFile(TEXT("../Bin/DataFiles/Navigation.dat"), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	_float3			vPoints[3];

	_uint CellSize = 10.f;

	for (int i = 0; i < 5; ++i)
	{
		for (int j = 0; j < 5; ++j)
		{
			vPoints[0] = _float3(i * CellSize,		 0.f, j * CellSize);
			vPoints[1] = _float3(i * CellSize,		 0.f, (j + 1) * CellSize);
			vPoints[2] = _float3((i + 1) * CellSize, 0.f, j * CellSize);
			WriteFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);
			
			vPoints[0] = _float3(i * CellSize, 0.f, (j + 1) * CellSize);
			vPoints[1] = _float3((i + 1) * CellSize, 0.f, (j + 1) * CellSize);
			vPoints[2] = _float3((i + 1) * CellSize, 0.f, j * CellSize);
			WriteFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);
		}
	}

	//vPoints[0] = _float3(0.0f, 0.f, 0.f);
	//vPoints[1] = _float3(0.0f, 0.f, 10.f);
	//vPoints[2] = _float3(10.0f, 0.f, 0.f);
	//WriteFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);

	//vPoints[0] = _float3(0.0f, 0.f, 10.f);
	//vPoints[1] = _float3(10.0f, 0.f, 10.f);
	//vPoints[2] = _float3(10.0f, 0.f, 0.f);
	//WriteFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);

	//vPoints[0] = _float3(0.0f, 0.f, 20.f);
	//vPoints[1] = _float3(10.0f, 0.f, 10.f);
	//vPoints[2] = _float3(0.0f, 0.f, 10.f);
	//WriteFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);

	//vPoints[0] = _float3(10.0f, 0.f, 10.f);
	//vPoints[1] = _float3(20.0f, 0.f, 0.f);
	//vPoints[2] = _float3(10.0f, 0.f, 0.f);
	//WriteFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);

	CloseHandle(hFile);

	ID3D11Texture2D*			pTexture2D = { nullptr };

	D3D11_TEXTURE2D_DESC	TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc.Width = 256;
	TextureDesc.Height = 256;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;
	TextureDesc.Usage = D3D11_USAGE_DYNAMIC;
	TextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;		
	TextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	TextureDesc.MiscFlags = 0;

	_ulong*		pPixel = new _ulong[TextureDesc.Width * TextureDesc.Height];

	for (size_t i = 0; i < TextureDesc.Height; i++)
	{
		for (size_t j = 0; j < TextureDesc.Width; j++)
		{
			_uint		iIndex = i * TextureDesc.Width + j;

			pPixel[iIndex] = D3DCOLOR_ARGB(255, 255, 0, 0);
		}
	}

	pPixel[0] = D3DCOLOR_ARGB(255, 255, 255, 0);

	D3D11_SUBRESOURCE_DATA		InitialData = {};

	InitialData.pSysMem = pPixel;
	InitialData.SysMemPitch = TextureDesc.Width * 4;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, &InitialData, &pTexture2D)))
		return E_FAIL;	

	for (size_t i = 0; i < TextureDesc.Height; i++)
	{
		for (size_t j = 0; j < TextureDesc.Width; j++)
		{
			_uint		iIndex = i * TextureDesc.Width + j;

			if(j < 128)
				pPixel[iIndex] = D3DCOLOR_ARGB(255, 255, 255, 255);
			else
				pPixel[iIndex] = D3DCOLOR_ARGB(255, 0, 0, 0);

		}
	}


	D3D11_MAPPED_SUBRESOURCE		SubResource = {};

	m_pContext->Map(pTexture2D, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource);

	memcpy(SubResource.pData, pPixel, sizeof(_ulong) * TextureDesc.Width * TextureDesc.Height);

	m_pContext->Unmap(pTexture2D, 0);

	if (FAILED(DirectX::SaveDDSTextureToFile(m_pContext, pTexture2D, TEXT("../Bin/Resources/Textures/Terrain/GaraMask.dds"))))
		return E_FAIL;

	Safe_Delete_Array(pPixel);
	Safe_Release(pTexture2D);

	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_Component_Static()
{
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPosTex.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;

	return S_OK;
}

CMainApp * CMainApp::Create()
{
	CMainApp*		pInstance = new CMainApp();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CMainApp");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMainApp::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	Safe_Release(m_pGameInstance);

	CGameInstance::Release_Engine();
}
