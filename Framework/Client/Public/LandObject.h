#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CTransform;
class CVIBuffer_Terrain_Basic;
END

BEGIN(Client)

class CLandObject abstract : public CGameObject
{
public:
	typedef struct : public GAMEOBJECT_DESC
	{
		CTransform*					pTerrainTransform = { nullptr };
		CVIBuffer_Terrain_Basic*	pTerrainVIBuffer = { nullptr };
	}LANDOBJ_DESC;
protected:
	CLandObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLandObject(const CLandObject& rhs);
	virtual ~CLandObject() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	CTransform*					m_pTerrainTransform = { nullptr };
	CVIBuffer_Terrain_Basic*	m_pTerrainVIBuffer = { nullptr };

protected:
	HRESULT SetUp_OnTerrain(class CTransform* pTargetTransform);

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END