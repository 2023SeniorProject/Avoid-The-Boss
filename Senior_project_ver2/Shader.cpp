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

//입력 조립기에게 정점 버퍼의 구조를 알려주기 위한 구조체를 반환한다.
D3D12_INPUT_LAYOUT_DESC CShader::CreateInputLayout()
{
//	UINT nInputElementDescs = 2;
//	D3D12_INPUT_ELEMENT_DESC* pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];
//
//	//정점은 위치 벡터(POSITION)와 색상(COLOR)을 가진다. 
//	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
//D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
//	pd3dInputElementDescs[1] = { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12,
//	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = NULL;
	d3dInputLayoutDesc.NumElements = 0;

	return(d3dInputLayoutDesc);
}

//래스터라이저 상태를 설정하기 위한 구조체를 반환한다. 
D3D12_RASTERIZER_DESC CShader::CreateRasterizerState()
{
	D3D12_RASTERIZER_DESC d3dRasterizerDesc;
	::ZeroMemory(&d3dRasterizerDesc, sizeof(D3D12_RASTERIZER_DESC));
	//D3D12_FILL_MODE_WIREFRAME은 프리미티브(삼각형)의 내부를 칠하지 않고 변(Edge)만 그린다.
	d3dRasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID; //D3D12_FILL_MODE_SOLID; // 삼각형 렌더링 시 색상 채우기 모드 설정
	d3dRasterizerDesc.CullMode = D3D12_CULL_MODE_BACK; //컬링 설정 : D3D12_CULL_MODE_BACK 뒷면 제거  / 은면 제거 안함 : D3D12_CULL_MODE_NONE(은면도 그린다) / D3D12_CULL_MODE_FRONT 앞면 제거
	d3dRasterizerDesc.FrontCounterClockwise = FALSE; //false ( 은면제거시 시계방향 또는 반시계방향 설정)
	d3dRasterizerDesc.DepthBias = 0;
	d3dRasterizerDesc.DepthBiasClamp = 0.0f;
	d3dRasterizerDesc.SlopeScaledDepthBias = 0.0f;
	d3dRasterizerDesc.DepthClipEnable = TRUE; // true 은면제거
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
	d3dBlendDesc.IndependentBlendEnable = FALSE; // 렌더타겟0만 사용
	d3dBlendDesc.RenderTarget[0].BlendEnable = FALSE; // 렌더타겟과 픽셀 셰이더 블렌드 여부
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL; //렌더타겟 색상 지정 : rgba모든값 ( 렌더타겟마다 설정 가능)

	return(d3dBlendDesc);
}

//깊이-스텐실 검사를 위한 상태를 설정하기 위한 구조체를 반환한다. 
D3D12_DEPTH_STENCIL_DESC CShader::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
	// FALSE 시 깊이-검사를 하지 않으므로 여러 개의 객체들이 겹쳐지는 것처럼 그려진다
	d3dDepthStencilDesc.DepthEnable = FALSE; // 깊이검사
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS; // 원본 데이터가 대상 데이터보다 작으면 비교를 통과
	d3dDepthStencilDesc.StencilEnable = FALSE; // 스텐실검사
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

//정점 셰이더 바이트 코드를 생성(컴파일)한다. 
D3D12_SHADER_BYTECODE CShader::CreateVertexShader(ID3DBlob** ppd3dShaderBlob)
{
	D3D12_SHADER_BYTECODE d3dShaderByteCode;
	d3dShaderByteCode.BytecodeLength = 0;
	d3dShaderByteCode.pShaderBytecode = NULL;

	return(d3dShaderByteCode);
	//return(CompileShaderFromFile(L"C:\\Users\\aeiou\\Desktop\\SeniorProject\\Avoid-The-Boss\\Senior_project_ver2\\Shaders.hlsl", "VSMain", "vs_5_1", ppd3dShaderBlob));
}

//픽셀 셰이더 바이트 코드를 생성(컴파일)한다. 
D3D12_SHADER_BYTECODE CShader::CreatePixelShader(ID3DBlob** ppd3dShaderBlob)
{
	D3D12_SHADER_BYTECODE d3dShaderByteCode;
	d3dShaderByteCode.BytecodeLength = 0;
	d3dShaderByteCode.pShaderBytecode = NULL;

	return(d3dShaderByteCode);
	//return(CompileShaderFromFile(L"C:\\Users\\aeiou\\Desktop\\SeniorProject\\Avoid-The-Boss\\Senior_project_ver2\\Shaders.hlsl", "PSMain", "ps_5_1", ppd3dShaderBlob));
}

