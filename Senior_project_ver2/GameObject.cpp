#include "stdafx.h"
#include "GameObject.h"
#include "Shader.h"

CGameObject::CGameObject()
{
	//단위행렬로 초기화해서 초기위치가 (0.0f,0.0f,0.0f)임
	XMStoreFloat4x4(&m_xmf4x4World, XMMatrixIdentity());
}

CGameObject::~CGameObject()
{
	if (m_pMesh) m_pMesh->Release();
	if (m_pShader)
	{
		m_pShader->ReleaseShaderVariables();
		m_pShader->Release();
	}
}

void CGameObject::SetShader(CShader* pShader)
{
	if (m_pShader) m_pShader->Release();
	m_pShader = pShader;
	if (m_pShader) m_pShader->AddRef();
}

void CGameObject::SetMesh(CMesh* pMesh)
{
	if (m_pMesh) m_pMesh->Release();
	m_pMesh = pMesh;
	if (m_pMesh) m_pMesh->AddRef();
}

void CGameObject::ReleaseUploadBuffers()
{
	//정점 버퍼를 위한 업로드 버퍼를 소멸시킨다. 
	if (m_pMesh) m_pMesh->ReleaseUploadBuffers();
}

void CGameObject::Animate(float fTimeElapsed)
{
}

void CGameObject::OnPrepareRender()
{
}

void CGameObject::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	OnPrepareRender();

	//객체의 정보를 셰이더 변수(상수 버퍼)로 복사한다. 
	UpdateShaderVariables(pd3dCommandList);
	
	//게임 객체에 셰이더 객체가 연결되어 있으면 셰이더 상태 객체를 설정한다. 
	if (m_pShader) m_pShader->Render(pd3dCommandList, pCamera);

	//게임 객체에 메쉬가 연결되어 있으면 메쉬를 렌더링한다. 
	if (m_pMesh) m_pMesh->Render(pd3dCommandList);
}

void CGameObject::Rotate(XMFLOAT3* pxmf3Axis, float fAngle)
{
	XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(pxmf3Axis),
		XMConvertToRadians(fAngle));
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
}

void CGameObject::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
}

void CGameObject::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	XMFLOAT4X4 xmf4x4World;
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4World))); // 첫번째 주소에 2번째 인자로 받은 행렬 데이터 값 저장

	//객체의 월드 변환 행렬을 루트 상수(32-비트 값)를 통하여 셰이더 변수(상수 버퍼)로 복사한다. 
	pd3dCommandList->SetGraphicsRoot32BitConstants(0, 16, &xmf4x4World, 0);
}

void CGameObject::ReleaseShaderVariables()
{
}

XMFLOAT3 CGameObject::GetPosition()
{
	return(XMFLOAT3(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43));
}

//게임 객체의 로컬 z-축 벡터를 반환한다.
XMFLOAT3 CGameObject::GetLook()
{
	XMFLOAT3 vec3 = XMFLOAT3(m_xmf4x4World._31, m_xmf4x4World._32, m_xmf4x4World._33);
	return(Vector3::Normalize(vec3));
}

//게임 객체의 로컬 y-축 벡터를 반환한다.
XMFLOAT3 CGameObject::GetUp()
{
	XMFLOAT3 vec3 = XMFLOAT3(m_xmf4x4World._21, m_xmf4x4World._22,
		m_xmf4x4World._23);
	return(Vector3::Normalize(vec3));
}

//게임 객체의 로컬 x-축 벡터를 반환한다
XMFLOAT3 CGameObject::GetRight()
{
	XMFLOAT3 vec3 = XMFLOAT3(m_xmf4x4World._11, m_xmf4x4World._12,m_xmf4x4World._13);
	return (Vector3::Normalize(vec3));
}

CRotatingObject::CRotatingObject()
{
	m_xmf3RotationAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_fRotationSpeed = 90.0f;
}

CRotatingObject::~CRotatingObject()
{
}

void CRotatingObject::Animate(float fTimeElapsed)
{
	CGameObject::Rotate(&m_xmf3RotationAxis, m_fRotationSpeed * fTimeElapsed);
}

void CGameObject::SetPosition(float x, float y, float z)
{
	//---월드행렬은 단위행렬로 초기화 되있으므로 4행에 값이 들어가면 위치가 된다.
	m_xmf4x4World._41 = x;
	m_xmf4x4World._42 = y;
	m_xmf4x4World._43 = z;
}

void CGameObject::SetPosition(XMFLOAT3 xmf3Position)
{
	//---float3 형식으로도 인자를 받을 수 있도록 오버라이드되었다.
	SetPosition(xmf3Position.x, xmf3Position.y, xmf3Position.z);
}

//게임 객체를 로컬 x-축 방향으로 이동한다.
void CGameObject::MoveStrafe(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Right = GetRight();
	//벡터끼리의 연산
	xmf3Position = Vector3::Add(xmf3Position, xmf3Right, fDistance);
	CGameObject::SetPosition(xmf3Position);
}

//게임 객체를 로컬 y-축 방향으로 이동한다
void CGameObject::MoveUp(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Up = GetUp();
	xmf3Position = Vector3::Add(xmf3Position,xmf3Up, fDistance);
	CGameObject::SetPosition(xmf3Position);
}

//게임 객체를 로컬 z-축 방향으로 이동한다.
void CGameObject::MoveForward(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Look = GetLook();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Look, fDistance);
	CGameObject::SetPosition(xmf3Position);
}

//게임 객체를 주어진 각도로 회전한다.
void CGameObject::Rotate(float fPitch, float fYaw, float fRoll)
{
	XMMATRIX mtxRotate = XMMatrixRotationRollPitchYaw(XMConvertToRadians(fPitch),
		XMConvertToRadians(fYaw), XMConvertToRadians(fRoll));
	//회전 행렬을 월드변환 행렬에 곱한다.
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
}
