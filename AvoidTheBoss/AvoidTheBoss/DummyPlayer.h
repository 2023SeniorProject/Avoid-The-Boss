#pragma once
#include "GameObject.h"

struct CB_DUMMYPLAYER_INFO
{
	XMFLOAT4X4					m_xmf4x4World;
};

class DummyPlayer : public CGameObject
{
protected:

	XMFLOAT3 m_xmf3Position;
	XMFLOAT3 m_xmf3Right;
	XMFLOAT3 m_xmf3Up;
	XMFLOAT3 m_xmf3Look;

	float m_fPitch;
	float m_fYaw;
	float m_fRoll;

	XMFLOAT3 m_xmf3Velocity; // 플레이어 속도
	XMFLOAT3 m_xmf3Gravity;  // 중력
	float m_fFriction;       // 마찰력

	// 업데이트 후처리 함수
	LPVOID m_pPlayerUpdatedContext;
	LPVOID m_pCameraUpdatedContext;

	ID3D12Resource* m_pd3dcbPlayer = NULL;
	CB_DUMMYPLAYER_INFO* m_pcbMappedPlayer = NULL;
public:
	DummyPlayer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, void* pContext = NULL, int nMeshes = 1);
	virtual ~DummyPlayer();

	XMFLOAT3 GetPosition() const { return(m_xmf3Position); }
	XMFLOAT3 GetLookVector() { return(m_xmf3Look); }
	XMFLOAT3 GetUpVector() { return(m_xmf3Up); }
	XMFLOAT3 GetRightVector() { return(m_xmf3Right); }

	void SetVelocity(const XMFLOAT3& xmf3Velocity) { m_xmf3Velocity = xmf3Velocity; }

	void SetPosition(const XMFLOAT3& xmf3Position)
	{
		UpdateMove(XMFLOAT3(xmf3Position.x - m_xmf3Position.x, xmf3Position.y - m_xmf3Position.y, xmf3Position.z - m_xmf3Position.z));
	}

	XMFLOAT3& GetVelocity() { return(m_xmf3Velocity); }

	float GetYaw() { return(m_fYaw); }
	float GetPitch() { return(m_fPitch); }
	float GetRoll() { return(m_fRoll); }

	void Move(DWORD dwDirection, float fDistance);
	void SetSpeed(const XMFLOAT3& xmf3Shift);
	void UpdateMove(const XMFLOAT3& velocity);

	void Rotate(float x, float y, float z);

	void Update(float fTimeElapsed);

	virtual void OnPlayerUpdateCallback(float fTimeElapsed) { }
	void SetPlayerUpdatedContext(LPVOID pContext) { m_pPlayerUpdatedContext = pContext; }

	virtual void OnCameraUpdateCallback(float fTimeElapsed) { }
	void SetCameraUpdatedContext(LPVOID pContext) { m_pCameraUpdatedContext = pContext; }

	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList
		* pd3dCommandList);
	virtual void ReleaseShaderVariables();
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);

	//플레이어의 위치와 회전축으로부터 월드 변환 행렬을 생성하는 함수이다. 
	virtual void OnPrepareRender();

	//플레이어의 카메라가 3인칭 카메라일 때 플레이어(메쉬)를 렌더링한다. 
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList,
		CCamera* pCamera = NULL);
};


class DummyCubePlayer : public DummyPlayer
{
public:
	DummyCubePlayer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, int nMeshes = 1);
	virtual ~DummyCubePlayer();
};