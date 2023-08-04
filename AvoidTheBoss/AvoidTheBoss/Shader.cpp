//-----------------------------------------------------------------------------
// File: Shader.cpp
//-----------------------------------------------------------------------------

#include "pch.h"
#include "Shader.h"
#include "CBullet.h"
#include "CGenerator.h"

CShader::CShader()
{
}

CShader::~CShader()
{
	ReleaseShaderVariables();

	if (m_pd3dPipelineState) m_pd3dPipelineState->Release();
}

D3D12_SHADER_BYTECODE CShader::CreateVertexShader()
{
	D3D12_SHADER_BYTECODE d3dShaderByteCode;
	d3dShaderByteCode.BytecodeLength = 0;
	d3dShaderByteCode.pShaderBytecode = NULL;

	return(d3dShaderByteCode);
}

D3D12_SHADER_BYTECODE CShader::CreatePixelShader()
{
	D3D12_SHADER_BYTECODE d3dShaderByteCode;
	d3dShaderByteCode.BytecodeLength = 0;
	d3dShaderByteCode.pShaderBytecode = NULL;

	return(d3dShaderByteCode);
}

D3D12_SHADER_BYTECODE CShader::CompileShaderFromFile(const WCHAR *pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderProfile, ID3DBlob **ppd3dShaderBlob)
{
	UINT nCompileFlags = 0;
#if defined(_DEBUG)
	nCompileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	ID3DBlob *pd3dErrorBlob = NULL;
	HRESULT hResult = ::D3DCompileFromFile(pszFileName, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, pszShaderName, pszShaderProfile, nCompileFlags, 0, ppd3dShaderBlob, &pd3dErrorBlob);
	char *pErrorString = NULL;
	if (pd3dErrorBlob) pErrorString = (char *)pd3dErrorBlob->GetBufferPointer();

	D3D12_SHADER_BYTECODE d3dShaderByteCode;
	d3dShaderByteCode.BytecodeLength = (*ppd3dShaderBlob)->GetBufferSize();
	d3dShaderByteCode.pShaderBytecode = (*ppd3dShaderBlob)->GetBufferPointer();

	return(d3dShaderByteCode);
}

#define _WITH_WFOPEN
//#define _WITH_STD_STREAM

#ifdef _WITH_STD_STREAM
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#endif

D3D12_SHADER_BYTECODE CShader::ReadCompiledShaderFromFile(WCHAR *pszFileName, ID3DBlob **ppd3dShaderBlob)
{
	UINT nReadBytes = 0;
#ifdef _WITH_WFOPEN
	FILE *pFile = NULL;
	::_wfopen_s(&pFile, pszFileName, L"rb");
	::fseek(pFile, 0, SEEK_END);
	int nFileSize = ::ftell(pFile);
	BYTE *pByteCode = new BYTE[nFileSize];
	::rewind(pFile);
	nReadBytes = (UINT)::fread(pByteCode, sizeof(BYTE), nFileSize, pFile);
	::fclose(pFile);
#endif
#ifdef _WITH_STD_STREAM
	std::ifstream ifsFile;
	ifsFile.open(pszFileName, std::ios::in | std::ios::ate | std::ios::binary);
	nReadBytes = (int)ifsFile.tellg();
	BYTE *pByteCode = new BYTE[*pnReadBytes];
	ifsFile.seekg(0);
	ifsFile.read((char *)pByteCode, nReadBytes);
	ifsFile.close();
#endif

	D3D12_SHADER_BYTECODE d3dShaderByteCode;
	if (ppd3dShaderBlob)
	{
		*ppd3dShaderBlob = NULL;
		HRESULT hResult = D3DCreateBlob(nReadBytes, ppd3dShaderBlob);
		memcpy((*ppd3dShaderBlob)->GetBufferPointer(), pByteCode, nReadBytes);
		d3dShaderByteCode.BytecodeLength = (*ppd3dShaderBlob)->GetBufferSize();
		d3dShaderByteCode.pShaderBytecode = (*ppd3dShaderBlob)->GetBufferPointer();
	}
	else
	{
		d3dShaderByteCode.BytecodeLength = nReadBytes;
		d3dShaderByteCode.pShaderBytecode = pByteCode;
	}

	return(d3dShaderByteCode);
}

