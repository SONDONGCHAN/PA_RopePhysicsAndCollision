#pragma once

#include <process.h>
#include <string>
#include "GameInstance.h"

namespace Client
{
	enum LEVEL { LEVEL_STATIC, LEVEL_LOADING, LEVEL_LOGO, LEVEL_GAMEPLAY, LEVEL_END };

	const unsigned int		g_iWinSizeX = 1280;
	const unsigned int		g_iWinSizeY = 720;

	const _uint STATE_IDLE = 0x00000001;
	const _uint STATE_RUN = 0x00000002;
}

extern HWND				g_hWnd;
extern HINSTANCE		g_hInst;

using namespace std;
using namespace Client;
