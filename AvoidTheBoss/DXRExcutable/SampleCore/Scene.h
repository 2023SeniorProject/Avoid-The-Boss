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

#include "RaytracingSceneDefines.h"
#include "DirectXRaytracingHelper.h"
#include "RaytracingAccelerationStructure.h"
#include "CameraController.h"
#include "PerformanceTimers.h"
#include "Sampler.h"
#include "GpuKernels.h"
#include "EngineTuning.h"
#include "SceneParameters.h"
#include "StepTimer.h"

namespace Scene_Args
{
    extern NumVar CameraRotationDuration;
}

class Scene
{
public:
    // Ctors.
    Scene();

    // Public methods.
    virtual void Setup(std::shared_ptr<DX::DeviceResources> deviceResources, std::shared_ptr<DX::DescriptorHeap> descriptorHeap);
    virtual void InitializeAccelerationStructures(){}
    virtual void OnUpdate(){}
    virtual void OnRender(){}
    virtual void OnKeyDown(UINT8 key){}
    virtual void OnLeftButtonDown(UINT x, UINT y);
    virtual void OnLeftButtonUp(UINT x, UINT y);
    virtual void OnDisplayChanged();
    virtual void OnSizeChanged(UINT width, UINT height, bool minimized);

    const GameCore::Camera& Camera() { return m_camera; }
    const GameCore::Camera& PrevFrameCamera() { return m_prevFrameCamera; }
    const StepTimer& Timer() { return m_timer; }
    const std::map<std::wstring, BottomLevelAccelerationStructureGeometry>& BottomLevelASGeometries() { return m_bottomLevelASGeometries; }
    const std::unique_ptr<RaytracingAccelerationStructureManager>& AccelerationStructure() { return m_accelerationStructure; }
 
    StructuredBuffer<PrimitiveMaterialBuffer>& MaterialBuffer() { return m_materialBuffer; }
    StructuredBuffer<XMFLOAT3X4>& PrevFrameBottomLevelASInstanceTransforms() { return m_prevFrameBottomLevelASInstanceTransforms; }
public:
    void CreateDeviceDependentResources();
    void CreateIndexAndVertexBuffers(const GeometryDescriptor& desc, D3DGeometry* geometry);   
    void InitializeScene();
    void UpdateAccelerationStructure();

    virtual void InitializeGeometry(){}
    virtual void LoadPBRTScene(){}

    virtual void InitializeAllBottomLevelAccelerationStructures();
    std::shared_ptr<DX::DeviceResources> m_deviceResources;
    std::shared_ptr<DX::DescriptorHeap> m_cbvSrvUavHeap;

    // Application state.
    StepTimer m_timer;
    bool m_animateLight = false;
    float lightAnimCount = 0.0f;
    GameCore::Camera m_camera;
    GameCore::Camera m_prevFrameCamera;
    std::unique_ptr<GameCore::CameraController> m_cameraController;
    
    // Geometry.
    UINT m_numTriangles;
    UINT m_numInstancedTriangles;

    std::map<std::wstring, BottomLevelAccelerationStructureGeometry> m_bottomLevelASGeometries;
    std::unique_ptr<RaytracingAccelerationStructureManager> m_accelerationStructure;

    const UINT MaxNumBottomLevelInstances = 1000;

    // Motion Vector resources.
    StructuredBuffer<XMFLOAT3X4> m_prevFrameBottomLevelASInstanceTransforms;        // Bottom-Level AS Instance transforms used for previous frame. Used for Temporal Reprojection.
       
    struct PBRTScene
    {
        std::wstring name;
        std::string path;
    };

    // Materials & textures.
    D3DTexture& EnvironmentMap() { return m_environmentMap; }
    std::vector<PrimitiveMaterialBuffer> m_materials;
    StructuredBuffer<PrimitiveMaterialBuffer> m_materialBuffer;
    D3DTexture m_environmentMap;
    D3DTexture m_nullTexture;

    XMVECTOR m_lightPosition;
    XMFLOAT3 m_lightColor;

    friend class D3D12RaytracingRealTimeDenoisedAmbientOcclusion;
    friend class Pathtracer;
    friend class Composition;
};