#include "pch.h"
#include "Shader.h"

CShader::CShader()
{
	m_d3dSrvCPUDescStartHandle.ptr = NULL;
	m_d3dSrvGPUDescStartHandle.ptr = NULL;
}

CShader::~CShader()
{
	if (m_pd3dPipelineState) m_pd3dPipelineState->Release();
}

//�Է� �����⿡�� ���� ������ ������ �˷��ֱ� ���� ����ü�� ��ȯ�Ѵ�.
D3D12_INPUT_LAYOUT_DESC CShader::CreateInputLayout()
{
	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = NULL;
	d3dInputLayoutDesc.NumElements = 0;

	return(d3dInputLayoutDesc);
}

//�����Ͷ����� ���¸� �����ϱ� ���� ����ü�� ��ȯ�Ѵ�. 
D3D12_RASTERIZER_DESC CShader::CreateRasterizerState()
{
	D3D12_RASTERIZER_DESC d3dRasterizerDesc;
	::ZeroMemory(&d3dRasterizerDesc, sizeof(D3D12_RASTERIZER_DESC));
	//D3D12_FILL_MODE_WIREFRAME�� ������Ƽ��(�ﰢ��)�� ���θ� ĥ���� �ʰ� ��(Edge)�� �׸���.
	d3dRasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID; //D3D12_FILL_MODE_SOLID; // �ﰢ�� ������ �� ���� ä��� ��� ����
	d3dRasterizerDesc.CullMode = D3D12_CULL_MODE_BACK; //�ø� ���� : D3D12_CULL_MODE_BACK �޸� ����  / ���� ���� ���� : D3D12_CULL_MODE_NONE(���鵵 �׸���) / D3D12_CULL_MODE_FRONT �ո� ����
	d3dRasterizerDesc.FrontCounterClockwise = FALSE; //false ( �������Ž� �ð���� �Ǵ� �ݽð���� ����)
	d3dRasterizerDesc.DepthBias = 0;
	d3dRasterizerDesc.DepthBiasClamp = 0.0f;
	d3dRasterizerDesc.SlopeScaledDepthBias = 0.0f;
	d3dRasterizerDesc.DepthClipEnable = TRUE; // true ��������
	d3dRasterizerDesc.MultisampleEnable = FALSE;
	d3dRasterizerDesc.AntialiasedLineEnable = FALSE;
	d3dRasterizerDesc.ForcedSampleCount = 0;
	d3dRasterizerDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	return(d3dRasterizerDesc);
}

D3D12_BLEND_DESC CShader::CreateBlendState()
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = FALSE;
	d3dBlendDesc.IndependentBlendEnable = FALSE; // ����Ÿ��0�� ���
	d3dBlendDesc.RenderTarget[0].BlendEnable = FALSE; // ����Ÿ�ٰ� �ȼ� ���̴� ���� ����
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL; //����Ÿ�� ���� ���� : rgba��簪 ( ����Ÿ�ٸ��� ���� ����)

	return(d3dBlendDesc);
}

//����-���ٽ� �˻縦 ���� ���¸� �����ϱ� ���� ����ü�� ��ȯ�Ѵ�. 
D3D12_DEPTH_STENCIL_DESC CShader::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));

	d3dDepthStencilDesc.DepthEnable = TRUE; // ���̰˻� : FALSE �� ����-�˻縦 ���� �����Ƿ� ���� ���� ��ü���� �������� ��ó�� �׷�����
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS; // ���� �����Ͱ� ��� �����ͺ��� ������ �񱳸� ���
	d3dDepthStencilDesc.StencilEnable = FALSE; // ���ٽǰ˻�
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

//���� ���̴� ����Ʈ �ڵ带 ����(������)�Ѵ�. 
D3D12_SHADER_BYTECODE CShader::CreateVertexShader(ID3DBlob** ppd3dShaderBlob)
{
	D3D12_SHADER_BYTECODE d3dShaderByteCode;
	d3dShaderByteCode.BytecodeLength = 0;
	d3dShaderByteCode.pShaderBytecode = NULL;

	return(d3dShaderByteCode);
}

//�ȼ� ���̴� ����Ʈ �ڵ带 ����(������)�Ѵ�. 
D3D12_SHADER_BYTECODE CShader::CreatePixelShader(ID3DBlob** ppd3dShaderBlob)
{
	D3D12_SHADER_BYTECODE d3dShaderByteCode;
	d3dShaderByteCode.BytecodeLength = 0;
	d3dShaderByteCode.pShaderBytecode = NULL;

	return(d3dShaderByteCode);
}