D3D12_INPUT_LAYOUT_DESC CShader::CreateInputLayout()
{
	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = NULL;
	d3dInputLayoutDesc.NumElements = 0;

	return(d3dInputLayoutDesc);
}

D3D12_RASTERIZER_DESC CShader::CreateRasterizerState()
{
	D3D12_RASTERIZER_DESC d3dRasterizerDesc;
	::ZeroMemory(&d3dRasterizerDesc, sizeof(D3D12_RASTERIZER_DESC));
	//	d3dRasterizerDesc.FillMode = D3D12_FILL_MODE_WIREFRAME;
	d3dRasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
	d3dRasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	d3dRasterizerDesc.FrontCounterClockwise = FALSE;
	d3dRasterizerDesc.DepthBias = 0;
	d3dRasterizerDesc.DepthBiasClamp = 0.0f;
	d3dRasterizerDesc.SlopeScaledDepthBias = 0.0f;
	d3dRasterizerDesc.DepthClipEnable = TRUE;
	d3dRasterizerDesc.MultisampleEnable = FALSE;
	d3dRasterizerDesc.AntialiasedLineEnable = FALSE;
	d3dRasterizerDesc.ForcedSampleCount = 0;
	d3dRasterizerDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	return(d3dRasterizerDesc);
}

D3D12_DEPTH_STENCIL_DESC CShader::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
	d3dDepthStencilDesc.DepthEnable = TRUE;
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	d3dDepthStencilDesc.StencilEnable = FALSE;
	d3dDepthStencilDesc.StencilReadMask = 0x00;
	d3dDepthStencilDesc.StencilWriteMask = 0x00;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;

	return(d3dDepthStencilDesc);
}

D3D12_BLEND_DESC CShader::CreateBlendState()
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = FALSE;
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return(d3dBlendDesc);
}

void CShader::CreateShader(ID3D12Device5 *pd3dDevice, ID3D12GraphicsCommandList4   *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature)
{
	::ZeroMemory(&m_d3dPipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	m_d3dPipelineStateDesc.pRootSignature = pd3dGraphicsRootSignature;
	m_d3dPipelineStateDesc.VS = CreateVertexShader();
	m_d3dPipelineStateDesc.PS = CreatePixelShader();
	m_d3dPipelineStateDesc.RasterizerState = CreateRasterizerState();
	m_d3dPipelineStateDesc.BlendState = CreateBlendState();
	m_d3dPipelineStateDesc.DepthStencilState = CreateDepthStencilState();
	m_d3dPipelineStateDesc.InputLayout = CreateInputLayout();
	m_d3dPipelineStateDesc.SampleMask = UINT_MAX;
	m_d3dPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	m_d3dPipelineStateDesc.NumRenderTargets = 1;
	m_d3dPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	m_d3dPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	m_d3dPipelineStateDesc.SampleDesc.Count = 1;
	m_d3dPipelineStateDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	HRESULT hResult = pd3dDevice->CreateGraphicsPipelineState(&m_d3dPipelineStateDesc, __uuidof(ID3D12PipelineState), (void **)&m_pd3dPipelineState);

	if (m_pd3dVertexShaderBlob) m_pd3dVertexShaderBlob->Release();
	if (m_pd3dPixelShaderBlob) m_pd3dPixelShaderBlob->Release();

	if (m_d3dPipelineStateDesc.InputLayout.pInputElementDescs) delete[] m_d3dPipelineStateDesc.InputLayout.pInputElementDescs;
}

void CShader::OnPrepareRender(ID3D12GraphicsCommandList4   *pd3dCommandList, int nPipelineState)
{
	if (m_pd3dPipelineState) pd3dCommandList->SetPipelineState(m_pd3dPipelineState);
}

void CShader::Render(ID3D12GraphicsCommandList4   *pd3dCommandList, CCamera *pCamera, bool bRaster)
{
	OnPrepareRender(pd3dCommandList);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CSkyBoxShader::CSkyBoxShader()
{
}

CSkyBoxShader::~CSkyBoxShader()
{
}

D3D12_INPUT_LAYOUT_DESC CSkyBoxShader::CreateInputLayout()
{
	UINT nInputElementDescs = 1;
	D3D12_INPUT_ELEMENT_DESC *pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}

D3D12_DEPTH_STENCIL_DESC CSkyBoxShader::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	d3dDepthStencilDesc.DepthEnable = FALSE;
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dDepthStencilDesc.StencilEnable = FALSE;
	d3dDepthStencilDesc.StencilReadMask = 0xff;
	d3dDepthStencilDesc.StencilWriteMask = 0xff;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_INCR;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_DECR;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;

	return(d3dDepthStencilDesc);
}

D3D12_SHADER_BYTECODE CSkyBoxShader::CreateVertexShader()
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VSSkyBox", "vs_5_1", &m_pd3dVertexShaderBlob));
}

