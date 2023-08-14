//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently.

#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers.
#endif


// Use the C++ standard templated min/max
#define NOMINMAX

#include <windows.h>


// C RunTime Header Files
#include <stdlib.h>
#include <sstream>
#include <iomanip>
#include <shellapi.h>
#include <atlbase.h>
#include <assert.h>
#include <functional>
#include <random>
#include <numeric>
#include <iterator>
#include <sal.h>
#include <stack>
#include <xstring>

#include <stdint.h>
#include <float.h>




#include <atlbase.h>
#include "d3dx12.h"	
#include <pix3.h>

#include <DirectXMath.h>
#include <WICTextureLoader.h>
#include <DDSTextureLoader.h>
#include "ResourceUploadBatch.h"





#ifdef _DEBUG
#pragma comment(lib, "Debug\\CoreEngine.lib")
#include <dxgidebug.h>
#else
#pragma comment(lib, "Release\\CoreEngine.lib")
#endif



#include "CorePch.h"
#include "RaytracingHlslCompat.h"
#include "RaytracingSceneDefines.h"
#include "DXSampleHelper.h"
#include "DeviceResources.h"