//���̴� �ҽ� �ڵ带 �������Ͽ� ����Ʈ �ڵ� ����ü�� ��ȯ�Ѵ�. 
D3D12_SHADER_BYTECODE CShader::CompileShaderFromFile(const WCHAR* pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderProfile, ID3DBlob** ppd3dShaderBlob)
{
	UINT nCompileFlags = 0;
#if defined(_DEBUG)
	nCompileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif //Microsoft HLSL(High Level Shader Language) �ڵ带 ������ ��� ���� ����Ʈ�ڵ�� ������
	::D3DCompileFromFile(pszFileName, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, pszShaderName, pszShaderProfile,
		nCompileFlags, 0, ppd3dShaderBlob, NULL);

	D3D12_SHADER_BYTECODE d3dShaderByteCode;
	d3dShaderByteCode.BytecodeLength = (*ppd3dShaderBlob)->GetBufferSize();
	d3dShaderByteCode.pShaderBytecode = (*ppd3dShaderBlob)->GetBufferPointer();

	return(d3dShaderByteCode);
}

//�׷��Ƚ� ���������� ���� ��ü�� �����Ѵ�. 
void CShader::CreateShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	ID3DBlob* pd3dVertexShaderBlob = NULL;
	ID3DBlob* pd3dPixelShaderBlob = NULL;

	//---�׷��� ���������� ���� ��ũ���� ����
	D3D12_GRAPHICS_PIPELINE_STATE_DESC d3dPipelineStateDesc;
	::ZeroMemory(&d3dPipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	d3dPipelineStateDesc.pRootSignature = pd3dGraphicsRootSignature;
	d3dPipelineStateDesc.VS = CreateVertexShader(&pd3dVertexShaderBlob);
	d3dPipelineStateDesc.PS = CreatePixelShader(&pd3dPixelShaderBlob);
	d3dPipelineStateDesc.RasterizerState = CreateRasterizerState();
	d3dPipelineStateDesc.BlendState = CreateBlendState();
	d3dPipelineStateDesc.DepthStencilState = CreateDepthStencilState();
	d3dPipelineStateDesc.InputLayout = CreateInputLayout();
	d3dPipelineStateDesc.SampleMask = UINT_MAX;
	d3dPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	d3dPipelineStateDesc.NumRenderTargets = 1;
	d3dPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3dPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dPipelineStateDesc.SampleDesc.Count = 1;
	d3dPipelineStateDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	HRESULT hResult = pd3dDevice->CreateGraphicsPipelineState(&d3dPipelineStateDesc,
		__uuidof(ID3D12PipelineState), (void**)&m_pd3dPipelineState);

	if (pd3dVertexShaderBlob) pd3dVertexShaderBlob->Release();
	if (pd3dPixelShaderBlob) pd3dPixelShaderBlob->Release();

	if (d3dPipelineStateDesc.InputLayout.pInputElementDescs)
		delete[] d3dPipelineStateDesc.InputLayout.pInputElementDescs;
}

void CShader::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
}

void CShader::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
}

void CShader::ReleaseShaderVariables()
{
	if (m_pCbvSrvDescHeap) 
		m_pCbvSrvDescHeap->Release();
}

void CShader::ReleaseUploadBuffers()
{
}

void CShader::UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT4X4* pxmf4x4World)
{
	//XMFLOAT4X4 xmf4x4World;
	//XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(pxmf4x4World)));
	//pd3dCommandList->SetGraphicsRoot32BitConstants(0, 16, &xmf4x4World, 0);
}


void CShader::OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList)
{
	//���������ο� �׷��Ƚ� ���� ��ü�� �����Ѵ�. 
	//pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[0]);
	if (m_pd3dPipelineState)
		pd3dCommandList->SetPipelineState(m_pd3dPipelineState);
	pd3dCommandList->SetDescriptorHeaps(1, &m_pCbvSrvDescHeap);

	UpdateShaderVariables(pd3dCommandList);
}

void CShader::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	OnPrepareRender(pd3dCommandList);
}

