#include "stdafx.h"
#include "Scene.h"

CScene::CScene()
{
}

CScene::~CScene()
{
}

bool CScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return false;
}

bool CScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return false;
}


void CScene::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	//그래픽 루트 시그너쳐를 생성한다. 
	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);

	m_nShaders = 1;
	m_ppShaders = new CShader*[m_nShaders];

	CObjectsShader* pObjectShader = new CObjectsShader();
	pObjectShader->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	pObjectShader->BuildObjects(pd3dDevice, pd3dCommandList, NULL);
	m_ppShaders[0] = pObjectShader;

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void CScene::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
}

void CScene::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
}

void CScene::ReleaseShaderVariables()
{
}

void CScene::ReleaseObjects()
{
	if (m_pd3dGraphicsRootSignature)
		m_pd3dGraphicsRootSignature->Release();

	for (int i = 0; i < m_nShaders; i++)
	{
		m_ppShaders[i]->ReleaseShaderVariables();
		m_ppShaders[i]->ReleaseObjects();
		m_ppShaders[i]->Release();
	}
	delete[] m_ppShaders;

	ReleaseShaderVariables();
}

bool CScene::ProcessInput(UCHAR* pKeysBuffer)
{
	return false;
}

void CScene::AnimateObjects(float fTimeElapsed)
{
	for (int i = 0; i < m_nShaders; i++)
	{
		m_ppShaders[i]->AnimateObjects(fTimeElapsed);
	}
}

void CScene::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);

	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	//그래픽 루트 시그너쳐를 파이프라인에 연결(설정)한다. 
	pCamera->UpdateShaderVariables(pd3dCommandList);

	UpdateShaderVariables(pd3dCommandList);
	
	//씬을 렌더링하는 것은 씬을 구성하는 게임 객체(셰이더를 포함하는 객체)들을 렌더링하는 것이다. 
	for (int i = 0; i < m_nShaders; i++)
	{
		m_ppShaders[i]->Render(pd3dCommandList, pCamera);
	}
}

void CScene::ReleaseUploadBuffers()
{
	/*if (m_ppShaders)
	{
		for (int j = 0; j < m_nShaders; j++)
		{
			if (m_ppShaders[j])
				m_ppShaders[j]->ReleaseUploadBuffers();
		}
	}*/
	/*if (m_ppObjects)
	{
		for (int j = 0; j < m_nObjects; j++) if (m_ppObjects[j])
			m_ppObjects[j]->ReleaseUploadBuffers();
	}*/
	//for (int i = 0; i < m_nShaders; i++) 
		//m_ppShaders[i]->ReleaseUploadBuffers();
	for (int i = 0; i < m_nShaders; i++) 
		m_ppShaders[i]->ReleaseUploadBuffers();
}

ID3D12RootSignature* CScene::CreateGraphicsRootSignature(ID3D12Device* pd3dDevice)
{
	// 서술자 범위 지정
	D3D12_DESCRIPTOR_RANGE DescRange[2];
	DescRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	DescRange[0].NumDescriptors = 1;
	DescRange[0].BaseShaderRegister = 2; // c : obj
	DescRange[0].RegisterSpace = 0;
	DescRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	DescRange[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	DescRange[1].NumDescriptors = 1;
	DescRange[1].BaseShaderRegister = 0;  // ? : texture
	DescRange[1].RegisterSpace = 0;
	DescRange[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	// 루트서명의 슬롯 설명
	D3D12_ROOT_PARAMETER RootParameters[4]; 
	// b0 : player / b1 : camera / b2 : objectVertex / t0 : OnjectTexture
	RootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; //32bit constant
	RootParameters[0].Constants.ShaderRegister = 0;
	RootParameters[0].Constants.RegisterSpace = 0;
	RootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	RootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	RootParameters[1].Constants.ShaderRegister = 1;
	RootParameters[1].Constants.RegisterSpace = 0;
	RootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	RootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	RootParameters[2].DescriptorTable.NumDescriptorRanges = 1;
	RootParameters[2].DescriptorTable.pDescriptorRanges = &DescRange[0];
	RootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	RootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	RootParameters[3].DescriptorTable.NumDescriptorRanges = 1;
	RootParameters[3].DescriptorTable.pDescriptorRanges = &DescRange[1];
	RootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;


	D3D12_TEXTURE_ADDRESS_MODE textureMode = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	D3D12_STATIC_SAMPLER_DESC samplerDesc;
	::ZeroMemory(&samplerDesc, sizeof(D3D12_STATIC_SAMPLER_DESC));
	samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.MipLODBias = 0;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS; //깊이-스텐실
	//samplerDesc.BorderColor = ;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
	samplerDesc.ShaderRegister = 0; //s0
	samplerDesc.RegisterSpace = 0; //0이 기본값
	samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		

	// 루트 시그너쳐 서술자 생성 
	D3D12_ROOT_SIGNATURE_DESC RootSignatureDesc;
	::ZeroMemory(&RootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	RootSignatureDesc.NumParameters = _countof(RootParameters);
	RootSignatureDesc.pParameters = RootParameters;
	RootSignatureDesc.NumStaticSamplers = 1;
	RootSignatureDesc.pStaticSamplers = &samplerDesc;
	RootSignatureDesc.Flags = 
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;


	ID3DBlob* pd3dSignatureBlob = NULL;
	ID3DBlob* pd3dErrorBlob = NULL;

	// ID3D12Device::CreateRootSignature 에 전달할 수 있는 루트 서명 버전 1.0을 직렬화
	D3D12SerializeRootSignature(
				&RootSignatureDesc, 
							D3D_ROOT_SIGNATURE_VERSION_1,
						&pd3dSignatureBlob, 
					&pd3dErrorBlob
	); 

	ID3D12RootSignature* pd3dGraphicsRootSignature = NULL;
	// 루트 시그니처 생성
	pd3dDevice->CreateRootSignature(
						0, 
			pd3dSignatureBlob->GetBufferPointer(),
				pd3dSignatureBlob->GetBufferSize(),
						__uuidof(ID3D12RootSignature), 
				(void**)&pd3dGraphicsRootSignature
	); 

	if (pd3dSignatureBlob) 
		pd3dSignatureBlob->Release();
	if (pd3dErrorBlob) 
		pd3dErrorBlob->Release();

	return(pd3dGraphicsRootSignature);
}

ID3D12RootSignature* CScene::GetGraphicsRootSignature()
{
	return(m_pd3dGraphicsRootSignature);
}