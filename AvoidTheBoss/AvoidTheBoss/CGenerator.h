#pragma once
#include "GameObject.h"

#define BUTTON_ANIM_FRAME 50
#define GENERATOR_ANIM_FRAM 16
class CGenerator : public CGameObject
{

	float radius = 0.0f;
	int m_nPipe = 3;
	CGameObject** m_ppPipe = NULL;
	CGameObject* m_pButton = NULL;
	bool m_nPipeStartAnimation[3];
public: //06-03 추가
	int m_idx = -1;
	float m_maxGuage = 100;
	float m_curGuage = 0;
	float m_guageSpeed = 50.0f;
public:
	std::mutex m_lock;
	bool m_bGenActive = false; // --> 발전기가 활성화 되었는가
	bool m_bOnInteraction = false; // --> 발전기가 상호작용 중인가?
	bool m_bAlreadyOn = false;
public:
	int  m_nButtonAnimationCount = BUTTON_ANIM_FRAME;
	int  m_nGeneratorAnimationCount[3];
public:
	CGenerator();
	virtual ~CGenerator() {};
	float GetRadius() { return radius; }

	void LogicUpdate();
	//void SetAnimationCount(int value) { m_nGeneratorAnimationCount[0] = value; }
	//bool GetAnimationCount() { return m_nGeneratorAnimationCount[0]; }
	void SetInteractionOn(bool value) { m_bOnInteraction = value; };
	bool GetInteractionOn() { return m_bOnInteraction; }
	void SetAlreadyOn(bool value) { m_bAlreadyOn = value; }
	bool GetAlreadyOn() { return m_bAlreadyOn; };
	bool IsAvailable() { return (!m_bAlreadyOn && !m_bGenActive); }

	virtual void Update(float fTimeElapsed);
	virtual void OnPrepareAnimate();
	virtual void Animate(float fTimeElapsed);
};


