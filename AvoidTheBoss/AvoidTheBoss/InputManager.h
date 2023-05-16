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
	KEY_PRESS = 0, // 처음 누른 것
	KEY_DOWN = 1,  // 이미 눌려있는 것
	KEY_UP, // 키를 눌렀다 땠을 경우
	KEY_NONE, // 애초에 누른 적이 없는 경우
};

// 키 입력 처리하기 위한 것 싱글톤 패턴으로 생성한다.
class InputManager
{
private:
	static InputManager* im;
	static float m_deltaX, m_deltaY;
	static float m_cursorX, m_cursorY;
	static int16 m_lastkeyInput; // 최종적으로 입력된 키입력을 나타낸다.
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

