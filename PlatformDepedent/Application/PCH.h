//
// Created by Administrator on 2022/12/26.
//

#ifndef DISTENGINE_PCH_H
#define DISTENGINE_PCH_H

//=================================================================================================
//
//  MJP's DX12 Sample Framework
//  http://mynameismjp.wordpress.com/
//
//  All code licensed under the MIT license
//
//=================================================================================================

#pragma once

// Add common controls 6.0 DLL to the manifest
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif

// Standard int typedefs
#include <stdint.h>
typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;
typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef intptr_t intptr;
typedef uintptr_t uintptr;
typedef wchar_t wchar;
typedef uint32_t bool32;

// Disabled compiler warnings
#pragma warning(disable : 4100) // unreferenced formal parameter
#pragma warning(disable : 4127) // conditional expression is constant
#pragma warning(disable : 4324) // structure was padded due to alignment specifier

// Platform SDK defines, specifies that our min version is Windows 10
#ifndef WINVER
#define WINVER 0x0A00
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0A00
#endif

#ifndef _WIN32_WINDOWS
#define _WIN32_WINDOWS 0x0410
#endif

#ifndef _WIN32_IE
#define _WIN32_IE 0x0A00
#endif

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#define STRICT                          // Use strict declarations for Windows types

#define assert(expression) ((void)0)

// Windows Header Files:
#include <windows.h>
#include <commctrl.h>
#include <psapi.h>
#include <process.h>
#include <wrl.h>

// MSVC COM Support
#include <comip.h>
#include <comdef.h>

// GDI+
#pragma warning(push)
#pragma warning(disable : 4458)
#include <gdiplus.h>
#pragma warning(pop)

// DirectX Includes
#ifdef _DEBUG
#ifndef D3D_DEBUG_INFO
#define D3D_DEBUG_INFO
#endif
#endif

#include <dxgi.h>
#include <dxgi1_5.h>
#include <d3d12.h>
#include <D3Dcompiler.h>

// DirectX Math
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXCollision.h>

// DirectX Tex
#include "..\\..\\3rd_party\\DirectXTex July 2017\\Include\\DirectXTex.h"
#ifdef _DEBUG
#pragma comment(lib, "..\\3rd_party\\DirectXTex July 2017\\Lib 2017\\Debug\\DirectXTex.lib")
#else
#pragma comment(lib, "..\\3rd_party\\DirectXTex July 2017\\Lib 2017\\Release\\DirectXTex.lib")
#endif

// Pix for Windows
#define USE_PIX
#include "..\\..\\3rd_party\\WinPixEventRuntime\\Include\\WinPixEventRuntime\\pix3.h"
#pragma comment(lib, "..\\3rd_party\\WinPixEventRuntime\\bin\\WinPixEventRuntime.lib")

// Un-define min and max from the windows headers
#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#pragma warning(push)
#pragma warning(disable : 4005)
#include <wincodec.h>
#pragma warning(pop)

// C++ Standard Library Header Files
#include <functional>
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <cmath>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <complex>
#include <cstdio>
#include <cstdarg>
#include <random>

#include "AppConfig.h"

// Assimp
#include "..\\..\\3rd_party\\Assimp-3.1.1\\include\\Importer.hpp"
#include "..\\..\\3rd_party\\Assimp-3.1.1\\include\\scene.h"
#include "..\\..\\3rd_party\\Assimp-3.1.1\\include\\postprocess.h"
#pragma comment(lib, "..\\3rd_party\\Assimp-3.1.1\\lib\\assimp.lib")

// Embree
#if EnableEmbree_
#include "..\\..\\3rd_party\\Embree-2.8\\include\\embree2\\rtcore.h"
    #include "..\\..\\3rd_party\\Embree-2.8\\include\\embree2\\rtcore_ray.h"
    #pragma comment(lib, "..\\3rd_party\\Embree-2.8\\lib\\embree.lib")
#endif

// cxxopts
#include "..\\..\\3rd_party\\cxxopts\\include\\\cxxopts.hpp"

// Windows SDK imports
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "DXGI.lib")
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "psapi.lib")
#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "D3D12.lib")

#if EnableShaderModel6_
#include "..\\..\\3rd_party\\DXCompiler\\Include\\dxcapi.h"
    #pragma comment(lib, "..\\3rd_party\\DXCompiler\\Lib\\dxcompiler.lib")
#else
#pragma comment(lib, "d3dcompiler.lib")
#endif

#ifdef _DEBUG
#pragma comment(lib, "comsuppwd.lib")
#else
#pragma comment(lib, "comsuppw.lib")
#endif

#endif //DISTENGINE_PCH_H
