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

void OtherScene::OnRender()
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

void OtherScene::LoadSceneGeometry()
{
    LoadPBRTScene();
}

void OtherScene::InitializeGeometry()
{
}

void OtherScene::LoadPBRTScene()
{
}

void OtherScene::CreateAuxilaryDeviceResources()
{
}

void OtherScene::InitializeAllBottomLevelAccelerationStructures()
{
}
