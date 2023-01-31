#include "pch.h"
#include "MSession.h"
#include "ThreadManager.h"
#include "SocketUtil.h"
#include "OBDC_MGR.h"
#include <string>
#include <future>

#define MAX_USER 10

HANDLE g_h_iocp;
SOCKET g_listen_sock;
SOCKET g_client_sock;
OVEREXTEN g_accept_over;
Atomic<int32> client_id = 1;
unordered_map<int32, MSession*> clients;
condition_variable dbcv;
Mutex m;

bool DB_Worker(int32 key, wstring sqlexec);
int32 GetSessionId()
{
    return clients.size();
}

// 패킷 처리 진행
void ProcessPacket(int32 c_id, char* packet)
{
    switch (packet[1]) 
    {
        case (int8)C_PACKET_TYPE::CHAT:
        {
          
            _CHAT* cp = (_CHAT*)packet;
            std::cout << cp->buf << endl;
        }
        break;
        case (int8)C_PACKET_TYPE::ACQ_LOGIN:
        {
         
            C2S_LOGIN* cp = (C2S_LOGIN*)packet;
            wstring sqlExec(L"EXEC search_user_db ");
            sqlExec += cp->name;
            sqlExec += L", ";
            sqlExec += cp->pw;
            DB_Worker(c_id, sqlExec);
        
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
           
            case OP_ACCEPT: // 클라이언트 접속 시, 등록
            {
                int32 Session_id = GetSessionId();
                if (Session_id < MAX_USER)
                {
                    MSession* tmp = new MSession(g_client_sock,client_id);
                    cout << "Client Accept!" << endl;
                    CreateIoCompletionPort(reinterpret_cast<HANDLE>(g_client_sock),
                        h_iocp, client_id, 0);
                    clients.try_emplace(client_id, tmp);
                    clients[client_id]->DoRecv();
                    g_client_sock = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
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
               
                int remain_data = num_bytes + clients[key]->_prev_remain;
                char* p = ex_over->_buf;
                while (remain_data > 0) 
                {
                    int8 packet_size = p[0];
                    if (packet_size <= remain_data)
                    {
                        ProcessPacket(static_cast<int32>(key), p);
                        p = p + packet_size;
                        remain_data = remain_data - packet_size;
                    }
                    else break;
                }
                clients[key]->_prev_remain = remain_data;
                if (remain_data > 0) {
                    memcpy(ex_over->_buf, p, remain_data);
                }
                clients[key]->DoRecv();
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


bool DB_Worker(int32 key,wstring sqlexec)
{
    USER_DB_MANAGER udb;
    udb.AllocateHandles();
    udb.ConnectDataSource(L"2023SENIORPROJECT");
    const WCHAR* a = sqlexec.c_str();
    udb.ExecuteStatementDirect(a);
    udb.RetrieveResult();

    lockG lg(m);
    {
        clients[key]->_cid = udb.user_cid;
        if (clients[key]->_cid == -1)
        {
            cout << "LoginFail" << endl;
            udb.DisconnectDataSource();
            return false;
        }
    }
    cout << "LoginSuccess" << endl;
    udb.DisconnectDataSource();
    return true;
}

int main() 
{

	GThreadManager = new ThreadManager();
    clients.reserve(10);
#pragma region 초기화
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
	// 1. 리슨 소켓
	// 2. 접속한 클라이언트 소켓,
	// 3. 로컬 주소 및 리모트 주소를 받기 위한 버퍼
	// 4. 최초 데이터 수신을 위한 버퍼 크기 = 0
	// 5. 주소를 얻기위한 버퍼 크기로 sockaddr_in + 16사이즈에 해당함.
	// 6. 6도 마찬가지
	// 7. NULL
	// 8. accept 전용 확장 오버랩드 구조체
    g_accept_over._comp_type = OP_ACCEPT; // Accept용 오버랩드 구조체임을 알려준다.
    
	AcceptEx(g_listen_sock, g_client_sock, g_accept_over._buf, 0, sizeof(clientaddr) + 16, sizeof(clientaddr) + 16, NULL, &g_accept_over._over);

#pragma endregion

	for (int i = 0; i < thread::hardware_concurrency(); ++i)
		GThreadManager->Launch(WorkerThread, g_h_iocp);
	GThreadManager->Join();
    
    WSACleanup();
}