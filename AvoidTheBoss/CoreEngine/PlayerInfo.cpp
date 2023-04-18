#include "pch.h"
#include "PlayerInfo.h"

PlayerInfo::PlayerInfo() 
{
	
	m_xmf3Right = XMFLOAT3(1.0f, 0.0f, 0.0f);
	m_xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_xmf3Look = XMFLOAT3(0.0f, 0.0f, 1.0f);

	m_xmf3Position = XMFLOAT3(0.0f, 0.0f, -50.0f);
	m_xmf3Velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);

	m_fPitch = 0.0f;
	m_fRoll = 0.0f;
	m_fYaw = 0.0f;
	
}

PlayerInfo::~PlayerInfo()
{
}

void PlayerInfo::SetSpeed(const XMFLOAT3& xmf3Shift)
{
	m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, xmf3Shift);
}

void PlayerInfo::SetDirection(const XMFLOAT3 look)
{
	m_xmf3Look = look;
	m_xmf3Right = Vector3::CrossProduct(m_xmf3Up, m_xmf3Look, true);
	//PrintRightVec();
}

void PlayerInfo::Move(uint8 dwDirection, float fDistance)
{
	XMFLOAT3 xmf3Shift = XMFLOAT3(0, 0, 0);
	if (dwDirection)
	{	
		if (dwDirection & DIR_FORWARD)  xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Look,fDistance);
		if (dwDirection & DIR_BACKWARD) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Look, -fDistance);
		if (dwDirection & DIR_RIGHT)    xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Right,fDistance);
		if (dwDirection & DIR_LEFT)     xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Right,-fDistance);
		//플레이어를 현재 위치 벡터에서 xmf3Shift 벡터만큼 이동한다. 
		m_xmf3Velocity = XMFLOAT3(0, 0, 0);
		SetVelocity(xmf3Shift);
	}
	else SetVelocity(xmf3Shift);
	
}

void PlayerInfo::Rotate(float x, float y, float z)
{
	if (x != 0.0f)
	{
		m_fPitch += x;
		if (m_fPitch > +89.0f) { x -= (m_fPitch - 89.0f); m_fPitch = +89.0f; }
		if (m_fPitch < -89.0f) { x -= (m_fPitch + 89.0f); m_fPitch = -89.0f; }
	}
	if (y != 0.0f)
	{

		m_fYaw += y;
		if (m_fYaw > 360.0f) m_fYaw -= 360.0f;
		if (m_fYaw < 0.0f) m_fYaw += 360.0f;
	}
	if (z != 0.0f)
	{

		m_fRoll += z;
		if (m_fRoll > +20.0f) { z -= (m_fRoll - 20.0f); m_fRoll = +20.0f; }
		if (m_fRoll < -20.0f) { z -= (m_fRoll + 20.0f); m_fRoll = -20.0f; }
	}


	if (y != 0.0f)
	{
		XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Up),
			XMConvertToRadians(y));
		m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
		m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);
	}

	m_xmf3Look = Vector3::Normalize(m_xmf3Look);
	m_xmf3Right = Vector3::CrossProduct(m_xmf3Up, m_xmf3Look, true);
	m_xmf3Up = Vector3::CrossProduct(m_xmf3Look, m_xmf3Right, true);
}

void PlayerInfo::UpdateMove(const XMFLOAT3& xmf3Shift)
{
	//플레이어를 현재 위치 벡터에서 xmf3Shift 벡터만큼 이동한다. 
	m_xmf3Position = Vector3::Add(m_xmf3Position, xmf3Shift);
}

void PlayerInfo::Update(float fTimeElapsed)
{
	//플레이어를 속도 벡터 만큼 실제로 이동한다(카메라도 이동될 것이다). 
	
	XMFLOAT3 xmf3Velocity = Vector3::ScalarProduct(m_xmf3Velocity, fTimeElapsed, false);
	UpdateMove(xmf3Velocity);
	PrintPos();
	OnPlayerUpdateCallback(fTimeElapsed);
}









