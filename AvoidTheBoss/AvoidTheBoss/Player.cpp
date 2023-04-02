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

/*�÷��̾��� ��ġ�� �����ϴ� �Լ��̴�. �÷��̾��� ��ġ�� �⺻������ ����ڰ� �÷��̾ �̵��ϱ� ���� Ű���带
���� �� ����ȴ�. �÷��̾��� �̵� ����(dwDirection)�� ���� �÷��̾ fDistance ��ŭ �̵��Ѵ�.*/
void CPlayer::Rotate(float x, float y, float z)
{
	DWORD nCameraMode = m_pCamera->GetMode();

	//1��Ī ī�޶� �Ǵ� 3��Ī ī�޶��� ��� �÷��̾��� ȸ���� �ణ�� ������ ������. 
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

		//ī�޶� x, y, z ��ŭ ȸ���Ѵ�. �÷��̾ ȸ���ϸ� ī�޶� ȸ���ϰ� �ȴ�. 
		m_pCamera->Rotate(x, y, z);

		/*�÷��̾ ȸ���Ѵ�. 1��Ī ī�޶� �Ǵ� 3��Ī ī�޶󿡼� �÷��̾��� ȸ���� ���� y-�࿡���� �Ͼ��. �÷��̾�
		�� ���� y-��(Up ����)�� �������� ���� z-��(Look ����)�� ���� x-��(Right ����)�� ȸ����Ų��. �⺻������ Up ��
		�͸� �������� ȸ���ϴ� ���� �÷��̾ �ȹٷ� ���ִ� ���� �����Ѵٴ� �ǹ��̴�.*/
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
		//ȭ��ǥ Ű ���衯�� ������ ���� z-�� �������� �̵�(����)�Ѵ�. ���顯�� ������ �ݴ� �������� �̵��Ѵ�. 
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

		//�÷��̾ ���� ��ġ ���Ϳ��� xmf3Shift ���͸�ŭ �̵��Ѵ�. 
		SetSpeed(xmf3Shift);
	}
}

void CPlayer::OtherMove(DWORD dwDirection, float fDistance)
{
	XMFLOAT3 xmf3Shift = XMFLOAT3(0, 0, 0);
	if (dwDirection)
	{
		// xmf3Shift == ���� ����

		//ȭ��ǥ Ű ���衯�� ������ ���� z-�� �������� �̵�(����)�Ѵ�. ���顯�� ������ �ݴ� �������� �̵��Ѵ�. 
		if (dwDirection & DIR_FORWARD) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Look,
			fDistance);
		if (dwDirection & DIR_BACKWARD) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Look,
			-fDistance);

		//ȭ��ǥ Ű ���桯�� ������ ���� x-�� �������� �̵��Ѵ�. ���硯�� ������ �ݴ� �������� �̵��Ѵ�. 
		if (dwDirection & DIR_RIGHT)
			xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Right,
				fDistance);
		if (dwDirection & DIR_LEFT) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Right,
			-fDistance);
		//�÷��̾ ���� ��ġ ���Ϳ��� xmf3Shift ���͸�ŭ �̵��Ѵ�. 
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
		//�÷��̾ ���� ��ġ ���Ϳ��� xmf3Shift ���͸�ŭ �̵��Ѵ�. 
		m_xmf3Position = Vector3::Add(m_xmf3Position, xmf3Shift);
		//�÷��̾��� ��ġ�� ����Ǿ����Ƿ� ī�޶��� ��ġ�� xmf3Shift ���͸�ŭ �̵��Ѵ�. 
		if (m_pCamera) m_pCamera->Move(xmf3Shift);
}

