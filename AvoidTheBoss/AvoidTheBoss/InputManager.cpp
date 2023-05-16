#include "pch.h"
#include "InputManager.h"

InputManager* InputManager::im = nullptr;
int16 InputManager::m_lastkeyInput = 0;
uint8 InputManager::keyBuffer[256] = {};
void InputManager::ProcessInput()
{
	// 기본적인 이동 방향에 관한 키 입력처리 0x8001 || 0x8000
	if (SetKeyStatus(KEY_TYPE::W)) m_lastkeyInput &= KEY_FORWARD;
	if (SetKeyStatus(KEY_TYPE::A)) m_lastkeyInput &= KEY_LEFT;
	if (SetKeyStatus(KEY_TYPE::S)) m_lastkeyInput &= KEY_BACKWARD;
	if (SetKeyStatus(KEY_TYPE::D)) m_lastkeyInput &= KEY_RIGHT;

	// 상호작용과 관련된 키 입력 처리 0x8000 처음 입력한 경우, 입력하다가 땐 경우 0x0001
	if (SetKeyStatus(KEY_TYPE::F))
	{
		m_lastkeyInput &= KEY_F;
	}
	else
	{
		m_lastkeyInput ^= KEY_F; // 이전에 누르다가 땠을 경우나 false 인 경우 xor연산으로 해당 부분을 지운다.
	}
	if (SetKeyStatus(KEY_TYPE::SPACE)) m_lastkeyInput &= KEY_SPACE;
}

bool InputManager::SetKeyStatus(const KEY_TYPE& key)
{
	if (GetAsyncKeyState((int32)key) == 0x8001) // 키를 처음 입력한 경우
	{
		keyBuffer[(int32)key] = (uint8)KEY_STATUS::KEY_PRESS;
		return true;
	}
	else if(GetAsyncKeyState((int32)key) == 0x8000) // 키를 이전부터 누르고 있었던 경우
	{
		if(GetKeyStatus(key) == (uint8)KEY_STATUS::KEY_PRESS) keyBuffer[(int32)key] = (uint8)KEY_STATUS::KEY_DOWN;
		return true;
	}
	else if (GetAsyncKeyState((int32)key) == 0x0001) // 키를 누르다가 땠을 경우
	{
		if (GetKeyStatus(key) == (uint8)KEY_STATUS::KEY_DOWN) keyBuffer[(int32)key] = (uint8)KEY_STATUS::KEY_UP;
		return false;
	}
	else if (GetAsyncKeyState((int32)key) == 0x0000) // 아예 안누른 경우
	{
		if (GetKeyStatus(key) == (uint8)KEY_STATUS::KEY_UP) keyBuffer[(int32)key] = (uint8)KEY_STATUS::KEY_DOWN;
		return false;
	}
}
