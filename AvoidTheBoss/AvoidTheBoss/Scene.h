#pragma once
#include "WorldRewinder.h"
#include "SceneInterface.h"
#include "Shader.h"
#include "Player.h"

#define MAX_LIGHTS			16 

#define POINT_LIGHT			1
#define SPOT_LIGHT			2
#define DIRECTIONAL_LIGHT	3

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


class CGameScene : public SceneInterface
{
	friend class CSession;
public:
	CGameScene();
	~CGameScene();

	//씬에서 마우스와 키보드 메시지를 처리한다.
	virtual void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM
		lParam);
	virtual void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM
		lParam);

	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseShaderVariables();

	void BuildDefaultLightsAndMaterials();
	void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList
		* pd3dCommandList);
	void InitScene() { _logic.StartGame(); };
	void ReleaseObjects();

	//그래픽 루트 시그너쳐를 생성한다. 
	ID3D12RootSignature* CreateGraphicsRootSignature(ID3D12Device* pd3dDevice);
	ID3D12RootSignature* GetGraphicsRootSignature() { return(m_pd3dGraphicsRootSignature); }

	virtual void ProcessInput(HWND hWnd);
	void AnimateObjects();
	void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);
	bool CollisionCheck();

	void ReleaseUploadBuffers();

	CPlayer* GetScenePlayer(const int16 sid) 
	{ 
		for (int i = 0; i < PLAYERNUM; ++i)
		{
			if (_players[i]->m_sid == sid) return _players[i];
		}
	}
public:

//protected:
//	//배치(Batch) 처리를 하기 위하여 씬을 셰이더들의 리스트로 표현한다. 

	CCamera* m_pCamera;
	WCHAR	 txtFrameBuf[20];
	CPlayer*					_players[4];
	PlayerInfo					_playerInfo[4]; // 자신의 플레이어 인포 
	
	int16						_playerIdx = 0;
	GameLogic					_logic;

	DWORD						m_lastKeyInput = 0;
	//마지막으로 마우스 버튼을 클릭할 때의 마우스 커서의 위치이다. 
	POINT						m_ptOldCursorPos;

	float								m_fElapsedTime = 0.0f;

	int									m_nGameObjects = 0;
	CGameObject** m_ppGameObjects = NULL;

	int									m_nHierarchicalGameObjects = 0;
	CGameObject** m_ppHierarchicalGameObjects = NULL;

	int									m_nShaders = 0;
	CShader** m_ppShaders = NULL;

	CSkyBox* m_pSkyBox = NULL;

	LIGHT* m_pLights = NULL;
	int									m_nLights = 0;

	XMFLOAT4							m_xmf4GlobalAmbient;

	ID3D12Resource* m_pd3dcbLights = NULL;
	LIGHTS* m_pcbMappedLights = NULL;

	CSwitch* m_pSwitch = NULL;
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

public:
	static void CreateCbvSrvDescriptorHeaps(ID3D12Device* pd3dDevice, int nConstantBufferViews, int nShaderResourceViews);

	static D3D12_GPU_DESCRIPTOR_HANDLE CreateConstantBufferViews(ID3D12Device* pd3dDevice, int nConstantBufferViews, ID3D12Resource* pd3dConstantBuffers, UINT nStride);
	static D3D12_GPU_DESCRIPTOR_HANDLE CreateShaderResourceViews(ID3D12Device* pd3dDevice, CTexture* pTexture, UINT nRootParameter, bool bAutoIncrement);

	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUCbvDescriptorStartHandle() { return(m_d3dCbvCPUDescriptorStartHandle); }
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUCbvDescriptorStartHandle() { return(m_d3dCbvGPUDescriptorStartHandle); }
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUSrvDescriptorStartHandle() { return(m_d3dSrvCPUDescriptorStartHandle); }
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUSrvDescriptorStartHandle() { return(m_d3dSrvGPUDescriptorStartHandle); }

	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUCbvDescriptorNextHandle() { return(m_d3dCbvCPUDescriptorNextHandle); }
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUCbvDescriptorNextHandle() { return(m_d3dCbvGPUDescriptorNextHandle); }
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUSrvDescriptorNextHandle() { return(m_d3dSrvCPUDescriptorNextHandle); }
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUSrvDescriptorNextHandle() { return(m_d3dSrvGPUDescriptorNextHandle); }
};

