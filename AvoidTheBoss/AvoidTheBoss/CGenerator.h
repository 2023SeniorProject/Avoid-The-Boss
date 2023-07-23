#pragma once
#include "GameObject.h"

#define BUTTON_ANIM_FRAME 50
#define GENERATOR_PIPE_ANIM_FRAM 16
#define GENERATOR_BODY_ANIM_FRAM 16
class CGenerator : public CGameObject
{
public:
	float radius = 0.0f;
	int m_nPipe = 3;
	XMFLOAT4 	xmf4NormalVector;

	CGameObject** m_ppPipe = NULL;
	CGameObject* m_pButton = NULL;
	CGameObject* m_pBody = NULL;

public: //06-03 추가
	int m_idx = -1;
	float m_maxGuage = 100;
	float m_curGuage = 0;
	float m_guageSpeed = 10.0f;
public:
	std::mutex m_lock;
	bool m_bGenActive = false; // --> 발전기가 활성화 되었는가
	bool m_bOnInteraction = false; // --> 발전기가 상호작용 중인가?
	bool m_bAlreadyOn = false;
	bool m_bOnGenAnimation = false; // 발전기 애니메이션 활성화 여부
	bool m_nPipeStartAnimation[3];
public:
	int  m_nButtonAnimationCount = BUTTON_ANIM_FRAME;
	int  m_nGenerPipeAnimationCount[3];
	int  m_nGenerBodyAnimationCount = 0;
	float m_AnimationDegree = 0.0f;
public:
	CGenerator();
	virtual ~CGenerator() {};
	float GetRadius() { return radius; }

	virtual void SetNormalVector();
	void LogicUpdate();
	void SetOnGenAnimation(bool value) { m_bOnGenAnimation = value; }
	bool GetOnGenAnimation() { return m_bOnGenAnimation; }
	void SetInteractionOn(bool value) { m_bOnInteraction = value; };
	bool GetInteractionOn() { return m_bOnInteraction; }
	void SetAlreadyOn(bool value) { m_bAlreadyOn = value; }
	bool GetAlreadyOn() { return m_bAlreadyOn; };
	bool IsAvailable() { return (!m_bAlreadyOn && !m_bGenActive); }

	virtual void Update(float fTimeElapsed);
	virtual void OnPrepareAnimate();
	virtual void Animate(float fTimeElapsed);

	void PipelineAnimate(float fTimeElapsed);
	void BodyAnimate(float fTimeElapsed);
};


