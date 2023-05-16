#pragma once
#include "GameObject.h"

#define BUTTON_ANIM_FRAME 50
#define GENERATOR_ANIM_FRAM 20
#define PIPE_ANIMATION_SPEED 0.01f
class CGenerator : public CGameObject
{
	float radius = 0.0f;
	int m_nPipe = 3;
	CGameObject** m_ppPipe = NULL;
	CGameObject* m_pButton = NULL;
	int m_fPipeDistanceCount[3];
	bool m_nPipeStartAnimation[3];
	bool m_bPipeMoveUp[3];
	//bool m_bPipeMoveDown = false;
public:
	bool m_bSwitchActive = false; // --> �����Ⱑ Ȱ��ȭ �Ǿ��°�
	bool m_bSwitchAnimationOn = false; // �ִϸ��̼� ����� ���� ����
	bool m_bOtherPlayerInteractionOn = false; // �ٸ� �÷��̾ ��ȣ�ۿ� ���ΰ�? --> �ٸ� �÷��̾ ��ȣ�ۿ� ���̶�� Ȱ��ȭ �Ұ���
	int  m_nButtonAnimationCount = BUTTON_ANIM_FRAME;
	int m_nGeneratorAnimationCount = GENERATOR_ANIM_FRAM;

	std::mutex m_lock;

	CGenerator();
	virtual ~CGenerator() {};

	CGameObject* GetButton() { std::cout << "button pos" << m_pButton->GetPosition().x << m_pButton->GetPosition().y << m_pButton->GetPosition().z << std::endl; return m_pButton; }

	float GetRadius() { return radius; }
	void SetAnimationCount(int value) { m_nGeneratorAnimationCount = value; }
	bool GetAnimationCount() { return m_nGeneratorAnimationCount; }
	void InteractAnimation(bool value) { m_bSwitchAnimationOn = value; };


	virtual void OnPrepareAnimate();
	virtual void Animate(float fTimeElapsed);
};


