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
#include "OtherScene.h"

class GameScene : public OtherScene
{
public:
    GameScene() {}
    ~GameScene() {}

    virtual void InitializeAccelerationStructures();
    virtual void OnUpdate();
    virtual void OnRender();
    virtual void OnKeyDown(UINT8 key);


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
};

