#include "pch.h"
// ��Ʈ��ũ ���� ���
#include "SocketUtil.h"
#include "ClientSession.h"
// ������ ��ũ ���
#include "GameFramework.h"
#include "SceneManager.h"
// �̺�Ʈ ó������ ���
#include "IocpEvent.h"
#include "ClientPacketEvent.h"
#include "CJobQueue.h"
// ��ü ���� ���
#include "CBullet.h"
#include "CBoss.h"
#include "CEmployee.h"
// ������ ���
#include "OtherScenes.h"
#include "GameScene.h"




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
		DoRecv(); // Connect�ϰ� Do recv ����
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

void CSession::DoDelaySend(C2S_ATTACK packet, float afterTick)
{
	DelayEvent<C2S_ATTACK>* de = new DelayEvent(packet);
	std::unique_lock<std::shared_mutex> wl(_DelayQueueLock);
	_DelayjobQueue->PushTask(static_cast<queueEvent*>(de), afterTick);
}

void CSession::DoDelayTask()
{
	std::unique_lock<std::shared_mutex> wl(_DelayQueueLock);
	_DelayjobQueue->DoTasks();
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

	if (!gs) return; // something error detected;
	if (!ls) return; // something error detected;
	switch ((uint8)packet[1])
	{
	
	// ================ �α��� ���� ó�� ================
#pragma region Title
	case (uint8)S_PACKET_TYPE::LOGIN_OK:
	{
		S2C_LOGIN_OK* lo = (S2C_LOGIN_OK*)packet;
		_cid = lo->cid;
		_sid = lo->sid;

		CScene::m_sid = lo->sid;
		CScene::m_cid = lo->cid;
		mainGame.ChangeScene(CGameFramework::SCENESTATE::LOBBY);
	}
	break;
	case (uint8)S_PACKET_TYPE::LOGIN_FAIL:
	{
		S2C_LOGIN_FAIL* lo = (S2C_LOGIN_FAIL*)packet;
		std::cout << "Login Fail" << std::endl;
	}
	break;
#pragma endregion
	// ================ �κ�� ��Ŷ      ===============
#pragma region  Lobby
	// ============= �� ���� ��Ŷ ============
	case (uint8)S_ROOM_PACKET_TYPE::REP_ENTER_RM:
	{
		S2C_ROOM_ENTER* re = (S2C_ROOM_ENTER*)packet;
		if (re->success)
		{
			mainGame.ChangeScene(CGameFramework::SCENESTATE::ROOM);
		}
		else std::cout << "FAIL TO ENTER ROOM" << std::endl;
	}
	break;
	case (uint8)S_ROOM_PACKET_TYPE::MK_RM_FAIL:
	{
		std::cout << "Fail to Create Room!!(MAX_CAPACITY)" << std::endl;
	}
	break;
	case (uint8)S_ROOM_PACKET_TYPE::MK_RM_OK:
	{
		mainGame.ChangeScene(CGameFramework::SCENESTATE::ROOM);
	}
		break;
	case (uint8)S_ROOM_PACKET_TYPE::UPDATE_LIST:
	{
		
		S2C_ROOM* rp = (S2C_ROOM*)packet;
		std::cout << "Update" << rp->rmNum << ")" << (int32)(rp->member) << "\n";
		ls->GetRoom(rp->rmNum).member = rp->member;
		ls->UpdateRoomText(rp->rmNum, rp->member);
	}
		break;
#pragma endregion
#pragma region Room
	case (uint8)S_PACKET_TYPE::GAME_START:
	{
		S2C_GAMESTART* gsp = reinterpret_cast<S2C_GAMESTART*>(packet);

		// ================= �÷��̾� �ʱ� ��ġ �ʱ�ȭ ==================
		 gs->InitGame(gsp, _sid);
		// ================= �ڽ��� Ŭ���̾�Ʈ IDX Ȯ�� =================
		std::cout << "MYPLAYER IDX : " << gs->m_playerIdx << "\n";

		// ================= ī�޶� ���� ================================
		CPlayer* myPlayer = gs->m_players[gs->m_playerIdx];
		std::wstring str = L"Client";
		str.append(std::to_wstring(gs->m_playerIdx));
		::SetConsoleTitle(str.c_str());
		//mainGame.ChangeScene(CGameFramework::SCENESTATE::INGAME);
		gs->InitScene();
	}
	break;
#pragma endregion
	// ============== �ΰ��� ���� ��Ŷ =============
#pragma region InGame
	case (uint8)S_PACKET_TYPE::SKEY:
	{
		S2C_KEY* movePacket = reinterpret_cast<S2C_KEY*>(packet);
		moveEvent* mev = new moveEvent();

		CPlayer* player = gs ->GetScenePlayerBySid(movePacket->sid);
		if (player == nullptr) break;

		//mev->player = player;
		mev->_dir.x = movePacket->x;
		mev->_dir.y = 0;
		mev->_dir.z = movePacket->z;
		mev->_key = movePacket->key;

		gs->AddEvent(static_cast<queueEvent*>(mev), 0);
	}
	break;

	case (uint8)S_PACKET_TYPE::SROT:
	{
		S2C_ROTATE* rotatePacket = reinterpret_cast<S2C_ROTATE*>(packet);
		CPlayer* player = gs->GetScenePlayerBySid(rotatePacket->sid);
		if (player != nullptr)
		{
			player->Rotate(0, rotatePacket->angle, 0);
		}

	}
	break;
	case (uint8)S_PACKET_TYPE::SPOS: // �̸� ����� ��ǥ���� �����ش�.
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
	case (uint8)SC_PACKET_TYPE::GAMEEVENT:
	{
		SC_EVENTPACKET* ev = (SC_EVENTPACKET*)packet;
		InteractionEvent* gev = new InteractionEvent();
		gev->eventId = ev->eventId;
		gs->AddEvent(static_cast<queueEvent*>(gev), 0.f);

	}
	break;
	// ================= �÷��̾� ����ġ �ִϸ��̼� ���� ��Ŷ ==================
	case (uint8)S_PACKET_TYPE::ANIM:
	{
		S2C_ANIMPACKET* sw = (S2C_ANIMPACKET*)packet;
		uint8 idx = sw->idx;
		CEmployee* myPlayer = (CEmployee*)gs->GetScenePlayerBySid(idx);
		if (myPlayer != nullptr)
		{
			if (sw->track == (uint8)ANIMTRACK::GEN_ANIM) myPlayer->SetBehavior(PLAYER_BEHAVIOR::SWITCH_INTER);
			else myPlayer->SetBehavior(PLAYER_BEHAVIOR::IDLE);
		}
	}
	break;
	case (uint8)S_PACKET_TYPE::FRAME:
	{
		S2C_FRAMEPACKET* fp = (S2C_FRAMEPACKET*)packet;
		FrameEvent* fe = new FrameEvent(fp->wf);
		gs->AddEvent(static_cast<queueEvent*>(fe),0);
	}
	break;
#pragma endregion
	}

}