void CShader::CreateCbvSrvDescriptorHeaps(ID3D12Device* pd3dDevice, int nCbv, int nSrv)
{
	D3D12_DESCRIPTOR_HEAP_DESC CbvSrvDescriptorHeap;
	CbvSrvDescriptorHeap.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	CbvSrvDescriptorHeap.NumDescriptors = nCbv + nSrv;
	CbvSrvDescriptorHeap.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	CbvSrvDescriptorHeap.NodeMask = 0;
	pd3dDevice->CreateDescriptorHeap(&CbvSrvDescriptorHeap, 
		__uuidof(ID3D12DescriptorHeap), (void**)&m_pCbvSrvDescHeap);


	m_d3dCbvCPUDescStartHandle = m_pCbvSrvDescHeap->GetCPUDescriptorHandleForHeapStart();
	m_d3dCbvGPUDescStartHandle = m_pCbvSrvDescHeap->GetGPUDescriptorHandleForHeapStart();
	
	m_d3dSrvCPUDescStartHandle.ptr = m_d3dCbvCPUDescStartHandle.ptr + (::gnCbvSrvDescIncrementSize * nCbv);
	m_d3dSrvGPUDescStartHandle.ptr = m_d3dCbvGPUDescStartHandle.ptr + (::gnCbvSrvDescIncrementSize * nCbv);

	m_d3dSrvCPUDescNextHandle = m_d3dSrvCPUDescStartHandle;
	m_d3dSrvGPUDescNextHandle = m_d3dSrvGPUDescStartHandle;
}

void CShader::CreateConstantBufferViews(ID3D12Device* pd3dDevice, int nConstantBufferViews, ID3D12Resource* pd3dConstantBuffers, UINT nStride)
{
	D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress = pd3dConstantBuffers->GetGPUVirtualAddress();

	D3D12_CONSTANT_BUFFER_VIEW_DESC d3dCBVDesc;
	d3dCBVDesc.SizeInBytes = nStride;

	for (int j = 0; j < nConstantBufferViews; j++)
	{
		d3dCBVDesc.BufferLocation = d3dGpuVirtualAddress + (nStride * j);

		D3D12_CPU_DESCRIPTOR_HANDLE d3dCbvCPUDescriptorHandle;
		d3dCbvCPUDescriptorHandle.ptr = m_d3dCbvCPUDescStartHandle.ptr + (::gnCbvSrvDescIncrementSize * j);

		pd3dDevice->CreateConstantBufferView(&d3dCBVDesc, d3dCbvCPUDescriptorHandle);
	}
}

void CShader::CreateShaderResourceViews(ID3D12Device* pd3dDevice, CTexture* pTexture, UINT nDescHeapIndex, UINT nRootParamStartIndex)
{
	m_d3dSrvCPUDescNextHandle.ptr += ::gnCbvSrvDescIncrementSize * nDescHeapIndex;
	m_d3dSrvGPUDescNextHandle.ptr += ::gnCbvSrvDescIncrementSize * nDescHeapIndex;

	int nTex = pTexture->GetTextureNum();
	//�ؽ��� �ڿ��� �����Ͽ� �� �ڿ��� CPU�� �����Ͽ� SRV���� �ش� �ؽ��� �ε����� �����Ѵ�. SRV �� ���� �����̴�.
	for (int i = 0; i < nTex; i++)
	{
		//�ؽ��� �ڿ��� CPU �ڵ�� ��� ���ҽ���� �������ش�.
		ID3D12Resource* pShaderResource = pTexture->GetResource(i);

		D3D12_SHADER_RESOURCE_VIEW_DESC SrvDescriptor = pTexture->GetShaderResourceViewDesc(i);

		pd3dDevice->CreateShaderResourceView(pShaderResource, &SrvDescriptor, m_d3dSrvCPUDescNextHandle);

		m_d3dSrvCPUDescNextHandle.ptr += ::gnCbvSrvDescIncrementSize;

		//SRV ������ �ڵ�� SRV�ε����� ���� GPU ���� ���� �����Ѵ�.
		pTexture->SetSrvGPUDescHandles(i, m_d3dSrvGPUDescNextHandle);

		m_d3dSrvGPUDescNextHandle.ptr += ::gnCbvSrvDescIncrementSize;
	}

	int nRootParam = pTexture->GetRootParamNum();
	for (int i = 0; i < nRootParam; i++)
	{
		//��Ʈ�Ķ��Ÿ �ε��� ���ۿ� �ε��� ����
		pTexture->SetRootParamIndexBuf(i, nRootParamStartIndex + i);
	}
	
}

CPlayerShader::CPlayerShader()
{
}

CPlayerShader::~CPlayerShader()
{
}

