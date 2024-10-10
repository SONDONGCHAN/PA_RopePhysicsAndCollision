#pragma once

#include "Client_Defines.h"
#include "Level.h"

/* 생성 시, 전달받은 index해당하는 레벨용 자원을 준비하기위한 로더객체를 생성한다. . */
/* 동시에 로딩화면을 보여준다. */

BEGIN(Client)

class CLevel_Loading final : public CLevel
{
private:
	CLevel_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Loading() = default;

public:
	virtual HRESULT Initialize(LEVEL eNextLevel);
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	HRESULT Ready_Layer_BackGround();
	HRESULT Ready_Layer_UI();

private:
	class CLoader*			m_pLoader = { nullptr };
	LEVEL					m_eNextLevel = { LEVEL_END };

public:
	static CLevel_Loading* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevel);
	virtual void Free() override;
};

END