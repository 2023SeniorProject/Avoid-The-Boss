#include "stdafx.h"
#include "OtherScene.h"
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

void OtherScene::InitializeAccelerationStructures()
{
}

void OtherScene::OnUpdate()
{
}

void OtherScene::OnKeyDown(UINT8 key)
{
    switch (key)
    {
    case 'N':
        break;
    default:
        break;
    }
}


void OtherScene::LoadPBRTScene()
{
}

void CTitleScene::InitializeAccelerationStructures()
{
    auto device = m_deviceResources->GetD3DDevice();

    // Initialize bottom-level AS.
    wstring bottomLevelASnames[] = {
        L"Spaceship",
        L"GroundPlane",
        L"Character1",
#if !LOAD_ONLY_ONE_PBRT_MESH
        L"IndustryMap",
        L"Door",
        L"Dragon",
        L"Car",
        L"House"
#endif
    };

    // Initialize the bottom-level AS instances, one for each BLAS.
    for (auto& bottomLevelASname : bottomLevelASnames)
    {
        UINT instanceIndex = m_accelerationStructure->AddBottomLevelASInstance(bottomLevelASname);

        if (bottomLevelASname.find(L"Car") != wstring::npos)
        {
            m_carByTheHouseInstanceIndex = instanceIndex;
        }
        else if (bottomLevelASname.find(L"Spaceship") != wstring::npos)
        {
            m_spaceshipInstanceIndex = instanceIndex;
        }
    }
    // 局聪皋捞记 贸府
#if !LOAD_ONLY_ONE_PBRT_MESH
    // Add one more instace of a Car BLAS for an animated car moving in circle.
    m_animatedCarInstanceIndex = m_accelerationStructure->AddBottomLevelASInstance(L"Car", UINT_MAX, XMMatrixIdentity());
#endif

#if RENDER_GRASS_GEOMETRY
    // Set up grass patches.
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

void CTitleScene::LoadPBRTScene()
{

    auto device = m_deviceResources->GetD3DDevice();
    auto commandList = m_deviceResources->GetCommandList();
    auto commandQueue = m_deviceResources->GetCommandQueue();
    auto commandAllocator = m_deviceResources->GetCommandAllocator();

    PBRTScene pbrtSceneDefinitions[] = {
        {L"Spaceship", "Assets\\spaceship\\scene.pbrt"}, //Physically Based Rendering Toolkit
        {L"GroundPlane", "Assets\\groundplane\\scene.pbrt"},
        {L"Character1", "Assets\\Character1\\scene.pbrt"},
#if !LOAD_ONLY_ONE_PBRT_MESH 
        {L"IndustryMap", "Assets\\IndustryMap\\scene.pbrt"},
        {L"Door", "Assets\\door\\scene.pbrt"},
        {L"Car", "Assets\\car\\scene.pbrt"},
        {L"Dragon", "Assets\\dragon\\scene.pbrt"},
        {L"House", "Assets\\house\\scene.pbrt"},
        {L"MirrorQuad", "Assets\\mirrorquad\\scene.pbrt"},
        {L"Quad", "Assets\\quad\\scene.pbrt"}
#endif
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


void CTitleScene::OnKeyDown(UINT8 key)
{
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
