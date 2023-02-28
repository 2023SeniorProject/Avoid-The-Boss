// header.h: 표준 시스템 포함 파일
// 또는 프로젝트 특정 포함 파일이 들어 있는 포함 파일입니다.
//

#pragma once

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.
// Windows 헤더 파일

#ifdef _DEBUG
#pragma comment(lib, "Debug\\CoreEngine.lib")
#else
#pragma comment(lib, "Release\\CoreEngine.lib")
#endif

#include "CorePch.h"

#define DIR_FORWARD	 0x01
#define DIR_BACKWARD 0x02
#define DIR_LEFT	 0x04
#define DIR_RIGHT	 0x08
#define DIR_UP		 0x10
#define DIR_DOWN	 0x20

#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")

