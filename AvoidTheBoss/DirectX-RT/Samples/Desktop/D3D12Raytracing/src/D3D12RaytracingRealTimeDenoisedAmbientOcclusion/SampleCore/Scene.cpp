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

void Scene::CreateDeviceDependentResources()
{
    auto device = m_deviceResources->GetD3DDevice();

    CreateAuxilaryDeviceResources();

    //InitializeGeometry();
    Sample::instance().GetSceneManager()->InitializeGeometry();

    m_prevFrameBottomLevelASInstanceTransforms.Create(device, MaxNumBottomLevelInstances, Sample::FrameCount, L"GPU buffer: Bottom Level AS Instance transforms for previous frame");
}

void Scene::CreateAuxilaryDeviceResources()
{
    auto device = m_deviceResources->GetD3DDevice();
    auto commandQueue = m_deviceResources->GetCommandQueue();

    ResourceUploadBatch resourceUpload(device);
    resourceUpload.Begin();

    m_grassGeometryGenerator.Initialize(device, L"Assets\\wind\\wind2.jpg", m_cbvSrvUavHeap.get(), &resourceUpload, Sample::FrameCount, UIParameters::NumGrassGeometryLODs);

    // Upload the resources to the GPU.
    auto finish = resourceUpload.End(commandQueue);

    // Wait for the upload thread to terminate
    finish.wait();
}

void Scene::GetGrassParameters(GenerateGrassStrawsConstantBuffer_AppParams* params, UINT LOD, float totalTime)
{
    params->activePatchDim = XMUINT2(
        g_UIparameters.GrassGeometryLOD[LOD].NumberStrawsX,
        g_UIparameters.GrassGeometryLOD[LOD].NumberStrawsZ);
    params->maxPatchDim = XMUINT2(MAX_GRASS_STRAWS_1D, MAX_GRASS_STRAWS_1D);

    params->timeOffset = XMFLOAT2(
        totalTime * g_UIparameters.GrassCommon.WindMapSpeedU * g_UIparameters.GrassGeometryLOD[LOD].WindFrequency,
        totalTime * g_UIparameters.GrassCommon.WindMapSpeedV * g_UIparameters.GrassGeometryLOD[LOD].WindFrequency);

    params->grassHeight = g_UIparameters.GrassGeometryLOD[LOD].StrawHeight;
    params->grassScale = g_UIparameters.GrassGeometryLOD[LOD].StrawScale;
    params->bendStrengthAlongTangent = g_UIparameters.GrassGeometryLOD[LOD].BendStrengthSideways;

    params->patchSize = XMFLOAT3(
        g_UIparameters.GrassCommon.PatchWidth,
        g_UIparameters.GrassCommon.PatchHeight,
        g_UIparameters.GrassCommon.PatchWidth);

    params->grassThickness = g_UIparameters.GrassGeometryLOD[LOD].StrawThickness;
    params->windDirection = XMFLOAT3(0, 0, 0);
    params->windStrength = g_UIparameters.GrassGeometryLOD[LOD].WindStrength;
    params->positionJitterStrength = g_UIparameters.GrassGeometryLOD[LOD].RandomPositionJitterStrength;
}


