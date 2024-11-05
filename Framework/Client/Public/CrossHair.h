#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CVIBuffer_Point;
END

BEGIN(Client)

class CCrossHair : public CGameObject
{
private:
	CCrossHair(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCrossHair(const CCrossHair& rhs);
	virtual ~CCrossHair() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_Point* m_pVIBufferCom = { nullptr };

private:
	_float			m_fX, m_fY, m_fSizeX, m_fSizeY;
	_float4x4		m_ViewMatrix, m_ProjMatrix;
	_float4			m_vColor;
	_float			m_fThickness;
	_float			m_fSize;

private:
	HRESULT Add_Component();


public:
	static CCrossHair* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END