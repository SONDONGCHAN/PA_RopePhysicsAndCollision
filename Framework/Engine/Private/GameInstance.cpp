#include "..\Public\GameInstance.h"
#include "Graphic_Device.h"
#include "Input_Device.h"
#include "Level_Manager.h"
#include "Object_Manager.h"
#include "Component_Manager.h"
#include "Timer_Manager.h"
#include "Light_Manager.h"
#include "Font_Manager.h"
#include "Target_Manager.h"
#include "Frustum.h"
#include "Json_Manager.h"
#include "Collision_Manager.h"


IMPLEMENT_SINGLETON(CGameInstance)

CGameInstance::CGameInstance()
{
}

HRESULT CGameInstance::Initialize_Engine(HINSTANCE hInst, _uint iNumLevels, const GRAPHIC_DESC& GraphicDesc, _Inout_ ID3D11Device** ppDevice, _Inout_ ID3D11DeviceContext** ppDeviceContext)
{
	/* 그래픽 초기화. */
	m_pGraphic_Device = CGraphic_Device::Create(GraphicDesc, ppDevice, ppDeviceContext);
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pFont_Manager = CFont_Manager::Create(*ppDevice, *ppDeviceContext);
	if (nullptr == m_pFont_Manager)
		return E_FAIL;

	/* 입력장치 초기화. */
	m_pInput_Device = CInput_Device::Create(hInst, GraphicDesc.hWnd);
	if (nullptr == m_pInput_Device)
		return E_FAIL;

	m_pLight_Manager = CLight_Manager::Create(*ppDevice, *ppDeviceContext);
	if (nullptr == m_pLight_Manager)
		return E_FAIL;

	m_pPipeLine = CPipeLine::Create();
	if (nullptr == m_pPipeLine)
		return E_FAIL;

	m_pTarget_Manager = CTarget_Manager::Create(*ppDevice, *ppDeviceContext);
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;


	m_pRenderer = CRenderer::Create(*ppDevice, *ppDeviceContext);
	if (nullptr == m_pRenderer)
		return E_FAIL;

	m_pLevel_Manager = CLevel_Manager::Create();
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	m_pTimer_Manager = CTimer_Manager::Create();
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;


	/* 오브젝트 매니져 리저브*/
	m_pObject_Manager = CObject_Manager::Create(iNumLevels);
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	/* 컴포넌트 매니져 리저브*/
	m_pComponent_Manager = CComponent_Manager::Create(iNumLevels);
	if (nullptr == m_pComponent_Manager)
		return E_FAIL;

	m_pFrustum = CFrustum::Create();
	if (nullptr == m_pFrustum)
		return E_FAIL;


	m_pJson_Manager = CJson_Manager::Create();
	if (nullptr == m_pJson_Manager)
		return E_FAIL;

	m_pCollision_Manager = CCollision_Manager::Create();
	if (nullptr == m_pCollision_Manager)
		return E_FAIL;

	return S_OK;
}

void CGameInstance::Tick_Engine(_float fTimeDelta)
{
	if (nullptr == m_pLevel_Manager || 
		nullptr == m_pObject_Manager)
		return;

	/* 다이렉트 인풋을 통해 키보드와 마우스의 상태를 저장ㅎ나다 .*/
	m_pInput_Device->Tick();
	
	m_pLevel_Manager->Tick(fTimeDelta);
	
	m_pObject_Manager->Priority_Tick(fTimeDelta);
	
	m_pPipeLine->Tick();
	
	m_pFrustum->Tick();
	
	m_pObject_Manager->Tick(fTimeDelta);

	m_pCollision_Manager->Check_Collision();
	
	m_pObject_Manager->Late_Tick(fTimeDelta);
}

HRESULT CGameInstance::Begin_Draw(const _float4 & vClearColor)
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device->Clear_BackBuffer_View(vClearColor);
	m_pGraphic_Device->Clear_DepthStencil_View();

	return S_OK;
}

HRESULT CGameInstance::End_Draw()
{
	return m_pGraphic_Device->Present();
}

HRESULT CGameInstance::Draw()
{

	/* 그리기용 객체를 통해 무언가를 그린다. */
	m_pRenderer->Render();
	m_pLevel_Manager->Render();
	

	return S_OK;
}

void CGameInstance::Clear(_uint iLevelIndex)
{
	if (nullptr == m_pObject_Manager ||
		nullptr == m_pComponent_Manager)
		return;

	m_pObject_Manager->Clear(iLevelIndex);
	m_pComponent_Manager->Clear(iLevelIndex);	
}

_byte CGameInstance::Get_DIKeyState(_ubyte byKeyID)
{
	return m_pInput_Device->Get_DIKeyState(byKeyID);	
}

_byte CGameInstance::Get_DIMouseState(MOUSEKEYSTATE eMouse)
{
	return m_pInput_Device->Get_DIMouseState(eMouse);
}

_long CGameInstance::Get_DIMouseMove(MOUSEMOVESTATE eMouseState)
{
	return m_pInput_Device->Get_DIMouseMove(eMouseState);
}

