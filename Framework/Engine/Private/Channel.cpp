#include "..\Public\Channel.h"
#include "Model.h"
#include "Bone.h"

CChannel::CChannel()
{
}

HRESULT CChannel::Initialize(const aiNodeAnim * pAIChannel, class CModel* pModel)
{
	strcpy_s(m_szName, pAIChannel->mNodeName.data);

	m_iBoneIndex = pModel->Get_BoneIndex(m_szName);

	m_iNumKeyFrames = max(pAIChannel->mNumScalingKeys, pAIChannel->mNumRotationKeys);
	m_iNumKeyFrames = max(m_iNumKeyFrames, pAIChannel->mNumPositionKeys);

	_float3			vScale;
	_float4			vRotation;
	_float3			vPosition;

	for (size_t i = 0; i < m_iNumKeyFrames; i++)
	{
		KEYFRAME		KeyFrame = {};

		if(pAIChannel->mNumScalingKeys > i)
		{
			memcpy(&vScale, &pAIChannel->mScalingKeys[i].mValue, sizeof(_float3));
			KeyFrame.Time = pAIChannel->mScalingKeys[i].mTime;
		}

		if (pAIChannel->mNumRotationKeys > i)
		{
			vRotation.x = pAIChannel->mRotationKeys[i].mValue.x;
			vRotation.y = pAIChannel->mRotationKeys[i].mValue.y;
			vRotation.z = pAIChannel->mRotationKeys[i].mValue.z;
			vRotation.w = pAIChannel->mRotationKeys[i].mValue.w;
			KeyFrame.Time = pAIChannel->mRotationKeys[i].mTime;
		}

		if (pAIChannel->mNumPositionKeys > i)
		{
			memcpy(&vPosition, &pAIChannel->mPositionKeys[i].mValue, sizeof(_float3));
			KeyFrame.Time = pAIChannel->mPositionKeys[i].mTime;
		}

		KeyFrame.vScale = vScale;
		KeyFrame.vRotation = vRotation;
		KeyFrame.vPosition = vPosition;

		m_KeyFrames.push_back(KeyFrame);
	}	
	
	return S_OK;
}

void CChannel::Invalidate_TransformationMatrix(_double TrackPosition, _uint* pCurrentKeyFrame, const vector<class CBone*>& Bones, _float3& RootTransform)
{
	if (0.0 == TrackPosition)
		*pCurrentKeyFrame = 0;

	KEYFRAME		LastKeyFrame = m_KeyFrames.back();

	_matrix			TransformationMatrix;

	_vector			vScale, vRotation, vTranslation;

	if (TrackPosition >= LastKeyFrame.Time)
	{
		vScale = XMLoadFloat3(&LastKeyFrame.vScale);
		vRotation = XMLoadFloat4(&LastKeyFrame.vRotation);
		vTranslation = XMVectorSetW(XMLoadFloat3(&LastKeyFrame.vPosition), 1.f);		
	}

	else
	{
		while (TrackPosition >= m_KeyFrames[*pCurrentKeyFrame + 1].Time)
			++*pCurrentKeyFrame;

		_double		Ratio = (TrackPosition - m_KeyFrames[*pCurrentKeyFrame].Time) /
			(m_KeyFrames[*pCurrentKeyFrame + 1].Time - m_KeyFrames[*pCurrentKeyFrame].Time);			

		vScale = XMVectorLerp(XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrame].vScale), XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrame + 1].vScale), Ratio);
		vRotation = XMQuaternionSlerp(XMLoadFloat4(&m_KeyFrames[*pCurrentKeyFrame].vRotation), XMLoadFloat4(&m_KeyFrames[*pCurrentKeyFrame + 1].vRotation), Ratio);
		vTranslation = XMVectorSetW(XMVectorLerp(XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrame].vPosition), XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrame + 1].vPosition), Ratio), 1.f);
	}

	TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vTranslation);

	Bones[m_iBoneIndex]->Set_TranslationMatrix(TransformationMatrix, RootTransform);
}

CChannel * CChannel::Create(const aiNodeAnim * pAIChannel, class CModel* pModel)
{
	CChannel*		pInstance = new CChannel();

	if (FAILED(pInstance->Initialize(pAIChannel, pModel)))
	{
		MSG_BOX("Failed to Created : CChannel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CChannel::Free()
{
}

