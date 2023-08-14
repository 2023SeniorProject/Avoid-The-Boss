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

#include "stdafx.h"
#include "D3D12RaytracingRealTimeDenoisedAmbientOcclusion.h"
#include "Sampler.h"
#include "clientIocpCore.h"
#include "ThreadManager.h"
#include "SocketUtil.h"


_Use_decl_annotations_
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    // Initialization For WICTextureLoader.
    ThrowIfFailed(CoInitializeEx(nullptr, COINITBASE_MULTITHREADED), L"Failed to initialize WIC component");

    D3D12RaytracingRealTimeDenoisedAmbientOcclusion sample(1920, 1080, L"D3D12 Raytracing - Real-Time Denoised Raytraced Ambient Occlusion");
    
    SocketUtil::Init();



    // 기본 메시지 루프입니다:
  


    return Win32Application::Run(&sample, hInstance, nCmdShow);
}
