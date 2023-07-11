#pragma once

#ifdef _DEBUG
#pragma comment(lib, "Debug\\CoreEngine.lib")
#else
#pragma comment(lib, "Release\\CoreEngine.lib")
#endif

#include "CorePch.h"
#include <atomic>
#include <thread>

#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")