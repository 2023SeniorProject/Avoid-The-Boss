#include "stdafx.h"
#include "GameObject.h"
#include "Shader.h"



//-----------------------------------------
//CTexture::CTexture(int nTexture, UINT nTextureType,  int nSamplers, int nRootParameters) : m_nTexture(nTexture), m_nTextureType(nTextureType), m_nSamplers(nSamplers), m_nRootParameters(nRootParameters)
//{
//	if (m_nTexture > 0)
//	{
//		m_ppTexture = new ID3D12Resource * [m_nTexture];
//		m_ppUploadBuffer = new ID3D12Resource * [m_nTexture];
//		for (int i = 0; i < m_nTexture; i++)
//		{
//			m_ppTexture[i] = NULL;
//			m_ppUploadBuffer[i] = NULL;
//		}
//
//		/*m_pTextureType = new UINT*[nTextureType];
//		for (int i = 0; i < (int)nTextureType; i++)
//		{
//			m_pTextureType = NULL;
//		}*/
//		m_pTextureName = new _TCHAR[m_nTexture][64];
//		for (int i = 0; i < m_nTexture; i++)
//		{
//			m_pTextureName[i][0] = '\0';
//		}
//
//		m_pSrvGPUDescHandles = new CD3DX12_GPU_DESCRIPTOR_HANDLE[m_nTexture];
//		for (int i = 0; i < m_nTexture; i++)
//		{
//			m_pSrvGPUDescHandles[i].ptr = NULL;
//		}
//
//		m_pnResourceTypes = new UINT[m_nTexture];
//		m_pdxgiBufferFormats = new DXGI_FORMAT[m_nTexture];
//		m_pnBufferElements = new int[m_nTexture];
//	}
//
//	if (nRootParameters > 0)
//	{
//		m_pnRootParamIndices = new int[m_nRootParameters];
//
//		for (int i = 0; i < m_nRootParameters; i++)
//		{
//			m_pnRootParamIndices[i] = -1;
//		}
//	}
//
//	if(m_nSamplers>0)
//		m_pSamplerGPUDescHandles = new CD3DX12_GPU_DESCRIPTOR_HANDLE[m_nSamplers];
//	for (int i = 0; i < m_nSamplers; i++)
//	{
//		m_pSamplerGPUDescHandles[i].ptr = NULL;
//	}
//}
//
//
//CTexture::~CTexture()
//{
//	if (m_ppTexture)
//	{
//		for (int i = 0; i < m_nTexture; i++)
//		{
//			if (m_ppTexture[i])
//				m_ppTexture[i]->Release();
//		}
//		delete[] m_ppTexture;
//	}
//
//	if (m_pTextureName)
//		delete[] m_pTextureName;
//
//	if (m_pnResourceTypes)
//		delete[] m_pnResourceTypes;
//	if (m_pdxgiBufferFormats)
//		delete[] m_pdxgiBufferFormats;
//	if (m_pnBufferElements)
//		delete[] m_pnBufferElements;
//
//	if (m_pnRootParamIndices)
//		delete[] m_pnRootParamIndices;
//	if (m_pSrvGPUDescHandles)
//		delete[] m_pSrvGPUDescHandles;
//
//	if (m_pSamplerGPUDescHandles)
//		delete[] m_pSamplerGPUDescHandles;
//}
//
//void CTexture::LoadTextureFromDDSFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, const wchar_t* pszFileName, UINT nResourceType, UINT nIndex)
//{
//	m_pnResourceTypes[nIndex] = nResourceType;
//	m_ppTexture[nIndex] = ::CreateTextureResourceFromDDSFile(pd3dDevice, pd3dCommandList, pszFileName, &m_ppUploadBuffer[nIndex], D3D12_RESOURCE_STATE_GENERIC_READ);
//}
//
//D3D12_SHADER_RESOURCE_VIEW_DESC CTexture::GetShaderResourceViewDesc(int nIndex)
//{
//	ID3D12Resource* pShaderResource = GetResource(nIndex);
//	D3D12_RESOURCE_DESC d3dResourceDesc = pShaderResource->GetDesc();
//
//	D3D12_SHADER_RESOURCE_VIEW_DESC d3dShaderResourceViewDesc;
//	d3dShaderResourceViewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
//
//	int nTextureType = GetTextureType(nIndex);
//	switch (nTextureType)
//	{
//	case RESOURCE_TEXTURE2D: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D)(d3dResourceDesc.DepthOrArraySize == 1)
//	case RESOURCE_TEXTURE2D_ARRAY: //[]
//		d3dShaderResourceViewDesc.Format = d3dResourceDesc.Format;
//		d3dShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
//		d3dShaderResourceViewDesc.Texture2D.MipLevels = -1;
//		d3dShaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
//		d3dShaderResourceViewDesc.Texture2D.PlaneSlice = 0;
//		d3dShaderResourceViewDesc.Texture2D.ResourceMinLODClamp = 0.0f;
//		break;
//	case RESOURCE_TEXTURE2DARRAY: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D)(d3dResourceDesc.DepthOrArraySize != 1)
//		d3dShaderResourceViewDesc.Format = d3dResourceDesc.Format;
//		d3dShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
//		d3dShaderResourceViewDesc.Texture2DArray.MipLevels = -1;
//		d3dShaderResourceViewDesc.Texture2DArray.MostDetailedMip = 0;
//		d3dShaderResourceViewDesc.Texture2DArray.PlaneSlice = 0;
//		d3dShaderResourceViewDesc.Texture2DArray.ResourceMinLODClamp = 0.0f;
//		d3dShaderResourceViewDesc.Texture2DArray.FirstArraySlice = 0;
//		d3dShaderResourceViewDesc.Texture2DArray.ArraySize = d3dResourceDesc.DepthOrArraySize;
//		break;
//	case RESOURCE_TEXTURE_CUBE: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D)(d3dResourceDesc.DepthOrArraySize == 6)
//		d3dShaderResourceViewDesc.Format = d3dResourceDesc.Format;
//		d3dShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
//		d3dShaderResourceViewDesc.TextureCube.MipLevels = 1;
//		d3dShaderResourceViewDesc.TextureCube.MostDetailedMip = 0;
//		d3dShaderResourceViewDesc.TextureCube.ResourceMinLODClamp = 0.0f;
//		break;
//	case RESOURCE_BUFFER: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_BUFFER)
//		d3dShaderResourceViewDesc.Format = m_pdxgiBufferFormats[nIndex];
//		d3dShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
//		d3dShaderResourceViewDesc.Buffer.FirstElement = 0;
//		d3dShaderResourceViewDesc.Buffer.NumElements = m_pnBufferElements[nIndex];
//		d3dShaderResourceViewDesc.Buffer.StructureByteStride = 0;
//		d3dShaderResourceViewDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
//		break;
//	}
//	return(d3dShaderResourceViewDesc);
//}
//void CTexture::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
//{
//	if (m_nRootParameters == m_nTexture)
//	{
//		for (int i = 0; i < m_nRootParameters; i++)
//		{
//			if (m_pSrvGPUDescHandles[i].ptr && (m_pnRootParamIndices[i] != -1))
//				pd3dCommandList->SetGraphicsRootDescriptorTable(m_pnRootParamIndices[i], m_pSrvGPUDescHandles[i]);
//		}
//	}
//	else
//	{
//		if (m_pSrvGPUDescHandles[0].ptr)
//			pd3dCommandList->SetGraphicsRootDescriptorTable(m_pnRootParamIndices[0], m_pSrvGPUDescHandles[0]);
//	}
//}
//
//void CTexture::UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, int nRootParam, int nTexIndex)
//{
//	pd3dCommandList->SetGraphicsRootDescriptorTable(m_pnRootParamIndices[nRootParam], m_pSrvGPUDescHandles[nTexIndex]);
//}
//
//void CTexture::ReleaseUploadBuffers()
//{
//	if (m_ppUploadBuffer)
//	{
//		for (int i = 0; i < m_nTexture; i++)
//		{
//			m_ppUploadBuffer[i]->Release();
//		}
//		delete[] m_ppUploadBuffer;
//		m_ppUploadBuffer = NULL;
//	}
//}
//
//void CTexture::ReleaseShaderVariables()
//{
//
//}
//=====================================
CShader* CMaterial::m_pIlluminatedShader = NULL;


