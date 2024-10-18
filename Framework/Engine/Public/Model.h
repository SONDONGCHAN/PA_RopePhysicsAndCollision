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

public:
	HRESULT Initialize_Prototype(TYPE eModelType, const _char* pModelFilePath, _fmatrix PivotMatrix, _uint _RootBoneIndex);
	HRESULT Initialize(void* pArg);
	HRESULT Render(_uint iMeshIndex);
	HRESULT Render();


public:
	HRESULT Bind_Material_ShaderResource(class CShader* pShader, _uint iMeshIndex, aiTextureType eMaterialType, const _char* pConstantName);
	HRESULT Bind_BoneMatrices(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex);
	HRESULT Update_BoneMatrices(CShader* pShader, const _char* pConstantName);
	void Set_Animation(_uint iAnimIndex);
	void Play_Animation(_float fTimeDelta, _bool isLoop = false);
	_float3 Get_Root_Transform();
	void Reset_Root_Transform() { m_RootTransform = { 0.f, 0.f, 0.f }; }


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
	_float	m_fBlendingTime		= { 0.f };
	_float	m_fEndBlendingTime	= { 0.1f };
	_bool	m_isBlending		= { false };
	_uint	m_RootBoneIndex		= { 0 };

	/*애니메이션 Json Data Parsing*/
	wstring m_strAnimSavePath = TEXT("..\\..\\Client\\Bin\\DataFiles\\AnimData\\");
	_float3 m_RootTransform = {0.f, 0.f, 0.f};
	_float3 m_PreRootTransform = { 0.f, 0.f, 0.f };


private:
	HRESULT Ready_Meshes();
	HRESULT Ready_Buffer_Skeletal();
	HRESULT Ready_Materials(const _char* pModelFilePath);
	HRESULT Ready_Bones(aiNode * pNode, _int iParentBoneIndex);
	HRESULT Ready_Bone_Matrices();
	HRESULT Ready_Animations(const _char* pModelFilePath);
	void	Make_Anim_Json(const _char* pModelFilePath);

public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eModelType, const _char* pModelFilePath, _fmatrix PivotMatrix = XMMatrixIdentity(), _uint _RootBoneIndex = 0);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

};

END