#pragma once

#include "Shader.h"

class CScene
{
public:
	CScene();
	~CScene();

	//씬에서 마우스와 키보드 메시지를 처리한다.
	bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM
		lParam);
	bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM
		lParam);

	void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList
		* pd3dCommandList);
	void ReleaseObjects();

	//그래픽 루트 시그너쳐를 생성한다. 
	ID3D12RootSignature* CreateGraphicsRootSignature(ID3D12Device* pd3dDevice);
	ID3D12RootSignature* GetGraphicsRootSignature() { return(m_pd3dGraphicsRootSignature); }
	void SetGraphicsRootSignature(ID3D12GraphicsCommandList* pd3dCommandList) { pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature); }

	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseShaderVariables();

	bool ProcessInput(UCHAR* pKeysBuffer);
	void AnimateObjects(float fTimeElapsed);
	void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);

	void ReleaseUploadBuffers();

	CPlayer* m_pPlayer = NULL;
protected:
	//배치(Batch) 처리를 하기 위하여 씬을 셰이더들의 리스트로 표현한다. 
	CShader **m_ppShaders = NULL;
	int m_nShaders = 0;

	ID3D12RootSignature* m_pd3dGraphicsRootSignature = NULL;
	//루트 시그너쳐를 나타내는 인터페이스 포인터이다. 


protected:
	CMapLand* m_pLand = NULL;
};

