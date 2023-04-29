#pragma once
#include "GameObject.h"

class CGenerator : public CGameObject
{
private:
	float radius = 0.0f;

public:
	bool m_bSwitchActive = false; // --> 발전기가 활성화 되었는가
	bool m_bSwitchAnimationOn = false; // 애니메이션 재생을 위한 변수
	bool m_bOtherPlayerInteractionOn = false; // 다른 플레이어가 상호작용 중인가? --> 다른 플레이어가 상호작용 중이라면 활성화 불가능
	int  m_nAnimationCount = BUTTON_ANIM_FRAME;
	std::mutex m_lock;
	CGenerator();
	CGenerator(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CLoadedModelInfo* pModel, int nAnimationTracks, int number);
	virtual ~CGenerator();
	float GetRadius() { return radius; }
	void SetAnimationCount(int value) { m_nAnimationCount = value; }
	bool GetAnimationCount() { return m_nAnimationCount; }
	void InteractAnimation(bool value) { m_bSwitchAnimationOn = value; };
	virtual void Animate(float fTimeElapsed);

};

