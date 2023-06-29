#pragma once
enum SceneInfo : int8 { LOGIN = 77,LOBBY = 78,ROOM = 79,GAMEROOM = 80};

class CCamera;

class CScene
{
	friend class ClientIocpCore;
public:
	Atomic<int8> _curSceneStatus = -1;
public:
	CScene() {}
	virtual ~CScene() {}
	//������ ���콺�� Ű���� �޽����� ó���Ѵ�.
	virtual void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM
		lParam) 
	{
		return;
	};
	virtual void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM
		lParam)
	{
		return;
	};

	virtual void ProcessInput(HWND& hWnd) {}
	virtual void Update(HWND hWnd) {}
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera) {}
};

