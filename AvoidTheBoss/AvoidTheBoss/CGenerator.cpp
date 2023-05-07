#include "pch.h"
#include "CGenerator.h"

CGenerator::CGenerator()
{
	radius = 0.5f;
	for (int i = 0; i < m_nPipe; i++)
	{
		m_nPipeStartAnimation[i] = false;
	}
}

void CGenerator::Animate(float fTimeElapsed)
{
	if (m_bSwitchAnimationOn)
	{
		float delta = 0.3f;
		for (int i = 0; i < m_nPipe; i++) //1.8 ->1.7    ̵  10.f
		{
			if (m_nGeneratorAnimationCount == GENERATOR_ANIM_FRAM)
			{
				m_nPipeStartAnimation[0] = true;
			}
			if (m_nGeneratorAnimationCount <= GENERATOR_ANIM_FRAM - 4)
			{
				m_nPipeStartAnimation[1] = true;
			}
			if (m_nGeneratorAnimationCount <= GENERATOR_ANIM_FRAM - 8)
			{
				m_nPipeStartAnimation[2] = true;
			}


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

	if (m_bSwitchAnimationOn)
	{
		float ButtonDelta = 0.1f;
		if (m_pButton)
		{
			if (m_nButtonAnimationCount == 0)
			{
				//m_nButtonAnimationCount = BUTTON_ANIM_FRAME;
			}
			else if (m_nButtonAnimationCount > 0)
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