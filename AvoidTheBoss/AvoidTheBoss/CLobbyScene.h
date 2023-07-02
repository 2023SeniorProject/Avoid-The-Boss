#pragma once
#include "CGameScene.h"

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
	
	
};

class TitleScene : public CScene
{
};

class RoomScene : public CScene
{

};