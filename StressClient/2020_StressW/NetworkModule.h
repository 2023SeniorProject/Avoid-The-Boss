#pragma once

#include "IocpCore.h"
#include "Asession.h"
#include "DummySession.h"

//void InitializeNetwork();
void GetPointCloud(int* size, float** points);



const static int MAX_TEST = 10000;
const static int MAX_CLIENTS = MAX_TEST * 2;
const static int INVALID_ID = -1;
const static int MAX_PACKET_SIZE = 255;
const static int MAX_BUFF_SIZE = 255;

extern int global_delay;
extern std::atomic_int active_clients;



constexpr int DELAY_LIMIT = 100;
constexpr int DELAY_LIMIT2 = 150;
constexpr int ACCEPT_DELY = 50;


class NetworkModule : public IocpCore
{
public:
	std::array<DummySession, MAX_CLIENTS> _clients;
public:

	virtual void Disconnect(int32 sid) override;
};