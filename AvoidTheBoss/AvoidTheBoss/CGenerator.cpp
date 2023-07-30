#include "pch.h"
#include "clientIocpCore.h"
#include "CGenerator.h"
#include "SceneManager.h"
#include "GameScene.h"
#include "CEmployee.h"
#include "SoundManager.h"

CGenerator::CGenerator()
{
	radius = 0.5f;
	for (int i = 0; i < m_nPipe; i++)
	{
		m_nPipeStartAnimation[i] = false;
		m_nGenerPipeAnimationCount[i] = 0;
	}
	m_bAlreadyOn = false;
}

void CGenerator::SetNormalVector()
{
	xmf4NormalVector = XMFLOAT4(m_pBody->GetParentUp().x, m_pBody->GetParentUp().y, m_pBody->GetParentUp().z, 0.0f);
}

void CGenerator::LogicUpdate()
{
	if (m_bOnInteraction || m_bAlreadyOn)
	{  
		m_nPipeStartAnimation[0] = true;	
		m_nGenerBodyAnimationCount++;
	}
	else if(!m_bOnInteraction && !m_bAlreadyOn)
	{
		for (int i = 0; i < m_nPipe; i++)
		{
			m_nPipeStartAnimation[i] = false;
		}	
	}

	if (m_nGenerPipeAnimationCount[0] == 4)
		m_nPipeStartAnimation[1] = true;
	if (m_nGenerPipeAnimationCount[1] == 4)
		m_nPipeStartAnimation[2] = true; 

	for (int i = 0; i < m_nPipe; i++)
	{
		if(m_nPipeStartAnimation[i])
			m_nGenerPipeAnimationCount[i] += 1;
	}
}

void CGenerator::Update(float fTimeElapsed)
{
	if (m_bOnInteraction && !m_bGenActive)
	{
		m_curGuage += m_guageSpeed * fTimeElapsed;
	}
	if (m_curGuage > m_maxGuage && !m_bGenActive)
	{
		m_bGenActive = true;
		m_bOnInteraction = false;
		m_bAlreadyOn = true;

		SC_EVENTPACKET packet;
		packet.type = (uint8)SC_GAME_PACKET_TYPE::GAMEEVENT;
		packet.size = sizeof(SC_EVENTPACKET);
		packet.eventId = (uint8)EVENT_TYPE::SWITCH_ONE_ACTIVATE_EVENT + m_idx;
		clientCore.DoSend(&packet);
		std::cout << m_idx << ") Gen Active\n";
		CGameScene* gs = static_cast<CGameScene*>(mainGame.m_SceneManager->GetSceneByIdx(3));
		static_cast<CEmployee*>(gs->GetScenePlayerByIdx(gs->m_playerIdx))->m_activeCnt += 1;
	}
	if (m_bOnInteraction || m_bAlreadyOn)
	{
		if (!GetbIsStartGenInter())
		{
			SoundManager::GetInstance().PlayObjectSound(17, 6);
			//SoundManager::GetInstance().PlayObjectSound(6, 6);
			SetbIsStartGenInter(true);
			std::cout << "startGenInteractionTrue : " << GetbIsStartGenInter() << std::endl;
		}
	}
	else if((!m_bOnInteraction && !m_bAlreadyOn) || m_bGenActive)
	{
		if (GetbIsStartGenInter())
		{
			SoundManager::GetInstance().SoundStop(6);
			SetbIsStartGenInter(false);
			std::cout << "startGenInteractionFalse : " << GetbIsStartGenInter() << std::endl;
		}
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
	m_pBody = FindFrame("Generator"); 
}

void CGenerator::BodyAnimate(float fTimeElapsed)
{
	if (m_bOnInteraction || m_bAlreadyOn)
	{
		XMMATRIX xmmtxTranslate;
		float move = 0.001f;
		int changeFrame = 8;
		if (m_nGenerBodyAnimationCount > changeFrame && m_nGenerBodyAnimationCount <= GENERATOR_BODY_ANIM_FRAM)
		{
			if (m_nGenerBodyAnimationCount <= GENERATOR_BODY_ANIM_FRAM && m_nGenerBodyAnimationCount > changeFrame + changeFrame / 2)
				xmmtxTranslate = DirectX::XMMatrixTranslation(-move, -move, 0.0f);
			else if (m_nGenerBodyAnimationCount <= changeFrame + changeFrame / 2 && m_nGenerBodyAnimationCount > changeFrame / 2)
				xmmtxTranslate = DirectX::XMMatrixTranslation(move, move, 0.0f);

			if (m_nGenerBodyAnimationCount == GENERATOR_BODY_ANIM_FRAM)
			{
				m_nGenerBodyAnimationCount = 0;
			}
		}
		else if (m_nGenerBodyAnimationCount <= changeFrame && m_nGenerBodyAnimationCount > 0)
		{
			if(m_nGenerBodyAnimationCount > 0 && m_nGenerBodyAnimationCount<= changeFrame /2)
				xmmtxTranslate = DirectX::XMMatrixTranslation(0.0f, move, move);
			else if (m_nGenerBodyAnimationCount <= changeFrame && m_nGenerBodyAnimationCount > changeFrame / 2)
				xmmtxTranslate = DirectX::XMMatrixTranslation(0.0f, -move, -move);
		}
		m_pBody->m_xmf4x4ToParent = Matrix4x4::Multiply(xmmtxTranslate, m_pBody->m_xmf4x4ToParent);
	}
}

void CGenerator::PipelineAnimate(float fTimeElapsed)
{
	if (m_bOnInteraction || m_bAlreadyOn)
	{
		float delta = 0.01f;
		for (int i = 0; i < m_nPipe; i++) //1.8 ->1.7    ̵  10.f
		{
			if (m_nPipeStartAnimation[i])
			{
				if (m_nGenerPipeAnimationCount[i] > 8 && m_nGenerPipeAnimationCount[i] <= GENERATOR_PIPE_ANIM_FRAM)
				{
					if (m_nGenerPipeAnimationCount[i] == GENERATOR_PIPE_ANIM_FRAM)
					{
						m_nGenerPipeAnimationCount[i] = -20;
					}

					XMMATRIX xmmtxTranslate = DirectX::XMMatrixTranslation(0.0f, delta, 0.0f);
					m_ppPipe[i]->m_xmf4x4ToParent = Matrix4x4::Multiply(xmmtxTranslate, m_ppPipe[i]->m_xmf4x4ToParent);
				}
				else if (m_nGenerPipeAnimationCount[i] <= 8 && m_nGenerPipeAnimationCount[i] > 0)
				{
					XMMATRIX xmmtxTranslate = DirectX::XMMatrixTranslation(0.0f, -delta, 0.0f);
					m_ppPipe[i]->m_xmf4x4ToParent = Matrix4x4::Multiply(xmmtxTranslate, m_ppPipe[i]->m_xmf4x4ToParent);
				}
			}
		}
	}
}

void CGenerator::Animate(float fTimeElapsed)
{
	LogicUpdate();
	PipelineAnimate(fTimeElapsed);
	BodyAnimate(fTimeElapsed);

	CGameObject::Animate(fTimeElapsed);
}