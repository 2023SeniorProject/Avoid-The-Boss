#pragma once

// ==== Socket Util
// 소켓 프로그램 처음 시작 시, winsock 초기화
class SocketUtil
{
public:
	static LPFN_ACCEPTEX AcceptEx;
	static LPFN_CONNECTEX ConnectEx;
	static LPFN_DISCONNECTEX DisconnectEx;
public:
	static void Init();
	static void Clear();
	static SOCKET CreateSocket();
	
	static bool Connect(SOCKET s);
	static bool Bind(SOCKET s);
	static bool Connect(SOCKET s, string Address);
	static bool Listen(SOCKET s);
	static bool Close(SOCKET& s);
	static bool BindWindowsFunction(SOCKET socket, GUID guid, LPVOID* fn);
	static bool SetLinger(SOCKET socket, uint16 onoff, uint16 linger);
	static bool SetReuseAddress(SOCKET socket, bool flag);
	static bool SetRecvBufferSize(SOCKET socket, int32 size);
	static bool SetSendBufferSize(SOCKET socket, int32 size);
	static bool SetTcpNoDelay(SOCKET socket, bool flag);
	static bool SetUpdateAcceptSocket(SOCKET socket, SOCKET listenSocket);
};

template<class T>
static inline bool SetSockOpt(SOCKET socket, int32 level, int32 optName, T optVal)
{
	return SOCKET_ERROR != ::setsockopt(socket, level, optName, reinterpret_cast<char*>(&optVal), sizeof(T));
}