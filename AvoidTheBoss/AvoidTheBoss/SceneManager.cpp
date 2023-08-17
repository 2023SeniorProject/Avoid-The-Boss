#include "pch.h"
#include "SceneManager.h"
#include "CScene.h"
#include "GameScene.h"
#include "OtherScenes.h"
#include "CSound.h"

void SceneManager::Render(ID3D12GraphicsCommandList4* pd3dCommandList, int32 idx, bool Raster)
{
	m_pScenes[idx]->Render(pd3dCommandList, m_pScenes[idx]->m_pCamera, Raster);
}

void SceneManager::Update(HWND& hWnd,int32 idx)
{
	m_pScenes[idx]->Update(hWnd);
}

void SceneManager::ProcessInput(HWND& hWnd, int32 idx)
{
	
	m_pScenes[idx]->ProcessInput(hWnd);
}


CScene* SceneManager::ChangeScene(int32 idx)
{
	return m_pScenes[idx];  // 해당 인덱스에 맞는 씬으로 변경하고 적절한 조치를 취한다.
}

void SceneManager::ResetScene()
{
}

void SceneManager::ReleaseUpBuffers()
{
	for (int i = 0; i < 4; ++i)  m_pScenes[i]->ReleaseUploadBuffers();
}
void SceneManager::ReleaseScene()
{
     m_pScenes[3]->ReleaseObjects();
}
void SceneManager::Animate(int32 idx)
{
	m_pScenes[idx]->AnimateObjects();
}

void SceneManager::BuildScene(ID3D12Device5* pd3dDevice, ID3D12GraphicsCommandList4* pd3dCommandList)
{
	m_pScenes[(int32)SCENESTATE::TITLE] = new CTitleScene();
	m_pScenes[(int32)SCENESTATE::TITLE]->BuildObjects(pd3dDevice, pd3dCommandList);
	m_pScenes[(int32)SCENESTATE::LOBBY] = new CLobbyScene();
	m_pScenes[(int32)SCENESTATE::LOBBY]->BuildObjects(pd3dDevice, pd3dCommandList);
	m_pScenes[(int32)SCENESTATE::ROOM] = new CRoomScene();
	m_pScenes[(int32)SCENESTATE::ROOM]->BuildObjects(pd3dDevice, pd3dCommandList);
	m_pScenes[(int32)SCENESTATE::INGAME] = new CGameScene();
	m_pScenes[(int32)SCENESTATE::INGAME]->BuildObjects(pd3dDevice, pd3dCommandList);

	m_pScenes[(int32)SCENESTATE::RESULT] = new CResultScene();
	m_pScenes[(int32)SCENESTATE::RESULT]->BuildObjects(pd3dDevice, pd3dCommandList);
}

