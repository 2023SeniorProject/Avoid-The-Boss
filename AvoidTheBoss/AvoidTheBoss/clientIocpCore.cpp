#include "pch.h"
#include "clientIocpCore.h"

ClientMananger clientCore;

// ================= new Client Session ==================
// =======================================================
// =======================================================
#include "SocketUtil.h"
#include "IocpEvent.h"

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
		WLock;
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
	case S_PACKET_TYPE::SMOVE:
	{
		S2C_MOVE* mp = reinterpret_cast<S2C_MOVE*>(packet);
		//std::lock_guard<std::mutex> lg(clientCore._client->_otherLock);
		//if(_sid != mp->sid && _mainGame.m_pScene->_curSceneStatus == SceneInfo::GAMEROOM) _mainGame.m_pScene->_other->Move(mp->key, (UNIT * 1.2f));
	}
	break;

	case S_PACKET_TYPE::SROTATE:
	{
		S2C_ROTATE* mp = reinterpret_cast<S2C_ROTATE*>(packet);
		
		//std::lock_guard<std::mutex> lg(clientCore._client->_otherLock);
		//if (_sid != mp->sid && _mainGame.m_pScene->_curSceneStatus == SceneInfo::GAMEROOM)
		//{
		//	_mainGame.m_pScene->_other->Rotate(0, mp->angle, 0);
		//}
	}
	break;
	case S_PACKET_TYPE::POSITION:
	{
		S2C_POSITION* mp = reinterpret_cast<S2C_POSITION*>(packet);
		//if (_sid == mp->sid && _mainGame.m_pScene->_curSceneStatus == SceneInfo::GAMEROOM)
		//{
		//	clientCore._client->_playerLock.lock();
		//	_mainGame.m_pScene->_player->MakePosition(mp->position);
		//	clientCore._client->_playerLock.unlock();
		//}
		
	}
	break;
	case S_PACKET_TYPE::SCHAT:
	{
		//_CHAT* cp = reinterpret_cast<_CHAT*>(packet);
	}
	break;
	case S_PACKET_TYPE::GAME_START:
	{
		S2C_GAMESTART* gsp = reinterpret_cast<S2C_GAMESTART*>(packet);
		_mainGame._curScene.store(SceneInfo::GAMEROOM);
		for (int i = 0; i < 4; ++i) _mainGame.m_pScene->_playersSid[i] = gsp->sids[i];
	}
	break;
	case S_PACKET_TYPE::LOGIN_OK:
	{
		S2C_LOGIN_OK* lo = (S2C_LOGIN_OK*)packet;
		_cid = lo->cid;
		_sid = lo->sid;
		_loginOk = 1;
	}
	break;
	case S_PACKET_TYPE::LOGIN_FAIL:
	{
		S2C_LOGIN_FAIL* lo = (S2C_LOGIN_FAIL*)packet;
		std::cout << "Login Fail" << std::endl;
		SocketUtil::Close(_sock);
		_loginOk = 0;
	}
	break;
	// ===== 방 관련 패킷 ============
	case S_ROOM_PACKET_TYPE::REP_ENTER_RM:
	{
		S2C_ROOM_ENTER* re = (S2C_ROOM_ENTER*)packet;
		if (re->success)
		{
			::system("cls");
		}
		else std::cout << "FAIL TO ENTER ROOM" << std::endl;
	}
	break;
	case S_ROOM_PACKET_TYPE::MK_RM_FAIL:
	{
		std::cout << "Fail to Create Room!!(MAX_CAPACITY)" << std::endl;
	}
	break;
	}
	DoRecv();
}


//=============== new Iocp Core =======================
//=====================================================
//=====================================================


ClientMananger::ClientMananger()
{
	_client = new CSession();
	::ZeroMemory(&_serveraddr, sizeof(sockaddr_in));
}

ClientMananger::~ClientMananger()
{
	if (_client != nullptr) delete _client;
}

void ClientMananger::InitConnect(const char* address)
{
	_client->_sock = SocketUtil::CreateSocket();
	_client->_sid = 0;
	ASSERT_CRASH(_client->_sock != INVALID_SOCKET);
	_serveraddr.sin_family = AF_INET;
	_serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	_serveraddr.sin_port = htons(0);
	ASSERT_CRASH(::bind(_client->_sock, reinterpret_cast<sockaddr*>(&_serveraddr), sizeof(_serveraddr)) != SOCKET_ERROR);
	inet_pton(AF_INET, address, &_serveraddr.sin_addr);
	_serveraddr.sin_port = htons(PORTNUM);
	ASSERT_CRASH(Register(static_cast<IocpObject*>(_client)));
}

void ClientMananger::DoConnect(void* loginInfo)
{
	DWORD sendBytes(0);
	DWORD sendLength = BUFSIZE / 2;
	ConnectEvent* _connectEvent = new ConnectEvent();
	memcpy(_connectEvent->_buf, loginInfo, BUFSIZE / 2);
	bool retVal = SocketUtil::ConnectEx(_client->_sock, reinterpret_cast<sockaddr*>(&_serveraddr), sizeof(_serveraddr), _connectEvent->_buf, (BUFSIZE / 2) - 1, NULL,
		static_cast<LPWSAOVERLAPPED>(_connectEvent));

	if (!retVal)
	{
		const int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			std::cout << errorCode << std::endl;
			std::cout << "Connect Error" << std::endl;
			delete _client;
			SocketUtil::Clear();
		}
	}
}

void ClientMananger::Disconnect(int32 sid = 0)
{
	std::cout << "Disconnect Client" << std::endl;
	DestroyGame();
	delete _client;
	_client = nullptr;
}