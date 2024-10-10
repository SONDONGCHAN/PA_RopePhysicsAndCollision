#pragma once

#include "Base.h"

/* 1. 현재 화면에 보여줘야하는 레벨을 보관한다. */
/* 2. 레벨의 반복적으로 틱과 렌더를 호출한다. */
/* 3. 레벨을 교체할 때 이전 레벨용 자원(객체원형, 리소스들) 들을 정리한다. */

BEGIN(Engine)

class CLevel_Manager final : public CBase
{
private:
	CLevel_Manager();
	virtual ~CLevel_Manager() = default;

public:
	HRESULT Initialize();
	void Tick(_float fTimeDelta);
	HRESULT Render();
	/* 새로운 레벨로 기존 레벨을 교체해준다. */
	HRESULT Open_Level(_uint iLevelIndex, class CLevel* pNewLevel);

private:
	_uint					m_iLevelIndex = { 0 } ;
	class CLevel*			m_pCurrentLevel = { nullptr };
	class CGameInstance*	m_pGameInstance = { nullptr };
public:
	static CLevel_Manager* Create();
	virtual void Free() override;
};

END