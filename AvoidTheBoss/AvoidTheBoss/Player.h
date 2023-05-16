#pragma once
#include "GameObject.h"
#include "PlayerInfo.h"

enum class PLAYER_TYPE
{
	OWNER,OTHER_PLAYER,NONE
};

#define INTERACTION_TIME 60
class CPlayer : public CGameObject
{
protected:
	 
	XMFLOAT3					m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3					m_xmf3Right = XMFLOAT3(1.0f, 0.0f, 0.0f);
	XMFLOAT3					m_xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	XMFLOAT3					m_xmf3Look = XMFLOAT3(0.0f, 0.0f, 1.0f);
	XMFLOAT3					m_xmf3Scale = XMFLOAT3(1.0f, 1.0f, 1.0f);

	
	float m_fPitch;
	float m_fYaw;
	float m_fRoll;

	XMFLOAT3 m_xmf3Velocity; // 플레이어 속도
	XMFLOAT3 m_xmf3Gravity;  // 중력
	float m_fFriction;       // 마찰력

	// 플레이어 카메라
	CCamera* m_pCamera = nullptr;

public:
	uint8 m_ctype = -1; // 자신의 캐릭터 타입을 구현
	int16 m_sid = -1; // 자신으 Session Id
	std::mutex m_lock; // 자신의 Lock
	BoundingSphere m_playerBV; // BV = bounding volume
	XMFLOAT3					m_predictPos = XMFLOAT3(0, 0, 0);
	bool m_OnInteraction = false;
	int m_InteractionCountTime = INTERACTION_TIME;

public: 
	CPlayer();
	virtual ~CPlayer();
	
	XMFLOAT3 GetPosition() const { return(m_xmf3Position); }
	XMFLOAT3& GetPredictPos() { return(m_predictPos); }
	XMFLOAT3 GetLookVector() { return(m_xmf3Look); }
	XMFLOAT3 GetUpVector() { return(m_xmf3Up); }
	XMFLOAT3 GetRightVector() { return(m_xmf3Right); }
	void SetDirection(const XMFLOAT3& look)
	{
		m_xmf3Look = look;
		m_xmf3Right = Vector3::CrossProduct(m_xmf3Up, m_xmf3Look, true);
		m_xmf3Up = Vector3::CrossProduct(m_xmf3Look, m_xmf3Right, true);
	}
	void SetVelocity(const XMFLOAT3& xmf3Velocity) { m_xmf3Velocity = xmf3Velocity; }
	void SetPlayerSid(const int16& sid) { m_sid = sid; }
	void SetPosition(const XMFLOAT3& xmf3Position) 
	{
		MakePosition(xmf3Position);
	}
	void SetPredicPos(const XMFLOAT3& pred) { m_predictPos = pred; }
	void SetScale(const XMFLOAT3& xmf3Scale) { m_xmf3Scale = xmf3Scale; }
	void MakePosition(const XMFLOAT3& xmf3Position)
	{
		m_xmf3Position = xmf3Position;
	}


	const XMFLOAT3& GetVelocity() const { return(m_xmf3Velocity); }
	float GetYaw() { return(m_fYaw); }
	float GetPitch() { return(m_fPitch); }
	float GetRoll() { return(m_fRoll); }

	void SetCamera(CCamera* pCamera) { m_pCamera = pCamera; }
	CCamera* GetCamera() { return(m_pCamera); }

<<<<<<< Updated upstream
	//플레이어를 이동하는 함수이다. 
	virtual void Move(DWORD dwDirection, float fDistance);
	void SetSpeed(const XMFLOAT3& xmf3Shift);
	void UpdateMove(const XMFLOAT3& velocity);

	void ProcesesInput();
	 
	void SetOnInteraction(bool value) 
	{
		m_OnInteraction = value; 
	}
	void OnInteractive();

	//플레이어를 회전하는 함수이다. 
	void Rotate(float x, float y, float z);

	//플레이어의 위치와 회전 정보를 경과 시간에 따라 갱신하는 함수이다.
	virtual void Update(float fTimeElapsed, PLAYER_TYPE ptype);

	virtual void OnPlayerUpdateCallback();
	void SetPlayerUpdatedContext(LPVOID pContext) { m_pPlayerUpdatedContext = pContext; }

	virtual void OnCameraUpdateCallback() { }
	void SetCameraUpdatedContext(LPVOID pContext) { m_pCameraUpdatedContext = pContext; }
=======
	//플레이어를 회전하는 함수이다. 
	virtual void Rotate(float x, float y, float z);
	
>>>>>>> Stashed changes

	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList
		* pd3dCommandList);
	virtual void ReleaseShaderVariables();
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);

	//카메라를 변경하기 위하여 호출하는 함수이다. 
	CCamera* OnChangeCamera(DWORD nNewCameraMode, DWORD nCurrentCameraMode);
	virtual CCamera* ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed) { return(NULL); }

	//플레이어의 위치와 회전축으로부터 월드 변환 행렬을 생성하는 함수이다. 
	virtual void OnPrepareRender();
	//플레이어의 카메라가 3인칭 카메라일 때 플레이어(메쉬)를 렌더링한다. 
<<<<<<< Updated upstream
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera =NULL);
=======
	

public: //04-29 추가함수 
	virtual void ProcessInput(const int16&) { };
	virtual void SetAnimationTrack(int32 num) {};
	virtual void Move(int16 dwDirection, float fDistance);
	virtual void Update(float fTimeElapsed, PLAYER_TYPE ptype);
	virtual void LateUpdate() {};
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera = NULL);
protected:
	int nInteractionNum = -1;
	bool m_OnInteraction = false;
	int m_InteractionCountTime = -1;;
public:
	void SetInteractionAnimation(bool value) { m_OnInteraction = value; }
	bool GetOnInteraction() { return m_OnInteraction; }

	void SetnInteractionNum(int value) { nInteractionNum = value; }
	int GetnInteractionNum() { return nInteractionNum; }
	
	void SetnInteractionCountTime(int value) { m_InteractionCountTime = value; }
	int GetnInteractionCountTime() { return m_InteractionCountTime; }

	virtual void OnInteractionAnimation() {}
>>>>>>> Stashed changes
};


class CWorker : public CPlayer
{
public:

	CWorker(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual ~CWorker();
public:
	virtual CCamera* ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed);
	virtual void OnPlayerUpdateCallback();
	virtual void OnCameraUpdateCallback();
	virtual void Move(DWORD dwDirection, float fDistance);
	virtual void Update(float fTimeElapsed, PLAYER_TYPE ptype);

};


class CSoundCallbackHandler : public CAnimationCallbackHandler
{
public:
	CSoundCallbackHandler() { }
	~CSoundCallbackHandler() { }

public:
	virtual void HandleCallback(void* pCallbackData, float fTrackPosition);
};

