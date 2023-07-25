#pragma once
//----CGameFramework 클래스
//----1 Direct3D 디바이스 생성과 관리 
//----2 화면 출력을 위한 처리 - 게임 객체 생성과 관리, 사용자 입력, 애니메이션 작업

#include "Camera.h"
#include "DXSampleHelper.h"

//#include "DXRHelpers/nv_helpers_dx12/TopLevelASGenerator.h"

class CSound;
class CScene;
class UIManager;
class SceneManager;

// 이 자습서에서는 단일 하위 수준 pResult에서 AS만 사용
// 최상위 AS의 경우 동적 변경을 구현할 가능성을 두고 생성한 구조체이다.
// 가속구조 설명자(버퍼) 구축
// #DXR
struct AccelerationStructureBuffers
{
	ComPtr<ID3D12Resource> pScratch; // Scratch memory for AS builder 스크래치 메모리
	ComPtr<ID3D12Resource> pResult; // Where the AS is 구조 실제 저장소
	ComPtr<ID3D12Resource> pInstanceDesc; // Hold the matrices of the instances 최상위 가속 구조 인스턴스 매트릭스 
};

class CGameFramework
{

public:	
	enum class SCENESTATE { TITLE = 0, LOBBY = 1, ROOM = 2, INGAME = 3, RESULT = 4 };
	
	friend class queueEvent;
	friend class moveEvnet;
	friend class posEvent;
	friend class InteractionEvent;
	friend class FrameEvent;
	
	friend class CEmployee;
	friend class CBoss;

	friend class CSession;

	friend class CGameScene;
	friend class CRoomScene;
	friend class CLobbyScene;
	friend class CTitleScene;

	friend class UIManager;
private:
	HINSTANCE m_hInstance;
	HWND m_hWnd;

	int	m_nWndClientWidth = FRAME_BUFFER_WIDTH;
	int	m_nWndClientHeight = FRAME_BUFFER_HEIGHT;

	
	IDXGIFactory4*				m_pdxgiFactory = NULL;
	//DXGI 팩토리 인터페이스에 대한 포인터이다. 
	IDXGISwapChain3*			m_pdxgiSwapChain = NULL;
	//스왑 체인 인터페이스에 대한 포인터이다. 주로 디스플레이를 제어하기 위하여 필요하다.		
	ID3D12Device5*				m_pd3dDevice = NULL;								//5.14 광선추적 Device5로 버전 변경
	//Direct3D 디바이스 인터페이스에 대한 포인터이다. 주로 리소스를 생성하기 위하여 필요하다.

	bool						m_bMsaa4xEnable = false;
	UINT						m_nMsaa4xQualityLevels = 0;
	//MSAA 다중 샘플링을 활성화하고 다중 샘플링 레벨을 설정한다.

	static const UINT			m_nSwapChainBuffers = 2;
	//스왑 체인의 후면 버퍼의 개수이다. 
	UINT						m_nSwapChainBufferIndex = 0;
	//현재 스왑 체인의 후면 버퍼 인덱스이다.

	ID3D12Resource*				m_ppd3dSwapChainBackBuffers[m_nSwapChainBuffers];
	ID3D12DescriptorHeap*		m_pd3dRtvDescriptorHeap;
	UINT						m_nRtvDescriptorIncrementSize;
	//렌더 타겟 버퍼, 서술자 힙 인터페이스 포인터, 렌더 타겟 서술자 원소의 크기이다.

	ID3D12Resource*				m_pd3dDepthStencilBuffer;
	ID3D12DescriptorHeap*		m_pd3dDsvDescriptorHeap;
	UINT						m_nDsvDescriptorIncrementSize;
	//깊이-스텐실 버퍼, 서술자 힙 인터페이스 포인터, 깊이-스텐실 서술자 원소의 크기이다.

	ID3D12CommandQueue*			m_pd3dCommandQueue;
	ID3D12CommandAllocator*		m_pd3dCommandAllocator;
	ID3D12GraphicsCommandList4*	m_pd3dCommandList;		//5.14 광선추적 버전 4로 변경
	//명령 큐, 명령 할당자, 명령 리스트 인터페이스 포인터이다.

	// 게임 프레임워크에 필요한 각종 매니저들
protected:
	SceneManager*				m_SceneManager = nullptr;
	UIManager*					m_UIRenderer   = nullptr;
#if defined(_DEBUG)
	ID3D12Debug*				m_pd3dDebugController;
#endif

