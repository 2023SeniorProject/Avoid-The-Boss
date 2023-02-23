#pragma once

#include "Mesh.h"
#include "Camera.h"

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
	ID3D12Resource** m_ppTexture;
	ID3D12Resource** m_ppUploadBuffer;

	UINT* m_pnResourceTypes = NULL;
	UINT m_pTextureType;
	_TCHAR(*m_pTextureName)[64];

	int m_nTexUploadBuf;
	ID3D12Resource** m_ppd3dTexUploadBuffers;

	int m_nRootParameters = 0;
	int* m_pnRootParamIndices = NULL;
	D3D12_GPU_DESCRIPTOR_HANDLE* m_pSrvDescriptorHandles;

	int m_nSamplers;
	D3D12_GPU_DESCRIPTOR_HANDLE* m_pSamplerDescriptorHandles;

	//DXGI_FORMAT* m_pdxgiBufferFormats = NULL;
	//int* m_pnBufferElements = NULL;

public:
	CTexture(int nTexture, UINT nTextureType, int nRootParameters, int nSamplers);
	virtual ~CTexture();

	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }
	void ReleaseShaderVariables();
	void ReleaseUploadBuffers();

	ID3D12Resource* LoadTextureFromDDSFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, const wchar_t* pszFileName, UINT nResourceType, UINT nIndex);

	D3D12_SHADER_RESOURCE_VIEW_DESC GetShaderResourceViewDesc(int nIndex);

	int GetTextureNum() { return m_nTexture; }
	ID3D12Resource** GetppTexture() { return m_ppTexture; }
	ID3D12Resource* GetpTexture(int index) { return m_ppTexture[index]; }

	void SetSrvGPUDescHandles(int index, D3D12_GPU_DESCRIPTOR_HANDLE hGpuDesc) { m_pSrvDescriptorHandles[index] = hGpuDesc; }

	void SetRootParamIndexBuf(int index, UINT nRootParam) { m_pnRootParamIndices[index] = nRootParam; }
	int GetRootParamNum() { return m_nRootParameters; }

	//��Ʈ ������ ���� �ؽ��� �ڿ� �Է�
	void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList, int nRootParam, int nTexIndex);
};




class CMaterial
{
private:
	int DiffuseSrvHeapIndex = -1;
	
	int m_nReferences = 0;
public:
	CMaterial();
	virtual ~CMaterial();

	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }
public:
	CTexture* m_pTexture=NULL;
	CShader* m_pShader=NULL;

	void SetTexture(CTexture* pTex) { m_pTexture = pTex; }
	void SetShader(CShader* pShader) { m_pShader = pShader; }

	XMFLOAT3 m_xmfAlbedo;
	void SetAlbedo(XMFLOAT3 albedo) { m_xmfAlbedo = albedo; }

	void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	void ReleaseShaderVariables();

	void ReleaseUploadBuffers();
};


class CGameObject
{
public:
	CGameObject();
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

	virtual void Animate(float fTimeElapsed);

	virtual void OnPrepareRender();
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);

	void Rotate(XMFLOAT3* pxmf3Axis, float fAngle);

public:
	//��� ���۸� �����Ѵ�. 
	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList
		* pd3dCommandList);
	//��� ������ ������ �����Ѵ�. 
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseShaderVariables();

	//���� ��ü�� ���� ��ȯ ��Ŀ��� ��ġ ���Ϳ� ����(x-��, y-��, z-��) ���͸� ��ȯ�Ѵ�. 
	XMFLOAT3 GetPosition();
	XMFLOAT3 GetLook();
	XMFLOAT3 GetUp();
	XMFLOAT3 GetRight();

	//XMFLOAT4X4 GetMat4x4World() { return m_xmf4x4World; }
	//���� ��ü�� ��ġ�� �����Ѵ�. 
	void SetPosition(float x, float y, float z);
	void SetPosition(XMFLOAT3 xmf3Position);
	void SetObjectInWorld(CGameObject** ppObjects, int i, XMFLOAT3 position, CMesh* pMesh, CMaterial* pMaterial, D3D12_GPU_DESCRIPTOR_HANDLE m_d3dCbvGPUDescriptorStartHandle, int nIndex);
	//���� ��ü�� ���� x-��, y-��, z-�� �������� �̵��Ѵ�.
	void MoveStrafe(float fDistance = 1.0f);
	void MoveUp(float fDistance = 1.0f);
	void MoveForward(float fDistance = 1.0f);

	//���� ��ü�� ȸ��(x-��, y-��, z-��)�Ѵ�. 
	void Rotate(float fPitch = 10.0f, float fYaw = 10.0f, float fRoll = 10.0f);
};


