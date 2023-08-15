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
        L"Door",
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

        if (bottomLevelASname.find(L"Car") != wstring::npos)
        {
            m_carByTheHouseInstanceIndex = instanceIndex;
        }
        else if (bottomLevelASname.find(L"Spaceship") != wstring::npos)
        {
            m_spaceshipInstanceIndex = instanceIndex;
        }
    }
    // 애니메이션 처리
#if !LOAD_ONLY_ONE_PBRT_MESH
    // Add one more instace of a Car BLAS for an animated car moving in circle.
    m_animatedCarInstanceIndex = m_accelerationStructure->AddBottomLevelASInstance(L"Car", UINT_MAX, XMMatrixIdentity());
#endif
    m_animatedCharacter1InstanceIndex = m_accelerationStructure->AddBottomLevelASInstance(L"Character1", UINT_MAX, XMMatrixIdentity());
#if RENDER_GRASS_GEOMETRY
    // ` up grass patches.
    UINT grassInstanceIndex = 0;
    for (int i = 0; i < NumGrassPatchesZ; i++)
        for (int j = 0; j < NumGrassPatchesX; j++)
        {
            int z = i - 15;
            int x = j - 15;

            if ((x < -1 || x > 2 || z < -2 || z > 1) &&
                (IsInRange(x, -2, 3) && IsInRange(z, -3, 2)))

            {
                m_grassInstanceIndices[grassInstanceIndex] = m_accelerationStructure->AddBottomLevelASInstance(L"Grass Patch LOD 0", UINT_MAX, XMMatrixIdentity());
                grassInstanceIndex++;
            }
        }
