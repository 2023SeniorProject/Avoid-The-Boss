#include "pch.h"
#include "Scene.h"
#include "GameFramework.h"
#include "clientIocpCore.h"

#define MAPVOLUME 50

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
	m_nLights = 1;
	m_pLights = new LIGHT[m_nLights];
	::ZeroMemory(m_pLights, sizeof(LIGHT) * m_nLights);

	float ambiant = 0.15;
	m_xmf4GlobalAmbient = XMFLOAT4(ambiant, ambiant, ambiant, 1.0f);

	m_pLights[0].m_bEnable = true;
	m_pLights[0].m_nType = DIRECTIONAL_LIGHT;
	m_pLights[0].m_xmf4Ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	m_pLights[0].m_xmf4Diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	m_pLights[0].m_xmf4Specular = XMFLOAT4(0.4f, 0.4f, 0.4f, 0.0f);
	m_pLights[0].m_xmf3Direction = XMFLOAT3(0.0f, 1.0f, -1.0f);

	/*m_pLights[0].m_bEnable = true;
	m_pLights[0].m_nType = POINT_LIGHT;
	m_pLights[0].m_fRange = 1000.0f;
	m_pLights[0].m_xmf4Ambient = XMFLOAT4(0.1f, 0.0f, 0.0f, 1.0f);
	m_pLights[0].m_xmf4Diffuse = XMFLOAT4(0.8f, 0.0f, 0.0f, 1.0f);
	m_pLights[0].m_xmf4Specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 0.0f);
	m_pLights[0].m_xmf3Position = XMFLOAT3(30.0f, 30.0f, 30.0f);
	m_pLights[0].m_xmf3Direction = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_pLights[0].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.001f, 0.0001f);
	m_pLights[1].m_bEnable = true;
	m_pLights[1].m_nType = SPOT_LIGHT;
	m_pLights[1].m_fRange = 500.0f;
	m_pLights[1].m_xmf4Ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	m_pLights[1].m_xmf4Diffuse = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
	m_pLights[1].m_xmf4Specular = XMFLOAT4(0.3f, 0.3f, 0.3f, 0.0f);
	m_pLights[1].m_xmf3Position = XMFLOAT3(-50.0f, 20.0f, -5.0f);
	m_pLights[1].m_xmf3Direction = XMFLOAT3(0.0f, 0.0f, 1.0f);
	m_pLights[1].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.01f, 0.0001f);
	m_pLights[1].m_fFalloff = 8.0f;
	m_pLights[1].m_fPhi = (float)cos(XMConvertToRadians(40.0f));
	m_pLights[1].m_fTheta = (float)cos(XMConvertToRadians(20.0f));
	m_pLights[2].m_bEnable = true;
	m_pLights[2].m_nType = DIRECTIONAL_LIGHT;
	m_pLights[2].m_xmf4Ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	m_pLights[2].m_xmf4Diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	m_pLights[2].m_xmf4Specular = XMFLOAT4(0.4f, 0.4f, 0.4f, 0.0f);
	m_pLights[2].m_xmf3Direction = XMFLOAT3(1.0f, 0.0f, 0.0f);
	m_pLights[3].m_bEnable = true;
	m_pLights[3].m_nType = SPOT_LIGHT;
	m_pLights[3].m_fRange = 600.0f;
	m_pLights[3].m_xmf4Ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	m_pLights[3].m_xmf4Diffuse = XMFLOAT4(0.3f, 0.7f, 0.0f, 1.0f);
	m_pLights[3].m_xmf4Specular = XMFLOAT4(0.3f, 0.3f, 0.3f, 0.0f);
	m_pLights[3].m_xmf3Position = XMFLOAT3(50.0f, 30.0f, 30.0f);
	m_pLights[3].m_xmf3Direction = XMFLOAT3(0.0f, 1.0f, 1.0f);
	m_pLights[3].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.01f, 0.0001f);
	m_pLights[3].m_fFalloff = 8.0f;
	m_pLights[3].m_fPhi = (float)cos(XMConvertToRadians(90.0f));
	m_pLights[3].m_fTheta = (float)cos(XMConvertToRadians(30.0f));*/
}

void CGameScene::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	BoxTree = new OcTree(XMFLOAT3(0, 0, 0), 100);
	BoxTree->BuildTree();
	//�׷��� ��Ʈ �ñ׳��ĸ� �����Ѵ�. 
	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);

	CMaterial::PrepareShaders(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
	BuildDefaultLightsAndMaterials();

	m_nGameObjects = 1;
	m_ppGameObjects = new CGameObject * [m_nGameObjects];
	// CGameObject* pMap = CGameObject::LoadGeometryFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Industry_Map.bin");

	for (int i = 0; i < PLAYERNUM; ++i)
	{
		_players[i] = new CWorker(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
	}
	m_pCamera = _players[0]->GetCamera();



	/*CGameObject* pMapObject = NULL;
	pMapObject = new CGameObject();
	pMapObject->SetChild(pMap, true);
	pMapObject->SetPosition(0.0f, 0.0f, 0.0f);
	pMapObject->SetScale(1.0f, 1.0f, 1.0f);
	pMapObject->Rotate(0.0f, 0.0f, 0.0f);
	m_ppGameObjects[0] = pMapObject;*/


	CGameObject* pBVMap = CGameObject::LoadGeometryFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Map_Bounding_Box.bin");
	pBVMap->SetPosition(0.0f, 0.0f, 0.0f);
	CGameObject* pBVObject = NULL;
	pBVObject = new CGameObject();
	pBVObject->SetChild(pBVMap, true);
	pBVObject->SetPosition(0.0f, 0.0f, 0.0f);
	pBVObject->SetScale(1.0f, 1.0f, 1.0f);
	pBVObject->Rotate(0.0f, 0.0f, 0.0f);
	m_ppGameObjects[0] = pBVObject;

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void CGameScene::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(LIGHTS) + 255) & ~255); //256�� ���
	m_pd3dcbLights = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbLights->Map(0, NULL, (void**)&m_pcbMappedLights);
}

