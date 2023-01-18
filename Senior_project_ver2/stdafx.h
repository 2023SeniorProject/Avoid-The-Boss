// header.h: 표준 시스템 포함 파일
// 또는 프로젝트 특정 포함 파일이 들어 있는 포함 파일입니다.
//

#pragma once

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.
// Windows 헤더 파일
#include <windows.h>
// C 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>


//필요한 d3d12 라이브러리들을 링크한다
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib,"D3D12.lib") //정적 Direct3D 12 API 스텁 라이브러리
#pragma comment(lib,"dxgi.lib")	