D3D12_SHADER_BYTECODE CSkyBoxShader::CreatePixelShader()
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PSSkyBox", "ps_5_1", &m_pd3dPixelShaderBlob));
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CStandardShader::CStandardShader()
{
}

CStandardShader::~CStandardShader()
{
}

D3D12_INPUT_LAYOUT_DESC CStandardShader::CreateInputLayout()
{
	UINT nInputElementDescs = 5;
	D3D12_INPUT_ELEMENT_DESC *pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[2] = { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 2, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[3] = { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 3, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[4] = { "BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 4, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}

D3D12_SHADER_BYTECODE CStandardShader::CreateVertexShader()
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VSStandard", "vs_5_1", &m_pd3dVertexShaderBlob));
}

D3D12_SHADER_BYTECODE CStandardShader::CreatePixelShader()
{
	
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PSStandard", "ps_5_1", &m_pd3dPixelShaderBlob));
	
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CSkinnedAnimationStandardShader::CSkinnedAnimationStandardShader()
{
}

CSkinnedAnimationStandardShader::~CSkinnedAnimationStandardShader()
{
}

D3D12_INPUT_LAYOUT_DESC CSkinnedAnimationStandardShader::CreateInputLayout()
{
	UINT nInputElementDescs = 7;
	D3D12_INPUT_ELEMENT_DESC *pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[2] = { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 2, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[3] = { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 3, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[4] = { "BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 4, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[5] = { "BONEINDEX", 0, DXGI_FORMAT_R32G32B32A32_SINT, 5, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[6] = { "BONEWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 6, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}

D3D12_SHADER_BYTECODE CSkinnedAnimationStandardShader::CreateVertexShader()
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VSSkinnedAnimationStandard", "vs_5_1", &m_pd3dVertexShaderBlob));
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CStandardObjectsShader::CStandardObjectsShader()
{
}

CStandardObjectsShader::~CStandardObjectsShader()
{
}

void CStandardObjectsShader::BuildObjects(ID3D12Device5 *pd3dDevice, ID3D12GraphicsCommandList4   *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, CLoadedModelInfo *pModel, void *pContext)
{
}

void CStandardObjectsShader::ReleaseObjects()
{
	if (m_ppObjects)
	{
		for (int j = 0; j < m_nObjects; j++) if (m_ppObjects[j]) m_ppObjects[j]->Release();
		delete[] m_ppObjects;
	}
}

void CStandardObjectsShader::AnimateObjects(float fTimeElapsed)
{
	m_fElapsedTime = fTimeElapsed;
}

void CStandardObjectsShader::ReleaseUploadBuffers()
{
	for (int j = 0; j < m_nObjects; j++) if (m_ppObjects[j]) m_ppObjects[j]->ReleaseUploadBuffers();
}

void CStandardObjectsShader::Render(ID3D12GraphicsCommandList4*pd3dCommandList, CCamera *pCamera,bool bRaster)
{
	CStandardShader::Render(pd3dCommandList, pCamera,bRaster);

	for (int j = 0; j < m_nObjects; j++)
	{
		if (m_ppObjects[j])
		{
			m_ppObjects[j]->Animate(m_fElapsedTime);
			m_ppObjects[j]->UpdateTransform(NULL);
			m_ppObjects[j]->Render(pd3dCommandList, pCamera, bRaster);
		}
	}
}

void CStandardObjectsShader::ResetState()
{
	for (int j = 0; j < m_nObjects; j++)
	{
		if (m_ppObjects[j])
		{
			m_ppObjects[j]->m_bEmpExit = false;
			m_ppObjects[j]->ResetState();
		}
	}
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CSkinnedAnimationObjectsShader::CSkinnedAnimationObjectsShader()
{
}

CSkinnedAnimationObjectsShader::~CSkinnedAnimationObjectsShader()
{
}

void CSkinnedAnimationObjectsShader::BuildObjects(ID3D12Device5 *pd3dDevice, ID3D12GraphicsCommandList4 *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, CLoadedModelInfo *pModel, void *pContext)
{
}

void CSkinnedAnimationObjectsShader::ReleaseObjects()
{
	if (m_ppObjects)
	{
		for (int j = 0; j < m_nObjects; j++) if (m_ppObjects[j]) m_ppObjects[j]->Release();
		delete[] m_ppObjects;
	}
}

void CSkinnedAnimationObjectsShader::AnimateObjects(float fTimeElapsed)
{
	m_fElapsedTime = fTimeElapsed;
}

void CSkinnedAnimationObjectsShader::ReleaseUploadBuffers()
{
	for (int j = 0; j < m_nObjects; j++) if (m_ppObjects[j]) m_ppObjects[j]->ReleaseUploadBuffers();
}

void CSkinnedAnimationObjectsShader::Render(ID3D12GraphicsCommandList4*pd3dCommandList, CCamera *pCamera, bool bRaster)
{
	CSkinnedAnimationStandardShader::Render(pd3dCommandList, pCamera, bRaster);

	for (int j = 0; j < m_nObjects; j++)
	{
		if (m_ppObjects[j])
		{
			m_ppObjects[j]->Animate(m_fElapsedTime);
			m_ppObjects[j]->Render(pd3dCommandList, pCamera, bRaster);
		}
	}
}



CMapObjectsShader::CMapObjectsShader()
{
}

CMapObjectsShader::~CMapObjectsShader()
{
}

void CMapObjectsShader::BuildObjects(ID3D12Device5 * pd3dDevice,ID3D12GraphicsCommandList4     * pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CLoadedModelInfo* pModel, void* pContext)
{
	m_nObjects = 3;
	m_ppObjects = new CGameObject * [m_nObjects];

	CGameObject* pMap = CGameObject::LoadGeometryFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Map/Industry_Map(6).bin", this,Layout::MAP);
	m_ppObjects[0] = new CGameObject();
	m_ppObjects[0]->SetChild(pMap ,true);
	m_ppObjects[0]->SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));

	CGameObject* pTile = CGameObject::LoadGeometryFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Map/Industry_Field2(1).bin", this, Layout::MAP);
	m_ppObjects[1] = new CGameObject();
	m_ppObjects[1]->SetChild(pTile ,true);
	m_ppObjects[1]->SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));

	CGameObject* pCrane = CGameObject::LoadGeometryFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Map/Crane.bin", this, Layout::MAP);
	m_ppObjects[2] = new CGameObject();
	m_ppObjects[2]->SetChild(pCrane ,true);
	m_ppObjects[2]->SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));

	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	/*if (pMap) delete pMap;
	if (pTile) delete pTile;
	if (pCrane) delete pCrane;*/

}

CBoundsObjectsShader::CBoundsObjectsShader()
{
}

CBoundsObjectsShader::~CBoundsObjectsShader()
{
}

void CBoundsObjectsShader::BuildObjects(ID3D12Device5* pd3dDevice,ID3D12GraphicsCommandList4     * pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CLoadedModelInfo* pModel, void* pContext)
{
	m_nObjects = 1;
	m_ppObjects = new CGameObject * [m_nObjects];

	CGameObject* pMap = CGameObject::LoadGeometryFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Map_Bounding_Box_(5).bin", this, Layout::BOUDS);
	pMap->m_type = 1;
	m_ppObjects[0] = new CGameObject();
	m_ppObjects[0]->SetChild(pMap ,true);
	m_ppObjects[0]->SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	//if (pMap) delete pMap;

}

CBulletObjectsShader::CBulletObjectsShader()
{
}

CBulletObjectsShader::~CBulletObjectsShader()
{
}

void CBulletObjectsShader::BuildObjects(ID3D12Device5* pd3dDevice,ID3D12GraphicsCommandList4     * pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CLoadedModelInfo* pModel, void* pContext)
{
	m_nObjects = BULLET_NUMBER;
	m_ppObjects = new CGameObject * [m_nObjects];

	CGameObject* pBullet = CGameObject::LoadGeometryFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/ÃÑ¾Ë/Bullet.bin", this, Layout::BULLET);
	for (int i = 0; i < m_nObjects; i++)
	{
		m_ppObjects[i] = new CBullet();
		m_ppObjects[i]->SetChild(pBullet ,true);
		m_ppObjects[i]->SetPosition(XMFLOAT3(0.0f+i, 1.0f, 0.0f));
	}
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	//if (pBullet) delete pBullet;
}

void CBulletObjectsShader::Render(ID3D12GraphicsCommandList4  * pd3dCommandList, CCamera* pCamera,bool bRaster)
{
	CStandardShader::Render(pd3dCommandList, pCamera, bRaster);
	for (int j = 0; j < m_nObjects; j++)
	{
		if (m_ppObjects[j])
		{
			if (m_ppObjects[j])
			{
				m_ppObjects[j]->Animate(m_fElapsedTime);
				m_ppObjects[j]->UpdateTransform(NULL);
				if(((CBullet*)m_ppObjects[j])->GetOnShoot()) m_ppObjects[j]->Render(pd3dCommandList, pCamera,bRaster);
			}
		}
	}
}

CDoorObjectsShader::CDoorObjectsShader()
{
}

CDoorObjectsShader::~CDoorObjectsShader()
{
}

void CDoorObjectsShader::BuildObjects(ID3D12Device5* pd3dDevice, ID3D12GraphicsCommandList4  * pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CLoadedModelInfo* pModel, void* pContext)
{
	m_nObjects = 5;
	m_ppObjects = new CGameObject * [m_nObjects];

	CGameObject* pFrontDoor = CGameObject::LoadGeometryFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Map/Front_Hanger_Door_Open.bin", this, Layout::DOOR);
	CGameObject* pEmergencyDoor = CGameObject::LoadGeometryFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Map/Emergency_Door_Open.bin", this, Layout::DOOR);
	CGameObject* pShutterDoor = CGameObject::LoadGeometryFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Map/Shutter_Door_Side.bin", this, Layout::DOOR);

	m_ppObjects[0] = new CFrontDoor();
	m_ppObjects[0]->SetChild(pFrontDoor ,true);
	m_ppObjects[0]->SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));
	m_ppObjects[0]->OnPrepareAnimate();
	m_ppObjects[0]->objLayer = Layout::DOOR;

	m_ppObjects[1] = new CEmergencyDoor();
	m_ppObjects[1]->SetChild(pEmergencyDoor ,true);
	m_ppObjects[1]->SetPosition(XMFLOAT3(-25.60735f, 0.01800204f, -22.68291f));
	m_ppObjects[1]->Rotate(0.0f, 90.0f, 0.0f);
	m_ppObjects[1]->OnPrepareAnimate();
	m_ppObjects[1]->objLayer = Layout::DOOR;

	m_ppObjects[2] = new CEmergencyDoor();
	m_ppObjects[2]->SetChild(pEmergencyDoor ,true);
	m_ppObjects[2]->SetPosition(XMFLOAT3(25.60001f, 0.01550287f, -21.44026f));
	m_ppObjects[2]->Rotate(0.0f, -90.0f, 0.0f);
	m_ppObjects[2]->OnPrepareAnimate();
	m_ppObjects[2]->objLayer = Layout::DOOR;

	m_ppObjects[3] = new CShutterDoor();
	m_ppObjects[3]->SetChild(pShutterDoor ,true);
	m_ppObjects[3]->SetPosition(XMFLOAT3(-0.044f, -0.5005361f, 0.06f));
	m_ppObjects[3]->Rotate(-90.0f, 0.0f, 90.0f);
	m_ppObjects[3]->OnPrepareAnimate();
	m_ppObjects[3]->objLayer = Layout::DOOR;

	m_ppObjects[4] = new CShutterDoor();
	m_ppObjects[4]->SetChild(pShutterDoor ,true);
	m_ppObjects[4]->SetPosition(XMFLOAT3(50.43907f, -0.503039f, -0.1099938f));
	m_ppObjects[4]->Rotate(-90.0f, 0.0f, 90.0f);
	m_ppObjects[4]->OnPrepareAnimate();
	m_ppObjects[4]->objLayer = Layout::DOOR;

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	/*if (pFrontDoor) delete pFrontDoor;
	if (pEmergencyDoor) delete pEmergencyDoor;
	if (pShutterDoor) delete pShutterDoor;*/

}

