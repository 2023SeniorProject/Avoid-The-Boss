#include "pch.h"
#include "Shader.h"
#include "Player.h"

CPlayer::CPlayer()
{
	m_type = 0;
	m_pCamera = NULL;
	m_playerBV.Center = GetPosition();
	m_playerBV.Extents = XMFLOAT3(0.02,0.02f,0.02);
	m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f);

	m_xmf3Right = XMFLOAT3(1.0f, 0.0f, 0.0f);
	m_xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_xmf3Look = XMFLOAT3(0.0f, 0.0f, 1.0f);

	m_xmf3Velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_xmf3Gravity = XMFLOAT3(0.0f, 0.0f, 0.0f);
	
	m_fFriction = 0.0f;

	m_fPitch = 0.0f;
	m_fRoll = 0.0f;
	m_fYaw = 0.0f;

	m_pPlayerUpdatedContext = NULL;
	m_pCameraUpdatedContext = NULL;
}

CPlayer::~CPlayer()
{
	ReleaseShaderVariables();

	if (m_pCamera) delete m_pCamera;
}

/*플레이어의 위치를 변경하는 함수이다. 플레이어의 위치는 기본적으로 사용자가 플레이어를 이동하기 위한 키보드를
누를 때 변경된다. 플레이어의 이동 방향(dwDirection)에 따라 플레이어를 fDistance 만큼 이동한다.*/
void CPlayer::Rotate(float x, float y, float z)
{
	DWORD nCameraMode = m_pCamera->GetMode();

	//1인칭 카메라 또는 3인칭 카메라의 경우 플레이어의 회전은 약간의 제약이 따른다. 
	if ((nCameraMode == FIRST_PERSON_CAMERA) || (nCameraMode == THIRD_PERSON_CAMERA))
	{

		if (x != 0.0f)
		{
			m_fPitch += x;
			if (m_fPitch > +89.0f) { x -= (m_fPitch - 89.0f); m_fPitch = +89.0f; }
			if (m_fPitch < -89.0f) { x -= (m_fPitch + 89.0f); m_fPitch = -89.0f; }
		}
		if (y != 0.0f)
		{

			m_fYaw += y;
			if (m_fYaw > 360.0f) m_fYaw -= 360.0f;
			if (m_fYaw < 0.0f) m_fYaw += 360.0f;
		}
		if (z != 0.0f)
		{

			if (m_fRoll > +20.0f) { z -= (m_fRoll - 20.0f); m_fRoll = +20.0f; }
			if (m_fRoll < -20.0f) { z -= (m_fRoll + 20.0f); m_fRoll = -20.0f; }
		}

		//카메라를 x, y, z 만큼 회전한다. 플레이어를 회전하면 카메라가 회전하게 된다. 
		m_pCamera->Rotate(x, y, z);

		/*플레이어를 회전한다. 1인칭 카메라 또는 3인칭 카메라에서 플레이어의 회전은 로컬 y-축에서만 일어난다. 플레이어
		의 로컬 y-축(Up 벡터)을 기준으로 로컬 z-축(Look 벡터)와 로컬 x-축(Right 벡터)을 회전시킨다. 기본적으로 Up 벡
		터를 기준으로 회전하는 것은 플레이어가 똑바로 서있는 것을 가정한다는 의미이다.*/
		if (y != 0.0f)
		{
			XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Up),
				XMConvertToRadians(y));
			m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
			m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);
		}
	}


	m_xmf3Look = Vector3::Normalize(m_xmf3Look);
	m_xmf3Right = Vector3::CrossProduct(m_xmf3Up, m_xmf3Look, true);
	m_xmf3Up = Vector3::CrossProduct(m_xmf3Look, m_xmf3Right, true);
}

