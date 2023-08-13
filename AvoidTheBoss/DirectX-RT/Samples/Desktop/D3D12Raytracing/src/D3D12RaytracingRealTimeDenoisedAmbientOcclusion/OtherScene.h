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

class OtherScene : public Scene
{
public:
    OtherScene() {}
    ~OtherScene() {}

    virtual void OnUpdate();
    virtual void OnRender();
    virtual void OnKeyDown(UINT8 key);

    virtual void LoadPBRTScene();
    virtual void LoadSceneGeometry();
    virtual void InitializeGeometry();

    //필요없는 함수
    virtual void InitializeAccelerationStructures();
    virtual void CreateAuxilaryDeviceResources();
    virtual void InitializeAllBottomLevelAccelerationStructures();
};
class CTitleScene : public OtherScene
{
public:
    CTitleScene() {}
    ~CTitleScene() {}
};

class CLobbyScene : public OtherScene
{
public:
    CLobbyScene() {}
    ~CLobbyScene() {}

 
};

class CRoomScene : public OtherScene
{
public:
    CRoomScene() {}
    ~CRoomScene() {}

  
};

class CResultScene : public OtherScene
{
public:
    CResultScene() {}
    ~CResultScene() {}

  
};