_bool CGameInstance::KeyDown(_ubyte _eKeyID)
{
	if (!m_pInput_Device)
		return false;

	return m_pInput_Device->KeyDown(_eKeyID);
}

_bool CGameInstance::KeyUp(_ubyte _eKeyID)
{
	if (!m_pInput_Device)
		return false;

	return m_pInput_Device->KeyUp(_eKeyID);
}

_bool CGameInstance::KeyPressing(_ubyte _eKeyID)
{
	if (!m_pInput_Device)
		return false;

	return m_pInput_Device->KeyPressing(_eKeyID);
}

_byte CGameInstance::MouseDown(MOUSEKEYSTATE _eMouse)
{
	return m_pInput_Device->MouseDown(_eMouse);
}

_byte CGameInstance::MousePressing(MOUSEKEYSTATE _eMouse)
{
	return m_pInput_Device->MousePressing(_eMouse);
}

_byte CGameInstance::MouseUp(MOUSEKEYSTATE _eMouse)
{
	return m_pInput_Device->MouseUp(_eMouse);
}

_float CGameInstance::Compute_TimeDelta(const wstring & pTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return 0.f;

	return m_pTimer_Manager->Compute_TimeDelta(pTimerTag);	
}

HRESULT CGameInstance::Add_Timer(const wstring & pTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;

	return m_pTimer_Manager->Add_Timer(pTimerTag);
}

HRESULT CGameInstance::Add_RenderGroup(CRenderer::RENDERGROUP eRenderGroup, CGameObject * pGameObject)
{
	if (nullptr == m_pRenderer)
		return E_FAIL;

	return m_pRenderer->Add_RenderGroup(eRenderGroup, pGameObject);	
}
#ifdef _DEBUG

HRESULT CGameInstance::Add_DebugComponent(CComponent * pComponent)
{
	return m_pRenderer->Add_DebugComponent(pComponent);
}
#endif // _DEBUG



HRESULT CGameInstance::Open_Level(_uint iLevelIndex, CLevel * pNewLevel)
{
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	return m_pLevel_Manager->Open_Level(iLevelIndex, pNewLevel);	
}

HRESULT CGameInstance::Add_Prototype(const wstring & strPrototypeTag, CGameObject * pPrototype)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_Prototype(strPrototypeTag, pPrototype);	
}

HRESULT CGameInstance::Add_CloneObject(_uint iLevelIndex, const wstring & strLayerTag, const wstring & strPrototypeTag, void * pArg)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_CloneObject(iLevelIndex, strLayerTag, strPrototypeTag, pArg);
}

HRESULT CGameInstance::Add_GameObject(_uint iLevelIndex, const wstring& strLayerTag, CGameObject* pGameObject)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_GameObject(iLevelIndex, strLayerTag, pGameObject);
}

CGameObject * CGameInstance::Clone_GameObject(const wstring & strPrototypeTag, void * pArg)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Clone_GameObject(strPrototypeTag, pArg);	
}

CComponent * CGameInstance::Get_Component(_uint iLevelIndex, const wstring & strLayerTag, const wstring & strComTag, _uint iIndex)
{
	return m_pObject_Manager->Get_Component(iLevelIndex, strLayerTag, strComTag, iIndex);	
}

CGameObject * CGameInstance::Get_GameObject(_uint iLevelIndex, const wstring & strLayerTag, _uint iIndex)
{
	return m_pObject_Manager->Get_GameObject(iLevelIndex, strLayerTag, iIndex);
}

HRESULT CGameInstance::Add_Prototype(_uint iLevelIndex, const wstring & strPrototypeTag, CComponent * pPrototype)
{
	if (nullptr == m_pComponent_Manager)
		return E_FAIL;

	return m_pComponent_Manager->Add_Prototype(iLevelIndex, strPrototypeTag, pPrototype);	
}

CComponent * CGameInstance::Clone_Component(_uint iLevelIndex, const wstring & strPrototypeTag, void * pArg)
{
	if (nullptr == m_pComponent_Manager)
		return nullptr;
	
	return m_pComponent_Manager->Clone_Component(iLevelIndex, strPrototypeTag, pArg);
}

void CGameInstance::Set_Transform(CPipeLine::D3DTRANSFORMSTATE eState, _fmatrix TransformMatrix)
{
	m_pPipeLine->Set_Transform(eState, TransformMatrix);
}

_float4x4 CGameInstance::Get_Transform_Float4x4(CPipeLine::D3DTRANSFORMSTATE eState)
{
	return m_pPipeLine->Get_Transform_Float4x4(eState);
}

_matrix CGameInstance::Get_Transform_Matrix(CPipeLine::D3DTRANSFORMSTATE eState)
{
	return m_pPipeLine->Get_Transform_Matrix(eState);
}

_float4x4 CGameInstance::Get_Transform_Float4x4_Inverse(CPipeLine::D3DTRANSFORMSTATE eState)
{
	return m_pPipeLine->Get_Transform_Float4x4_Inverse(eState);
}

