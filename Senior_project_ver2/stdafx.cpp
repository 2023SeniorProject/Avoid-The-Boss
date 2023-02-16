#include "stdafx.h"

#include "d3dx12.h"
#include "DDSTextureLoader12.h"


UINT gnCbvSrvDescriptorIncrementSize = 0;

/*버퍼 리소스를 생성하는 함수이다. 버퍼의 힙 유형에 따라 버퍼 리소스를 생성하고 초기화 데이터가 있으면 초기화
한다.*/
ID3D12Resource* CreateBufferResource(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList
	* pd3dCommandList, const void* pData, UINT nBytes, D3D12_HEAP_TYPE d3dHeapType,
	D3D12_RESOURCE_STATES d3dResourceStates, ID3D12Resource** ppd3dUploadBuffer)
{
	ID3D12Resource* pd3dBuffer = NULL;

	D3D12_HEAP_PROPERTIES d3dHeapPropertiesDesc;
	::ZeroMemory(&d3dHeapPropertiesDesc, sizeof(D3D12_HEAP_PROPERTIES));
	d3dHeapPropertiesDesc.Type = d3dHeapType;
	d3dHeapPropertiesDesc.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	d3dHeapPropertiesDesc.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	d3dHeapPropertiesDesc.CreationNodeMask = 1;
	d3dHeapPropertiesDesc.VisibleNodeMask = 1;

	D3D12_RESOURCE_DESC d3dResourceDesc;
	::ZeroMemory(&d3dResourceDesc, sizeof(D3D12_RESOURCE_DESC));
	d3dResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	d3dResourceDesc.Alignment = 0;
	d3dResourceDesc.Width = nBytes;
	d3dResourceDesc.Height = 1;
	d3dResourceDesc.DepthOrArraySize = 1;
	d3dResourceDesc.MipLevels = 1;
	d3dResourceDesc.Format = DXGI_FORMAT_UNKNOWN;
	d3dResourceDesc.SampleDesc.Count = 1;
	d3dResourceDesc.SampleDesc.Quality = 0;
	d3dResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	d3dResourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	D3D12_RESOURCE_STATES d3dResourceInitialStates = D3D12_RESOURCE_STATE_COPY_DEST;
	if (d3dHeapType == D3D12_HEAP_TYPE_UPLOAD) d3dResourceInitialStates =
		D3D12_RESOURCE_STATE_GENERIC_READ;
	else if (d3dHeapType == D3D12_HEAP_TYPE_READBACK) d3dResourceInitialStates =
		D3D12_RESOURCE_STATE_COPY_DEST;

	HRESULT hResult = pd3dDevice->CreateCommittedResource(&d3dHeapPropertiesDesc,
		D3D12_HEAP_FLAG_NONE, &d3dResourceDesc, d3dResourceInitialStates, NULL,
		__uuidof(ID3D12Resource), (void**)&pd3dBuffer);

	if (pData)
	{
		switch (d3dHeapType)
		{
		case D3D12_HEAP_TYPE_DEFAULT:
		{
			if (ppd3dUploadBuffer)
			{
				//업로드 버퍼를 생성한다.
				d3dHeapPropertiesDesc.Type = D3D12_HEAP_TYPE_UPLOAD;
				pd3dDevice->CreateCommittedResource(&d3dHeapPropertiesDesc,
					D3D12_HEAP_FLAG_NONE, &d3dResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, NULL,
					__uuidof(ID3D12Resource), (void**)ppd3dUploadBuffer);

#ifdef _WITH_MAPPING
				D3D12_RANGE d3dReadRange = { 0, 0 };
				UINT8* pBufferDataBegin = NULL;
				(*ppd3dUploadBuffer)->Map(0, &d3dReadRange, (void**)&pBufferDataBegin);
				memcpy(pBufferDataBegin, pData, nBytes);
				(*ppd3dUploadBuffer)->Unmap(0, NULL);

				pd3dCommandList->CopyResource(pd3dBuffer, *ppd3dUploadBuffer);
#else
				D3D12_SUBRESOURCE_DATA d3dSubResourceData;
				::ZeroMemory(&d3dSubResourceData, sizeof(D3D12_SUBRESOURCE_DATA));
				d3dSubResourceData.pData = pData;
				d3dSubResourceData.SlicePitch = d3dSubResourceData.RowPitch = nBytes;
				::UpdateSubresources<1>(pd3dCommandList, pd3dBuffer, *ppd3dUploadBuffer, 0, 0, 1, &d3dSubResourceData);

#endif
				D3D12_RESOURCE_BARRIER d3dResourceBarrier;
				::ZeroMemory(&d3dResourceBarrier, sizeof(D3D12_RESOURCE_BARRIER));
				d3dResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
				d3dResourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
				d3dResourceBarrier.Transition.pResource = pd3dBuffer;
				d3dResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
				d3dResourceBarrier.Transition.StateAfter = d3dResourceStates;
				d3dResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
				pd3dCommandList->ResourceBarrier(1, &d3dResourceBarrier);
			}
			break;
		}
		case D3D12_HEAP_TYPE_UPLOAD:
		{
			D3D12_RANGE d3dReadRange = { 0, 0 };
			UINT8* pBufferDataBegin = NULL;
			pd3dBuffer->Map(0, &d3dReadRange, (void**)&pBufferDataBegin);
			memcpy(pBufferDataBegin, pData, nBytes);
			pd3dBuffer->Unmap(0, NULL);
			break;
		}
		case D3D12_HEAP_TYPE_READBACK:
			break;
		}
	}
	return(pd3dBuffer);
}


