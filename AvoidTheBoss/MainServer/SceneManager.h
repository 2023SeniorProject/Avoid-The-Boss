#pragma once

// ������ ����
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
	// ť�鿡 ���� ��
	std::queue<Scene*> m_scenes;
public:
	SceneManager() {}
	~SceneManager() {}

	// �� ���� �Լ�
	void ChangeToPrevScene() {} // ������ ������ ���ư���.
	void ChangeToNextScene() {} // ������ ������ ���ư���.
	void UpdateScene() { m_scenes.front()->Update();}
	
};


