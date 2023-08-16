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

#define HANGER_MOVE 0.01f
#define HANGER_ANIM_LENGTH 5.0f

#define SHUTTER_MOVE 0.01f
#define SHUTTER_ANIM_LENGTH 6.0f

#define EMERGENCY_MOVE_ANGLE 1.0f
#define EMERGENCY_ANIM_LENGTH 120.0f
class GameScene : public Scene
{
private:
    POINT m_ptOldCursorPos;

    float x = 0.0f;
    float z = 0.0f;
    float yaw = 0.0f;
    float move = 1.3f;
    bool m_bIsMoveForward = false;
    bool m_bIsMoveStrafe = false;
    bool m_bIsRotate = false;

    float HangerX = 0.0f;
    float HangerY = 0.0f;
    float HangerZ = 0.0f;
    float Hangeryaw = 0.0f;
    bool PlusDirection = true;

    float ShutterX = 0.0f;
    float ShutterY = 0.0f;
    float ShutterZ = 0.0f;
    bool PlusDirection2 = true;

    float EmergencyX = -25.23101f;//-25.23101 1.096791 22.00051
    float EmergencyY = 1.096791f;
    float EmergencyZ = 22.00051f;
    float EmergencyYaw = 0.0f;
    bool PlusDirection3 = true;
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

    UINT m_Character1InstanceIndex = UINT_MAX;
    UINT m_LHangerInstanceIndex = UINT_MAX;
    UINT m_RHangerInstanceIndex = UINT_MAX;
    UINT m_LEmergencyInstanceIndex = UINT_MAX;
    UINT m_REmergencyInstanceIndex = UINT_MAX;
    UINT m_LShutterInstanceIndex = UINT_MAX;
    UINT m_RShutterInstanceIndex = UINT_MAX;

    //XMVECTOR m_Character1Position = XMVectorZero();
    //XMVECTOR m_LHangerPosition = XMVectorZero();
    //XMVECTOR m_RHangerPosition = XMVectorZero();
    //XMVECTOR m_LEmergencyPosition = XMVectorZero();
    //XMVECTOR m_REmergencyPosition = XMVectorZero();
    XMVECTOR m_LShutterPosition = XMVectorZero();
    XMVECTOR m_RShutterPosition = XMVectorZero();
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