CSirenObjectsShader::CSirenObjectsShader()
{
}

CSirenObjectsShader::~CSirenObjectsShader()
{
}

void CSirenObjectsShader::BuildObjects(ID3D12Device5* pd3dDevice, ID3D12GraphicsCommandList4  * pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CLoadedModelInfo* pModel, void* pContext)
{
	m_nObjects = 16;
	m_ppObjects = new CGameObject * [m_nObjects];

	CGameObject* pSiren = CGameObject::LoadGeometryFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Map/Siren_Alarm_One.bin", this, Layout::SIREN);

	m_ppObjects[0] = new CSiren();
	m_ppObjects[0]->SetChild(pSiren ,true);
	m_ppObjects[0]->SetPosition(XMFLOAT3(23.60255f, 3.744244f, 19.36822f));
	m_ppObjects[0]->Rotate(-0.0f, -0.0f, 90.0f);
	m_ppObjects[0]->OnPrepareAnimate();
	m_ppObjects[0]->objLayer = Layout::SIREN;

	m_ppObjects[1] = new CSiren();
	m_ppObjects[1]->SetChild(pSiren , true);
	m_ppObjects[1]->SetPosition(XMFLOAT3(23.60255f, 3.744244f, 4.83103f));
	m_ppObjects[1]->Rotate(-0.0f, -0.0f, 90.0f);
	m_ppObjects[1]->OnPrepareAnimate();
	m_ppObjects[1]->objLayer = Layout::SIREN;

	m_ppObjects[2] = new CSiren();
	m_ppObjects[2]->SetChild(pSiren , true);
	m_ppObjects[2]->SetPosition(XMFLOAT3(23.60255f, 3.744244f,-0.008120117f));
	m_ppObjects[2]->Rotate(-0.0f, -0.0f, 90.0f);
	m_ppObjects[2]->OnPrepareAnimate();
	m_ppObjects[2]->objLayer = Layout::SIREN;

	m_ppObjects[3] = new CSiren();
	m_ppObjects[3]->SetChild(pSiren , true);;
	m_ppObjects[3]->SetPosition(XMFLOAT3(23.60255f, 3.744244f, -14.57508f));
	m_ppObjects[3]->Rotate(0.0f, -0.0f, 90.0f);
	m_ppObjects[3]->OnPrepareAnimate();
	m_ppObjects[3]->objLayer = Layout::SIREN;

	m_ppObjects[4] = new CSiren();
	m_ppObjects[4]->SetChild(pSiren , true);
	m_ppObjects[4]->SetPosition(XMFLOAT3(-23.6237f, 3.744244f, 19.47554f));
	m_ppObjects[4]->Rotate(0.0f, 0.0f, -90.0f);
	m_ppObjects[4]->OnPrepareAnimate();
	m_ppObjects[1]->objLayer = Layout::SIREN;

	m_ppObjects[5] = new CSiren();
	m_ppObjects[5]->SetChild(pSiren , true);
	m_ppObjects[5]->SetPosition(XMFLOAT3(-23.6237f, 3.744244f, 4.938357f));
	m_ppObjects[5]->Rotate(0.0f, 0.0f, -90.0f);;
	m_ppObjects[5]->OnPrepareAnimate();
	m_ppObjects[5]->objLayer = Layout::SIREN;

	m_ppObjects[6] = new CSiren();
	m_ppObjects[6]->SetChild(pSiren , true);
	m_ppObjects[6]->SetPosition(XMFLOAT3(-23.6237f, 3.744244f, 0.09920654f));
	m_ppObjects[6]->Rotate(0.0f, 0.0f, -90.0f);
	m_ppObjects[6]->OnPrepareAnimate();
	m_ppObjects[6]->objLayer = Layout::SIREN;

	m_ppObjects[7] = new CSiren();
	m_ppObjects[7]->SetChild(pSiren , true);
	m_ppObjects[7]->SetPosition(XMFLOAT3(-23.6237f, 3.744244f, -14.46775f));
	m_ppObjects[7]->Rotate(0.0f, 0.0f, -90.0f);
	m_ppObjects[7]->OnPrepareAnimate();
	m_ppObjects[7]->objLayer = Layout::SIREN;

	m_ppObjects[8] = new CSiren();
	m_ppObjects[8]->SetChild(pSiren , true);
	m_ppObjects[8]->SetPosition(XMFLOAT3(-19.46681f, 3.744244f, -23.56555f));
	m_ppObjects[8]->Rotate(90.0f, 0.0f, 0.0f);
	m_ppObjects[8]->OnPrepareAnimate();
	m_ppObjects[8]->objLayer = Layout::SIREN;

	m_ppObjects[9] = new CSiren();
	m_ppObjects[9]->SetChild(pSiren , true);
	m_ppObjects[9]->SetPosition(XMFLOAT3(-4.909707f, 3.744244f, -23.56555f));
	m_ppObjects[9]->Rotate(90.0f, 0.0f, 0.0f);
	m_ppObjects[9]->OnPrepareAnimate();
	m_ppObjects[9]->objLayer = Layout::SIREN;

	m_ppObjects[10] = new CSiren();
	m_ppObjects[10]->SetChild(pSiren , true);
	m_ppObjects[10]->SetPosition(XMFLOAT3(4.786877f, 3.744244f, -23.56555f));
	m_ppObjects[10]->Rotate(90.0f, 0.0f, 0.0f);
	m_ppObjects[10]->OnPrepareAnimate();
	m_ppObjects[10]->objLayer = Layout::SIREN;

	m_ppObjects[11] = new CSiren();
	m_ppObjects[11]->SetChild(pSiren , true);
	m_ppObjects[11]->SetPosition(XMFLOAT3(19.33877f, 3.744244f, -23.56555f));
	m_ppObjects[11]->Rotate(90.0f, 0.0f, 0.0f);
	m_ppObjects[11]->OnPrepareAnimate();
	m_ppObjects[11]->objLayer = Layout::SIREN;

	m_ppObjects[12] = new CSiren();
	m_ppObjects[12]->SetChild(pSiren , true);
	m_ppObjects[12]->SetPosition(XMFLOAT3(4.893795f, 3.744244f, 23.64812f));
	m_ppObjects[12]->Rotate(-90.0f, 0.0f, 0.0f);
	m_ppObjects[12]->OnPrepareAnimate();
	m_ppObjects[12]->objLayer = Layout::SIREN;

	m_ppObjects[13] = new CSiren();
	m_ppObjects[13]->SetChild(pSiren , true);
	m_ppObjects[13]->SetPosition(XMFLOAT3(19.45037f, 3.744244f, 23.64812f));
	m_ppObjects[13]->Rotate(-90.0f, -0.0f, 0.0f);
	m_ppObjects[13]->OnPrepareAnimate();
	m_ppObjects[13]->objLayer = Layout::SIREN;

	m_ppObjects[14] = new CSiren();
	m_ppObjects[14]->SetChild(pSiren , true);
	m_ppObjects[14]->SetPosition(XMFLOAT3(4.897f, 3.721f, 23.64812f));
	m_ppObjects[14]->Rotate(-90.0f, 0.0f, 0.0f);
	m_ppObjects[14]->OnPrepareAnimate();
	m_ppObjects[14]->objLayer = Layout::SIREN;

	m_ppObjects[15] = new CSiren();
	m_ppObjects[15]->SetChild(pSiren , true);
	m_ppObjects[15]->SetPosition(XMFLOAT3(-19.35391f, 3.744244f, 23.64812f));
	m_ppObjects[15]->Rotate(-90.0f, 0.0f, 0.0f);
	m_ppObjects[15]->OnPrepareAnimate();
	m_ppObjects[15]->objLayer = Layout::SIREN;

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	//if (pSiren) delete pSiren;
}

