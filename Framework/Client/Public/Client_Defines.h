#pragma once

#include <process.h>
#include <string>
#include "GameInstance.h"

namespace Client
{
	enum LEVEL { LEVEL_STATIC, LEVEL_LOADING, LEVEL_LOGO, LEVEL_GAMEPLAY, LEVEL_END };

	enum Direction { DIR_FRONT, DIR_BACK, DIR_LEFT, DIR_RIGHT, DIR_FLEFT, DIR_FRIGHT, DIR_BLEFT, DIR_BRIGHT, DIR_END };

	const unsigned int		g_iWinSizeX = 1280;
	const unsigned int		g_iWinSizeY = 720;

	inline bool KEYDOWN(uint8_t key)  {return CGameInstance::GetInstance()->KeyDown(key); }
	inline bool KEYPRESSING(uint8_t key) { return CGameInstance::GetInstance()->KeyPressing(key); }
	inline bool KEYINPUT(uint8_t key) { return (KEYDOWN(key) || KEYPRESSING(key)) ; }
	inline bool KEYUP(uint8_t key) { return CGameInstance::GetInstance()->KeyUp(key); }


}

extern HWND				g_hWnd;
extern HINSTANCE		g_hInst;

using namespace std;
using namespace Client;
