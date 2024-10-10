#include "..\Public\Model.h"

#include "Bone.h"
#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"
#include "Animation.h"

CModel::CModel(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)
{

}

CModel::CModel(const CModel & rhs)
	: CComponent(rhs)
	, m_PivotMatrix(rhs.m_PivotMatrix)
	, m_iNumMeshes(rhs.m_iNumMeshes)
	, m_Meshes(rhs.m_Meshes)
	, m_iNumMaterials(rhs.m_iNumMaterials)
	, m_Materials(rhs.m_Materials)	
	, m_iNumAnimations(rhs.m_iNumAnimations)	
{
	for (auto& pPrototypeAnimation : rhs.m_Animations)
		m_Animations.push_back(pPrototypeAnimation->Clone());

	for (auto& pPrototypeBone : rhs.m_Bones)
		m_Bones.push_back(pPrototypeBone->Clone());	

	for(auto& Material : m_Materials)
	{
		for (auto& pTexture : Material.pMtrlTextures)
			Safe_AddRef(pTexture);		
	}

	for (auto& pMesh : m_Meshes)
		Safe_AddRef(pMesh);
}

_int CModel::Get_BoneIndex(const _char * pBoneName) const
{

	_int		iBoneIndex = { -1 };

	auto	iter = find_if(m_Bones.begin(), m_Bones.end(), [&](class CBone* pBone) {

		iBoneIndex++;

		if (false == strcmp(pBoneName, pBone->Get_BoneName()))
			return true;
		return false;
	});

	if (iter == m_Bones.end())
		return -1;
		
	return iBoneIndex;
}
_float4x4 * CModel::Get_CombinedBoneMatrixPtr(const _char * pBoneName)
{
	auto	iter = find_if(m_Bones.begin(), m_Bones.end(), [&](CBone* pBone) 
	{
		if (false == strcmp(pBone->Get_BoneName(), pBoneName))
			return true;

		return false;
	});

	return (*iter)->Get_CombindTransformationMatrixPtr();
}

HRESULT CModel::Initialize_Prototype(TYPE eModelType, const _char * pModelFilePath, _fmatrix PivotMatrix)
{
	m_eModelType = eModelType;

	XMStoreFloat4x4(&m_PivotMatrix, PivotMatrix);

	_uint			iFlag = aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;

	if (TYPE_NONANIM == eModelType)
		iFlag |= aiProcess_PreTransformVertices;	

	/* .fbx파일의 정보를 읽어서 aiScene안에 모두 저장해준다. */
	m_pAIScene = m_Importer.ReadFile(pModelFilePath, iFlag);
	if (nullptr == m_pAIScene)
		return E_FAIL;

	/* aiScene안에있는 정보들을 내가 사용하기 좋도록 정리하면된다.  */

	if (FAILED(Ready_Bones(m_pAIScene->mRootNode, -1)))
		return E_FAIL;

	/* 모델을 구성하는 정점과 인덱스를 생성한다. */
	/* 모델 = 메시 + 메시 + 메시 ... */
	if (FAILED(Ready_Meshes()))
		return E_FAIL;

	/* 텍스쳐를 로드해야한다. (머테리얼을 로드한다.) */
	if (FAILED(Ready_Materials(pModelFilePath)))
		return E_FAIL;

	if (FAILED(Ready_Animations()))
		return E_FAIL;

	return S_OK;
}

HRESULT CModel::Initialize(void * pArg)
{
	return S_OK;
}

HRESULT CModel::Render(_uint iMeshIndex)
{		
	m_Meshes[iMeshIndex]->Bind_Buffers();
	m_Meshes[iMeshIndex]->Render();

	return S_OK;
}

HRESULT CModel::Bind_Material_ShaderResource(CShader * pShader, _uint iMeshIndex, aiTextureType eMaterialType, const _char* pConstantName)
{
	_uint		iMaterialIndex = m_Meshes[iMeshIndex]->Get_MaterialIndex();

	return m_Materials[iMaterialIndex].pMtrlTextures[eMaterialType]->Bind_ShaderResource(pShader, pConstantName, 0);	
}

HRESULT CModel::Bind_BoneMatrices(CShader * pShader, const _char * pConstantName, _uint iMeshIndex)
{
	_float4x4		BoneMatrices[512];

	m_Meshes[iMeshIndex]->SetUp_BoneMatrices(BoneMatrices, m_Bones);

	return pShader->Bind_Matrices(pConstantName, BoneMatrices, 512);
}