CGeneratorObjectsShader::CGeneratorObjectsShader()
{
}

CGeneratorObjectsShader::~CGeneratorObjectsShader()
{
}

void CGeneratorObjectsShader::BuildObjects(ID3D12Device5* pd3dDevice, ID3D12GraphicsCommandList4  * pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CLoadedModelInfo* pModel, void* pContext)
{
	m_nObjects = 3;
	m_ppObjects = new CGameObject * [m_nObjects];

	CGameObject* pGenerator1 = CGameObject::LoadGeometryFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Map/Generator.bin", this, Layout::GENERATOR);

	CGameObject* pGenerator2 = CGameObject::LoadGeometryFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Map/Generator.bin", this, Layout::GENERATOR);

	CGameObject* pGenerator3 = CGameObject::LoadGeometryFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Map/Generator.bin", this, Layout::GENERATOR);

	m_ppObjects[0] = new CGenerator();
	m_ppObjects[0]->SetChild(pGenerator1 ,true);
	m_ppObjects[0]->SetPosition(XMFLOAT3(-22.884f, 0.0f, 2.46665f));
	m_ppObjects[0]->Rotate(0.0f, 90.0f, 0.0f); 
	m_ppObjects[0]->OnPrepareAnimate();
	m_ppObjects[0]->SetNormalVector();
	m_ppObjects[0]->objLayer = Layout::GENERATOR;

	m_ppObjects[1] = new CGenerator();
	m_ppObjects[1]->SetChild(pGenerator2 ,true);
	m_ppObjects[1]->SetPosition(XMFLOAT3(22.95006f, 0.0f, 2.506552f));
	m_ppObjects[1]->Rotate(0.0f, -90.0f, 0.0f);
	m_ppObjects[1]->OnPrepareAnimate();
	m_ppObjects[1]->SetNormalVector();
	m_ppObjects[1]->objLayer = Layout::GENERATOR;

	m_ppObjects[2] = new CGenerator();
	m_ppObjects[2]->SetChild(pGenerator3 ,true);
	m_ppObjects[2]->SetPosition(XMFLOAT3(0.0f, 0.0f, -22.82f));
	m_ppObjects[2]->Rotate(0.0f, 0.0f, 0.0f);
	m_ppObjects[2]->OnPrepareAnimate();
	m_ppObjects[2]->SetNormalVector();
	m_ppObjects[2]->objLayer = Layout::GENERATOR;

	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	/*if (pGenerator1)delete pGenerator1;
	if (pGenerator2)delete pGenerator2;
	if (pGenerator3)delete pGenerator3;*/
}



