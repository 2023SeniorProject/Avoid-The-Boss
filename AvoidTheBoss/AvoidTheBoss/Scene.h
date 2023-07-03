#pragma once
#include "CTimer.h"
#include "SceneInterface.h"
#include "Shader.h"
#include "Player.h"
#include "CEmployee.h"
#include "CBoss.h"
#include "CGenerator.h" // ����ġ �и�
#include "ClientPacketEvent.h"

#define MAX_LIGHTS			16 

#define POINT_LIGHT			1
#define SPOT_LIGHT			2
#define DIRECTIONAL_LIGHT	3

class Scheduler;
struct LIGHT
{
	XMFLOAT4				m_xmf4Ambient;
	XMFLOAT4				m_xmf4Diffuse;
	XMFLOAT4				m_xmf4Specular;
	XMFLOAT3				m_xmf3Position;
	float 					m_fFalloff;
	XMFLOAT3				m_xmf3Direction;
	float 					m_fTheta; //cos(m_fTheta)
	XMFLOAT3				m_xmf3Attenuation;
	float					m_fPhi; //cos(m_fPhi)
	bool					m_bEnable;
	int						m_nType;
	float					m_fRange;
	float					padding;
};

struct LIGHTS
{
	LIGHT					m_pLights[MAX_LIGHTS];
	XMFLOAT4				m_xmf4GlobalAmbient;
	int						m_nLights;
};

class CGameScene : public CScene
{
	friend class CSession;
	friend class queueEvent;
	friend class FrameEvent;
public:
	static void CreateCbvSrvDescriptorHeaps(ID3D12Device5* pd3dDevice, int nConstantBufferViews, int nShaderResourceViews);

	static D3D12_GPU_DESCRIPTOR_HANDLE CreateConstantBufferViews(ID3D12Device5* pd3dDevice, int nConstantBufferViews, ID3D12Resource* pd3dConstantBuffers, UINT nStride);
	static D3D12_GPU_DESCRIPTOR_HANDLE CreateShaderResourceViews(ID3D12Device5* pd3dDevice, CTexture* pTexture, UINT nRootParameter, bool bAutoIncrement);

	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUCbvDescriptorStartHandle() { return(m_d3dCbvCPUDescriptorStartHandle); }
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUCbvDescriptorStartHandle() { return(m_d3dCbvGPUDescriptorStartHandle); }
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUSrvDescriptorStartHandle() { return(m_d3dSrvCPUDescriptorStartHandle); }
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUSrvDescriptorStartHandle() { return(m_d3dSrvGPUDescriptorStartHandle); }

	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUCbvDescriptorNextHandle() { return(m_d3dCbvCPUDescriptorNextHandle); }
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUCbvDescriptorNextHandle() { return(m_d3dCbvGPUDescriptorNextHandle); }
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUSrvDescriptorNextHandle() { return(m_d3dSrvCPUDescriptorNextHandle); }
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUSrvDescriptorNextHandle() { return(m_d3dSrvGPUDescriptorNextHandle); }
public:
	CGameScene();
	~CGameScene();
	virtual void InitScene() { _timer.Reset(); }
	//������ ���콺�� Ű���� �޽����� ó���Ѵ�.
	virtual void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM	lParam);
	virtual void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	virtual void CreateShaderVariables(ID3D12Device5* pd3dDevice, 
		ID3D12GraphicsCommandList4* pd3dCommandList);
	virtual void UpdateShaderVariables(
		
		
		ID3D12GraphicsCommandList4  * pd3dCommandList);
	virtual void ReleaseShaderVariables();

	void BuildDefaultLightsAndMaterials();
	virtual void BuildObjects(ID3D12Device5* pd3dDevice, ID3D12GraphicsCommandList4
		* pd3dCommandList);
	void ReleaseObjects();

	//�׷��� ��Ʈ �ñ׳��ĸ� �����Ѵ�. 
	ID3D12RootSignature* CreateGraphicsRootSignature(ID3D12Device5* pd3dDevice);
	ID3D12RootSignature* GetGraphicsRootSignature() { return(m_pd3dGraphicsRootSignature); }

	void AnimateObjects();
	virtual void Render(ID3D12GraphicsCommandList4  * pd3dCommandList, CCamera* pCamera, bool bRaster);

	void ReleaseUploadBuffers();
	void ChangeMyPlayerCamera() 
	{
		_players[_playerIdx]->OnChangeCamera(FIRST_PERSON_CAMERA, 0.f);
		m_pCamera = _players[_playerIdx]->GetCamera();
		m_pd3dcbLights->Map(0, NULL, (void**)&m_pcbMappedLights);
	}
public : // SceneInterface ��� �Լ�
	virtual void ProcessInput(HWND& hWnd);
	virtual void Update(HWND hWnd);
	//virtual void Render(ID3D12GraphicsCommandList5* pd3dCommandList, CCamera* pCamera);
