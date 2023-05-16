#include "pch.h"
#include "InputManager.h"

InputManager* InputManager::im = nullptr;
int16 InputManager::m_lastkeyInput = 0;
uint8 InputManager::keyBuffer[256] = {};
void InputManager::ProcessInput()
{
	// �⺻���� �̵� ���⿡ ���� Ű �Է�ó�� 0x8001 || 0x8000
	if (SetKeyStatus(KEY_TYPE::W)) m_lastkeyInput &= KEY_FORWARD;
	if (SetKeyStatus(KEY_TYPE::A)) m_lastkeyInput &= KEY_LEFT;
	if (SetKeyStatus(KEY_TYPE::S)) m_lastkeyInput &= KEY_BACKWARD;
	if (SetKeyStatus(KEY_TYPE::D)) m_lastkeyInput &= KEY_RIGHT;

	// ��ȣ�ۿ�� ���õ� Ű �Է� ó�� 0x8000 ó�� �Է��� ���, �Է��ϴٰ� �� ��� 0x0001
	if (SetKeyStatus(KEY_TYPE::F))
	{
		m_lastkeyInput &= KEY_F;
	}
	else
	{
		m_lastkeyInput ^= KEY_F; // ������ �����ٰ� ���� ��쳪 false �� ��� xor�������� �ش� �κ��� �����.
	}
	if (SetKeyStatus(KEY_TYPE::SPACE)) m_lastkeyInput &= KEY_SPACE;
}

bool InputManager::SetKeyStatus(const KEY_TYPE& key)
{
	if (GetAsyncKeyState((int32)key) == 0x8001) // Ű�� ó�� �Է��� ���
	{
		keyBuffer[(int32)key] = (uint8)KEY_STATUS::KEY_PRESS;
		return true;
	}
	else if(GetAsyncKeyState((int32)key) == 0x8000) // Ű�� �������� ������ �־��� ���
	{
		if(GetKeyStatus(key) == (uint8)KEY_STATUS::KEY_PRESS) keyBuffer[(int32)key] = (uint8)KEY_STATUS::KEY_DOWN;
		return true;
	}
	else if (GetAsyncKeyState((int32)key) == 0x0001) // Ű�� �����ٰ� ���� ���
	{
		if (GetKeyStatus(key) == (uint8)KEY_STATUS::KEY_DOWN) keyBuffer[(int32)key] = (uint8)KEY_STATUS::KEY_UP;
		return false;
	}
	else if (GetAsyncKeyState((int32)key) == 0x0000) // �ƿ� �ȴ��� ���
	{
		if (GetKeyStatus(key) == (uint8)KEY_STATUS::KEY_UP) keyBuffer[(int32)key] = (uint8)KEY_STATUS::KEY_DOWN;
		return false;
	}
}
