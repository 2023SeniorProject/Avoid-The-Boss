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
	E = 0x45,
	F = 0x46,
	G = 0x47,

	NUM0 = 0x30,
	NUM1 = 0x31,
	NUM2 = 0x32,
	NUM3 = 0x33,
	NUM4 = 0x34,
	NUM5 = 0x35,
	NUM6 = 0x36,
	NUM7 = 0x37,
	NUM8 = 0x38,
	NUM9 = 0x39,
};

enum class KEY_STATUS : int8
{
	KEY_NONE = -1, // 애초에 누른 적이 없는 경우
	KEY_UP = 0, // 키를 눌렀다 땠을 경우
	KEY_PRESS = 1, // 처음 누른 것
	KEY_DOWN = 2,  // 이미 눌려있는 것
	
	
};

// 키 입력 처리하기 위한 것 싱글톤 패턴으로 생성한다.
class InputManager
{
private:
	static InputManager* instance;
	static int8 m_keyBuffer[256];
private:
	InputManager() 
	{ 
		for (int i = 0; i < 256; ++i) m_keyBuffer[i] = (int8)KEY_STATUS::KEY_NONE;
	}
	InputManager(const InputManager& ref) {}
	InputManager& operator=(const InputManager& ref) {}
	~InputManager() {}
	static void Update(const int32 key);
	static void SetKeyPress(const int32 key);
	static void SetKeyUp(const int32 key);
public:	
static InputManager& GetInstance() 
{
	if (instance == nullptr) instance = new InputManager();
	return *instance;
}
public:

	static void InputStatusUpdate();
	static void MouseInputStatusUpdate();
	static int8 GetKeyBuffer(const KEY_TYPE key) { return m_keyBuffer[(int32)key]; }
	static int8 GetKeyBuffer(int32 key) { return m_keyBuffer[(int32)key]; }
};

