#pragma once
//----CGameFramework Ŭ����
//----1 Direct3D ����̽� ������ ���� 
//----2 ȭ�� ����� ���� ó�� - ���� ��ü ������ ����, ����� �Է�, �ִϸ��̼� �۾�
#include "GameTimer.h"
#include "Scene.h"
#include "Camera.h"
#include "Player.h"

class CGameFramework
{
private:
	HINSTANCE m_hInstance;
	HWND m_hWnd;

	int	m_nWndClientWidth = FRAME_BUFFER_WIDTH;
	int	m_nWndClientHeight = FRAME_BUFFER_HEIGHT;

	IDXGIFactory4*				m_pdxgiFactory;
	//DXGI ���丮 �������̽��� ���� �������̴�. 
	IDXGISwapChain3 *			m_pdxgiSwapChain;
	//���� ü�� �������̽��� ���� �������̴�. �ַ� ���÷��̸� �����ϱ� ���Ͽ� �ʿ��ϴ�. 
	ID3D12Device *				m_pd3dDevice;
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
	//���� ť, ���� �Ҵ���, ���� ����Ʈ �������̽� �������̴�.

	//ID3D12PipelineState*		m_pd3dPipelineState;
	//�׷��Ƚ� ���������� ���� ��ü�� ���� �������̽� �������̴�.

	ID3D12Fence*				m_pd3dFence;
	UINT64						m_nFenceValues[m_nSwapChainBuffers];
	HANDLE						m_hFenceEvent;
	//�潺 �������̽� ������, �潺�� ��, �̺�Ʈ �ڵ��̴�.

	//D3D12_VIEWPORT				m_d3dViewport;
	//D3D12_RECT					m_d3dScissorRect;
	//����Ʈ�� ���� �簢���̴�. 
private:
	//������ ���� �����ӿ�ũ���� ����� Ÿ�̸��̴�. 
	CGameTimer					m_GameTimer;

	//������ ������ ����Ʈ�� �� �������� ĸ�ǿ� ����ϱ� ���� ���ڿ��̴�. 
	_TCHAR						m_pszFrameRate[50];

private:
	CScene*						m_pScene;
public:
	//�÷��̾� ��ü�� ���� �������̴�.
	CPlayer* m_pPlayer = NULL;

	//���������� ���콺 ��ư�� Ŭ���� ���� ���콺 Ŀ���� ��ġ�̴�. 
	POINT m_ptOldCursorPos; 
public:
	CGameFramework();
	~CGameFramework();

	//�����ӿ�ũ�� �ʱ�ȭ�ϴ� �Լ��̴�(�� �����찡 �����Ǹ� ȣ��ȴ�). 
	bool OnCreate(HINSTANCE hInstance, HWND hMainWnd);
	void OnDestroy();

	void CreateSwapChain();
	void CreateDirect3DDevice();	//createDirect3DDisplay()
	void CreateCommandQueueAndList();
	//���� ü��, ����̽�, ������ ��, ���� ť/�Ҵ���/����Ʈ�� �����ϴ� �Լ��̴�. 

	void CreateRtvAndDsvDescriptorHeaps();

	void CreateRenderTargetViews();
	void CreateDepthStencilView();
	//���� Ÿ�� ��� ����-���ٽ� �並 �����ϴ� �Լ��̴�.

	//----��ü ȭ�� ��� F9
	void ChangeSwapChainState();

	void BuildObjects();
	void ReleaseObjects();
	//�������� �޽��� ���� ��ü�� �����ϰ� �Ҹ��ϴ� �Լ��̴�. 

	//�����ӿ�ũ�� �ٽ�(����� �Է�, �ִϸ��̼�, ������)�� �����ϴ� �Լ��̴�. 
	void ProcessInput();
	void AnimateObjects();
	void FrameAdvance();

	void WaitForGpuComplete();
	//CPU�� GPU�� ����ȭ�ϴ� �Լ��̴�. 
	void MoveToNextFrame();

	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM
		lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM
		lParam);
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam,
		LPARAM lParam);
	//�������� �޽���(Ű����, ���콺 �Է�)�� ó���ϴ� �Լ��̴�. 

public:
	CCamera* m_pCamera = NULL;
};
