#pragma once
class SceneInterface
{
public:
	SceneInterface() {}
	virtual ~SceneInterface() {}
	//������ ���콺�� Ű���� �޽����� ó���Ѵ�.
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

