#include "stdafx.h"
#include "Mesh.h"

CMesh::CMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
}
CMesh::~CMesh()
{
	if (m_pd3dVertexBuffer) m_pd3dVertexBuffer->Release();
	if (m_pd3dVertexUploadBuffer) m_pd3dVertexUploadBuffer->Release();

	if (m_pd3dIndexBuffer) m_pd3dIndexBuffer->Release();
	if (m_pd3dIndexUploadBuffer) m_pd3dIndexUploadBuffer->Release();
}

void CMesh::ReleaseUploadBuffers()
{
	//���� ���۸� ���� ���ε� ���۸� �Ҹ��Ų��. 
	if (m_pd3dVertexUploadBuffer) m_pd3dVertexUploadBuffer->Release();
	m_pd3dVertexUploadBuffer = NULL;

	if (m_pd3dIndexUploadBuffer) m_pd3dIndexUploadBuffer->Release();
	m_pd3dIndexUploadBuffer = NULL;
};

void CMesh::Render(ID3D12GraphicsCommandList* pd3dCommandList)
{
	//�޽��� ������Ƽ�� ������ �����Ѵ�. 
	pd3dCommandList->IASetPrimitiveTopology(m_d3dPrimitiveTopology);
	//�޽��� ���� ���� �並 �����Ѵ�. 
	pd3dCommandList->IASetVertexBuffers(m_nSlot, 1, &m_d3dVertexBufferView);

	if (m_pd3dIndexBuffer)
	{
		pd3dCommandList->IASetIndexBuffer(&m_d3dIndexBufferView);
		pd3dCommandList->DrawIndexedInstanced(m_nIndices, 1, 0, 0, 0);
		//�ε��� ���۰� ������ �ε��� ���۸� ����������(IA: �Է� ������)�� �����ϰ� �ε����� ����Ͽ� �������Ѵ�. 
	}
	else
	{
		//�޽��� ���� ���� �並 �������Ѵ�(����������(�Է� ������)�� �۵��ϰ� �Ѵ�).
		pd3dCommandList->DrawInstanced(m_nVertices, 1, m_nOffset, 0);
	}
}

//----�ﰢ�� �޽� ������
CTriangleMesh::CTriangleMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList
	* pd3dCommandList) : CMesh(pd3dDevice, pd3dCommandList)
{
	//�ﰢ�� �޽��� �����Ѵ�.
	m_nVertices = 3;
	m_nStride = sizeof(CDiffusedVertex);
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	/*����(�ﰢ���� ������)�� ������ �ð���� ������� ������, ���, �Ķ������� �����Ѵ�. RGBA(Red, Green, Blue,
	Alpha) 4���� �Ķ���͸� ����Ͽ� ������ ǥ���Ѵ�. �� �Ķ���ʹ� 0.0~1.0 ������ �Ǽ����� ������.*/
	CDiffusedVertex pVertices[3];

	//---������ġ, �ȼ���������� ������ ȣ��
	pVertices[0] = CDiffusedVertex(XMFLOAT3(0.0f, 0.5f, 0.0f), XMFLOAT4(1.0f, 0.0f, 0.0f,
		1.0f));
	pVertices[1] = CDiffusedVertex(XMFLOAT3(0.5f, -0.5f, 0.0f), XMFLOAT4(0.0f, 1.0f, 0.0f,
		1.0f));
	pVertices[2] = CDiffusedVertex(XMFLOAT3(-0.5f, -0.5f, 0.0f), XMFLOAT4(Colors::Blue));

	//�ﰢ�� �޽��� ���ҽ�(���� ����)�� �����Ѵ�.
	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices,
		m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

	//���� ���� �並 �����Ѵ�. 
	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;
}