#endif
    // Initialize the top-level AS.
    D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS buildFlags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE;
    bool allowUpdate = false;
    bool performUpdateOnBuild = false;
    m_accelerationStructure->InitializeTopLevelAS(device, buildFlags, allowUpdate, performUpdateOnBuild, L"Top-Level Acceleration Structure");
    // Build acceleration structure needed for raytracing.
    // Requires: BLASes to have their instanceContributionToHitGroupIndex already initialized. 
}
void GameScene::GenerateGrassGeometry()
{
    auto commandList = m_deviceResources->GetCommandList();
    auto resourceStateTracker = m_deviceResources->GetGpuResourceStateTracker();
    float totalTime = Scene_Args::AnimateScene && Scene_Args::AnimateGrass ? static_cast<float>(m_timer.GetTotalSeconds()) : 0;

    m_currentGrassPatchVBIndex = (m_currentGrassPatchVBIndex + 1) % 2;
    ScopedTimer _prof(L"Generate Grass Geometry", commandList);

    // Update all LODs.
    for (UINT i = 0; i < UIParameters::NumGrassGeometryLODs; i++)
    {
        GenerateGrassStrawsConstantBuffer_AppParams params;
        GetGrassParameters(&params, i, totalTime);

        UINT vbID = m_currentGrassPatchVBIndex & 1;
        auto& grassPatchVB = m_grassPatchVB[i][vbID];

        // Transition output vertex buffer to UAV state and make sure the resource is done being read from.      
        {
            resourceStateTracker->TransitionResource(&grassPatchVB, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
            resourceStateTracker->InsertUAVBarrier(&grassPatchVB);
        }

        resourceStateTracker->FlushResourceBarriers();
        m_grassGeometryGenerator.Run(commandList, params, m_cbvSrvUavHeap->GetHeap(), grassPatchVB.gpuDescriptorWriteAccess);

        // Transition the output vertex buffer to VB state and make sure the CS is done writing.        
        {
            resourceStateTracker->TransitionResource(&grassPatchVB, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
            resourceStateTracker->InsertUAVBarrier(&grassPatchVB);
        }

        // Point bottom-levelAS VB pointer to the updated VB.
        auto& bottomLevelAS = m_accelerationStructure->GetBottomLevelAS(L"Grass Patch LOD " + to_wstring(i));
        auto& geometryDesc = bottomLevelAS.GetGeometryDescs()[0];
        geometryDesc.Triangles.VertexBuffer.StartAddress = grassPatchVB.resource->GetGPUVirtualAddress();
        bottomLevelAS.SetDirty(true);
    }

    // Update bottom-level AS instances.
    {
        // Enumerate all hit contribution indices for grass bottom-level acceleration structures.
        BottomLevelAccelerationStructure* grassBottomLevelAS[UIParameters::NumGrassGeometryLODs];

        for (UINT i = 0; i < UIParameters::NumGrassGeometryLODs; i++)
        {
            grassBottomLevelAS[i] = &m_accelerationStructure->GetBottomLevelAS(L"Grass Patch LOD " + to_wstring(i));
        }


        std::mt19937 m_generatorURNG;  // Uniform random number generator
        m_generatorURNG.seed(1729);
        uniform_real_distribution<float> unitSquareDistributionInclusive(0.f, nextafter(1.f, FLT_MAX));
        function<float()> GetRandomFloat01inclusive = bind(unitSquareDistributionInclusive, ref(m_generatorURNG));

        XMVECTOR baseIndex = XMVectorSet(0, 0, 2, 0);
        XMVECTOR patchOffset = XMLoadFloat3(&g_UIparameters.GrassCommon.PatchOffset);
        float width = g_UIparameters.GrassCommon.PatchWidth;

        UINT grassInstanceIndex = 0;
        for (int i = 0; i < NumGrassPatchesZ; i++)
            for (int j = 0; j < NumGrassPatchesX; j++)
            {
                int z = i - 15;
                int x = j - 15;

                if ((x < -1 || x > 2 || z < -2 || z > 1) &&
                    (IsInRange(x, -2, 3) && IsInRange(z, -3, 2)))

                {
                    auto& BLASinstance = m_accelerationStructure->GetBottomLevelASInstance(m_grassInstanceIndices[grassInstanceIndex]);

                    float jitterX = 2 * GetRandomFloat01inclusive() - 1;
                    float jitterZ = 2 * GetRandomFloat01inclusive() - 1;
                    XMVECTOR position = patchOffset + width * (baseIndex + XMVectorSet(static_cast<float>(x), 0, static_cast<float>(z), 0) + 0.01f * XMVectorSet(jitterX, 0, jitterZ, 0));
                    XMMATRIX transform = XMMatrixTranslationFromVector(position);
                    BLASinstance.SetTransform(transform);

                    // Find the LOD for this instance based on the distance from the camera.
                    XMVECTOR centerPosition = position + XMVectorSet(0.5f * width, 0, 0.5f * width, 0);
                    float approxDistanceToCamera = max(0.f, XMVectorGetX(XMVector3Length((centerPosition - m_camera.Eye()))) - 0.5f * width);
                    UINT LOD = UIParameters::NumGrassGeometryLODs - 1;
                    if (!g_UIparameters.GrassCommon.ForceLOD0)
                    {
                        for (UINT i = 0; i < UIParameters::NumGrassGeometryLODs - 1; i++)
                        {
                            if (approxDistanceToCamera <= g_UIparameters.GrassGeometryLOD[i].MaxLODdistance)
                            {
                                LOD = i;
                                break;
                            }
                        }
                    }
                    else
                    {
                        LOD = 0;
                    }

                    auto GetShaderRecordIndexOffset = [&](UINT* outShaderRecordIndexOffset, UINT instanceIndex, UINT LOD, UINT prevFrameLOD)
                    {
                        UINT baseShaderRecordID = grassBottomLevelAS[LOD]->GetInstanceContributionToHitGroupIndex();

                        UINT NumTransitionTypes = 3;
                        UINT transitionType;
                        if (LOD > prevFrameLOD) transitionType = 0;
                        else if (LOD == prevFrameLOD) transitionType = 1;
                        else transitionType = 2;
                        UINT NumShaderRecordsPerHitGroup = PathtracerRayType::Count;

                        *outShaderRecordIndexOffset = baseShaderRecordID + (m_currentGrassPatchVBIndex * NumTransitionTypes + transitionType) * NumShaderRecordsPerHitGroup;
                    };

                    UINT shaderRecordIndexOffset;
                    GetShaderRecordIndexOffset(&shaderRecordIndexOffset, grassInstanceIndex, LOD, m_prevFrameLODs[grassInstanceIndex]);

                    // Point the instance at BLAS at the LOD.
                    BLASinstance.InstanceContributionToHitGroupIndex = shaderRecordIndexOffset;
                    BLASinstance.AccelerationStructure = grassBottomLevelAS[LOD]->GetResource()->GetGPUVirtualAddress();

                    m_prevFrameLODs[grassInstanceIndex] = LOD;
                    grassInstanceIndex++;
                }
            }
    }


}
//조명,카메라,자동차 애니메이션 의 변환 계산
void GameScene::OnUpdate()
{
    m_timer.Tick();
    float elapsedTime = static_cast<float>(m_timer.GetElapsedSeconds());
    // 카메라 Update
    m_prevFrameCamera = m_camera;
    if (!m_isCameraFrozen)
    {
        m_cameraController->Update(elapsedTime);
    }
    // 씬 Update
    if (Scene_Args::AnimateScene)
    {
        float animationDuration = 180.0f;
        float t = static_cast<float>(m_timer.GetTotalSeconds());
        float rotAngle1 = XMConvertToRadians(t * 360.0f / animationDuration);
        float rotAngle2 = XMConvertToRadians((t + 12) * 360.0f / animationDuration);
        float rotAngle3 = XMConvertToRadians((t + 24) * 360.0f / animationDuration);

#if !LOAD_ONLY_ONE_PBRT_MESH
     // 자동차 애니메이션
        {
            float radius = 64;
            XMMATRIX mTranslationSceneCenter = XMMatrixTranslation(-7, 0, 7);
            XMMATRIX mTranslation = XMMatrixTranslation(0, 0, radius);

            float lapSeconds = 50;
            float angleToRotateBy = 360.0f * (-t) / lapSeconds;
            XMMATRIX mRotateSceneCenter = XMMatrixRotationY(XMConvertToRadians(-20));
            XMMATRIX mRotate = XMMatrixRotationY(XMConvertToRadians(angleToRotateBy));
            float scale = 1;
            XMMATRIX mScale = XMMatrixScaling(scale, scale, scale);
            XMMATRIX mTransform = mScale * mRotateSceneCenter * mTranslation * mRotate * mTranslationSceneCenter;

            m_accelerationStructure->GetBottomLevelASInstance(m_animatedCarInstanceIndex).SetTransform(mTransform);
        }
#endif
        {

            if (GameInput::IsPressed(GameInput::kKey_w))
            {
                z += move * elapsedTime;
                m_bIsMoveForward = true;
            }
            if (GameInput::IsPressed(GameInput::kKey_s))
            {
                z -= move * elapsedTime;
                m_bIsMoveForward = true;
            }
            if (GameInput::IsPressed(GameInput::kKey_d))
            {
                x += move * elapsedTime;
                m_bIsMoveStrafe = true;
            }

            if (GameInput::IsPressed(GameInput::kKey_a))
            {
                x -= move * elapsedTime;
                m_bIsMoveStrafe = true;
            }
            float cxDelta = 0.0f, cyDelta = 0.0f;
            POINT ptCursorPos;
           if (GameInput::IsPressed(GameInput::kMouse0))
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

                m_accelerationStructure->GetBottomLevelASInstance(m_animatedCharacter1InstanceIndex).SetTransform(mTransform);
            }
            else
            {
                m_bIsMoveForward = false;
                m_bIsMoveStrafe = false;
                m_bIsRotate = false;
            }
        }
    }


     //Rotate the camera around Y axis. 
    if (m_animateCamera)
    {
        float secondsToRotateAround = Scene_Args::CameraRotationDuration;
        float angleToRotateBy = 360.0f * (elapsedTime / secondsToRotateAround);
        XMMATRIX axisCenter = XMMatrixTranslation(5.87519f, 0, 8.52134f);
        XMMATRIX rotate = XMMatrixRotationY(XMConvertToRadians(angleToRotateBy));
    
        XMVECTOR eye = m_camera.Eye();
        XMVECTOR at = m_camera.At();
        XMVECTOR up = m_camera.Up();
        at = XMVector3TransformCoord(at, rotate);
        eye = XMVector3TransformCoord(eye, rotate);
        up = XMVector3TransformNormal(up, rotate);
        m_camera.Set(eye, at, up);
    }

    // Rotate the second light around Y axis. 
    // 조명 회전
    if (m_animateLight)
    {
        float secondsToRotateAround = 8.0f;
        float angleToRotateBy = -360.0f/10 * (elapsedTime / secondsToRotateAround);
        XMMATRIX rotate = XMMatrixRotationY(XMConvertToRadians(angleToRotateBy));
        XMVECTOR prevLightPosition = m_lightPosition;

        m_lightPosition = XMVector3Transform(m_lightPosition, rotate);
    }
}

void GameScene::OnRender()
{
#if RENDER_GRASS_GEOMETRY
    GenerateGrassGeometry();
#endif
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

    if (m_carByTheHouseInstanceIndex != UINT_MAX)
    {
        m_carByTheHousePosition = XMVectorClamp(m_carByTheHousePosition, XMVectorSet(-9, 0, 0, 0), XMVectorZero());
        XMMATRIX transform = XMMatrixTranslationFromVector(m_carByTheHousePosition);
        m_accelerationStructure->GetBottomLevelASInstance(m_carByTheHouseInstanceIndex).SetTransform(transform);
    }

    if (m_Character1InstanceIndex != UINT_MAX)
    {
        m_Character1Position = XMVectorClamp(m_Character1Position, XMVectorSet(0, 0, 0, 0), XMVectorZero());
        XMMATRIX transform = XMMatrixTranslationFromVector(m_Character1Position);
        m_accelerationStructure->GetBottomLevelASInstance(m_Character1InstanceIndex).SetTransform(transform);
    }
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

void GameScene::LoadPBRTScene()
{
    auto device = m_deviceResources->GetD3DDevice();
    auto commandList = m_deviceResources->GetCommandList();
    auto commandQueue = m_deviceResources->GetCommandQueue();
    auto commandAllocator = m_deviceResources->GetCommandAllocator();

    PBRTScene pbrtSceneDefinitions[] = {
        {L"Spaceship", "Assets\\spaceship\\scene.pbrt"}, //Physically Based Rendering Toolkit
        {L"GroundPlane", "Assets\\groundplane\\scene.pbrt"},
        {L"Boss", "Assets\\boss\\Boss.pbrt"},
        {L"Character1", "Assets\\character1\\Character1.pbrt"},
        {L"Character2", "Assets\\character2\\Character2.pbrt"},
        {L"Character3", "Assets\\character3\\Character3.pbrt"},
        {L"Door", "Assets\\door\\scene.pbrt"},
#if !LOAD_ONLY_ONE_PBRT_MESH 
        {L"Car", "Assets\\car\\scene.pbrt"},
        {L"Dragon", "Assets\\dragon\\scene.pbrt"},
        {L"House", "Assets\\house\\scene.pbrt"},
        {L"MirrorQuad", "Assets\\mirrorquad\\scene.pbrt"},
        {L"Quad", "Assets\\quad\\scene.pbrt"},
#endif
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

    // Initialize the bottom-level AS instances, one for each BLAS.
  /*  for (auto& bottomLevelASname : bottomLevelASnames)
    {
        UINT instanceIndex = m_accelerationStructure->AddBottomLevelASInstance(bottomLevelASname);

        if (bottomLevelASname.find(L"Character1") != wstring::npos)
        {
            m_Character1InstanceIndex = instanceIndex;
        }
        if (bottomLevelASname.find(L"Car") != wstring::npos)
        {
            m_carByTheHouseInstanceIndex = instanceIndex;
        }
        else if (bottomLevelASname.find(L"Spaceship") != wstring::npos)
        {
            m_spaceshipInstanceIndex = instanceIndex;
        }*/
    //}
    // 애니메이션 처리
//#if !LOAD_ONLY_ONE_PBRT_MESH
//    // Add one more instace of a Car BLAS for an animated car moving in circle.
//    m_animatedCarInstanceIndex = m_accelerationStructure->AddBottomLevelASInstance(L"Car", UINT_MAX, XMMatrixIdentity());
//#endif
//    m_animatedCharacter1InstanceIndex = m_accelerationStructure->AddBottomLevelASInstance(L"Character1", UINT_MAX, XMMatrixIdentity());

//#if RENDER_GRASS_GEOMETRY
//    // Set up grass patches.
//    UINT grassInstanceIndex = 0;
//    for (int i = 0; i < NumGrassPatchesZ; i++)
//        for (int j = 0; j < NumGrassPatchesX; j++)
//        {
//            int z = i - 15;
//            int x = j - 15;
//
//            if ((x < -1 || x > 2 || z < -2 || z > 1) &&
//                (IsInRange(x, -2, 3) && IsInRange(z, -3, 2)))
//
//            {
//                m_grassInstanceIndices[grassInstanceIndex] = m_accelerationStructure->AddBottomLevelASInstance(L"Grass Patch LOD 0", UINT_MAX, XMMatrixIdentity());
//                grassInstanceIndex++;
//            }
//        }
//#endif
    // Initialize the top-level AS.
    D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS buildFlags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE;
    bool allowUpdate = false;
    bool performUpdateOnBuild = false;
    m_accelerationStructure->InitializeTopLevelAS(device, buildFlags, allowUpdate, performUpdateOnBuild, L"Top-Level Acceleration Structure");
    // Build acceleration structure needed for raytracing.
    // Requires: BLASes to have their instanceContributionToHitGroupIndex already initialized. 
}

void CTitleScene::LoadPBRTScene()
{
    auto device = m_deviceResources->GetD3DDevice();
    auto commandList = m_deviceResources->GetCommandList();
    auto commandQueue = m_deviceResources->GetCommandQueue();
    auto commandAllocator = m_deviceResources->GetCommandAllocator();

    PBRTScene pbrtSceneDefinitions[] = {
        {L"Spaceship", "Assets\\spaceship\\scene.pbrt"}, //Physically Based Rendering Toolkit
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

void CTitleScene::OnUpdate()
{
}

void CTitleScene::OnRender()
{
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

void CLobbyScene::LoadPBRTScene()
{
}

void CLobbyScene::OnUpdate()
{
}

void CLobbyScene::OnRender()
{
}

void CLobbyScene::OnKeyDown(UINT8 key)
{
}

void CRoomScene::InitializeAccelerationStructures()
{
}

void CRoomScene::LoadPBRTScene()
{
}

void CRoomScene::OnUpdate()
{
}

void CRoomScene::OnRender()
{
}

void CRoomScene::OnKeyDown(UINT8 key)
{
}

void CResultScene::InitializeAccelerationStructures()
{
}

void CResultScene::LoadPBRTScene()
{
}

void CResultScene::OnUpdate()
{
}

void CResultScene::OnRender()
{
}

void CResultScene::OnKeyDown(UINT8 key)
{
}
