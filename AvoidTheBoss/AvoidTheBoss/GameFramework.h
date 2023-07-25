#pragma once
//----CGameFramework Ŭ����
//----1 Direct3D ����̽� ������ ���� 
//----2 ȭ�� ����� ���� ó�� - ���� ��ü ������ ����, ����� �Է�, �ִϸ��̼� �۾�

#include "Camera.h"
#include "DXSampleHelper.h"

//#include "DXRHelpers/nv_helpers_dx12/TopLevelASGenerator.h"

class CSound;
class CScene;
class UIManager;
class SceneManager;

// �� �ڽ��������� ���� ���� ���� pResult���� AS�� ���
// �ֻ��� AS�� ��� ���� ������ ������ ���ɼ��� �ΰ� ������ ����ü�̴�.
// ���ӱ��� ������(����) ����
// #DXR
struct AccelerationStructureBuffers
{
	ComPtr<ID3D12Resource> pScratch; // Scratch memory for AS builder ��ũ��ġ �޸�
	ComPtr<ID3D12Resource> pResult; // Where the AS is ���� ���� �����
	ComPtr<ID3D12Resource> pInstanceDesc; // Hold the matrices of the instances �ֻ��� ���� ���� �ν��Ͻ� ��Ʈ���� 
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
	//DXGI ���丮 �������̽��� ���� �������̴�. 
	IDXGISwapChain3*			m_pdxgiSwapChain = NULL;
	//���� ü�� �������̽��� ���� �������̴�. �ַ� ���÷��̸� �����ϱ� ���Ͽ� �ʿ��ϴ�.		
	ID3D12Device5*				m_pd3dDevice = NULL;								//5.14 �������� Device5�� ���� ����
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
	ID3D12GraphicsCommandList4*	m_pd3dCommandList;		//5.14 �������� ���� 4�� ����
	//��� ť, ��� �Ҵ���, ��� ����Ʈ �������̽� �������̴�.

	// ���� �����ӿ�ũ�� �ʿ��� ���� �Ŵ�����
protected:
	SceneManager*				m_SceneManager = nullptr;
	UIManager*					m_UIRenderer   = nullptr;
#if defined(_DEBUG)
	ID3D12Debug*				m_pd3dDebugController;
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
public:
	int32 m_curFrame = 0;

protected:
	static const int							m_nScene = 4;
	Atomic<int32>								m_curScene = 3;

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

	// �� ���� �Լ� �߰� 07-03~ 07-04
	void ChangeScene(SCENESTATE ss);
public:
	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM
		lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM
		lParam);
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam,
		LPARAM lParam);
	//�������� �޽���(Ű����, ���콺 �Է�)�� ó���ϴ� �Լ��̴�. 

	void CheckRaytracingSupport(); //�������� ���� üũ �Լ� D3D12_FEATURE_D3D12_OPTIONS5

	virtual void OnKeyDown(UINT8 key);
	bool m_raster = true; // ������ȭ ���·� ����, ������ȭ off �� �������� ����

	// ���� ���� ����
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
//	AccelerationStructureBuffers CreateBottomLevelAS(std::vector<std::pair<ComPtr<ID3D12Resource>, uint32_t>> vVertexBuffers); //  GPU �޸��� ���� ���� �� ���� �� ���� ���� AS�� ����
//
//	/// Create the main acceleration structure that holds
//	/// all instances of the scene
//	/// \param instances : pair of BLAS and transform
//	void CreateTopLevelAS(const std::vector<std::pair<ComPtr<ID3D12Resource>, DirectX::XMMATRIX>>& instances); // ������ AS �� ��ȯ ��� ���� �ֻ��� AS�� ����
//
//	/// Create all acceleration structures, bottom and top
//	void CreateAccelerationStructures(); //���� ����� �Բ� ���ε�
};

extern CGameFramework mainGame;
