#include "stdafx.h"
#include "SceneManager.h"
#include "Scene.h"
#include "GameScene.h"

Scene* SceneManager::ChangeScene(int idx)
{
	return m_pScenes[idx];  
	// �ش� �ε����� �´� ������ �����ϰ� ������ ��ġ�� ���Ѵ�.
}

SceneManager::SceneManager()
{
	for (int i = 0; i < 5; ++i) m_pScenes[i] = nullptr;

	m_pScenes[(int)SCENESTATE::TITLE] = new CTitleScene();
	m_pScenes[(int)SCENESTATE::LOBBY] = new CLobbyScene();
	m_pScenes[(int)SCENESTATE::ROOM] = new CRoomScene();
	m_pScenes[(int)SCENESTATE::INGAME] = new GameScene();
	m_pScenes[(int)SCENESTATE::RESULT] = new CResultScene();
}

void SceneManager::OnUpdate()
{
	GetSceneByIdx(m_nCurSceneIdx)->OnUpdate();
}

void SceneManager::OnRender()
{
	GetSceneByIdx(m_nCurSceneIdx)->OnRender();
}

void SceneManager::OnKeyDown(UINT8 key)
{
	//if (hWnd != ::GetActiveWindow()) return;
	GetSceneByIdx(m_nCurSceneIdx)->OnKeyDown(key);
}

void SceneManager::LoadPBRTScene()
{
	GetSceneByIdx(m_nCurSceneIdx)->LoadPBRTScene();
}

void SceneManager::InitializeGeometry()
{
	GetSceneByIdx(m_nCurSceneIdx)->InitializeGeometry();
}

void SceneManager::ResetScene()
{
}