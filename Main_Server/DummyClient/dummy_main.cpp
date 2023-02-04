#include "pch.h"
#include "SocketUtil.h"



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
	WSAEVENT wsaEvent = ::WSACreateEvent();
	while (true)
	{
		std::cout << "msg : ";
		std::cin.getline(sendBuffer, 100);
		
		_CHAT chat_packet;
		chat_packet.type = (int8)C_PACKET_TYPE::CHAT;
		chat_packet.size = sizeof(_CHAT);
		strcpy_s(chat_packet.buf, 100, sendBuffer);

		OVEREXTEN cover(reinterpret_cast<char*>(&chat_packet));
		cover._over.hEvent = wsaEvent;

		DWORD sendLen = 0;
		DWORD flags = 0;
		if (::WSASend(clientSock, &cover._wsabuf, 1, &sendLen, flags, (LPWSAOVERLAPPED)&cover, nullptr) == SOCKET_ERROR)
		{
			if (::WSAGetLastError() == WSA_IO_PENDING)
			{
				// Pending
				::WSAWaitForMultipleEvents(1, &wsaEvent, TRUE, WSA_INFINITE, FALSE);
				::WSAGetOverlappedResult(clientSock, (LPWSAOVERLAPPED)&cover, &sendLen, FALSE, &flags);
				WSACloseEvent(wsaEvent);

			}
			else if(WSAGetLastError() != 0)
			{
				// 진짜 문제 있는 상황
				cout << "error1" << endl;
				break;
			}
		}
	}
	WSACloseEvent(wsaEvent);
}

void RecvThread()
{

	WSAEVENT wsaEvent = ::WSACreateEvent();
	OVEREXTEN cover;

	while (true)
	{
		::ZeroMemory(&cover, sizeof(WSAOVERLAPPED));
		cover._over.hEvent = wsaEvent;
		DWORD recvLen = 0;
		DWORD flags = 0;

		if (::WSARecv(clientSock, &cover._wsabuf, 1, &recvLen, &flags, (LPWSAOVERLAPPED)&cover, nullptr) == SOCKET_ERROR)
		{
			if (::WSAGetLastError() == WSA_IO_PENDING)
			{
				// Pending
				::WSAWaitForMultipleEvents(1, &wsaEvent, TRUE, WSA_INFINITE, FALSE);
				::WSAGetOverlappedResult(clientSock, (LPWSAOVERLAPPED)&cover, &recvLen, FALSE, &flags);
				 ProcessPacket(cover._buf);
			}
			else if(WSAGetLastError() != 0)
			{
				// 진짜 문제 있는 상황
				cout << "error2" << endl;
				
				break;
			}
		}
	}
	WSACloseEvent(wsaEvent);
}



int main()
{
#pragma region init winsock
	/*WSAData wsaData;
	if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		return 0;

	clientSock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (clientSock == INVALID_SOCKET)
		return 0;*/
	SocketUtil::Init();
	clientSock = SocketUtil::CreateSocket();
	sockaddr_in serveraddr;
	serveraddr.sin_family = AF_INET;
	inet_pton(AF_INET, "127.0.0.1", &serveraddr.sin_addr);
	serveraddr.sin_port = htons(PORTNUM);
	char buf[512] = "hello\0";
	DWORD sb(0);
	WSAOVERLAPPED over;
	::ZeroMemory(&over, sizeof(WSAOVERLAPPED));
	if(!SocketUtil::ConnectEx(clientSock,reinterpret_cast<sockaddr*>(&serveraddr),sizeof(serveraddr),buf,BUFSIZE,&sb, &over)) cout << "connect fail" << endl;
	//SOCKADDR_IN serveraddr;
	//::memset(&serveraddr, 0, sizeof(serveraddr));
	//serveraddr.sin_family = AF_INET;
	//::inet_pton(AF_INET, "127.0.0.1", &serveraddr.sin_addr);
	//serveraddr.sin_port = ::htons(PORTNUM);

	// Connect
	// 커넥트와 동시에 로그인 패킷 전송
	/*C2S_LOGIN loginPacket;
	loginPacket.size = sizeof(C2S_LOGIN);
	loginPacket.type = (int8)C_PACKET_TYPE::ACQ_LOGIN;
	std::cout << "ID : ";
	std::wcin.getline(loginPacket.name, sizeof(WCHAR) * 10);
	std::cout << endl;
	std::cout << "PW : ";
	std::wcin.getline(loginPacket.pw, sizeof(WCHAR) * 10);
	*/
	// if (WSAConnect(clientSock, (SOCKADDR*)&serveraddr, sizeof(serveraddr), NULL, NULL, NULL, NULL) == SOCKET_ERROR) return - 1;
	while (true)
	{
		::GetQueuedCompletionStatus()
	}
	cout << "Connected to Server!" << endl;

	//WSAEVENT wsaEvent = ::WSACreateEvent();
	//OVEREXTEN cover(reinterpret_cast<char*>(&loginPacket));
	//cover._over.hEvent = wsaEvent;
	//DWORD sendlen(0);
	//DWORD flags(0);

	//if (::WSASend(clientSock, &cover._wsabuf, 1, &sendlen, flags, &cover._over, nullptr) == SOCKET_ERROR)
	//{
	//	if (WSAGetLastError() == WSA_IO_PENDING)
	//	{
	//		::WSAWaitForMultipleEvents(1, &wsaEvent, TRUE, WSA_INFINITE, FALSE);
	//		::WSAGetOverlappedResult(clientSock, &cover._over, &sendlen, FALSE, &flags);
	//		WSACloseEvent(wsaEvent);
	//		
	//	}
	//	else if (WSAGetLastError() != 0)
	//	{

	//		cout << ::WSAGetLastError() << "error" << endl;
	//		WSACloseEvent(wsaEvent);
	//		return -1;
	//	}
	//}

 //  
	//// Send
	//std::thread st(SendThread);
	//std::thread rt(RecvThread);

	//st.join();
	//rt.join();

	// 소켓 리소스 반환
	//if(clientSock != INVALID_SOCKET)::closesocket(clientSock);

	//// 윈속 종료
	//::WSACleanup();
	SocketUtil::Close(clientSock);
	SocketUtil::Clear();
}