#include "stdafx.h"
#include "Shader.h"

CShader::CShader()
{
}

CShader::~CShader()
{
	if (m_ppd3dPipelineStates)
	{
		for (int i = 0; i < m_nPipelineStates; i++)
		{
			if (m_ppd3dPipelineStates[i])
				m_ppd3dPipelineStates[i]->Release();
		}
		delete[] m_ppd3dPipelineStates;
	}
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
	::D3DCompileFromFile(pszFileName, NULL, NULL, pszShaderName, pszShaderProfile,
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
	pd3dDevice->CreateGraphicsPipelineState(&d3dPipelineStateDesc,
		__uuidof(ID3D12PipelineState), (void**)&m_ppd3dPipelineStates[0]);

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
}

void CShader::UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT4X4* pxmf4x4World)
{
	XMFLOAT4X4 xmf4x4World;
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(pxmf4x4World)));
	pd3dCommandList->SetGraphicsRoot32BitConstants(0, 16, &xmf4x4World, 0);
}


void CShader::OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList)
{
	//���������ο� �׷��Ƚ� ���� ��ü�� �����Ѵ�. 
	pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[0]);
}

void CShader::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	OnPrepareRender(pd3dCommandList);
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
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VSDiffused", "vs_5_1",
		ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE CPlayerShader::CreatePixelShader(ID3DBlob** ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PSDiffused", "ps_5_1",
		ppd3dShaderBlob));
}

void CPlayerShader::CreateShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	m_nPipelineStates = 1;
	m_ppd3dPipelineStates = new ID3D12PipelineState * [m_nPipelineStates];

	CShader::CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
}

CObjectsShader::CObjectsShader()
{
}

CObjectsShader::~CObjectsShader()
{
}