ID3D12Resource* CreateTextureResourceFromDDSFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, const wchar_t* pszFileName, ID3D12Resource** ppd3dUploadBuffer, D3D12_RESOURCE_STATES d3dResourceStates)
{
	ID3D12Resource* pd3dTexture = NULL;
	std::unique_ptr<uint8_t[]> ddsData;
	std::vector<D3D12_SUBRESOURCE_DATA> vSubresources;
	DDS_ALPHA_MODE ddsAlphaMode = DDS_ALPHA_MODE_UNKNOWN;
	bool bIsCubeMap = false;

	HRESULT hResult = DirectX::LoadDDSTextureFromFileEx(pd3dDevice, pszFileName, 0, D3D12_RESOURCE_FLAG_NONE, DDS_LOADER_DEFAULT, &pd3dTexture, ddsData, vSubresources, &ddsAlphaMode, &bIsCubeMap);

	D3D12_HEAP_PROPERTIES d3dHeapPropertiesDesc;
	::ZeroMemory(&d3dHeapPropertiesDesc, sizeof(D3D12_HEAP_PROPERTIES));
	d3dHeapPropertiesDesc.Type = D3D12_HEAP_TYPE_UPLOAD;
	d3dHeapPropertiesDesc.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	d3dHeapPropertiesDesc.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	d3dHeapPropertiesDesc.CreationNodeMask = 1;
	d3dHeapPropertiesDesc.VisibleNodeMask = 1;

	UINT nSubResources = (UINT)vSubresources.size();
	UINT64 nBytes = GetRequiredIntermediateSize(pd3dTexture, 0, nSubResources);

	D3D12_RESOURCE_DESC d3dResourceDesc;
	::ZeroMemory(&d3dResourceDesc, sizeof(D3D12_RESOURCE_DESC));
	d3dResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER; //Upload Heap에는 텍스쳐를 생성할 수 없음
	d3dResourceDesc.Alignment = 0;
	d3dResourceDesc.Width = nBytes;
	d3dResourceDesc.Height = 1;
	d3dResourceDesc.DepthOrArraySize = 1;
	d3dResourceDesc.MipLevels = 1;
	d3dResourceDesc.Format = DXGI_FORMAT_UNKNOWN;
	d3dResourceDesc.SampleDesc.Count = 1;
	d3dResourceDesc.SampleDesc.Quality = 0;
	d3dResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	d3dResourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	pd3dDevice->CreateCommittedResource(&d3dHeapPropertiesDesc, D3D12_HEAP_FLAG_NONE, &d3dResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, NULL, __uuidof(ID3D12Resource), (void**)ppd3dUploadBuffer);

	::UpdateSubresources(pd3dCommandList, pd3dTexture, *ppd3dUploadBuffer, 0, 0, nSubResources, &vSubresources[0]);

	D3D12_RESOURCE_BARRIER d3dResourceBarrier;
	::ZeroMemory(&d3dResourceBarrier, sizeof(D3D12_RESOURCE_BARRIER));
	d3dResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	d3dResourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	d3dResourceBarrier.Transition.pResource = pd3dTexture;
	d3dResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	d3dResourceBarrier.Transition.StateAfter = d3dResourceStates;
	d3dResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	pd3dCommandList->ResourceBarrier(1, &d3dResourceBarrier);

	return(pd3dTexture);
}


//============================================

//====초기화

//----파이프라인 초기화

//디버그 계층을 사용하도록 설정합니다.

//디바이스를 만듭니다.

//명령 큐를 만듭니다.

//스왑 체인을 만듭니다.

//RTV(렌더링 대상 보기) 설명자 힙을 만듭니다.

//프레임 리소스(각 프레임에 대한 렌더링 대상 보기)를 만듭니다.

//명령 할당자를 만듭니다.

//----자산을 초기화 합니다.

//빈 루트 시그니쳐를 만듭니다.

//셰이더를 컴파일합니다.

//꼭짓점 입력 레이아웃을 만듭니다.

//파이프라인 상태 개체 디스크립터를 만든 다음 개체를 만듭니다.

//명령 목록을 만듭니다.

//명령 목록을 닫습니다.

//꼭짓점 버퍼를 만들고 로드합니다.

//꼭짓점 버퍼 뷰를 만듭니다.

//펜스를 만듭니다.(CPU-GPU 동기화 하는데 사용되는 펜스)

//이벤트 핸들을 만듭니다.

//GPU가 완료될 때까지 기다립니다.

//========================================

//====업데이터

//필요에 따라 상수,꼭짓점,인덱스 버퍼 및 다른 모든 항목을 수정합니다.

//OnUpdate

//=======================================

//====렌더링

//----명령 목록을 채웁니다.

//명령 리스트 할당자를 다시 설정합니다.

//명령 리스트를 다시 설정합니다.

//그래픽 루트 시그너쳐를 설정합니다.(현재 명령 리스트에 사용할 그래픽 루트 시그니쳐를 설정)

//뷰포트 및 가위 사각형을 설정합니다.

//렌더링 대상으로 사용할 백 버퍼를 나타내는 리소스 디스펜서를 설정합니다.(리소스 장벽은 리소슷 전환을 관리하는데 사용)

//명령을 명령 리스트에 기록합니다.

//명령 목록이 실행된 후 백 버퍼가 표시하는데 사용됨을 나타냅니다.(리소스 디스펜서를 설정하는 또 다른 호출)

//추가 기록을 위해 명령 리스트를 닫습니다.

//----명령 목록을 실행합니다.

//----프레임을 제공합니다.

//----GPU가 완료될 때까지 기다립니다.(펜스를 계속 업데이트하고 확인하세요)

//OnRender

//=========================================

//====삭제 : 앱을 완전히 닫습니다.

//----GPU가 완료될 때까지 기다립니다.(펜스를 최종적으로 확인하세요.)

//----이벤트 핸들을 닫습니다.

//OnDestroy



