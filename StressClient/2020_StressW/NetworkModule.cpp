#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "pch.h"
#include "NetworkModule.h"
#include "IocpEvent.h"


extern HWND		hWnd;



std::chrono::high_resolution_clock::time_point last_connect_time;







std::array<int, MAX_CLIENTS> client_map;

std::atomic_int num_connections;
std::atomic_int client_to_close;
std::atomic_int active_clients;

int			global_delay;				// ms단위, 1000이 넘으면 클라이언트 증가 종료

float point_cloud[MAX_TEST * 2];

// 나중에 NPC까지 추가 확장 용
struct ALIEN {
	int id;
	int x, y;
	int visible_count;
};

void DisconnectClient(int ci)
{
	bool status = true;
	if (true == std::atomic_compare_exchange_strong(&g_clients[ci].connected, &status, false)) {
		closesocket(g_clients[ci].client_socket);
		active_clients--;
	}
	
}


void ShutdownNetwork()
{

}

void Do_Network()
{
	return;
}

void GetPointCloud(int* size, float** points)
{
	int index = 0;
	for (int i = 0; i < num_connections; ++i)
		/*if (true == g_clients[i].connected) {
			point_cloud[index * 2] = static_cast<float>(g_clients[i].x);
			point_cloud[index * 2 + 1] = static_cast<float>(g_clients[i].y);
			index++;
		}*/

	*size = index;
	*points = point_cloud;
}

void NetworkModule::Disconnect(int32 sid)
{
}
