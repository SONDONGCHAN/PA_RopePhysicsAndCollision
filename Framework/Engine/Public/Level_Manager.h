#pragma once

#include "Base.h"

/* 1. ���� ȭ�鿡 ��������ϴ� ������ �����Ѵ�. */
/* 2. ������ �ݺ������� ƽ�� ������ ȣ���Ѵ�. */
/* 3. ������ ��ü�� �� ���� ������ �ڿ�(��ü����, ���ҽ���) ���� �����Ѵ�. */

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
	/* ���ο� ������ ���� ������ ��ü���ش�. */
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