#pragma once
#include "CTimer.h"
#include "SceneInterface.h"
#include "Shader.h"
#include "Player.h"
#include "DummyPlayer.h"

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

	//������ ���콺�� Ű���� �޽����� ó���Ѵ�.
	virtual void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM
		lParam);
	virtual void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM
		lParam);

	void BuildDefaultLightsAndMaterials();
	void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList
		* pd3dCommandList);
	void InitScene() { m_Timer.Reset(); };
	void ReleaseObjects();

	//�׷��� ��Ʈ �ñ׳��ĸ� �����Ѵ�. 
	ID3D12RootSignature* CreateGraphicsRootSignature(ID3D12Device* pd3dDevice);
	ID3D12RootSignature* GetGraphicsRootSignature();
	//void SetGraphicsRootSignature(ID3D12GraphicsCommandList* pd3dCommandList) { pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature); }
	
	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseShaderVariables();

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
//	//��ġ(Batch) ó���� �ϱ� ���Ͽ� ���� ���̴����� ����Ʈ�� ǥ���Ѵ�. 

	CCamera* m_pCamera;
	Timer m_Timer;

	CPlayer*					_players[4];
	int16						_playerIdx = 0;
	DWORD						m_lastKeyInput = 0;

	//���������� ���콺 ��ư�� Ŭ���� ���� ���콺 Ŀ���� ��ġ�̴�. 
	POINT						m_ptOldCursorPos;
	ID3D12RootSignature*		m_pd3dGraphicsRootSignature = NULL;
	//��Ʈ �ñ׳��ĸ� ��Ÿ���� �������̽� �������̴�. 

	CGameObject**				m_ppGameObjects = NULL;
	int							m_nGameObjects = 0;

	LIGHT*						m_pLights = NULL;
	int							m_nLights = 0;

	XMFLOAT4					m_xmf4GlobalAmbient;

	ID3D12Resource*				m_pd3dcbLights = NULL;
	LIGHTS*						m_pcbMappedLights = NULL;

	float						m_fElapsedTime = 0.0f;
};

