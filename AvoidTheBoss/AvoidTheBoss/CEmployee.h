#pragma once
#include "Player.h"
class CEmployee : public CPlayer
{
public:
	bool m_bIsSwitchInterationing = false; // F키를 눌렀다 땠는지 확인하는 용도
private:
	bool  m_bIsSwitchArea = false;	 // 스위치 영역에 들어와 있는가?
	
public:
	CEmployee(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CHARACTER_TYPE nType);
	virtual ~CEmployee();

	virtual CCamera* ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed);
	virtual void OnPlayerUpdateCallback();
	virtual void OnCameraUpdateCallback();

	virtual void Move(DWORD dwDirection, float fDistance);

	virtual void Update(float fTimeElapsed, PLAYER_TYPE ptype);

	virtual void OnInteractive();

	void SetSwitchArea(bool value) { m_bIsSwitchArea = value; }
	bool GetSwitchArea() { return m_bIsSwitchArea; }

	bool CheckSwitchArea();
	SwitchInformation m_pSwitch;
};