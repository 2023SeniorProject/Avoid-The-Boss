#pragma once
class Scene;
class SceneManager
{
public:
	enum class SCENESTATE { TITLE = 0, LOBBY = 1, ROOM = 2, INGAME = 3, RESULT = 4 };
protected:
	Scene* m_pScenes[5];
	int m_nCurSceneIdx = DEFAULT_SCENE_INDEX;
public:
	SceneManager();
	~SceneManager() { }

	virtual void OnUpdate();
	virtual void OnRender();
	virtual void OnKeyDown(UINT8 key);

	virtual void LoadPBRTScene();
	virtual void InitializeGeometry();

	void ResetScene();

	Scene* ChangeScene(int idx);
	Scene* GetSceneByIdx(int idx) { return m_pScenes[idx]; } // 현재 씬을 반환 받는다.

	int GetCurSceneIdx() {
		return m_nCurSceneIdx
			;
	}
	void SetCurSceneIdx( int idx) {
	 m_nCurSceneIdx = idx
			;
	}
};

