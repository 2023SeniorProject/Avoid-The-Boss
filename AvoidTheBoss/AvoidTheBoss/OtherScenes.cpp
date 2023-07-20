#include "pch.h"

#include "Player.h"
#include "GameFramework.h"

#include "InputManager.h"
#include "SoundManager.h"
#include "SceneManager.h"
#include "UIManager.h"
#include "OtherScenes.h"
#include "clientIocpCore.h"
#include "SoundManager.h"
#include "CSound.h"


bool IntersectRectByPoint(const D2D1_RECT_F& rect, const POINT& mp)
{
	return ((rect.left <= mp.x && mp.x <= rect.right) && (rect.top <= mp.y && mp.y <= rect.bottom));
}

#pragma region Lobby

void CLobbyScene::BuildObjects(ID3D12Device5* pd3dDevice, ID3D12GraphicsCommandList4* pd3dCommandList)
{
	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);
	CreateCbvSrvDescriptorHeaps(pd3dDevice, 0, 3);
	CMaterial::PrepareShaders(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
	BuildDefaultLightsAndMaterials();
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	
	m_player = new CVirtualPlayer(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);	
	m_pCamera = m_player->GetCamera();
	
}

void CLobbyScene::ProcessInput(HWND& hWnd)
{
}

void CLobbyScene::Update(HWND hWnd)
{
}

void CLobbyScene::Render(ID3D12GraphicsCommandList4* pd3dCommandList, CCamera* pCamera, bool Raster)
{
	if (m_pd3dGraphicsRootSignature) pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);
	if (m_pd3dCbvSrvDescriptorHeap) pd3dCommandList->SetDescriptorHeaps(1, &m_pd3dCbvSrvDescriptorHeap);

	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	pCamera->UpdateShaderVariables(pd3dCommandList);

	UpdateShaderVariables(pd3dCommandList);

	D3D12_GPU_VIRTUAL_ADDRESS d3dcbLightsGpuVirtualAddress = m_pd3dcbLights->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(2, d3dcbLightsGpuVirtualAddress); //Lights

	if (m_pSkyBox) m_pSkyBox->Render(pd3dCommandList, pCamera, Raster);

	
	for (int i = 0; i < m_nShaders; i++) if (m_ppShaders[i]) m_ppShaders[i]->Render(pd3dCommandList, pCamera, Raster);

	for (int i = 0; i < m_nHierarchicalGameObjects; i++)
	{
		if (m_ppHierarchicalGameObjects[i])
		{
			m_ppHierarchicalGameObjects[i]->Animate(m_fElapsedTime);
			if (!m_ppHierarchicalGameObjects[i]->m_pSkinnedAnimationController) m_ppHierarchicalGameObjects[i]->UpdateTransform(NULL);
			//if (!m_ppHierarchicalGameObjects[i]->m_pSkinnedAnimationController1) m_ppHierarchicalGameObjects[i]->UpdateTransform(NULL);
			//if (!m_ppHierarchicalGameObjects[i]->m_pSkinnedAnimationController2) m_ppHierarchicalGameObjects[i]->UpdateTransform(NULL);

			m_ppHierarchicalGameObjects[i]->Render(pd3dCommandList, pCamera, Raster);
		}
	}
	
	if (m_player) m_player->Render(pd3dCommandList, pCamera, Raster);

}
void CLobbyScene::BuildDefaultLightsAndMaterials()
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
void CLobbyScene::MouseAction(const POINT& mp)
{
	SoundManager::GetInstance().SoundStop(21);
	SoundManager::GetInstance().PlayObjectSound(21, 21);
	// 체크리스트 충돌체크 처리
	for (int i = 0; i < mainGame.m_UIRenderer->m_nRoomListPerPage; ++i)
	{
		if (IntersectRectByPoint(mainGame.m_UIRenderer->m_RoomListLayout[i], mp))
		{
			if (m_rooms[m_curPage * 5 + i].status != ROOM_STATUS::EMPTY) m_selected_rm = m_curPage * 5 + i;
			mainGame.m_UIRenderer->m_selectedLayout = i;
			std::cout << "Selected RM:" << i << "\n";
		}
	}

	if (IntersectRectByPoint(mainGame.m_UIRenderer->m_LobbyButtons[0].d2dLayoutRect, mp) )
	{
		//enter
		if (m_selected_rm != -1)
		{
			C2S_ROOM_ENTER packet;
			packet.size = sizeof(C2S_ROOM_ENTER);
			packet.type = (uint8)C_ROOM_PACKET_TYPE::ACQ_ENTER_RM;
			packet.rmNum = m_selected_rm;
			m_selected_rm = -1;
			clientCore.DoSend(&packet);
		}
		else std::cout << "There is No Any Room Available\n";
	}
	else if (IntersectRectByPoint(mainGame.m_UIRenderer->m_LobbyButtons[1].d2dLayoutRect, mp))
	{
		//create
		C2S_ROOM_EVENT packet;
		packet.size = sizeof(C2S_ROOM_EVENT);
		packet.type = (uint8)C_ROOM_PACKET_TYPE::ACQ_MK_RM;
		clientCore.DoSend(&packet);
	}
	else if (IntersectRectByPoint(mainGame.m_UIRenderer->m_LobbyButtons[2].d2dLayoutRect, mp))
	{
		//quit
		clientCore.Disconnect(0);
		mainGame.ChangeScene(CGameFramework::SCENESTATE::TITLE);
	}
}
void CLobbyScene::ChangePage(int32 newPage)
{
	m_lastPage = m_curPage;
	m_curPage = newPage;
}
void CLobbyScene::UpdateRoomText(int32 index = -1, int32 member = -1)
{
	
	if (index >= 0 || member >= 0)
	{
		m_rooms[m_curPage * 5 + index].member = member;
		if (PLAYERNUM == member) m_rooms[m_curPage * 5 + index].status = ROOM_STATUS::FULL;
		else if (0 == member)	 m_rooms[m_curPage * 5 + index].status = ROOM_STATUS::EMPTY;
		else m_rooms[m_curPage * 5 + index].status = ROOM_STATUS::NOT_FULL;
	}
	mainGame.m_UIRenderer->UpdateRoomText();
}
CLobbyScene::Room & CLobbyScene::GetRoom(int32 idx)
{
	return m_rooms[idx];
}
#pragma endregion


