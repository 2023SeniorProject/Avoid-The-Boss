#include "pch.h"
#include "Shader.h"
#include "DummyPlayer.h"

DummyPlayer::DummyPlayer(int nMeshes) : CGameObject(nMeshes)
{

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

DummyPlayer::~DummyPlayer()
{
	ReleaseShaderVariables();
}

void DummyPlayer::Move(DWORD dwDirection, float fDistance)
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

void DummyPlayer::SetSpeed(const XMFLOAT3& xmf3Shift)
{
	m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, xmf3Shift);
	

}

void DummyPlayer::UpdateMove(const XMFLOAT3& xmf3Shift)
{
	m_xmf3Position = Vector3::Add(m_xmf3Position, xmf3Shift);
}

//�÷��̾ ���� x-��, y-��, z-���� �߽����� ȸ���Ѵ�.
void DummyPlayer::Rotate(float x, float y, float z)
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
		
		m_fRoll += z;
		if (m_fRoll > +20.0f) { z -= (m_fRoll - 20.0f); m_fRoll = +20.0f; }
		if (m_fRoll < -20.0f) { z -= (m_fRoll + 20.0f); m_fRoll = -20.0f; }
	}


	if (y != 0.0f)
	{
		XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Up),
			XMConvertToRadians(y));
		m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
		m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);
	}
	
	m_xmf3Look = Vector3::Normalize(m_xmf3Look);
	m_xmf3Right = Vector3::CrossProduct(m_xmf3Up, m_xmf3Look, true);
	m_xmf3Up = Vector3::CrossProduct(m_xmf3Look, m_xmf3Right, true);
}

void DummyPlayer::Update(float fTimeElapsed)
{
	
	XMFLOAT3 xmf3Velocity = Vector3::ScalarProduct(m_xmf3Velocity, fTimeElapsed, false);
	UpdateMove(xmf3Velocity);
	if (m_pPlayerUpdatedContext) OnPlayerUpdateCallback(fTimeElapsed);
}

void DummyPlayer::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	CGameObject::CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void DummyPlayer::ReleaseShaderVariables()
{
	CGameObject::ReleaseShaderVariables();
}

void DummyPlayer::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	CGameObject::UpdateShaderVariables(pd3dCommandList);
}



/*�÷��̾��� ��ġ�� ȸ�������κ��� ���� ��ȯ ����� �����ϴ� �Լ��̴�. �÷��̾��� Right ���Ͱ� ���� ��ȯ ���
�� ù ��° �� ����, Up ���Ͱ� �� ��° �� ����, Look ���Ͱ� �� ��° �� ����, �÷��̾��� ��ġ ���Ͱ� �� ��° ��
���Ͱ� �ȴ�.*/
void DummyPlayer::OnPrepareRender()
{
	m_xmf4x4World._11 = m_xmf3Right.x;
	m_xmf4x4World._12 = m_xmf3Right.y;
	m_xmf4x4World._13 = m_xmf3Right.z;
	m_xmf4x4World._21 = m_xmf3Up.x;
	m_xmf4x4World._22 = m_xmf3Up.y;
	m_xmf4x4World._23 = m_xmf3Up.z;
	m_xmf4x4World._31 = m_xmf3Look.x;
	m_xmf4x4World._32 = m_xmf3Look.y;
	m_xmf4x4World._33 = m_xmf3Look.z;
	m_xmf4x4World._41 = m_xmf3Position.x;
	m_xmf4x4World._42 = m_xmf3Position.y;
	m_xmf4x4World._43 = m_xmf3Position.z;
}

void DummyPlayer::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	DWORD nCameraMode = (pCamera) ? pCamera->GetMode() : 0x00;

	//ī�޶� ��尡 3��Ī�̸� �÷��̾� ��ü�� �������Ѵ�. 
	if (nCameraMode == THIRD_PERSON_CAMERA)
	{
		if (m_ppShaders) m_ppShaders->Render(pd3dCommandList, pCamera);
		CGameObject::Render(pd3dCommandList, pCamera);
	}
}

DummyCubePlayer::DummyCubePlayer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, int nMeshes) : DummyPlayer(nMeshes)
{
	CMesh* pPlayerCubeMesh = new CCubeMeshDiffused(pd3dDevice, pd3dCommandList, 0.37f * UNIT, 1.5f * UNIT, 0.23f * UNIT);

	SetMesh(0, pPlayerCubeMesh);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	SetPosition(XMFLOAT3(0.0f, (1.5f / 2.0f) * UNIT, 0.0f));

	CPlayerShader* pShader = new CPlayerShader();
	pShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
	SetShader(pShader);
}


DummyCubePlayer::~DummyCubePlayer()
{
}






