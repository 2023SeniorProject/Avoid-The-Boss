#pragma once
enum SceneInfo : int8 { LOGIN = 77,LOBBY = 78,ROOM = 79,GAMEROOM = 80};
class SceneInterface
{
	friend class ClientIocpCore;
public:
	Atomic<int8> _curSceneStatus = -1;
public:
	SceneInterface() {}
	virtual ~SceneInterface() {}
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

	virtual void Update(HWND hWnd) {}
};