void CModel::Play_Animation(_float fTimeDelta, _bool isLoop)
{
	/* 현재 애니메이션 상태에 맞게 뼈들의 TransformationMatrix행렬을 갱신한다. */
	m_Animations[m_iCurrentAnimation]->Invalidate_TransformationMatrix(fTimeDelta, m_Bones, isLoop);


	/* 모든 뼈들의 CombinedTransformationMatrix를 구해준다. */
	for (auto& pBone : m_Bones)	
		pBone->Invalidate_CombinedTransformationMatrix(m_Bones);
	
}

HRESULT CModel::Ready_Meshes()
{
	m_iNumMeshes = m_pAIScene->mNumMeshes;

	for (size_t i = 0; i < m_iNumMeshes; i++)
	{
		CMesh*		pMesh = CMesh::Create(m_pDevice, m_pContext, m_eModelType, m_pAIScene->mMeshes[i], this, XMLoadFloat4x4(&m_PivotMatrix));
		if (nullptr == pMesh)
			return E_FAIL;

		m_Meshes.push_back(pMesh);
	}

	return S_OK;
}

HRESULT CModel::Ready_Materials(const _char* pModelFilePath)
{
	_char			szDrive[MAX_PATH] = "";
	_char			szDirectory[MAX_PATH] = "";

	_splitpath_s(pModelFilePath, szDrive, MAX_PATH, szDirectory, MAX_PATH, nullptr, 0, nullptr, 0);

	m_iNumMaterials = m_pAIScene->mNumMaterials;

	for (size_t i = 0; i < m_iNumMaterials; i++)
	{
		MESH_MATERIAL_DESC			MaterialDesc = {};

		aiMaterial*	pAIMaterial = m_pAIScene->mMaterials[i];

		for (size_t j = 0; j < AI_TEXTURE_TYPE_MAX; j++)
		{
			// pAIMaterial->GetTextureCount(aiTextureType(j));
			aiString			strTextureFilePath;			

			if (FAILED(pAIMaterial->GetTexture(aiTextureType(j), 0, &strTextureFilePath)))
				continue;

			_char				szFileName[MAX_PATH] = "";
			_char				szExt[MAX_PATH] = "";

			_splitpath_s(strTextureFilePath.data, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);

			_char				szTmp[MAX_PATH] = "";
			strcpy_s(szTmp, szDrive);
			strcat_s(szTmp, szDirectory);
			strcat_s(szTmp, szFileName);
			strcat_s(szTmp, szExt);

			_tchar				szFullPath[MAX_PATH] = TEXT("");
			MultiByteToWideChar(CP_ACP, 0, szTmp, strlen(szTmp), szFullPath, MAX_PATH);

			MaterialDesc.pMtrlTextures[j] = CTexture::Create(m_pDevice, m_pContext, szFullPath, 1);		
		}

		m_Materials.push_back(MaterialDesc);
	}

	return S_OK;
}

HRESULT CModel::Ready_Bones(aiNode* pNode, _int iParentBoneIndex)
{
	CBone* pBone = CBone::Create(pNode, iParentBoneIndex, XMLoadFloat4x4(&m_PivotMatrix));
	if (nullptr == pBone)
		return E_FAIL;

	m_Bones.push_back(pBone);	

	_uint	iParentIndex = m_Bones.size() - 1;

	for (size_t i = 0; i < pNode->mNumChildren; i++)
	{
		Ready_Bones(pNode->mChildren[i], iParentIndex);
	}

	return S_OK;
}

HRESULT CModel::Ready_Animations()
{
	m_iNumAnimations = m_pAIScene->mNumAnimations;

	for (size_t i = 0; i < m_iNumAnimations; i++)
	{
		CAnimation*		pAnimation = CAnimation::Create(m_pAIScene->mAnimations[i], this);
		if (nullptr == pAnimation)
			return E_FAIL;

		m_Animations.push_back(pAnimation);
	}

	return S_OK;
}

CModel * CModel::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, TYPE eModelType, const _char * pModelFilePath, _fmatrix PivotMatrix)
{
	CModel*		pInstance = new CModel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eModelType, pModelFilePath, PivotMatrix)))
	{
		MSG_BOX("Failed to Created : CModel");
		Safe_Release(pInstance);
	}

	return pInstance;
}


CComponent * CModel::Clone(void * pArg)
{
	CModel*		pInstance = new CModel(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CModel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CModel::Free()
{
	__super::Free();

	for (auto& pAnimation : m_Animations)
		Safe_Release(pAnimation);

	m_Animations.clear();

	for (auto& pBone : m_Bones)
		Safe_Release(pBone);

	m_Bones.clear();

	for (auto& Material : m_Materials)
	{
		for (auto& pTexture : Material.pMtrlTextures)
			Safe_Release(pTexture);
	}

	for (auto& pMesh : m_Meshes)
		Safe_Release(pMesh);

	m_Importer.FreeScene();

}