CCubeMeshDiffused::CCubeMeshDiffused(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fWidth, float fHeight, float fDepth) : CMesh(pd3dDevice,
	pd3dCommandList)
{
	//====���� ����=====
	//������ü�� ������(����)�� 8���̴�. 
	m_nVertices = 8;
	m_nStride = sizeof(CDiffusedVertex);
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	float fx = fWidth * 0.5f, fy = fHeight * 0.5f, fz = fDepth * 0.5f;

	//���� ���۴� ������ü�� ������ 8���� ���� ���� �����͸� ������. 
	CDiffusedVertex pVertices[8];
	pVertices[0] = CDiffusedVertex(XMFLOAT3(-fx, +fy, -fz), RANDOM_COLOR);
	pVertices[1] = CDiffusedVertex(XMFLOAT3(+fx, +fy, -fz), RANDOM_COLOR);
	pVertices[2] = CDiffusedVertex(XMFLOAT3(+fx, +fy, +fz), RANDOM_COLOR);
	pVertices[3] = CDiffusedVertex(XMFLOAT3(-fx, +fy, +fz), RANDOM_COLOR);
	pVertices[4] = CDiffusedVertex(XMFLOAT3(-fx, -fy, -fz), RANDOM_COLOR);
	pVertices[5] = CDiffusedVertex(XMFLOAT3(+fx, -fy, -fz), RANDOM_COLOR);
	pVertices[6] = CDiffusedVertex(XMFLOAT3(+fx, -fy, +fz), RANDOM_COLOR);
	pVertices[7] = CDiffusedVertex(XMFLOAT3(-fx, -fy, +fz), RANDOM_COLOR);

	//���� ���� ���� (constant buffer)
	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices,
		m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

	//���� ���� �並 �����Ѵ�. 
	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;

	//====�ε��� ����====
	/*�ε��� ���۴� ������ü�� 6���� ��(�簢��)�� ���� ���� ������ ���´�. �ﰢ�� ����Ʈ�� ������ü�� ǥ���� ����
�Ƿ� �� ���� 2���� �ﰢ���� ������ �� �ﰢ���� 3���� ������ �ʿ��ϴ�. ��, �ε��� ���۴� ��ü 36(=6*2*3)���� ��
������ ������ �Ѵ�.*/
	m_nIndices = 36;

	UINT pnIndices[36];
	//�� �ո�(Front) �簢���� ���� �ﰢ��
	pnIndices[0] = 3; pnIndices[1] = 1; pnIndices[2] = 0;
	//�� �ո�(Front) �簢���� �Ʒ��� �ﰢ��
	pnIndices[3] = 2; pnIndices[4] = 1; pnIndices[5] = 3;
	//�� ����(Top) �簢���� ���� �ﰢ��
	pnIndices[6] = 0; pnIndices[7] = 5; pnIndices[8] = 4;
	//�� ����(Top) �簢���� �Ʒ��� �ﰢ��
	pnIndices[9] = 1; pnIndices[10] = 5; pnIndices[11] = 0;
	//�� �޸�(Back) �簢���� ���� �ﰢ��
	pnIndices[12] = 3; pnIndices[13] = 4; pnIndices[14] = 7;
	//�� �޸�(Back) �簢���� �Ʒ��� �ﰢ��
	pnIndices[15] = 0; pnIndices[16] = 4; pnIndices[17] = 3;
	//�� �Ʒ���(Bottom) �簢���� ���� �ﰢ��
	pnIndices[18] = 1; pnIndices[19] = 6; pnIndices[20] = 5;
	//�� �Ʒ���(Bottom) �簢���� �Ʒ��� �ﰢ��
	pnIndices[21] = 2; pnIndices[22] = 6; pnIndices[23] = 1;
	//�� ����(Left) �簢���� ���� �ﰢ��
	pnIndices[24] = 2; pnIndices[25] = 7; pnIndices[26] = 6;
	//�� ����(Left) �簢���� �Ʒ��� �ﰢ��
	pnIndices[27] = 3; pnIndices[28] = 7; pnIndices[29] = 2;
	//�� ����(Right) �簢���� ���� �ﰢ��
	pnIndices[30] = 6; pnIndices[31] = 4; pnIndices[32] = 5;
	//�� ����(Right) �簢���� �Ʒ��� �ﰢ��
	pnIndices[33] = 7; pnIndices[34] = 4; pnIndices[35] = 6;

	//�ε��� ���۸� �����Ѵ�.(index buffer)
	m_pd3dIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pnIndices,
		sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER,
		&m_pd3dIndexUploadBuffer);

	//�ε��� ���� �並 �����Ѵ�.
	m_d3dIndexBufferView.BufferLocation = m_pd3dIndexBuffer->GetGPUVirtualAddress();
	m_d3dIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_d3dIndexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices;
}

CCubeMeshDiffused::~CCubeMeshDiffused()
{
}


