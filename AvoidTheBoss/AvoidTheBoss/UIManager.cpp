#include "pch.h"
#include "UIManager.h"

UIManager::UIManager(UINT nFrames, UINT nTextBlocks, ID3D12Device* pd3dDevice, ID3D12CommandQueue* pd3dCommandQueue, ID3D12Resource** ppd3dRenderTargets, UINT nWidth, UINT nHeight)
{

    m_fWidth = static_cast<float>(nWidth);
    m_fHeight = static_cast<float>(nHeight);
    m_nRenderTargets = nFrames; // 렌더 타겟
    m_ppd3d11WrappedRenderTargets = new ID3D11Resource * [nFrames];
    m_ppd2dRenderTargets = new ID2D1Bitmap1 * [nFrames];

    m_nTextBlocks = nTextBlocks;
    m_pTextBlocks = new UITextBlock[nTextBlocks];

    InitializeDevice(pd3dDevice, pd3dCommandQueue, ppd3dRenderTargets);

}



void UIManager::CreateD3D11On12Device(ID3D12Device* pd3dDevice, ID3D12CommandQueue* pd3dCommandQueue)
{
    UINT d3d11DeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT; // ui 출력을 위해 필요한 플레그
    D2D1_FACTORY_OPTIONS d2dFactoryOptions = { };

    // D3D11On12 디바이스 생성
    ComPtr<ID3D11Device> pd3d11Device = NULL;
    ID3D12CommandQueue* ppd3dCommandQueues[] = { pd3dCommandQueue };
    ::D3D11On12CreateDevice
    (pd3dDevice,
        d3d11DeviceFlags,
        nullptr, 0,
        reinterpret_cast<IUnknown**>(ppd3dCommandQueues),
        _countof(ppd3dCommandQueues), 0,
        &pd3d11Device,
        &m_pd3d11DeviceContext, nullptr);
    // 1. d3d12Device 2. 디바이스 플레그, 3. d3d11Device*의 주소 4. DeviceContext 주소

    // 생성된 d3d11on12디바이스에 관한 인터페이스를 m_pd3d11on12Device로 이동
    //pd3d11Device->QueryInterface(__uuidof(ID3D11On12Device), (void**)m_pd3d11On12Device.GetAddressOf()); 
    pd3d11Device.As(&m_pd3d11On12Device);
    pd3d11Device->Release(); // 사용이 끝난 pd3d11Device는 릴리즈
}

void UIManager::CreateD2DDevice() // d3d11on12디바이스를 활용해 d2ddevice랑 d2dFactory 생성
{
    D2D1_FACTORY_OPTIONS d2dFactoryOptions{};
    D2D1_DEVICE_CONTEXT_OPTIONS deviceOptions = D2D1_DEVICE_CONTEXT_OPTIONS_NONE;

    IDXGIDevice* pdxgiDevice = NULL;
    m_pd3d11On12Device->QueryInterface(__uuidof(IDXGIDevice), (void**)&pdxgiDevice);

    ::D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory3), &d2dFactoryOptions, (void**)&m_pd2dFactory);
    HRESULT hResult = m_pd2dFactory->CreateDevice(pdxgiDevice, (ID2D1Device2**)&m_pd2dDevice);
    m_pd2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, (ID2D1DeviceContext2**)&m_pd2dDeviceContext);

    m_pd2dDeviceContext->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);

    ::DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), (IUnknown**)&m_pd2dWriteFactory);
    pdxgiDevice->Release();
}


void UIManager::CreateRenderTarget(ID3D12Resource** ppd3dRenderTargets)
{
    D2D1_BITMAP_PROPERTIES1 d2dBitmapProperties = D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW, D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED));

    for (UINT i = 0; i < m_nRenderTargets; i++)
    {
        D3D11_RESOURCE_FLAGS d3d11Flags = { D3D11_BIND_RENDER_TARGET };
        m_pd3d11On12Device->CreateWrappedResource(ppd3dRenderTargets[i], &d3d11Flags, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT, IID_PPV_ARGS(&m_ppd3d11WrappedRenderTargets[i]));
        IDXGISurface* pdxgiSurface = NULL;
        m_ppd3d11WrappedRenderTargets[i]->QueryInterface(__uuidof(IDXGISurface), (void**)&pdxgiSurface);
        m_pd2dDeviceContext->CreateBitmapFromDxgiSurface(pdxgiSurface, &d2dBitmapProperties, &m_ppd2dRenderTargets[i]);
        pdxgiSurface->Release();
    }
}


void UIManager::UpdateTextOutputs(UINT nIndex, WCHAR* pstrUIText, D2D1_RECT_F* pd2dLayoutRect, IDWriteTextFormat* pdwFormat, ID2D1SolidColorBrush* pd2dTextBrush)
{
}

void UIManager::Render2D(UINT nFrame)
{

    ID3D11Resource* ppResources[] = { m_ppd3d11WrappedRenderTargets[nFrame] };

    m_pd2dDeviceContext->SetTarget(m_ppd2dRenderTargets[nFrame]);
    m_pd3d11On12Device->AcquireWrappedResources(ppResources, _countof(ppResources));

    m_pd2dDeviceContext->BeginDraw();

    // 렌더링 공간 특정씬이 있는 경우..~
    /*for (UINT i = 0; i < m_nTextBlocks; i++)
    {
        m_pd2dDeviceContext->DrawText(m_pTextBlocks[i].m_pstrText, (UINT)wcslen(m_pTextBlocks[i].m_pstrText), m_pTextBlocks[i].m_pdwFormat, m_pTextBlocks[i].m_d2dLayoutRect, m_pTextBlocks[i].m_pd2dTextBrush);
    }*/
    m_pd2dDeviceContext->EndDraw();

    m_pd3d11On12Device->ReleaseWrappedResources(ppResources, _countof(ppResources));
    m_pd3d11DeviceContext->Flush();
}

void UIManager::ReleaseResources()
{
}

ID2D1SolidColorBrush* UIManager::CreateBrush(D2D1::ColorF d2dColor)
{
    ID2D1SolidColorBrush* pd2dDefaultTextBrush = NULL;
    m_pd2dDeviceContext->CreateSolidColorBrush(d2dColor, &pd2dDefaultTextBrush);

    return(pd2dDefaultTextBrush);
}

IDWriteTextFormat* UIManager::CreateTextFormat(WCHAR* pszFontName, float fFontSize)
{
    IDWriteTextFormat* pdwDefaultTextFormat = NULL;
    m_pd2dWriteFactory->CreateTextFormat(L"맑은 고딕", nullptr, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, fFontSize, L"en-us", &pdwDefaultTextFormat);

    pdwDefaultTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
    pdwDefaultTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
    //m_pd2dWriteFactory->CreateTextFormat(L"Arial", nullptr, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, fSmallFontSize, L"en-us", &m_pdwDefaultTextFormat);

    return(pdwDefaultTextFormat);
}

void UIManager::InitializeDevice(ID3D12Device* pd3dDevice, ID3D12CommandQueue* pd3dCommandQueue, ID3D12Resource** ppd3dRenderTargets)
{
    
    CreateD3D11On12Device(pd3dDevice,pd3dCommandQueue);
    CreateD2DDevice();
    CreateRenderTarget(ppd3dRenderTargets);
   
}
