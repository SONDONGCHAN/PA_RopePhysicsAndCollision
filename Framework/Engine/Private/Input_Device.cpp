#include "Input_Device.h"

Engine::CInput_Device::CInput_Device(void)
{

}

_bool CInput_Device::KeyDown(_ubyte _eKeyID)
{
	if (!(m_preKeyState[_eKeyID] & 0x80) && (m_byKeyState[_eKeyID] & 0x80))
		return true;

	return false;
}

_bool CInput_Device::KeyUp(_ubyte _eKeyID)
{
	if ((m_preKeyState[_eKeyID] & 0x80) && !(m_byKeyState[_eKeyID] & 0x80))
		return true;

	return false;
}

_bool CInput_Device::KeyPressing(_ubyte _eKeyID)
{
	if ((m_preKeyState[_eKeyID] & 0x80) && (m_byKeyState[_eKeyID] & 0x80))
		return true;

	return false;
}

_byte CInput_Device::MouseDown(MOUSEKEYSTATE _eMouse)
{
	return ((m_tMouseState.rgbButtons[_eMouse]) && !(m_preMouseState.rgbButtons[_eMouse]));
}

_byte CInput_Device::MousePressing(MOUSEKEYSTATE _eMouse)
{
	return ((m_tMouseState.rgbButtons[_eMouse]) && (m_preMouseState.rgbButtons[_eMouse]));
}

_byte CInput_Device::MouseUp(MOUSEKEYSTATE _eMouse)
{
	return (!(m_tMouseState.rgbButtons[_eMouse]) && !(m_preMouseState.rgbButtons[_eMouse]));
}

HRESULT Engine::CInput_Device::Ready_InputDev(HINSTANCE hInst, HWND hWnd)
{
	if (FAILED(DirectInput8Create(hInst,
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		(void**)&m_pInputSDK,
		nullptr)))
		return E_FAIL;

	if (FAILED(m_pInputSDK->CreateDevice(GUID_SysKeyboard, &m_pKeyBoard, nullptr)))
		return E_FAIL;

	// ������ Ű���� ��ü�� ���� ������ �� ��ü���� �����ϴ� �Լ�
	m_pKeyBoard->SetDataFormat(&c_dfDIKeyboard);

	// ��ġ�� ���� �������� �������ִ� �Լ�, (Ŭ���̾�Ʈ�� ���ִ� ���¿��� Ű �Է��� ������ ������ �����ϴ� �Լ�)
	m_pKeyBoard->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

	// ��ġ�� ���� access ������ �޾ƿ��� �Լ�
	m_pKeyBoard->Acquire();


	// ���콺 ��ü ����
	if (FAILED(m_pInputSDK->CreateDevice(GUID_SysMouse, &m_pMouse, nullptr)))
		return E_FAIL;

	// ������ ���콺 ��ü�� ���� ������ �� ��ü���� �����ϴ� �Լ�
	m_pMouse->SetDataFormat(&c_dfDIMouse);

	// ��ġ�� ���� �������� �������ִ� �Լ�, Ŭ���̾�Ʈ�� ���ִ� ���¿��� Ű �Է��� ������ ������ �����ϴ� �Լ�
	m_pMouse->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

	// ��ġ�� ���� access ������ �޾ƿ��� �Լ�
	m_pMouse->Acquire();


	return S_OK;
}

void Engine::CInput_Device::Tick(void)
{
	memcpy(m_preKeyState, m_byKeyState, sizeof(_byte) * 256);
	memcpy(&m_preMouseState, &m_tMouseState, sizeof(m_tMouseState));

	m_pKeyBoard->GetDeviceState(256, m_byKeyState);
	m_pMouse->GetDeviceState(sizeof(m_tMouseState), &m_tMouseState);
}

CInput_Device * CInput_Device::Create(HINSTANCE hInst, HWND hWnd)
{
	CInput_Device*		pInstance = new CInput_Device();

	if (FAILED(pInstance->Ready_InputDev(hInst, hWnd)))
	{
		MSG_BOX("Failed to Created : CInput_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Engine::CInput_Device::Free(void)
{
	Safe_Release(m_pKeyBoard);
	Safe_Release(m_pMouse);
	Safe_Release(m_pInputSDK);
}