void CGameScene::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	::memcpy(m_pcbMappedLights->m_pLights, m_pLights, sizeof(LIGHT) * m_nLights);
	::memcpy(&m_pcbMappedLights->m_xmf4GlobalAmbient, &m_xmf4GlobalAmbient, sizeof(XMFLOAT4));
	::memcpy(&m_pcbMappedLights->m_nLights, &m_nLights, sizeof(int));
}

void CGameScene::ReleaseShaderVariables()
{
	if (m_pd3dcbLights)
	{
		m_pd3dcbLights->Unmap(0, NULL);
		m_pd3dcbLights->Release();
	}
}

void CGameScene::ReleaseObjects()
{
	if (m_pd3dGraphicsRootSignature)
		m_pd3dGraphicsRootSignature->Release();

	if (m_ppGameObjects)
	{
		for (int i = 0; i < m_nGameObjects; i++)
			if (m_ppGameObjects[i]) m_ppGameObjects[i]->Release();
		delete[] m_ppGameObjects;
	}

	ReleaseShaderVariables();

	if (m_pLights) delete[] m_pLights;
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
	{
		std::lock_guard<std::mutex> lg(_players[_playerIdx]->m_lock);
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
		}
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
	

	//ī�޶� �����Ѵ�. �߷°� �������� ������ �ӵ� ���Ϳ� �����Ѵ�.
	for (int k = 0; k < PLAYERNUM; ++k)
	{	
		if (k == _playerIdx) _players[k]->Update(m_Timer.GetTimeElapsed(),PLAYER_TYPE::OWNER);
		else _players[k]->Update(m_Timer.GetTimeElapsed(),PLAYER_TYPE::OTHER_PLAYER);
	}


}

void CGameScene::AnimateObjects()
{
	for (int i = 0; i < m_nGameObjects; i++) m_ppGameObjects[i]->Animate(m_Timer.GetTimeElapsed(), NULL);

	if (m_pLights)
	{
		m_pLights[1].m_xmf3Position = _players[_playerIdx]->GetPosition();
		m_pLights[1].m_xmf3Direction = _players[_playerIdx]->GetLookVector();
	}
}

void CGameScene::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);

	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	//�׷��� ��Ʈ �ñ׳��ĸ� ���������ο� ����(����)�Ѵ�. 
	pCamera->UpdateShaderVariables(pd3dCommandList);

	UpdateShaderVariables(pd3dCommandList);
	
	D3D12_GPU_VIRTUAL_ADDRESS d3dcbLightsGpuVirtualAddress = m_pd3dcbLights->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(2, d3dcbLightsGpuVirtualAddress); //Lights


	for (int i = 0; i < m_nGameObjects; i++)
	{
		if (m_ppGameObjects[i])
		{
			m_ppGameObjects[i]->Animate(m_Timer.GetTimeElapsed(), NULL);
			m_ppGameObjects[i]->UpdateTransform(NULL);
			m_ppGameObjects[i]->Render(pd3dCommandList, pCamera);
		}
	}
	for (int i = 0; i < PLAYERNUM; ++i) _players[i]->Render(pd3dCommandList, pCamera);
}

bool CGameScene::CollisionCheck()
{
	for(auto& i : bv) if(i.Intersects(_players[_playerIdx]->m_playerBV)) return true;
 	return false;
}

void CGameScene::ReleaseUploadBuffers()
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
	//for (int i = 0; i < m_nShaders; i++) 
	//	m_ppShaders[i]->ReleaseUploadBuffers();

	for (int i = 0; i < m_nGameObjects; i++)
		m_ppGameObjects[i]->ReleaseUploadBuffers();
}

ID3D12RootSignature* CGameScene::CreateGraphicsRootSignature(ID3D12Device* pd3dDevice)
{
	ID3D12RootSignature* pd3dGraphicsRootSignature = NULL;

	D3D12_ROOT_PARAMETER pd3dRootParameters[3];

	pd3dRootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[0].Descriptor.ShaderRegister = 1; //Camera
	pd3dRootParameters[0].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	pd3dRootParameters[1].Constants.Num32BitValues = 32;
	pd3dRootParameters[1].Constants.ShaderRegister = 2; //GameObject
	pd3dRootParameters[1].Constants.RegisterSpace = 0;
	pd3dRootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[2].Descriptor.ShaderRegister = 4; //Lights
	pd3dRootParameters[2].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;


	// ��Ʈ �ñ׳��� ������ ���� 
	D3D12_ROOT_SIGNATURE_DESC RootSignatureDesc;
	::ZeroMemory(&RootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	RootSignatureDesc.NumParameters = _countof(pd3dRootParameters);
	RootSignatureDesc.pParameters = pd3dRootParameters;
	RootSignatureDesc.NumStaticSamplers = 0;
	RootSignatureDesc.pStaticSamplers = NULL;
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

	//ID3D12RootSignature* pd3dGraphicsRootSignature = NULL;
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