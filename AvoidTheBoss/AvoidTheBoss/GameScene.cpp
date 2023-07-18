#include "pch.h"
#include "GameScene.h"
#include "GameFramework.h"
#include "UIManager.h"
#include "InputManager.h"
#include "SoundManager.h"
//��Ʈ��ũ ����
#include "clientIocpCore.h"
#include "CJobQueue.h"

// ��ü ����
#include "CBoss.h"
#include "CEmployee.h"
#include "CGenerator.h"



#define MAPVOLUME 50

CGameScene::CGameScene()
{
	m_curFrame = 0;
	m_jobQueue = new Scheduler();
}

CGameScene::~CGameScene()
{
	delete m_jobQueue;
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

// Ư��Ű ó���� ���� ��
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
	m_nLights = 5;
	m_pLights = new LIGHT[m_nLights];
	::ZeroMemory(m_pLights, sizeof(LIGHT) * m_nLights);

	m_xmf4GlobalAmbient = XMFLOAT4(0.15f, 0.15f, 0.15f, 1.0f);

	m_pLights[0].m_bEnable = false;
	m_pLights[0].m_nType = POINT_LIGHT;
	m_pLights[0].m_fRange = 300.0f;
	m_pLights[0].m_xmf4Ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	m_pLights[0].m_xmf4Diffuse = XMFLOAT4(0.4f, 0.3f, 0.8f, 1.0f);
	m_pLights[0].m_xmf4Specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 0.0f);
	m_pLights[0].m_xmf3Position = XMFLOAT3(230.0f, 330.0f, 480.0f);
	m_pLights[0].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.001f, 0.0001f);

	m_pLights[1].m_bEnable = false;
	m_pLights[1].m_nType = SPOT_LIGHT;
	m_pLights[1].m_fRange = 500.0f;
	m_pLights[1].m_xmf4Ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	m_pLights[1].m_xmf4Diffuse = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
	m_pLights[1].m_xmf4Specular = XMFLOAT4(0.3f, 0.3f, 0.3f, 0.0f);
	m_pLights[1].m_xmf3Position = XMFLOAT3(-50.0f, 20.0f, -5.0f);
	m_pLights[1].m_xmf3Direction = XMFLOAT3(0.0f, -1.0f, 1.0f);
	m_pLights[1].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.01f, 0.0001f);
	m_pLights[1].m_fFalloff = 8.0f;
	m_pLights[1].m_fPhi = (float)cos(XMConvertToRadians(40.0f));
	m_pLights[1].m_fTheta = (float)cos(XMConvertToRadians(20.0f));


	//m_pLights[2].m_bEnable = true;
	//m_pLights[2].m_nType = DIRECTIONAL_LIGHT;
	//m_pLights[2].m_xmf4Ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	//m_pLights[2].m_xmf4Diffuse = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	//m_pLights[2].m_xmf4Specular = XMFLOAT4(0.4f, 0.4f, 0.4f, 0.0f);
	//m_pLights[2].m_xmf3Direction = XMFLOAT3(1.0f, -1.0f, 0.0f);

	m_pLights[2].m_bEnable = true;
	m_pLights[2].m_nType = DIRECTIONAL_LIGHT;
	m_pLights[2].m_xmf4Ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	m_pLights[2].m_xmf4Diffuse = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	m_pLights[2].m_xmf4Specular = XMFLOAT4(0.4f, 0.4f, 0.4f, 0.0f);
	m_pLights[2].m_xmf3Direction = XMFLOAT3(0.0f, -1.0f, -1.0f);

	m_pLights[3].m_bEnable = false;
	m_pLights[3].m_nType = SPOT_LIGHT;
	m_pLights[3].m_fRange = 600.0f;
	m_pLights[3].m_xmf4Ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	m_pLights[3].m_xmf4Diffuse = XMFLOAT4(0.3f, 0.7f, 0.0f, 1.0f);
	m_pLights[3].m_xmf4Specular = XMFLOAT4(0.3f, 0.3f, 0.3f, 0.0f);
	m_pLights[3].m_xmf3Position = XMFLOAT3(550.0f, 330.0f, 530.0f);
	m_pLights[3].m_xmf3Direction = XMFLOAT3(0.0f, -1.0f, 1.0f);
	m_pLights[3].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.01f, 0.0001f);
	m_pLights[3].m_fFalloff = 8.0f;
	m_pLights[3].m_fPhi = (float)cos(XMConvertToRadians(90.0f));
	m_pLights[3].m_fTheta = (float)cos(XMConvertToRadians(30.0f));

	m_pLights[4].m_bEnable = false;
	m_pLights[4].m_nType = POINT_LIGHT;
	m_pLights[4].m_fRange = 200.0f;
	m_pLights[4].m_xmf4Ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	m_pLights[4].m_xmf4Diffuse = XMFLOAT4(0.8f, 0.3f, 0.3f, 1.0f);
	m_pLights[4].m_xmf4Specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 0.0f);
	m_pLights[4].m_xmf3Position = XMFLOAT3(600.0f, 250.0f, 700.0f);
	m_pLights[4].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.001f, 0.0001f);
}

