#include "pch.h"
// 네트워크 관련 헤더
#include "SocketUtil.h"
#include "ClientSession.h"
// 프레임 워크 헤더
#include "GameFramework.h"
#include "SceneManager.h"
#include "UIManager.h"
// 이벤트 처리관련 헤더
#include "IocpEvent.h"
#include "ClientPacketEvent.h"
#include "CJobQueue.h"
// 객체 관련 헤더
#include "CBullet.h"
#include "CBoss.h"
#include "CEmployee.h"
// 씬관련 헤더
#include "OtherScenes.h"
#include "GameScene.h"




CSession::CSession()
{

	_sock = SocketUtil::CreateSocket();
	_DelayjobQueue = new Scheduler();

}

CSession::~CSession()
{
	SocketUtil::Close(_sock);
}

HANDLE CSession::GetHandle()
{
	return reinterpret_cast<HANDLE>(_sock);
}

void CSession::Processing(IocpEvent* iocpEvent, int32 numOfBytes)
{
	//TODO
	switch (iocpEvent->_comp)
	{
	case EventType::Connect:
	{
		ConnectEvent* connectEvent = static_cast<ConnectEvent*>(iocpEvent);
		delete connectEvent;
		_sid = 0;
		DoRecv(); // Connect하고 Do recv 수행
	}
	break;
	case EventType::Recv:
	{
		RecvEvent* rev = static_cast<RecvEvent*>(iocpEvent);
		int remain_data = numOfBytes + _prev_remain;
		char* p = rev->_rbuf;
		while (remain_data > 0)
		{
			uint8 packet_size = p[0];
			if (packet_size <= remain_data)
			{
				ProcessPacket(p);
				p = p + packet_size;
				remain_data = remain_data - packet_size;
			}
			else break;
		}
		_prev_remain = remain_data;
		if (remain_data > 0)
		{
			memcpy(rev->_rbuf, p, remain_data);
		}
		DoRecv();
	}
	break;
	case EventType::Send:
	{
		SendEvent* sev = static_cast<SendEvent*>(iocpEvent);
		if (iocpEvent == nullptr) ASSERT_CRASH("double Del");
		delete iocpEvent;
	}
	break;
	}
}

bool CSession::DoSend(void* packet)
{
	DWORD sendLen(0);
	DWORD flag(0);
	SendEvent* sev = new SendEvent(reinterpret_cast<char*>(packet));
	sev->_sid = _sid;
	if (WSASend(_sock, &sev->_sWsaBuf, 1, &sendLen, flag, static_cast<LPWSAOVERLAPPED>(sev), NULL) == SOCKET_ERROR)
	{
		int32 errcode = WSAGetLastError();
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			std::cout << errcode << std::endl;
			return false;
		}
	}
	return true;
}



bool CSession::DoRecv()
{

	_rev.Init();
	_rev._sid = _sid;
	_rev._cid = _cid;
	DWORD recvBytes(0);
	DWORD flag(0);
	_rev._rWsaBuf.buf = _rev._rbuf + _prev_remain;
	_rev._rWsaBuf.len = BUFSIZE - _prev_remain;
	if (WSARecv(_sock, &_rev._rWsaBuf, 1, &recvBytes, &flag, static_cast<LPWSAOVERLAPPED>(&_rev), NULL) == SOCKET_ERROR)
	{
		int32 errcode = WSAGetLastError();
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			std::cout << errcode << std::endl;
			return false;
		}
	}
	return true;

}

