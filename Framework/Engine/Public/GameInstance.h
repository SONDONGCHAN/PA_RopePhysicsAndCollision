#pragma once

/* 엔진에서 제공하는 대부분의 기능들을 가지고 있는다. */
/* Object_Manager, Level_Manager, Component_Manager, Graphic_Device, Input_Device */
/* 위와 같은 여러 클래스들의 기능들을 클라이언트에 보여주고 실제 호출도 수행한다. */
#include "Renderer.h"
#include "Component_Manager.h"
#include "PipeLIne.h"

BEGIN(Engine)

class ENGINE_DLL CGameInstance final : public CBase
{
	DECLARE_SINGLETON(CGameInstance)
private:
	CGameInstance();
	virtual ~CGameInstance() = default;

public:
	HRESULT Initialize_Engine(HINSTANCE hInst, _uint iNumLevels, const GRAPHIC_DESC& GraphicDesc, _Inout_ ID3D11Device** ppDevice, _Inout_ ID3D11DeviceContext** ppDeviceContext);
	void Tick_Engine(_float fTimeDelta);
	
	HRESULT Begin_Draw(const _float4& vClearColor);
	HRESULT Draw();
	HRESULT End_Draw();
	
	void Clear(_uint iLevelIndex);

public: /* For.Input_Device */
	_byte	Get_DIKeyState(_ubyte byKeyID);
	_byte	Get_DIMouseState(MOUSEKEYSTATE eMouse);
	_long	Get_DIMouseMove(MOUSEMOVESTATE eMouseState);

	_bool KeyDown(_ubyte _eKeyID);
	_bool KeyUp(_ubyte _eKeyID);
	_bool KeyPressing(_ubyte _eKeyID);

	_byte MouseDown(MOUSEKEYSTATE _eMouse);
	_byte MousePressing(MOUSEKEYSTATE _eMouse);
	_byte MouseUp(MOUSEKEYSTATE _eMouse);

public: /* For.Timer_Manager */
	_float Compute_TimeDelta(const wstring& pTimerTag);
	HRESULT Add_Timer(const wstring& pTimerTag);

public: /* For.Renderer*/ 
	HRESULT Add_RenderGroup(CRenderer::RENDERGROUP eRenderGroup, class CGameObject* pGameObject);
#ifdef _DEBUG
public:
	HRESULT Add_DebugComponent(class CComponent* pComponent);
#endif // _DEBUG

public: /* For.Level_Manager */
	HRESULT Open_Level(_uint iLevelIndex, class CLevel* pNewLevel);

public: /* For.Object_Manager */
	HRESULT Add_Prototype(const wstring& strPrototypeTag, class CGameObject* pPrototype);
	HRESULT Add_CloneObject(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strPrototypeTag, void* pArg = nullptr);
	HRESULT Add_GameObject(_uint iLevelIndex, const wstring& strLayerTag, class CGameObject* pGameObject);
	class CGameObject* Clone_GameObject(const wstring& strPrototypeTag, void* pArg = nullptr);
	class CComponent* Get_Component(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strComTag, _uint iIndex = 0);
	class CGameObject* Get_GameObject(_uint iLevelIndex, const wstring& strLayerTag, _uint iIndex = 0);

public: /* For.Component_Manager */
	HRESULT Add_Prototype(_uint iLevelIndex, const wstring& strPrototypeTag, class CComponent* pPrototype);
	class CComponent* Clone_Component(_uint iLevelIndex, const wstring& strPrototypeTag, void* pArg = nullptr);

public: /* For.PipeLine */
	void Set_Transform(CPipeLine::D3DTRANSFORMSTATE eState, _fmatrix TransformMatrix);
	_float4x4 Get_Transform_Float4x4(CPipeLine::D3DTRANSFORMSTATE eState);
	_matrix Get_Transform_Matrix(CPipeLine::D3DTRANSFORMSTATE eState);
	_float4x4 Get_Transform_Float4x4_Inverse(CPipeLine::D3DTRANSFORMSTATE eState);
	_matrix Get_Transform_Matrix_Inverse(CPipeLine::D3DTRANSFORMSTATE eState);
	_float4 Get_CamPosition();

public: /* For.Light_Manager */
	HRESULT Add_Light(const LIGHT_DESC& LightDesc);
	const LIGHT_DESC* Get_LightDesc(_uint iIndex) const;
	HRESULT Render_Light(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);

public: /* For.Font_Manager */
	HRESULT Add_Font(const wstring& strFontTag, const wstring& strFontFilePath);
	HRESULT Render_Font(const wstring& strFontTag, const wstring& strText, const _float2& vPosition, _fvector vColor = Colors::White, _float fRotation = 0.f, const _float2& vOrigin = _float2(0.f, 0.f), _float fScale = 1.f);

public: /* For.Target_Manager */
	HRESULT Add_RenderTarget(const wstring& strTargetTag, _uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const _float4& vClearColor);
	HRESULT Add_MRT(const wstring& strMRTTag, const wstring& strTargetTag);
	HRESULT Begin_MRT(const wstring& strMRTTag, ID3D11DepthStencilView* pDSView = nullptr);
	HRESULT End_MRT();
	HRESULT Bind_SRV(const wstring& strTargetTag, class CShader* pShader, const _char* pConstantName);

public: /* For.Frustum */
	void Transform_ToLocalSpace(_fmatrix WorldMatrix);
	_bool isIn_Frustum_WorldSpace(_fvector vPosition, _float fRadius = 0.0f);
	_bool isIn_Frustum_LocalSpace(_fvector vPosition, _float fRadius = 0.f);

public: /* For.Json_Manager */
	void WriteJson(Json::Value _value, const wstring & _strSavePath);
	Json::Value ReadJson(const wstring & _strReadPath);

public: /* etc */
	int		Get_CurrentLevel() { return m_iCurrent_Level;}
	void	Set_CurrentLevel(_int _iLevel) { m_iCurrent_Level = _iLevel; }





#ifdef _DEBUG
	HRESULT Ready_Debug(const wstring& strTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY);
	HRESULT Render_MRT(const wstring& strMRTTag, class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
#endif // _DEBUG



private:
	class CGraphic_Device*			m_pGraphic_Device = { nullptr };
	class CInput_Device*			m_pInput_Device = { nullptr };
	class CTimer_Manager*			m_pTimer_Manager = { nullptr };
	class CLevel_Manager*			m_pLevel_Manager = { nullptr };
	class CObject_Manager*			m_pObject_Manager = { nullptr };
	class CComponent_Manager*		m_pComponent_Manager = { nullptr };
	class CRenderer*				m_pRenderer = { nullptr };
	class CPipeLine*				m_pPipeLine = { nullptr };
	class CLight_Manager*			m_pLight_Manager = { nullptr };
	class CFont_Manager*			m_pFont_Manager = { nullptr };
	class CTarget_Manager*			m_pTarget_Manager = { nullptr };
	class CFrustum*					m_pFrustum = { nullptr };
	class CJson_Manager*			m_pJson_Manager = { nullptr };

	
	
	int m_iCurrent_Level = { 0 };

public:
	static void Release_Engine();
	void Release_Manager();
	virtual void Free() override;
};

END