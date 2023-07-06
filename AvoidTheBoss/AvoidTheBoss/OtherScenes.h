#pragma once
#include "CScene.h"

class CLobbyScene : public CScene
{
	CPlayer* m_player = NULL;
public:
	CLobbyScene() {}
	~CLobbyScene() {}
	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ProcessInput(HWND& hWnd);
	virtual void Update(HWND hWnd);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);
	void		 BuildDefaultLightsAndMaterials();

	virtual void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM	lParam) {};
	virtual void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) {};
};

class CTitleScene : public CScene
{
	CPlayer* m_player = NULL;

public:
	CTitleScene() {}
	~CTitleScene() {}
	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ProcessInput(HWND& hWnd);
	virtual void Update(HWND hWnd);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);

	virtual void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM	lParam);
	virtual void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	void		 MouseAction(const POINT& mp);
	void		 BuildDefaultLightsAndMaterials();
};

class CRoomScene : public CScene
{
	CPlayer* m_player = NULL;
	int32 m_nMembers = 0;
public:
	CRoomScene() {}
	~CRoomScene() {}
	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ProcessInput(HWND& hWnd);
	virtual void Update(HWND hWnd);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);
	void		 BuildDefaultLightsAndMaterials();

	virtual void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM	lParam) {};
	virtual void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) {};
};