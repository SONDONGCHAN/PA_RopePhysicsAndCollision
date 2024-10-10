#include "..\Public\Bone.h"

CBone::CBone()
{
}

HRESULT CBone::Initialize(const aiNode * pAIBone, _int iParentBoneIndex, _fmatrix PivotMatrix)
{
	strcpy_s(m_szName, pAIBone->mName.data);
	
	memcpy(&m_TransformationMatrix, &pAIBone->mTransformation, sizeof(_float4x4));	

	if(-1 != iParentBoneIndex)
		XMStoreFloat4x4(&m_TransformationMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_TransformationMatrix)));
	else
		XMStoreFloat4x4(&m_TransformationMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_TransformationMatrix)) * PivotMatrix);



	XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMMatrixIdentity());

	m_iParentBoneIndex = iParentBoneIndex;

	return S_OK;
}

void CBone::Invalidate_CombinedTransformationMatrix(const vector<CBone*>& Bones)
{
	if (-1 == m_iParentBoneIndex)
		m_CombinedTransformationMatrix = m_TransformationMatrix;
	else
		XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMLoadFloat4x4(&m_TransformationMatrix) * XMLoadFloat4x4(&Bones[m_iParentBoneIndex]->m_CombinedTransformationMatrix));
}

CBone * CBone::Create(const aiNode * pAIBone, _int iParentBoneIndex, _fmatrix PivotMatrix)
{
	CBone*		pInstance = new CBone();

	if (FAILED(pInstance->Initialize(pAIBone, iParentBoneIndex, PivotMatrix)))
	{
		MSG_BOX("Failed to Created : CBone");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CBone * CBone::Clone()
{
	return new CBone(*this);	
}

void CBone::Free()
{
	


}
