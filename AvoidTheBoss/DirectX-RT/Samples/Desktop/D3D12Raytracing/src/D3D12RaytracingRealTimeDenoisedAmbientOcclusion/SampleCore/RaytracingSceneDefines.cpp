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

#pragma once

#include "RayTracingSceneDefines.h"

namespace SampleScene
{
	const WCHAR* Type::Names[Type::Count] = { L"Main Sample Scene" };
	Params args[Type::Count];

	// Initialize scene parameters
	Initialize initializeObject;
	Initialize::Initialize()
	{
		// Camera Position 카메라 위치
		{
			float height = 1.35f;
			auto& camera = args[Type::Main].camera;
            camera.position.eye = { 0.0f, height+10.0f, 10.0f, 1 };
            camera.position.at = { 0.0f, height, -1.0f, 1 };
            camera.position.up = { 0.0f, height+2.0f, 0.0f, 0 };
			camera.boundaries.min = XMVectorSetY(-XMVectorSplatInfinity(), 0);
			camera.boundaries.max = XMVectorSplatInfinity();
		}
	}
}
