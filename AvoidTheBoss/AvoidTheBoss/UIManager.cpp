#include "pch.h"
#include "GameFramework.h"
#include "UIManager.h"
#include <wincodec.h>

#pragma comment(lib,"windowscodecs.lib")

UIManager::UIManager(UINT nFrames, UINT nTextBlocks, ID3D12Device* pd3dDevice, ID3D12CommandQueue* pd3dCommandQueue, ID3D12Resource** ppd3dRenderTargets, UINT nWidth, UINT nHeight)
{

    m_fWidth = static_cast<float>(nWidth);
    m_fHeight = static_cast<float>(nHeight);
    m_nRenderTargets = nFrames; // ���� Ÿ��
    m_ppd3d11WrappedRenderTargets = new ID3D11Resource * [nFrames];
    m_ppd2dRenderTargets = new ID2D1Bitmap1 * [nFrames];

    m_nTextBlocks = nTextBlocks;
    m_pTextBlocks = new UITextBlock[nTextBlocks];

    InitializeDevice(pd3dDevice, pd3dCommandQueue, ppd3dRenderTargets);

}



void UIManager::CreateD3D11On12Device(ID3D12Device* pd3dDevice, ID3D12CommandQueue* pd3dCommandQueue)
{
    UINT d3d11DeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT; // ui ����� ���� �ʿ��� �÷���
    D2D1_FACTORY_OPTIONS d2dFactoryOptions = { };

    // D3D11On12 ����̽� ����
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
    // 1. d3d12Device 2. ����̽� �÷���, 3. d3d11Device*�� �ּ� 4. DeviceContext �ּ�

    // ������ d3d11on12����̽��� ���� �������̽��� m_pd3d11on12Device�� �̵�
    //pd3d11Device->QueryInterface(__uuidof(ID3D11On12Device), (void**)m_pd3d11On12Device.GetAddressOf()); 
    pd3d11Device.As(&m_pd3d11On12Device);
    pd3d11Device->Release(); // ����� ���� pd3d11Device�� ������
}

void UIManager::CreateD2DDevice() // d3d11on12����̽��� Ȱ���� d2ddevice�� d2dFactory ����
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

int32 UIManager::LoadPngFromFile(const wchar_t* filePath, ID2D1Bitmap* bit)
{
    if (bit != NULL)
    {
        bit->Release();
        bit = NULL;
    }

    // WIC Factory ��ü ����
    IWICImagingFactory* pWicFactory;
    CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(&pWicFactory));
    // �̹��� ��ü�� ���� ��ü ����
    IWICBitmapDecoder* pDecoder;
    IWICBitmapFrameDecode* pFrame; // �̹����� �����ϱ� ���� ��ü
    IWICFormatConverter* pConverter; // �̹��� ��ȯ ��ü
    
    int32 result = 0;
    if (S_OK == pWicFactory->CreateDecoderFromFilename(filePath, NULL, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &pDecoder)) 
    {
        // ������ �����ϴ� �̹��� �߿��� ù��° �̹����� �����Ѵ�.
        if (S_OK == pDecoder->GetFrame(0, &pFrame))
        {
            // IWICBitmap������ ��Ʈ���� ID2D1Bitmap. �������� ��ȯ�ϱ� ���� ��ü ����
            if (S_OK == pWicFactory->CreateFormatConverter(&pConverter))
            {
                // ���õ� �׸��� � ������ ��Ʈ������ ��ȯ�� ������ ����
                if (S_OK == pConverter->Initialize(pFrame, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.0f, WICBitmapPaletteTypeCustom))
                {
                    ID2D1Bitmap* mbit;
                    // IWICBitmap ������ ��Ʈ������ ID2D1Bitmap ��ü�� ����
                    if (S_OK == m_pd2dDeviceContext->CreateBitmapFromWicBitmap(pConverter, NULL, &m_bitmaps)) result = 1;  // ���������� ������ ���
                   
                }
                // �̹��� ��ȯ ��ü ����
                pConverter->Release();
            }
            // �׸����Ͽ� �ִ� �̹����� �����ϱ� ���� ����� ��ü ����
            pFrame->Release();
        }
        // ������ �����ϱ� ���� ������ ��ü ����
        pDecoder->Release();
    }
    // WIC�� ����ϱ� ���� ������� Factory ��ü ����
    pWicFactory->Release();

    return result;  // PNG ������ ���� ����� ��ȯ�Ѵ�.

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

