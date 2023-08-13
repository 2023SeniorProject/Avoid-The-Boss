#pragma once
class Scene;
class SceneManager
{
public:
	enum class SCENESTATE { TITLE = 0, LOBBY = 1, ROOM = 2, INGAME = 3, RESULT = 4 };
protected:

	CScene* m_pScenes[5];
public:
	SceneManager()
	{
		for (int i = 0; i < 5; ++i) m_pScenes[i] = nullptr;
	}
	~SceneManager() { }

	void Render(ID3D12GraphicsCommandList4* pd3dCommandList, int32, bool);
	void Update(HWND& hWnd, int32);
	void Animate(int32);
	void ProcessInput(HWND& hWnd, int32);


	void BuildScene(ID3D12Device5* pd3dDevice, ID3D12GraphicsCommandList4* pd3dCommandList);
	void ReleaseUpBuffers();
	void ReleaseScene();


	void ResetScene();
	CScene* ChangeScene(int32 idx);
	CScene* GetSceneByIdx(int32 idx) { return m_pScenes[idx]; } // 현재 씬을 반환 받는다.
};