CMaterial::CMaterial()
{
	//m_xmfAlbedo = XMFLOAT3(1.0f, 1.0f, 1.0f);
}

CMaterial::~CMaterial()
{
	//if (m_pTexture)
	//	m_pTexture->Release();
	if (m_pShader)
		m_pShader->Release();
	if (m_pMaterialColors) 
		m_pMaterialColors->Release();
}

//void CMaterial::SetTexture(CTexture* pTex)
//{
//	if (m_pTexture) m_pTexture->Release();
//	m_pTexture = pTex;
//	if (m_pTexture) m_pTexture->AddRef();
//}

void CMaterial::SetMaterialColors(CMaterialColors* pMaterialColors)
{
	if (m_pMaterialColors) m_pMaterialColors->Release();
	m_pMaterialColors = pMaterialColors;
	if (m_pMaterialColors) m_pMaterialColors->AddRef();
}

void CMaterial::SetShader(CShader* pShader)
{
	if (m_pShader) m_pShader->Release();
	m_pShader = pShader;
	if (m_pShader) m_pShader->AddRef();
}

void CMaterial::PrepareShaders(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	m_pIlluminatedShader = new CIlluminatedShader();
	m_pIlluminatedShader->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_pIlluminatedShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void CMaterial::UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList)
{
	//if (m_pTexture)
	//	m_pTexture->UpdateShaderVariables(pd3dCommandList);
	//
	//if(m_pMaterialColors)
	//{
		pd3dCommandList->SetGraphicsRoot32BitConstants(1, 4, &(m_pMaterialColors->m_xmf4Ambient), 16);
		pd3dCommandList->SetGraphicsRoot32BitConstants(1, 4, &(m_pMaterialColors->m_xmf4Diffuse), 20);
		pd3dCommandList->SetGraphicsRoot32BitConstants(1, 4, &(m_pMaterialColors->m_xmf4Specular), 24);
		pd3dCommandList->SetGraphicsRoot32BitConstants(1, 4, &(m_pMaterialColors->m_xmf4Emissive), 28);
	//}
}

