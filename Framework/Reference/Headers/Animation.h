#pragma once

#include "Base.h"

BEGIN(Engine)

class CAnimation final : public CBase
{
private:
	CAnimation();
	CAnimation(const CAnimation& rhs);
	virtual ~CAnimation() = default;

public:
	HRESULT Initialize(const aiAnimation* pAIAnimation, class CModel* pModel);
	_bool Invalidate_TransformationMatrix(_float fTimeDelta, const vector<class CBone*>& Bones, _bool isLoop, _float3& RootTransform);

public:
	void Reset_TrackPosition();

private:
	_uint					m_iNumAnimations = { 0 };
	_char					m_szName[MAX_PATH] = "";

	
	_double					m_Duration = { 0.0 };
	_double					m_TickPerSecond = { 0.0 };
	_double					m_TrackPosition = { 0.0 }; /* ���� �� �ִϸ��̼��� ��� ��ġ */

	_uint					m_iNumChannels = { 0 };
	vector<class CChannel*>	m_Channels = {};
	vector<_uint>			m_iCurrentKeyFrames;

	_bool					m_isFinished = { false };

public:
	static CAnimation* Create(const aiAnimation* pAIAnimation, class CModel* pModel);
	CAnimation* Clone();
	virtual void Free() override;
};

END