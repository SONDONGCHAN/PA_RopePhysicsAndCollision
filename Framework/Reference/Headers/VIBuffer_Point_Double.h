#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Point_Double final : public CVIBuffer
{
private:
	CVIBuffer_Point_Double(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Point_Double(const CVIBuffer_Point_Double& rhs);
	virtual ~CVIBuffer_Point_Double() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;

public:
	static CVIBuffer_Point_Double* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END