public: // ���´� �ۼ� �Լ�
	CPlayer* GetScenePlayerBySid(const int32 sid) 
	{ 
		for (int i = 0; i < PLAYERNUM; ++i)
		{
			if (_players[i]->m_sid == sid)
			{
				return _players[i];
			}
		}
		return nullptr;
	}
	CPlayer* GetScenePlayerByIdx(const int32 idx)
	{
		return _players[idx];
	}

	CGenerator* GetSceneGenByIdx(const int32 idx)
	{
		if (idx == -1) return nullptr;
		return m_ppGenerator[idx];
	}

	void InitGame(void* packet ,int32 sid);
	void StopTimer() { _timer.Stop(); }
	void StartTimer() { _timer.Start(); }
	void AddEvent(queueEvent*, float);
	void Exit();
public:
	
	CCamera*					m_pCamera;
	WCHAR						txtFrameBuf[20];

	
	//���������� ���콺 ��ư�� Ŭ���� ���� ���콺 Ŀ���� ��ġ�̴�. 
	POINT								m_ptOldCursorPos;

	float								m_fElapsedTime = 0.0f;

	int									m_nGameObjects = 0;
	CGameObject**						m_ppGameObjects = NULL;

	int									m_nHierarchicalGameObjects = 0;
	CGameObject**						m_ppHierarchicalGameObjects = NULL;

	int									m_nShaders = 0;
	CShader**							m_ppShaders = NULL;
	CSkyBox*							m_pSkyBox = NULL;

	LIGHT*								m_pLights = NULL;
	int									m_nLights = 0;
	XMFLOAT4							m_xmf4GlobalAmbient;
	ID3D12Resource*						m_pd3dcbLights = NULL;
	LIGHTS*								m_pcbMappedLights = NULL;


private:
	Timer _timer;
// ========== ���� ó���� ���� ����ϴ� ������ ==============
public: // ���� �ִ� ������Ʈ ���� ����
	CPlayer*					_players[4];
	int16						_playerIdx = 0;
	int16						m_lastKeyInput = 0;


	// ������
	int							m_nGenerator = 3;
	CGenerator**				m_ppGenerator = NULL;
public:
	int32				m_ActiveGeneratorCnt = 0; // Ȱ��ȭ �� ����ġ ī��Ʈ;
public:
	bool						m_bEmpExit = false;
	bool						m_bBossWin = false;
public:
	Scheduler* _jobQueue;
	
	std::shared_mutex _jobQueueLock;
	int32 _curFrame;
	int32 m_cid = -1;
	int32 m_sid = -1;
// ========================================================
protected:
	ID3D12RootSignature* m_pd3dGraphicsRootSignature = NULL;

	static ID3D12DescriptorHeap* m_pd3dCbvSrvDescriptorHeap;

	static D3D12_CPU_DESCRIPTOR_HANDLE	m_d3dCbvCPUDescriptorStartHandle;
	static D3D12_GPU_DESCRIPTOR_HANDLE	m_d3dCbvGPUDescriptorStartHandle;
	static D3D12_CPU_DESCRIPTOR_HANDLE	m_d3dSrvCPUDescriptorStartHandle;
	static D3D12_GPU_DESCRIPTOR_HANDLE	m_d3dSrvGPUDescriptorStartHandle;

	static D3D12_CPU_DESCRIPTOR_HANDLE	m_d3dCbvCPUDescriptorNextHandle;
	static D3D12_GPU_DESCRIPTOR_HANDLE	m_d3dCbvGPUDescriptorNextHandle;
	static D3D12_CPU_DESCRIPTOR_HANDLE	m_d3dSrvCPUDescriptorNextHandle;
	static D3D12_GPU_DESCRIPTOR_HANDLE	m_d3dSrvGPUDescriptorNextHandle;
};

class CMainScene : public CGameScene
{
public:
	CMainScene() {}
	~CMainScene() {}
	virtual void BuildObjects(ID3D12Device5* pd3dDevice, ID3D12GraphicsCommandList4
		* pd3dCommandList);

	//������ ���콺�� Ű���� �޽����� ó���Ѵ�.
	virtual void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM
		lParam);
	virtual void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM
		lParam);

	virtual void ProcessInput(HWND hWnd);
};

class CLobbyScene : public CGameScene
{
public:
	CLobbyScene() {}
	~CLobbyScene() {}
	virtual void BuildObjects(ID3D12Device5* pd3dDevice, ID3D12GraphicsCommandList4
		* pd3dCommandList);

	//������ ���콺�� Ű���� �޽����� ó���Ѵ�.
	virtual void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM
		lParam);
	virtual void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM
		lParam);

	virtual void ProcessInput(HWND hWnd);
};