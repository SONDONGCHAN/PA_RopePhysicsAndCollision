#pragma once

#pragma warning (disable : 4251)

#include <d3d11.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <DirectXCollision.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <Effects11\d3dx11effect.h>
#include <DirectXTK\DDSTextureLoader.h>
#include <DirectXTK\WICTextureLoader.h>
#include <DirectXTK\ScreenGrab.h>
#include <DirectXTK\PrimitiveBatch.h>
#include <DirectXTK\VertexTypes.h>
#include <DirectXTK\Effects.h>
#include <DirectXTK\SpriteBatch.h>
#include <DirectXTK\SpriteFont.h>

#include <assimp\scene.h>
#include <assimp\Importer.hpp>
#include <assimp\postprocess.h>
#include <random>

using namespace DirectX;

#include <vector>
#include <list>
#include <map>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
using namespace std;

namespace Engine
{
	enum MOUSEKEYSTATE { DIM_LB, DIM_RB, DIM_MB, DIM_END };
	enum MOUSEMOVESTATE { DIMS_X, DIMS_Y, DIMS_Z, DIMS_END };

	const wstring		g_strTransformTag = TEXT("Com_Transform");

	enum COL_TYPE
	{
		COL_NONE = 0,
		COL_PLAYER = 1 << 0,
		COL_STATIC_OBJECT = 1 << 1,
		COL_PROJECTILE = 1 << 2,

		COL_END = 1 << 31
	};
}



#include "Engine_Function.h"
#include "Engine_Macro.h"
#include "Engine_Struct.h"
#include "Engine_Typedef.h"


using namespace Engine;


/*Json*/
#include "Json/json.h"
using namespace Json;


#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifndef DBG_NEW 

#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
#define new DBG_NEW 

#endif

#endif // _DEBUG




