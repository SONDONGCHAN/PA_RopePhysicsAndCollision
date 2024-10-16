#pragma once

/* 여러개의 빛을 저장한다. */
#include "Base.h"

BEGIN(Engine)

class CJson_Manager final : public CBase
{
private:
	CJson_Manager();
	virtual ~CJson_Manager() = default;

public:
	void WriteJson(Json::Value _value, const wstring& _strSavePath);
	Json::Value ReadJson(const wstring& _strReadPath);

public:
	static CJson_Manager* Create();
	virtual void Free() override;
};

END