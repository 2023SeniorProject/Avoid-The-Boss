#pragma once

#include "GameObject.h"
#include "Camera.h"

//���̴� �ҽ� �ڵ带 �������ϰ� �׷��Ƚ� ���� ��ü�� �����Ѵ�. 
class CShader
{
public:
	CShader();
	virtual ~CShader();

private:
	int m_nReferences = 0;

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_RASTERIZER_DESC CreateRasterizerState();
	virtual D3D12_BLEND_DESC CreateBlendState();
	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob** ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob** ppd3dShaderBlob);
	D3D12_SHADER_BYTECODE CompileShaderFromFile(const WCHAR* pszFileName, LPCSTR pszShaderName,
		LPCSTR pszShaderProfile, ID3DBlob** ppd3dShaderBlob);

	virtual void CreateShader(ID3D12Device* pd3dDevice, ID3D12RootSignature
		* pd3dGraphicsRootSignature);

	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList
		* pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseShaderVariables();

	virtual void UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList,
		XMFLOAT4X4* pxmf4x4World);

	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, void* pContext = NULL) { }
	virtual void OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);

	void CreateCbvSrvDescriptorHeaps(ID3D12Device* pd3dDevice, int nCbv, int nSrv);
	void CreateConstantBufferViews(ID3D12Device* pd3dDevice, int nConstantBufferViews, ID3D12Resource* pd3dConstantBuffers, UINT nStride);
	void CreateShaderResourceView(ID3D12Device* pd3dDevice, CTexture* pTexture, UINT nDescHeapIndex, UINT nRootParameStartIndex);

	D3D12_GPU_DESCRIPTOR_HANDLE GetCbvGPUDescStartHandle() { return m_d3dCbvGPUDescStartHandle; }
protected:
	//���������� ���� ��ü���� ����Ʈ(�迭)�̴�. 
	ID3D12PipelineState* m_pd3dPipelineState = NULL;
	int m_nPipelineStates = 0;

	ID3D12DescriptorHeap* m_pCbvSrvDescHeap;

	//---CPU�� cbv/srv �����ּ�
	D3D12_CPU_DESCRIPTOR_HANDLE m_d3dCbvCPUDescStartHandle;
	D3D12_CPU_DESCRIPTOR_HANDLE m_d3dSrvCPUDescStartHandle;

	D3D12_CPU_DESCRIPTOR_HANDLE m_d3dCbvCPUDescNextHandle;
	D3D12_CPU_DESCRIPTOR_HANDLE m_d3dSrvCPUDescNextHandle;

	//---GPU�� cbv/srv �����ּ�
	D3D12_GPU_DESCRIPTOR_HANDLE m_d3dCbvGPUDescStartHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE m_d3dSrvGPUDescStartHandle;

	D3D12_GPU_DESCRIPTOR_HANDLE m_d3dCbvGPUDescNextHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE m_d3dSrvGPUDescNextHandle;

	D3D12_SHADER_RESOURCE_VIEW_DESC m_SrvDescriptor;
};

class CPlayerShader : public CShader // CDiffusedShader���� CPlayerShader�� ����
{
public:
	CPlayerShader();
	virtual ~CPlayerShader();

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob** ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob** ppd3dShaderBlob);

	virtual void CreateShader(ID3D12Device* pd3dDevice, ID3D12RootSignature
		* pd3dGraphicsRootSignature);
};

class CTexturedShader : public CShader
{
public:
	CTexturedShader();
	virtual ~CTexturedShader();

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();

	virtual void CreateShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature);

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob** ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob** ppd3dShaderBlob);
};

#define _WITH_BATCH_MATERIAL
//��CObjectsShader�� Ŭ������ ���� ��ü���� �����ϴ� ���̴� ��ü�̴�. 
class CObjectsShader : public CTexturedShader
{
public:
	CObjectsShader();
	virtual ~CObjectsShader();

	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList
		* pd3dCommandList, void* pContext);
	virtual void AnimateObjects(float fTimeElapsed);
	virtual void ReleaseObjects();

	//virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	//virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob** ppd3dShaderBlob);
	//virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob** ppd3dShaderBlob);
	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);

	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);//�ؽ��� �߰�
	virtual void ReleaseShaderVariables();//�ؽ��� �߰�
	//virtual void CreateShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature);

	virtual void ReleaseUploadBuffers();

	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);

protected:
	CGameObject** m_ppObjects = NULL;
	int m_nObjects = 0;

	ID3D12Resource* m_pd3dcbGameObjects = NULL;
	CB_GAMEOBJECT_INFO* m_pcbMappedGameObjects = NULL;
};





class CRectShader : public CShader
{
public:
	CRectShader();
	virtual ~CRectShader();

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob** ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob** ppd3dShaderBlob);
};