CRectangleMesh::CRectangleMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float nWidth, float nLength, float nScale) : CMesh(pd3dDevice, pd3dCommandList)
{
	m_nVertices = 4;
	m_nStride = sizeof(CDiffusedVertex);
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	CDiffusedVertex pVertices[4];
	pVertices[0] = CDiffusedVertex(XMFLOAT3((+nWidth * nScale), 0.0f, (+nLength * nScale)), DEFAULT_MAP_COLOR);
	pVertices[1] = CDiffusedVertex(XMFLOAT3((+nWidth * nScale), 0.0f, (-nLength * nScale)), MAP_COLOR_2);
	pVertices[2] = CDiffusedVertex(XMFLOAT3((-nWidth * nScale), 0.0f, (-nLength * nScale)), MAP_COLOR_3);
	pVertices[3] = CDiffusedVertex(XMFLOAT3((-nWidth * nScale), 0.0f, (+nLength * nScale)), MAP_COLOR_4);

	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices,
		m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;

	m_nIndices = 6;
	UINT pnIndices[6];
	// 3   \ �� 0
	// 2 �� \   1 (�簢�� ����)
	pnIndices[0] = 0;
	pnIndices[1] = 1;
	pnIndices[2] = 3;
	pnIndices[3] = 3;
	pnIndices[4] = 1;
	pnIndices[5] = 2;

	m_pd3dIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pnIndices, sizeof(CDiffusedVertex), D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_pd3dIndexUploadBuffer);

	m_d3dIndexBufferView.BufferLocation = m_pd3dIndexBuffer->GetGPUVirtualAddress();
	m_d3dIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_d3dIndexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices;
}

CRectangleMesh::~CRectangleMesh()
{
}



