#include "stdafx.h"
#include "Level_Loading.h"
#include "Loader.h"

#include "GameInstance.h"

#include "Level_Logo.h"
#include "Level_GamePlay.h"


CLevel_Loading::CLevel_Loading(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Loading::Initialize(LEVEL eNextLevel)
{
	m_eNextLevel = eNextLevel;

	/* 다음레벨에 대한 자원을 준비한다. */
	m_pLoader = CLoader::Create(m_pDevice, m_pContext, eNextLevel);
	if (nullptr == m_pLoader)
		return E_FAIL;

	/*if (FAILED(Ready_Layer_BackGround()))
		return E_FAIL;

	if (FAILED(Ready_Layer_UI()))
		return E_FAIL;*/

	return S_OK;
}

void CLevel_Loading::Tick(_float fTimeDelta)
{
	/* 로더에게 로딩이 마무리되었냐? */
	if (true == m_pLoader->isFinished())
	{
		//if (GetKeyState(VK_SPACE) & 0x8000)
		//{
			/* 진짜 진입하고 싶었던 레벨로 들어간다. */
			CLevel*		pLevel = { nullptr };
			
			CGameInstance::GetInstance()->Set_CurrentLevel(m_eNextLevel);

			switch (m_eNextLevel)
			{
			case LEVEL_LOGO:
				pLevel = CLevel_Logo::Create(m_pDevice, m_pContext);
				break;

			case LEVEL_GAMEPLAY:
				pLevel = CLevel_GamePlay::Create(m_pDevice, m_pContext);

				break;
			}

			if (nullptr == pLevel)
				return;

			if (FAILED(m_pGameInstance->Open_Level(m_eNextLevel, pLevel)))
				return;

		//}
	}
}

HRESULT CLevel_Loading::Render()
{
	if (nullptr == m_pLoader)
		return E_FAIL;

#ifdef _DEBUG
	m_pLoader->Output_LoadingText();
#endif

	return S_OK;
}

HRESULT CLevel_Loading::Ready_Layer_BackGround()
{
	return E_NOTIMPL;
}

HRESULT CLevel_Loading::Ready_Layer_UI()
{
	return E_NOTIMPL;
}

CLevel_Loading * CLevel_Loading::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, LEVEL eNextLevel)
{
	CLevel_Loading*		pInstance = new CLevel_Loading(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevel)))
	{
		MSG_BOX("Failed to Created : CLevel_Loading");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Loading::Free()
{
	__super::Free();

	Safe_Release(m_pLoader);

}