#pragma region  Title
void CTitleScene::BuildObjects(ID3D12Device5* pd3dDevice, ID3D12GraphicsCommandList4* pd3dCommandList)
{
	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);

	CreateCbvSrvDescriptorHeaps(pd3dDevice, 0, 3);

	CMaterial::PrepareShaders(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);

	BuildDefaultLightsAndMaterials();

	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	m_player = new CVirtualPlayer(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
	m_pCamera = m_player->GetCamera();

	SoundManager::GetInstance().PlayBackGroundSound(0);
}
void CTitleScene::MouseAction(const POINT& mp)
{
	SoundManager::GetInstance().SoundStop(20);
	SoundManager::GetInstance().PlayObjectSound(20, 20);

	if (IntersectRectByPoint(mainGame.m_UIRenderer->m_IDPWTextBlocks[0].m_d2dLayoutRect, mp))
	{
		std::cout << "Focus Change 0\n";
		focus = 0;
	}
	else if (IntersectRectByPoint(mainGame.m_UIRenderer->m_IDPWTextBlocks[1].m_d2dLayoutRect, mp))
	{
		std::cout << "Focus Change 1\n";
		focus = 1;
	}

	if (IntersectRectByPoint(mainGame.m_UIRenderer->GetButtonRect(0,0),mp))
	{
		if (mainGame.m_UIRenderer->m_IDPWTextBlocks[0].m_pstrText.length() <= 3
			|| mainGame.m_UIRenderer->m_IDPWTextBlocks[1].m_pstrText.length() <= 3) return;

		clientCore.InitConnect("127.0.0.1");
		C2S_LOGIN loginPacket;
		lstrcpyn(loginPacket.name, mainGame.m_UIRenderer->m_IDPWTextBlocks[0].m_pstrText.c_str(), 10);
		lstrcpyn(loginPacket.pw, mainGame.m_UIRenderer->m_IDPWTextBlocks[1].m_pstrText.c_str(), 10);
		clientCore.DoConnect(&loginPacket);
		mainGame.ChangeScene(CGameFramework::SCENESTATE::LOBBY);
	}

	else if (IntersectRectByPoint(mainGame.m_UIRenderer->GetButtonRect(0, 1), mp))
	{
		mainGame.OnDestroy();
	}
}
void CTitleScene::BuildDefaultLightsAndMaterials()
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
void CTitleScene::ProcessInput(HWND& hWnd)
{
	InputManager::GetInstance().InputStatusUpdate();

	// TAB 처리
	if ((int8)KEY_STATUS::KEY_UP == InputManager::GetInstance().GetKeyBuffer(VK_TAB))
	{
		if (0 == focus) focus = 1;
		else if (1 == focus) focus = 0;
		std::cout << focus << "\n";
	}
	// CAP 처리
	if ((int8)KEY_STATUS::KEY_UP == InputManager::GetInstance().GetKeyBuffer(VK_CAPITAL))
	{
		if (cap) cap = false;
		else cap = true;
	}
	//알파벳 입력 받기
	for (int i = 65; i < 90; ++i)
	{
		if ((int8)KEY_STATUS::KEY_UP == InputManager::GetInstance().GetKeyBuffer(i))
		{
			wchar_t str[2];
			if (cap) str[0] = i;
			else str[0] = i + 32;
			str[1] = '\0';
			if (focus == 0 && mainGame.m_UIRenderer->m_IDPWTextBlocks[0].m_pstrText.length() <= 10)
				mainGame.m_UIRenderer->m_IDPWTextBlocks[0].m_pstrText.append(str);
			else if( focus == 1 && mainGame.m_UIRenderer->m_IDPWTextBlocks[1].m_pstrText.length() <= 10)
				mainGame.m_UIRenderer->m_IDPWTextBlocks[1].m_pstrText.append(str);
		}
	}
	
	// 텍스트 지우기
	if ((int8)KEY_STATUS::KEY_UP == InputManager::GetInstance().GetKeyBuffer(VK_BACK))
	{
		if (1 == focus)
		{
			if (mainGame.m_UIRenderer->m_IDPWTextBlocks[1].m_pstrText.length() > 3)
				mainGame.m_UIRenderer->m_IDPWTextBlocks[1].m_pstrText.
				erase(mainGame.m_UIRenderer->m_IDPWTextBlocks[1].m_pstrText.length() - 1,
					1);
		}
		else
			if (mainGame.m_UIRenderer->m_IDPWTextBlocks[0].m_pstrText.length() > 3)
				mainGame.m_UIRenderer->m_IDPWTextBlocks[0].m_pstrText.
				erase(mainGame.m_UIRenderer->m_IDPWTextBlocks[0].m_pstrText.length() - 1,
					1);
	}
}
void CTitleScene::Update(HWND hWnd)
{
}
void CTitleScene::Render(ID3D12GraphicsCommandList4* pd3dCommandList, CCamera* pCamera,bool Raster)
{

	if (m_pd3dGraphicsRootSignature) pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);
	if (m_pd3dCbvSrvDescriptorHeap) pd3dCommandList->SetDescriptorHeaps(1, &m_pd3dCbvSrvDescriptorHeap);

	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	pCamera->UpdateShaderVariables(pd3dCommandList);

	UpdateShaderVariables(pd3dCommandList);

	D3D12_GPU_VIRTUAL_ADDRESS d3dcbLightsGpuVirtualAddress = m_pd3dcbLights->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(2, d3dcbLightsGpuVirtualAddress); //Lights

	if (m_pSkyBox) m_pSkyBox->Render(pd3dCommandList, pCamera, Raster);


	for (int i = 0; i < m_nShaders; i++) if (m_ppShaders[i]) m_ppShaders[i]->Render(pd3dCommandList, pCamera, Raster);

	for (int i = 0; i < m_nHierarchicalGameObjects; i++)
	{
		if (m_ppHierarchicalGameObjects[i])
		{
			m_ppHierarchicalGameObjects[i]->Animate(m_fElapsedTime);
			if (!m_ppHierarchicalGameObjects[i]->m_pSkinnedAnimationController) m_ppHierarchicalGameObjects[i]->UpdateTransform(NULL);
			//if (!m_ppHierarchicalGameObjects[i]->m_pSkinnedAnimationController1) m_ppHierarchicalGameObjects[i]->UpdateTransform(NULL);
			//if (!m_ppHierarchicalGameObjects[i]->m_pSkinnedAnimationController2) m_ppHierarchicalGameObjects[i]->UpdateTransform(NULL);

			m_ppHierarchicalGameObjects[i]->Render(pd3dCommandList, pCamera, Raster);
		}
	}

	if (m_player) m_player->Render(pd3dCommandList, pCamera, Raster);
}
#pragma endregion