CCubeMeshTextured::CCubeMeshTextured(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fWidth, float fHeight, float fDepth) : CMesh(pd3dDevice, pd3dCommandList)
{
	m_nStride = sizeof(CTextureVertex);
	m_nOffset = 0;
	m_nSlot = 0;
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	float fx = fWidth * 0.5f, fy = fHeight * 0.5f, fz = fDepth * 0.5f;

#ifdef _WITH_INDEX_BUFFER
	m_nVertices = 24;

	CTextureVertex pVertices[24];

	//Back
	pVertices[0] = CTextureVertex(XMFLOAT3(+fx, -fy, +fz), XMFLOAT2(0.0f, 1.0f));
	pVertices[1] = CTextureVertex(XMFLOAT3(+fx, +fy, +fz), XMFLOAT2(0.0f, 0.0f));
	pVertices[2] = CTextureVertex(XMFLOAT3(-fx, +fy, +fz), XMFLOAT2(1.0f, 0.0f));
	pVertices[3] = CTextureVertex(XMFLOAT3(-fx, -fy, +fz), XMFLOAT2(1.0f, 1.0f));
	//Front
	pVertices[4] = CTextureVertex(XMFLOAT3(-fx, +fy, -fz), XMFLOAT2(0.0f, 0.0f));
	pVertices[5] = CTextureVertex(XMFLOAT3(+fx, +fy, -fz), XMFLOAT2(1.0f, 0.0f));
	pVertices[6] = CTextureVertex(XMFLOAT3(+fx, -fy, -fz), XMFLOAT2(1.0f, 1.0f));
	pVertices[7] = CTextureVertex(XMFLOAT3(-fx, -fy, -fz), XMFLOAT2(0.0f, 1.0f));
	//Top
	pVertices[8] = CTextureVertex(XMFLOAT3(-fx, +fy, +fz), XMFLOAT2(0.0f, 0.0f));
	pVertices[9] = CTextureVertex(XMFLOAT3(+fx, +fy, +fz), XMFLOAT2(1.0f, 0.0f));
	pVertices[10] = CTextureVertex(XMFLOAT3(+fx, +fy, -fz), XMFLOAT2(0.0f, 1.0f));
	pVertices[11] = CTextureVertex(XMFLOAT3(-fx, +fy, -fz), XMFLOAT2(1.0f, 1.0f));
	//Bottom
	pVertices[12] = CTextureVertex(XMFLOAT3(+fx, -fy, -fz), XMFLOAT2(0.0f, 0.0f));
	pVertices[13] = CTextureVertex(XMFLOAT3(+fx, -fy, +fz), XMFLOAT2(0.0f, 1.0f));
	pVertices[14] = CTextureVertex(XMFLOAT3(-fx, -fy, +fz), XMFLOAT2(1.0f, 1.0f));
	pVertices[15] = CTextureVertex(XMFLOAT3(-fx, -fy, -fz), XMFLOAT2(1.0f, 0.0f));
	//Left
	pVertices[16] = CTextureVertex(XMFLOAT3(-fx, +fy, +fz), XMFLOAT2(0.0f, 0.0f));
	pVertices[17] = CTextureVertex(XMFLOAT3(-fx, +fy, -fz), XMFLOAT2(1.0f, 0.0f));
	pVertices[18] = CTextureVertex(XMFLOAT3(-fx, -fy, -fz), XMFLOAT2(1.0f, 1.0f));
	pVertices[19] = CTextureVertex(XMFLOAT3(-fx, -fy, +fz), XMFLOAT2(0.0f, 1.0f));
	//Right
	pVertices[20] = CTextureVertex(XMFLOAT3(+fx, +fy, -fz), XMFLOAT2(0.0f, 0.0f));
	pVertices[21] = CTextureVertex(XMFLOAT3(+fx, +fy, +fz), XMFLOAT2(1.0f, 0.0f));
	pVertices[22] = CTextureVertex(XMFLOAT3(+fx, -fy, +fz), XMFLOAT2(1.0f, 1.0f));
	pVertices[23] = CTextureVertex(XMFLOAT3(+fx, -fy, -fz), XMFLOAT2(0.0f, 1.0f));

	m_pd3dVertexBuffer = CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices, m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;

	m_nIndices = 36;
	UINT pnIndices[36];

	//Back
	pnIndices[0] = 0; pnIndices[1] = 1; pnIndices[2] = 2;
	pnIndices[3] = 0; pnIndices[4] = 2; pnIndices[5] = 3;
	//Front
	pnIndices[6] = 4; pnIndices[7] = 5; pnIndices[8] = 6;
	pnIndices[9] = 4; pnIndices[10] = 6; pnIndices[11] = 7;
	//Top
	pnIndices[12] = 8; pnIndices[13] = 9; pnIndices[14] = 10;
	pnIndices[15] = 8; pnIndices[16] = 10; pnIndices[17] = 11;
	//Bottom
	pnIndices[18] = 12; pnIndices[19] = 13; pnIndices[20] = 14;
	pnIndices[21] = 12; pnIndices[22] = 14; pnIndices[23] = 15;
	//Left
	pnIndices[24] = 16; pnIndices[25] = 17; pnIndices[26] = 18;
	pnIndices[27] = 16; pnIndices[28] = 18; pnIndices[29] = 19;
	//Right
	pnIndices[30] = 20; pnIndices[31] = 21; pnIndices[32] = 22;
	pnIndices[33] = 20; pnIndices[34] = 22; pnIndices[35] = 23;

	m_pd3dIndexBuffer = CreateBufferResource(pd3dDevice, pd3dCommandList, pnIndices, sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_pd3dIndexUploadBuffer);

	m_d3dIndexBufferView.BufferLocation = m_pd3dIndexBuffer->GetGPUVirtualAddress();
	m_d3dIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_d3dIndexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices;
#else
	m_nVertices = 36;
	CTextureVertex pVertices[36];
	int i = 0;

	pVertices[i++] = CTextureVertex(XMFLOAT3(-fx, +fy, -fz), XMFLOAT2(0.0f, 0.0f));
	pVertices[i++] = CTextureVertex(XMFLOAT3(+fx, +fy, -fz), XMFLOAT2(1.0f, 0.0f));
	pVertices[i++] = CTextureVertex(XMFLOAT3(+fx, -fy, -fz), XMFLOAT2(1.0f, 1.0f));

	pVertices[i++] = CTextureVertex(XMFLOAT3(-fx, +fy, -fz), XMFLOAT2(0.0f, 0.0f));
	pVertices[i++] = CTextureVertex(XMFLOAT3(+fx, -fy, -fz), XMFLOAT2(1.0f, 1.0f));
	pVertices[i++] = CTextureVertex(XMFLOAT3(-fx, -fy, -fz), XMFLOAT2(0.0f, 1.0f));

	pVertices[i++] = CTextureVertex(XMFLOAT3(-fx, +fy, +fz), XMFLOAT2(0.0f, 0.0f));
	pVertices[i++] = CTextureVertex(XMFLOAT3(+fx, +fy, +fz), XMFLOAT2(1.0f, 0.0f));
	pVertices[i++] = CTextureVertex(XMFLOAT3(+fx, +fy, -fz), XMFLOAT2(1.0f, 1.0f));

	pVertices[i++] = CTextureVertex(XMFLOAT3(-fx, +fy, +fz), XMFLOAT2(0.0f, 0.0f));
	pVertices[i++] = CTextureVertex(XMFLOAT3(+fx, +fy, -fz), XMFLOAT2(1.0f, 1.0f));
	pVertices[i++] = CTextureVertex(XMFLOAT3(-fx, +fy, -fz), XMFLOAT2(0.0f, 1.0f));

	pVertices[i++] = CTextureVertex(XMFLOAT3(-fx, -fy, +fz), XMFLOAT2(0.0f, 0.0f));
	pVertices[i++] = CTextureVertex(XMFLOAT3(+fx, -fy, +fz), XMFLOAT2(1.0f, 0.0f));
	pVertices[i++] = CTextureVertex(XMFLOAT3(+fx, +fy, +fz), XMFLOAT2(1.0f, 1.0f));

	pVertices[i++] = CTextureVertex(XMFLOAT3(-fx, -fy, +fz), XMFLOAT2(0.0f, 0.0f));
	pVertices[i++] = CTextureVertex(XMFLOAT3(+fx, +fy, +fz), XMFLOAT2(1.0f, 1.0f));
	pVertices[i++] = CTextureVertex(XMFLOAT3(-fx, +fy, +fz), XMFLOAT2(0.0f, 1.0f));

	pVertices[i++] = CTextureVertex(XMFLOAT3(-fx, -fy, -fz), XMFLOAT2(0.0f, 0.0f));
	pVertices[i++] = CTextureVertex(XMFLOAT3(+fx, -fy, -fz), XMFLOAT2(1.0f, 0.0f));
	pVertices[i++] = CTextureVertex(XMFLOAT3(+fx, -fy, +fz), XMFLOAT2(1.0f, 1.0f));

	pVertices[i++] = CTextureVertex(XMFLOAT3(-fx, -fy, -fz), XMFLOAT2(0.0f, 0.0f));
	pVertices[i++] = CTextureVertex(XMFLOAT3(+fx, -fy, +fz), XMFLOAT2(1.0f, 1.0f));
	pVertices[i++] = CTextureVertex(XMFLOAT3(-fx, -fy, +fz), XMFLOAT2(0.0f, 1.0f));

	pVertices[i++] = CTextureVertex(XMFLOAT3(-fx, +fy, +fz), XMFLOAT2(0.0f, 0.0f));
	pVertices[i++] = CTextureVertex(XMFLOAT3(-fx, +fy, -fz), XMFLOAT2(1.0f, 0.0f));
	pVertices[i++] = CTextureVertex(XMFLOAT3(-fx, -fy, -fz), XMFLOAT2(1.0f, 1.0f));

	pVertices[i++] = CTextureVertex(XMFLOAT3(-fx, +fy, +fz), XMFLOAT2(0.0f, 0.0f));
	pVertices[i++] = CTextureVertex(XMFLOAT3(-fx, -fy, -fz), XMFLOAT2(1.0f, 1.0f));
	pVertices[i++] = CTextureVertex(XMFLOAT3(-fx, -fy, +fz), XMFLOAT2(0.0f, 1.0f));

	pVertices[i++] = CTextureVertex(XMFLOAT3(+fx, +fy, -fz), XMFLOAT2(0.0f, 0.0f));
	pVertices[i++] = CTextureVertex(XMFLOAT3(+fx, +fy, +fz), XMFLOAT2(1.0f, 0.0f));
	pVertices[i++] = CTextureVertex(XMFLOAT3(+fx, -fy, +fz), XMFLOAT2(1.0f, 1.0f));

	pVertices[i++] = CTextureVertex(XMFLOAT3(+fx, +fy, -fz), XMFLOAT2(0.0f, 0.0f));
	pVertices[i++] = CTextureVertex(XMFLOAT3(+fx, -fy, +fz), XMFLOAT2(1.0f, 1.0f));
	pVertices[i++] = CTextureVertex(XMFLOAT3(+fx, -fy, -fz), XMFLOAT2(0.0f, 1.0f));

	m_pd3dVertexBuffer = CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices, m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;
#endif
}

CCubeMeshTextured::~CCubeMeshTextured()
{
}
