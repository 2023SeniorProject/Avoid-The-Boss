#pragma once
// ==== Socket Util
// ���� ���α׷� ó�� ���� ��, winsock �ʱ�ȭ
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