void CPlayer::Update(float fTimeElapsed, PLAYER_TYPE ptype)
{

	//�÷��̾ �ӵ� ���� ��ŭ ������ �̵��Ѵ�(ī�޶� �̵��� ���̴�).
	XMFLOAT3 xmf3Velocity = Vector3::ScalarProduct(m_xmf3Velocity, fTimeElapsed, false); // 1/60 �� ���� �ӵ���
	UpdateMove(xmf3Velocity);
	m_playerBV.Center = GetPosition();
	/*�÷��̾��� ��ġ�� ����� �� �߰��� ������ �۾��� �����Ѵ�. �÷��̾��� ���ο� ��ġ�� ��ȿ�� ��ġ�� �ƴ� ����
	�ְ� �Ǵ� �÷��̾��� �浹 �˻� ���� ������ �ʿ䰡 �ִ�. �̷��� ��Ȳ���� �÷��̾��� ��ġ�� ��ȿ�� ��ġ�� �ٽ�
	������ �� �ִ�.*/
	
	if (m_pCamera == NULL) return;
	DWORD nCameraMode = m_pCamera->GetMode();

	//�÷��̾��� ��ġ�� ����Ǿ����Ƿ� 3��Ī ī�޶� �����Ѵ�. 
	if (nCameraMode == THIRD_PERSON_CAMERA) m_pCamera->Update(m_xmf3Position,
		fTimeElapsed);

	//ī�޶��� ��ġ�� ����� �� �߰��� ������ �۾��� �����Ѵ�. 
	if (m_pCameraUpdatedContext) OnCameraUpdateCallback(fTimeElapsed);

	//ī�޶� 3��Ī ī�޶��̸� ī�޶� ����� �÷��̾� ��ġ�� �ٶ󺸵��� �Ѵ�. 
	if (nCameraMode == THIRD_PERSON_CAMERA) m_pCamera->SetLookAt(m_xmf3Position);

	//ī�޶��� ī�޶� ��ȯ ����� �ٽ� �����Ѵ�. 
	m_pCamera->RegenerateViewMatrix();
	if(ptype == PLAYER_TYPE::OWNER) m_xmf3Velocity = XMFLOAT3(0, 0, 0);
	OnPlayerUpdateCallback();
	
}

void CPlayer::OtherUpdate(float fTimeElapsed)
{
	//�÷��̾ �ӵ� ���� ��ŭ ������ �̵��Ѵ�(ī�޶� �̵��� ���̴�). 
	XMFLOAT3 xmf3Velocity = Vector3::ScalarProduct(m_xmf3Velocity, fTimeElapsed, false); // 1/60 �� ���� �ӵ���
	UpdateMove(xmf3Velocity);

	if (m_pCamera == NULL) return;
	DWORD nCameraMode = m_pCamera->GetMode();

	//�÷��̾��� ��ġ�� ����Ǿ����Ƿ� 3��Ī ī�޶� �����Ѵ�. 
	if (nCameraMode == THIRD_PERSON_CAMERA) m_pCamera->Update(m_xmf3Position,
		fTimeElapsed);

	//ī�޶��� ��ġ�� ����� �� �߰��� ������ �۾��� �����Ѵ�. 
	if (m_pCameraUpdatedContext) OnCameraUpdateCallback(fTimeElapsed);

	//ī�޶� 3��Ī ī�޶��̸� ī�޶� ����� �÷��̾� ��ġ�� �ٶ󺸵��� �Ѵ�. 
	if (nCameraMode == THIRD_PERSON_CAMERA) m_pCamera->SetLookAt(m_xmf3Position);

	//ī�޶��� ī�޶� ��ȯ ����� �ٽ� �����Ѵ�. 
	m_pCamera->RegenerateViewMatrix();
}

void CPlayer::OnPlayerUpdateCallback()
{
	if (BoxTree->CheckCollision(m_playerBV,m_xmf3Look,m_xmf3Right,m_xmf3Up)) SetPosition(m_playerBV.Center);
}

//�÷��̾ ���� x-��, y-��, z-���� �߽����� ȸ���Ѵ�.


//�� �Լ��� �� �����Ӹ��� ȣ��ȴ�. �÷��̾��� �ӵ� ���Ϳ� �߷°� ������ ���� �����Ѵ�.




void CPlayer::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	//CGameObject::CreateShaderVariables(pd3dDevice, pd3dCommandList);

	if (m_pCamera) m_pCamera->CreateShaderVariables(pd3dDevice, pd3dCommandList);

	//UINT ncbElementBytes = ((sizeof(CB_PLAYER_INFO) + 255) & ~255); //256�� ���
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
	//���ο� ī�޶��� ��忡 ���� ī�޶� ���� �����Ѵ�. 
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
		//���� ī�޶� ����ϴ� �÷��̾� ��ü�� �����Ѵ�. 
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

	//ī�޶� ��尡 3��Ī�̸� �÷��̾� ��ü�� �������Ѵ�. 
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


