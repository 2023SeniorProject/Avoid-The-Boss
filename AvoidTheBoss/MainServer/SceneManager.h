#pragma once

// 씬들을 정의
enum class SCENE_INFO { LOBBY, ROOM, INGAME};

class Scene
{
private:
	SCENE_INFO m_curScene = SCENE_INFO::LOBBY;
public:
	virtual void Reset() {}
	virtual void Update() {}
};


class SceneManager
{
private:
	// 큐들에 대한 씬
	std::queue<Scene*> m_scenes;
public:
	SceneManager() {}
	~SceneManager() {}

	// 씬 변경 함수
	void ChangeToPrevScene() {} // 이전의 씬으로 돌아간다.
	void ChangeToNextScene() {} // 다음의 씬으로 돌아간다.
	void UpdateScene() { m_scenes.front()->Update();}
	
};


