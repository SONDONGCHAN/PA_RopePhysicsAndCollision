#pragma once

#include "Base.h"

/* 1. 원형객체들을 생성하여 보관한다. */
/* 2. 지정한 원형객체를 복제하여 사본객체를 생성한다. */
/* 3. 생성한 사본객체를 레이어에 추가하여 지정한 레벨의 갯수로 나누어 보관한다.(원형객체는 선택) */
/* 4. 추가되어있는 사본객체들의 PriorityTick, Tick, LateTick을 호출한다 .*/

BEGIN(Engine)


class CObject_Manager final : public CBase
{	
private:
	CObject_Manager();
	virtual ~CObject_Manager() = default;

public:
	class CComponent* Get_Component(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strComTag, _uint iIndex);
	class CGameObject* Get_GameObject(_uint iLevelIndex, const wstring& strLayerTag, _uint iIndex);

public:
	HRESULT Initialize(_uint iNumLevels);
	void Priority_Tick(_float fTimeDelta);
	void Tick(_float fTimeDelta);
	void Late_Tick(_float fTimeDelta);

public:
	HRESULT Add_Prototype(const wstring& strPrototypeTag, class CGameObject* pPrototype);
	HRESULT Add_CloneObject(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strPrototypeTag, void* pArg);
	HRESULT Add_GameObject(_uint iLevelIndex, const wstring& strLayerTag, class CGameObject* pGameObject);
	class CGameObject* Clone_GameObject(const wstring& strPrototypeTag, void* pArg);
	void Clear(_uint iLevelIndex);
	

private:
	map<const wstring, class CGameObject*>			m_Prototypes;

	_uint											m_iNumLevels = { 0 };
	map<const wstring, class CLayer*>*				m_pLayers = { nullptr };
	typedef map<const wstring, class CLayer*>		LAYERS;

private:
	class CGameObject* Find_Prototype(const wstring& strPrototypeTag);
	class CLayer* Find_Layer(_uint iLevelIndex, const wstring& strLayerTag);

public:
	static CObject_Manager* Create(_uint iNumLevels);
	virtual void Free() override;
};

END

