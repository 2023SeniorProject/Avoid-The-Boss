#pragma once
class SceneInterface
{
public:
	SceneInterface() {}
	virtual ~SceneInterface() {}
	//씬에서 마우스와 키보드 메시지를 처리한다.
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

	virtual void ProcessInput(HWND hWnd) {}
};

