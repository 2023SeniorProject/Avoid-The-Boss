#include "pch.h"
#include "SocketUtil.h"
int main()
{
#pragma region init winsock
	WSAData wsaData;
	if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		return 0;

	SOCKET clientSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (clientSocket == INVALID_SOCKET)
		return 0;

	/*u_long on = 1;
	if (::ioctlsocket(clientSocket, FIONBIO, &on) == INVALID_SOCKET)
		return 0;*/

	SOCKADDR_IN serveraddr;
	::memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	::inet_pton(AF_INET, "127.0.0.1", &serveraddr.sin_addr);
	serveraddr.sin_port = ::htons(PORTNUM);

	// Connect
	
	if (WSAConnect(clientSocket, (SOCKADDR*)&serveraddr, sizeof(serveraddr), NULL, NULL, NULL, NULL) == SOCKET_ERROR) return - 1;
	

	cout << "Connected to Server!" << endl;

	
	WSAEVENT wsaEvent = ::WSACreateEvent();
	OVEREXTEN cover;
	cover._over.hEvent = wsaEvent;
	char sendBuffer[100];
	// Send
	while (true)
	{
		cin.getline(sendBuffer, 100);

		C2S_CHAT chat_packet;
		chat_packet.type = (int8)CHAT;
		chat_packet.size = sizeof(C2S_CHAT);
		strcpy_s(chat_packet.buf,100, sendBuffer);

		WSABUF wsaBuf;
		wsaBuf.buf = (char*)&chat_packet;
		wsaBuf.len = sizeof(C2S_CHAT);

		DWORD sendLen = 0;
		DWORD flags = 0;
		if (::WSASend(clientSocket, &wsaBuf, 1, &sendLen, flags, (LPWSAOVERLAPPED)&cover, nullptr) == SOCKET_ERROR)
		{
			if (::WSAGetLastError() == WSA_IO_PENDING)
			{
				// Pending
				::WSAWaitForMultipleEvents(1, &wsaEvent, TRUE, WSA_INFINITE, FALSE);
				::WSAGetOverlappedResult(clientSocket, (LPWSAOVERLAPPED)&cover, &sendLen, FALSE, &flags);
			}
			else
			{
				// 진짜 문제 있는 상황
				break;
			}
		}

		cout << "Send Data ! Len = " << sizeof(sendBuffer) << endl;

		this_thread::sleep_for(1s);
	}

	// 소켓 리소스 반환
	::closesocket(clientSocket);

	// 윈속 종료
	::WSACleanup();
}