_matrix CGameInstance::Get_Transform_Matrix_Inverse(CPipeLine::D3DTRANSFORMSTATE eState)
{
	return m_pPipeLine->Get_Transform_Matrix_Inverse(eState);
}

_float4 CGameInstance::Get_CamPosition()
{
	return m_pPipeLine->Get_CamPosition();
}

HRESULT CGameInstance::Add_Light(const LIGHT_DESC & LightDesc)
{
	return m_pLight_Manager->Add_Light(LightDesc);
}

const LIGHT_DESC * CGameInstance::Get_LightDesc(_uint iIndex) const
{
	return m_pLight_Manager->Get_LightDesc(iIndex);
}

HRESULT CGameInstance::Render_Light(CShader * pShader, CVIBuffer_Rect * pVIBuffer)
{
	return m_pLight_Manager->Render(pShader, pVIBuffer);	
}

HRESULT CGameInstance::Add_Font(const wstring & strFontTag, const wstring & strFontFilePath)
{
	return m_pFont_Manager->Add_Font(strFontTag, strFontFilePath);	
}

HRESULT CGameInstance::Render_Font(const wstring & strFontTag, const wstring & strText, const _float2 & vPosition, _fvector vColor, _float fRotation, const _float2 & vOrigin, _float fScale)
{
	return m_pFont_Manager->Render(strFontTag, strText, vPosition, vColor, fRotation, vOrigin, fScale);
}

HRESULT CGameInstance::Add_RenderTarget(const wstring & strTargetTag, _uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const _float4 & vClearColor)
{
	return m_pTarget_Manager->Add_RenderTarget(strTargetTag, iSizeX, iSizeY, ePixelFormat, vClearColor);
	
}

HRESULT CGameInstance::Add_MRT(const wstring & strMRTTag, const wstring & strTargetTag)
{
	return m_pTarget_Manager->Add_MRT(strMRTTag, strTargetTag);
}

HRESULT CGameInstance::Begin_MRT(const wstring & strMRTTag, ID3D11DepthStencilView* pDSView)
{
	return m_pTarget_Manager->Begin_MRT(strMRTTag, pDSView);

}

HRESULT CGameInstance::End_MRT()
{
	return m_pTarget_Manager->End_MRT();
}

HRESULT CGameInstance::Bind_SRV(const wstring & strTargetTag, CShader * pShader, const _char * pConstantName)
{
	return m_pTarget_Manager->Bind_SRV(strTargetTag, pShader, pConstantName);
	
}

void CGameInstance::Transform_ToLocalSpace(_fmatrix WorldMatrix)
{
	m_pFrustum->Transform_ToLocalSpace(WorldMatrix);
}

_bool CGameInstance::isIn_Frustum_WorldSpace(_fvector vPosition, _float fRadius)
{
	return m_pFrustum->isIn_WorldSpace(vPosition, fRadius);
}

_bool CGameInstance::isIn_Frustum_LocalSpace(_fvector vPosition, _float fRadius)
{
	return m_pFrustum->isIn_LocalSpace(vPosition, fRadius);	
}

void CGameInstance::WriteJson(Json::Value _value, const wstring& _strSavePath)
{
	if (!m_pJson_Manager)
		return;

	m_pJson_Manager->WriteJson(_value, _strSavePath);
}

Json::Value CGameInstance::ReadJson(const wstring& _strReadPath)
{
	return m_pJson_Manager->ReadJson(_strReadPath);
}

_bool CGameInstance::Add_Collider(CCollider* _pCollider)
{
	return m_pCollision_Manager->Add_Collider(_pCollider);
}

#ifdef _DEBUG
HRESULT CGameInstance::Ready_Debug(const wstring & strTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY)
{
	return m_pTarget_Manager->Ready_Debug(strTargetTag, fX, fY, fSizeX, fSizeY);	
}
HRESULT CGameInstance::Render_MRT(const wstring & strMRTTag, CShader * pShader, CVIBuffer_Rect * pVIBuffer)
{
	return m_pTarget_Manager->Render_MRT(strMRTTag, pShader, pVIBuffer);	
}
#endif // _DEBUG

void CGameInstance::Release_Engine()
{
	CGameInstance::GetInstance()->Release_Manager();

	DestroyInstance();
}

void CGameInstance::Release_Manager()
{
	Safe_Release(m_pFrustum);
	Safe_Release(m_pTarget_Manager);
	Safe_Release(m_pFont_Manager);
	Safe_Release(m_pLight_Manager);
	Safe_Release(m_pPipeLine);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pComponent_Manager);
	Safe_Release(m_pObject_Manager);
	Safe_Release(m_pLevel_Manager);	
	Safe_Release(m_pTimer_Manager);
	Safe_Release(m_pInput_Device);	
	Safe_Release(m_pGraphic_Device);
	Safe_Release(m_pJson_Manager);
	Safe_Release(m_pCollision_Manager);
}

void CGameInstance::Free()
{

}
