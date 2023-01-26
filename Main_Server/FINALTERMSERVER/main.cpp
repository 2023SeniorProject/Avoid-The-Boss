//#include "MSession.h"
#include "pch.h"
#include "CorePch.h"
//HANDLE h_iocp;
//SOCKET g_s_sock;
//SOCKET g_c_sock;
//OVER_EXP g_a_over;


int main() {
//
//#pragma region 초기화
//	WSADATA wsa;
//	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return -1;
//
//	g_s_sock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
//	if (g_s_sock == INVALID_SOCKET) return -1;
//
//	sockaddr_in serveraddr;
//	::ZeroMemory(&serveraddr, sizeof(serveraddr));
//	serveraddr.sin_addr.s_addr = INADDR_ANY;
//	serveraddr.sin_family = AF_INET;
//	serveraddr.sin_port = htons(PORTNUM);
//
//	if (bind(g_s_sock, (sockaddr*)&serveraddr, sizeof(serveraddr)) == SOCKET_ERROR) return -1;
//	if (listen(g_s_sock, SOMAXCONN) == SOCKET_ERROR) return -1;
//
//	h_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 9999, 0);
//	CreateIoCompletionPort((HANDLE)(g_s_sock), h_iocp, 9999, 0); // 서버 소켓 iocp 핸들에 등록
//	
//	g_c_sock = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
//	
//	SOCKADDR_IN clientaddr;
//	::ZeroMemory(&clientaddr, sizeof(clientaddr));
//	int addr_size = sizeof(clientaddr);
//	g_a_over._comp_type = COMP_TYPE::OP_ACCEPT;
//    DWORD recv_bytes(0);
//	AcceptEx(g_s_sock, g_c_sock, g_a_over._buf, 0, addr_size + 16, addr_size + 16, &recv_bytes, &g_a_over._over);
//#pragma endregion
//
//    
//	
//	WSACleanup();

}