#pragma once

#include "Base.h"

BEGIN(Engine)

class CChannel final : public CBase
{
public:
	typedef struct
	{
		_float3		vScale, vPosition;
		_float4		vRotation;
		_double		Time;
	}KEYFRAME;
		
private:
	CChannel();
	virtual ~CChannel() = default;

public:
	HRESULT Initialize(const aiNodeAnim* pAIChannel, class CModel* pModel);
	void Invalidate_TransformationMatrix(_double TrackPosition, _uint* pCurrentKeyFrame, const vector<class CBone*>& Bones);

private:
	_char				m_szName[MAX_PATH] = "";
	_uint				m_iNumKeyFrames = { 0 };
	vector<KEYFRAME>	m_KeyFrames;

	
	_uint				m_iBoneIndex = { 0 };

public:
	static CChannel* Create(const aiNodeAnim* pAIChannel, class CModel* pModel);
	virtual void Free() override;
};

END