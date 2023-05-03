#pragma once
#include "GameObject.h"

enum class PLAYER_TYPE
{
	OWNER,OTHER_PLAYER,NONE
};

enum class CHARACTER_TYPE: int32
{
	BOSS = 0, YELLOW_EMP = 1, MASK_EMP, CAP_EMP, GOGGLE_EMP, COUNT
};

static const char *g_pstrCharactorRefernece[5] =
{
	"Model/Boss_Idle.bin",
	"Model/Character1_Idle.bin",
	"Model/Character2_Idle.bin",
	"Model/Character3_Idle.bin",
	"Model/Character4_Idle.bin"
};

#define BOSS_INTERACTION_TIME 65
#define EMPLOYEE_INTERACTION_TIME 40

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

	XMFLOAT3 m_xmf3Velocity; // �÷��̾� �ӵ�
	XMFLOAT3 m_xmf3Gravity;  // �߷�
	float m_fFriction;       // ������

	// ������Ʈ ��ó�� �Լ�
	LPVOID m_pPlayerUpdatedContext; 
	LPVOID m_pCameraUpdatedContext;

	// �÷��̾� ī�޶�
	CCamera* m_pCamera = nullptr;

public:
	int16 m_sid = -1; // �ڽ��� Session Id
	std::mutex m_lock; // �ڽ��� Lock
	BoundingSphere m_playerBV; // BV = bounding volume
	CHARACTER_TYPE m_nCharacterType;
	bool m_hide = false;// �÷��̾ ���� ���̳� �׸� ���̳�
public: 
	CPlayer();
	virtual ~CPlayer();
	
	XMFLOAT3 GetPosition() const { return(m_xmf3Position); }
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
		UpdateMove(XMFLOAT3(xmf3Position.x - m_xmf3Position.x, xmf3Position.y - m_xmf3Position.y, xmf3Position.z - m_xmf3Position.z));
	}
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

	//�÷��̾ �̵��ϴ� �Լ��̴�. 
	virtual void Move(DWORD dwDirection, float fDistance);
	void SetSpeed(const XMFLOAT3& xmf3Shift);
	void UpdateMove(const XMFLOAT3& velocity);

	//�÷��̾ ȸ���ϴ� �Լ��̴�. 
	void Rotate(float x, float y, float z);

	//�÷��̾��� ��ġ�� ȸ�� ������ ��� �ð��� ���� �����ϴ� �Լ��̴�.
	virtual void Update(float fTimeElapsed, PLAYER_TYPE ptype);

	virtual void OnPlayerUpdateCallback();
	void SetPlayerUpdatedContext(LPVOID pContext) { m_pPlayerUpdatedContext = pContext; }

	virtual void OnCameraUpdateCallback() { }
	void SetCameraUpdatedContext(LPVOID pContext) { m_pCameraUpdatedContext = pContext; }

	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList
		* pd3dCommandList);
	virtual void ReleaseShaderVariables();
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);

	//ī�޶� �����ϱ� ���Ͽ� ȣ���ϴ� �Լ��̴�. 
	CCamera* OnChangeCamera(DWORD nNewCameraMode, DWORD nCurrentCameraMode);
	virtual CCamera* ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed) { return(NULL); }

	//�÷��̾��� ��ġ�� ȸ�������κ��� ���� ��ȯ ����� �����ϴ� �Լ��̴�. 
	virtual void OnPrepareRender();
	//�÷��̾��� ī�޶� 3��Ī ī�޶��� �� �÷��̾�(�޽�)�� �������Ѵ�. 
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera =NULL);

public: //04-29 �߰��Լ� 
	virtual void ProcessInput(DWORD&) { };
protected:
	int nInteractionNum = -1;
	bool m_OnInteraction = false;
	int m_InteractionCountTime;
public:
	void SetInteractionAnimation(bool value) { m_OnInteraction = value; }
	bool GetOnInteraction() { return m_OnInteraction; }

	void SetnInteractionNum(int value) { nInteractionNum = value; }
	int GetnInteractionNum() { return nInteractionNum; }
	
	void SetnInteractionCountTime(int value) { m_InteractionCountTime = value; }
	int GetnInteractionCountTime() { return m_InteractionCountTime; }

	virtual void OnInteractionAnimation() {}
};



struct SwitchInformation
{
	XMFLOAT3 position;
	float radius; //raderArea
};


class CSoundCallbackHandler : public CAnimationCallbackHandler
{
public:
	CSoundCallbackHandler() { }
	~CSoundCallbackHandler() { }

public:
	virtual void HandleCallback(void* pCallbackData, float fTrackPosition);
};

