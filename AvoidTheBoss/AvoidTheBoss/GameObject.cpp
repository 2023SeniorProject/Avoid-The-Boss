#include "stdafx.h"
#include "GameObject.h"
#include "Shader.h"

CGameObject::CGameObject()
{
	//단위행렬로 초기화해서 초기위치가 (0.0f,0.0f,0.0f)임
	XMStoreFloat4x4(&m_xmf4x4World, XMMatrixIdentity());
}

CGameObject::CGameObject(int nMeshes)
{
	m_xmf4x4World = Matrix4x4::Identity();

	m_nMeshes = nMeshes;
	m_ppMeshes = NULL;
	if (m_nMeshes > 0)
	{
		m_ppMeshes = new CMesh * [m_nMeshes];
		for (int i = 0; i < m_nMeshes; i++) m_ppMeshes[i] = NULL;
	}
}

CGameObject::~CGameObject()
{
	if (m_ppMeshes)
	{
		for (int i = 0; i < m_nMeshes; i++)
		{
			if (m_ppMeshes[i]) m_ppMeshes[i]->Release();
			m_ppMeshes[i] = NULL;
		}
		delete[] m_ppMeshes;
	}
	if (m_pMaterial)
	{
		m_pMaterial->ReleaseShaderVariables();
		m_pMaterial->Release();
	}
}

void CGameObject::SetMesh(int nIndex, CMesh* pMesh)
{
	if (m_ppMeshes)
	{
		if (m_ppMeshes[nIndex]) m_ppMeshes[nIndex]->Release();
		m_ppMeshes[nIndex] = pMesh;
		if (pMesh) pMesh->AddRef();
	}
}

void CGameObject::SetShader(CShader* pShader)
{
	if (m_pMaterial->m_pShader)
	{
		m_pMaterial->m_pShader->Release();
		m_pMaterial->m_pShader = pShader;
		m_pMaterial->m_pShader->AddRef();
	}
}

void CGameObject::ReleaseUploadBuffers()
{
	//정점 버퍼를 위한 업로드 버퍼를 소멸시킨다. 
	if (m_ppMeshes)
	{
		for (int i = 0; i < m_nMeshes; i++)
		{
			if (m_ppMeshes[i])
				m_ppMeshes[i]->ReleaseUploadBuffers();
		}
	}
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

	//게임 객체에 셰이더 객체가 연결되어 있으면 셰이더 상태 객체를 설정한다. 
	if (m_pMaterial)
	{
		if (m_pMaterial->m_pShader)
		{
			m_pMaterial->m_pShader->Render(pd3dCommandList, pCamera);
			m_pMaterial->m_pShader->UpdateShaderVariables(pd3dCommandList);

			//객체의 정보를 셰이더 변수(상수 버퍼)로 복사한다. 
			UpdateShaderVariables(pd3dCommandList);
		}
		
		if (m_pMaterial->m_pTexture)
		{
			m_pMaterial->m_pTexture->UpdateShaderVariables(pd3dCommandList);
		}
	}

	pd3dCommandList->SetGraphicsRootDescriptorTable(2, m_d3dCbvGPUDescriptorHandle);

	//게임 객체에 메쉬가 연결되어 있으면 메쉬를 렌더링한다. 
	//게임 객체가 포함하는 모든 메쉬를 렌더링한다. 
	if (m_ppMeshes)
	{
		for (int i = 0; i < m_nMeshes; i++)
		{
			if (m_ppMeshes[i]) m_ppMeshes[i]->Render(pd3dCommandList);
		}
	}
}

void CGameObject::Rotate(XMFLOAT3* pxmf3Axis, float fAngle)
{
	XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(pxmf3Axis),
		XMConvertToRadians(fAngle));
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
}

void CGameObject::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수
	m_pd3dcbGameObject = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbGameObject->Map(0, NULL, (void**)&m_pcbMappedGameObject);
}

void CGameObject::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	XMFLOAT4X4 xmf4x4World;
	XMStoreFloat4x4(&m_pcbMappedGameObject->m_xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4World)));

	//객체의 월드 변환 행렬을 루트 상수(32-비트 값)를 통하여 셰이더 변수(상수 버퍼)로 복사한다. 
	//pd3dCommandList->SetGraphicsRoot32BitConstants(0, 16, &xmf4x4World, 0);
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
	XMFLOAT3 vec3 = XMFLOAT3(m_xmf4x4World._11, m_xmf4x4World._12, m_xmf4x4World._13);
	return (Vector3::Normalize(vec3));
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

void CGameObject::SetObjectInWorld(CGameObject** ppObjects, int i, XMFLOAT3 position, CMesh* pMesh,CMaterial* pMaterial,D3D12_GPU_DESCRIPTOR_HANDLE m_d3dCbvGPUDescriptorStartHandle, int nIndex)
{
	SetMesh(nIndex, pMesh);
	SetMaterial(pMaterial);
	SetPosition(position);
	SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescIncrementSize * i));
	ppObjects[i] = this;
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
	xmf3Position = Vector3::Add(xmf3Position, xmf3Up, fDistance);
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

