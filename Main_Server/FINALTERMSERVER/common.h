#pragma once
#include <WS2tcpip.h>
#include <MSWSock.h>
#include <WinSock2.h>
#include <process.h>
#include <thread>
#include "MSession.h"
#include "protocol.h"
#include "OBDC_MGR.h"
#include "types.h"
#pragma comment(lib, "WS2_32.lib")
#pragma comment(lib, "MSWSock.lib")

using namespace std;