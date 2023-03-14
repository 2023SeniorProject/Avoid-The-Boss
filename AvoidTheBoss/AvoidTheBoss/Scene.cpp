#include "pch.h"
#include "Scene.h"
#include "GameFramework.h"
#include "clientIocpCore.h"

CGameScene::CGameScene()
{
	_curSceneStatus = SceneInfo::GAMEROOM;
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
	/*	case VK_F1:
			if (_player)m_pCamera = _player->ChangeCamera(FIRST_PERSON_CAMERA, m_Timer.GetTimeElapsed());
			break;
		case VK_F3:
			if (_player) m_pCamera = _player->ChangeCamera(THIRD_PERSON_CAMERA, m_Timer.GetTimeElapsed());
			break;*/
		case VK_F9:
			//��F9�� Ű�� �������� ������ ���� ��üȭ�� ����� ��ȯ�� ó���Ѵ�. 
			mainGame.ChangeSwapChainState();
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
}


void CGameScene::BuildDefaultLightsAndMaterials()
{
}

void CGameScene::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	

	//�׷��� ��Ʈ �ñ׳��ĸ� �����Ѵ�. 
	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);

	/*m_nShaders = 1;
	m_ppShaders = new CShader*[m_nShaders];

	CObjectsShader* pObjectShader = new CObjectsShader();
	pObjectShader->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	pObjectShader->BuildObjects(pd3dDevice, pd3dCommandList, NULL);
	m_ppShaders[0] = pObjectShader;*/
	
	CMaterial::PrepareShaders(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
	BuildDefaultLightsAndMaterials();

	for (int i = 0; i < PLAYERNUM; ++i)
	{
		_players[i] = new CMyPlayer(pd3dDevice, pd3dCommandList, GetGraphicsRootSignature());
		_players[i]->SetPosition(XMFLOAT3(0, 75, 0));
	}

	m_pCamera = _players[0]->GetCamera();

	CGameObject* pMap = CGameObject::LoadGeometryFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Industry_Map_(1).bin");

	CGameObject* pMapObject = NULL;
	pMapObject = new CGameObject();
	pMapObject->SetChild(pMap, true);
	pMapObject->SetPosition(0.0f, 0.0f, 0.0f);
	pMapObject->SetScale(1.0f, 1.0f, 1.0f);
	pMapObject->Rotate(0.0f, 0.0f, 0.0f);
	m_ppGameObjects[0] = pMapObject;

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	
	
	m_Timer.Reset();
}

void CGameScene::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
}

void CGameScene::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
}

void CGameScene::ReleaseShaderVariables()
{
}

