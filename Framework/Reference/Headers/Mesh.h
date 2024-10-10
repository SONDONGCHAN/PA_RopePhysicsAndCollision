#pragma once

#include "VIBuffer.h"
#include "Model.h"

/* CMesh : 모델을 구성하는 하나의 메시를 위한 정점과 인덱스를 보관한다. */
BEGIN(Engine)

class CMesh final : public CVIBuffer
{
private:
	CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMesh(const CMesh& rhs);
	virtual ~CMesh() = default;

public:
	_uint	Get_MaterialIndex() const {
		return m_iMaterialIndex;
	}


	
	_uint Get_NumBones() const {
		return m_iNumBones;
	}

public:
	virtual HRESULT Initialize_Prototype(CModel::TYPE eType, const aiMesh* pAIMesh, CModel* pModel, _fmatrix PivotMatrix);
	virtual HRESULT Initialize(void* pArg) override;
public:
	void SetUp_BoneMatrices(_float4x4* pBoneMatrices, vector<class CBone*>& Bones);


private:
	_char				m_szName[MAX_PATH] = "";
	_uint				m_iMaterialIndex = { 0 };

	/* 메시별로 이용하고 있는 뼈들. */
	_uint				m_iNumBones = { 0 };
	vector<_uint>		m_BoneIndices;
	vector<_float4x4>	m_OffsetMatrices;
public:
	HRESULT Ready_Vertex_Buffer_NonAnim(const aiMesh* pAIMesh, _fmatrix PivotMatrix);
	HRESULT Ready_Vertex_Buffer_Anim(const aiMesh* pAIMesh, CModel* pModel);

public:
	static CMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel::TYPE eType, const aiMesh* pAIMesh, CModel* pModel, _fmatrix PivotMatrix);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END