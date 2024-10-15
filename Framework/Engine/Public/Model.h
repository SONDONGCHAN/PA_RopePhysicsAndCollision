#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CModel final : public CComponent
{
public:
	enum TYPE { TYPE_NONANIM, TYPE_ANIM, TYPE_ANIM_NONMESH, TYPE_END };
private:
	CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CModel(const CModel& rhs);
	virtual ~CModel() = default;

public:
	_uint Get_NumMeshes() const {
		return m_iNumMeshes;
	}

	_int Get_BoneIndex(const _char* pBoneName) const;

	_float4x4* Get_CombinedBoneMatrixPtr(const _char* pBoneName);

	void Set_Animation(_uint iAnimIndex) {
		m_iCurrentAnimation = iAnimIndex;
	}

public:
	HRESULT Initialize_Prototype(TYPE eModelType, const _char* pModelFilePath, _fmatrix PivotMatrix);
	HRESULT Initialize(void* pArg);
	HRESULT Render(_uint iMeshIndex);
	HRESULT Render();


public:
	HRESULT Bind_Material_ShaderResource(class CShader* pShader, _uint iMeshIndex, aiTextureType eMaterialType, const _char* pConstantName);
	HRESULT Bind_BoneMatrices(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex);
	HRESULT Update_BoneMatrices(CShader* pShader, const _char* pConstantName);
	void Play_Animation(_float fTimeDelta, _bool isLoop = false);

private:
	const aiScene*			m_pAIScene = {};
	Assimp::Importer		m_Importer;

private:
	_float4x4				m_PivotMatrix;

private:
	_uint					m_iNumMeshes = { 0 };
	vector<class CMesh*>	m_Meshes;

	_uint						m_iNumMaterials = { 0 };
	vector<MESH_MATERIAL_DESC>	m_Materials;

	/* 내 모델의 모든 뼈를 다 로드하여 컨테이너에 저장한다.  */
	vector<class CBone*>		m_Bones;
	TYPE						m_eModelType;
	vector<XMFLOAT4X4*>			m_BoneMatrices;

	_uint						m_iNumAnimations = { 0 };
	_uint						m_iCurrentAnimation = { 0 };
	vector<class CAnimation*>	m_Animations;

	class CVIBuffer_Skeletal*	m_pBuffer_Skeletal { nullptr };

private:
	HRESULT Ready_Meshes();
	HRESULT Ready_Buffer_Skeletal();
	HRESULT Ready_Materials(const _char* pModelFilePath);
	HRESULT Ready_Bones(aiNode * pNode, _int iParentBoneIndex);
	HRESULT Ready_Bone_Matrices();
	HRESULT Ready_Animations();

public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eModelType, const _char* pModelFilePath, _fmatrix PivotMatrix = XMMatrixIdentity());
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

};

END