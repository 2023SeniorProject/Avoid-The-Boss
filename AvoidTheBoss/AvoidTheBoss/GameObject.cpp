#include "stdafx.h"
#include "GameObject.h"
#include "Shader.h"

CGameObject::CGameObject()
{
	//������ķ� �ʱ�ȭ�ؼ� �ʱ���ġ�� (0.0f,0.0f,0.0f)��
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
	//���� ���۸� ���� ���ε� ���۸� �Ҹ��Ų��. 
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

	//���� ��ü�� ���̴� ��ü�� ����Ǿ� ������ ���̴� ���� ��ü�� �����Ѵ�. 
	if (m_pMaterial)
	{
		if (m_pMaterial->m_pShader)
		{
			m_pMaterial->m_pShader->Render(pd3dCommandList, pCamera);
			m_pMaterial->m_pShader->UpdateShaderVariables(pd3dCommandList);

			//��ü�� ������ ���̴� ����(��� ����)�� �����Ѵ�. 
			UpdateShaderVariables(pd3dCommandList);
		}
		
		if (m_pMaterial->m_pTexture)
		{
			m_pMaterial->m_pTexture->UpdateShaderVariables(pd3dCommandList);
		}
	}

	pd3dCommandList->SetGraphicsRootDescriptorTable(2, m_d3dCbvGPUDescriptorHandle);

	//���� ��ü�� �޽��� ����Ǿ� ������ �޽��� �������Ѵ�. 
	//���� ��ü�� �����ϴ� ��� �޽��� �������Ѵ�. 
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
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256�� ���
	m_pd3dcbGameObject = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbGameObject->Map(0, NULL, (void**)&m_pcbMappedGameObject);
}

void CGameObject::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	XMFLOAT4X4 xmf4x4World;
	XMStoreFloat4x4(&m_pcbMappedGameObject->m_xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4World)));

	//��ü�� ���� ��ȯ ����� ��Ʈ ���(32-��Ʈ ��)�� ���Ͽ� ���̴� ����(��� ����)�� �����Ѵ�. 
	//pd3dCommandList->SetGraphicsRoot32BitConstants(0, 16, &xmf4x4World, 0);
}

void CGameObject::ReleaseShaderVariables()
{
}

XMFLOAT3 CGameObject::GetPosition()
{
	return(XMFLOAT3(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43));
}

//���� ��ü�� ���� z-�� ���͸� ��ȯ�Ѵ�.
XMFLOAT3 CGameObject::GetLook()
{
	XMFLOAT3 vec3 = XMFLOAT3(m_xmf4x4World._31, m_xmf4x4World._32, m_xmf4x4World._33);
	return(Vector3::Normalize(vec3));
}

//���� ��ü�� ���� y-�� ���͸� ��ȯ�Ѵ�.
XMFLOAT3 CGameObject::GetUp()
{
	XMFLOAT3 vec3 = XMFLOAT3(m_xmf4x4World._21, m_xmf4x4World._22,
		m_xmf4x4World._23);
	return(Vector3::Normalize(vec3));
}

//���� ��ü�� ���� x-�� ���͸� ��ȯ�Ѵ�
XMFLOAT3 CGameObject::GetRight()
{
	XMFLOAT3 vec3 = XMFLOAT3(m_xmf4x4World._11, m_xmf4x4World._12, m_xmf4x4World._13);
	return (Vector3::Normalize(vec3));
}

void CGameObject::SetPosition(float x, float y, float z)
{
	//---��������� ������ķ� �ʱ�ȭ �������Ƿ� 4�࿡ ���� ���� ��ġ�� �ȴ�.
	m_xmf4x4World._41 = x;
	m_xmf4x4World._42 = y;
	m_xmf4x4World._43 = z;
}

void CGameObject::SetPosition(XMFLOAT3 xmf3Position)
{
	//---float3 �������ε� ���ڸ� ���� �� �ֵ��� �������̵�Ǿ���.
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

//���� ��ü�� ���� x-�� �������� �̵��Ѵ�.
void CGameObject::MoveStrafe(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Right = GetRight();
	//���ͳ����� ����
	xmf3Position = Vector3::Add(xmf3Position, xmf3Right, fDistance);
	CGameObject::SetPosition(xmf3Position);
}

//���� ��ü�� ���� y-�� �������� �̵��Ѵ�
void CGameObject::MoveUp(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Up = GetUp();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Up, fDistance);
	CGameObject::SetPosition(xmf3Position);
}

//���� ��ü�� ���� z-�� �������� �̵��Ѵ�.
void CGameObject::MoveForward(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Look = GetLook();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Look, fDistance);
	CGameObject::SetPosition(xmf3Position);
}

//���� ��ü�� �־��� ������ ȸ���Ѵ�.
void CGameObject::Rotate(float fPitch, float fYaw, float fRoll)
{
	XMMATRIX mtxRotate = XMMatrixRotationRollPitchYaw(XMConvertToRadians(fPitch),
		XMConvertToRadians(fYaw), XMConvertToRadians(fRoll));
	//ȸ�� ����� ���庯ȯ ��Ŀ� ���Ѵ�.
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
	srvDescriptor.Texture2D.MipLevels = -1; //-1 : MostDetailedMip~������ ���� ��� ����
	srvDescriptor.Texture2D.MostDetailedMip = 0;
	srvDescriptor.Texture2D.PlaneSlice = 0; //Ư�� �����̶� 0���� ����
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
