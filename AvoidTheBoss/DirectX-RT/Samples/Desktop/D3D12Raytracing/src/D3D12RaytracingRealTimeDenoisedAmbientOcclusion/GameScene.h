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

    POINT m_ptOldCursorPos;

public:
    GameScene() {}
    ~GameScene() {}

    virtual void InitializeAccelerationStructures();
    virtual void LoadPBRTScene();

    virtual void InitializeGeometry();

    virtual void OnUpdate();
    virtual void OnRender();
    virtual void OnKeyDown(UINT8 key);
    virtual void OnLeftButtonDown(UINT x, UINT y);
    virtual void OnLeftButtonUp(UINT x, UINT y);

    UINT                                m_animatedCharacter1InstanceIndex = UINT_MAX;
    UINT                                m_Character1InstanceIndex = UINT_MAX;
    XMVECTOR                            m_Character1Position = XMVectorZero();

};

class CTitleScene : public Scene
{
public:
    CTitleScene() {}
    ~CTitleScene() {}

    virtual void InitializeGeometry();
    virtual void LoadPBRTScene();
    virtual void InitializeAccelerationStructures();
    virtual void OnUpdate();
    virtual void OnRender();
    virtual void OnKeyDown(UINT8 key);
};

class CLobbyScene : public Scene
{
public:
    CLobbyScene() {}
    ~CLobbyScene() {}

    virtual void InitializeAccelerationStructures();
    virtual void OnUpdate();
   
    virtual void OnKeyDown(UINT8 key);
};

class CRoomScene : public Scene
{
public:
    CRoomScene() {}
    ~CRoomScene() {}

    virtual void InitializeAccelerationStructures();
    virtual void OnUpdate();
    //virtual void OnRender();
    virtual void OnKeyDown(UINT8 key);
};

class CResultScene : public Scene
{
public:
    CResultScene() {}
    ~CResultScene() {}

    virtual void InitializeAccelerationStructures();

    virtual void OnUpdate();
    //virtual void OnRender();
    virtual void OnKeyDown(UINT8 key);
};

