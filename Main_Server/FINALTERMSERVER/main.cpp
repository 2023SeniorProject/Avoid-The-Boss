#include "pch.h"
#include "MSession.h"
#include "ThreadManager.h"
#include "SocketUtil.h"

HANDLE g_h_iocp;
SOCKET g_listen_sock;
SOCKET g_client_sock;
OVEREXTEN g_accept_over;
Atomic<int32> client_id = 1;
MSession client;

// ��Ŷ ó�� ����
void ProcessPacket(int32 c_id, char* packet)
{
    switch (packet[1]) 
    {
        case CHAT:
        {
            C2S_CHAT* cp = (C2S_CHAT*)packet;
            std::cout << cp->buf;
        }
        break;
    }
}


void WorkerThread(HANDLE h_iocp)
{
    while (true)
    {
        DWORD num_bytes(0);
        ULONG_PTR key;
        WSAOVERLAPPED* over = nullptr;
        BOOL ret = GetQueuedCompletionStatus(h_iocp, &num_bytes, &key, &over, INFINITE);
        OVEREXTEN* ex_over = reinterpret_cast<OVEREXTEN*>(over);
        if (FALSE == ret) {
            if (ex_over->_comp_type == OP_ACCEPT) cout << "Accept Error";
            else {
                cout << "GQCS Error on client[" << key << "]\n";
                if (ex_over->_comp_type == OP_SEND) delete ex_over;
                continue;
            }
        }

        if ((0 == num_bytes) && ((ex_over->_comp_type == OP_RECV) || (ex_over->_comp_type == OP_SEND))) {
            if (ex_over->_comp_type == OP_SEND) delete ex_over;
            continue;
        }

        switch (ex_over->_comp_type)
        {
           
            case OP_ACCEPT: // Ŭ���̾�Ʈ ���� ��, ���
            {
                if (client_id == 1)
                {
                    cout << "Client Accept!" << endl;
                    client._sock = g_client_sock;
                    client._cid = client_id;
                    client.do_recv_packet();
                    CreateIoCompletionPort(reinterpret_cast<HANDLE>(g_client_sock),
                        h_iocp, client_id, 0);
                    g_client_sock = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
                    client_id.fetch_add(1);
                }
                else {
                    cout << "Max user exceeded.\n";
                }
                
                ZeroMemory(&g_accept_over._over, sizeof(g_accept_over._over));
                int addrsize = sizeof(SOCKADDR_IN);
                AcceptEx(g_listen_sock, g_client_sock, g_accept_over._buf, 0, addrsize + 16, addrsize + 16, 0, &g_accept_over._over);
                break;
            }
            case OP_RECV:
            {
                int remain_data = num_bytes + client._prev_remain;
                char* p = ex_over->_buf;
                while (remain_data > 0) 
                {
                    int packet_size = p[0];
                    if (packet_size <= remain_data)
                    {
                        ProcessPacket(static_cast<int>(key), p);
                        p = p + packet_size;
                        remain_data = remain_data - packet_size;
                    }
                    else break;
                }
                client._prev_remain = remain_data;
                if (remain_data > 0) {
                    memcpy(ex_over->_buf, p, remain_data);
                }
                client.do_recv_packet();
                break;
            }
            case OP_SEND:
            {
                delete ex_over;
                break;
            }
        }
    }
}


int main() 
{

	GThreadManager = new ThreadManager();
    
#pragma region �ʱ�ȭ
    SocketUtil::Init();
    g_listen_sock = SocketUtil::CreateSocket();
    if (g_listen_sock == INVALID_SOCKET) return -1;
    g_client_sock = SocketUtil::CreateSocket();
    if (g_client_sock == INVALID_SOCKET) return -1;
    if(!SocketUtil::Bind(g_listen_sock)) return -2;
    if(!SocketUtil::Listen(g_listen_sock)) return -3;
    
    SOCKADDR_IN clientaddr;
    ::ZeroMemory(&clientaddr, sizeof(clientaddr));
#pragma endregion
    g_h_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
    CreateIoCompletionPort(reinterpret_cast<HANDLE>(g_listen_sock), g_h_iocp, 9999, 0);
	// 1. ���� ����
	// 2. ������ Ŭ���̾�Ʈ ����,
	// 3. ���� �ּ� �� ����Ʈ �ּҸ� �ޱ� ���� ����
	// 4. ���� ������ ������ ���� ���� ũ�� = 0
	// 5. �ּҸ� ������� ���� ũ��� sockaddr_in + 16����� �ش���.
	// 6. 6�� ��������
	// 7. NULL
	// 8. accept ���� Ȯ�� �������� ����ü
    g_accept_over._comp_type = OP_ACCEPT; // Accept�� �������� ����ü���� �˷��ش�.
	AcceptEx(g_listen_sock, g_client_sock, g_accept_over._buf, 0, sizeof(clientaddr) + 16, sizeof(clientaddr) + 16, NULL, &g_accept_over._over);

#pragma endregion

	for (int i = 0; i < thread::hardware_concurrency(); ++i)
		GThreadManager->Launch(WorkerThread, g_h_iocp);
	GThreadManager->Join();
    
    WSACleanup();
}