void CPlayer::Move(DWORD dwDirection, float fDistance)
{
	XMFLOAT3 xmf3Shift = XMFLOAT3(0, 0, 0);
	if (dwDirection)
	{	
		//화살표 키 ‘↑’를 누르면 로컬 z-축 방향으로 이동(전진)한다. ‘↓’를 누르면 반대 방향으로 이동한다. 
		if (dwDirection & DIR_FORWARD) 
			xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Look,fDistance);
		if (dwDirection & DIR_BACKWARD) 
			xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Look,-fDistance);
		if (dwDirection & DIR_RIGHT)
			xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Right,fDistance);
		if (dwDirection & DIR_LEFT)	
			xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Right,-fDistance);
		if (dwDirection & DIR_UP)
			xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Up, fDistance);
		if (dwDirection & DIR_DOWN) 
			xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Up,-fDistance);

		//플레이어를 현재 위치 벡터에서 xmf3Shift 벡터만큼 이동한다. 
		SetSpeed(xmf3Shift);
	}
}

void CPlayer::OtherMove(DWORD dwDirection, float fDistance)
{
	XMFLOAT3 xmf3Shift = XMFLOAT3(0, 0, 0);
	if (dwDirection)
	{
		// xmf3Shift == 방향 벡터

		//화살표 키 ‘↑’를 누르면 로컬 z-축 방향으로 이동(전진)한다. ‘↓’를 누르면 반대 방향으로 이동한다. 
		if (dwDirection & DIR_FORWARD) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Look,
			fDistance);
		if (dwDirection & DIR_BACKWARD) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Look,
			-fDistance);

		//화살표 키 ‘→’를 누르면 로컬 x-축 방향으로 이동한다. ‘←’를 누르면 반대 방향으로 이동한다. 
		if (dwDirection & DIR_RIGHT)
			xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Right,
				fDistance);
		if (dwDirection & DIR_LEFT) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Right,
			-fDistance);
		//플레이어를 현재 위치 벡터에서 xmf3Shift 벡터만큼 이동한다. 
		m_xmf3Velocity = XMFLOAT3(0, 0, 0);
		SetSpeed(xmf3Shift);
	}
	else
	{
		SetVelocity(xmf3Shift);
	}
}

void CPlayer::SetSpeed(const XMFLOAT3& xmf3Shift)
{
	m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, xmf3Shift);	
}

void CPlayer::UpdateMove(const XMFLOAT3& xmf3Shift)
{
		//플레이어를 현재 위치 벡터에서 xmf3Shift 벡터만큼 이동한다. 
		m_xmf3Position = Vector3::Add(m_xmf3Position, xmf3Shift);
		//플레이어의 위치가 변경되었으므로 카메라의 위치도 xmf3Shift 벡터만큼 이동한다. 
		if (m_pCamera) m_pCamera->Move(xmf3Shift);
}

void CPlayer::Update(float fTimeElapsed, PLAYER_TYPE ptype)
{

	//플레이어를 속도 벡터 만큼 실제로 이동한다(카메라를 이동할 것이다).
	XMFLOAT3 xmf3Velocity = Vector3::ScalarProduct(m_xmf3Velocity, fTimeElapsed, false); // 1/60 초 변경 속도를
	UpdateMove(xmf3Velocity);
	m_playerBV.Center = GetPosition();
	/*플레이어의 위치가 변경될 때 추가로 수행할 작업을 수행한다. 플레이어의 새로운 위치가 유효한 위치가 아닐 수도
	있고 또는 플레이어의 충돌 검사 등을 수행할 필요가 있다. 이러한 상황에서 플레이어의 위치를 유효한 위치로 다시
	변경할 수 있다.*/
	
	if (m_pCamera == NULL) return;
	DWORD nCameraMode = m_pCamera->GetMode();

	//플레이어의 위치가 변경되었으므로 3인칭 카메라를 갱신한다. 
	if (nCameraMode == THIRD_PERSON_CAMERA) m_pCamera->Update(m_xmf3Position,
		fTimeElapsed);

	//카메라의 위치가 변경될 때 추가로 수행할 작업을 수행한다. 
	if (m_pCameraUpdatedContext) OnCameraUpdateCallback(fTimeElapsed);

	//카메라가 3인칭 카메라이면 카메라가 변경된 플레이어 위치를 바라보도록 한다. 
	if (nCameraMode == THIRD_PERSON_CAMERA) m_pCamera->SetLookAt(m_xmf3Position);

	//카메라의 카메라 변환 행렬을 다시 생성한다. 
	m_pCamera->RegenerateViewMatrix();
	if(ptype == PLAYER_TYPE::OWNER) m_xmf3Velocity = XMFLOAT3(0, 0, 0);
	OnPlayerUpdateCallback();
	
}

