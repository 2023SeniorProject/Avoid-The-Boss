#pragma once
#include "CTimer.h"
#include "SceneInterface.h"
#include "Shader.h"

class CGameScene : public SceneInterface
{
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

	virtual void ProcessInput(HWND hWnd);
	void AnimateObjects();
	void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);

	void ReleaseUploadBuffers();

	//�׷��� ��Ʈ �ñ׳��ĸ� �����Ѵ�. 
	ID3D12RootSignature* CreateGraphicsRootSignature(ID3D12Device* pd3dDevice);
	ID3D12RootSignature* GetGraphicsRootSignature();

public:

	Timer				m_Timer;
	CCamera* m_pCamera = NULL;
protected:
	//��ġ(Batch) ó���� �ϱ� ���Ͽ� ���� ���̴����� ����Ʈ�� ǥ���Ѵ�. 
	CObjectsShader* m_pShaders = NULL;
	int m_nShaders = 0;

	
	DWORD				m_lastKeyInput = 0;
	//���������� ���콺 ��ư�� Ŭ���� ���� ���콺 Ŀ���� ��ġ�̴�. 
	POINT				m_ptOldCursorPos;
	ID3D12RootSignature* m_pd3dGraphicsRootSignature = NULL;
	//��Ʈ �ñ׳��ĸ� ��Ÿ���� �������̽� �������̴�. 
};
