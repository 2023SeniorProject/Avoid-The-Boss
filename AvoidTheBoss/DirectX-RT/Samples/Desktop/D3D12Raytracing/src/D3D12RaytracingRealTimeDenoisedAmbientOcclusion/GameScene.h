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
#include "Scene.h"

class GameScene : public Scene
{
public:
    GameScene() {}
    ~GameScene() {}

    virtual void InitializeAccelerationStructures();
    virtual void OnUpdate();
    virtual void OnRender();
    virtual void OnKeyDown(UINT8 key);

    // Getters & setters.
    GpuResource(&GrassPatchVB())[UIParameters::NumGrassGeometryLODs][2]{ return m_grassPatchVB; }
private:
    virtual void CreateAuxilaryDeviceResources();

    void GetGrassParameters(GenerateGrassStrawsConstantBuffer_AppParams* params, UINT LOD, float totalTime);
    void GenerateGrassGeometry();
    void LoadSceneGeometry();

    // Build geometry used in the sample.
    virtual void InitializeGeometry();
    void InitializeGrassGeometry();
    virtual void InitializeAllBottomLevelAccelerationStructures();

    virtual void LoadPBRTScene();
    // Grass geometry.
    static const UINT NumGrassPatchesX = 30;
    static const UINT NumGrassPatchesZ = 30;
    static const UINT MaxBLAS = 10 + NumGrassPatchesX * NumGrassPatchesZ;

    GpuKernels::GenerateGrassPatch     m_grassGeometryGenerator;
    UINT                                m_animatedCarInstanceIndex = UINT_MAX;
    UINT                                m_carByTheHouseInstanceIndex = UINT_MAX;
    UINT                                m_spaceshipInstanceIndex = UINT_MAX;
    XMVECTOR                            m_carByTheHousePosition = XMVectorZero();
    XMVECTOR                            m_spaceshipPosition = XMVectorZero();
    float                               m_spaceshipRotationAngleY = 0;
    UINT                                m_grassInstanceIndices[NumGrassPatchesX * NumGrassPatchesZ];
    UINT                                m_currentGrassPatchVBIndex = 0;
    UINT                                m_grassInstanceShaderRecordOffsets[2];
    UINT                                m_prevFrameLODs[NumGrassPatchesX * NumGrassPatchesZ];

    GpuResource m_grassPatchVB[UIParameters::NumGrassGeometryLODs][2];      // Two VBs: current and previous frame.
};

