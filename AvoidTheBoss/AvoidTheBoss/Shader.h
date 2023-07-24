//-----------------------------------------------------------------------------
// File: Shader.h
//-----------------------------------------------------------------------------

#pragma once

#include "GameObject.h"
#include "Camera.h"

class CShader
{
public:
	CShader();
	virtual ~CShader();

private:
	int									m_nReferences = 0;

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_RASTERIZER_DESC CreateRasterizerState();
	virtual D3D12_BLEND_DESC CreateBlendState();
	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader();
	virtual D3D12_SHADER_BYTECODE CreatePixelShader();

	D3D12_SHADER_BYTECODE CompileShaderFromFile(const WCHAR *pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderProfile, ID3DBlob **ppd3dShaderBlob);
	D3D12_SHADER_BYTECODE ReadCompiledShaderFromFile(WCHAR *pszFileName, ID3DBlob **ppd3dShaderBlob=NULL);

	virtual void CreateShader(ID3D12Device5 *pd3dDevice, 
		ID3D12GraphicsCommandList4
		*pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature);

	virtual void CreateShaderVariables(ID3D12Device5 *pd3dDevice, ID3D12GraphicsCommandList4  *pd3dCommandList) { }
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList4  *pd3dCommandList) { }
	virtual void ReleaseShaderVariables() { }

	virtual void UpdateShaderVariable(ID3D12GraphicsCommandList4  *pd3dCommandList, XMFLOAT4X4 *pxmf4x4World) { }

	virtual void OnPrepareRender(ID3D12GraphicsCommandList4  *pd3dCommandList, int nPipelineState=0);
	virtual void Render(ID3D12GraphicsCommandList4  *pd3dCommandList, CCamera *pCamera, bool bRaster);

	virtual void ReleaseUploadBuffers() { }

	virtual void BuildObjects(ID3D12Device5 *pd3dDevice, ID3D12GraphicsCommandList4  *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, CLoadedModelInfo *pModel, void *pContext = NULL) { }
	virtual void AnimateObjects(float fTimeElapsed) { }
	virtual void ReleaseObjects() { }

protected:
	ID3DBlob							*m_pd3dVertexShaderBlob = NULL;
	ID3DBlob							*m_pd3dPixelShaderBlob = NULL;

	ID3D12PipelineState					*m_pd3dPipelineState = NULL;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC	m_d3dPipelineStateDesc;

	float								m_fElapsedTime = 0.0f;
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CSkyBoxShader : public CShader
{
public:
	CSkyBoxShader();
	virtual ~CSkyBoxShader();

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader();
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(int16 playerIdx);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CStandardShader : public CShader
{
public:
	CStandardShader();
	virtual ~CStandardShader();

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader();
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(int16 playerIdx);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CStandardObjectsShader : public CStandardShader
{
public:
	CStandardObjectsShader();
	virtual ~CStandardObjectsShader();

	virtual void BuildObjects(ID3D12Device5 *pd3dDevice, ID3D12GraphicsCommandList4  *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, CLoadedModelInfo *pModel, void *pContext = NULL);
	virtual void AnimateObjects(float fTimeElapsed);
	virtual void ReleaseObjects();

	virtual void ReleaseUploadBuffers();

	virtual void Render(ID3D12GraphicsCommandList4  *pd3dCommandList, CCamera *pCamera,bool bRaster);

	CGameObject						**m_ppObjects = 0;
	int								m_nObjects = 0;
};

class CMapObjectsShader : public CStandardObjectsShader
{
public:
	CMapObjectsShader();
	virtual ~CMapObjectsShader();

	virtual void BuildObjects(ID3D12Device5* pd3dDevice, ID3D12GraphicsCommandList4 * pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CLoadedModelInfo* pModel, void* pContext = NULL);
};

class CBoundsObjectsShader : public CStandardObjectsShader
{
public:
	CBoundsObjectsShader();
	virtual ~CBoundsObjectsShader();

	virtual void BuildObjects(ID3D12Device5* pd3dDevice, ID3D12GraphicsCommandList4 * pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CLoadedModelInfo* pModel, void* pContext = NULL);
};

class CBulletObjectsShader : public CStandardObjectsShader
{
public:
	CBulletObjectsShader();
	virtual ~CBulletObjectsShader();

	virtual void BuildObjects(ID3D12Device5* pd3dDevice, ID3D12GraphicsCommandList4 * pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CLoadedModelInfo* pModel, void* pContext = NULL);
	virtual void Render(ID3D12GraphicsCommandList4 * pd3dCommandList, CCamera* pCamera, bool bRaster);
};

class CDoorObjectsShader : public CStandardObjectsShader
{
public:
	CDoorObjectsShader();
	virtual ~CDoorObjectsShader();

	virtual void BuildObjects(ID3D12Device5* pd3dDevice, ID3D12GraphicsCommandList4 * pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CLoadedModelInfo* pModel, void* pContext = NULL);
};

class CSirenObjectsShader : public CStandardObjectsShader
{
public:
	CSirenObjectsShader();
	virtual ~CSirenObjectsShader();

	virtual void BuildObjects(ID3D12Device5* pd3dDevice, ID3D12GraphicsCommandList4 * pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CLoadedModelInfo* pModel, void* pContext = NULL);
};

class CGeneratorObjectsShader : public CStandardObjectsShader
{
public:
	CGeneratorObjectsShader();
	virtual ~CGeneratorObjectsShader();

	virtual void BuildObjects(ID3D12Device5* pd3dDevice, ID3D12GraphicsCommandList4* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CLoadedModelInfo* pModel, void* pContext = NULL);
	//virtual void Render(ID3D12GraphicsCommandList4* pd3dCommandList, CCamera* pCamera, bool bRaster);
};

class CVirtualObjectsShader : public CStandardObjectsShader
{
public:
	CVirtualObjectsShader();
	virtual ~CVirtualObjectsShader();

	virtual void BuildObjects(ID3D12Device5* pd3dDevice, ID3D12GraphicsCommandList4* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CLoadedModelInfo* pModel, void* pContext = NULL);
};

class CHitEffectObjectsShader : public CStandardObjectsShader
{
public:
	CHitEffectObjectsShader();
	virtual ~CHitEffectObjectsShader();

	virtual void BuildObjects(ID3D12Device5* pd3dDevice, ID3D12GraphicsCommandList4* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CLoadedModelInfo* pModel, void* pContext = NULL);

	virtual void Render(ID3D12GraphicsCommandList4* pd3dCommandList, CCamera* pCamera, bool bRaster);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CSkinnedAnimationStandardShader : public CStandardShader
{
public:
	CSkinnedAnimationStandardShader();
	virtual ~CSkinnedAnimationStandardShader();

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_SHADER_BYTECODE CreateVertexShader();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CSkinnedAnimationObjectsShader : public CSkinnedAnimationStandardShader
{
public:
	CSkinnedAnimationObjectsShader();
	virtual ~CSkinnedAnimationObjectsShader();

	virtual void BuildObjects(ID3D12Device5 *pd3dDevice, ID3D12GraphicsCommandList4  *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, CLoadedModelInfo *pModel, void *pContext = NULL);
	virtual void AnimateObjects(float fTimeElapsed);
	virtual void ReleaseObjects();

	virtual void ReleaseUploadBuffers();

	virtual void Render(ID3D12GraphicsCommandList4  *pd3dCommandList, CCamera *pCamera,bool bRaster);

protected:
	CGameObject						**m_ppObjects = 0;
	int								m_nObjects = 0;
};