	ID3D12Fence					*m_pd3dFence;
	UINT64						m_nFenceValues[m_nSwapChainBuffers];
	HANDLE						m_hFenceEvent;
	//펜스 인터페이스 포인터, 펜스의 값, 이벤트 핸들이다.

	//다음은 게임 프레임워크에서 사용할 타이머이다. 
	

	//다음은 프레임 레이트를 주 윈도우의 캡션에 출력하기 위한 문자열이다. 
	WCHAR										m_pszFrameRate[500];
public:
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>  m_d3d11DeviceContext;
	Microsoft::WRL::ComPtr<ID3D11On12Device>	 m_d3d11On12Device;
public:
	int32 m_curFrame = 0;

protected:
	static const int							m_nScene = 4;
	Atomic<int32>								m_curScene = 3;

public:
	CGameFramework();
	~CGameFramework();

	//프레임워크를 초기화하는 함수이다(주 윈도우가 생성되면 호출된다). 
	bool OnCreate(HINSTANCE hInstance, HWND hMainWnd);
	void OnDestroy();

	void CreateSwapChain();
	void CreateDirect3DDevice();	//createDirect3DDisplay()
	void CreateCommandQueueAndList();
	//스왑 체인, 디바이스, 서술자 힙, 명령 큐/할당자/리스트를 생성하는 함수이다. 
	
	
	void CreateRtvAndDsvDescriptorHeaps();

	void CreateRenderTargetViews();
	void CreateDepthStencilView();
	//렌더 타겟 뷰와 깊이-스텐실 뷰를 생성하는 함수이다.

	//----전체 화면 모드 F9
	void ChangeSwapChainState();

	void BuildScenes();
	void ReleaseScenes();
	//렌더링할 메쉬와 게임 객체를 생성하고 소멸하는 함수이다. 

	//프레임워크의 핵심(사용자 입력, 애니메이션, 렌더링)을 구성하는 함수이다. 
	void ProcessInput();
	void UpdateObject();
	void AnimateObjects();
	void FrameAdvance();
	void WaitForGpuComplete();
	//CPU와 GPU를 동기화하는 함수이다.
	void Render();
	void MoveToNextFrame();

	// 씬 관련 함수 추가 07-03~ 07-04
	void ChangeScene(SCENESTATE ss);
public:
	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM
		lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM
		lParam);
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam,
		LPARAM lParam);
	//윈도우의 메시지(키보드, 마우스 입력)를 처리하는 함수이다. 

	void CheckRaytracingSupport(); //광선추적 지원 체크 함수 D3D12_FEATURE_D3D12_OPTIONS5

	virtual void OnKeyDown(UINT8 key);
	bool m_raster = true; // 래스터화 상태로 시작, 래스터화 off 시 광선추적 상태

	// 가속 구조 구축
private:
	//ComPtr<ID3D12Resource> m_bottomLevelAS; // Storage for the bottom Level AS
	//nv_helpers_dx12::TopLevelASGenerator m_topLevelASGenerator;
	//AccelerationStructureBuffers m_topLevelASBuffers;
	//std::vector<std::pair<ComPtr<ID3D12Resource>, DirectX::XMMATRIX>> m_instances;
//
//	/// Create the acceleration structure of an instance
//	///
//	/// \param vVertexBuffers : pair of buffer and vertex count
//	/// \return AccelerationStructureBuffers for TLAS
//	AccelerationStructureBuffers CreateBottomLevelAS(std::vector<std::pair<ComPtr<ID3D12Resource>, uint32_t>> vVertexBuffers); //  GPU 메모리의 정점 버퍼 및 정점 수 하위 레벨 AS를 생성
//
//	/// Create the main acceleration structure that holds
//	/// all instances of the scene
//	/// \param instances : pair of BLAS and transform
//	void CreateTopLevelAS(const std::vector<std::pair<ComPtr<ID3D12Resource>, DirectX::XMMATRIX>>& instances); // 최하위 AS 및 변환 행렬 에서 최상위 AS를 생성
//
//	/// Create all acceleration structures, bottom and top
//	void CreateAccelerationStructures(); //위의 방법을 함께 바인딩
};

extern CGameFramework mainGame;
