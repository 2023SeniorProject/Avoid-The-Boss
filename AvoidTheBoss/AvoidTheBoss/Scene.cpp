#include "pch.h"
#include "Scene.h"
#include "GameFramework.h"
CGameScene::CGameScene()
{
}

CGameScene::~CGameScene()
{
}

void CGameScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
		//���콺 ĸ�ĸ� �ϰ� ���� ���콺 ��ġ�� �����´�. 
		::SetCapture(hWnd);
		::GetCursorPos(&m_ptOldCursorPos);
		break;
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
		//���콺 ĸ�ĸ� �����Ѵ�. 
		::ReleaseCapture();
		break;
	case WM_MOUSEMOVE:
		break;
	default:
		break;
	}
}

void CGameScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_KEYUP:
		switch (wParam)
		{
		case VK_ESCAPE:
			::PostQuitMessage(0);
			break;
		case VK_RETURN:
			break;
			/*��F1�� Ű�� ������ 1��Ī ī�޶�, ��F3�� Ű�� ������ 3��Ī ī�޶�� �����Ѵ�.*/
		case VK_F1:
			if (clientIocpCore._client->_player)m_pCamera = clientIocpCore._client->_player->ChangeCamera(FIRST_PERSON_CAMERA, m_Timer.GetTimeElapsed());
			break;
		case VK_F3:
			if (clientIocpCore._client->_player) m_pCamera = clientIocpCore._client->_player->ChangeCamera(THIRD_PERSON_CAMERA, m_Timer.GetTimeElapsed());
			break;
		case VK_F9:
			//��F9�� Ű�� �������� ������ ���� ��üȭ�� ����� ��ȯ�� ó���Ѵ�. 
			gGameFramework.ChangeSwapChainState();
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
}


void CGameScene::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	

	//�׷��� ��Ʈ �ñ׳��ĸ� �����Ѵ�. 
	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);

	m_nShaders = 1;
	m_pShaders = new CObjectsShader[m_nShaders];
	m_pShaders[0].CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	m_pShaders[0].BuildObjects(pd3dDevice, pd3dCommandList, NULL);

	clientIocpCore._client->_player = new CCubePlayer(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, 1);
	clientIocpCore._client->_other = new DummyCubePlayer(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, 1);
	clientIocpCore._client->_other->SetPosition(XMFLOAT3(0, 75, 0));
	m_pCamera = clientIocpCore._client->_player->GetCamera();
	InitScene();
}

void CGameScene::InitScene()
{
	m_Timer.Reset();
}

void CGameScene::ReleaseObjects()
{
	if (m_pd3dGraphicsRootSignature) m_pd3dGraphicsRootSignature->Release();

	for (int i = 0; i < m_nShaders; i++)
	{
		m_pShaders[i].ReleaseShaderVariables();
		m_pShaders[i].ReleaseObjects();
	}
	if (m_pShaders) delete[] m_pShaders;
}

void CGameScene::ProcessInput(HWND hWnd)
{
	m_Timer.Tick(0.0f);
	static UCHAR pKeyBuffer[256];
	// ����Ű�� ����Ʈ�� ó���Ѵ�.

	uint8 dwDirection = 0;
	if (::GetKeyboardState(pKeyBuffer))
	{
		if (pKeyBuffer[VK_UP] & 0xF0) dwDirection |= DIR_FORWARD;
		if (pKeyBuffer[VK_DOWN] & 0xF0) dwDirection |= DIR_BACKWARD;
		if (pKeyBuffer[VK_LEFT] & 0xF0) dwDirection |= DIR_LEFT;
		if (pKeyBuffer[VK_RIGHT] & 0xF0) dwDirection |= DIR_RIGHT;

	}



	float cxDelta = 0.0f, cyDelta = 0.0f;
	POINT ptCursorPos;
	if (::GetCapture() == hWnd)
	{
		//���콺 Ŀ���� ȭ�鿡�� ���ش�(������ �ʰ� �Ѵ�).
		::SetCursor(NULL);
		//���� ���콺 Ŀ���� ��ġ�� �����´�. 
		::GetCursorPos(&ptCursorPos);
		//���콺 ��ư�� ���� ���¿��� ���콺�� ������ ���� ���Ѵ�. 
		cxDelta = (float)(ptCursorPos.x - m_ptOldCursorPos.x) / 3.0f;
		cyDelta = (float)(ptCursorPos.y - m_ptOldCursorPos.y) / 3.0f;
		//���콺 Ŀ���� ��ġ�� ���콺�� �������� ��ġ�� �����Ѵ�. 
		::SetCursorPos(m_ptOldCursorPos.x, m_ptOldCursorPos.y);


	}


	if ((dwDirection != 0) || (cxDelta != 0.0f) || (cyDelta != 0.0f))
	{
		if (cxDelta || cyDelta)
		{

			/*cxDelta�� y-���� ȸ���� ��Ÿ���� cyDelta�� x-���� ȸ���� ��Ÿ����. ������ ���콺 ��ư�� ������ ���
			cxDelta�� z-���� ȸ���� ��Ÿ����.*/
			if (pKeyBuffer[VK_RBUTTON] & 0xF0) clientIocpCore._client->_player->Rotate(cyDelta, 0.0f, -cxDelta);
			else if (pKeyBuffer[VK_LBUTTON] & 0xF0) clientIocpCore._client->_player->Rotate(cyDelta, cxDelta, 0.0f);

			if (pKeyBuffer[VK_LBUTTON] & 0xF0)
			{
				C2S_ROTATE packet;
				packet.size = sizeof(C2S_ROTATE);
				packet.type = C_PACKET_TYPE::ROTATE;
				packet.angle = cxDelta;
				clientIocpCore._client->DoSend(&packet);
			}
		}

		/*�÷��̾ dwDirection �������� �̵��Ѵ�(�����δ� �ӵ� ���͸� �����Ѵ�). �̵� �Ÿ��� �ð��� ����ϵ��� �Ѵ�. �÷��̾��� �̵� �ӷ��� (1.3UNIT/��)�� �����Ѵ�.*/
		if (dwDirection) clientIocpCore._client->_player->Move(dwDirection, (1.2f * UNIT));
		// �ӵ��� �����ְ� 

	}


	if (m_lastKeyInput != dwDirection || (cxDelta != 0.0f) || (cyDelta != 0.0f)) // ������ ����(Ű�Է��� �ٸ� ��쿡�� ���� �̺�Ʈ ��Ŷ�� ������)
	{

		C2S_MOVE packet;
		packet.size = sizeof(C2S_MOVE);
		packet.type = C_PACKET_TYPE::MOVE;
		packet.key = dwDirection;

		clientIocpCore._client->DoSend(&packet);
	}
	m_lastKeyInput = dwDirection;

	//ī�޶� �����Ѵ�. �߷°� �������� ������ �ӵ� ���Ϳ� �����Ѵ�.
	{
		std::lock_guard<std::mutex> lg(clientIocpCore._client->_otherLock);
		clientIocpCore._client->_other->Update(m_Timer.GetTimeElapsed());
	}
	clientIocpCore._client->_playerLock.lock();
	clientIocpCore._client->_player->Update(m_Timer.GetTimeElapsed());
	clientIocpCore._client->_playerLock.unlock();
}