void UIManager::Render2D(UINT nFrame, int32 curScene)
{

    ID3D11Resource* ppResources[] = { m_ppd3d11WrappedRenderTargets[nFrame] };

    m_pd2dDeviceContext->SetTarget(m_ppd2dRenderTargets[nFrame]);
    m_pd3d11On12Device->AcquireWrappedResources(ppResources, _countof(ppResources));

    m_pd2dDeviceContext->BeginDraw();

    //if (curScene == (int32)CGameFramework::SCENESTATE::INGAME) DrawInGameBitmap();
    //if (curScene == (int32)CGameFramework::SCENESTATE::TITLE) DrawTitleBitmap();
    //if (curScene == (int32)CGameFramework::SCENESTATE::LOBBY) DrawLobbyBitmap();
    m_pd2dDeviceContext->DrawBitmap(m_bitmaps, (D2D1_RECT_F*)0, 1.0, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, (D2D1_RECT_F*)0);
    m_pd2dDeviceContext->EndDraw();

    m_pd3d11On12Device->ReleaseWrappedResources(ppResources, _countof(ppResources));
    m_pd3d11DeviceContext->Flush();
}

void UIManager::ReleaseResources()
{
    for (UINT i = 0; i < m_nRenderTargets; i++)
    {
        ID3D11Resource* ppResources[] = { m_ppd3d11WrappedRenderTargets[i] };
        m_pd3d11On12Device->ReleaseWrappedResources(ppResources, _countof(ppResources));
    }

    m_pd2dDeviceContext->SetTarget(nullptr);
    m_pd3d11DeviceContext->Flush();

    for (UINT i = 0; i < m_nRenderTargets; i++)
    {
        m_ppd2dRenderTargets[i]->Release();
        m_ppd3d11WrappedRenderTargets[i]->Release();
    }

    m_pd2dDeviceContext->Release();
    m_pd2dWriteFactory->Release();
    m_pd2dDevice->Release();
    m_pd2dFactory->Release();
    m_pd3d11DeviceContext->Release();
    m_pd3d11On12Device->Release();
}

void UIManager::CreateBackGroundLayer(UIBackGround& ui, const wchar_t* filepath, const D2D1_RECT_F& rLayout)
{
    LoadPngFromFile(filepath, ui.resource);
    ui.d2dLayoutRect = rLayout;
}

void UIManager::CreateTextBoxLayer(UITextBlock& ui, const wchar_t* text, const D2D1_RECT_F& rLayout)
{
    wcsncpy_s(ui.m_pstrText, text, sizeof(text));
    
}

void UIManager::CreateButtonLayer(UIButton& ui, const wchar_t* filepath, const D2D1_RECT_F& rLayout)
{
    LoadPngFromFile(filepath, ui.resource);
    ui.d2dLayoutRect = rLayout;
}

void UIManager::DrawTitleBitmap()
{
    m_pd2dDeviceContext->DrawBitmap(m_TitleBitmaps.resource,
        D2D1::RectF(0.0f, 0.0f, m_TitleBitmaps.resource->GetSize().width, m_TitleBitmaps.resource->GetSize().height)
        , 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, m_TitleBitmaps.d2dLayoutRect);
}

void UIManager::DrawLobbyBitmap()
{
   if(m_LobbyBitmaps.resource) m_pd2dDeviceContext->DrawBitmap(m_LobbyBitmaps.resource,
        D2D1::RectF(0.0f, 0.0f, m_LobbyBitmaps.resource->GetSize().width, m_LobbyBitmaps.resource->GetSize().height)
        , 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, m_LobbyBitmaps.d2dLayoutRect);
}

void UIManager::DrawInGameBitmap()
{
    m_pd2dDeviceContext->DrawBitmap(m_InGameBitmaps.resource,
        D2D1::RectF(0.0f, 0.0f, m_InGameBitmaps.resource->GetSize().width, m_InGameBitmaps.resource->GetSize().height)
        , 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, m_InGameBitmaps.d2dLayoutRect);
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
    m_pd2dWriteFactory->CreateTextFormat(L"���� ���", nullptr, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, fFontSize, L"en-us", &pdwDefaultTextFormat);

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
    //CreateBackGroundLayer(m_TitleBitmaps, L"UI/Title.png", { 0,0,m_fWidth,m_fHeight });
    //CreateBackGroundLayer(m_LobbyBitmaps, L"UI/Lobby.png", { 0,0,m_fWidth,m_fHeight });
    LoadPngFromFile(L"UI/Same.png", m_LobbyBitmaps.resource);
    //LoadPngFromFile(L"UI/Same.png", m_bitmaps);
}
