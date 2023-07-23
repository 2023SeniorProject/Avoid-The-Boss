#pragma once

class CGameObject;
class BinParser
{
private:
	CGameObject* m_pTexturedMeshObject;
public:
	BinParser(){}
	~BinParser(){}

	void RasterDataStructureAlignmentToDXR();
	
	void LoadBinScene(ID3D12Device5* pd3dDevice, ID3D12GraphicsCommandList4* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, const char* pstrFileName);
};
