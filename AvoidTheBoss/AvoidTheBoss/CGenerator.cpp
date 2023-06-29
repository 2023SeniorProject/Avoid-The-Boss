#include "pch.h"
#include "clientIocpCore.h"
#include "CGenerator.h"

CGenerator::CGenerator()
{
	radius = 0.5f;
	for (int i = 0; i < m_nPipe; i++)
	{
		m_nPipeStartAnimation[i] = false;
		m_nGeneratorAnimationCount[i] = 0;
	}
	
}

void CGenerator::LogicUpdate()
{
	//if (m_bOnInteraction || m_bAlreadyOn)
	{
			//if (m_nGeneratorAnimationCount == 1)	   
				m_nPipeStartAnimation[0] = true;
			//if (m_nGeneratorAnimationCount == 1) 
				m_nPipeStartAnimation[1] = true;
			//if (m_nGeneratorAnimationCount == 1) 
				m_nPipeStartAnimation[2] = true;
	}
	//else
	//{
	//	for (int i = 0; i < m_nPipe; i++)
	//	{
	//		m_nPipeStartAnimation[i] = false;
	//	}
	// 
	//}
	for (int i = 0; i < m_nPipe; i++)
	{
		m_nGeneratorAnimationCount[i] += 1;

	}
}

void CGenerator::Update(float fTimeElapsed)
{
	if (m_bOnInteraction && !m_bGenActive) m_curGuage += m_guageSpeed * fTimeElapsed;

	if (m_curGuage > m_maxGuage && !m_bGenActive)
	{
		m_bGenActive = true;
		SC_EVENTPACKET packet;
		packet.type = (uint8)SC_PACKET_TYPE::GAMEEVENT;
		packet.size = sizeof(SC_EVENTPACKET);
		packet.eventId = (uint8)EVENT_TYPE::SWITCH_ONE_ACTIVATE_EVENT + m_idx;
		clientCore.DoSend(&packet);
		std::cout << "Gen Active\n";
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
	LogicUpdate();

	float delta = 0.01f;
	//if (m_bOnInteraction)
	{
		for (int i = 0; i < m_nPipe; i++) //1.8 ->1.7    ̵  10.f
		{
			if (m_nPipeStartAnimation[i])
			{
				if (	m_nGeneratorAnimationCount[i] > 8
					&& 	m_nGeneratorAnimationCount[i] <= GENERATOR_ANIM_FRAM)
				{
					if (	m_nGeneratorAnimationCount[i] == GENERATOR_ANIM_FRAM)
					{
							m_nGeneratorAnimationCount[i] = 0;
						std::cout << m_ppPipe[i]->GetPosition().y << std::endl;
					}
					XMMATRIX xmmtxTranslate = DirectX::XMMatrixTranslation(0.0f, delta, 0.0f);
					m_ppPipe[i]->m_xmf4x4ToParent = Matrix4x4::Multiply(xmmtxTranslate, m_ppPipe[i]->m_xmf4x4ToParent);
				}
				else if (	m_nGeneratorAnimationCount[i] <= 8 
					&& 	m_nGeneratorAnimationCount[i] > 0)
				{
					XMMATRIX xmmtxTranslate = DirectX::XMMatrixTranslation(0.0f, -delta, 0.0f);
					m_ppPipe[i]->m_xmf4x4ToParent = Matrix4x4::Multiply(xmmtxTranslate, m_ppPipe[i]->m_xmf4x4ToParent);

					if(	m_nGeneratorAnimationCount[i]==1)
						std::cout << m_ppPipe[i]->GetPosition().y << std::endl;
				}
			}
		}

	}


	CGameObject::Animate(fTimeElapsed);
}