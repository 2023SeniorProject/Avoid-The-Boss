#pragma once
#include "CTimer.h"
#include "SceneInterface.h"
#include "Shader.h"
#include "Player.h"
#include "DummyPlayer.h"


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

	void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList
		* pd3dCommandList);
	void InitScene() { m_Timer.Reset(); };
	void ReleaseObjects();

	//그래픽 루트 시그너쳐를 생성한다. 
	ID3D12RootSignature* CreateGraphicsRootSignature(ID3D12Device* pd3dDevice);
	ID3D12RootSignature* GetGraphicsRootSignature();
	void SetGraphicsRootSignature(ID3D12GraphicsCommandList* pd3dCommandList) { pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature); }
	
	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseShaderVariables();

	virtual void ProcessInput(HWND hWnd);
	void AnimateObjects();
	void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);

	void ReleaseUploadBuffers();

	CPlayer* GetScenePlayer(const int16 sid) 
	{ 
		for (int i = 0; i < PLAYERNUM; ++i)
		{
			if (_players[i]->m_sid == sid) return _players[i];
		}
	}
public:

	Timer				m_Timer;
	CCamera* m_pCamera = NULL;
public:
	//배치(Batch) 처리를 하기 위하여 씬을 셰이더들의 리스트로 표현한다. 
	CShader** m_ppShaders = NULL;
	int m_nShaders = 0;

	CPlayer* _players[4];
	int16 _playerIdx = -1;
	int16 _playerNum = PLAYERNUM;
	DWORD				m_lastKeyInput = 0;
	//마지막으로 마우스 버튼을 클릭할 때의 마우스 커서의 위치이다. 
	POINT				m_ptOldCursorPos;
	ID3D12RootSignature* m_pd3dGraphicsRootSignature = NULL;
	//루트 시그너쳐를 나타내는 인터페이스 포인터이다. 
};