void CGameObject::SetMaterial(CMaterial* pMaterial)
{
	if (m_pMaterial) m_pMaterial->Release();
	m_pMaterial = pMaterial;
	if (m_pMaterial) m_pMaterial->AddRef();
}
//-----------------------------------------
CTexture::CTexture(int nTexture, UINT nTextureType, int nRootParameters, int nSamplers) : m_nTexture(nTexture), m_nRootParameters(nRootParameters), m_nSamplers(nSamplers)
{
	if (m_nTexture > 0)
	{
		m_ppTexture = new ID3D12Resource * [m_nTexture];
		m_ppUploadBuffer = new ID3D12Resource * [m_nTexture];

		for (int i = 0; i < m_nTexture; i++)
		{
			m_ppTexture[i] = NULL;
			m_ppUploadBuffer[i] = NULL;
		}

		m_pTextureType = new UINT[nTextureType];
		for (int i = 0; i < (int)nTextureType; i++)
		{
			m_pTextureType[i] = NULL;
		}
		m_pTextureName = new _TCHAR[m_nTexture][64];
		for (int i = 0; i <m_nTexture; i++)
		{
			m_pTextureName[i][0] = '\0';
		}


		m_ppd3dTexUploadBuffers = new ID3D12Resource*[m_nTexUploadBuf];
		for (int i = 0; i < m_nTexUploadBuf; i++)
		{
			m_ppd3dTexUploadBuffers[i] = NULL;
		}

		m_pSrvDescriptorHandles = new CD3DX12_GPU_DESCRIPTOR_HANDLE[m_nTexture];
		for (int i = 0; i < m_nTexture; i++)
		{
			m_pSrvDescriptorHandles[i].ptr = NULL;
		}

		m_pSamplerDescriptorHandles = new CD3DX12_GPU_DESCRIPTOR_HANDLE[m_nSamplers];
		for (int i = 0; i < m_nSamplers; i++)
		{
			m_pSamplerDescriptorHandles[i].ptr = NULL;
		}
	}

	if (nRootParameters > 0)
	{
		m_pnRootParamIndices = new int[m_nRootParameters];

		for (int i = 0; i < m_nRootParameters; i++)
		{
			m_pnRootParamIndices[i] = -1;
		}
	}
}


CTexture::~CTexture()
{
	if (m_ppTexture)
		delete[] m_ppTexture;

	if (m_ppUploadBuffer)
		delete[] m_ppUploadBuffer;

	if (m_pTextureName)
		delete[] m_pTextureName;

	if (m_pSamplerDescriptorHandles)
		delete[] m_pSamplerDescriptorHandles;

	if (m_pSrvDescriptorHandles)
		delete[] m_pSrvDescriptorHandles;

	if (m_pnResourceTypes)
		delete[] m_pnResourceTypes;
}

ID3D12Resource* CTexture::LoadTextureFromDDSFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, const wchar_t* pszFileName, UINT nResourceType, UINT nIndex)
{
	m_pnResourceTypes[nIndex] = nResourceType;
	m_ppTexture[nIndex] = ::CreateTextureResourceFromDDSFile(pd3dDevice, pd3dCommandList, pszFileName, &m_ppUploadBuffer[nIndex]);
	return m_ppTexture[nIndex];
}

D3D12_SHADER_RESOURCE_VIEW_DESC CTexture::GetShaderResourceViewDesc(int nIndex)
{
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDescriptor;
	srvDescriptor.Format = m_ppTexture[nIndex]->GetDesc().Format;
	srvDescriptor.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDescriptor.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDescriptor.Texture2D.MipLevels = -1; //-1 : MostDetailedMip~마지막 수준 모두 포함
	srvDescriptor.Texture2D.MostDetailedMip = 0;
	srvDescriptor.Texture2D.PlaneSlice = 0; //특정 형식이라 0으로 포맷
	srvDescriptor.Texture2D.ResourceMinLODClamp = 0.0f;
	
	return srvDescriptor;
}
void CTexture::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	for (int i = 0; i < m_nRootParameters; i++)
	{
		pd3dCommandList->SetGraphicsRootDescriptorTable(m_pnRootParamIndices[i], m_pSrvDescriptorHandles[i]);
	}
}

void CTexture::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList, int nRootParam, int nTexIndex)
{
	pd3dCommandList->SetGraphicsRootDescriptorTable(m_pnRootParamIndices[nRootParam], m_pSrvDescriptorHandles[nTexIndex]);
}

void CTexture::ReleaseUploadBuffers()
{
	if (m_nTexUploadBuf)
	{
		for (int i = 0; i < m_nTexUploadBuf; i++)
		{
			m_ppd3dTexUploadBuffers[i]->Release();
		}
		delete[] m_ppd3dTexUploadBuffers;
		m_ppd3dTexUploadBuffers = NULL;
	}
}
void CTexture::ReleaseShaderVariables()
{

}

CMaterial::CMaterial()
{
	m_xmfAlbedo = XMFLOAT3(1.0f, 1.0f, 1.0f);
}

CMaterial::~CMaterial()
{
	if (m_pTexture) 
		m_pTexture->Release();
	if (m_pShader) 
		m_pShader->Release();
}

void CMaterial::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	if (m_pTexture)
		m_pTexture->UpdateShaderVariables(pd3dCommandList);
}

void CMaterial::ReleaseShaderVariables()
{
	if (m_pTexture)
		m_pTexture->ReleaseShaderVariables();
	if (m_pShader)
		m_pShader->ReleaseShaderVariables();
}

void CMaterial::ReleaseUploadBuffers()
{
	if (m_pTexture)
		m_pTexture->ReleaseUploadBuffers();
}
