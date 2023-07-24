#pragma once
#include "SceneParser.h"
#include "GameObject.h"

using namespace SceneParser;

class CGameObject;

class BinParser //: public SceneParser::SceneParserClass
{
private:
	Mesh m_pMesh;
	Material m_pMatarial;
	CGameObject* m_pTexturedMeshObject;
public:
	BinParser();
	~BinParser();

	void RasterDataStructureAlignmentToDXR();
	
	//void LoadBinScene(ID3D12Device5* pd3dDevice, ID3D12GraphicsCommandList4* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, const char* pstrFileName);
};