void Scene::LoadSceneGeometry()
{
    LoadPBRTScene();
#if RENDER_GRASS_GEOMETRY
    InitializeGrassGeometry();
#endif
}
void Scene::InitializeGrassGeometry()
{
    auto device = m_deviceResources->GetD3DDevice();
    auto commandList = m_deviceResources->GetCommandList();
    auto commandQueue = m_deviceResources->GetCommandQueue();

    D3DTexture* diffuseTexture = nullptr;
    D3DTexture* normalTexture = &m_nullTexture;

    // Initialize all LOD bottom-level Acceleration Structures for the grass.
    for (UINT i = 0; i < UIParameters::NumGrassGeometryLODs; i++)
    {
        wstring name = L"Grass Patch LOD " + to_wstring(i);
        auto& bottomLevelASGeometry = m_bottomLevelASGeometries[name];
        bottomLevelASGeometry.SetName(name);
        bottomLevelASGeometry.m_indexFormat = StandardIndexFormat;
        bottomLevelASGeometry.m_vertexFormat = DXGI_FORMAT_R32G32B32_FLOAT;
        bottomLevelASGeometry.m_ibStrideInBytes = StandardIndexStride;
        bottomLevelASGeometry.m_vbStrideInBytes = StandardVertexStride;

        // Single patch geometry per bottom-level AS.
        bottomLevelASGeometry.m_geometries.resize(1);
        auto& geometry = bottomLevelASGeometry.m_geometries[0];
        auto& textures = bottomLevelASGeometry.m_textures;

        // Initialize index and vertex buffers.
        {
            const UINT NumStraws = MAX_GRASS_STRAWS_1D * MAX_GRASS_STRAWS_1D;
            const UINT NumTrianglesPerStraw = N_GRASS_TRIANGLES;
            const UINT NumTriangles = NumStraws * NumTrianglesPerStraw;
            const UINT NumVerticesPerStraw = N_GRASS_VERTICES;
            const UINT NumVertices = NumStraws * NumVerticesPerStraw;
            const UINT NumIndicesPerStraw = NumTrianglesPerStraw * 3;
            const UINT NumIndices = NumStraws * NumIndicesPerStraw;
            UINT strawIndices[NumIndicesPerStraw] = { 0, 2, 1, 1, 2, 3, 2, 4, 3, 3, 4, 5, 4, 6, 5 };
            vector<UINT> indices;
            indices.resize(NumIndices);

            UINT indexID = 0;
            for (UINT i = 0, indexID = 0; i < NumStraws; i++)
            {
                UINT baseVertexID = i * NumVerticesPerStraw;
                for (auto index : strawIndices)
                {
                    indices[indexID++] = baseVertexID + index;
                }
            }
            // Preallocate subsequent descriptor indices for both SRV and UAV groups.
            UINT baseSRVHeapIndex = m_cbvSrvUavHeap->AllocateDescriptorIndices(3);      // 1 IB + 2 VB
            geometry.ib.buffer.heapIndex = baseSRVHeapIndex;
            m_grassPatchVB[i][0].srvDescriptorHeapIndex = baseSRVHeapIndex + 1;
            m_grassPatchVB[i][1].srvDescriptorHeapIndex = baseSRVHeapIndex + 2;

            UINT baseUAVHeapIndex = m_cbvSrvUavHeap->AllocateDescriptorIndices(2);      // 2 VB
            m_grassPatchVB[i][0].uavDescriptorHeapIndex = baseUAVHeapIndex;
            m_grassPatchVB[i][1].uavDescriptorHeapIndex = baseUAVHeapIndex + 1;

            AllocateIndexBuffer(device, NumIndices, sizeof(Index), m_cbvSrvUavHeap.get(), &geometry.ib.buffer, D3D12_RESOURCE_STATE_COPY_DEST);
            UploadDataToBuffer(device, commandList, &indices[0], NumIndices, sizeof(Index), geometry.ib.buffer.resource.Get(), &geometry.ib.upload, D3D12_RESOURCE_STATE_INDEX_BUFFER);

            for (auto& vb : m_grassPatchVB[i])
            {
                AllocateUAVBuffer(device, NumVertices, sizeof(VertexPositionNormalTextureTangent), &vb, DXGI_FORMAT_UNKNOWN, m_cbvSrvUavHeap.get(), D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, L"Vertex Buffer: Grass geometry");
            }

            geometry.vb.buffer.resource = m_grassPatchVB[i][0].resource;
            geometry.vb.buffer.gpuDescriptorHandle = m_grassPatchVB[i][0].gpuDescriptorReadAccess;
            geometry.vb.buffer.heapIndex = m_grassPatchVB[i][0].srvDescriptorHeapIndex;
        }

        // Load textures during initialization of the first LOD.
        if (i == 0)
        {
            ResourceUploadBatch resourceUpload(device);
            resourceUpload.Begin();

            auto LoadTexture = [&](auto** ppOutTexture, const wchar_t* textureFilename)
            {
                D3DTexture texture;
                LoadWICTexture(device, &resourceUpload, textureFilename, m_cbvSrvUavHeap.get(), &texture.resource, &texture.heapIndex, &texture.cpuDescriptorHandle, &texture.gpuDescriptorHandle, false);
                textures.push_back(texture);

                *ppOutTexture = &textures.back();
            };
            LoadTexture(&diffuseTexture, L"assets\\grass\\albedo.png");

            // Upload the resources to the GPU.
            auto finish = resourceUpload.End(commandQueue);

            // Wait for the upload thread to terminate
            finish.wait();
        }
        else
        {
            textures.push_back(*diffuseTexture);
        }

        UINT materialID;
        {
            PrimitiveMaterialBuffer materialCB;

            switch (i)
            {
            case 0: materialCB.Kd = XMFLOAT3(0.25f, 0.75f, 0.25f); break;
            case 1: materialCB.Kd = XMFLOAT3(0.5f, 0.75f, 0.5f); break;
            case 2: materialCB.Kd = XMFLOAT3(0.25f, 0.5f, 0.5f); break;
            case 3: materialCB.Kd = XMFLOAT3(0.5f, 0.5f, 0.75f); break;
            case 4: materialCB.Kd = XMFLOAT3(0.75f, 0.25f, 0.75f); break;
            }

            materialCB.Ks = XMFLOAT3(0, 0, 0);
            materialCB.Kr = XMFLOAT3(0, 0, 0);
            materialCB.Kt = XMFLOAT3(0, 0, 0);
            materialCB.opacity = XMFLOAT3(1, 1, 1);
            materialCB.eta = XMFLOAT3(1, 1, 1);
            materialCB.roughness = 0.1f;
            materialCB.hasDiffuseTexture = true;
            materialCB.hasNormalTexture = false;
            materialCB.hasPerVertexTangents = false;
            materialCB.type = MaterialType::Matte;

            materialID = static_cast<UINT>(m_materials.size());
            m_materials.push_back(materialCB);
        }


        // Create geometry instance.
        bool isVertexAnimated = true;
        bottomLevelASGeometry.m_geometryInstances.push_back(GeometryInstance(geometry, materialID, diffuseTexture->gpuDescriptorHandle, normalTexture->gpuDescriptorHandle, D3D12_RAYTRACING_GEOMETRY_FLAG_OPAQUE, isVertexAnimated));

        bottomLevelASGeometry.m_numTriangles = bottomLevelASGeometry.m_geometryInstances[0].ib.count / 3;
    }

    ZeroMemory(m_prevFrameLODs, ARRAYSIZE(m_prevFrameLODs) * sizeof(m_prevFrameLODs[0]));
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
        m_lightPosition = XMVectorSet(-0.0f, 60.0f, 20.0f, 0);
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

void Scene::InitializeGeometry()
{
    auto device = m_deviceResources->GetD3DDevice();
    auto commandList = m_deviceResources->GetCommandList();

    // Create a null SRV for geometries with no diffuse texture.
    // Null descriptors are needed in order to achieve the effect of an "unbound" resource.
    {
        D3D12_SHADER_RESOURCE_VIEW_DESC nullSrvDesc = {};
        nullSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        nullSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        nullSrvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        nullSrvDesc.Texture2D.MipLevels = 1;
        nullSrvDesc.Texture2D.MostDetailedMip = 0;
        nullSrvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

        m_nullTexture.heapIndex = m_cbvSrvUavHeap->AllocateDescriptor(&m_nullTexture.cpuDescriptorHandle, m_nullTexture.heapIndex);
        device->CreateShaderResourceView(nullptr, &nullSrvDesc, m_nullTexture.cpuDescriptorHandle);
        m_nullTexture.gpuDescriptorHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(m_cbvSrvUavHeap->GetHeap()->GetGPUDescriptorHandleForHeapStart(),
            m_nullTexture.heapIndex, m_cbvSrvUavHeap->DescriptorSize());
    }


    // Begin frame.
    m_deviceResources->ResetCommandAllocatorAndCommandlist();

    LoadSceneGeometry();
    InitializeAllBottomLevelAccelerationStructures();

    m_materialBuffer.Create(device, static_cast<UINT>(m_materials.size()), 1, L"Structured buffer: materials");
    copy(m_materials.begin(), m_materials.end(), m_materialBuffer.begin());

    // 바닥 텍스쳐 로드
    LoadDDSTexture(device, commandList, L"Assets\\Textures\\FlowerRoad\\flower_road_8khdri_1kcubemap.BC7.dds", m_cbvSrvUavHeap.get(), &m_environmentMap, D3D12_SRV_DIMENSION_TEXTURECUBE);

    m_materialBuffer.CopyStagingToGpu();
    m_deviceResources->ExecuteCommandList();
}