D3D12_INPUT_LAYOUT_DESC CPlayerShader::CreateInputLayout()
{
	UINT nInputElementDescs = 2;
	D3D12_INPUT_ELEMENT_DESC* pd3dInputElementDescs = new
		D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12,
	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}

D3D12_SHADER_BYTECODE CPlayerShader::CreateVertexShader(ID3DBlob** ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VSPlayer", "vs_5_1",
		ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE CPlayerShader::CreatePixelShader(ID3DBlob** ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PSPlayer", "ps_5_1",
		ppd3dShaderBlob));
}

void CPlayerShader::CreateShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	CShader::CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
}

CObjectsShader::CObjectsShader()
{
}

CObjectsShader::~CObjectsShader()
{
}

#define TEXTURES 6
void CObjectsShader::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, void* pContext)
{
	CTexture* ppTextures[TEXTURES];
	ppTextures[0] = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
	ppTextures[0]->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Industry_Field_Tile_BaseColor.dds", RESOURCE_TEXTURE2D, 0);	//�ٴ�
	ppTextures[1] = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
	ppTextures[1]->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Industry_Wall_BaseColor.dds", RESOURCE_TEXTURE2D, 0);	//��
	ppTextures[2] = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
	ppTextures[2]->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Industry_Roof_BaseColor.dds", RESOURCE_TEXTURE2D, 0);	//õ��
	ppTextures[3] = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
	ppTextures[3]->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Industry_Pillar1_BaseColor.dds", RESOURCE_TEXTURE2D, 0);	//���
	ppTextures[4] = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
	ppTextures[4]->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Industry_Pillar2_BaseColor.dds", RESOURCE_TEXTURE2D, 0); // 
	ppTextures[5] = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
	ppTextures[5]->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Lava(Diffuse).dds", RESOURCE_TEXTURE2D, 0);

	//1.0f = 1cm / 100.0f = 1m
	float TileSize = (float)2.5 * UNIT;
	float Width = 90 * UNIT;
	float Depth = 90 * UNIT;
	int nWidth = (int)Width / TileSize;
	int nDepth = (int)Depth / TileSize;

	m_nObjects = nWidth * nDepth + 5 +360;// +6;

	//----������ �� ���� �� CBV/SRV������ ����
	CreateCbvSrvDescriptorHeaps(pd3dDevice, m_nObjects, TEXTURES);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateConstantBufferViews(pd3dDevice, m_nObjects, m_pd3dcbGameObjects, ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255));
	for (int i = 0; i < TEXTURES; i++)
		CreateShaderResourceViews(pd3dDevice, ppTextures[i], 0, 3);

	//-------------------------------------------
	CMaterial* pMats[TEXTURES];
	for (int i = 0; i < TEXTURES; i++)
	{
		pMats[i] = new CMaterial();
		pMats[i]->SetTexture(ppTextures[i]);
	}
	//-------------------------------------------

	m_ppObjects = new CGameObject * [m_nObjects];
	int i = 0;
	//Ÿ�� �ٴ�
	CCubeMeshTextured* pTile = new CCubeMeshTextured(pd3dDevice, pd3dCommandList, TileSize, 1.0f, TileSize);
	for (float x = -Width / 2 + TileSize / 2; x < Width / 2; x += TileSize)
	{
		for (float z = -Depth / 2 + TileSize / 2; z < Depth / 2; z += TileSize)
		{
			CGameObject* pMap = new CGameObject(1);
			XMFLOAT3 pos = XMFLOAT3(x, 0.0f, z);
			pMap->SetObjectInWorld(0, pTile, pMats[0], pos);
			pMap->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescStartHandle.ptr + (::gnCbvSrvDescIncrementSize * i));

			m_ppObjects[i++] = pMap;
		}
	}

	//------ ���� �� ���� 5��
	int WarehouseSizeXZ = 60;
	int WarehouseSizeY = 60;

	CCubeMeshTextured* pSideXWall = new CCubeMeshTextured(pd3dDevice, pd3dCommandList, 1 * UNIT, WarehouseSizeY * UNIT, WarehouseSizeXZ * UNIT);
	CCubeMeshTextured* pSideZWall = new CCubeMeshTextured(pd3dDevice, pd3dCommandList, WarehouseSizeXZ * UNIT, WarehouseSizeY * UNIT, 1 * UNIT);
	CCubeMeshTextured* pSideYWall = new CCubeMeshTextured(pd3dDevice, pd3dCommandList, WarehouseSizeXZ * UNIT, 1 * UNIT, WarehouseSizeXZ * UNIT);
	//CCubeMeshTextured* test = new CCubeMeshTextured(pd3dDevice, pd3dCommandList, 2.5f * UNIT, 1 * UNIT, 2.5f * UNIT);

	//��
	CGameObject* pWareHouseLeft = new CGameObject(1);
	pWareHouseLeft->SetObjectInWorld(0, pSideXWall, pMats[1], XMFLOAT3(-WarehouseSizeXZ / 2 * UNIT, (WarehouseSizeY / 2 - 0.1f) * UNIT, 0.0f));
	pWareHouseLeft->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescStartHandle.ptr + (::gnCbvSrvDescIncrementSize * i));
	m_ppObjects[i++] = pWareHouseLeft;

	CGameObject* pWareHouseRight = new CGameObject(1);
	pWareHouseRight->SetObjectInWorld(0, pSideXWall, pMats[1], XMFLOAT3(WarehouseSizeXZ / 2 * UNIT, (WarehouseSizeY / 2 - 0.1f) * UNIT, 0.0f));
	pWareHouseRight->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescStartHandle.ptr + (::gnCbvSrvDescIncrementSize * i));
	m_ppObjects[i++] = pWareHouseRight;

	CGameObject* pWareHouseFront = new CGameObject(1);
	pWareHouseFront->SetObjectInWorld(0, pSideZWall, pMats[1], XMFLOAT3(0.0f, (WarehouseSizeY / 2 - 0.1f) * UNIT, WarehouseSizeXZ / 2 * UNIT));
	pWareHouseFront->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescStartHandle.ptr + (::gnCbvSrvDescIncrementSize * i));
	m_ppObjects[i++] = pWareHouseFront;

	CGameObject* pWareHouseBack = new CGameObject(1);
	pWareHouseBack->SetObjectInWorld(0, pSideZWall, pMats[1], XMFLOAT3(0.0f, (WarehouseSizeY / 2 - 0.1f) * UNIT, -WarehouseSizeXZ / 2 * UNIT));
	pWareHouseBack->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescStartHandle.ptr + (::gnCbvSrvDescIncrementSize * i));
	m_ppObjects[i++] = pWareHouseBack;

	//õ��
	CGameObject* pWareHouseTopFloor = new CGameObject(1);
	pWareHouseTopFloor->SetObjectInWorld(0, pSideYWall, pMats[2], XMFLOAT3(0.0f, (WarehouseSizeY / 2 + 0.5f) * UNIT, 0.0f));
	pWareHouseTopFloor->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescStartHandle.ptr + (::gnCbvSrvDescIncrementSize * i));
	m_ppObjects[i++] = pWareHouseTopFloor;



	//------- ���� ��� ���� 9��
	float SizeOfBlock = 1.5 * UNIT;
	CCubeMeshTextured* pRod = new CCubeMeshTextured(pd3dDevice, pd3dCommandList, SizeOfBlock, SizeOfBlock, SizeOfBlock);
	float l = WarehouseSizeXZ / 2 * UNIT;
	float nHeignt = (WarehouseSizeY * UNIT) / SizeOfBlock;
	for (float x = -l / 2; x <= l / 2; x += l / 2)
	{
		for (float z = -l / 2; z <= l / 2; z += l / 2)
		{
			for (float y = SizeOfBlock / 2; y < WarehouseSizeY * UNIT; y += SizeOfBlock)
			{
				CGameObject* pPillar = new CGameObject(1);
				pPillar->SetObjectInWorld(0, pRod, pMats[4], XMFLOAT3(x, y, z));// XMFLOAT3(x, (WarehouseSizeY / 2 - 0.1f) * UNIT,��);// 
				pPillar->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescStartHandle.ptr + (::gnCbvSrvDescIncrementSize * i));
				m_ppObjects[i++] = pPillar;
			}
		}
	}


}

