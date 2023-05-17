#pragma once

enum class KEY_TYPE
{
	UP = VK_UP,
	DOWN = VK_DOWN,
	LEFT = VK_LEFT,
	RIGHT = VK_RIGHT,
	SPACE = VK_SPACE,
	MLBUTTON = VK_LBUTTON,
	W = 0x57,
	A = 0x41,
	S = 0x53,
	D = 0x44,
	F = 0x45
};

enum class KEY_STATUS :	uint8
{
	KEY_PRESS = 0, // ó�� ���� ��
	KEY_DOWN = 1,  // �̹� �����ִ� ��
	KEY_UP, // Ű�� ������ ���� ���
	KEY_NONE, // ���ʿ� ���� ���� ���� ���
};

// Ű �Է� ó���ϱ� ���� �� �̱��� �������� �����Ѵ�.
class InputManager
{
private:
	static InputManager* im;
	static float m_deltaX, m_deltaY;
	static float m_cursorX, m_cursorY;
	static int16 m_lastkeyInput; // ���������� �Էµ� Ű�Է��� ��Ÿ����.
	static uint8  keyBuffer[256];
private:
	InputManager() { m_lastkeyInput = 0; }
	InputManager(const InputManager& ref) {}
	InputManager& operator=(const InputManager& ref) {}
	~InputManager() {}
	static bool SetKeyStatus(const KEY_TYPE& key);
public:	
static InputManager& GetInstance() 
{
	if (im == nullptr) im = new InputManager();
	return *im;
}
public:
	static void ProcessInput();
	static void ProcessMouseInput() {}
	static int16 GetLastInput() { return m_lastkeyInput; }
	static uint8 GetKeyStatus(const KEY_TYPE& key) { return keyBuffer[(int32)key]; }
};

