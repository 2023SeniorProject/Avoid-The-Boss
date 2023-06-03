#include "pch.h"
#include "ClientSession.h"
#include "SocketUtil.h"
#include "GameFramework.h"
#include "IocpEvent.h"
#include "ClientPacketEvent.h"
#include "CBullet.h"
#include <string>

CSession::CSession()
{

	_sock = SocketUtil::CreateSocket();

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

	case (uint8)S_PACKET_TYPE::SKEY:
	{
		S2C_KEY* movePacket = reinterpret_cast<S2C_KEY*>(packet);
		moveEvent* mev = new moveEvent();

		CPlayer* player = mainGame.m_pScene->GetScenePlayer(movePacket->sid);
		if (player == nullptr) break;

		mev->player = player;
		mev->_dir.x = movePacket->x;
		mev->_dir.y = 0;
		mev->_dir.z = movePacket->z;
		mev->_key = movePacket->key;
		mainGame.m_pScene->AddEvent(static_cast<queueEvent*>(mev), 0);
	}
	break;

	case (uint8)S_PACKET_TYPE::SROT:
	{
		S2C_ROTATE* rotatePacket = reinterpret_cast<S2C_ROTATE*>(packet);
		CPlayer* player = mainGame.m_pScene->GetScenePlayer(rotatePacket->sid);
		if (player != nullptr)
		{
			//player->Rotate(0, rotatePacket->angle, 0);
		}

	}
	break;
	case (uint8)S_PACKET_TYPE::SPOS: // 미리 계산한 좌표값을 보내준다.
	{
		S2C_POS* posPacket = reinterpret_cast<S2C_POS*>(packet);
		CPlayer* player = mainGame.m_pScene->GetScenePlayer(posPacket->sid);
		mainGame.m_pScene->_curFrameIdx.store(posPacket->fidx);		
		if (player == nullptr) break;
		
		XMFLOAT3 newPos = XMFLOAT3(posPacket->x, player->GetPosition().y, posPacket->z);
		posEvent* pe = new posEvent();
		pe->player = player;
		pe->_pos = newPos;
	
		mainGame.m_pScene->AddEvent(static_cast<queueEvent*>(pe), 0.f);
		
	}
	break;
	case (uint8)S_PACKET_TYPE::SCHAT:
	{
		break;
	}
	case (uint8)S_PACKET_TYPE::GAME_START:
	{
		S2C_GAMESTART* gsp = reinterpret_cast<S2C_GAMESTART*>(packet);
		for (int i = 0; i < PLAYERNUM; ++i)
		{
			if (gsp->sids[i] == _sid) mainGame.m_pScene->_playerIdx = i;
			mainGame.m_pScene->_players[i]->SetPlayerSid(gsp->sids[i]);
			// 각 플레이어 별로 세션 아이디 부여
		}
		// ================= 플레이어 초기 위치 초기화 ==================
		mainGame.m_pScene->_players[0]->MakePosition(XMFLOAT3(0, 0.25, -18));
		if (mainGame.m_pScene->_players[1] != nullptr) mainGame.m_pScene->_players[1]->MakePosition(XMFLOAT3(10, 0.25, -18));
		if(mainGame.m_pScene->_players[2] != nullptr) mainGame.m_pScene->_players[2]->MakePosition(XMFLOAT3(15, 0.25, -18));
		if (mainGame.m_pScene->_players[3] != nullptr) mainGame.m_pScene->_players[3]->MakePosition(XMFLOAT3(20, 0.25, -18));
		// ================= 자신의 클라이언트 IDX 확인 =================
		std::cout << "MYPLAYER IDX : " << mainGame.m_pScene->_playerIdx << "\n";
		
		// ================= 카메라 셋팅 ================================
		CPlayer* myPlayer = mainGame.m_pScene->_players[mainGame.m_pScene->_playerIdx];
		std::wstring str = L"Client";
		str.append(std::to_wstring(mainGame.m_pScene->_playerIdx));
		::SetConsoleTitle(str.c_str());
		mainGame.m_pScene->m_pCamera = myPlayer->GetCamera();
		mainGame.m_pScene->m_cid = _cid;
		mainGame.m_pScene->m_sid = _sid;
		mainGame._curScene.store(SceneInfo::GAMEROOM);
		mainGame.m_pScene->InitScene();
	}
	break;
	// ================ 로그인 관련 처리 ================
	case (uint8)S_PACKET_TYPE::LOGIN_OK:
	{
		S2C_LOGIN_OK* lo = (S2C_LOGIN_OK*)packet;
		_cid = lo->cid;
		_sid = lo->sid;
	}
	break;
	case (uint8)S_PACKET_TYPE::LOGIN_FAIL:
	{
		S2C_LOGIN_FAIL* lo = (S2C_LOGIN_FAIL*)packet;
		std::cout << "Login Fail" << std::endl;
		::SendMessage(mainGame.m_hWnd, WM_QUIT, 0, 0);
	}
	break;
	// ============= 방 관련 패킷 ============
	case (uint8)S_ROOM_PACKET_TYPE::REP_ENTER_RM:
	{
		S2C_ROOM_ENTER* re = (S2C_ROOM_ENTER*)packet;
		if (re->success)
		{
			//::system("cls");
		}
		else std::cout << "FAIL TO ENTER ROOM" << std::endl;
	}
	break;
	case (uint8)S_ROOM_PACKET_TYPE::MK_RM_FAIL:
	{
		std::cout << "Fail to Create Room!!(MAX_CAPACITY)" << std::endl;
	}
	break;
	case (uint8)SC_PACKET_TYPE::GAMEEVENT:
	{
		SC_EVENTPACKET* ev = (SC_EVENTPACKET*)packet;
		InteractionEvent* gev = new InteractionEvent();
		gev->eventId = ev->eventId;
		mainGame.m_pScene->AddEvent(static_cast<queueEvent*>(gev), 0.f);

	}
	break;
	// ================= 플레이어 스위치 애니메이션 관련 패킷 ==================
	case (uint8)S_PACKET_TYPE::SWITCH_ANIM:
	{
		S2C_SWITCH_ANIM* sw = (S2C_SWITCH_ANIM*)packet;
		uint8 idx = sw->idx;
		CEmployee* myPlayer = (CEmployee*)mainGame.m_pScene->_players[idx];
		if (myPlayer != nullptr)
		{
			myPlayer->SetInteractionOn(true);
			myPlayer->SetInteractionAnimTrackOtherClient();
		}
	}
	break;
	case (uint8)S_PACKET_TYPE::SWITCH_ANIM_CANCEL:
	{
		S2C_SWITCH_ANIM* sw = (S2C_SWITCH_ANIM*)packet;
		uint8 idx = sw->idx;
		CPlayer* myPlayer = mainGame.m_pScene->_players[idx];
		if (myPlayer != nullptr)
		{
			myPlayer->SetInteractionOn(false);
		}
	}
	break;
	}
}