void CObjectsShader::AnimateObjects(float fTimeElapsed)
{
	for (int j = 0; j < m_nObjects; j++)
	{
		m_ppObjects[j]->Animate(fTimeElapsed);
	}
}

void CObjectsShader::ReleaseObjects()
{
	if (m_ppObjects)
	{
		for (int j = 0; j < m_nObjects; j++)
		{
			if (m_ppObjects[j]) delete m_ppObjects[j];
		}
		delete[] m_ppObjects;
	}
}
void CObjectsShader::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256�� ���
	m_pd3dcbGameObjects = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes * m_nObjects, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbGameObjects->Map(0, NULL, (void**)&m_pcbMappedGameObjects);
}

//D3D12_INPUT_LAYOUT_DESC CObjectsShader::CreateInputLayout()
//{
//	UINT nInputElementDescs = 2;
//	D3D12_INPUT_ELEMENT_DESC* pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];
//
//	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
//	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
//	pd3dInputElementDescs[1] = { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12,
//	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
//
//	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
//	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
//	d3dInputLayoutDesc.NumElements = nInputElementDescs;
//	return(d3dInputLayoutDesc);
//}
//
//D3D12_SHADER_BYTECODE CObjectsShader::CreateVertexShader(ID3DBlob** ppd3dShaderBlob)
//{
//	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VSDiffused", "vs_5_1", ppd3dShaderBlob));
//}
//
//D3D12_SHADER_BYTECODE CObjectsShader::CreatePixelShader(ID3DBlob** ppd3dShaderBlob)
//{
//	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PSDiffused", "ps_5_1", ppd3dShaderBlob));
//}
//
//void CObjectsShader::CreateShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature)
//{
//	m_nPipelineStates = 1;
//	m_ppd3dPipelineStates = new ID3D12PipelineState * [m_nPipelineStates];
//
//	CShader::CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
//}
void CObjectsShader::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
	for (int j = 0; j < m_nObjects; j++)
	{
		CB_GAMEOBJECT_INFO* pbMappedcbGameObject = (CB_GAMEOBJECT_INFO*)((UINT8*)m_pcbMappedGameObjects + (j * ncbElementBytes));
		XMStoreFloat4x4(&pbMappedcbGameObject->m_xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_ppObjects[j]->m_xmf4x4World)));
	}
}

