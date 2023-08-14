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
private:
    float x = 0.0f;
    float z = 0.0f;
    float yaw = 0.0f;
    float move = 1.3f;
    bool m_bIsMoveForward = false;
    bool m_bIsMoveStrafe = false;

    bool m_bIsRotate = false;

public:
    GameScene() {}
    ~GameScene() {}

    virtual void InitializeAccelerationStructures();

    void GenerateGrassGeometry();

    virtual void OnUpdate();
    virtual void OnRender();
    virtual void OnKeyDown(UINT8 key);

    virtual void LoadPBRTScene();
};

class CTitleScene : public Scene
{
public:
    CTitleScene() {}
    ~CTitleScene() {}

    virtual void InitializeAccelerationStructures();
    virtual void LoadPBRTScene();

    virtual void OnUpdate();
    //virtual void OnRender();
    virtual void OnKeyDown(UINT8 key);
};

class CLobbyScene : public Scene
{
public:
    CLobbyScene() {}
    ~CLobbyScene() {}

    virtual void InitializeAccelerationStructures();
    virtual void LoadPBRTScene();

    virtual void OnUpdate();
    virtual void OnRender();
    virtual void OnKeyDown(UINT8 key);
};

class CRoomScene : public Scene
{
public:
    CRoomScene() {}
    ~CRoomScene() {}

    virtual void InitializeAccelerationStructures();
    virtual void LoadPBRTScene();

    virtual void OnUpdate();
    virtual void OnRender();
    virtual void OnKeyDown(UINT8 key);
};

class CResultScene : public Scene
{
public:
    CResultScene() {}
    ~CResultScene() {}

    virtual void InitializeAccelerationStructures();
    virtual void LoadPBRTScene();

    virtual void OnUpdate();
    virtual void OnRender();
    virtual void OnKeyDown(UINT8 key);
};

