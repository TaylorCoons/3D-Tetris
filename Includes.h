#pragma once
// Windows includes
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers.
#endif

#include <windows.h>

// Direct3D 12 includes
#include <d3d12.h>
#include <dxgi1_4.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include <d3dx12.h>

#pragma comment(lib, "Dxgi.lib")
#pragma comment(lib, "D3D12.lib")

// System includes
#include <string>
#include <wrl.h>
#include <shellapi.h>

#include "Utility.h"


// Platform x64
// Configuration Manager->General->Version Platform 10.0.14393.0
// "							"->Version 10
// "							"->Character Set->Use Unicode Character Set

