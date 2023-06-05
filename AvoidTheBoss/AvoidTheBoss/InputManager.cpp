#include "pch.h"
#include "InputManager.h"

InputManager* InputManager::instance = nullptr;

int8 InputManager::m_keyBuffer[256] = {-1,};

void InputManager::InputStatusUpdate()
{
	// 기본적인 이동 방향에 관한 키 입력처리 0x8001 || 0x8000	
	Update(KEY_TYPE::W);
	Update(KEY_TYPE::A);
	Update(KEY_TYPE::S);
	Update(KEY_TYPE::D);

	//// 상호작용과 관련된 키 입력 처리 0x8000 처음 입력한 경우, 입력하다가 땐 경우 0x0001
	Update(KEY_TYPE::F);
	Update(KEY_TYPE::SPACE); 

} 

void InputManager::MouseInputStatusUpdate()
{
	if (::GetCapture())	Update(KEY_TYPE::MLBUTTON);
}

void InputManager::SetKeyPress(const KEY_TYPE key)
{
	if (m_keyBuffer[(int)key] <= 0 )
	{
		
		m_keyBuffer[(int)key] = (int8)KEY_STATUS::KEY_PRESS;
	}
	else if(m_keyBuffer[(int)key] == (int8)KEY_STATUS::KEY_PRESS)
	{
		
		m_keyBuffer[(int)key] = (int8)KEY_STATUS::KEY_DOWN;
	}
}

void InputManager::SetKeyUp(const KEY_TYPE key)
{
	if (m_keyBuffer[(int)key] > 0)
	{
		m_keyBuffer[(int)key] = (int8)KEY_STATUS::KEY_UP;
	}
	else if(m_keyBuffer[(int)key] == (int8)KEY_STATUS::KEY_UP)
	{
		m_keyBuffer[(int)key] = (int8)KEY_STATUS::KEY_NONE;
	}
}

void InputManager::Update(const KEY_TYPE key)
{
	
	if (::GetAsyncKeyState((int32)key) & 0x8000) // 키를 이전부터 누르고 있었던 경우
	{
		SetKeyPress(key);
	}	
	else 
	{
		SetKeyUp(key);
	}

}
