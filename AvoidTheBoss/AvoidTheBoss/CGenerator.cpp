#include "pch.h"
#include "clientIocpCore.h"
#include "CGenerator.h"

CGenerator::CGenerator()
{
	radius = 0.5f;
	for (int i = 0; i < m_nPipe; i++)
	{
		m_nPipeStartAnimation[i] = false;
	}
	
}

void CGenerator::Update(float fTimeElapsed)
{
	if (m_bOnInteraction && !m_bGenActive) m_curGuage += m_guageSpeed * fTimeElapsed;
	if (m_curGuage > m_maxGuage && !m_bGenActive)
	{
		m_bGenActive = true;
		SC_EVENTPACKET packet;
		packet.size = sizeof(SC_EVENTPACKET);
		packet.eventId = (uint8)EVENT_TYPE::SWITCH_ONE_ACTIVATE_EVENT + m_idx;
		clientCore._client->DoSend(&packet);
	}


	
}

void CGenerator::OnPrepareAnimate()
{
	m_ppPipe = new CGameObject * [m_nPipe];

	if (m_ppPipe)
	{
		m_ppPipe[0] = FindFrame("Generator_Pipe1");
		m_ppPipe[1] = FindFrame("Generator_Pipe2");
		m_ppPipe[2] = FindFrame("Generator_Pipe3");
	}
	m_pButton = FindFrame("Button001"); //Button -> 통짜 이름
}

void CGenerator::Animate(float fTimeElapsed)
{
	if (m_bOnInteraction || m_bAlreadyOn)
	{
		float delta = 0.3f;
		for (int i = 0; i < m_nPipe; i++) //1.8 ->1.7    ̵  10.f
		{
			if (m_nGeneratorAnimationCount == GENERATOR_ANIM_FRAM)	   m_nPipeStartAnimation[0] = true;
			if (m_nGeneratorAnimationCount <= GENERATOR_ANIM_FRAM - 4) m_nPipeStartAnimation[1] = true;
			if (m_nGeneratorAnimationCount <= GENERATOR_ANIM_FRAM - 8) m_nPipeStartAnimation[2] = true;
		
			if (m_nPipeStartAnimation[i])
			{
				if (m_nGeneratorAnimationCount < 0)
				{
					m_nGeneratorAnimationCount = GENERATOR_ANIM_FRAM;
				}
				else if (m_nGeneratorAnimationCount > 25 && m_nGeneratorAnimationCount <= GENERATOR_ANIM_FRAM)
				{
					XMMATRIX xmmtxTranslate = DirectX::XMMatrixTranslation(0.0f, -delta * fTimeElapsed, 0.0f);
					m_ppPipe[i]->m_xmf4x4ToParent = Matrix4x4::Multiply(xmmtxTranslate, m_ppPipe[i]->m_xmf4x4ToParent);
				}
				else if (m_nGeneratorAnimationCount <= 25 && m_nGeneratorAnimationCount > 0)
				{
					XMMATRIX xmmtxTranslate = DirectX::XMMatrixTranslation(0.0f, +delta * fTimeElapsed, 0.0f);
					m_ppPipe[i]->m_xmf4x4ToParent = Matrix4x4::Multiply(xmmtxTranslate, m_ppPipe[i]->m_xmf4x4ToParent);
				}
			}
		}
		m_nGeneratorAnimationCount -= delta * fTimeElapsed;
	}
	CGameObject::Animate(fTimeElapsed);
}