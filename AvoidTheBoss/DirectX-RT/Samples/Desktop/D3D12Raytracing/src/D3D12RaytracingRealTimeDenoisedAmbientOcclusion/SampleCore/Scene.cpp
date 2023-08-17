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
#include "Scene.h"
#include "GameInput.h"
#include "EngineTuning.h"
#include "EngineProfiling.h"
#include "GpuTimeManager.h"
#include "RaytracingSceneDefines.h"
#include "D3D12RaytracingRealTimeDenoisedAmbientOcclusion.h"
#include "SceneManager.h"

using namespace std;
using namespace DX;
using namespace DirectX;
using namespace SceneEnums;
using namespace GameCore;

UIParameters g_UIparameters;

const UINT StandardVertexStride = 44;
const DXGI_FORMAT StandardIndexFormat = DXGI_FORMAT_R32_UINT;
const UINT StandardIndexStride = sizeof(UINT);



Scene::Scene()
{
    InitializeScene();
}

void Scene::Setup(shared_ptr<DeviceResources> deviceResources, shared_ptr<DX::DescriptorHeap> descriptorHeap)
{
    m_deviceResources = deviceResources;
    m_cbvSrvUavHeap = descriptorHeap;

    CreateDeviceDependentResources();
}

void Scene::OnLeftButtonDown(UINT x, UINT y)
{

}

void Scene::OnLeftButtonUp(UINT x, UINT y)
{

}

void Scene::OnDisplayChanged()
{
}

void Scene::OnSizeChanged(UINT width, UINT height, bool minimized)
{
}

void Scene::CreateDeviceDependentResources()
{
    auto device = m_deviceResources->GetD3DDevice();

    Sample::instance().GetSceneManager()->InitializeGeometry();

    m_prevFrameBottomLevelASInstanceTransforms.Create(device, MaxNumBottomLevelInstances, Sample::FrameCount, L"GPU buffer: Bottom Level AS Instance transforms for previous frame");
}

void Scene::InitializeAllBottomLevelAccelerationStructures()
{
    auto device = m_deviceResources->GetD3DDevice();
    auto FrameCount = m_deviceResources->GetBackBufferCount();

    m_accelerationStructure = make_unique<RaytracingAccelerationStructureManager>(device, MaxNumBottomLevelInstances, FrameCount);

    D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS buildFlags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE;
    for (auto& bottomLevelASGeometryPair : m_bottomLevelASGeometries)
    {
        auto& bottomLevelASGeometry = bottomLevelASGeometryPair.second;
        bool updateOnBuild = false;

        if (bottomLevelASGeometry.GetName().find(L"Grass Patch LOD") != wstring::npos)
        {
            updateOnBuild = true;
        }
        m_accelerationStructure->AddBottomLevelAS(device, buildFlags, bottomLevelASGeometry, updateOnBuild, updateOnBuild);
    }
}
//지오메트리의 정점버퍼 인덱스를 위한 걸명자를 만들어서 넘겨준다.
void Scene::CreateIndexAndVertexBuffers(
    const GeometryDescriptor& desc,
    D3DGeometry* geometry)
{
    auto device = m_deviceResources->GetD3DDevice();
    auto commandList = m_deviceResources->GetCommandList();

    CreateGeometry(device, commandList, m_cbvSrvUavHeap.get(), desc, geometry);
}


void Scene::InitializeScene()
{
    // Setup materials.
    {
        // Albedos
        XMFLOAT4 green = XMFLOAT4(0.1f, 1.0f, 0.5f, 1.0f);
        XMFLOAT4 red = XMFLOAT4(1.0f, 0.5f, 0.5f, 1.0f);
        XMFLOAT4 yellow = XMFLOAT4(1.0f, 1.0f, 0.5f, 1.0f);
    }

    // Setup camera.
    {
        auto& camera = SampleScene::args[SampleScene::Type::Main].camera;
        m_camera.Set(camera.position.eye, camera.position.at, camera.position.up);
        m_cameraController = make_unique<CameraController>(m_camera);
        m_cameraController->SetBoundaries(camera.boundaries.min, camera.boundaries.max);
        m_cameraController->EnableMomentum(false);
        m_prevFrameCamera = m_camera;
    }

    // Setup lights.
    {
        m_lightPosition = XMVectorSet(0.0f, 70.0f, -70.0f, 0);
        m_lightColor = XMFLOAT3(0.8f, 0.53f, 0.27f);
    }
}


void Scene::UpdateAccelerationStructure()
{
    auto commandList = m_deviceResources->GetCommandList();
    auto resourceStateTracker = m_deviceResources->GetGpuResourceStateTracker();
    auto frameIndex = m_deviceResources->GetCurrentFrameIndex();

    resourceStateTracker->FlushResourceBarriers();
    m_accelerationStructure->Build(commandList, m_cbvSrvUavHeap->GetHeap(), frameIndex);

    // Copy previous frame Bottom Level AS instance transforms to GPU. 
    m_prevFrameBottomLevelASInstanceTransforms.CopyStagingToGpu(frameIndex);

    // Update the CPU staging copy with the current frame transforms.
    const auto& bottomLevelASInstanceDescs = m_accelerationStructure->GetBottomLevelASInstancesBuffer();
    for (UINT i = 0; i < bottomLevelASInstanceDescs.NumElements(); i++)
    {
        m_prevFrameBottomLevelASInstanceTransforms[i] = *reinterpret_cast<const XMFLOAT3X4*>(bottomLevelASInstanceDescs[i].Transform);
    }
}

