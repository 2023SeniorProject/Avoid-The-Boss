#pragma once
class SceneInterface
{
public:
	SceneInterface() {}
	virtual ~SceneInterface() {}
	//씬에서 마우스와 키보드 메시지를 처리한다.
	virtual bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM
		lParam) 
	{
		return false;
	};
	virtual bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM
		lParam)
	{
		return false;
	};

	void ProcessInput(UCHAR* pKeysBuffer){}
};

