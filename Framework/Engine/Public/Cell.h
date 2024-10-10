#pragma once

/* 네비게이션을 구성하는 하나의 삼각형이다. */

#include "Base.h"

BEGIN(Engine)

class CCell final : public CBase
{
public:
	enum POINTS { POINT_A, POINT_B, POINT_C, POINT_END };
	enum LINES { LINE_AB, LINE_BC, LINE_CA, LINE_END };
private:
	CCell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CCell() = default;

public:
	_float3 Get_Point(POINTS ePoint) {
		return m_vPoints[ePoint];
	}

	void Set_Neighbor(LINES eLine, const CCell* pNeighbor) {
		m_iNeighborIndices[eLine] = pNeighbor->m_iIndex;
	}

public:
	HRESULT Initialize(const _float3* pPoints, _uint iCellIndex);
	_bool Compare_Points(const _float3& vSourPoint, const _float3& vDestPoint);
	_bool isIn(_fvector vPosition, _int* pNeighborIndex);

#ifdef _DEBUG
public:
	HRESULT Render();

#endif

private:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };

private:
	_float3					m_vPoints[POINT_END];
	_uint					m_iIndex = { 0 };
	_float3					m_vNormals[LINE_END];

	_int					m_iNeighborIndices[LINE_END] = { -1, -1, -1 };

#ifdef _DEBUG
private:
	class CVIBuffer_Cell*	m_pVIBuffer = { nullptr };
#endif

public:
	static CCell* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* pPoints, _uint iCellIndex);
	virtual void Free() override;
};

END