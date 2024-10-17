#include "..\Public\Bone.h"

CBone::CBone()
{
}

void CBone::Set_TranslationMatrix(_fmatrix TranslationMatrix, _float3& RootTransform)
{
	XMStoreFloat4x4(&m_TransformationMatrix, TranslationMatrix);

	if (0 == m_iParentBoneIndex)
	{
		_matrix			PivotMatrix = XMMatrixIdentity();
		PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

		_float4x4 Mat;
		XMStoreFloat4x4(&Mat, XMLoadFloat4x4(&m_TransformationMatrix) * PivotMatrix);
		
		RootTransform = { Mat._41, Mat._42,Mat._43 };

		m_TransformationMatrix._41 = 0.f;
		m_TransformationMatrix._42 = 0.f;
		m_TransformationMatrix._43 = 0.f;
	}
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

void CBone::Invalidate_CombinedTransformationMatrix(const vector<CBone*>& Bones, _bool _isBlending, _float _fRatio)
{
	if (_isBlending)
	{
		XMMATRIX Mat1 = XMLoadFloat4x4(&m_PreTransformationMatrix);
		XMMATRIX Mat2 = XMLoadFloat4x4(&m_TransformationMatrix);

		XMVECTOR scale1, rotation1, translation1;
		XMVECTOR scale2, rotation2, translation2;

		XMMatrixDecompose(&scale1, &rotation1, &translation1, Mat1);
		XMMatrixDecompose(&scale2, &rotation2, &translation2, Mat2);

		XMVECTOR scale = XMVectorLerp(scale1, scale2, _fRatio);
		XMVECTOR rotation = XMQuaternionSlerp(rotation1, rotation2, _fRatio);
		XMVECTOR translation = XMVectorLerp(translation1, translation2, _fRatio);

		XMStoreFloat4x4(&m_TransformationMatrix, XMMatrixAffineTransformation(scale, XMVectorZero(), rotation, translation));
	}


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
