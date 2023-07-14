#include "pch.h"
#include "Shader.h"
#include "Player.h"
#include "CBullet.h"


CPlayer::CPlayer()
{
	m_type = 0;
	m_pCamera = NULL;
	m_xmf3Position = XMFLOAT3(0.0f, 0.25f, 0.0f);

	m_xmf3Right = XMFLOAT3(1.0f, 0.0f, 0.0f);
	m_xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_xmf3Look = XMFLOAT3(0.0f, 0.0f, 1.0f);

	m_xmf3Velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_xmf3Gravity = XMFLOAT3(0.0f, 0.0f, 0.0f);
	
	m_fFriction = 0.0f;

	m_fPitch = 0.0f;
	m_fRoll = 0.0f;
	m_fYaw = 0.0f;

	
}

CPlayer::~CPlayer()
{
	ReleaseShaderVariables();

	if (m_pCamera) delete m_pCamera;
}

/*�÷��̾��� ��ġ�� �����ϴ� �Լ��̴�. �÷��̾��� ��ġ�� �⺻������ ����ڰ� �÷��̾ �̵��ϱ� ���� Ű���带
���� �� ����ȴ�. �÷��̾��� �̵� ����(dwDirection)�� ���� �÷��̾ fDistance ��ŭ �̵��Ѵ�.*/

void CPlayer::Move(const int8& dwDirection, float fDistance)
{
	XMFLOAT3 xmf3Shift = XMFLOAT3(0, 0, 0);
	if (LOBYTE(dwDirection))
	{	
		//ȭ��ǥ Ű ���衯�� ������ ���� z-�� �������� �̵�(����)�Ѵ�. ���顯�� ������ �ݴ� �������� �̵��Ѵ�.
		
		if (LOBYTE(dwDirection) & KEY_FORWARD) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Look, fDistance);
		if (LOBYTE(dwDirection) & KEY_BACKWARD) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Look, -fDistance);
		if (LOBYTE(dwDirection) & KEY_RIGHT) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Right, fDistance);
		if (LOBYTE(dwDirection) & KEY_LEFT) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Right, -fDistance);
		//�÷��̾ ���� ��ġ ���Ϳ��� xmf3Shift ���͸�ŭ �̵��Ѵ�
		m_xmf3Velocity = XMFLOAT3(0, 0, 0);
		SetVelocity(xmf3Shift);
	}
	else SetVelocity(xmf3Shift);
}

void CPlayer::Update(float fTimeElapsed, CLIENT_TYPE ptype)
{
	XMFLOAT3 vel = Vector3::ScalarProduct(m_xmf3Velocity, fTimeElapsed, false);
	m_xmf3Position = Vector3::Add(m_xmf3Position, vel);
	m_playerBV.Center = GetPosition();

	DWORD nCameraMode = m_pCamera->GetMode();
	if (m_pCamera)
	{
		m_pCamera->Move(vel);
		m_pCamera->Update(m_xmf3Position, fTimeElapsed);
	}
	if (nCameraMode == THIRD_PERSON_CAMERA)
		m_pCamera->SetLookAt(m_xmf3Position);
	//else 
	//	m_pCamera->SetPosition(Vector3::Add(m_xmf3Position, m_pCamera->GetPosition());); //ī�޶� offset
	m_pCamera->RegenerateViewMatrix();
}

//�÷��̾ ���� x-��, y-��, z-���� �߽����� ȸ���Ѵ�.
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


