#include "pch.h"
#include "CGameScene.h"
#include "Scene.h"
#include "SceneManager.h"
#include "OtherScenes.h"

void SceneManager::Render(ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_pScenes[m_curScene]->Render(pd3dCommandList, m_pScenes[m_curScene]->m_pCamera);
}

void SceneManager::Update(HWND& hWnd)
{
	m_pScenes[m_curScene]->Update(hWnd);
}

void SceneManager::ProcessInput(HWND& hWnd)
{
	m_pScenes[m_curScene]->ProcessInput(hWnd);
}


CScene* SceneManager::ChangeScene(int32 idx)
{
	m_curScene = idx; 
	return m_pScenes[idx];  // 해당 인덱스에 맞는 씬으로 변경하고 적절한 조치를 취한다.
}

void SceneManager::ResetScene()
{
}

void SceneManager::ReleaseUpBuffers()
{
	for (auto& i : m_pScenes) i->ReleaseUploadBuffers();
}
void SceneManager::ReleaseScene()
{
	for (auto& i : m_pScenes) i->ReleaseObjects();
}

void SceneManager::Animate()
{
	m_pScenes[m_curScene]->AnimateObjects();
}

void SceneManager::BuildScene(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_pScenes[(int32)SCENESTATE::TITLE] = new CTitleScene();
	m_pScenes[(int32)SCENESTATE::TITLE]->BuildObjects(pd3dDevice, pd3dCommandList);
	m_pScenes[(int32)SCENESTATE::LOBBY] = new CLobbyScene();
	m_pScenes[(int32)SCENESTATE::LOBBY]->BuildObjects(pd3dDevice, pd3dCommandList);
	m_pScenes[(int32)SCENESTATE::ROOM] = new CRoomScene();
	m_pScenes[(int32)SCENESTATE::ROOM]->BuildObjects(pd3dDevice, pd3dCommandList);
	m_pScenes[(int32)SCENESTATE::INGAME] = new CGameScene();
	m_pScenes[(int32)SCENESTATE::INGAME]->BuildObjects(pd3dDevice, pd3dCommandList);

	ReleaseUpBuffers();
}