void CSession::ProcessPacket(char* packet)
{

	
	CGameScene* gs =
		static_cast<CGameScene*>(mainGame.m_SceneManager->GetSceneByIdx((int32)CGameFramework::SCENESTATE::INGAME));
	CLobbyScene* ls = static_cast<CLobbyScene*>(mainGame.m_SceneManager->GetSceneByIdx((int32)CGameFramework::SCENESTATE::LOBBY));
	CRoomScene* rs = static_cast<CRoomScene*>(mainGame.m_SceneManager->GetSceneByIdx((int32)CGameFramework::SCENESTATE::ROOM));
	
	if (!gs) return; // something error detected;
	if (!ls) return; // something error detected;
	if (!rs) return;

	switch ((uint8)packet[1])
	{
	
	// ================ 로그인 관련 처리 ================
#pragma region Title
	case (uint8)S_TITLE_PACKET_TYPE::LOGIN_OK:
	{
		S2C_LOGIN_OK* lo = (S2C_LOGIN_OK*)packet;
		_cid = lo->cid;
		_sid = lo->sid;
		CScene::m_sid = lo->sid;
		CScene::m_cid = lo->cid;

		mainGame.m_UIRenderer->m_LoginResult[0].m_hide = false;
		mainGame.m_UIRenderer->m_LoginResult[1].m_hide = true;
		mainGame.m_UIRenderer->m_LoginResult[2].m_hide = true;

		mainGame.ChangeScene(CGameFramework::SCENESTATE::LOBBY);
	}
	break;
	case (uint8)S_TITLE_PACKET_TYPE::LOGIN_FAIL:
	{
		mainGame.m_UIRenderer->m_LoginResult[0].m_hide = true;
		mainGame.m_UIRenderer->m_LoginResult[1].m_hide = false;
		mainGame.m_UIRenderer->m_LoginResult[2].m_hide = true;
	}
	break;
	case (uint8)S_TITLE_PACKET_TYPE::REG_OK:
	{
		mainGame.m_UIRenderer->m_LoginResult[0].m_hide = true;
		mainGame.m_UIRenderer->m_LoginResult[1].m_hide = true;
		mainGame.m_UIRenderer->m_LoginResult[2].m_hide = false;
	}
	break;
	case (uint8)S_TITLE_PACKET_TYPE::REG_FAIL:
	{

	
	}
	break;
#pragma endregion
	// ================ 로비씬 패킷      ===============
#pragma region  Lobby
	// ============= 방 관련 패킷 ============
	case (uint8)S_ROOM_PACKET_TYPE::REP_ENTER_OK:
	{
		S2C_ROOM_ENTER* rep = (S2C_ROOM_ENTER*)packet;
		rs->m_rmnum = rep->rmNum;
		mainGame.ChangeScene(CGameFramework::SCENESTATE::ROOM);
	}
	break;
	case (uint8)S_ROOM_PACKET_TYPE::REP_ENTER_FAIL:
	{

	}
	break;
	case (uint8)S_ROOM_PACKET_TYPE::MK_RM_FAIL:
	{
		
	}
	break;
	case (uint8)S_ROOM_PACKET_TYPE::MK_RM_OK:
	{

		mainGame.ChangeScene(CGameFramework::SCENESTATE::ROOM);
	}
		break;
	case (uint8)S_ROOM_PACKET_TYPE::UPDATE_LIST:
	{
		
		S2C_ROOM_LIST* rp = (S2C_ROOM_LIST*)packet;;
		ls->UpdateRoomStatus(rp->rmNum, rp->member);	
	}
		break;
	
#pragma endregion
#pragma region Room
	case (uint8)S_ROOM_PACKET_TYPE::REP_READY:
	{
		S2C_ROOM_READY* rp = (S2C_ROOM_READY*)packet;
		rs->m_memLock.lock();
		rs->UpdateReady(rp->sid, true);
		rs->m_memLock.unlock();

		std::cout << rp->sid << "Ready\n";
	}
		break;
	case (uint8)S_ROOM_PACKET_TYPE::REP_READY_CANCEL:
	{
		S2C_ROOM_READY* rp = (S2C_ROOM_READY*)packet;
		rs->m_memLock.lock();
		rs->UpdateReady(rp->sid, false);
		rs->m_memLock.unlock();
		std::cout << rp->sid << "Cancel Ready\n";
	}
		break;
	case (uint8)S_ROOM_PACKET_TYPE::ROOM_INFO:
	{
		
		S2C_ROOM_INFO* rp = (S2C_ROOM_INFO*)packet;
		rs->m_memLock.lock();
		for (int i = 0; i < PLAYERNUM; ++i) rs->m_members[i].m_sid = rp->sids[i];
		rs->m_memLock.unlock();
	}
	break;
	case (uint8)S_ROOM_PACKET_TYPE::GAME_START:
	{
		// ================= 플레이어 초기 위치 초기화 ==================
		for (int i = 0; i < PLAYERNUM; ++i)
		{
			rs->m_members[i].m_sid = -1;
			
		}

		 gs->InitGame(packet, _sid);
		// ================= 카메라 셋팅 ================================
		std::wstring str = L"Client";
		str.append(std::to_wstring(gs->m_playerIdx));
		::SetConsoleTitle(str.c_str());
		mainGame.m_UIRenderer->InitGameSceneUI(gs);
		mainGame.ChangeScene(CGameFramework::SCENESTATE::INGAME);
		gs->InitScene();
	}
	break;
#pragma endregion
	// ============== 인게임 관련 패킷 =============
#pragma region InGame
	case (uint8)S_GAME_PACKET_TYPE::SKEY:
	{
		S2C_KEY* movePacket = reinterpret_cast<S2C_KEY*>(packet);
		moveEvent* mev = new moveEvent();
		CPlayer* player = gs ->GetScenePlayerBySid(movePacket->sid);
		if (player == nullptr) break;
		
		mev->player = player;
		mev->_dir.x = movePacket->x;
		mev->_dir.y = 0;
		mev->_dir.z = movePacket->z;
		mev->_key = movePacket->key;

		gs->AddEvent(static_cast<queueEvent*>(mev), 0);
	}
	break;
	case (uint8)S_GAME_PACKET_TYPE::SROT:
	{
		S2C_ROTATE* rotatePacket = reinterpret_cast<S2C_ROTATE*>(packet);
		CPlayer* player = gs->GetScenePlayerBySid(rotatePacket->sid);
		if (player != nullptr)
		{
			player->Rotate(0, rotatePacket->angle, 0);
		}

	}
	break;
	case (uint8)S_GAME_PACKET_TYPE::SPOS: // 미리 계산한 좌표값을 보내준다.
	{
		S2C_POS* posPacket = reinterpret_cast<S2C_POS*>(packet);
		CPlayer* player = gs->GetScenePlayerBySid(posPacket->sid);		
		if (player == nullptr) break;
		
		XMFLOAT3 newPos = XMFLOAT3(posPacket->x, player->GetPosition().y, posPacket->z);
		posEvent* pe = new posEvent();
		pe->player = player;
		pe->_pos = newPos;

		gs->AddEvent(static_cast<queueEvent*>(pe), 0.f);

	}
	break;
	case (uint8)SC_GAME_PACKET_TYPE::GAMEEVENT:
	{
		SC_EVENTPACKET* ev = (SC_EVENTPACKET*)packet;
		InteractionEvent* gev = new InteractionEvent();
		gev->eventId = ev->eventId;
		if (gev->eventId == (uint8)EVENT_TYPE::GAME_END)
		{
			gs->ResetGame();
			mainGame.ChangeScene(CGameFramework::SCENESTATE::RESULT);
		}
		else gs->AddEvent(static_cast<queueEvent*>(gev), 0.f);

	}
	break;
	// ================= 플레이어 스위치 애니메이션 관련 패킷 ==================
	case (uint8)S_GAME_PACKET_TYPE::ANIM:
	{
		S2C_ANIMPACKET* sw = (S2C_ANIMPACKET*)packet;
		uint8 idx = sw->idx;
		CPlayer* myPlayer = nullptr;
	
		myPlayer = gs->GetScenePlayerBySid(idx);
		if (myPlayer != nullptr)
		{
			if (sw->track == (uint8)ANIMTRACK::GEN_ANIM) myPlayer->SetBehavior(PLAYER_BEHAVIOR::SWITCH_INTER);
			else if (sw->track == (uint8)ANIMTRACK::ATTACK_ANIM) static_cast<CBoss*>(myPlayer)->SetAttackAnimOtherClient();
			else myPlayer->SetBehavior(PLAYER_BEHAVIOR::IDLE);
		}
	}
	break;
	case (uint8)S_GAME_PACKET_TYPE::FRAME:
	{
		S2C_FRAMEPACKET* fp = (S2C_FRAMEPACKET*)packet;
		FrameEvent* fe = new FrameEvent(fp->wf);
		gs->AddEvent(static_cast<queueEvent*>(fe),0);
	}
	break;
#pragma endregion
	}

}