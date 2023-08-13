#pragma once
class Scene;
class SceneManager
{
public:
	enum class SCENESTATE { TITLE = 0, LOBBY = 1, ROOM = 2, INGAME = 3, RESULT = 4 };
protected:
	Scene* m_pScenes[5];
public:
	SceneManager()
	{
		for (int i = 0; i < 5; ++i) m_pScenes[i] = nullptr;
		BuildScene();
	}
	~SceneManager() { }

	virtual void OnUpdate();
	virtual void OnRender();
	virtual void OnKeyDown(UINT8 key);

	virtual void LoadPBRTScene();
	virtual void LoadSceneGeometry();
	virtual void InitializeGeometry();

	//void ReleaseUpBuffers();
	//void ReleaseScene();

	void ResetScene();

	void BuildScene();
	Scene* ChangeScene(int idx);
	Scene* GetSceneByIdx(int idx) { return m_pScenes[idx]; } // 현재 씬을 반환 받는다.

};