CVirtualObjectsShader::CVirtualObjectsShader()
{
}

CVirtualObjectsShader::~CVirtualObjectsShader()
{
}

void CVirtualObjectsShader::BuildObjects(ID3D12Device5* pd3dDevice, ID3D12GraphicsCommandList4* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CLoadedModelInfo* pModel, void* pContext)
{

}

CHitEffectObjectsShader::CHitEffectObjectsShader()
{
}

CHitEffectObjectsShader::~CHitEffectObjectsShader()
{
}

void CHitEffectObjectsShader::BuildObjects(ID3D12Device5* pd3dDevice, ID3D12GraphicsCommandList4* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CLoadedModelInfo* pModel, void* pContext)
{
	m_nObjects = 1;
	m_ppObjects = new CGameObject * [m_nObjects];
	
	CGameObject* pHit = CGameObject::LoadGeometryFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/hit.bin", this, Layout::EFFECT);
	pHit->m_type = 1;
	m_ppObjects[0] = new CHitEffect();
	m_ppObjects[0]->SetChild(pHit ,true);
	m_ppObjects[0]->OnPrepareAnimate();
	m_ppObjects[0]->SetPosition(XMFLOAT3(0.2f, 1.114f, 3.7f));
	m_ppObjects[0]->SetScale(0.5f, 0.5f, 0.5f);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	//if (pHit) delete pHit;

}

void CHitEffectObjectsShader::Render(ID3D12GraphicsCommandList4* pd3dCommandList, CCamera* pCamera, bool bRaster)
{
	CStandardShader::Render(pd3dCommandList, pCamera, bRaster);
	for (int j = 0; j < m_nObjects; j++)
	{
		if (m_ppObjects[j])
		{
			if (m_ppObjects[j])
			{
				m_ppObjects[j]->Animate(m_fElapsedTime);
				m_ppObjects[j]->UpdateTransform(NULL);
				if (((CHitEffect*)m_ppObjects[j])->GetOnHit())
					m_ppObjects[j]->Render(pd3dCommandList, pCamera, bRaster);
			}
		}
	}
}
