#pragma once



#include "GameObject.h"
#include "Camera.h"
#include "Session.h"



class CPlayer : public CGameObject
{
protected:
	//플레이어의 위치 벡터, x-축(Right), y-축(Up), z-축(Look) 벡터이다. 
	XMFLOAT3 m_xmf3Position;
	XMFLOAT3 m_xmf3Right;
	XMFLOAT3 m_xmf3Up;
	XMFLOAT3 m_xmf3Look;

	//플레이어가 로컬 x-축(Right), y-축(Up), z-축(Look)으로 얼마만큼 회전했는가를 나타낸다. 
	float m_fPitch;
	float m_fYaw;
	float m_fRoll;

	//플레이어의 이동 속도를 나타내는 벡터이다. 
	XMFLOAT3 m_xmf3Velocity;

	//플레이어에 작용하는 중력을 나타내는 벡터이다.
	XMFLOAT3 m_xmf3Gravity;

	//xz-평면에서 (한 프레임 동안) 플레이어의 이동 속력의 최대값을 나타낸다.
	float m_fMaxVelocityXZ;

	//y-축 방향으로 (한 프레임 동안) 플레이어의 이동 속력의 최대값을 나타낸다.
	float m_fMaxVelocityY;

	//플레이어에 작용하는 마찰력을 나타낸다.
	float m_fFriction;

	//플레이어의 위치가 바뀔 때마다 호출되는 OnPlayerUpdateCallback() 함수에서 사용하는 데이터이다.
	LPVOID m_pPlayerUpdatedContext;
	//카메라의 위치가 바뀔 때마다 호출되는 OnCameraUpdateCallback() 함수에서 사용하는 데이터이다.
	LPVOID m_pCameraUpdatedContext;
	//플레이어에 현재 설정된 카메라이다. 

	CCamera* m_pCamera = NULL;

public:
	CPlayer(int nMeshes = 1);
	virtual ~CPlayer();

	XMFLOAT3 GetPosition() const { return(m_xmf3Position); }
	XMFLOAT3 GetLookVector() { return(m_xmf3Look); }
	XMFLOAT3 GetUpVector() { return(m_xmf3Up); }
	XMFLOAT3 GetRightVector() { return(m_xmf3Right); }

	void SetVelocity(const XMFLOAT3& xmf3Velocity) { m_xmf3Velocity = xmf3Velocity; }

	/*플레이어의 위치를 xmf3Position 위치로 설정한다. xmf3Position 벡터에서 현재 플레이어의 위치 벡터를 빼면 현
	재 플레이어의 위치에서 xmf3Position 방향으로의 벡터가 된다. 현재 플레이어의 위치에서 이 벡터 만큼 이동한다.*/
	void SetPosition(const XMFLOAT3& xmf3Position) 
	{
		UpdateMove(XMFLOAT3(xmf3Position.x - m_xmf3Position.x, xmf3Position.y - m_xmf3Position.y, xmf3Position.z - m_xmf3Position.z));
	}

	XMFLOAT3& GetVelocity() { return(m_xmf3Velocity); }
	float GetYaw() { return(m_fYaw); }
	float GetPitch() { return(m_fPitch); }
	float GetRoll() { return(m_fRoll); }

	void SetCamera(CCamera* pCamera) { m_pCamera = pCamera; }
	CCamera* GetCamera() { return(m_pCamera); }

	//플레이어를 이동하는 함수이다. 
	void Move(DWORD dwDirection, float fDistance);
	void SetSpeed(const XMFLOAT3& xmf3Shift);
	void UpdateMove(const XMFLOAT3& velocity);

	//플레이어를 회전하는 함수이다. 
	void Rotate(float x, float y, float z);

	//플레이어의 위치와 회전 정보를 경과 시간에 따라 갱신하는 함수이다.
	void Update(float fTimeElapsed);

	//플레이어의 위치가 바뀔 때마다 호출되는 함수와 그 함수에서 사용하는 정보를 설정하는 함수이다.
	virtual void OnPlayerUpdateCallback(float fTimeElapsed) { }
	void SetPlayerUpdatedContext(LPVOID pContext) { m_pPlayerUpdatedContext = pContext; }

	//카메라의 위치가 바뀔 때마다 호출되는 함수와 그 함수에서 사용하는 정보를 설정하는 함수이다. 
	virtual void OnCameraUpdateCallback(float fTimeElapsed) { }
	void SetCameraUpdatedContext(LPVOID pContext) 
	{
		m_pCameraUpdatedContext = pContext;
	}
	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList
		* pd3dCommandList);
	virtual void ReleaseShaderVariables();
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);

	//카메라를 변경하기 위하여 호출하는 함수이다. 
	CCamera* OnChangeCamera(DWORD nNewCameraMode, DWORD nCurrentCameraMode);
	virtual CCamera* ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed) 
	{
		return(NULL);
	}

	//플레이어의 위치와 회전축으로부터 월드 변환 행렬을 생성하는 함수이다. 
	virtual void OnPrepareRender();

	//플레이어의 카메라가 3인칭 카메라일 때 플레이어(메쉬)를 렌더링한다. 
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, 
		CCamera* pCamera = NULL);
};

class CCubePlayer : public CPlayer
{
public:
	CCubePlayer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, int nMeshes = 1);
	virtual ~CCubePlayer();

	virtual CCamera* ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed);
};


// =============== new Client Session ================
class NewClientSession : public IocpObject
{
public:
	NewClientSession();
	
	virtual ~NewClientSession();
public:
	// 세션 인터페이스
	virtual HANDLE GetHandle() override;
	virtual void Processing(class IocpEvent* iocpEvent, int32 numOfBytes = 0) override;
public:
	// 세션 정보를 얻어 내거나 세팅할 수 있는 함수들
	SOCKET GetSock() { return _sock; }
	bool DoSend(void* packet);
	bool DoRecv();
	void ProcessPacket(char*);
public:
	int32 _cid = -1;
	int32 _sid = -1;
	int16 _myRm = -1;
	int32 _prev_remain = 0;
	int8 _curScene = -1;
public:
	SOCKET _sock = INVALID_SOCKET;
	CCubePlayer* _player = nullptr;
	CCubePlayer* _other[3] = {};
	RecvEvent _rev;
	RWLOCK;
};

// ========= new Iocp Core ==============

class NCIocpCore : public IocpCore
{
public:
	NCIocpCore();
	~NCIocpCore();
	void InitConnect(const char* address);
	void DoConnect(void* loginInfo);
	virtual void Disconnect(int32 sid) override;
public:
	RWLOCK;
	NewClientSession* _client;
	SOCKADDR_IN _serveraddr;
};

extern class NCIocpCore ncIocpCore;

