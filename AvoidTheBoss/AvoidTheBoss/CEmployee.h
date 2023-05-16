#pragma once
#include "Player.h"
class CEmployee : public CPlayer
{
public:
	bool m_bIsPlayerOnSwitchInteration = false; // F키를 눌렀다 땠는지 확인하는 용도
private:
	bool m_bIsInSwitchArea = false;
public:
	CEmployee(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CHARACTER_TYPE nType);
	virtual ~CEmployee();

	virtual CCamera* ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed);
	

	//virtual void Rotate(float x, float y, float z);
	virtual void Move(DWORD dwDirection, float fDistance);
	virtual void Update(float fTimeElapsed, PLAYER_TYPE ptype);
	virtual void OnInteractionAnimation();
	void SwitchAnimationForOtherClient();
	virtual void ProcessInput(const int16&);
	bool IsPlayerCanSwitchInteraction() { return m_bIsInSwitchArea; }
	int32 GetAvailableSwitchIdx();

	SwitchInformation m_pSwitches[3];
};