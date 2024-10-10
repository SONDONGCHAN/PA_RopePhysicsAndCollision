#pragma once

#include "Engine_Defines.h"

BEGIN(Engine)

class ENGINE_DLL CBase abstract
{
protected:
	CBase();
	virtual ~CBase() = default;

public:
	/* ���۷��� ī��Ʈ�� ������Ų��. */
	/* ������Ų ������ ���۷��� ī��Ʈ�� ��ȯ�Ѵ�.*/
	_uint AddRef();

	/* ���۷��� ī��Ʈ�� ���ҽ�Ű�ų� �����Ѵ�.*/
	/* ���ҽ�Ű�� ������ ���۷��� ī��Ʈ�� ��ȯ�Ѵ�. */
	_uint Release();

private:
	_uint			m_iRefCnt = { 0 };

public:
	virtual void Free() = 0;
};

END