#pragma once

class CScene;
class SceneManager
{
public:
	enum class SCENESTATE { TITLE = 0, LOBBY = 1, ROOM = 2, INGAME = 3 };
protected:

	std::array<CScene*, 4> m_pScenes;
public:
	SceneManager() 
	{
		
	}
	~SceneManager(){}
	
	void Render(ID3D12GraphicsCommandList4* pd3dCommandList, int32, bool);
	void Update(HWND& hWnd, int32);
	void Animate();
	void ProcessInput(HWND& hWnd, int32);


	void BuildScene(ID3D12Device5* pd3dDevice, ID3D12GraphicsCommandList4* pd3dCommandList);
	void ReleaseUpBuffers();
	void ReleaseScene();

	
	void ResetScene();
	CScene* ChangeScene(int32 idx);
	CScene* GetSceneByIdx(int32 idx) { return m_pScenes[idx];   } // 현재 씬을 반환 받는다.
	CScene* GetSceneByIndex(int32 idx) { return m_pScenes[idx]; }
};

