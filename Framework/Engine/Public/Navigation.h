#pragma once

/* 네비게이션을 구성하는 Cell을 보관한다. */
/* 이 네비게이션컴포넌트를 이요하는 객체가 어떤 쎌 안에 있었는가?! */

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CNavigation final : public CComponent
{
public:
	typedef struct
	{
		_int			iStartCellIndex;
	}NAVI_DESC;

private:
	CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNavigation(const CNavigation& rhs);
	virtual ~CNavigation() = default;

public:
	HRESULT Initialize_Prototype(const wstring& strNavigationDataFilePath);
	HRESULT Initialize(void* pArg) override;
	void Tick(_fmatrix TerrainWorldMatrix);
	_bool isMove(_fvector vPosition);


#ifdef _DEBUG
public:
	virtual HRESULT Render() override;
#endif

private:
	_int							m_iCurrentIndex = { -1 };
	vector<class CCell*>			m_Cells;
	static _float4x4				m_WorldMatrix;
	

#ifdef _DEBUG
private:
	class CShader*					m_pShader = { nullptr };
#endif

private:
	HRESULT Make_Neighbors();

public:
	static CNavigation* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strNavigationDataFilePath);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END