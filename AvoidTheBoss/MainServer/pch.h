#pragma once

#define WIN32_LEAN_AND_MEAN // ���� ������ �ʴ� ������ Windows ������� �����մϴ�.

#ifdef _DEBUG
#pragma comment(lib, "Debug\\CoreEngine.lib")
#else
#pragma comment(lib, "Release\\CoreEngine.lib")
#endif

#include "CorePch.h"
