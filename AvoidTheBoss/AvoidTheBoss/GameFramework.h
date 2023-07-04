#pragma once
//----CGameFramework Ŭ����
//----1 Direct3D ����̽� ������ ���� 
//----2 ȭ�� ����� ���� ó�� - ���� ��ü ������ ����, ����� �Է�, �ִϸ��̼� �۾�
#include "CGameScene.h"
#include "Camera.h"
#include <queue> 

class UIManager;

class CGameFramework
{

public:	enum class SCENESTATE { TITLE = 0, LOBBY = 1, ROOM = 2, INGAME = 3 };
	friend class queueEvent;
	friend class moveEvnet;
	friend class posEvent;
	friend class InteractionEvent;
	friend class FrameEvent;
	friend class CSession;
	friend class CEmployee;
	friend class CBoss;
	friend class CGameScene;
private:
	HINSTANCE m_hInstance;
	HWND m_hWnd;

	int	m_nWndClientWidth = FRAME_BUFFER_WIDTH;
	int	m_nWndClientHeight = FRAME_BUFFER_HEIGHT;

	
	IDXGIFactory4*				m_pdxgiFactory = NULL;
	//DXGI ���丮 �������̽��� ���� �������̴�. 
	IDXGISwapChain3*			m_pdxgiSwapChain = NULL;
	//���� ü�� �������̽��� ���� �������̴�. �ַ� ���÷��̸� �����ϱ� ���Ͽ� �ʿ��ϴ�. 
	ID3D12Device*				m_pd3dDevice = NULL;
	//Direct3D ����̽� �������̽��� ���� �������̴�. �ַ� ���ҽ��� �����ϱ� ���Ͽ� �ʿ��ϴ�.

	bool						m_bMsaa4xEnable = false;
	UINT						m_nMsaa4xQualityLevels = 0;
	//MSAA ���� ���ø��� Ȱ��ȭ�ϰ� ���� ���ø� ������ �����Ѵ�.

	static const UINT			m_nSwapChainBuffers = 2;
	//���� ü���� �ĸ� ������ �����̴�. 
	UINT						m_nSwapChainBufferIndex = 0;
	//���� ���� ü���� �ĸ� ���� �ε����̴�.

	ID3D12Resource*				m_ppd3dSwapChainBackBuffers[m_nSwapChainBuffers];
	ID3D12DescriptorHeap*		m_pd3dRtvDescriptorHeap;
	UINT						m_nRtvDescriptorIncrementSize;
	//���� Ÿ�� ����, ������ �� �������̽� ������, ���� Ÿ�� ������ ������ ũ���̴�.

	ID3D12Resource*				m_pd3dDepthStencilBuffer;
	ID3D12DescriptorHeap*		m_pd3dDsvDescriptorHeap;
	UINT						m_nDsvDescriptorIncrementSize;
	//����-���ٽ� ����, ������ �� �������̽� ������, ����-���ٽ� ������ ������ ũ���̴�.

	ID3D12CommandQueue*			m_pd3dCommandQueue;
	ID3D12CommandAllocator*		m_pd3dCommandAllocator;
	ID3D12GraphicsCommandList*	m_pd3dCommandList;
	//��� ť, ��� �Ҵ���, ��� ����Ʈ �������̽� �������̴�.

	UIManager* m_UIRenderer = nullptr;
#if defined(_DEBUG)
	ID3D12Debug* m_pd3dDebugController;
#endif

	ID3D12Fence					*m_pd3dFence;
	UINT64						m_nFenceValues[m_nSwapChainBuffers];
	HANDLE						m_hFenceEvent;
	//�潺 �������̽� ������, �潺�� ��, �̺�Ʈ �ڵ��̴�.

	//������ ���� �����ӿ�ũ���� ����� Ÿ�̸��̴�. 
	

	//������ ������ ����Ʈ�� �� �������� ĸ�ǿ� ����ϱ� ���� ���ڿ��̴�. 
	WCHAR										m_pszFrameRate[500];
public:
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>  m_d3d11DeviceContext;
	Microsoft::WRL::ComPtr<ID3D11On12Device>	 m_d3d11On12Device;
protected:
	static const int							m_nScene = 4;

	CScene*										m_ppScene[m_nScene];
	Atomic<int32>								m_curScene = 1;
public:
	CGameFramework();
	~CGameFramework();

	//�����ӿ�ũ�� �ʱ�ȭ�ϴ� �Լ��̴�(�� �����찡 �����Ǹ� ȣ��ȴ�). 
	bool OnCreate(HINSTANCE hInstance, HWND hMainWnd);
	void OnDestroy();

	void CreateSwapChain();
	void CreateDirect3DDevice();	//createDirect3DDisplay()
	void CreateCommandQueueAndList();
	//���� ü��, ����̽�, ������ ��, ��� ť/�Ҵ���/����Ʈ�� �����ϴ� �Լ��̴�. 
	
	
	void CreateRtvAndDsvDescriptorHeaps();

	void CreateRenderTargetViews();
	void CreateDepthStencilView();
	//���� Ÿ�� ��� ����-���ٽ� �並 �����ϴ� �Լ��̴�.

	//----��ü ȭ�� ��� F9
	void ChangeSwapChainState();

	void BuildScenes();
	void ReleaseScenes();
	//�������� �޽��� ���� ��ü�� �����ϰ� �Ҹ��ϴ� �Լ��̴�. 

	//�����ӿ�ũ�� �ٽ�(����� �Է�, �ִϸ��̼�, ������)�� �����ϴ� �Լ��̴�. 
	void ProcessInput();
	void UpdateObject();
	void AnimateObjects();
	void FrameAdvance();
	void WaitForGpuComplete();
	//CPU�� GPU�� ����ȭ�ϴ� �Լ��̴�.
	void Render();
	void MoveToNextFrame();

public:
	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM
		lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM
		lParam);
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam,
		LPARAM lParam);
	//�������� �޽���(Ű����, ���콺 �Է�)�� ó���ϴ� �Լ��̴�. 
};

extern CGameFramework mainGame;
