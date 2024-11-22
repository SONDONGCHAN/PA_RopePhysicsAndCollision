#include "..\Public\Transform.h"
#include "Shader.h"

#include "Navigation.h"

CTransform::CTransform(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)
{

}

void CTransform::Set_Scaling(_float fX, _float fY, _float fZ)
{
	Set_State(STATE_RIGHT, XMVector3Normalize(Get_State(STATE_RIGHT)) * fX);
	Set_State(STATE_UP, XMVector3Normalize(Get_State(STATE_UP)) * fY);
	Set_State(STATE_LOOK, XMVector3Normalize(Get_State(STATE_LOOK)) * fZ);
}

HRESULT CTransform::Initialize_Prototype()
{
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

	return S_OK;
}

HRESULT CTransform::Initialize(void * pArg)
{
	if (nullptr == pArg)
		return S_OK;

	TRANSFORM_DESC*		pTransformDesc = (TRANSFORM_DESC*)pArg;

	m_fSpeedPerSec = pTransformDesc->fSpeedPerSec;
	m_fRotationPerSec = pTransformDesc->fRotationPerSec;

	return S_OK;
}

HRESULT CTransform::Bind_ShaderResource(CShader* pShader, const _char * pContantName)
{
	return pShader->Bind_Matrix(pContantName, &m_WorldMatrix);	
}

void CTransform::Go_Straight(_float fTimeDelta, CNavigation* pNavigation)
{
	_vector		vPosition = Get_State(STATE_POSITION);
	_vector		vLook = Get_State(STATE_LOOK);
	
	vPosition += XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;

	if(nullptr == pNavigation || 
		true == pNavigation->isMove(vPosition))
		Set_State(STATE_POSITION, vPosition);		
}

