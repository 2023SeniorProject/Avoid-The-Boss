#include "pch.h"
#include "SocketUtil.h"
#include "ThreadManager.h"
#include "iocpCore.h"
#include "ConnectManager.h"
#include "Session.h"

GameSession gameClient;

SOCKET clientSock;
int32 cid = 0;
void ProcessPacket(char* packet)
{
	switch (packet[1])
	{
		case (int8)S_PACKET_TYPE::LOGIN_OK:
		{
			
			S2C_LOGIN_OK* lp = (S2C_LOGIN_OK*)packet;
			cid = lp->cid;
		}
		break;
		case (int8)S_PACKET_TYPE::LOGIN_FAIL:
		{
			cout << "Login fail" << endl;
			closesocket(clientSock);
		}
		break;
		case (int8)S_PACKET_TYPE::CHAT:
		{
			_CHAT* chat = reinterpret_cast<_CHAT*>(packet);
			std::cout << chat->buf << std::endl;
		}
		break;
	}
}

void SendThread()
{
	char sendBuffer[100];
	//WSAEVENT wsaEvent = ::WSACreateEvent();
	while (true)
	{
		std::cout << "msg : ";
		std::cin.getline(sendBuffer, 100);
		
		_CHAT chat_packet;
		chat_packet.type = (int8)C_PACKET_TYPE::CHAT;
		chat_packet.size = sizeof(_CHAT);
		strcpy_s(chat_packet.buf, 100, sendBuffer);

		SendEvent cover(reinterpret_cast<char*>(&chat_packet));

		DWORD sendLen = 0;
		DWORD flags = 0;
		if (::WSASend(clientSock, &cover._sWsaBuf, 1, &sendLen, flags, (LPWSAOVERLAPPED)&cover, nullptr) == SOCKET_ERROR)
		{
			//if (::WSAGetLastError() == WSA_IO_PENDING)
			//{
			//	// Pending
			//	::WSAWaitForMultipleEvents(1, &wsaEvent, TRUE, WSA_INFINITE, FALSE);
			//	::WSAGetOverlappedResult(clientSock, (LPWSAOVERLAPPED)&cover, &sendLen, FALSE, &flags);
			//	WSACloseEvent(wsaEvent);

			//}
			//else if(WSAGetLastError() != 0)
			//{
			//	// 진짜 문제 있는 상황
			//	cout << "error1" << endl;
			//	break;
			//}
		}
	}
	//WSACloseEvent(wsaEvent);
}

void RecvThread()
{

	//WSAEVENT wsaEvent = ::WSACreateEvent();
	//OVEREXTEN cover;

	//while (true)
	//{
	//	::ZeroMemory(&cover, sizeof(WSAOVERLAPPED));
	//	cover._over.hEvent = wsaEvent;
	//	DWORD recvLen = 0;
	//	DWORD flags = 0;

	//	if (::WSARecv(clientSock, &cover._wsabuf, 1, &recvLen, &flags, (LPWSAOVERLAPPED)&cover, nullptr) == SOCKET_ERROR)
	//	{
	//		if (::WSAGetLastError() == WSA_IO_PENDING)
	//		{
	//			// Pending
	//			::WSAWaitForMultipleEvents(1, &wsaEvent, TRUE, WSA_INFINITE, FALSE);
	//			::WSAGetOverlappedResult(clientSock, (LPWSAOVERLAPPED)&cover, &recvLen, FALSE, &flags);
	//			 ProcessPacket(cover._buf);
	//		}
	//		else if(WSAGetLastError() != 0)
	//		{
	//			// 진짜 문제 있는 상황
	//			cout << "error2" << endl;
	//			
	//			break;
	//		}
	//	}
	//}
	//WSACloseEvent(wsaEvent);
	while (true)
	{
		gameClient.DoRecv();
	}
}



int main()
{
#pragma region init winsock
	SocketUtil::Init();
	ConnectManager cmgr;
	
	C2S_LOGIN loginPacket;
	loginPacket.size = sizeof(C2S_LOGIN);
	loginPacket.type = (int8)C_PACKET_TYPE::ACQ_LOGIN;
	std::cout << "ID : ";
	std::wcin.getline(loginPacket.name, sizeof(WCHAR) * 10);
	std::cout << endl;
	std::cout << "PW : ";
	std::wcin.getline(loginPacket.pw, sizeof(WCHAR) * 10);

	cmgr.InitConnect(gameClient, "127.0.0.1");
	cmgr.DoConnect(reinterpret_cast<char*>(&loginPacket));
	
	

	GCThreadManager->Launch([=]()
		{
			while (true)
			{
				ClientIocpCore.Processing(); 
				//기존 게임 서버 프로그래밍 Worker Thread에 해당하는 부분
			}
		});
	//GCThreadManager->Launch(RecvThread);
	//GCThreadManager->Launch(SendThread);
	GCThreadManager->Join();

	// Connect
	// 커넥트와 동시에 로그인 패킷 전송
	
	
	// if (WSAConnect(clientSock, (SOCKADDR*)&serveraddr, sizeof(serveraddr), NULL, NULL, NULL, NULL) == SOCKET_ERROR) return - 1;
	

	//::WSACleanup();
	SocketUtil::Close(clientSock);
	SocketUtil::Clear();
}