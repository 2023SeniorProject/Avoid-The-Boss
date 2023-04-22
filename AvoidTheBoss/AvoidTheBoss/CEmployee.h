#pragma once
#include "Player.h"
class CEmployee : public CPlayer
{
public:
	bool m_bIsSwitchInterationing = false; // FŰ�� ������ ������ Ȯ���ϴ� �뵵
private:
	bool  m_bIsSwitchArea = false;	 // ����ġ ������ ���� �ִ°�?
	
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