/*
void CMaterial::ReleaseShaderVariables()
{
	if (m_pShader)
		m_pShader->ReleaseShaderVariables();
	if (m_pTexture)
		m_pTexture->ReleaseShaderVariables();
}

void CMaterial::ReleaseUploadBuffers()
{
	if (m_pTexture)
		m_pTexture->ReleaseUploadBuffers();
}


//=====================================

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


CGameObject::CGameObject(int nMeshes, int nMaterials)
{
	m_xmf4x4World = Matrix4x4::Identity();
	m_nMeshes = nMeshes;
	m_ppMeshes = NULL;
	if (m_nMeshes > 0)
	{
		m_ppMeshes = new CMesh * [m_nMeshes];
		for (int i = 0; i < m_nMeshes; i++) m_ppMeshes[i] = NULL;
	}

	m_nMaterials = nMaterials;
	if (m_nMaterials > 0)
	{
		m_ppMaterials = new CMaterial * [m_nMaterials];
		for (int i = 0; i < m_nMaterials; i++) m_ppMaterials[i] = NULL;
	}
}
*/

CGameObject::CGameObject()
{
	m_xmf4x4Transform = Matrix4x4::Identity();
	m_xmf4x4World = Matrix4x4::Identity();
}

CGameObject::~CGameObject()
{
	ReleaseShaderVariables();

	//if (m_ppMeshes)
	//{
	//	for (int i = 0; i < m_nMeshes; i++)
	//	{
	//		if (m_ppMeshes[i]) m_ppMeshes[i]->Release();
	//		m_ppMeshes[i] = NULL;
	//	}
	//	delete[] m_ppMeshes;
	//}
	if (m_pMesh) 
		m_pMesh->Release();

	if (m_nMaterials > 0)
	{
		for (int i = 0; i < m_nMaterials; i++)
		{
			if (m_ppMaterials[i]) m_ppMaterials[i]->Release();
		}
	}
	if (m_ppMaterials)
	{
		//delete m_ppMaterials[0];
		delete[] m_ppMaterials;
	}
}

void CGameObject::SetMaterial(int nMaterial, CMaterial* pMaterial)
{
	if (m_ppMaterials[nMaterial]) 
		m_ppMaterials[nMaterial]->Release();
	m_ppMaterials[nMaterial] = pMaterial;
	if (m_ppMaterials[nMaterial]) 
		m_ppMaterials[nMaterial]->AddRef();
}

void CGameObject::SetMesh(CMesh* pMesh)
{
	//if (m_ppMeshes)
	//{
	//	if (m_ppMeshes[nIndex]) m_ppMeshes[nIndex]->Release();
	//	m_ppMeshes[nIndex] = pMesh;
	//	if (pMesh) pMesh->AddRef();
	//}
	if (m_pMesh) m_pMesh->Release();
	m_pMesh = pMesh;
	if (m_pMesh) m_pMesh->AddRef();
}