void CPlayer::OtherUpdate(float fTimeElapsed)
{
	//플레이어를 속도 벡터 만큼 실제로 이동한다(카메라를 이동할 것이다). 
	XMFLOAT3 xmf3Velocity = Vector3::ScalarProduct(m_xmf3Velocity, fTimeElapsed, false); // 1/60 초 변경 속도를
	UpdateMove(xmf3Velocity);

	if (m_pCamera == NULL) return;
	DWORD nCameraMode = m_pCamera->GetMode();

	//플레이어의 위치가 변경되었으므로 3인칭 카메라를 갱신한다. 
	if (nCameraMode == THIRD_PERSON_CAMERA) m_pCamera->Update(m_xmf3Position,
		fTimeElapsed);

	//카메라의 위치가 변경될 때 추가로 수행할 작업을 수행한다. 
	if (m_pCameraUpdatedContext) OnCameraUpdateCallback(fTimeElapsed);

	//카메라가 3인칭 카메라이면 카메라가 변경된 플레이어 위치를 바라보도록 한다. 
	if (nCameraMode == THIRD_PERSON_CAMERA) m_pCamera->SetLookAt(m_xmf3Position);

	//카메라의 카메라 변환 행렬을 다시 생성한다. 
	m_pCamera->RegenerateViewMatrix();
}

void CPlayer::OnPlayerUpdateCallback()
{
	if (BoxTree->CheckCollision(m_playerBV,m_xmf3Look,m_xmf3Right,m_xmf3Up)) SetPosition(m_playerBV.Center);
}

//플레이어를 로컬 x-축, y-축, z-축을 중심으로 회전한다.


//이 함수는 매 프레임마다 호출된다. 플레이어의 속도 벡터에 중력과 마찰력 등을 적용한다.




void CPlayer::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	//CGameObject::CreateShaderVariables(pd3dDevice, pd3dCommandList);

	if (m_pCamera) m_pCamera->CreateShaderVariables(pd3dDevice, pd3dCommandList);

	//UINT ncbElementBytes = ((sizeof(CB_PLAYER_INFO) + 255) & ~255); //256의 배수
	//m_pd3dcbPlayer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, //D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	//
	//m_pd3dcbPlayer->Map(0, NULL, (void**)&m_pcbMappedPlayer);
}

void CPlayer::ReleaseShaderVariables()
{
	//CGameObject::ReleaseShaderVariables();

	if (m_pCamera) m_pCamera->ReleaseShaderVariables();

	//if (m_pd3dcbPlayer)
	//{
	//	m_pd3dcbPlayer->Unmap(0, NULL);
	//	m_pd3dcbPlayer->Release();
	//}
}

void CPlayer::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	//XMStoreFloat4x4(&m_pcbMappedPlayer->m_xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4World)));
	//
	//D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress = m_pd3dcbPlayer->GetGPUVirtualAddress();
	//pd3dCommandList->SetGraphicsRootConstantBufferView(0, d3dGpuVirtualAddress);
	//CGameObject::UpdateShaderVariables(pd3dCommandList);
}

CCamera* CPlayer::OnChangeCamera(DWORD nNewCameraMode, DWORD nCurrentCameraMode)
{
	//새로운 카메라의 모드에 따라 카메라를 새로 생성한다. 
	CCamera* pNewCamera = NULL;
	switch (nNewCameraMode)
	{
	case FIRST_PERSON_CAMERA:
		pNewCamera = new CFirstPersonCamera(m_pCamera);
		break;
	case THIRD_PERSON_CAMERA:
		pNewCamera = new CThirdPersonCamera(m_pCamera);
		break;
	}

	if (pNewCamera)
	{
		pNewCamera->SetMode(nNewCameraMode);
		//현재 카메라를 사용하는 플레이어 객체를 설정한다. 
		pNewCamera->SetPlayer(this);
	}
	if (m_pCamera) delete m_pCamera;

	return(pNewCamera);
}

