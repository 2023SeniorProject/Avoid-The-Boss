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

	//������ ���콺�� Ű���� �޽����� ó���Ѵ�.
	virtual void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM
		lParam);
	virtual void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM
		lParam);

	void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList
		* pd3dCommandList);
	void InitScene();
	void ReleaseObjects();

	//�׷��� ��Ʈ �ñ׳��ĸ� �����Ѵ�. 
	ID3D12RootSignature* CreateGraphicsRootSignature(ID3D12Device* pd3dDevice);
	ID3D12RootSignature* GetGraphicsRootSignature();
	void SetGraphicsRootSignature(ID3D12GraphicsCommandList* pd3dCommandList) { pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature); }
	
	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseShaderVariables();

	virtual void ProcessInput(HWND hWnd);
	void AnimateObjects();
	void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);

	int16 GetPlayerIdx(int16 sid) { for (int i = 0; i < 4; ++i) if (_playersSid[i] == sid) return i; }
	void ReleaseUploadBuffers();
public:

	Timer				m_Timer;
	CCamera* m_pCamera = NULL;
public:
	//��ġ(Batch) ó���� �ϱ� ���Ͽ� ���� ���̴����� ����Ʈ�� ǥ���Ѵ�. 
	CShader** m_ppShaders = NULL;
	int m_nShaders = 0;

	int16 _playersSid[4];
	CPlayer* _players[4];
	
	DWORD				m_lastKeyInput = 0;
	//���������� ���콺 ��ư�� Ŭ���� ���� ���콺 Ŀ���� ��ġ�̴�. 
	POINT				m_ptOldCursorPos;
	ID3D12RootSignature* m_pd3dGraphicsRootSignature = NULL;
	//��Ʈ �ñ׳��ĸ� ��Ÿ���� �������̽� �������̴�. 
};