void CGameObject::SetShader(CShader* pShader)
{
	//if (!m_ppMaterials)
	//{
	//	m_nMaterials = 1;
	//	m_ppMaterials = new CMaterial * [m_nMaterials];
	//	CMaterial* pMaterial = new CMaterial();
	//	SetMaterial(pMaterial);
	//}
	//if(m_ppMaterials)
	//	m_ppMaterials[0]->SetShader(pShader);
	m_nMaterials = 1;
	m_ppMaterials = new CMaterial * [m_nMaterials];
	m_ppMaterials[0] = new CMaterial();
	m_ppMaterials[0]->SetShader(pShader);
}

void CGameObject::SetShader(int nMaterial, CShader* pShader)
{
	if (m_ppMaterials[nMaterial]) 
		m_ppMaterials[nMaterial]->SetShader(pShader);
}

void CGameObject::ReleaseUploadBuffers()
{
	//정점 버퍼를 위한 업로드 버퍼를 소멸시킨다. 
	//if (m_ppMeshes)
	//{
	//	for (int i = 0; i < m_nMeshes; i++)
	//	{
	//		if (m_ppMeshes[i])
	//			m_ppMeshes[i]->ReleaseUploadBuffers();
	//	}
	//}
	//for (int i = 0; i < m_nMaterials; i++)
	//{
	//	if (m_ppMaterials[i])
	//		m_ppMaterials[i]->ReleaseUploadBuffers();
	//}
	if (m_pMesh) m_pMesh->ReleaseUploadBuffers();

	if (m_pSibling) m_pSibling->ReleaseUploadBuffers();
	if (m_pChild) m_pChild->ReleaseUploadBuffers();
}

void CGameObject::Animate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent)
{
	if (m_pSibling) m_pSibling->Animate(fTimeElapsed, pxmf4x4Parent);
	if (m_pChild) m_pChild->Animate(fTimeElapsed, &m_xmf4x4World);
}


void CGameObject::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	OnPrepareRender();

	UpdateShaderVariable(pd3dCommandList, &m_xmf4x4World);

	//게임 객체에 셰이더 객체가 연결되어 있으면 셰이더 상태 객체를 설정한다. 
	if (m_nMaterials > 0)
	{
		for (int i = 0; i < m_nMaterials; i++)
		{
			if (m_ppMaterials[i])
			{
				if (m_ppMaterials[i]->m_pShader) m_ppMaterials[i]->m_pShader->Render(pd3dCommandList, pCamera);
				m_ppMaterials[i]->UpdateShaderVariable(pd3dCommandList);
			}

			if (m_pMesh) m_pMesh->Render(pd3dCommandList, i);
		}
	}
	if (m_pSibling) m_pSibling->Render(pd3dCommandList, pCamera);
	if (m_pChild) m_pChild->Render(pd3dCommandList, pCamera);

	//pd3dCommandList->SetGraphicsRootDescriptorTable(3, m_d3dCbvGPUDescriptorHandle);
	//
	////게임 객체에 메쉬가 연결되어 있으면 메쉬를 렌더링한다. 
	////게임 객체가 포함하는 모든 메쉬를 렌더링한다. 
	//if (m_ppMeshes)
	//{
	//	for (int i = 0; i < m_nMeshes; i++)
	//	{
	//		if (m_ppMeshes[i]) m_ppMeshes[i]->Render(pd3dCommandList);
	//	}
	//}
}

void CGameObject::Rotate(XMFLOAT3* pxmf3Axis, float fAngle)
{
	XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(pxmf3Axis),
		XMConvertToRadians(fAngle));
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
}

void CGameObject::Rotate(XMFLOAT4* pxmf4Quaternion)
{
	XMMATRIX mtxRotate = XMMatrixRotationQuaternion(XMLoadFloat4(pxmf4Quaternion));
	m_xmf4x4Transform = Matrix4x4::Multiply(mtxRotate, m_xmf4x4Transform);

	UpdateTransform(NULL);
}

void CGameObject::UpdateTransform(XMFLOAT4X4* pxmf4x4Parent)
{
	m_xmf4x4World = (pxmf4x4Parent) ? Matrix4x4::Multiply(m_xmf4x4Transform, *pxmf4x4Parent) : m_xmf4x4Transform;

	if (m_pSibling) m_pSibling->UpdateTransform(pxmf4x4Parent);
	if (m_pChild) m_pChild->UpdateTransform(&m_xmf4x4World);
}

