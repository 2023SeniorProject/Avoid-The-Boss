#pragma once

#include "GameObject.h"

//셰이더 소스 코드를 컴파일하고 그래픽스 상태 객체를 생성한다. 
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
		* pd3dRootSignature);

	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList
		* pd3dCommandList) { }
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList) { }
	virtual void ReleaseShaderVariables() { }

	virtual void ReleaseUploadBuffers();

	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList
		* pd3dCommandList, void* pContext = NULL);
	virtual void AnimateObjects(float fTimeElapsed);
	virtual void ReleaseObjects();

	virtual void OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList);

protected:
	//셰이더가 포함하는 게임 객체들의 리스트(배열)이다.
	CGameObject** m_ppObjects = NULL;
	int m_nObjects = 0;

	//파이프라인 상태 객체들의 리스트(배열)이다. 
	ID3D12PipelineState** m_ppd3dPipelineStates = NULL;
	int m_nPipelineStates = 0;
};