void CGameScene::ReleaseObjects()
{
	if (m_pd3dGraphicsRootSignature)
		m_pd3dGraphicsRootSignature->Release();

	//for (int i = 0; i < m_nShaders; i++)
	//{
	//	m_ppShaders[i]->ReleaseShaderVariables();
	//	m_ppShaders[i]->ReleaseObjects();
	//	m_ppShaders[i]->Release();
	//}
	//delete[] m_ppShaders;

	ReleaseShaderVariables();
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
	std::cout << "_playerIdx\n";
	_players[_playerIdx]->m_lock.lock();
	if ((dwDirection != 0) || (cxDelta != 0.0f) || (cyDelta != 0.0f))
	{
		if (cxDelta || cyDelta)
		{

			/*cxDelta�� y-���� ȸ���� ��Ÿ���� cyDelta�� x-���� ȸ���� ��Ÿ����. ������ ���콺 ��ư�� ������ ���
			cxDelta�� z-���� ȸ���� ��Ÿ����.*/
			if (pKeyBuffer[VK_RBUTTON] & 0xF0) _players[_playerIdx]->Rotate(cyDelta, 0.0f, -cxDelta);
			else if (pKeyBuffer[VK_LBUTTON] & 0xF0) _players[_playerIdx]->Rotate(cyDelta, cxDelta, 0.0f);

			if (pKeyBuffer[VK_LBUTTON] & 0xF0)
			{
				C2S_ROTATE packet;
				packet.size = sizeof(C2S_ROTATE);
				packet.type = C_PACKET_TYPE::ROTATE;
				packet.angle = cxDelta;
				clientCore._client->DoSend(&packet);
			}
		}

		/*�÷��̾ dwDirection �������� �̵��Ѵ�(�����δ� �ӵ� ���͸� �����Ѵ�). �̵� �Ÿ��� �ð��� ����ϵ��� �Ѵ�. �÷��̾��� �̵� �ӷ��� (1.3UNIT/��)�� �����Ѵ�.*/
		if (dwDirection) _players[_playerIdx]->Move(dwDirection, PLAYER_VELOCITY);
		// �ӵ��� �����ְ� 

	}


	if (m_lastKeyInput != dwDirection || (cxDelta != 0.0f) || (cyDelta != 0.0f)) // ������ ����(Ű�Է��� �ٸ� ��쿡�� ���� �̺�Ʈ ��Ŷ�� ������)
	{

		C2S_MOVE packet;
		packet.size = sizeof(C2S_MOVE);
		packet.type = C_PACKET_TYPE::MOVE;
		packet.key = dwDirection;

		clientCore._client->DoSend(&packet);
	}
	m_lastKeyInput = dwDirection;
	_players[_playerIdx]->m_lock.unlock();
	//ī�޶� �����Ѵ�. �߷°� �������� ������ �ӵ� ���Ϳ� �����Ѵ�.
	for (int k = 0; k < PLAYERNUM; ++k)
	{
		_players[k]->Update(m_Timer.GetTimeElapsed());
	}
}

void CGameScene::AnimateObjects()
{
	/*for (int i = 0; i < m_nShaders; i++)
	{
		m_ppShaders[i]->AnimateObjects(m_Timer.GetTimeElapsed());
	}*/
}

void CGameScene::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);

	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	//�׷��� ��Ʈ �ñ׳��ĸ� ���������ο� ����(����)�Ѵ�. 
	pCamera->UpdateShaderVariables(pd3dCommandList);

	UpdateShaderVariables(pd3dCommandList);
	
	//���� �������ϴ� ���� ���� �����ϴ� ���� ��ü(���̴��� �����ϴ� ��ü)���� �������ϴ� ���̴�. 
	//for (int i = 0; i < m_nShaders; i++)
	//{
	//	m_ppShaders[i]->Render(pd3dCommandList, pCamera);
	//}
}

void CGameScene::ReleaseUploadBuffers()
{
	
	//for (int i = 0; i < m_nShaders; i++) 
	//	m_ppShaders[i]->ReleaseUploadBuffers();

}

ID3D12RootSignature* CGameScene::CreateGraphicsRootSignature(ID3D12Device* pd3dDevice)
{
	// ������ ���� ����
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

	// ��Ʈ������ ���� ����
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
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS; //����-���ٽ�
	//samplerDesc.BorderColor = ;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
	samplerDesc.ShaderRegister = 0; //s0
	samplerDesc.RegisterSpace = 0; //0�� �⺻��
	samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		

	// ��Ʈ �ñ׳��� ������ ���� 
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

	// ID3D12Device::CreateRootSignature �� ������ �� �ִ� ��Ʈ ���� ���� 1.0�� ����ȭ
	D3D12SerializeRootSignature(
				&RootSignatureDesc, 
							D3D_ROOT_SIGNATURE_VERSION_1,
						&pd3dSignatureBlob, 
					&pd3dErrorBlob
	); 

	ID3D12RootSignature* pd3dGraphicsRootSignature = NULL;
	// ��Ʈ �ñ״�ó ����
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

ID3D12RootSignature* CGameScene::GetGraphicsRootSignature()
{
	return(m_pd3dGraphicsRootSignature);
}