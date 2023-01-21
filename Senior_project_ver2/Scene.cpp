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

ID3D12RootSignature* CScene::CreateGraphicsRootSignature(ID3D12Device* pd3dDevice)
{
	ID3D12RootSignature* pd3dGraphicsRootSignature = NULL;

	//매개변수가 없는 루트 시그너쳐를 생성한다. 
	D3D12_ROOT_SIGNATURE_DESC d3dRootSignatureDesc;
	::ZeroMemory(&d3dRootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	d3dRootSignatureDesc.NumParameters = 0;
	d3dRootSignatureDesc.pParameters = NULL;
	d3dRootSignatureDesc.NumStaticSamplers = 0;
	d3dRootSignatureDesc.pStaticSamplers = NULL;
	d3dRootSignatureDesc.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	ID3DBlob* pd3dSignatureBlob = NULL;
	ID3DBlob* pd3dErrorBlob = NULL;
	::D3D12SerializeRootSignature(&d3dRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1,
		&pd3dSignatureBlob, &pd3dErrorBlob); //ID3D12Device::CreateRootSignature 에 전달할 수 있는 루트 서명 버전 1.0을 직렬화
	pd3dDevice->CreateRootSignature(0, pd3dSignatureBlob->GetBufferPointer(),
		pd3dSignatureBlob->GetBufferSize(), __uuidof(ID3D12RootSignature), (void
			**)&m_pd3dGraphicsRootSignature); // 루트 시그니처 생성
	if (pd3dSignatureBlob) pd3dSignatureBlob->Release();
	if (pd3dErrorBlob) pd3dErrorBlob->Release();

	return(pd3dGraphicsRootSignature);
}

ID3D12RootSignature* CScene::GetGraphicsRootSignature()
{
	return(m_pd3dGraphicsRootSignature);
}

void CScene::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	//그래픽 루트 시그너쳐를 생성한다. 
	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);

	//씬을 그리기 위한 셰이더 객체를 생성한다. 
	m_nShaders = 1;
	m_ppShaders = new CShader * [m_nShaders];

	CShader* pShader = new CShader();
	pShader->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	pShader->BuildObjects(pd3dDevice, pd3dCommandList, NULL);

	m_ppShaders[0] = pShader;
}

void CScene::ReleaseObjects()
{
	if (m_pd3dGraphicsRootSignature) m_pd3dGraphicsRootSignature->Release();

	if (m_ppShaders)
	{
		for (int i = 0; i < m_nShaders; i++)
		{
			m_ppShaders[i]->ReleaseShaderVariables();
			m_ppShaders[i]->ReleaseObjects();
			m_ppShaders[i]->Release();
		}
		delete[] m_ppShaders;
	}
}

bool CScene::ProcessInput()
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

void CScene::Render(ID3D12GraphicsCommandList* pd3dCommandList)
{
	//그래픽 루트 시그너쳐를 파이프라인에 연결(설정)한다. 
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);

	//씬을 렌더링하는 것은 씬을 구성하는 셰이더(셰이더가 포함하는 객체)들을 렌더링하는 것이다. 
	for (int i = 0; i < m_nShaders; i++)
	{
		m_ppShaders[i]->Render(pd3dCommandList);
	}
}

void CScene::ReleaseUploadBuffers()
{
	if (m_ppShaders)
	{
		for (int j = 0; j < m_nShaders; j++)
		{
			if (m_ppShaders[j])
				m_ppShaders[j]->ReleaseUploadBuffers();
		}
	}
}