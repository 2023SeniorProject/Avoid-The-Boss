#pragma once
#include "GameObject.h"

class CGenerator : public CGameObject
{
private:
	float radius = 0.0f;

public:
	bool m_bSwitchActive = false; // --> �����Ⱑ Ȱ��ȭ �Ǿ��°�
	bool m_bSwitchAnimationOn = false; // �ִϸ��̼� ����� ���� ����
	bool m_bOtherPlayerInteractionOn = false; // �ٸ� �÷��̾ ��ȣ�ۿ� ���ΰ�? --> �ٸ� �÷��̾ ��ȣ�ۿ� ���̶�� Ȱ��ȭ �Ұ���
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