void CGameScene::BuildObjects(ID3D12Device5* pd3dDevice,ID3D12GraphicsCommandList4*  pd3dCommandList)
{
	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);

	CreateCbvSrvDescriptorHeaps(pd3dDevice, 0, 52 + 1 + 1 + 12 + 3 + 12 + 3 + 89 + 5 + 5 * 4 + 6 * 17 + 2 * 50 + 3 * 2 + 14 + 1 + 12);//Albedomap 52 / player 1 / skybox 1 / box subTexture 3 * 4/ tile subTexture 3 * 1/ woodPallet 3 * 4 / pillar2 3 * 1 / BoundsMap 89 / ����ġ 2 + 3 / �� / ���̷� 6 / �Ѿ� 100 / ���̵� �� 3*2 / 14

	CMaterial::PrepareShaders(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);

	BuildDefaultLightsAndMaterials();

	m_pSkyBox = new CSkyBox(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
	m_pSkyBox->SetScale(50.0f, 50.0f, 50.0f);

	XMFLOAT3 xmf3Scale(8.0f, 2.0f, 8.0f);
	XMFLOAT4 xmf4Color(0.0f, 0.3f, 0.0f, 0.0f);

	m_nShaders = 5;
	m_ppShaders = new CShader * [m_nShaders];

	CMapObjectsShader* pMapShader = new CMapObjectsShader();
	pMapShader->CreateShader(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
	pMapShader->BuildObjects(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, NULL, NULL);
	m_ppShaders[0] = pMapShader;

	CBulletObjectsShader* pBulletObjectShader = new CBulletObjectsShader();
	pBulletObjectShader->CreateShader(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
	pBulletObjectShader->BuildObjects(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, NULL, NULL);
	m_ppShaders[1] = pBulletObjectShader;

	CDoorObjectsShader* pDoorObjectShader = new CDoorObjectsShader();
	pDoorObjectShader->CreateShader(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
	pDoorObjectShader->BuildObjects(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, NULL, NULL);
	m_ppShaders[2] = pDoorObjectShader;

	CSirenObjectsShader* pSirenObjectShader = new CSirenObjectsShader();
	pSirenObjectShader->CreateShader(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
	pSirenObjectShader->BuildObjects(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, NULL, NULL);
	m_ppShaders[3] = pSirenObjectShader;

	CGeneratorObjectsShader* pGeneratorObjectsShader = new CGeneratorObjectsShader();
	pGeneratorObjectsShader->CreateShader(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
	pGeneratorObjectsShader->BuildObjects(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, NULL, NULL);
	m_ppShaders[4] = pGeneratorObjectsShader;

	CBoundsObjectsShader* pBoundsMapShader = new CBoundsObjectsShader();
	pBoundsMapShader->CreateShader(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
	pBoundsMapShader->BuildObjects(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, NULL, NULL);

	m_ppGenerator = new CGenerator * [m_nGenerator];
	for (int i = 0; i < m_nGenerator; ++i)
	{
		//m_ppGenerator[i] = ((CGenerator*)pGeneratorObjectsShader->m_ppObjects[i]);
		//m_ppGenerator[i]->m_idx = i;
	}
	for (int i = 0; i < PLAYERNUM; ++i)
	{
		if (i == (int)(CHARACTER_TYPE::CAP_EMP))
		{
			m_players[i] = new CBoss(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
			if (m_ppShaders[1])
			{
				((CBoss*)m_players[i])->m_pBullet = (CBullet*)pBulletObjectShader->m_ppObjects[0];
			}
		}
		else
		{
			m_players[i] = new CEmployee(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, (CHARACTER_TYPE)(i + 1));
			((CEmployee*)m_players[i])->m_pSwitches[0].position = XMFLOAT3(-23.12724, 1.146619, 1.814123);
			((CEmployee*)m_players[i])->m_pSwitches[0].radius = 0.2f;
			((CEmployee*)m_players[i])->m_pSwitches[1].position = XMFLOAT3(23.08867, 1.083242, 3.155997);
			((CEmployee*)m_players[i])->m_pSwitches[1].radius = 0.2f;
			((CEmployee*)m_players[i])->m_pSwitches[2].position = XMFLOAT3(0.6774719, 1.083242, -23.05909);
			((CEmployee*)m_players[i])->m_pSwitches[2].radius = 0.2f;
		}
	}
	m_pCamera = m_players[m_playerIdx]->GetCamera();
	
	//���� ���� ����
	
	SoundManager::GetInstance().PlayBackGroundSound(3);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void CGameScene::ProcessInput(HWND& hWnd)
{
	if (::GetActiveWindow() != hWnd) return;
	
	uint8 keyInput = 0;
	InputManager::GetInstance().InputStatusUpdate();
	InputManager::GetInstance().MouseInputStatusUpdate();
	

	// ============= ���콺 ��ư ���� ó�� ================
	float cxDelta = 0.0f, cyDelta = 0.0f;
	if (InputManager::GetInstance().GetKeyBuffer(KEY_TYPE::MLBUTTON) > 0 )
	{
		
		POINT ptCursorPos;
		if (::GetCapture() == hWnd)
		{
			::SetCursor(NULL);
			::GetCursorPos(&ptCursorPos);
			cxDelta = (float)(ptCursorPos.x - m_ptOldCursorPos.x) / 3.0f;
			cyDelta = (float)(ptCursorPos.y - m_ptOldCursorPos.y) / 3.0f;
			::SetCursorPos(m_ptOldCursorPos.x, m_ptOldCursorPos.y);
		}
		if(cxDelta != 0) m_players[m_playerIdx]->Rotate(0.f, cxDelta, 0.0f);
	}


	//============  �÷��̾�� ���� Ű�Է� ó�� ============
	keyInput = m_players[m_playerIdx]->ProcessInput(); // �Էµ� Ű�� ������� ��ǲ ó�� ����
	

	// ============ ��Ŷ �۽� ��Ʈ ===================
	// �̵� Ű �Է¿� ��ȭ�� �ְų� Ű �Է� �� ȸ���� �����ϴ� ��쿡��.. �̵� ���� ��Ŷ�� �����Ѵ�.
	if (m_lastKeyInput != keyInput || (keyInput && cxDelta != 0))
	{

		C2S_KEY packet; // Ű �Է� + ���� ������ ������.
		packet.size = sizeof(C2S_KEY);
		packet.type = (uint8)C_GAME_PACKET_TYPE::CKEY;
		packet.key = keyInput;
		packet.x = m_players[m_playerIdx]->GetLook().x;
		packet.z = m_players[m_playerIdx]->GetLook().z;
		clientCore.DoSend(&packet);
	}
	m_lastKeyInput = keyInput;
}

void CGameScene::Update(HWND& hWnd)

{
	m_timer.Tick(0);
	
	{
		std::unique_lock<std::shared_mutex> wl(m_jobQueueLock);
		m_jobQueue->DoTasks();
	}
	
	

	for (int k = 0; k < PLAYERNUM; ++k)
	{
		if (k == m_playerIdx) m_players[k]->Update(m_timer.GetTimeElapsed(), CLIENT_TYPE::OWNER);
		else m_players[k]->Update(m_timer.GetTimeElapsed(), CLIENT_TYPE::OTHER_PLAYER);
	}
	//for (int k = 0; k < m_nGenerator; ++k) m_ppGenerator[k]->Update(_timer.GetTimeElapsed());

	mainGame.m_UIRenderer->UpdateGameSceneUI(this);

	// ��� ������ ����Ʈ ���
	std::wstring str = L"[";
	str.append(std::to_wstring(m_sid));
	str.append(L"] (");
	str.append(std::to_wstring(m_players[m_playerIdx]->GetPosition().x));
	str.append(L" ");
	str.append(std::to_wstring(m_players[m_playerIdx]->GetPosition().z));
	str.append(L")- FPS: ");
	str.append(std::to_wstring(m_curFrame));
	//if(_timer.GetFrameRate()) str.append(std::to_wstring(1000.f / _timer.GetFrameRate()));
	::SetWindowText(hWnd, str.c_str());
}

void CGameScene::ChangeMyPlayerCamera()
{
	m_players[m_playerIdx]->OnChangeCamera(FIRST_PERSON_CAMERA, 0.f);
	m_pCamera = m_players[m_playerIdx]->GetCamera();
	m_pd3dcbLights->Map(0, NULL, (void**)&m_pcbMappedLights);
}

void CGameScene::AnimateObjects()
{ 
	//for (int i = 0; i < m_nGameObjects; i++) if (m_ppGameObjects[i]) m_ppGameObjects[i]->Animate(_timer.GetTimeElapsed());
	for (int i = 0; i < m_nShaders; i++) if (m_ppShaders[i]) m_ppShaders[i]->AnimateObjects(m_timer.GetTimeElapsed());

	for (int i = 0; i < PLAYERNUM; i++)
	{
		if(m_players[i])
			m_players[i]->Animate(m_timer.GetTimeElapsed());
	}
	if (m_pLights)
	{
		m_pLights[1].m_xmf3Position = m_players[m_playerIdx]->GetPosition();
		m_pLights[1].m_xmf3Direction = m_players[m_playerIdx]->GetLookVector();
	}
}

void CGameScene::Render(ID3D12GraphicsCommandList4* pd3dCommandList, CCamera* pCamera, bool bRaster)
{
	if (m_pd3dGraphicsRootSignature) pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);
	if (m_pd3dCbvSrvDescriptorHeap) pd3dCommandList->SetDescriptorHeaps(1, &m_pd3dCbvSrvDescriptorHeap);

	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	pCamera->UpdateShaderVariables(pd3dCommandList);

	UpdateShaderVariables(pd3dCommandList);

	D3D12_GPU_VIRTUAL_ADDRESS d3dcbLightsGpuVirtualAddress = m_pd3dcbLights->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(2, d3dcbLightsGpuVirtualAddress); //Lights

	if (m_pSkyBox) m_pSkyBox->Render(pd3dCommandList, pCamera,bRaster);

	for (int i = 0; i < m_nGameObjects; i++) if (m_ppGameObjects[i]) m_ppGameObjects[i]->Render(pd3dCommandList, pCamera,bRaster);
	for (int i = 0; i < m_nShaders; i++) if (m_ppShaders[i]) m_ppShaders[i]->Render(pd3dCommandList, pCamera,bRaster);

	for (int i = 0; i < m_nHierarchicalGameObjects; i++)
	{
		if (m_ppHierarchicalGameObjects[i])
		{
			m_ppHierarchicalGameObjects[i]->Animate(m_fElapsedTime);
			if (!m_ppHierarchicalGameObjects[i]->m_pSkinnedAnimationController) m_ppHierarchicalGameObjects[i]->UpdateTransform(NULL);
			m_ppHierarchicalGameObjects[i]->Render(pd3dCommandList, pCamera, bRaster);
		}
	}

	for (int i = 0; i < PLAYERNUM; ++i)
	{
		if(!m_players[i]->m_hide) m_players[i]->Render(pd3dCommandList, pCamera, bRaster);
	}
}

CPlayer* CGameScene::GetScenePlayerBySid(const int32 sid)
{
	for (int i = 0; i < PLAYERNUM; ++i)
	{
		if (m_players[i]->m_sid == sid)
		{
			return m_players[i];
		}
	}
	return nullptr;
}

CPlayer* CGameScene::GetScenePlayerByIdx(const int32 idx)
{
	return m_players[idx];
}

CGenerator* CGameScene::GetSceneGenByIdx(const int32 idx)
{
	if (idx == -1) return nullptr;
	return m_ppGenerator[idx];
}

void CGameScene::InitGame(void* packet, int32 sid)
{
	S2C_GAMESTART* gsp = reinterpret_cast<S2C_GAMESTART*>(packet);
	for (int i = 0; i < PLAYERNUM; ++i)
	{
		if (gsp->sids[i] == sid) m_playerIdx = i;
		m_players[i]->SetPlayerSid(gsp->sids[i]);
		// �� �÷��̾� ���� ���� ���̵� �ο�
	}

	std::cout << "PLAYER_IDX: " << m_playerIdx << "\n";
	m_players[0]->SetPosition(XMFLOAT3(0, 0.25, -18));
	if(m_players[1] != nullptr)m_players[1]->SetPosition(XMFLOAT3(10, 0.25, -18));
	if(m_players[2] != nullptr)m_players[2]->SetPosition(XMFLOAT3(15, 0.25, -18));
	if(m_players[3] != nullptr)m_players[3]->SetPosition(XMFLOAT3(20, 0.25, -18));
}

void CGameScene::AddEvent(queueEvent* ev, float after)
{
	std::unique_lock<std::shared_mutex> wl(m_jobQueueLock);
	m_jobQueue->PushTask(ev, after);
}

void CGameScene::Exit()
{
	if (m_bEmpExit) // Ż�� ���� �� , �ؾ��� �� ó��
	{
		std::cout << "Exit Ready\n";
		for (int i = 0; i < m_nShaders; i++)
		{
			CStandardObjectsShader* pShaderObjects = (CStandardObjectsShader*)m_ppShaders[i];
			for (int i = 0; i < pShaderObjects->m_nObjects; i++)
			{
				if (pShaderObjects->m_ppObjects[i])
				{
					if ((pShaderObjects->m_ppObjects[i]->objLayer == Layout::SIREN) || (pShaderObjects->m_ppObjects[i]->objLayer == Layout::DOOR))
						pShaderObjects->m_ppObjects[i]->m_bEmpExit = true;
				}
			}
		}
	}
}