void CPlayer::OnPrepareRender()
{
	m_xmf4x4Transform._11 = m_xmf3Right.x;
	m_xmf4x4Transform._12 = m_xmf3Right.y;
	m_xmf4x4Transform._13 = m_xmf3Right.z;

	m_xmf4x4Transform._21 = m_xmf3Up.x;		   
	m_xmf4x4Transform._22 = m_xmf3Up.y;
	m_xmf4x4Transform._23 = m_xmf3Up.z;
	
	m_xmf4x4Transform._31 = m_xmf3Look.x;
	m_xmf4x4Transform._32 = m_xmf3Look.y;
	m_xmf4x4Transform._33 = m_xmf3Look.z;
	
	m_xmf4x4Transform._41 = m_xmf3Position.x;
	m_xmf4x4Transform._42 = m_xmf3Position.y;
	m_xmf4x4Transform._43 = m_xmf3Position.z;

	UpdateTransform(NULL);
}

void CPlayer::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	DWORD nCameraMode = (pCamera) ? pCamera->GetMode() : 0x03;

	//카메라 모드가 3인칭이면 플레이어 객체를 렌더링한다. 
	if (nCameraMode == THIRD_PERSON_CAMERA)
	{
		CGameObject::Render(pd3dCommandList, pCamera);
	}
}

CWorker::CWorker(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	m_pCamera = ChangeCamera(THIRD_PERSON_CAMERA, 0.0f);

	CGameObject* pGameObject = CGameObject::LoadGeometryFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Character_Boss.bin");

	float scale = 1.0f;
	pGameObject->Rotate(0.0f,0.0f,0.0f);
	pGameObject->SetScale(scale, scale, scale);
	SetChild(pGameObject, true);
	SetPosition(XMFLOAT3(0.0f, 1.25f, -50.0f));

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

CWorker::~CWorker()
{
}

CCamera* CWorker::ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed)
{
	DWORD nCurrentCameraMode = (m_pCamera) ? m_pCamera->GetMode() : 0x00;
	if (nCurrentCameraMode == nNewCameraMode)
		return(m_pCamera);

	float MaxDepthofMap = sqrt(2) * 50 * UNIT + 2 * UNIT;
	switch (nNewCameraMode)
	{
	case FIRST_PERSON_CAMERA:
		m_pCamera = OnChangeCamera(FIRST_PERSON_CAMERA, nCurrentCameraMode);
		m_pCamera->SetTimeLag(0.0f);
		m_pCamera->SetOffset(XMFLOAT3(0.0f, 1.4f, 0.0f));
		m_pCamera->GenerateProjectionMatrix(1.01f, MaxDepthofMap, ASPECT_RATIO, 60.0f); //5000.f
		m_pCamera->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
		m_pCamera->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
		break;
	case THIRD_PERSON_CAMERA:
		m_pCamera = OnChangeCamera(THIRD_PERSON_CAMERA, nCurrentCameraMode);
		m_pCamera->SetTimeLag(0.0f);
		m_pCamera->SetOffset(XMFLOAT3(0.0f, 1.7f * UNIT, -5 * UNIT));
		m_pCamera->GenerateProjectionMatrix(1.01f, MaxDepthofMap, ASPECT_RATIO, 60.0f);
		m_pCamera->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
		m_pCamera->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
		break;
	default:
		break;
	}
	m_pCamera->SetPosition(Vector3::Add(m_xmf3Position, m_pCamera->GetOffset()));

	Update(fTimeElapsed,PLAYER_TYPE::NONE);

	return(m_pCamera);
}

void CWorker::OnPrepareRender()
{
	CPlayer::OnPrepareRender();
}

void CWorker::OnInitialize()
{
}

void CWorker::Animate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent)
{
	CPlayer::Animate(fTimeElapsed, pxmf4x4Parent);
}


