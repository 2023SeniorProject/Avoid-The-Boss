#pragma once

#define WIN32_LEAN_AND_MEAN // ���� ������ �ʴ� ������ Windows ������� �����մϴ�.

#ifdef _DEBUG
#pragma comment(lib, "Debug\\ServerCoreEngine.lib")
#else
#pragma comment(lib, "Release\\ServerCoreEngine.lib")
#endif

#include "CorePch.h"