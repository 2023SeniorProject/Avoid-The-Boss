#include "stdafx.h"
#include "SceneManager.h"
#include "Scene.h"
#include "GameScene.h"
#include "OtherScene.h"

Scene* SceneManager::ChangeScene(int idx)
{
	return m_pScenes[idx];  
	// 해당 인덱스에 맞는 씬으로 변경하고 적절한 조치를 취한다.
}

void SceneManager::OnUpdate()
{
	GetSceneByIdx(0)->OnUpdate();
}

void SceneManager::OnRender()
{
	GetSceneByIdx(0)->OnRender();
}

void SceneManager::OnKeyDown(UINT8 key)
{
	//if (hWnd != ::GetActiveWindow()) return;
	GetSceneByIdx(0)->OnKeyDown(key);
}

void SceneManager::LoadPBRTScene()
{
}

void SceneManager::LoadSceneGeometry()
{
}

void SceneManager::InitializeGeometry()
{
}

void SceneManager::ResetScene()
{
}

//void SceneManager::ReleaseUpBuffers()
//{
//	for (int i = 0; i < 4; ++i)  m_pScenes[i]-//>ReleaseUploadBuffers();
//}
//void SceneManager::ReleaseScene()
//{
//	m_pScenes[3]->ReleaseObjects();
//}
//void SceneManager::Animate(int32 idx)
//{
//	m_pScenes[idx]->AnimateObjects();
//}

void SceneManager::BuildScene()
{
	m_pScenes[(int)SCENESTATE::TITLE] = new CTitleScene();
	//m_pScenes[(int)SCENESTATE::TITLE]->InitializeGeometry();
	m_pScenes[(int)SCENESTATE::LOBBY] = new CLobbyScene();
	//m_pScenes[(int)SCENESTATE::LOBBY]->InitializeGeometry();
	m_pScenes[(int)SCENESTATE::ROOM] = new CRoomScene();
	//m_pScenes[(int)SCENESTATE::ROOM]->InitializeGeometry();
	m_pScenes[(int)SCENESTATE::INGAME] = new GameScene();
	//m_pScenes[(int)SCENESTATE::INGAME]->InitializeGeometry();

	m_pScenes[(int)SCENESTATE::RESULT] = new CResultScene();
	//m_pScenes[(int)SCENESTATE::RESULT]->InitializeGeometry();
}