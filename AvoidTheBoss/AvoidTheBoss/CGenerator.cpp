#include "pch.h"
#include "CGenerator.h"

CGenerator::CGenerator()
{
	radius = 0.5f;
	for (int i = 0; i < m_nPipe; i++)
	{
		m_nPipeStartIndexCount[i] = 0;
		m_nPipeStartAnimation[i] = false;
	}
	
}

CGenerator::~CGenerator()
{
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
	if (m_bSwitchAnimationOn)//m_bSwitchActive)
	{
		float delta = 1.0f;
		for (int i = 0; i < m_nPipe; i++) //1.8 ->1.7로 이동 10.f
		{
			if (m_nGeneratorAnimationCount == 0)
			{
				m_nPipeStartAnimation[0] = true;
			}
			if (m_nGeneratorAnimationCount == 4)
			{
				m_nPipeStartAnimation[1] = true;
			}
			if (m_nGeneratorAnimationCount == 8)
			{
				m_nPipeStartAnimation[2] = true;
			}

			if (m_nPipeStartAnimation[i])
			{
				if (m_nGeneratorAnimationCount > GENERATOR_ANIM_FRAM)
				{
					m_nGeneratorAnimationCount = 0;
				}
				else if (m_nGeneratorAnimationCount > (GENERATOR_ANIM_FRAM / 2.0f) && m_nGeneratorAnimationCount <= GENERATOR_ANIM_FRAM)
				{
					XMMATRIX xmmtxTranslate = DirectX::XMMatrixTranslation(0.0f, delta * fTimeElapsed, 0.0f);
					m_ppPipe[i]->m_xmf4x4ToParent = Matrix4x4::Multiply(xmmtxTranslate, m_ppPipe[i]->m_xmf4x4ToParent);
				}
				else if (m_nGeneratorAnimationCount <= (GENERATOR_ANIM_FRAM / 2.0f))
				{
					XMMATRIX xmmtxTranslate = DirectX::XMMatrixTranslation(0.0f, -delta * fTimeElapsed, 0.0f);
					m_ppPipe[i]->m_xmf4x4ToParent = Matrix4x4::Multiply(xmmtxTranslate, m_ppPipe[i]->m_xmf4x4ToParent);
				}
			}
		}
		m_nGeneratorAnimationCount -= delta * fTimeElapsed;
	}
	if(m_bSwitchAnimationOn)
	{	
		float ButtonDelta = 0.1f;
		if (m_pButton)
		{
			if(m_nButtonAnimationCount == 0)
			{ 
				//m_nButtonAnimationCount = BUTTON_ANIM_FRAME;
			}
			else if (m_nButtonAnimationCount >0)
			{
				XMMATRIX xmmtxTranslate = DirectX::XMMatrixTranslation(0.0f, -ButtonDelta * fTimeElapsed, 0.0f);
				m_pButton->m_xmf4x4ToParent = Matrix4x4::Multiply(xmmtxTranslate, m_pButton->m_xmf4x4ToParent);
				m_nButtonAnimationCount -= ButtonDelta * fTimeElapsed;
			}
		}
		//MoveStrafe(1.0f * fTimeElapsed);
		//MoveUp(1.0f * fTimeElapsed);
		//MoveForward(1.0f * fTimeElapsed);
	}
	CGameObject::Animate(fTimeElapsed);
}