#include "stdafx.h"
#include "CSession.h"
#include "SocketUtil.h"
#include "DXSample.h"
#include "CScheduler.h"

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

	
	

	switch ((uint8)packet[1])
	{

		// ================ 로그인 관련 처리 ================
#pragma region Title
	case (uint8)S_TITLE_PACKET_TYPE::LOGIN_OK:
	{
		S2C_LOGIN_OK* lo = (S2C_LOGIN_OK*)packet;
		
	}
	break;
	case (uint8)S_TITLE_PACKET_TYPE::LOGIN_FAIL:
	{
		/*mainGame.m_UIRenderer->m_LoginResult[0].m_hide = true;
		mainGame.m_UIRenderer->m_LoginResult[1].m_hide = false;
		mainGame.m_UIRenderer->m_LoginResult[2].m_hide = true;*/
	}
	break;
	case (uint8)S_TITLE_PACKET_TYPE::REG_OK:
	{
		/*mainGame.m_UIRenderer->m_LoginResult[0].m_hide = true;
		mainGame.m_UIRenderer->m_LoginResult[1].m_hide = true;
		mainGame.m_UIRenderer->m_LoginResult[2].m_hide = false;*/
	}
	break;

#pragma endregion
	// ================ 로비씬 패킷      ===============
#pragma region  Lobby
	// ============= 방 관련 패킷 ============
	case (uint8)S_ROOM_PACKET_TYPE::REP_ENTER_OK:
	{
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

	
	}
	break;
	case (uint8)S_ROOM_PACKET_TYPE::UPDATE_LIST:
	{

		S2C_ROOM_LIST* rp = (S2C_ROOM_LIST*)packet;;
		
	}
	break;

#pragma endregion
#pragma region Room
	case (uint8)S_ROOM_PACKET_TYPE::REP_READY:
	{
		S2C_ROOM_READY* rp = (S2C_ROOM_READY*)packet;

	}
	break;
	case (uint8)S_ROOM_PACKET_TYPE::REP_READY_CANCEL:
	{
		S2C_ROOM_READY* rp = (S2C_ROOM_READY*)packet;
		
	}
	break;
	case (uint8)S_ROOM_PACKET_TYPE::ROOM_INFO:
	{

		S2C_ROOM_INFO* rp = (S2C_ROOM_INFO*)packet;

	}
	break;
	case (uint8)S_ROOM_PACKET_TYPE::GAME_START:
	{
		// ================= 플레이어 초기 위치 초기화 ==================


		//gs->InitGame(packet, _sid);


		//// ================= 카메라 셋팅 ================================
		//mainGame.m_UIRenderer->InitGameSceneUI(gs);

		//mainGame.scLock.lock();
		//mainGame.ChangeScene(CGameFramework::SCENESTATE::INGAME);
		//mainGame.scLock.unlock();
		//gs->InitScene();
		//rs->m_memLock.lock();

		//for (int i = 0; i < PLAYERNUM; ++i)
		//{
		//	rs->m_members[i].m_sid = -1;
		//	rs->m_members->isReady = false;
		//}
		//rs->m_memLock.unlock();
	}
	break;
#pragma endregion
	// ============== 인게임 관련 패킷 =============
#pragma region InGame
	case (uint8)S_GAME_PACKET_TYPE::SKEY:
	{
	/*	S2C_KEY* movePacket = reinterpret_cast<S2C_KEY*>(packet);
		moveEvent* mev = new moveEvent();
		CPlayer* player = gs->GetScenePlayerBySid(movePacket->sid);
		if (player == nullptr) break;

		mev->player = player;
		mev->_dir.x = movePacket->x;
		mev->_dir.y = 0;
		mev->_dir.z = movePacket->z;
		mev->_key = movePacket->key;

		mainGame.dalock.lock();
		if (mainGame.m_activeDelay) gs->AddEvent(static_cast<queueEvent*>(mev), 320);
		else if (!mainGame.m_activeDelay) gs->AddEvent(static_cast<queueEvent*>(mev), 0);
		mainGame.dalock.unlock();*/
	}
	break;
	case (uint8)S_GAME_PACKET_TYPE::SROT:
	{
	/*	S2C_ROTATE* rotatePacket = reinterpret_cast<S2C_ROTATE*>(packet);
		CPlayer* player = gs->GetScenePlayerBySid(rotatePacket->sid);
		if (player != nullptr)
		{
			player->Rotate(0, rotatePacket->angle, 0);
		}*/

	}
	break;
	case (uint8)S_GAME_PACKET_TYPE::SPOS: // 미리 계산한 좌표값을 보내준다.
	{
		/*S2C_POS* posPacket = reinterpret_cast<S2C_POS*>(packet);
		CPlayer* player = gs->GetScenePlayerBySid(posPacket->sid);
		if (player == nullptr) break;

		XMFLOAT3 newPos = XMFLOAT3(posPacket->x, player->GetPosition().y, posPacket->z);
		posEvent* pe = new posEvent();
		pe->player = player;
		pe->_pos = newPos;

		mainGame.dalock.lock();
		if (mainGame.m_activeDelay) gs->AddEvent(static_cast<queueEvent*>(pe), 320);
		else if (!mainGame.m_activeDelay) gs->AddEvent(static_cast<queueEvent*>(pe), 0);
		mainGame.dalock.unlock();*/
	}
	break;
	case (uint8)SC_GAME_PACKET_TYPE::GAMEEVENT:
	{
		/*SC_EVENTPACKET* ev = (SC_EVENTPACKET*)packet;
		InteractionEvent* gev = new InteractionEvent();
		gev->eventId = ev->eventId;
		if (gev->eventId == (uint8)EVENT_TYPE::BOSS_WIN || gev->eventId == (uint8)EVENT_TYPE::EMP_WIN)
		{
			std::cout << "Go to Result\n";

			if (gev->eventId == (uint8)EVENT_TYPE::BOSS_WIN) rrs->m_case = 1;
			if (gev->eventId == (uint8)EVENT_TYPE::EMP_WIN) rrs->m_case = 0;

			rrs->m_timer.Reset();
			mainGame.scLock.lock();
			mainGame.ChangeScene(CGameFramework::SCENESTATE::RESULT);
			mainGame.scLock.unlock();
			mainGame.m_curFrame = 0;
			gs->ResetGame();
		}
		else
		{
			mainGame.dalock.lock();
			if (mainGame.m_activeDelay) gs->AddEvent(static_cast<queueEvent*>(gev), 320);
			else if (!mainGame.m_activeDelay) gs->AddEvent(static_cast<queueEvent*>(gev), 0);
			mainGame.dalock.unlock();
		}*/
	}
	break;
	// ================= 플레이어 스위치 애니메이션 관련 패킷 ==================
	case (uint8)S_GAME_PACKET_TYPE::ANIM:
	{
		
	}
	break;
	case (uint8)S_GAME_PACKET_TYPE::FRAME:
	{
		S2C_FRAMEPACKET* fp = (S2C_FRAMEPACKET*)packet;
	

	}
	break;
#pragma endregion
	}

}

void CSession::SetGame(DXSample* sample)
{
	_sample = sample;
}
