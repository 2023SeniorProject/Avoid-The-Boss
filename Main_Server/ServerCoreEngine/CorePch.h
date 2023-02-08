#pragma once

#include "Types.h"
#include "CoreGlobal.h"
#include "CoreMacro.h"
#include "CoreTLS.h"
#include "RWLock.h"
#include "protocol.h"


#include <vector>
#include <list>
#include <queue>
#include <stack>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <shared_mutex>

#include <iostream>
#include <WinSock2.h>
#include <MSWSock.h>
#include <WS2tcpip.h>

#pragma comment(lib, "WS2_32.lib")
#pragma comment(lib, "MSWSock.lib")
using namespace std;

#define BUFSIZE 512

enum COMP_TYPE{ OP_RECV, OP_SEND, OP_ACCEPT };

class OVEREXTEN
{

public:
	OVEREXTEN()
	{
		_wsabuf.len = BUFSIZE;
		_wsabuf.buf = _buf;
		_comp_type = OP_RECV;
		::ZeroMemory(&_over, sizeof(_over));
	}
	OVEREXTEN(char* packet)
	{
		_wsabuf.buf = _buf;
		_wsabuf.len = packet[0];
		::ZeroMemory(&_over, sizeof(_over));
		_comp_type = OP_SEND;
		memcpy(_buf, packet, packet[0]);
	}
	~OVEREXTEN()
	{
	}
public:
	WSAOVERLAPPED _over;
	COMP_TYPE _comp_type;
	WSABUF _wsabuf;
	char _buf[BUFSIZE];

};