#pragma once
#include "SceneInterface.h"
class LobbyScene : public SceneInterface
{
	ID3D12Device* m_d3dDevice;
public:
	LobbyScene(ID3D12Device* device) { m_d3dDevice = device; }
	~LobbyScene()  {}

	virtual void Render() override 
	{
		if (m_d3dDevice == nullptr) return;

	}

};

