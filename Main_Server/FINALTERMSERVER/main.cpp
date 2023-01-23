#include "common.h"

HANDLE h_iocp;
SOCKET g_s_sock;
SOCKET g_c_sock;
OVER_EXP g_a_over;


void worker_thread(HANDLE h_iocp)
{
    while (true) {
        DWORD num_bytes(0);
        ULONG_PTR key;
        WSAOVERLAPPED* over = nullptr;
        BOOL ret = GetQueuedCompletionStatus(h_iocp, &num_bytes, &key, &over, INFINITE);
        OVER_EXP* ex_over = reinterpret_cast<OVER_EXP*>(over);
        if (FALSE == ret) {
            if (ex_over->_comp_type == COMP_TYPE::OP_ACCEPT) std::cout << "Accept Error";
            else 
            {
                std::cout << "GQCS Error on client[" << key << "]\n";
                if (ex_over->_comp_type == COMP_TYPE::OP_SEND) delete ex_over;
                continue;
            }
        }

        if ((0 == num_bytes) && ((ex_over->_comp_type == COMP_TYPE::OP_RECV) || (ex_over->_comp_type == COMP_TYPE::OP_SEND))) {
            if (ex_over->_comp_type == COMP_TYPE::OP_SEND) delete ex_over;
            continue;
        }

        switch (ex_over->_comp_type) {
        case COMP_TYPE::OP_ACCEPT: {
           
            ZeroMemory(&g_a_over._over, sizeof(g_a_over._over));
            int addr_size = sizeof(SOCKADDR_IN);
            AcceptEx(g_s_sock, g_c_sock, g_a_over._buf, 0, addr_size + 16, addr_size + 16, 0, &g_a_over._over);

            break;
        }
        case COMP_TYPE::OP_RECV:
        {
        }
            break;
        case COMP_TYPE::OP_SEND:
            delete ex_over;
            break;
        }
    }
}

int main() {

#pragma region 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return -1;

	g_s_sock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (g_s_sock == INVALID_SOCKET) return -1;

	sockaddr_in serveraddr;
	::ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_addr.s_addr = INADDR_ANY;
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(PORTNUM);

	if (bind(g_s_sock, (sockaddr*)&serveraddr, sizeof(serveraddr)) == SOCKET_ERROR) return -1;
	if (listen(g_s_sock, SOMAXCONN) == SOCKET_ERROR) return -1;

	h_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 9999, 0);
	CreateIoCompletionPort((HANDLE)(g_s_sock), h_iocp, 9999, 0); // 서버 소켓 iocp 핸들에 등록
	
	g_c_sock = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	
	SOCKADDR_IN clientaddr;
	::ZeroMemory(&clientaddr, sizeof(clientaddr));
	int addr_size = sizeof(clientaddr);
	g_a_over._comp_type = COMP_TYPE::OP_ACCEPT;
	AcceptEx(g_s_sock, g_c_sock, g_a_over._buf, 0, addr_size + 16, addr_size + 16, 0, &g_a_over._over);
#pragma endregion

	
	WSACleanup();
}