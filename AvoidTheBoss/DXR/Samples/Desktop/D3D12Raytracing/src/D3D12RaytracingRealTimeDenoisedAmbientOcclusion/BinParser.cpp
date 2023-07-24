#include "stdafx.h"
#include "BinParser.h"

BinParser::BinParser()
{
}

BinParser::~BinParser()
{
}

//void BinParser::LoadBinScene(ID3D12Device5* pd3dDevice, ID3D12GraphicsCommandList4* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, const char* pstrFileName)
//{
//	CGameObject::LoadGeometryFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, pstrFileName);
//}



void BinParser::RasterDataStructureAlignmentToDXR()
{
	//Mesh 정보 넘기기
	//m_pMesh.m_VertexBuffer.Position = m_pTexturedMeshObject.m_pd3dPositionBuffer;
	//m_pMesh.m_VertexBuffer.normal = m_pTexturedMeshObject.m_pd3dNormalBuffer;
	//m_pMesh.m_VertexBuffer.UV.x = m_pTexturedMeshObject.m_pd3dTextureCoord0Buffer;
	//m_pMesh.m_VertexBuffer.UV.y = m_pTexturedMeshObject.m_pd3dTextureCoord1Buffer;
	//m_pMesh.Tangent.Tangent = m_pTexturedMeshObjectm_pd3dTangentBuffer;
	//
	//m_pMesh.GenerateTangents();	
	//
	////SceneParser::Scene.Material.m_MaterialName = pMaterial->m_ppstrTextureNames[0;
	//m_pMaterial.DiffuseTextureFilename = m_pTexturedMeshObjectpMaterial->m_ppstrTextureNames[0];
	//m_pMaterial.SpecularTextureFilename = m_pTexturedMeshObjectpMaterial->m_ppstrTextureNames[1];
	//m_pMaterial.NormalMapTextureFilename = m_pTexturedMeshObjectpMaterial->m_ppstrTextureNames[2];
	//m_pMaterial.OpacityTextureFilename = m_pTexturedMeshObjectpMaterial->m_ppstrTextureNames[3];
	//
	//Materia.Initialize("matte");


}