//셰이더 소스 코드를 컴파일하여 바이트 코드 구조체를 반환한다. 
D3D12_SHADER_BYTECODE CShader::CompileShaderFromFile( const WCHAR* pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderProfile, ID3DBlob** ppd3dShaderBlob)
{
	UINT nCompileFlags = 0;
#if defined(_DEBUG)
	nCompileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif //Microsoft HLSL(High Level Shader Language) 코드를 지정된 대상에 대한 바이트코드로 컴파일
	::D3DCompileFromFile(pszFileName, NULL, NULL, pszShaderName, pszShaderProfile,
		nCompileFlags, 0, ppd3dShaderBlob, NULL);

	D3D12_SHADER_BYTECODE d3dShaderByteCode;
	d3dShaderByteCode.BytecodeLength = (*ppd3dShaderBlob)->GetBufferSize();
	d3dShaderByteCode.pShaderBytecode = (*ppd3dShaderBlob)->GetBufferPointer();

	return(d3dShaderByteCode);
}

//그래픽스 파이프라인 상태 객체를 생성한다. 
void CShader::CreateShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	//그래픽스 파이프라인 상태 객체 배열을 생성한다. 
	//m_nPipelineStates = 1;
	//m_ppd3dPipelineStates = new ID3D12PipelineState * [m_nPipelineStates];

	ID3DBlob* pd3dVertexShaderBlob = NULL;
	ID3DBlob* pd3dPixelShaderBlob = NULL;

	//---그래픽 파이프라인 상태 디스크립터 설정
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
	//파이프라인에 그래픽스 상태 객체를 설정한다. 
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
	return(CShader::CompileShaderFromFile(L"C:\\Users\\aeiou\\Desktop\\SeniorProject\\Avoid-The-Boss\\Senior_project_ver2\\Shaders.hlsl", "VSDiffused", "vs_5_1",
		ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE CPlayerShader::CreatePixelShader(ID3DBlob** ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"C:\\Users\\aeiou\\Desktop\\SeniorProject\\Avoid-The-Boss\\Senior_project_ver2\\Shaders.hlsl", "PSDiffused", "ps_5_1",
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

void CObjectsShader::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	//가로x세로x높이가 12x12x12인 정육면체 메쉬를 생성한다. 
	CCubeMeshDiffused *pCubeMesh = new CCubeMeshDiffused(pd3dDevice, pd3dCommandList, 
	12.0f, 12.0f, 12.0f);

	/*x-축, y-축, z-축 양의 방향의 객체 개수이다. 각 값을 1씩 늘리거나 줄이면서 실행할 때 프레임 레이트가 어떻게
	변하는 가를 살펴보기 바란다.*/
	int xObjects = 10, yObjects = 10, zObjects = 10, i = 0;

	//x-축, y-축, z-축으로 21개씩 총 21 x 21 x 21 = 9261개의 정육면체를 생성하고 배치한다.
	m_nObjects = (xObjects * 2 + 1) * (yObjects * 2 + 1) * (zObjects * 2 + 1);

	m_ppObjects = new CGameObject * [m_nObjects];

	float fxPitch = 12.0f * 2.5f;
	float fyPitch = 12.0f * 2.5f;
	float fzPitch = 12.0f * 2.5f;

	CRotatingObject* pRotatingObject = NULL;
	/*for (int x = -xObjects; x <= xObjects; x++)
	{
		for (int y = -yObjects; y <= yObjects; y++)
		{
			for (int z = -zObjects; z <= zObjects; z++)
			{
				pRotatingObject = new CRotatingObject();
				pRotatingObject->SetMesh(pCubeMesh);
				//각 정육면체 객체의 위치를 설정한다. 
				pRotatingObject->SetPosition(fxPitch*x, fyPitch*y, fzPitch*z);
				pRotatingObject->SetRotationAxis(XMFLOAT3(0.0f, 1.0f, 0.0f));
				pRotatingObject->SetRotationSpeed(10.0f * (i % 10) + 3.0f);
				m_ppObjects[i++] = pRotatingObject;
			}
		}
	}*/
	for (int z = zObjects; z >= -zObjects; z--)	// 깊이 검사 시 멀리있는 물체부터 렌더링 +z~-z
	{
		for (int y = -yObjects; y <= yObjects; y++)
		{
			for (int x = -xObjects; x <= xObjects; x++)
			{
				pRotatingObject = new CRotatingObject();
				pRotatingObject->SetMesh(pCubeMesh);
				//각 정육면체 객체의 위치를 설정한다. 
				pRotatingObject->SetPosition(fxPitch * x, fyPitch * y, fzPitch * z);
				pRotatingObject->SetRotationAxis(XMFLOAT3(0.0f, 1.0f, 0.0f));
				pRotatingObject->SetRotationSpeed(10.0f * (i % 10) + 3.0f);
				m_ppObjects[i++] = pRotatingObject;
			}
		}
	}
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

D3D12_SHADER_BYTECODE CObjectsShader::CreateVertexShader(ID3DBlob** ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"C:\\Users\\aeiou\\Desktop\\SeniorProject\\Avoid-The-Boss\\Senior_project_ver2\\Shaders.hlsl", "VSDiffused", "vs_5_1",
		ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE CObjectsShader::CreatePixelShader(ID3DBlob** ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"C:\\Users\\aeiou\\Desktop\\SeniorProject\\Avoid-The-Boss\\Senior_project_ver2\\Shaders.hlsl", "PSDiffused", "ps_5_1",
		ppd3dShaderBlob));
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
