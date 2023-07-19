#pragma once
#include "GameObject.h"

enum class CLIENT_TYPE
{
	OWNER,OTHER_PLAYER,NONE
};

enum class PLAYER_TYPE
{
	NONE = 0 ,BOSS = 1, EMPLOYEE = 2
};

enum class CHARACTER_TYPE: int32
{
	BOSS = 0, YELLOW_EMP = 1, MASK_EMP, CAP_EMP, GOGGLE_EMP, COUNT
};

static const char *g_pstrThirdCharactorRefernece[5] =
{
	"null",
	"Model/Character/Character1_Idle.bin",
	"Model/Character/Character2_Idle.bin",
	"Model/Character/Character3_Idle.bin",
	"Model/Character/Character4_Idle.bin"
};

static const char* g_pstrFirstCharactorRefernece[5] =
{
	"null",
	"Model/Character/Character1_Run_1st_View.bin",
	"Model/Character/Character2_Idle.bin",
	"Model/Character/Character3_Idle.bin",
	"Model/Character/Character4_Idle.bin"
};

#define BOSS_ATTACK_TIME 25 //25������ (���� 65)
#define BOSS_RUNATTACK_TIME 25 //25������ (���� 65)

#define EMPLOYEE_ATTACKED_TIME 30 //20������ (���� 65)
#define EMPLOYEE_DOWN_TIME 30 //25������ (���� 65)
#define EMPLOYEE_STAND_TIME 30 //25������ (���� 65)
#define EMPLOYEE_INTERACTION_TIME 40 //20������

class CPlayer : public CGameObject
{
	friend class CSession;
	friend class InteractionEvent;
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

	// �÷��̾� ī�޶�
	//CCamera* m_pCamera = nullptr;

public:
	// �÷��̾� ī�޶�
	CCamera* m_pCamera = nullptr;

	uint8 m_ctype = -1; // �ڽ��� ĳ���� Ÿ���� ����
	int16 m_sid = -1; // �ڽ��� Session Id
	std::mutex m_lock; // �ڽ��� Lock

	BoundingSphere m_playerBV; // BV = bounding volume
	CHARACTER_TYPE m_nCharacterType;
	
	bool m_hide = false;// �÷��̾ ���� ���̳� �׸� ���̳�
	
	// 05-21 �߰�
	int32 m_hp = 5; // hp�� 5�� ����
	int32 m_idx = 0;
	// 05-22 �߰�
	int32 m_behavior = (int32)PLAYER_BEHAVIOR::IDLE;

	bool m_bOnMoveSound = false;
	virtual void SetOnMoveSound(bool bOnMoveSound) {
		m_bOnMoveSound = bOnMoveSound;
	}
	virtual bool GetOnMoveSound() {
		return m_bOnMoveSound;
	}

	// �ǰ� ����Ʈ �ڵ�
	bool m_bIsOnUIActive = false;
	void SetIsOnUIActive(bool value) { m_bIsOnUIActive = value; }
	bool GetIsOnUIActive() { return m_bIsOnUIActive; }
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
	}
	void SetVelocity(const XMFLOAT3& xmf3Velocity) { m_xmf3Velocity = xmf3Velocity; }
	void SetPlayerSid(const int16& sid) { m_sid = sid; }
	void SetPosition(const XMFLOAT3& xmf3Position) 
	{
		m_xmf3Position = xmf3Position;
	}
	void SetScale(const XMFLOAT3& xmf3Scale) { m_xmf3Scale = xmf3Scale; }
	const XMFLOAT3& GetVelocity() const { return(m_xmf3Velocity); }
	float GetYaw() { return(m_fYaw); }
	float GetPitch() { return(m_fPitch); }
	float GetRoll() { return(m_fRoll); }

	void SetCamera(CCamera* pCamera) { m_pCamera = pCamera; }
	CCamera* GetCamera() { return(m_pCamera); }

	//�÷��̾ ȸ���ϴ� �Լ��̴�. 
	virtual void Rotate(float x, float y, float z);
	virtual void CreateShaderVariables(ID3D12Device5* pd3dDevice, ID3D12GraphicsCommandList4
		* pd3dCommandList);
	virtual void ReleaseShaderVariables();
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList4* pd3dCommandList);

	//ī�޶� �����ϱ� ���Ͽ� ȣ���ϴ� �Լ��̴�. 
	CCamera* OnChangeCamera(DWORD nNewCameraMode, DWORD nCurrentCameraMode);
	virtual CCamera* ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed) { return(NULL); }

	//�÷��̾��� ��ġ�� ȸ�������κ��� ���� ��ȯ ����� �����ϴ� �Լ��̴�. 
	virtual void OnPrepareRender();
	//�÷��̾��� ī�޶� 3��Ī ī�޶��� �� �÷��̾�(�޽�)�� �������Ѵ�. 
	virtual void Render(ID3D12GraphicsCommandList4* pd3dCommandList, CCamera* pCamera, bool bRaster);

public: //04-29 �߰��Լ� 
	virtual uint8 ProcessInput() { return 0; };
	virtual void SetAnimationTrack(int32 num) {};
	virtual void Move(const int8& dwDirection, float fDistance);
	virtual void Update(float fTimeElapsed, CLIENT_TYPE ptype);
	virtual void LateUpdate() {};
	// 05-22 �߰� �Լ�
	virtual void AnimTrackUpdate(float ,CLIENT_TYPE) {};
	virtual void SetBehavior(PLAYER_BEHAVIOR b) { m_behavior = (int32)b; };
	virtual int32 GetBehavior() { return m_behavior; }

	virtual void BuildObjects(ID3D12Device5* pd3dDevice, ID3D12GraphicsCommandList4
		* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature) {}
};

class CVirtualPlayer : public CPlayer
{
public:
	CVirtualPlayer();
	CVirtualPlayer(ID3D12Device5* pd3dDevice, ID3D12GraphicsCommandList4* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual ~CVirtualPlayer();
public:
	virtual CCamera* ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed);

	virtual void Move(DWORD dwDirection, float fDistance);
	virtual void Animate(float fTimeElapsed);
	virtual void Update(float fTimeElapsed);
	virtual void BuildObjects(ID3D12Device5* pd3dDevice, ID3D12GraphicsCommandList4* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual uint8 ProcessInput();
};

struct GEN_INFO
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

