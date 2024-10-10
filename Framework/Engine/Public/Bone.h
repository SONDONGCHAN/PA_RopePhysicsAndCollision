#pragma once

#include "Base.h"

BEGIN(Engine)

class CBone final : public CBase
{
private:
	CBone();
	virtual ~CBone() = default;
	
public:
	const _char* Get_BoneName() const {
		return m_szName;
	}
	
	_float4x4 Get_CombindTransformationMatrix() const {
		return m_CombinedTransformationMatrix;
	}

	_float4x4* Get_CombindTransformationMatrixPtr() {
		return &m_CombinedTransformationMatrix;
	}



	void Set_TranslationMatrix(_fmatrix TranslationMatrix) {
		XMStoreFloat4x4(&m_TransformationMatrix, TranslationMatrix);
	}

public:
	HRESULT Initialize(const aiNode* pAIBone, _int iParentBoneIndex, _fmatrix PivotMatrix);
	void Invalidate_CombinedTransformationMatrix(const vector<CBone*>& Bones);

private:
	_char			m_szName[MAX_PATH] = "";
	_float4x4		m_TransformationMatrix; /* �θ� �������� ǥ���� ������ ���� ���. */
	_float4x4		m_CombinedTransformationMatrix; /* ���� TransformationMatrix * �θ�`s m_CombinedTransformationMatrix*/

	_int			m_iParentBoneIndex = { 0 };

public:
	static CBone* Create(const aiNode* pAIBone, _int iParentBoneIndex, _fmatrix PivotMatrix);
	CBone* Clone();
	virtual void Free() override;
};

END