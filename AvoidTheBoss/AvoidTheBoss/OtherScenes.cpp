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
}

void CLobbyScene::ProcessInput(HWND& hWnd)
{
}

void CLobbyScene::Update(HWND hWnd)
{
}

void CLobbyScene::Render(ID3D12GraphicsCommandList4* pd3dCommandList, CCamera* pCamera, bool Raster)
{
}
void CLobbyScene::BuildDefaultLightsAndMaterials()
{

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
			if (m_rooms[m_curPage * 5 + i].status != ROOM_STATUS::EMPTY)
			{
				m_selected_rm = m_curPage * 5 + i;
				std::cout << "Selected RM:" << m_selected_rm << "\n";
			}
			mainGame.m_UIRenderer->m_selectedLayout = i;
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
		if(member != -1) m_rooms[m_curPage * 5 + index].member = member;
		if (PLAYERNUM == m_rooms[m_curPage * 5 + index].member) m_rooms[m_curPage * 5 + index].status = ROOM_STATUS::FULL;
		else if (0 ==	m_rooms[m_curPage * 5 + index].member)	 m_rooms[m_curPage * 5 + index].status = ROOM_STATUS::EMPTY;
		else m_rooms[m_curPage * 5 + index].status = ROOM_STATUS::NOT_FULL;
	}
	mainGame.m_UIRenderer->UpdateRoomText();
}

#pragma endregion


#pragma region  Title
void CTitleScene::BuildObjects(ID3D12Device5* pd3dDevice, ID3D12GraphicsCommandList4* pd3dCommandList)
{
	m_timer.Reset();
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
		if (mainGame.m_UIRenderer->m_IDPWTextBlocks[0].m_pstrText.length() <= 0
			|| mainGame.m_UIRenderer->m_IDPWTextBlocks[1].m_pstrText.length() <= 0) return;

		
		C2S_LOGIN loginPacket;
		loginPacket.size = sizeof(C2S_LOGIN);
		loginPacket.type = (uint8)C_TITLE_PACKET_TYPE::ACQ_LOGIN;
		lstrcpyn(loginPacket.name, mainGame.m_UIRenderer->m_IDPWTextBlocks[0].m_pstrText.c_str(), 10);
		lstrcpyn(loginPacket.pw, mainGame.m_UIRenderer->m_IDPWTextBlocks[1].m_pstrText.c_str(), 10);
		clientCore.DoSend(&loginPacket);

	}

	else if (IntersectRectByPoint(mainGame.m_UIRenderer->GetButtonRect(0, 1), mp))
	{
		mainGame.OnDestroy();
	}
}
void CTitleScene::BuildDefaultLightsAndMaterials()
{
	
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
	for(int i = 0; i < 10; ++i)
		if ((int8)KEY_STATUS::KEY_UP == InputManager::GetInstance().GetKeyBuffer((int32)KEY_TYPE::NUM0 + i))
		{
			wchar_t str[2];
			_itow_s(i, str, 10);
			if (focus == 0 && mainGame.m_UIRenderer->m_IDPWTextBlocks[0].m_pstrText.length() <= 10)
				mainGame.m_UIRenderer->m_IDPWTextBlocks[0].m_pstrText.append(str);
			else if (focus == 1 && mainGame.m_UIRenderer->m_IDPWTextBlocks[1].m_pstrText.length() <= 10)
				mainGame.m_UIRenderer->m_IDPWTextBlocks[1].m_pstrText.append(str);
		}
	
	// 텍스트 지우기
	if ((int8)KEY_STATUS::KEY_UP == InputManager::GetInstance().GetKeyBuffer(VK_BACK))
	{
		if (1 == focus)
		{
			if (mainGame.m_UIRenderer->m_IDPWTextBlocks[1].m_pstrText.length() > 0)
				mainGame.m_UIRenderer->m_IDPWTextBlocks[1].m_pstrText.
				erase(mainGame.m_UIRenderer->m_IDPWTextBlocks[1].m_pstrText.length() - 1,
					1);
		}
		else
			if (mainGame.m_UIRenderer->m_IDPWTextBlocks[0].m_pstrText.length() > 0)
				mainGame.m_UIRenderer->m_IDPWTextBlocks[0].m_pstrText.
				erase(mainGame.m_UIRenderer->m_IDPWTextBlocks[0].m_pstrText.length() - 1,
					1);
	}
}
void CTitleScene::Update(HWND hWnd)
{
	m_timer.Tick(60);
	for (int i = 0; i < 3; ++i)
	{
		if (!mainGame.m_UIRenderer->m_LoginResult[i].m_hide) mainGame.m_UIRenderer->m_LoginResult[i].animTime -= m_timer.GetTimeElapsed();
		if (mainGame.m_UIRenderer->m_LoginResult[i].animTime <= 0)
		{
			mainGame.m_UIRenderer->m_LoginResult[i].animTime = 1.0f;
			mainGame.m_UIRenderer->m_LoginResult[i].m_hide = true;
		}
	}
}
void CTitleScene::Render(ID3D12GraphicsCommandList4* pd3dCommandList, CCamera* pCamera,bool Raster)
{

	
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