#pragma once

#include "Base.h"

/* 1. ������ü���� �����Ͽ� �����Ѵ�. */
/* 2. ������ ������ü�� �����Ͽ� �纻��ü�� �����Ѵ�. */
/* 3. ������ �纻��ü�� ���̾ �߰��Ͽ� ������ ������ ������ ������ �����Ѵ�.(������ü�� ����) */
/* 4. �߰��Ǿ��ִ� �纻��ü���� PriorityTick, Tick, LateTick�� ȣ���Ѵ� .*/

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