CGameObject* CGameObject::FindFrame(char* pstrFrameName)
{
	CGameObject* pFrameObject = NULL;
	if (!strncmp(m_pstrFrameName, pstrFrameName, strlen(pstrFrameName))) return(this);

	if (m_pSibling) if (pFrameObject = m_pSibling->FindFrame(pstrFrameName)) return(pFrameObject);
	if (m_pChild) if (pFrameObject = m_pChild->FindFrame(pstrFrameName)) return(pFrameObject);

	return(NULL);
}

void CGameObject::AddRef()
{
	m_nReferences++;

	if (m_pSibling) m_pSibling->AddRef();
	if (m_pChild) m_pChild->AddRef();
}

void CGameObject::Release()
{
	if (m_pChild) m_pChild->Release();
	if (m_pSibling) m_pSibling->Release();

	if (--m_nReferences <= 0) delete this;
}

void CGameObject::SetChild(CGameObject* pChild, bool bReferenceUpdate)
{
	if (pChild)
	{
		pChild->m_pParent = this;
		if (bReferenceUpdate) pChild->AddRef();
	}
	if (m_pChild)
	{
		if (pChild) pChild->m_pSibling = m_pChild->m_pSibling;
		m_pChild->m_pSibling = pChild;
	}
	else
	{
		m_pChild = pChild;
	}
}

void CGameObject::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	//UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수
	//m_pd3dcbGameObject = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, //D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	//
	//m_pd3dcbGameObject->Map(0, NULL, (void**)&m_pcbMappedGameObject);
}

void CGameObject::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
}

void CGameObject::UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT4X4* pxmf4x4World)
{
	XMFLOAT4X4 xmf4x4World;
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(pxmf4x4World)));
	pd3dCommandList->SetGraphicsRoot32BitConstants(1, 16, &xmf4x4World, 0);
}

void CGameObject::UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, CMaterial* pMaterial)
{
}