void CGameScene::AnimateObjects()
{
	for (int i = 0; i < m_nShaders; i++)
	{
		m_pShaders[i].AnimateObjects(m_Timer.GetTimeElapsed());
	}
}

void CGameScene::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	//�׷��� ��Ʈ �ñ׳��ĸ� ���������ο� ����(����)�Ѵ�. 
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);
	pCamera->UpdateShaderVariables(pd3dCommandList);

	//���� �������ϴ� ���� ���� �����ϴ� ���� ��ü(���̴��� �����ϴ� ��ü)���� �������ϴ� ���̴�. 
	for (int i = 0; i < m_nShaders; i++)
	{
		m_pShaders[i].Render(pd3dCommandList, pCamera);
	}
}

void CGameScene::ReleaseUploadBuffers()
{
	
	for (int i = 0; i < m_nShaders; i++) m_pShaders[i].ReleaseUploadBuffers();

}

ID3D12RootSignature* CGameScene::CreateGraphicsRootSignature(ID3D12Device* pd3dDevice)
{
	ID3D12RootSignature* pd3dGraphicsRootSignature = NULL;

	D3D12_ROOT_PARAMETER pd3dRootParameters[2];
	pd3dRootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS; //32bit constant
	pd3dRootParameters[0].Constants.Num32BitValues = 16;
	pd3dRootParameters[0].Constants.ShaderRegister = 0;
	pd3dRootParameters[0].Constants.RegisterSpace = 0;
	pd3dRootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	pd3dRootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	pd3dRootParameters[1].Constants.Num32BitValues = 32;
	pd3dRootParameters[1].Constants.ShaderRegister = 1;
	pd3dRootParameters[1].Constants.RegisterSpace = 0;
	pd3dRootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	D3D12_ROOT_SIGNATURE_FLAGS d3dRootSignatureFlags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;
	// ��Ʈ �ñ׳��ĸ� �����Ѵ�. 
	D3D12_ROOT_SIGNATURE_DESC d3dRootSignatureDesc;
	::ZeroMemory(&d3dRootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	d3dRootSignatureDesc.NumParameters = _countof(pd3dRootParameters);
	d3dRootSignatureDesc.pParameters = pd3dRootParameters;
	d3dRootSignatureDesc.NumStaticSamplers = 0;
	d3dRootSignatureDesc.pStaticSamplers = NULL;
	d3dRootSignatureDesc.Flags = d3dRootSignatureFlags;

	ID3DBlob* pd3dSignatureBlob = NULL;
	ID3DBlob* pd3dErrorBlob = NULL;
	::D3D12SerializeRootSignature(&d3dRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1,
		&pd3dSignatureBlob, &pd3dErrorBlob); //ID3D12Device::CreateRootSignature �� ������ �� �ִ� ��Ʈ ���� ���� 1.0�� ����ȭ
	pd3dDevice->CreateRootSignature(0, pd3dSignatureBlob->GetBufferPointer(),
		pd3dSignatureBlob->GetBufferSize(), __uuidof(ID3D12RootSignature), (void
			**)&pd3dGraphicsRootSignature); // ��Ʈ �ñ״�ó ����
	if (pd3dSignatureBlob) pd3dSignatureBlob->Release();
	if (pd3dErrorBlob) pd3dErrorBlob->Release();

	return(pd3dGraphicsRootSignature);
}

ID3D12RootSignature* CGameScene::GetGraphicsRootSignature()
{
	return(m_pd3dGraphicsRootSignature);
}