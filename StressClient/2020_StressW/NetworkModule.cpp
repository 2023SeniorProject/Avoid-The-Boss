#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "pch.h"
#include "NetworkModule.h"
#include "IocpEvent.h"



int			global_delay;				// ms����, 1000�� ������ Ŭ���̾�Ʈ ���� ����

float point_cloud[MAX_TEST * 2];


void GetPointCloud(int* size, float** points)
{
	int index = 0;
	//for (int i = 0; i < num_connections; ++i)
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
	bool status = true;
	/*if (true == std::atomic_compare_exchange_strong(&g_clients[ci].connected, &status, false)) {
		closesocket(g_clients[ci].client_socket);
		active_clients--;
	}*/
}