void CGameObject::ReleaseShaderVariables()
{
	//if (m_pd3dcbGameObject)
	//{
	//	m_pd3dcbGameObject->Unmap(0, NULL);
	//	m_pd3dcbGameObject->Release();
	//}
	//if (m_ppMaterials && m_ppMaterials[0])
	//{
	//	m_ppMaterials[0]->ReleaseShaderVariables();
	//}
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

void CGameObject::SetScale(float x, float y, float z)
{
	XMMATRIX mtxScale = XMMatrixScaling(x, y, z);
	m_xmf4x4Transform = Matrix4x4::Multiply(mtxScale, m_xmf4x4Transform);

	UpdateTransform(NULL);
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

//void CGameObject::SetMaterial(CMaterial* pMaterial)
//{
//	if (pMaterial) pMaterial->Release();
//	pMaterial = pMaterial;
//	if (pMaterial) pMaterial->AddRef();
//}

CMaterialColors::CMaterialColors(MATERIALLOADINFO* pMaterialInfo)
{
	m_xmf4Diffuse = pMaterialInfo->m_xmf4AlbedoColor;
	m_xmf4Specular = pMaterialInfo->m_xmf4SpecularColor; //(r,g,b,a=power)
	m_xmf4Specular.w = (pMaterialInfo->m_fGlossiness * 255.0f);
	m_xmf4Emissive = pMaterialInfo->m_xmf4EmissiveColor;
}
int ReadIntegerFromFile(FILE* pInFile)
{
	int nValue = 0;
	UINT nReads = (UINT)::fread(&nValue, sizeof(int), 1, pInFile);
	return(nValue);
}

float ReadFloatFromFile(FILE* pInFile)
{
	float fValue = 0;
	UINT nReads = (UINT)::fread(&fValue, sizeof(float), 1, pInFile);
	return(fValue);
}

BYTE ReadStringFromFile(FILE* pInFile, char* pstrToken)
{
	BYTE nStrLength = 0;
	UINT nReads = 0;
	nReads = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, pInFile);
	nReads = (UINT)::fread(pstrToken, sizeof(char), nStrLength, pInFile);
	pstrToken[nStrLength] = '\0';

	return(nStrLength);
}

#define _WITH_DEBUG_FRAME_HIERARCHY

CMeshLoadInfo* CGameObject::LoadMeshInfoFromFile(FILE* pInFile)
{
	char pstrToken[64] = { '\0' };
	UINT nReads = 0;

	int nPositions = 0, nColors = 0, nNormals = 0, nIndices = 0, nSubMeshes = 0, nSubIndices = 0;

	CMeshLoadInfo* pMeshInfo = new CMeshLoadInfo;

	pMeshInfo->m_nVertices = ::ReadIntegerFromFile(pInFile);
	::ReadStringFromFile(pInFile, pMeshInfo->m_pstrMeshName);

	for (; ; )
	{
		::ReadStringFromFile(pInFile, pstrToken);

		if (!strcmp(pstrToken, "<Bounds>:"))
		{
			nReads = (UINT)::fread(&(pMeshInfo->m_xmf3AABBCenter), sizeof(XMFLOAT3), 1, pInFile);
			nReads = (UINT)::fread(&(pMeshInfo->m_xmf3AABBExtents), sizeof(XMFLOAT3), 1, pInFile);
		}
		else if (!strcmp(pstrToken, "<Positions>:"))
		{
			nPositions = ::ReadIntegerFromFile(pInFile);
			if (nPositions > 0)
			{
				pMeshInfo->m_nType |= VERTEXT_POSITION;
				pMeshInfo->m_pxmf3Positions = new XMFLOAT3[nPositions];
				nReads = (UINT)::fread(pMeshInfo->m_pxmf3Positions, sizeof(XMFLOAT3), nPositions, pInFile);
			}
		}
		else if (!strcmp(pstrToken, "<Colors>:"))
		{
			nColors = ::ReadIntegerFromFile(pInFile);
			if (nColors > 0)
			{
				pMeshInfo->m_nType |= VERTEXT_COLOR;
				pMeshInfo->m_pxmf4Colors = new XMFLOAT4[nColors];
				nReads = (UINT)::fread(pMeshInfo->m_pxmf4Colors, sizeof(XMFLOAT4), nColors, pInFile);
			}
		}
		else if (!strcmp(pstrToken, "<Normals>:"))
		{
			nNormals = ::ReadIntegerFromFile(pInFile);
			if (nNormals > 0)
			{
				pMeshInfo->m_nType |= VERTEXT_NORMAL;
				pMeshInfo->m_pxmf3Normals = new XMFLOAT3[nNormals];
				nReads = (UINT)::fread(pMeshInfo->m_pxmf3Normals, sizeof(XMFLOAT3), nNormals, pInFile);
			}
		}
		else if (!strcmp(pstrToken, "<Indices>:"))
		{
			nIndices = ::ReadIntegerFromFile(pInFile);
			if (nIndices > 0)
			{
				pMeshInfo->m_pnIndices = new UINT[nIndices];
				nReads = (UINT)::fread(pMeshInfo->m_pnIndices, sizeof(int), nIndices, pInFile);
			}
		}
		else if (!strcmp(pstrToken, "<SubMeshes>:"))
		{
			pMeshInfo->m_nSubMeshes = ::ReadIntegerFromFile(pInFile);
			if (pMeshInfo->m_nSubMeshes > 0)
			{
				pMeshInfo->m_pnSubSetIndices = new int[pMeshInfo->m_nSubMeshes];
				pMeshInfo->m_ppnSubSetIndices = new UINT * [pMeshInfo->m_nSubMeshes];
				for (int i = 0; i < pMeshInfo->m_nSubMeshes; i++)
				{
					::ReadStringFromFile(pInFile, pstrToken);
					if (!strcmp(pstrToken, "<SubMesh>:"))
					{
						int nIndex = ::ReadIntegerFromFile(pInFile);
						pMeshInfo->m_pnSubSetIndices[i] = ::ReadIntegerFromFile(pInFile);
						if (pMeshInfo->m_pnSubSetIndices[i] > 0)
						{
							pMeshInfo->m_ppnSubSetIndices[i] = new UINT[pMeshInfo->m_pnSubSetIndices[i]];
							nReads = (UINT)::fread(pMeshInfo->m_ppnSubSetIndices[i], sizeof(int), pMeshInfo->m_pnSubSetIndices[i], pInFile);
						}

					}
				}
			}
		}
		else if (!strcmp(pstrToken, "</Mesh>"))
		{
			break;
		}
	}
	return(pMeshInfo);
}

MATERIALSLOADINFO* CGameObject::LoadMaterialsInfoFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, FILE* pInFile)
{
	char pstrToken[64] = { '\0' };
	UINT nReads = 0;

	int nMaterial = 0;

	MATERIALSLOADINFO* pMaterialsInfo = new MATERIALSLOADINFO;

	pMaterialsInfo->m_nMaterials = ::ReadIntegerFromFile(pInFile);
	pMaterialsInfo->m_pMaterials = new MATERIALLOADINFO[pMaterialsInfo->m_nMaterials];

	for (; ; )
	{
		::ReadStringFromFile(pInFile, pstrToken);

		if (!strcmp(pstrToken, "<Material>:"))
		{
			nMaterial = ::ReadIntegerFromFile(pInFile);
		}
		else if (!strcmp(pstrToken, "<AlbedoColor>:"))
		{
			nReads = (UINT)::fread(&(pMaterialsInfo->m_pMaterials[nMaterial].m_xmf4AlbedoColor), sizeof(float), 4, pInFile);
		}
		else if (!strcmp(pstrToken, "<EmissiveColor>:"))
		{
			nReads = (UINT)::fread(&(pMaterialsInfo->m_pMaterials[nMaterial].m_xmf4EmissiveColor), sizeof(float), 4, pInFile);
		}
		else if (!strcmp(pstrToken, "<SpecularColor>:"))
		{
			nReads = (UINT)::fread(&(pMaterialsInfo->m_pMaterials[nMaterial].m_xmf4SpecularColor), sizeof(float), 4, pInFile);
		}
		else if (!strcmp(pstrToken, "<Glossiness>:"))
		{
			nReads = (UINT)::fread(&(pMaterialsInfo->m_pMaterials[nMaterial].m_fGlossiness), sizeof(float), 1, pInFile);
		}
		else if (!strcmp(pstrToken, "<Smoothness>:"))
		{
			nReads = (UINT)::fread(&(pMaterialsInfo->m_pMaterials[nMaterial].m_fSmoothness), sizeof(float), 1, pInFile);
		}
		else if (!strcmp(pstrToken, "<Metallic>:"))
		{
			nReads = (UINT)::fread(&(pMaterialsInfo->m_pMaterials[nMaterial].m_fSpecularHighlight), sizeof(float), 1, pInFile);
		}
		else if (!strcmp(pstrToken, "<SpecularHighlight>:"))
		{
			nReads = (UINT)::fread(&(pMaterialsInfo->m_pMaterials[nMaterial].m_fMetallic), sizeof(float), 1, pInFile);
		}
		else if (!strcmp(pstrToken, "<GlossyReflection>:"))
		{
			nReads = (UINT)::fread(&(pMaterialsInfo->m_pMaterials[nMaterial].m_fGlossyReflection), sizeof(float), 1, pInFile);
		}
		else if (!strcmp(pstrToken, "</Materials>"))
		{
			break;
		}
	}
	return(pMaterialsInfo);
}