void CObjectsShader::ReleaseShaderVariables()
{
	if (m_pd3dcbGameObjects)
	{
		m_pd3dcbGameObjects->Unmap(0, NULL);
		m_pd3dcbGameObjects->Release();
	}

	CTexturedShader::ReleaseShaderVariables();
}

void CObjectsShader::ReleaseUploadBuffers()
{
	if (m_ppObjects)
	{
		for (int j = 0; j < m_nObjects; j++) 
			m_ppObjects[j]->ReleaseUploadBuffers();
	}
}

void CObjectsShader::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	CTexturedShader::Render(pd3dCommandList, pCamera);
	for (int j = 0; j < m_nObjects; j++)
	{
		if (m_ppObjects[j])
		{
			m_ppObjects[j]->Render(pd3dCommandList, pCamera);
		}
	}
}



CTexturedShader::CTexturedShader()
{
}

CTexturedShader::~CTexturedShader()
{
}

D3D12_INPUT_LAYOUT_DESC CTexturedShader::CreateInputLayout()
{
	UINT nInputElementDescs = 2;
	D3D12_INPUT_ELEMENT_DESC* pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}

D3D12_SHADER_BYTECODE CTexturedShader::CreateVertexShader(ID3DBlob** ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VSTextured", "vs_5_1", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE CTexturedShader::CreatePixelShader(ID3DBlob** ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PSTextured", "ps_5_1", ppd3dShaderBlob));
}

void CTexturedShader::CreateShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	CShader::CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
}

//------���� �ʿ�
CRectShader::CRectShader()
{
}

CRectShader::~CRectShader()
{
}

D3D12_INPUT_LAYOUT_DESC CRectShader::CreateInputLayout()
{
	UINT nInputElemetDescs = 2;
	D3D12_INPUT_ELEMENT_DESC* pd3InputElemetDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElemetDescs];
	pd3InputElemetDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3InputElemetDescs[1] = { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3InputElemetDescs;
	d3dInputLayoutDesc.NumElements = nInputElemetDescs;

	return d3dInputLayoutDesc;
}

D3D12_SHADER_BYTECODE CRectShader::CreateVertexShader(ID3DBlob** ppd3dShaderBlob)
{
	return CShader::CompileShaderFromFile(L"Shaders.hlsl", "VSDiffused", "vs_5_1", ppd3dShaderBlob);
}

D3D12_SHADER_BYTECODE CRectShader::CreatePixelShader(ID3DBlob** ppd3dShaderBlob)
{
	return CShader::CompileShaderFromFile(L"Shaders.hlsl", "PSDiffused", "vs_5_1", ppd3dShaderBlob);
}