void CObjectsShader::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, void* pContext)
{
	//1.0f = 1cm / 100.0f = 1m
	float TileSize = (float)1 * UNIT;
	float Width = 90 * UNIT;
	float Depth = 90 * UNIT;
	int nWidth = (int)Width / TileSize;
	int nDepth = (int)Depth / TileSize;

	m_nObjects = nWidth * nDepth + 5 + 9;// +6;
	m_ppObjects = new CGameObject * [m_nObjects];

	CRectangleMesh* pRect = new CRectangleMesh(pd3dDevice, pd3dCommandList, TileSize, TileSize, 1.0f);

	int i = 0;
	for (float x = -Width / 2 + TileSize / 2; x < Width / 2; x += TileSize)
	{
		for (float z = -Depth / 2 + TileSize / 2; z < Depth / 2; z += TileSize)
		{
			CGameObject* pMap = new CGameObject(1);
			pMap->SetMesh(0, pRect);
			pMap->SetPosition(x, 0.0f, z);

			m_ppObjects[i++] = pMap;
		}
	}
	//------ ���� �� ���� 5��
	int WarehouseSizeXZ = 60;
	int WarehouseSizeY = 60;

	CCubeMeshDiffused* pSideXWall = new CCubeMeshDiffused(pd3dDevice, pd3dCommandList, 1 * UNIT, WarehouseSizeY * UNIT, WarehouseSizeXZ * UNIT);
	CCubeMeshDiffused* pSideZWall = new CCubeMeshDiffused(pd3dDevice, pd3dCommandList, WarehouseSizeXZ * UNIT, WarehouseSizeY * UNIT, 1 * UNIT);
	CCubeMeshDiffused* pSideYWall = new CCubeMeshDiffused(pd3dDevice, pd3dCommandList, WarehouseSizeXZ * UNIT, 1 * UNIT, WarehouseSizeXZ * UNIT);

	CGameObject* pWareHouseLeft = new CGameObject(1);
	pWareHouseLeft->SetObjectInWorld(m_ppObjects, i++, XMFLOAT3(-WarehouseSizeXZ / 2 * UNIT, (WarehouseSizeY / 2 - 0.1f) * UNIT, 0.0f), pSideXWall, 0);

	CGameObject* pWareHouseRight = new CGameObject(1);
	pWareHouseRight->SetObjectInWorld(m_ppObjects, i++, XMFLOAT3(WarehouseSizeXZ / 2 * UNIT, (WarehouseSizeY / 2 - 0.1f) * UNIT, 0.0f), pSideXWall, 0);

	CGameObject* pWareHouseFront = new CGameObject(1);
	pWareHouseFront->SetObjectInWorld(m_ppObjects, i++, XMFLOAT3(0.0f, (WarehouseSizeY / 2 - 0.1f) * UNIT, WarehouseSizeXZ / 2 * UNIT), pSideZWall, 0);

	CGameObject* pWareHouseBack = new CGameObject(1);
	pWareHouseBack->SetObjectInWorld(m_ppObjects, i++, XMFLOAT3(0.0f, (WarehouseSizeY / 2 - 0.1f) * UNIT, -WarehouseSizeXZ / 2 * UNIT), pSideZWall, 0);

	CGameObject* pWareHouseFloor = new CGameObject(1);
	pWareHouseFloor->SetObjectInWorld(m_ppObjects, i++, XMFLOAT3(0.0f, (WarehouseSizeY / 2 + 0.5f) * UNIT, 0.0f), pSideYWall, 0);

	//------- ���� ��� ���� 9��
	CCubeMeshDiffused* pRod = new CCubeMeshDiffused(pd3dDevice, pd3dCommandList, 1 * UNIT, WarehouseSizeY * UNIT, 1 * UNIT);
	float l = WarehouseSizeXZ / 2 * UNIT;

	for (float x = -l / 2; x <= l / 2; x += l / 2)
	{
		for (float z = -l / 2; z <= l / 2; z += l / 2)
		{
			CGameObject* pPillar = new CGameObject(1);
			pPillar->SetObjectInWorld(m_ppObjects, i++, XMFLOAT3(x, (WarehouseSizeY / 2 - 0.1f) * UNIT, z), pRod, 0);
		}
	}

	/*//------ �ҷ� �ù� ���� 6��
	float tem = (float)(l / 2) * (float)(1 / 3);
	float blockHeight = 2.5f;
	CCubeMeshDiffused* pBlock = new CCubeMeshDiffused(pd3dDevice, pd3dCommandList, (tem-0.3f) * UNIT, blockHeight * UNIT, 1 * UNIT);
	for (float x = tem; x < tem*3; x += tem)
	{
		for (float z = l/2-tem; z <= l/2+tem; z += tem)
		{
			CGameObject* pShelf = new CGameObject(1);
			pShelf->SetObjectInWorld(m_ppObjects, i++, XMFLOAT3(x, (blockHeight/2-0.1f) * UNIT, z), pBlock, 0);
		}
	}*/

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
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

D3D12_INPUT_LAYOUT_DESC CObjectsShader::CreateInputLayout()
{
	UINT nInputElementDescs = 2;
	D3D12_INPUT_ELEMENT_DESC* pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12,
	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;
	return(d3dInputLayoutDesc);
}

D3D12_SHADER_BYTECODE CObjectsShader::CreateVertexShader(ID3DBlob** ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VSDiffused", "vs_5_1", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE CObjectsShader::CreatePixelShader(ID3DBlob** ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PSDiffused", "ps_5_1", ppd3dShaderBlob));
}

void CObjectsShader::CreateShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	m_nPipelineStates = 1;
	m_ppd3dPipelineStates = new ID3D12PipelineState * [m_nPipelineStates];

	CShader::CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
}

void CObjectsShader::ReleaseUploadBuffers()
{
	if (m_ppObjects)
	{
		for (int j = 0; j < m_nObjects; j++) m_ppObjects[j]->ReleaseUploadBuffers();
	}
}

void CObjectsShader::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	CShader::Render(pd3dCommandList, pCamera);
	for (int j = 0; j < m_nObjects; j++)
	{
		if (m_ppObjects[j])
		{
			m_ppObjects[j]->Render(pd3dCommandList, pCamera);
		}
	}
}

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