CGameObject* CGameObject::LoadFrameHierarchyFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, FILE* pInFile)
{
	char pstrToken[64] = { '\0' };
	UINT nReads = 0;

	int nFrame = 0;

	CGameObject* pGameObject = NULL;

	for (; ; )
	{
		::ReadStringFromFile(pInFile, pstrToken);
		if (!strcmp(pstrToken, "<Frame>:"))
		{
			pGameObject = new CGameObject();

			nFrame = ::ReadIntegerFromFile(pInFile);
			::ReadStringFromFile(pInFile, pGameObject->m_pstrFrameName);
		}
		else if (!strcmp(pstrToken, "<Transform>:"))
		{
			XMFLOAT3 xmf3Position, xmf3Rotation, xmf3Scale;
			XMFLOAT4 xmf4Rotation;
			nReads = (UINT)::fread(&xmf3Position, sizeof(float), 3, pInFile);
			nReads = (UINT)::fread(&xmf3Rotation, sizeof(float), 3, pInFile); //Euler Angle
			nReads = (UINT)::fread(&xmf3Scale, sizeof(float), 3, pInFile);
			nReads = (UINT)::fread(&xmf4Rotation, sizeof(float), 4, pInFile); //Quaternion
		}
		else if (!strcmp(pstrToken, "<TransformMatrix>:"))
		{
			nReads = (UINT)::fread(&pGameObject->m_xmf4x4Transform, sizeof(float), 16, pInFile);
		}
		else if (!strcmp(pstrToken, "<Mesh>:"))
		{
			CMeshLoadInfo* pMeshInfo = pGameObject->LoadMeshInfoFromFile(pInFile);
			if (pMeshInfo)
			{
				CMesh* pMesh = NULL;
				if (pMeshInfo->m_nType & VERTEXT_NORMAL)
				{
					pMesh = new CMeshIlluminatedFromFile(pd3dDevice, pd3dCommandList, pMeshInfo);
				}
				if (pMesh) pGameObject->SetMesh(pMesh);
				delete pMeshInfo;
			}
		}
		else if (!strcmp(pstrToken, "<Materials>:"))
		{
			MATERIALSLOADINFO* pMaterialsInfo = pGameObject->LoadMaterialsInfoFromFile(pd3dDevice, pd3dCommandList, pInFile);
			if (pMaterialsInfo && (pMaterialsInfo->m_nMaterials > 0))
			{
				pGameObject->m_nMaterials = pMaterialsInfo->m_nMaterials;
				pGameObject->m_ppMaterials = new CMaterial * [pMaterialsInfo->m_nMaterials];

				for (int i = 0; i < pMaterialsInfo->m_nMaterials; i++)
				{
					pGameObject->m_ppMaterials[i] = NULL;

					CMaterial* pMaterial = new CMaterial();

					CMaterialColors* pMaterialColors = new CMaterialColors(&pMaterialsInfo->m_pMaterials[i]);
					pMaterial->SetMaterialColors(pMaterialColors);

					if (pGameObject->GetMeshType() & VERTEXT_NORMAL) pMaterial->SetIlluminatedShader();

					pGameObject->SetMaterial(i, pMaterial);
				}
			}
		}
		else if (!strcmp(pstrToken, "<Children>:"))
		{
			int nChilds = ::ReadIntegerFromFile(pInFile);
			if (nChilds > 0)
			{
				for (int i = 0; i < nChilds; i++)
				{
					CGameObject* pChild = CGameObject::LoadFrameHierarchyFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, pInFile);
					if (pChild) pGameObject->SetChild(pChild);
#ifdef _WITH_DEBUG_RUNTIME_FRAME_HIERARCHY
					TCHAR pstrDebug[256] = { 0 };
					_stprintf_s(pstrDebug, 256, _T("(Child Frame: %p) (Parent Frame: %p)\n"), pChild, pGameObject);
					OutputDebugString(pstrDebug);
#endif
				}
			}
		}
		else if (!strcmp(pstrToken, "</Frame>"))
		{
			break;
		}
	}
	return(pGameObject);
}

