#pragma once

/* �׺���̼��� �����ϴ� Cell�� �����Ѵ�. */
/* �� �׺���̼�������Ʈ�� �̿��ϴ� ��ü�� � �� �ȿ� �־��°�?! */

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