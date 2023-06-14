#include "pch.h"
#include "PlayerInfo.h"
#include "CollisionDetector.h"
#include "CSIocpCore.h"

SPlayer::SPlayer() 
{
	
	m_xmf3Right = XMFLOAT3(1.0f, 0.0f, 0.0f);
	m_xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_xmf3Look = XMFLOAT3(0.0f, 0.0f, 1.0f);

	m_xmf3Position = XMFLOAT3(0.0f, 0.25f, -20.0f);
	m_xmf3Velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);

	m_fPitch = 0.0f;
	m_fRoll = 0.0f;
	m_fYaw = 0.0f;

	m_playerBV.Center = m_xmf3Position;
	m_playerBV.Radius = 0.2f;
	
}

SPlayer::~SPlayer()
{
}

void SPlayer::SetSpeed(const XMFLOAT3& xmf3Shift)
{
	m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, xmf3Shift);
}

void SPlayer::SetDirection(const XMFLOAT3& lookVec)
{
	m_xmf3Look = lookVec;
	m_xmf3Right = Vector3::CrossProduct(m_xmf3Up, m_xmf3Look, true);
}

void SPlayer::ProcessInput(const int16& input,const XMFLOAT3& lookVec)
{
	SetDirection(lookVec);
	if (m_idx == 0) Move(input, BOSS_VELOCITY);
	else Move(input, EMPLOYEE_VELOCITY);
}

void SPlayer::Move(const int16& dwDirection, float fDistance)
{
	XMFLOAT3 xmf3Shift = XMFLOAT3(0, 0, 0);
	if (GetBehavior() != (int32)PLAYER_BEHAVIOR::IDLE)
	{
		SetVelocity(xmf3Shift);
		return;
	}
	
	if (dwDirection)
	{
		if (dwDirection & KEY_FORWARD)  xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Look,fDistance);
		if (dwDirection & KEY_BACKWARD) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Look, -fDistance);
		if (dwDirection & KEY_RIGHT)    xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Right,fDistance);
		if (dwDirection & KEY_LEFT)     xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Right,-fDistance);
		//플레이어를 현재 위치 벡터에서 xmf3Shift 벡터만큼 이동한다. 
		m_xmf3Velocity = XMFLOAT3(0, 0, 0);
		SetVelocity(xmf3Shift);
	}
	else SetVelocity(xmf3Shift);
	
}

void SPlayer::Update(float fTimeElapsed)
{
	//플레이어를 속도 벡터 만큼 실제로 이동한다(카메라도 이동될 것이다). 

	XMFLOAT3 xmf3Velocity = Vector3::ScalarProduct(m_xmf3Velocity, fTimeElapsed, false);
	m_xmf3Position = Vector3::Add(m_xmf3Position, xmf3Velocity);
	LateUpdate(fTimeElapsed);
}

void SPlayer::LateUpdate(float fTimeElapsed)
{
	m_playerBV.Center = GetPosition();
	BoxTree->CheckCollision(m_playerBV, m_xmf3Position);
}









