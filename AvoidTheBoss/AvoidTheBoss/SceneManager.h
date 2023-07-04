#pragma once

class CScene;
class SceneManager
{
public:
	enum class SCENESTATE { TITLE = 0, LOBBY = 1, ROOM = 2, INGAME = 3 };
protected:
	int32 m_curScene = 0;
	std::array<CScene*, 4> m_pScenes;
public:
	SceneManager() 
	{
		m_curScene = 0;
		for (auto& i : m_pScenes) i = nullptr; 
	}
	~SceneManager(){}
	
	void Render(ID3D12GraphicsCommandList* pd3dCommandList);
	void Update(HWND& hWnd);
	void Animate();
	void ProcessInput(HWND& hWnd);


	void BuildScene(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void ReleaseUpBuffers();
	void ReleaseScene();

	
	void ResetScene();
	CScene* ChangeScene(int32 idx);
	CScene* GetCurScene() { return m_pScenes[m_curScene]; } // 현재 씬을 반환 받는다.
	CScene* GetSceneByIndex(int32 idx) { return m_pScenes[idx]; }
	int32 GetCurSceneState() { return m_curScene; }

};