#pragma region Room

void CRoomScene::BuildObjects(ID3D12Device5* pd3dDevice, ID3D12GraphicsCommandList4* pd3dCommandList)
{
}

void CRoomScene::ProcessInput(HWND& hWnd)
{
}

void CRoomScene::Update(HWND hWnd)
{
}

void CRoomScene::Render(ID3D12GraphicsCommandList4* pd3dCommandList, CCamera* pCamera, bool Raster)
{
}

void CRoomScene::MouseAction(const POINT& mp)
{
	SoundManager::GetInstance().SoundStop(21);
	SoundManager::GetInstance().PlayObjectSound(21, 21);

	m_memLock.lock();
	if (IntersectRectByPoint(mainGame.m_UIRenderer->m_RoomButtons[0].d2dLayoutRect, mp)) // Ready
	{
		
		for (int i = 0; i < PLAYERNUM; ++i)
		{
			if (m_members[i].m_sid == CScene::m_sid)
			{
				if (!m_members[i].isReady)
				{
					m_members[i].isReady = true;
					C2S_ROOM_EVENT packet;
					packet.size = sizeof(C2S_ROOM_EVENT);
					packet.type = (uint8)C_ROOM_PACKET_TYPE::ACQ_READY;
					clientCore.DoSend(&packet);
				}
				else
				{
					m_members[i].isReady = false;
					C2S_ROOM_EVENT packet;
					packet.size = sizeof(C2S_ROOM_EVENT);
					packet.type = (uint8)C_ROOM_PACKET_TYPE::ACQ_READY_CANCEL;
					clientCore.DoSend(&packet);
				}
				break;
			}
		}
		
	}
	else if (IntersectRectByPoint(mainGame.m_UIRenderer->m_RoomButtons[1].d2dLayoutRect, mp))
	{
		
		C2S_ROOM_EVENT acpacket;
		acpacket.size = sizeof(C2S_ROOM_EVENT);
		acpacket.type = (uint8)C_ROOM_PACKET_TYPE::ACQ_EXIT_ROOM;
		clientCore.DoSend(&acpacket);
		
		mainGame.ChangeScene(CGameFramework::SCENESTATE::LOBBY);
	}
	m_memLock.unlock();
}

void CRoomScene::BuildDefaultLightsAndMaterials()
{
}
#pragma endregion