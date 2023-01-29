#pragma once
// ==== Socket Util
// 소켓 프로그램 처음 시작 시, winsock 초기화
class SocketUtil
{
public:
	static void Init();
	static void Clear();
	static SOCKET CreateSocket();
	
	static bool Connect(SOCKET s);
	static bool Bind(SOCKET s);
	static bool Listen(SOCKET s);
	static bool Close(SOCKET& s);
};

