#include "..\Public\Model.h"

#include "Bone.h"
#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"
#include "Animation.h"
#include "VIBuffer_Skeletal.h"
#include "GameInstance.h"
#include "Channel.h"

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
	, m_pBuffer_Skeletal(rhs.m_pBuffer_Skeletal)
	, m_RootBoneIndex(rhs.m_RootBoneIndex)
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

	Safe_AddRef(m_pBuffer_Skeletal);
	Ready_Bone_Matrices();
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

HRESULT CModel::Initialize_Prototype(TYPE eModelType, const _char * pModelFilePath, _fmatrix PivotMatrix, _uint _RootBoneIndex)
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

	if (eModelType != TYPE_NONANIM)
	{
		if (FAILED(Ready_Bones(m_pAIScene->mRootNode, -1)))
			return E_FAIL;

		m_RootBoneIndex = _RootBoneIndex;
	}

	if (eModelType != TYPE_ANIM_NONMESH)
	{
		/* 모델을 구성하는 정점과 인덱스를 생성한다. */
		/* 모델 = 메시 + 메시 + 메시 ... */
		if (FAILED(Ready_Meshes()))
			return E_FAIL;

		/* 텍스쳐를 로드해야한다. (머테리얼을 로드한다.) */
		if (FAILED(Ready_Materials(pModelFilePath)))
			return E_FAIL;
	}
	else
	{
		if (FAILED(Ready_Buffer_Skeletal()))
			return E_FAIL;
	}

	if (eModelType != TYPE_NONANIM)
	{
		if (FAILED(Ready_Animations(pModelFilePath)))
			return E_FAIL;
	}

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

HRESULT CModel::Render()
{
	//m_pBuffer_Skeletal->Bind_Buffers();
	m_pBuffer_Skeletal->Render();

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

HRESULT CModel::Update_BoneMatrices(CShader* pShader, const _char* pConstantName)
{
	//m_pBuffer_Skeletal->UpdateBoneMatrices(m_BoneMatrices);

	_float4x4		BoneMatrices[700];

	for (int i = 0; i < m_BoneMatrices.size(); ++i)
	{
		BoneMatrices[i] = *(m_BoneMatrices[i]);
	}

	return pShader->Bind_Matrices(pConstantName, BoneMatrices, 700);
}

void CModel::Set_Animation(_uint iAnimIndex)
{
	if (m_iCurrentAnimation != iAnimIndex)
	{
		for (auto& iter : m_Bones)
			iter->Save_PreTransformationMatrix();

		m_Animations[m_iCurrentAnimation]->Reset_TrackPosition(); // 초기화

		m_iCurrentAnimation = iAnimIndex;

		m_isBlending = true;
		m_fBlendingTime = 0.f; 
		Reset_Root_Transform();
	}
}

void CModel::Play_Animation(_float fTimeDelta, _bool isLoop)
{
	m_PreRootTransform = m_RootTransform;
	/* 현재 애니메이션 상태에 맞게 뼈들의 TransformationMatrix행렬 갱신 */

	if (!(m_Animations[m_iCurrentAnimation]->Invalidate_TransformationMatrix(fTimeDelta, m_Bones, isLoop, m_RootTransform)))
		m_PreRootTransform = { 0.f, 0.f, 0.f };

	_float fRatio = 1.f;

	if (m_isBlending)
	{
		m_fBlendingTime += fTimeDelta;
		
		if (m_fBlendingTime >= m_fEndBlendingTime)
		{
			m_fBlendingTime = 0.f;
			m_isBlending = false;
		}
		else
			fRatio = m_fBlendingTime / m_fEndBlendingTime;

	}
	/* 모든 뼈들의 CombinedTransformationMatrix 연산 */
	for (auto& pBone : m_Bones)
		pBone->Invalidate_CombinedTransformationMatrix(m_Bones, m_isBlending, fRatio);
}

_float3 CModel::Get_Root_Transform()
{
	_float3 result;
	XMStoreFloat3(&result, XMLoadFloat3(&m_RootTransform) - XMLoadFloat3(&m_PreRootTransform));
	return result;
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

HRESULT CModel::Ready_Buffer_Skeletal()
{
	m_pBuffer_Skeletal = CVIBuffer_Skeletal::Create(m_pDevice, m_pContext, m_Bones);
	
	if (nullptr == m_pBuffer_Skeletal)
		return E_FAIL;

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

		m_Materials
			.push_back(MaterialDesc);
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

	_int iNumChildren = pNode->mNumChildren;

	if (iNumChildren == 0)
		pBone->Set_IsLeaf(true);

	for (size_t i = 0; i < iNumChildren; i++)
	{
		Ready_Bones(pNode->mChildren[i], iParentIndex);
	}

	return S_OK;
}

HRESULT CModel::Ready_Bone_Matrices()
{
	m_BoneMatrices.resize(m_Bones.size());

	for (_int i = 0; i < m_Bones.size(); ++i)
	{
		m_BoneMatrices[i] = m_Bones[i]->Get_CombindTransformationMatrixPtr();
	}

	return S_OK;
}

HRESULT CModel::Ready_Animations(const _char* pModelFilePath)
{
	m_iNumAnimations = m_pAIScene->mNumAnimations;

	for (size_t i = 0; i < m_iNumAnimations; i++)
	{
		CAnimation*		pAnimation = CAnimation::Create(m_pAIScene->mAnimations[i], this);
		if (nullptr == pAnimation)
			return E_FAIL;

		m_Animations.push_back(pAnimation);
	}

	//Make_Anim_Json(pModelFilePath);

	return S_OK;
}

void CModel::Make_Anim_Json(const _char* pModelFilePath)
{
	Json::Value root;

	for (size_t i = 0; i < m_iNumAnimations; ++i)
	{
		Json::Value Animation;

		_char m_szName[MAX_PATH] = "";
		strcpy_s(m_szName, m_pAIScene->mAnimations[i]->mName.data);

		/*Json Data*/
		Animation["AnimName"] = m_szName;
		Animation["Speed"] = 1.f;
		
		string index = to_string(i);
		root[index] = Animation;
	}

	const char* pLastSlash = strrchr(pModelFilePath, '/');
	if (!pLastSlash) 
		pLastSlash = strrchr(pModelFilePath, '\\');  // Windows 경로 지원
	

	// 슬래시가 없으면 전체 문자열이 파일 이름이므로 그대로 설정
	const char* fileName = (pLastSlash) ? (pLastSlash + 1) : pModelFilePath;

	// 확장자 위치 찾기
	const char* pExtension = strrchr(fileName, '.');

	string tempName;
	if (pExtension) 
		tempName.assign(fileName, pExtension - fileName);  // 확장자 제거
	else 
		tempName = fileName;  // 확장자가 없으면 전체 파일명 그대로 사용

	// std::string을 std::wstring으로 변환하여 strObjName에 저장
	wstring strObjName = std::wstring(tempName.begin(), tempName.end());
	wstring strCompletePath = m_strAnimSavePath + strObjName + TEXT(".json");

	CGameInstance::GetInstance()->WriteJson(root, strCompletePath);
}

CModel * CModel::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, TYPE eModelType, const _char * pModelFilePath, _fmatrix PivotMatrix, _uint _RootBoneIndex)
{
	CModel*		pInstance = new CModel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eModelType, pModelFilePath, PivotMatrix, _RootBoneIndex)))
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

	Safe_Release(m_pBuffer_Skeletal);

	m_Importer.FreeScene();
}
