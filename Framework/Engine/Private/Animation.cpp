#include "..\Public\Animation.h"
#include "Channel.h"

CAnimation::CAnimation()
{
}

CAnimation::CAnimation(const CAnimation & rhs)
	: m_iNumAnimations(rhs.m_iNumAnimations)
	, m_Duration(rhs.m_Duration)
	, m_TickPerSecond(rhs.m_TickPerSecond)
	, m_TrackPosition(rhs.m_TrackPosition)
	, m_iNumChannels(rhs.m_iNumChannels)
	, m_Channels(rhs.m_Channels)
	, m_iCurrentKeyFrames(rhs.m_iCurrentKeyFrames)
	, m_isFinished(rhs.m_isFinished)
{
	for (auto pChannel : m_Channels)	
		Safe_AddRef(pChannel);	

	strcpy_s(m_szName, rhs.m_szName);
}

HRESULT CAnimation::Initialize(const aiAnimation * pAIAnimation, class CModel* pModel)
{
	strcpy_s(m_szName, pAIAnimation->mName.data);

	/* �� �ִϸ��̼��� ��ü ����. */
	m_Duration = pAIAnimation->mDuration;

	/* �ʴ� ��� �ӵ� .*/
	m_TickPerSecond = pAIAnimation->mTicksPerSecond;	

	/* �� �ִϸ��̼��� �����ϴµ� �ʿ��� ���� ����. */
	m_iNumChannels = pAIAnimation->mNumChannels;

	m_iCurrentKeyFrames.resize(m_iNumChannels);
	
	/* �� �ִϸ��̼��� �����ϴµ� �ʿ��� ������ ������ �����Ѵ�.*/
	/* ������ ����? : */
	for (size_t i = 0; i < m_iNumChannels; i++)
	{
		CChannel*	pChannel = CChannel::Create(pAIAnimation->mChannels[i], pModel);
		if (nullptr == pChannel)
			return E_FAIL;

		m_Channels.push_back(pChannel);		
	}	

	return S_OK;
}

void CAnimation::Invalidate_TransformationMatrix(_float fTimeDelta, const vector<class CBone*>& Bones, _bool isLoop)
{
	m_TrackPosition += m_TickPerSecond * fTimeDelta;

	if (m_TrackPosition >= m_Duration)
	{
		if (false == isLoop)
		{
			m_isFinished = true;

			for (size_t i = 0; i < m_iNumChannels; i++)
				m_Channels[i]->Invalidate_TransformationMatrix(m_Duration, &m_iCurrentKeyFrames[i], Bones);

			return;
		}
		else
		{
			Reset_TrackPosition();
		}
	}

	for (size_t i = 0; i < m_iNumChannels; i++)
	{
		m_Channels[i]->Invalidate_TransformationMatrix(m_TrackPosition, &m_iCurrentKeyFrames[i], Bones);
	}
}

void CAnimation::Reset_TrackPosition()
{
	m_TrackPosition = 0.f;
	for (auto iter : m_iCurrentKeyFrames)
		iter = 0;
}

CAnimation * CAnimation::Create(const aiAnimation * pAIAnimation, class CModel* pModel)
{
	CAnimation*		pInstance = new CAnimation();

	if (FAILED(pInstance->Initialize(pAIAnimation, pModel)))
	{
		MSG_BOX("Failed to Created : CAnimation");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CAnimation * CAnimation::Clone()
{
	return new CAnimation(*this);	
}

void CAnimation::Free()
{
	for (auto& pChannel : m_Channels)
		Safe_Release(pChannel);	

	m_Channels.clear();
}

