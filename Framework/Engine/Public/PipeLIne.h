#pragma once

#include "Base.h"

/* 뷰, 투영행렬을 보관한다. */
/* 뷰, 투영행렬을 리턴한다. */
/* 뷰, 투영행렬의 역행렬을 구하고 보관하낟.. */
/* 카메라 월드위치를 구하여 보관한다. */
BEGIN(Engine)

class CPipeLine final : public CBase
{
public:
	enum D3DTRANSFORMSTATE { D3DTS_VIEW, D3DTS_PROJ, D3DTS_END };
private:
	CPipeLine();
	virtual ~CPipeLine() = default;

public:
	void Set_Transform(D3DTRANSFORMSTATE eState, _fmatrix TransformMatrix) {
		XMStoreFloat4x4(&m_TransformMatrix[eState], TransformMatrix);
	}

	_float4x4 Get_Transform_Float4x4(D3DTRANSFORMSTATE eState) {
		return m_TransformMatrix[eState];
	}

	_matrix Get_Transform_Matrix(D3DTRANSFORMSTATE eState) {
		return XMLoadFloat4x4(&m_TransformMatrix[eState]);
	}

	_float4x4 Get_Transform_Float4x4_Inverse(D3DTRANSFORMSTATE eState) {
		return m_TransformMatrix_Inverse[eState];
	}

	_matrix Get_Transform_Matrix_Inverse(D3DTRANSFORMSTATE eState) {
		return XMLoadFloat4x4(&m_TransformMatrix_Inverse[eState]);
	}

	_float4 Get_CamPosition() {
		return m_vCamPosition;
	}

public:
	void Tick();

private:
	_float4x4			m_TransformMatrix[D3DTS_END] = {};
	_float4x4			m_TransformMatrix_Inverse[D3DTS_END] = {};
	_float4				m_vCamPosition = {};

public:
	static CPipeLine* Create();
	virtual void Free() override;
};

END