void CTransform::Go_Backward(_float fTimeDelta)
{
	_vector		vPosition = Get_State(STATE_POSITION);
	_vector		vLook = Get_State(STATE_LOOK);

	vPosition -= XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Left(_float fTimeDelta)
{
	_vector		vPosition = Get_State(STATE_POSITION);
	_vector		vRight = Get_State(STATE_RIGHT);

	vPosition -= XMVector3Normalize(vRight) * m_fSpeedPerSec * fTimeDelta;

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Right(_float fTimeDelta)
{
	_vector		vPosition = Get_State(STATE_POSITION);
	_vector		vRight = Get_State(STATE_RIGHT);

	vPosition += XMVector3Normalize(vRight) * m_fSpeedPerSec * fTimeDelta;

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Dir(Direction _eDirection, _float fTimeDelta)
{
	if (_eDirection >= DIR_END)
		return;

	_vector		vPosition	= Get_State(STATE_POSITION);
	_vector		vUp			= Get_State(STATE_UP);
	_vector		vLook		= Get_State(STATE_LOOK);
	_vector		vRight		= Get_State(STATE_RIGHT);

	_vector		vDir {0.f, 0.f, 1.f, 0.f};
	
	switch (_eDirection)
	{
	case Direction::DIR_FRONT:
		vDir = vLook;
		break;

	case Direction::DIR_BACK:
		vDir = -vLook;
		break;

	case Direction::DIR_LEFT:
		vDir = -vRight;
		break;

	case Direction::DIR_RIGHT:
		vDir = vRight;
		break;

	case Direction::DIR_FRONT_LEFT:
		vDir = vLook - vRight;
		break;

	case Direction::DIR_FRONT_RIGHT:
		vDir = vLook + vRight;
		break;

	case Direction::DIR_BACK_LEFT:
		vDir = -vLook - vRight;
		break;

	case Direction::DIR_BACK_RIGHT:
		vDir = -vLook + vRight;
		break;


	case Direction::DIR_UP:
		vDir = vUp;
		break;

	case Direction::DIR_DOWN:
		vDir = -vUp;
		break;

	case Direction::DIR_UP_LEFT:
		vDir = vUp - vRight;
		break;

	case Direction::DIR_UP_RIGHT:
		vDir = vUp + vRight;
		break;

	case Direction::DIR_DOWN_LEFT:
		vDir = -vUp - vRight;
		break;

	case Direction::DIR_DOWN_RIGHT:
		vDir = -vUp + vRight;
		break;
	}
	vPosition += XMVector3Normalize(vDir) * m_fSpeedPerSec * fTimeDelta;

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Move(XMVECTOR _Movement, CNavigation* pNavigation)
{
	_vector		vPosition = Get_State(STATE_POSITION);

	_vector		vLook = Get_State(STATE_LOOK);
	vPosition += XMVector3Normalize(vLook) * XMVectorGetZ(_Movement) ;

	_vector		vRight = Get_State(STATE_RIGHT);
	vPosition += XMVector3Normalize(vRight) * XMVectorGetX(_Movement);

	if (nullptr == pNavigation ||
		true == pNavigation->isMove(vPosition))
		Set_State(STATE_POSITION, vPosition);
}

void CTransform::Move_Dir(_vector _Dir, _float _fScale)
{
	_vector		vPosition = Get_State(STATE_POSITION);
	vPosition += XMVectorScale(_Dir, _fScale);
	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Turn(_fvector vAxis, _float fTimeDelta)
{
	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, m_fRotationPerSec * fTimeDelta);

	for (size_t i = STATE_RIGHT; i < STATE_POSITION; i++)
	{
		_vector		vStateDir = Get_State(STATE(i));
		
		Set_State(STATE(i), XMVector4Transform(vStateDir, RotationMatrix));		
	}
}

void CTransform::Rotation(_fvector vAxis, _float fRadian)
{
	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, fRadian);
	_float3		vScaled = Get_Scaled();

	for (size_t i = STATE_RIGHT; i < STATE_POSITION; i++)
	{
		_float4		vTmp = _float4(0.f, 0.f, 0.f, 0.f);

		*((_float*)&vTmp + i) = 1.f * *((_float*)&vScaled + i);

		_vector		vStateDir = XMLoadFloat4(&vTmp);

		Set_State(STATE(i), XMVector4Transform(vStateDir, RotationMatrix));
	}
}

void CTransform::MoveTo(_fvector vPoint, _float fLimit, _float fTimeDelta)
{
	_vector		vPosition = Get_State(STATE_POSITION);
	_vector		vDir = vPoint - vPosition;

	_float		fDistance = XMVector3Length(vDir).m128_f32[0];

	if(fDistance >= fLimit)
		vPosition += XMVector3Normalize(vDir) * m_fSpeedPerSec * fTimeDelta;
}

void CTransform::LookAt(_fvector vPoint)
{
	_float3	vScaled = Get_Scaled();

	_vector	vPosition = Get_State(CTransform::STATE_POSITION);

	_vector	vLook = vPoint - vPosition;
	_vector vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
	_vector vUp = XMVector3Cross(vLook, vRight);

	Set_State(STATE_RIGHT, XMVector3Normalize(vRight) * vScaled.x);
	Set_State(STATE_UP, XMVector3Normalize(vUp) * vScaled.y);
	Set_State(STATE_LOOK, XMVector3Normalize(vLook) * vScaled.z);

}

void CTransform::LookAt_ForLandObject(_fvector vPoint)
{
	_float3	vScaled = Get_Scaled();

	_vector	vPosition = Get_State(CTransform::STATE_POSITION);

	_vector	vLook = vPoint - vPosition;
	_vector vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
	vLook = XMVector3Cross(vRight, XMVectorSet(0.f, 1.f, 0.f, 0.f));

	Set_State(STATE_RIGHT, XMVector3Normalize(vRight) * vScaled.x);	
	Set_State(STATE_LOOK, XMVector3Normalize(vLook) * vScaled.z);
}

void CTransform::Set_Look(_fvector direction)
{
	_float3	vScaled = Get_Scaled();

	_vector	vPosition = Get_State(CTransform::STATE_POSITION);

	_vector	vLook = direction;
	_vector vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
	_vector vUp = XMVector3Cross(vLook, vRight);

	Set_State(STATE_RIGHT, XMVector3Normalize(vRight) * vScaled.x);
	Set_State(STATE_UP, XMVector3Normalize(vUp) * vScaled.y);
	Set_State(STATE_LOOK, XMVector3Normalize(vLook) * vScaled.z);
}

void CTransform::Set_Look_ForLandObject(_fvector direction)
{
	_float3	vScaled = Get_Scaled();
	_vector	vLook = direction;
	_vector vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
	vLook = XMVector3Cross(vRight, XMVectorSet(0.f, 1.f, 0.f, 0.f));

	Set_State(STATE_RIGHT, XMVector3Normalize(vRight) * vScaled.x);
	Set_State(STATE_LOOK, XMVector3Normalize(vLook) * vScaled.z);
}

CTransform * CTransform::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CTransform*		pInstance = new CTransform(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTransform");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CTransform::Clone(void * pArg)
{
	return nullptr;
}

void CTransform::Free()
{
	__super::Free();

}
