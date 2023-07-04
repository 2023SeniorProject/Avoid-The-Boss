#pragma once

class CScene;
class SceneManager
{
public:
	int32 m_curScene;
	std::array<CScene*,4> m_scenes;
public:
	SceneManager() {}
	~SceneManager(){}
	void Render() {}
	void BuildScene() {}
	void ChangeScene(int32) {}

};

