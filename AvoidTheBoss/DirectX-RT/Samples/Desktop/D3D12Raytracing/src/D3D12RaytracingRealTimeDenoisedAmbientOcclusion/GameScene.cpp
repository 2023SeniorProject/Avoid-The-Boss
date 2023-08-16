#include "stdafx.h"
#include "GameScene.h"
#include "Scene.h"
#include "GameInput.h"
#include "EngineTuning.h"
#include "EngineProfiling.h"
#include "GpuTimeManager.h"
#include "RaytracingSceneDefines.h"
#include "D3D12RaytracingRealTimeDenoisedAmbientOcclusion.h"

using namespace std;
using namespace DX;
using namespace DirectX;
using namespace SceneEnums;
using namespace GameCore;

namespace Scene_Args
{
    void OnSceneChange(void*)
    {
        Sample::instance().RequestSceneInitialization();
    }

    NumVar CameraRotationDuration(L"Scene/Camera rotation time", 48.f, 1.f, 120.f, 1.f);
    BoolVar AnimateGrass(L"Scene/Animate grass", true);
    BoolVar AnimateScene(L"Scene/Animate scene", true);
}
const D3D12_INPUT_ELEMENT_DESC StandardVertexDescription[] =
{
    { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    { "TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
};
const UINT StandardVertexStride = 44;
const DXGI_FORMAT StandardIndexFormat = DXGI_FORMAT_R32_UINT;
const UINT StandardIndexStride = sizeof(UINT);

void GameScene::InitializeAccelerationStructures()
{
    auto device = m_deviceResources->GetD3DDevice();

    // Initialize bottom-level AS.
    wstring bottomLevelASnames[] = {
        L"Boss",
        L"Character1",
        L"Character2",
        L"Character3",
        L"EmergencyL",
        L"EmergencyR",
        L"HangerL",
        L"HangerR",
        L"SutterR",
        L"SutterSideR",
        L"SutterL",
        L"SutterSideL",
        L"IndustryMap",
    };

    // Initialize the bottom-level AS instances, one for each BLAS.
    for (auto& bottomLevelASname : bottomLevelASnames)
    {
       UINT instanceIndex = m_accelerationStructure->AddBottomLevelASInstance(bottomLevelASname);

        if (bottomLevelASname.find(L"Character1") != wstring::npos)
        {
            m_Character1InstanceIndex = instanceIndex;
        }
        if (bottomLevelASname.find(L"EmergencyL") != wstring::npos)
        {
            m_LEmergencyInstanceIndex = instanceIndex;
        }

        if (bottomLevelASname.find(L"EmergencyR") != wstring::npos)
        {
            m_REmergencyInstanceIndex = instanceIndex;
        }
        if (bottomLevelASname.find(L"HangerL") != wstring::npos)
        {
            m_LHangerInstanceIndex = instanceIndex;
        }
        if (bottomLevelASname.find(L"HangerR") != wstring::npos)
        {
            m_RHangerInstanceIndex = instanceIndex;
        }
        if (bottomLevelASname.find(L"SutterR") != wstring::npos)
        {
            m_RShutterInstanceIndex = instanceIndex;
        }
        if (bottomLevelASname.find(L"SutterL") != wstring::npos)
        {
            m_LShutterInstanceIndex = instanceIndex;
        }
    }
    // 애니메이션 처리
    //m_animatedCharacter1InstanceIndex = m_accelerationStructure->AddBottomLevelASInstance(L"Character1", UINT_MAX, XMMatrixIdentity());
   /* m_animatedLEmergencyInstanceIndex = m_accelerationStructure->AddBottomLevelASInstance(L"EmergencyL", UINT_MAX, XMMatrixIdentity());
    m_animatedREmergencyInstanceIndex = m_accelerationStructure->AddBottomLevelASInstance(L"EmergencyR", UINT_MAX, XMMatrixIdentity());
    m_animatedLHangerInstanceIndex = m_accelerationStructure->AddBottomLevelASInstance(L"HangerL", UINT_MAX, XMMatrixIdentity());
    m_animatedRHangerInstanceIndex = m_accelerationStructure->AddBottomLevelASInstance(L"HangerR", UINT_MAX, XMMatrixIdentity());
    m_animatedLShutterInstanceIndex = m_accelerationStructure->AddBottomLevelASInstance(L"SutterR", UINT_MAX, XMMatrixIdentity());
    m_animatedRShutterInstanceIndex = m_accelerationStructure->AddBottomLevelASInstance(L"SutterL", UINT_MAX, XMMatrixIdentity());*/

    // Initialize the top-level AS.
    D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS buildFlags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE;
    bool allowUpdate = false;
    bool performUpdateOnBuild = false;
    m_accelerationStructure->InitializeTopLevelAS(device, buildFlags, allowUpdate, performUpdateOnBuild, L"Top-Level Acceleration Structure");
}

//조명,카메라,자동차 애니메이션 의 변환 계산
void GameScene::OnUpdate()
{
    m_timer.Tick();
    float elapsedTime = static_cast<float>(m_timer.GetElapsedSeconds());
    // 카메라 Update
    m_prevFrameCamera = m_camera;
    m_cameraController->Update(elapsedTime);
    // 씬 Update
    if (Scene_Args::AnimateScene)
    {
        {
            if (GameInput::IsPressed(GameInput::kKey_up))
            {
                z += move * elapsedTime;
                m_bIsMoveForward = true;
            }
            if (GameInput::IsPressed(GameInput::kKey_down))
            {
                z -= move * elapsedTime;
                m_bIsMoveForward = true;
            }
            if (GameInput::IsPressed(GameInput::kKey_left))
            {
                x += move * elapsedTime;
                m_bIsMoveStrafe = true;
            }

            if (GameInput::IsPressed(GameInput::kKey_right))
            {
                x -= move * elapsedTime;
                m_bIsMoveStrafe = true;
            }
            float cxDelta = 0.0f, cyDelta = 0.0f;
            POINT ptCursorPos;
           if (GameInput::IsPressed(GameInput::kMouse1))
           {
               m_bIsRotate = true;
               ::SetCursor(NULL);
               ::GetCursorPos(&ptCursorPos);
               cxDelta = (float)(ptCursorPos.x - m_ptOldCursorPos.x) / 3.0f;
               cyDelta = (float)(ptCursorPos.y - m_ptOldCursorPos.y) / 3.0f;
               ::SetCursorPos(m_ptOldCursorPos.x, m_ptOldCursorPos.y);
    
               if (cxDelta != 0.0f)
               {
                   yaw += cxDelta;
                   if (yaw > 360.0f) yaw -= 360.0f;
                   if (yaw < 0.0f) yaw += 360.0f;
               }
           }
            XMMATRIX mTranslation = XMMatrixIdentity();
            XMMATRIX mRotate = XMMatrixIdentity();
            //if (m_bIsMoveForward)
            {
                mTranslation = XMMatrixTranslation(-x, 0, -z);
            }
            if (m_bIsRotate)
            {
                mRotate = XMMatrixRotationY(XMConvertToRadians(yaw));
            }

            float scale = 1;
            XMMATRIX mScale = XMMatrixScaling(scale, scale, scale);

            if (m_bIsMoveForward || m_bIsMoveStrafe || m_bIsRotate)
            {
                XMMATRIX mTransform = mScale * mRotate * mTranslation;

                m_accelerationStructure->GetBottomLevelASInstance(m_Character1InstanceIndex).SetTransform(mTransform);
            }
            else
            {
                m_bIsMoveForward = false;
                m_bIsMoveStrafe = false;
                m_bIsRotate = false;
            }
        }
        //if(GameInput::IsPressed(GameInput::kKey_1))
        {
            if (PlusDirection)
            {
                HangerX += HANGER_MOVE;
                if (HangerX >= HANGER_ANIM_LENGTH)
                    PlusDirection = false;
            }
            else
            {
                 HangerX -= HANGER_MOVE;
                 if (HangerX <= 0)
                     PlusDirection = true;
            }
            XMMATRIX mTranslation = XMMatrixIdentity();
            XMMATRIX mRotate = XMMatrixIdentity();    

            mTranslation = XMMatrixTranslation(HangerX, HangerY, HangerZ);
            //mRotate = XMMatrixRotationY(XMConvertToRadians(yaw));

            XMMATRIX mTransform = mRotate * mTranslation;
            m_accelerationStructure->GetBottomLevelASInstance(m_LHangerInstanceIndex).SetTransform(mTransform);

            mTranslation = XMMatrixTranslation(-HangerX, HangerY, HangerZ);
            mTransform = mRotate * mTranslation;
            m_accelerationStructure->GetBottomLevelASInstance(m_RHangerInstanceIndex).SetTransform(mTransform);
        }
        {
            if (PlusDirection2)
            {
                ShutterY += SHUTTER_MOVE;
                if (ShutterY >= HANGER_ANIM_LENGTH)
                    PlusDirection2 = false;
            }
            else
            {
                ShutterY -= SHUTTER_MOVE;
                if (ShutterY <= 0)
                    PlusDirection2 = true;
            }
            XMMATRIX mTranslation = XMMatrixIdentity();
            XMMATRIX mRotate = XMMatrixIdentity();

            mTranslation = XMMatrixTranslation(ShutterX, ShutterY, ShutterZ);

            XMMATRIX mTransform = mRotate * mTranslation;
            m_accelerationStructure->GetBottomLevelASInstance(m_LShutterInstanceIndex).SetTransform(mTransform);
            m_accelerationStructure->GetBottomLevelASInstance(m_RShutterInstanceIndex).SetTransform(mTransform);
        }
        {
            if (PlusDirection3)
            {
                EmergencyYaw += EMERGENCY_MOVE_ANGLE;
                if (EmergencyYaw >= EMERGENCY_ANIM_LENGTH)
                    PlusDirection3 = false;
            }
            else
            {
                EmergencyYaw -= EMERGENCY_MOVE_ANGLE;
                if (EmergencyYaw <= 0)
                    PlusDirection3 = true;
            }
            XMMATRIX mTranslationCenter = XMMatrixIdentity();
            XMMATRIX mTranslation = XMMatrixIdentity();
            XMMATRIX mRotate = XMMatrixIdentity();

            mRotate = XMMatrixRotationY(XMConvertToRadians(-EmergencyYaw));        
            mTranslationCenter = XMMatrixTranslation(EmergencyX+ 0.02588548f,-EmergencyY,-EmergencyZ-0.6920313f);
            mTranslation = XMMatrixTranslation(-EmergencyX- 0.02588548f, EmergencyY, EmergencyZ+ 0.6920313f);
            XMMATRIX mTransform = mTranslationCenter*mRotate * mTranslation;

            m_accelerationStructure->GetBottomLevelASInstance(m_LEmergencyInstanceIndex).SetTransform(mTransform);

            mRotate = XMMatrixRotationY(XMConvertToRadians(EmergencyYaw));
            mTranslationCenter = XMMatrixTranslation(-EmergencyX+ 0.02588548f, -EmergencyY, -EmergencyZ - 0.6920313f);
            mTranslation = XMMatrixTranslation(EmergencyX- 0.02588548f, EmergencyY, EmergencyZ+ 0.6920313f);
            mTransform = mTranslationCenter * mRotate * mTranslation;

            m_accelerationStructure->GetBottomLevelASInstance(m_REmergencyInstanceIndex).SetTransform(mTransform);
        }
    }
}

void GameScene::OnRender()
{
    UpdateAccelerationStructure();
}

void GameScene::OnKeyDown(UINT8 key)
{
    switch (key)
    {
    case 'L':
        m_animateLight = !m_animateLight;
        break;
    default:
        break;
    }

    //if (m_Character1InstanceIndex != UINT_MAX)
    //{
    //    m_Character1Position = XMVectorClamp(m_Character1Position, XMVectorSet(0, 0, 0, 0), XMVectorZero());
    //    XMMATRIX transform = XMMatrixTranslationFromVector(m_Character1Position);
    //    m_accelerationStructure->GetBottomLevelASInstance(m_Character1InstanceIndex).SetTransform(transform);
    //}
   /* if (m_LHangerInstanceIndex != UINT_MAX)
    {
        m_LHangerPosition = XMVectorClamp(m_LHangerPosition, XMVectorSet(0, 0, 0, 0), XMVectorZero());
        XMMATRIX transform = XMMatrixTranslationFromVector(m_LHangerPosition);
        m_accelerationStructure->GetBottomLevelASInstance(m_LHangerInstanceIndex).SetTransform(transform);
    }
    if (m_RHangerInstanceIndex != UINT_MAX)
    {
        m_RHangerPosition = XMVectorClamp(m_RHangerPosition, XMVectorSet(0, 0, 0, 0), XMVectorZero());
        XMMATRIX transform = XMMatrixTranslationFromVector(m_RHangerPosition);
        m_accelerationStructure->GetBottomLevelASInstance(m_RHangerInstanceIndex).SetTransform(transform);
    }
    if (m_LEmergencyInstanceIndex != UINT_MAX)
    {
        m_LEmergencyPosition = XMVectorClamp(m_LEmergencyPosition, XMVectorSet(0, 0, 0, 0), XMVectorZero());
        XMMATRIX transform = XMMatrixTranslationFromVector(m_LEmergencyPosition);
        m_accelerationStructure->GetBottomLevelASInstance(m_LEmergencyInstanceIndex).SetTransform(transform);
    }
    if (m_REmergencyInstanceIndex != UINT_MAX)
    {
        m_REmergencyPosition = XMVectorClamp(m_REmergencyPosition, XMVectorSet(0, 0, 0, 0), XMVectorZero());
        XMMATRIX transform = XMMatrixTranslationFromVector(m_REmergencyPosition);
        m_accelerationStructure->GetBottomLevelASInstance(m_REmergencyInstanceIndex).SetTransform(transform);
    }
    if (m_LShutterInstanceIndex != UINT_MAX)
    {
        m_LShutterPosition = XMVectorClamp(m_LShutterPosition, XMVectorSet(0, 0, 0, 0), XMVectorZero());
        XMMATRIX transform = XMMatrixTranslationFromVector(m_LShutterPosition);
        m_accelerationStructure->GetBottomLevelASInstance(m_LShutterInstanceIndex).SetTransform(transform);
    }
    if (m_RShutterInstanceIndex != UINT_MAX)
    {
        m_RShutterPosition = XMVectorClamp(m_RShutterPosition, XMVectorSet(0, 0, 0, 0), XMVectorZero());
        XMMATRIX transform = XMMatrixTranslationFromVector(m_RShutterPosition);
        m_accelerationStructure->GetBottomLevelASInstance(m_RShutterInstanceIndex).SetTransform(transform);
    }*/
}

void GameScene::OnLeftButtonDown(UINT x, UINT y)
{
    m_ptOldCursorPos.x = x;
    m_ptOldCursorPos.y = y;
    ::GetCursorPos(&m_ptOldCursorPos);
}

void GameScene::OnLeftButtonUp(UINT x, UINT y)
{
    //마우스 캡쳐를 해제한다. 
    ::ReleaseCapture();
}
void GameScene::InitializeGeometry()
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

    Sample::instance().GetSceneManager()->LoadPBRTScene();
    InitializeAllBottomLevelAccelerationStructures();

    m_materialBuffer.Create(device, static_cast<UINT>(m_materials.size()), 1, L"Structured buffer: materials");
    copy(m_materials.begin(), m_materials.end(), m_materialBuffer.begin());

    // 환경맵 로드
    LoadDDSTexture(device, commandList, L"Assets\\Textures\\FlowerRoad\\flower_road_8khdri_1kcubemap.BC7.dds", m_cbvSrvUavHeap.get(), &m_environmentMap, D3D12_SRV_DIMENSION_TEXTURECUBE);

    m_materialBuffer.CopyStagingToGpu();
    m_deviceResources->ExecuteCommandList();
}


void GameScene::LoadPBRTScene()
{
    auto device = m_deviceResources->GetD3DDevice();
    auto commandList = m_deviceResources->GetCommandList();
    auto commandQueue = m_deviceResources->GetCommandQueue();
    auto commandAllocator = m_deviceResources->GetCommandAllocator();

    PBRTScene pbrtSceneDefinitions[] = {
        {L"Boss", "Assets\\boss\\Boss.pbrt"},
        {L"Character1", "Assets\\character1\\Character1.pbrt"},
        {L"Character2", "Assets\\character2\\Character2.pbrt"},
        {L"Character3", "Assets\\character3\\Character3.pbrt"},
        {L"EmergencyL", "Assets\\door\\Emergency_Door_L_low.pbrt"},
        {L"EmergencyR", "Assets\\door\\Emergency_Door_R_low.pbrt"},
        {L"HangerL", "Assets\\door\\Hanger_Door_Left.pbrt"},
        {L"HangerR", "Assets\\door\\Hanger_Door_Right.pbrt"},
        {L"SutterR", "Assets\\door\\Shutter_Door_Right_low.pbrt"},
        {L"SutterSideR", "Assets\\door\\Shutter_Door_Right_Side.pbrt"},
        {L"SutterL", "Assets\\door\\Shutter_Door_Left_low.pbrt"},
        {L"SutterSideL", "Assets\\door\\Shutter_Door_Left_Side.pbrt"},
        {L"IndustryMap", "Assets\\IndustryMap\\scene.pbrt"},
    };

    ResourceUploadBatch resourceUpload(device);
    resourceUpload.Begin();

    bool isVertexAnimated = false;
    for (auto& pbrtSceneDefinition : pbrtSceneDefinitions)
    {
        SceneParser::Scene pbrtScene;
        PBRTParser::PBRTParser().Parse(pbrtSceneDefinition.path, pbrtScene);

        auto& bottomLevelASGeometry = m_bottomLevelASGeometries[pbrtSceneDefinition.name];
        bottomLevelASGeometry.SetName(pbrtSceneDefinition.name);

        bottomLevelASGeometry.m_indexFormat = StandardIndexFormat;
        bottomLevelASGeometry.m_ibStrideInBytes = StandardIndexStride;
        bottomLevelASGeometry.m_vertexFormat = DXGI_FORMAT_R32G32B32_FLOAT;
        bottomLevelASGeometry.m_vbStrideInBytes = StandardVertexStride;

        UINT numGeometries = static_cast<UINT>(pbrtScene.m_Meshes.size());
        auto& geometries = bottomLevelASGeometry.m_geometries;
        geometries.resize(numGeometries);

        auto& textures = bottomLevelASGeometry.m_textures;
        auto& numTriangles = bottomLevelASGeometry.m_numTriangles;

        for (UINT i = 0; i < pbrtScene.m_Meshes.size(); i++)
        {
            auto& mesh = pbrtScene.m_Meshes[i];
            if (mesh.m_VertexBuffer.size() == 0 || mesh.m_IndexBuffer.size() == 0)
            {
                continue;
            }
            vector<VertexPositionNormalTextureTangent> vertexBuffer;
            vector<Index> indexBuffer;
            vertexBuffer.reserve(mesh.m_VertexBuffer.size());
            indexBuffer.reserve(mesh.m_IndexBuffer.size());

            GeometryDescriptor desc;
            desc.ib.count = static_cast<UINT>(mesh.m_IndexBuffer.size());
            desc.vb.count = static_cast<UINT>(mesh.m_VertexBuffer.size());

            for (auto& parseIndex : mesh.m_IndexBuffer)
            {
                Index index = parseIndex;
                indexBuffer.push_back(index);
            }
            desc.ib.indices = indexBuffer.data();

            for (auto& parseVertex : mesh.m_VertexBuffer)
            {
                VertexPositionNormalTextureTangent vertex;

                // Apply the initial transform to VB attributes.
                XMStoreFloat3(&vertex.normal, XMVector3TransformNormal(parseVertex.Normal.GetXMVECTOR(), mesh.m_transform));
                XMStoreFloat3(&vertex.position, XMVector3TransformCoord(parseVertex.Position.GetXMVECTOR(), mesh.m_transform));

                vertex.tangent = parseVertex.Tangent.xmFloat3;
                vertex.textureCoordinate = parseVertex.UV.xmFloat2;
                vertexBuffer.push_back(vertex);
            }
            desc.vb.vertices = vertexBuffer.data();

            auto& geometry = geometries[i];
            CreateIndexAndVertexBuffers(desc, &geometry);

            PrimitiveMaterialBuffer cb;
            cb.Kd = mesh.m_pMaterial->m_Kd.xmFloat3;
            cb.Ks = mesh.m_pMaterial->m_Ks.xmFloat3;
            cb.Kr = mesh.m_pMaterial->m_Kr.xmFloat3;
            cb.Kt = mesh.m_pMaterial->m_Kt.xmFloat3;
            cb.opacity = mesh.m_pMaterial->m_Opacity.xmFloat3;
            cb.eta = mesh.m_pMaterial->m_Eta.xmFloat3;
            cb.roughness = mesh.m_pMaterial->m_Roughness;
            cb.hasDiffuseTexture = !mesh.m_pMaterial->m_DiffuseTextureFilename.empty();
            cb.hasNormalTexture = !mesh.m_pMaterial->m_NormalMapTextureFilename.empty();
            cb.hasPerVertexTangents = true;
            cb.type = mesh.m_pMaterial->m_Type;

            auto LoadPBRTTexture = [&](auto* pOutTextureHandle, auto& textureFilename)
            {
                wstring filename(textureFilename.begin(), textureFilename.end());
                D3DTexture texture;

                if (filename.find(L".dds") != wstring::npos)
                {
                    LoadDDSTexture(device, commandList, filename.c_str(), m_cbvSrvUavHeap.get(), &texture);
                }
                else
                {
                    LoadWICTexture(device, &resourceUpload, filename.c_str(), m_cbvSrvUavHeap.get(), &texture.resource, &texture.heapIndex, &texture.cpuDescriptorHandle, &texture.gpuDescriptorHandle, false);
                }
                textures.push_back(texture);

                *pOutTextureHandle = textures.back().gpuDescriptorHandle;
            };

            D3D12_GPU_DESCRIPTOR_HANDLE diffuseTextureHandle = m_nullTexture.gpuDescriptorHandle;
            if (cb.hasDiffuseTexture)
            {
                LoadPBRTTexture(&diffuseTextureHandle, mesh.m_pMaterial->m_DiffuseTextureFilename);
            }

            D3D12_GPU_DESCRIPTOR_HANDLE normalTextureHandle = m_nullTexture.gpuDescriptorHandle;
            if (cb.hasNormalTexture)
            {
                LoadPBRTTexture(&normalTextureHandle, mesh.m_pMaterial->m_NormalMapTextureFilename);
            }

            UINT materialID = static_cast<UINT>(m_materials.size());
            m_materials.push_back(cb);

            D3D12_RAYTRACING_GEOMETRY_FLAGS geometryFlags;


            if (cb.opacity.x > 0.99f && cb.opacity.y > 0.99f && cb.opacity.z > 0.99f &&
                // AO Rays should reflect of perfect occluders, but that'd be more expensive to trace.
                // WORKAROUND: to prevent perfect mirrors occluding nearby surfaces.
                // Mark fully reflective mirrors as non opaque so that AO rays can skip them 
                // as occluders by ignoring non-opaque geometry on TraceRay.
                !(cb.type == MaterialType::Mirror && cb.Kr.x > 0.99f && cb.Kr.y > 0.99f && cb.Kr.z > 0.99f))
            {
                geometryFlags = D3D12_RAYTRACING_GEOMETRY_FLAG_OPAQUE;
            }
            else
            {
                geometryFlags = D3D12_RAYTRACING_GEOMETRY_FLAG_NONE;
            }

            bottomLevelASGeometry.m_geometryInstances.push_back(GeometryInstance(geometry, materialID, diffuseTextureHandle, normalTextureHandle, geometryFlags, isVertexAnimated));
            numTriangles += desc.ib.count / 3;
        }
    }

    // Upload the resources to the GPU.
    auto finish = resourceUpload.End(commandQueue);

    // Wait for the upload thread to terminate
    finish.wait();
}


//=================================
void CTitleScene::InitializeAccelerationStructures()
{
    auto device = m_deviceResources->GetD3DDevice();
    
    // Initialize bottom-level AS.
    wstring bottomLevelASnames[] = {
        L"GroundPlane"
    };

     for (auto& bottomLevelASname : bottomLevelASnames)
     {
         UINT instanceIndex = m_accelerationStructure->AddBottomLevelASInstance(bottomLevelASname);
     }

    // Initialize the top-level AS.
    D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS buildFlags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE;
    bool allowUpdate = false;
    bool performUpdateOnBuild = false;
    m_accelerationStructure->InitializeTopLevelAS(device, buildFlags, allowUpdate, performUpdateOnBuild, L"Top-Level Acceleration Structure");
}

void CTitleScene::InitializeGeometry()
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

    Sample::instance().GetSceneManager()->LoadPBRTScene();
    InitializeAllBottomLevelAccelerationStructures();

    m_materialBuffer.Create(device, static_cast<UINT>(m_materials.size()), 1, L"Structured buffer: materials");
    copy(m_materials.begin(), m_materials.end(), m_materialBuffer.begin());

    // 환경맵 로드
    //LoadDDSTexture(device, commandList, L"Assets\\Textures\\FlowerRoad\\flower_road_8khdri_1kcubemap.BC7.dds", m_cbvSrvUavHeap.get(), &m_environmentMap, D3D12_SRV_DIMENSION_TEXTURECUBE);

    m_materialBuffer.CopyStagingToGpu();
    m_deviceResources->ExecuteCommandList();
}

