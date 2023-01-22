﻿// header.h: 표준 시스템 포함 파일
// 또는 프로젝트 특정 포함 파일이 들어 있는 포함 파일입니다.
// PCH : 미리 컴파일된 헤더 포함

#pragma once

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN  // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.

//----Windows 헤더 파일
#include <windows.h>

#pragma comment(lib,"winmm.lib")

//----C 런타임 헤더 파일
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <timeapi.h>


//----클라이언트 영역 크기 및 후면 버퍼 크기
#define FRAME_BUFFER_WIDTH 800
#define FRAME_BUFFER_HEIGHT 600

//#define _WITH_SWAPCHAIN_FULLSCREEN_STATE

//----D3D 라이브러리 헤더 포함
#include <string>
#include <wrl.h>
#include <shellapi.h>

#include <d3d12.h>	//Direct3D 12 API 함수
#include <dxgi1_4.h>
#include <D3Dcompiler.h>

#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>

#include <DXGIDebug.h>

using namespace DirectX;
using namespace DirectX::PackedVector;

using Microsoft::WRL::ComPtr;

//----임포트 라이브러리
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib,"d3d12.lib") //정적 Direct3D 12 API 스텁 라이브러리
//#pragma comment(lib,"D3D12.lib") //정적 Direct3D 12 API 스텁 라이브러리
#pragma comment(lib, "dxgi.lib")

#pragma comment(lib, "dxguid.lib")

//----버퍼 리소스 생성 함수
extern ID3D12Resource* CreateBufferResource(ID3D12Device* pd3dDevice,
	ID3D12GraphicsCommandList* pd3dCommandList, void* pData, UINT nBytes, D3D12_HEAP_TYPE
	d3dHeapType = D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATES d3dResourceStates =
	D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, ID3D12Resource** ppd3dUploadBuffer =
	NULL);