void CGameObject::PrintFrameInfo(CGameObject* pGameObject, CGameObject* pParent)
{
	TCHAR pstrDebug[256] = { 0 };

	_stprintf_s(pstrDebug, 256, _T("(Frame: %p) (Parent: %p)\n"), pGameObject, pParent);
	OutputDebugString(pstrDebug);

	if (pGameObject->m_pSibling) CGameObject::PrintFrameInfo(pGameObject->m_pSibling, pParent);
	if (pGameObject->m_pChild) CGameObject::PrintFrameInfo(pGameObject->m_pChild, pGameObject);
}

CGameObject* CGameObject::LoadGeometryFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, const char* pstrFileName)
{
	FILE* pInFile = NULL;
	::fopen_s(&pInFile, pstrFileName, "rb");
	::rewind(pInFile);

	CGameObject* pGameObject = NULL;
	char pstrToken[64] = { '\0' };

	for (; ; )
	{
		::ReadStringFromFile(pInFile, pstrToken);

		if (!strcmp(pstrToken, "<Hierarchy>:"))
		{
			pGameObject = CGameObject::LoadFrameHierarchyFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, pInFile);
		}
		else if (!strcmp(pstrToken, "</Hierarchy>"))
		{
			break;
		}
	}

#ifdef _WITH_DEBUG_FRAME_HIERARCHY
	TCHAR pstrDebug[256] = { 0 };
	_stprintf_s(pstrDebug, 256, _T("Frame Hierarchy\n"));
	OutputDebugString(pstrDebug);

	CGameObject::PrintFrameInfo(pGameObject, NULL);
#endif

	return(pGameObject);
}

/*
CMapObject::CMapObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, void* pContext, int nMeshes) : CGameObject(1,0)
{
	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	CGameObject* pMap = CGameObject::LoadGeometryFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Map.bin");

	pMap->SetPosition(0.0f, 0.0f, 0.0f);
	SetChild(pMap, true);
	//SetMesh(0, pMap);
	//SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수

	CIlluminatedShader* pShader = new CIlluminatedShader();
	pShader->CreateShader(pd3dDevice, pd3dCommandList,pd3dGraphicsRootSignature);
	pShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);// ++추가코드
	pShader->CreateCbvSrvDescriptorHeaps(pd3dDevice, 1, 0);// ++추가코드
	pShader->CreateConstantBufferViews(pd3dDevice, 1, m_pd3dcbGameObject, ncbElementBytes);// ++추가코드

	SetCbvGPUDescriptorHandle(pShader->GetCbvGPUDescStartHandle());// ++추가코드

	SetShader(pShader);
}
*/