void CTitleScene::LoadPBRTScene()
{
    auto device = m_deviceResources->GetD3DDevice();
    auto commandList = m_deviceResources->GetCommandList();
    auto commandQueue = m_deviceResources->GetCommandQueue();
    auto commandAllocator = m_deviceResources->GetCommandAllocator();

    PBRTScene pbrtSceneDefinitions[] = {
        {L"GroundPlane", "Assets\\groundPlane\\scene.pbrt"}, //Physically Based Rendering Toolkit
    };

    ResourceUploadBatch resourceUpload(device);
    resourceUpload.Begin();

    bool isVertexAnimated = false;
    for (auto& pbrtSceneDefinition : pbrtSceneDefinitions)
    {
        SceneParser::Scene pbrtScene;
        PBRTParser::PBRTParser().Parse(pbrtSceneDefinition.path, pbrtScene);

        auto& bottomLevelASGeometry = m_bottomLevelASGeometries[pbrtSceneDefinition.name];
        bottomLevelASGeometry.SetName(pbrtSceneDefinition.name);

        bottomLevelASGeometry.m_indexFormat = StandardIndexFormat;
        bottomLevelASGeometry.m_ibStrideInBytes = StandardIndexStride;
        bottomLevelASGeometry.m_vertexFormat = DXGI_FORMAT_R32G32B32_FLOAT;
        bottomLevelASGeometry.m_vbStrideInBytes = StandardVertexStride;

        UINT numGeometries = static_cast<UINT>(pbrtScene.m_Meshes.size());
        auto& geometries = bottomLevelASGeometry.m_geometries;
        geometries.resize(numGeometries);

        auto& textures = bottomLevelASGeometry.m_textures;
        auto& numTriangles = bottomLevelASGeometry.m_numTriangles;

        for (UINT i = 0; i < pbrtScene.m_Meshes.size(); i++)
        {
            auto& mesh = pbrtScene.m_Meshes[i];
            if (mesh.m_VertexBuffer.size() == 0 || mesh.m_IndexBuffer.size() == 0)
            {
                continue;
            }
            vector<VertexPositionNormalTextureTangent> vertexBuffer;
            vector<Index> indexBuffer;
            vertexBuffer.reserve(mesh.m_VertexBuffer.size());
            indexBuffer.reserve(mesh.m_IndexBuffer.size());

            GeometryDescriptor desc;
            desc.ib.count = static_cast<UINT>(mesh.m_IndexBuffer.size());
            desc.vb.count = static_cast<UINT>(mesh.m_VertexBuffer.size());

            for (auto& parseIndex : mesh.m_IndexBuffer)
            {
                Index index = parseIndex;
                indexBuffer.push_back(index);
            }
            desc.ib.indices = indexBuffer.data();

            for (auto& parseVertex : mesh.m_VertexBuffer)
            {
                VertexPositionNormalTextureTangent vertex;

                // Apply the initial transform to VB attributes.
                XMStoreFloat3(&vertex.normal, XMVector3TransformNormal(parseVertex.Normal.GetXMVECTOR(), mesh.m_transform));
                XMStoreFloat3(&vertex.position, XMVector3TransformCoord(parseVertex.Position.GetXMVECTOR(), mesh.m_transform));

                vertex.tangent = parseVertex.Tangent.xmFloat3;
                vertex.textureCoordinate = parseVertex.UV.xmFloat2;
                vertexBuffer.push_back(vertex);
            }
            desc.vb.vertices = vertexBuffer.data();

            auto& geometry = geometries[i];
            CreateIndexAndVertexBuffers(desc, &geometry);

            PrimitiveMaterialBuffer cb;
            cb.Kd = mesh.m_pMaterial->m_Kd.xmFloat3;
            cb.Ks = mesh.m_pMaterial->m_Ks.xmFloat3;
            cb.Kr = mesh.m_pMaterial->m_Kr.xmFloat3;
            cb.Kt = mesh.m_pMaterial->m_Kt.xmFloat3;
            cb.opacity = mesh.m_pMaterial->m_Opacity.xmFloat3;
            cb.eta = mesh.m_pMaterial->m_Eta.xmFloat3;
            cb.roughness = mesh.m_pMaterial->m_Roughness;
            cb.hasDiffuseTexture = !mesh.m_pMaterial->m_DiffuseTextureFilename.empty();
            cb.hasNormalTexture = !mesh.m_pMaterial->m_NormalMapTextureFilename.empty();
            cb.hasPerVertexTangents = true;
            cb.type = mesh.m_pMaterial->m_Type;

            auto LoadPBRTTexture = [&](auto* pOutTextureHandle, auto& textureFilename)
            {
                wstring filename(textureFilename.begin(), textureFilename.end());
                D3DTexture texture;

                if (filename.find(L".dds") != wstring::npos)
                {
                    LoadDDSTexture(device, commandList, filename.c_str(), m_cbvSrvUavHeap.get(), &texture);
                }
                else
                {
                    LoadWICTexture(device, &resourceUpload, filename.c_str(), m_cbvSrvUavHeap.get(), &texture.resource, &texture.heapIndex, &texture.cpuDescriptorHandle, &texture.gpuDescriptorHandle, false);
                }
                textures.push_back(texture);

                *pOutTextureHandle = textures.back().gpuDescriptorHandle;
            };

            D3D12_GPU_DESCRIPTOR_HANDLE diffuseTextureHandle = m_nullTexture.gpuDescriptorHandle;
            if (cb.hasDiffuseTexture)
            {
                LoadPBRTTexture(&diffuseTextureHandle, mesh.m_pMaterial->m_DiffuseTextureFilename);
            }

            D3D12_GPU_DESCRIPTOR_HANDLE normalTextureHandle = m_nullTexture.gpuDescriptorHandle;
            if (cb.hasNormalTexture)
            {
                LoadPBRTTexture(&normalTextureHandle, mesh.m_pMaterial->m_NormalMapTextureFilename);
            }

            UINT materialID = static_cast<UINT>(m_materials.size());
            m_materials.push_back(cb);

            D3D12_RAYTRACING_GEOMETRY_FLAGS geometryFlags;


            if (cb.opacity.x > 0.99f && cb.opacity.y > 0.99f && cb.opacity.z > 0.99f &&
                // AO Rays should reflect of perfect occluders, but that'd be more expensive to trace.
                // WORKAROUND: to prevent perfect mirrors occluding nearby surfaces.
                // Mark fully reflective mirrors as non opaque so that AO rays can skip them 
                // as occluders by ignoring non-opaque geometry on TraceRay.
                !(cb.type == MaterialType::Mirror && cb.Kr.x > 0.99f && cb.Kr.y > 0.99f && cb.Kr.z > 0.99f))
            {
                geometryFlags = D3D12_RAYTRACING_GEOMETRY_FLAG_OPAQUE;
            }
            else
            {
                geometryFlags = D3D12_RAYTRACING_GEOMETRY_FLAG_NONE;
            }

                                                                          bottomLevelASGeometry.m_geometryInstances.push_back (GeometryInstance(geometry, materialID, diffuseTextureHandle, normalTextureHandle, geometryFlags,   isVertexAnimated));
            numTriangles += desc.ib.count / 3;
        }
    }

    // Upload the resources to the GPU.
    auto finish = resourceUpload.End(commandQueue);

    // Wait for the upload thread to terminate
    finish.wait();

}

void CTitleScene::OnUpdate()
{
    m_timer.Tick();
    float elapsedTime = static_cast<float>(m_timer.GetElapsedSeconds());
    // 카메라 Update
    m_prevFrameCamera = m_camera;
    m_cameraController->Update(elapsedTime);
}

void CTitleScene::OnRender()
{
    UpdateAccelerationStructure();
}

void CTitleScene::OnKeyDown(UINT8 key)
{
    switch (key)
    {
    case 'Q':
        Sample::instance().GetSceneManager()->SetCurSceneIdx(3);
        Sample::instance().m_nCurScene = 3;
        Scene_Args::OnSceneChange(NULL);
        break;
    }
}

void CLobbyScene::InitializeAccelerationStructures()
{
}


void CLobbyScene::OnUpdate()
{
}


void CLobbyScene::OnKeyDown(UINT8 key)
{
}

void CRoomScene::InitializeAccelerationStructures()
{
}

void CRoomScene::OnUpdate()
{
}


void CRoomScene::OnKeyDown(UINT8 key)
{
}

void CResultScene::InitializeAccelerationStructures()
{
}


void CResultScene::OnUpdate()
{
}

void CResultScene::OnKeyDown(UINT8 key)
{
}
