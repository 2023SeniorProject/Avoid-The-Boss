#pragma once

#include "Mesh.h"
#include "Camera.h"

#define RESOURCE_TEXTURE2D			0x01
#define RESOURCE_TEXTURE2D_ARRAY	0x02	//[]
#define RESOURCE_TEXTURE2DARRAY		0x03
#define RESOURCE_TEXTURE_CUBE		0x04
#define RESOURCE_BUFFER				0x05


class CShader;

//���� ��ü�� ������ ���̴����� �Ѱ��ֱ� ���� ����ü(��� ����)�̴�. 
struct CB_GAMEOBJECT_INFO
{
	XMFLOAT4X4 m_xmf4x4World;
};

class CTexture
{
private:
	int m_nReferences = 0;

	int m_nTexture;
	ID3D12Resource** m_ppTexture = NULL;
	ID3D12Resource** m_ppUploadBuffer;

	_TCHAR(*m_pTextureName)[64] = NULL;
	UINT m_nTextureType;

	UINT* m_pnResourceTypes = NULL;

	DXGI_FORMAT* m_pdxgiBufferFormats = NULL;
	int* m_pnBufferElements = NULL;


	int m_nRootParameters = 0;
	int* m_pnRootParamIndices = NULL;
	D3D12_GPU_DESCRIPTOR_HANDLE* m_pSrvGPUDescHandles = NULL;

	int m_nSamplers = 0;
	D3D12_GPU_DESCRIPTOR_HANDLE* m_pSamplerGPUDescHandles = NULL;

public:
	CTexture(int nTexture, UINT nTextureType,  int nSamplers, int nRootParameters);
	virtual ~CTexture();

	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }
	void ReleaseShaderVariables();
	void ReleaseUploadBuffers();

	void LoadTextureFromDDSFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, const wchar_t* pszFileName, UINT nResourceType, UINT nIndex);

	D3D12_SHADER_RESOURCE_VIEW_DESC GetShaderResourceViewDesc(int nIndex);

	int GetTextureNum() { return m_nTexture; }
	ID3D12Resource* GetResource(int nIndex) { return(m_ppTexture[nIndex]); }

	void SetSrvGPUDescHandles(int index, D3D12_GPU_DESCRIPTOR_HANDLE hGpuDesc) { m_pSrvGPUDescHandles[index] = hGpuDesc; }
	D3D12_GPU_DESCRIPTOR_HANDLE GetGpuDescriptorHandle(int nIndex) { return(m_pSrvGPUDescHandles[nIndex]); }

	UINT GetTextureType() { return(m_nTextureType); }
	UINT GetTextureType(int nIndex) { return(m_pnResourceTypes[nIndex]); }

	void SetRootParamIndexBuf(int index, UINT nRootParam) { m_pnRootParamIndices[index] = nRootParam; }
	int GetRootParamNum() { return m_nRootParameters; }


	void SetSampler(int nIndex, D3D12_GPU_DESCRIPTOR_HANDLE d3dSamplerGpuDescriptorHandle) { m_pSamplerGPUDescHandles[nIndex] = d3dSamplerGpuDescriptorHandle; }

	//��Ʈ ������ ���� �ؽ��� �ڿ� �Է�
	void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	void UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, int nRootParam, int nTexIndex);
};




class CMaterial
{
private:
	//int DiffuseSrvHeapIndex = -1;
	
	int m_nReferences = 0;
public:
	CMaterial();
	virtual ~CMaterial();

	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

	CTexture* m_pTexture=NULL;
	CShader* m_pShader=NULL;

	XMFLOAT3 m_xmfAlbedo;
	void SetAlbedo(XMFLOAT3 albedo) { m_xmfAlbedo = albedo; }
	void SetTexture(CTexture* pTex);
	void SetShader(CShader* pShader);

	void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	void ReleaseShaderVariables();

	void ReleaseUploadBuffers();
};


class CGameObject
{
public:
	//CGameObject();
	CGameObject(int nMeshes = 1);
	virtual ~CGameObject();

	XMFLOAT4X4 m_xmf4x4World;

	//���� ��ü�� ���� ���� �޽��� �����ϴ� ��� ���� ��ü�� ������ �޽��鿡 ���� �����Ϳ� �� �����̴�.
	CMesh** m_ppMeshes = NULL;
	int m_nMeshes = 0;

	CMaterial* m_pMaterial = NULL;

	D3D12_GPU_DESCRIPTOR_HANDLE	m_d3dCbvGPUDescriptorHandle;

protected:
	ID3D12Resource* m_pd3dcbGameObject = NULL;
	CB_GAMEOBJECT_INFO* m_pcbMappedGameObject = NULL;

public:
	void ReleaseUploadBuffers();

	//virtual void SetMesh(CMesh* pMesh);
	void SetMesh(int nIndex, CMesh* pMesh);
	void SetShader(CShader* pShader);
	void SetMaterial(CMaterial* pMaterial);

	void SetCbvGPUDescriptorHandle(D3D12_GPU_DESCRIPTOR_HANDLE d3dCbvGPUDescriptorHandle) { m_d3dCbvGPUDescriptorHandle = d3dCbvGPUDescriptorHandle; }
	void SetCbvGPUDescriptorHandlePtr(UINT64 nCbvGPUDescriptorHandlePtr) { m_d3dCbvGPUDescriptorHandle.ptr = nCbvGPUDescriptorHandlePtr; }
	D3D12_GPU_DESCRIPTOR_HANDLE GetCbvGPUDescriptorHandle() { return(m_d3dCbvGPUDescriptorHandle); }

	//��� ���۸� �����Ѵ�. 
	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList
		* pd3dCommandList);
	virtual void ReleaseShaderVariables();
	//��� ������ ������ �����Ѵ�. 
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);


	virtual void Animate(float fTimeElapsed);
	virtual void OnPrepareRender() {}
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);

	virtual void BuildMaterials(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) { }

	//���� ��ü�� ���� ��ȯ ��Ŀ��� ��ġ ���Ϳ� ����(x-��, y-��, z-��) ���͸� ��ȯ�Ѵ�. 
	XMFLOAT3 GetPosition();
	XMFLOAT3 GetLook();
	XMFLOAT3 GetUp();
	XMFLOAT3 GetRight();

	//XMFLOAT4X4 GetMat4x4World() { return m_xmf4x4World; }
	//���� ��ü�� ��ġ�� �����Ѵ�. 
	void SetPosition(float x, float y, float z);
	void SetPosition(XMFLOAT3 xmf3Position);
	void SetObjectInWorld(int nIndex, CMesh* pMesh, CMaterial* pMaterial, XMFLOAT3 position);

	//���� ��ü�� ���� x-��, y-��, z-�� �������� �̵��Ѵ�.
	void MoveStrafe(float fDistance = 1.0f);
	void MoveUp(float fDistance = 1.0f);
	void MoveForward(float fDistance = 1.0f);

	//���� ��ü�� ȸ��(x-��, y-��, z-��)�Ѵ�. 
	void Rotate(float fPitch = 10.0f, float fYaw = 10.0f, float fRoll = 10.0f);
	void Rotate(XMFLOAT3* pxmf3Axis, float fAngle);
};