void CPlayer::CreateShaderVariables(ID3D12Device5* pd3dDevice, ID3D12GraphicsCommandList4* pd3dCommandList)
{
	if (m_pCamera) m_pCamera->CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void CPlayer::ReleaseShaderVariables()
{
	if (m_pCamera) m_pCamera->ReleaseShaderVariables();
}

void CPlayer::UpdateShaderVariables(
	ID3D12GraphicsCommandList4* pd3dCommandList)
{
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
	//if (m_pCamera) delete m_pCamera;
	m_pCamera = pNewCamera;
	
	return(pNewCamera);
}

void CPlayer::OnPrepareRender()
{
	m_xmf4x4ToParent._11 = m_xmf3Right.x; m_xmf4x4ToParent._12 = m_xmf3Right.y; m_xmf4x4ToParent._13 = m_xmf3Right.z;
	m_xmf4x4ToParent._21 = m_xmf3Up.x; m_xmf4x4ToParent._22 = m_xmf3Up.y; m_xmf4x4ToParent._23 = m_xmf3Up.z;
	m_xmf4x4ToParent._31 = m_xmf3Look.x; m_xmf4x4ToParent._32 = m_xmf3Look.y; m_xmf4x4ToParent._33 = m_xmf3Look.z;
	m_xmf4x4ToParent._41 = m_xmf3Position.x; m_xmf4x4ToParent._42 = m_xmf3Position.y; m_xmf4x4ToParent._43 = m_xmf3Position.z;

	m_xmf4x4ToParent = Matrix4x4::Multiply(XMMatrixScaling(m_xmf3Scale.x, m_xmf3Scale.y, m_xmf3Scale.z), m_xmf4x4ToParent);
}

void CPlayer::Render(ID3D12GraphicsCommandList4 * pd3dCommandList, CCamera* pCamera,bool bRaster)
{
	DWORD nCameraMode = (pCamera) ? pCamera->GetMode() : 0x03;

	//ī�޶� ��尡 3��Ī�̸� �÷��̾� ��ü�� �������Ѵ�. 
	CGameObject::Render(pd3dCommandList, pCamera, bRaster); 
}

CVirtualPlayer::CVirtualPlayer()
{
	m_pCamera = ChangeCamera(FIRST_PERSON_CAMERA, 0.0f);
	SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));
}

CVirtualPlayer::CVirtualPlayer(ID3D12Device5* pd3dDevice, ID3D12GraphicsCommandList4* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	m_pCamera = ChangeCamera(FIRST_PERSON_CAMERA, 0.0f);

	//CGameObject* pVirtualModel = CGameObject::LoadGeometryFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, /*"Model/Boss_Run.bin"*/"Model///Plane.bin", NULL);
	//SetChild(pVirtualModel, true);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);	
	//if (pVirtualModel) delete pVirtualModel;
}

CVirtualPlayer::~CVirtualPlayer()
{
}

void CVirtualPlayer::Animate(float fTimeElapsed)
{
}
CCamera* CVirtualPlayer::ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed)
{
	DWORD nCurrentCameraMode = (m_pCamera) ? m_pCamera->GetMode() : 0x00;
	if (nCurrentCameraMode == nNewCameraMode)
		return(m_pCamera);

	
	float MaxDepthofMap = 5000.0f;//sqrt(2) * 50 * UNIT + 2 * UNIT;
	switch (nNewCameraMode)
	{
	case FIRST_PERSON_CAMERA:
		m_pCamera = OnChangeCamera(FIRST_PERSON_CAMERA, nCurrentCameraMode);
		m_pCamera->SetTimeLag(0.0f);
		m_pCamera->SetOffset(XMFLOAT3(0.0f, 0.0f, 0.0f));
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

	Update(fTimeElapsed);

	return(m_pCamera);
}

void CVirtualPlayer::Move(DWORD dwDirection, float fDistance)
{

}

void CVirtualPlayer::Update(float fTimeElapsed)
{

}

void CVirtualPlayer::BuildObjects(ID3D12Device5* pd3dDevice, ID3D12GraphicsCommandList4
	* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{

}
uint8 CVirtualPlayer::ProcessInput()
{
	return uint8();
}
#define _WITH_DEBUG_CALLBACK_DATA

void CSoundCallbackHandler::HandleCallback(void* pCallbackData, float fTrackPosition)
{
	_TCHAR* pWavName = (_TCHAR*)pCallbackData;
//#ifdef _WITH_DEBUG_CALLBACK_DATA
//	TCHAR pstrDebug[256] = { 0 };
//	_stprintf_s(pstrDebug, 256, _T("%s(%f)\n"), pWavName, fTrackPosition);
//	OutputDebugString(pstrDebug);
//#endif
//#ifdef _WITH_SOUND_RESOURCE
//	PlaySound(pWavName, ::ghAppInstance, SND_RESOURCE | SND_ASYNC);
//#else
	PlaySound(pWavName, NULL, SND_LOOP | SND_ASYNC);
//#endif

}


