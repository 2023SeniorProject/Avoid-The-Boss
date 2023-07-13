#include "pch.h"
#include "GameFramework.h"
#include "UIManager.h"
#include <d2d1_1.h>
#include <wincodec.h>

#pragma comment(lib,"windowscodecs.lib")


// LobbyButton
const float CENTER_X = FRAME_BUFFER_WIDTH / 2.0;
const float CENTER_Y = FRAME_BUFFER_HEIGHT / 2.0;
const float TITLEBUTTON_X_OFFSET = FRAME_BUFFER_WIDTH / 2.0 - 100.f;
const float TITLEBUTTON_Y_OFFSET = 100.f;

const float LOBBYBUTTON_X_OFFSET = FRAME_BUFFER_WIDTH / 3.0;
const float LOBBYBUTTON_Y_OFFSET = FRAME_BUFFER_HEIGHT/ 4.0f * 3;

const float LOBBYROOMLIST_X_OFFSET = FRAME_BUFFER_WIDTH / 22;
const float LOBBYROOMLIST_Y_OFFSET = FRAME_BUFFER_HEIGHT / 18.5;


const float FontSize = 50;
const float IDPW_Y_OFFSET = FontSize / 2.0f;

D2D1_RECT_F MakeLayoutRect(float cx, float cy , float width, float height)
{
    return D2D1_RECT_F{ cx - width / 2.0f , cy - height / 2.0f , cx + width / 2.0f , cy + height / 2.0f };
}

D2D1_RECT_F MakeLayoutRectByCorner(float left, float top, float width, float height)
{
    return D2D1_RECT_F{ left , top , left + width, top + height};
}

UIManager::UIManager(UINT nFrames, UINT nTextBlocks, ID3D12Device5* pd3dDevice, ID3D12CommandQueue* pd3dCommandQueue, ID3D12Resource** ppd3dRenderTargets, UINT nWidth, UINT nHeight)
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



void UIManager::CreateD3D11On12Device(ID3D12Device5* pd3dDevice, ID3D12CommandQueue* pd3dCommandQueue)
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

ID2D1Bitmap1* UIManager::LoadPngFromFile(const wchar_t* filePath)
{
    ID2D1Bitmap1* bit = NULL;
    if (bit != NULL)
    {
       bit->Release();
       bit = NULL;
    }

    // WIC Factory 객체 생성
    IWICImagingFactory* pWicFactory;
    CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(&pWicFactory));
    // 이미지 해체를 위한 객체 생성
    IWICBitmapDecoder* pDecoder;
    IWICBitmapFrameDecode* pFrame; // 이미지를 선택하기 위한 객체
    IWICFormatConverter* pConverter; // 이미지 변환 객체
    
    int32 result = 0;
    if (S_OK == pWicFactory->CreateDecoderFromFilename(filePath, NULL, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &pDecoder)) 
    {
        // 파일을 구성하는 이미지 중에서 첫번째 이미지를 선택한다.
        if (S_OK == pDecoder->GetFrame(0, &pFrame))
        {
            // IWICBitmap형식의 비트맵을 ID2D1Bitmap. 형식으로 변환하기 위한 객체 생성
            if (S_OK == pWicFactory->CreateFormatConverter(&pConverter))
            {
                // 선택된 그림을 어떤 형식의 비트맵으로 변환할 것인지 설정
                if (S_OK == pConverter->Initialize(pFrame, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.0f, WICBitmapPaletteTypeCustom))
                {
                    
                    // IWICBitmap 형식의 비트맵으로 ID2D1Bitmap 객체를 생성
                    if (S_OK == m_pd2dDeviceContext->CreateBitmapFromWicBitmap(pConverter, NULL, &bit)) result = 1;  // 성공적으로 생성한 경우 
                }
                // 이미지 변환 객체 제거
                pConverter->Release();
            }
            // 그림파일에 있는 이미지를 선택하기 위해 사용한 객체 제거
            pFrame->Release();
        }
        // 압축을 해제하기 위해 생성한 객체 제거
        pDecoder->Release();
    }
    // WIC를 사용하기 위해 만들었던 Factory 객체 제거
    pWicFactory->Release();

    if(result) return bit;  // PNG 파일을 읽은 결과를 반환한다.

    return nullptr;

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

void UIManager::DrawBackGround(int32 Scene)
{
    switch (Scene)
    {
    case 0:
    case 1:
    case 2:
        m_pd2dDeviceContext->DrawBitmap(m_backGround[Scene].resource,
            D2D1_RECT_F{ 0,0,m_fWidth,m_fHeight }, 1.0f, D2D1_INTERPOLATION_MODE_LINEAR, (D2D1_RECT_F*)0);
        break;
    case 3:
        break;
    }
}

void UIManager::DrawButton(int32 Scene,int32 idx)
{
   
    if (Scene == 0) // 타이틀 씬
    {
        m_pd2dDeviceContext->DrawRectangle(m_TitleButtons[0].d2dLayoutRect, redBrush);
        m_pd2dDeviceContext->DrawBitmap(m_TitleButtons[0].resource, m_TitleButtons[0].d2dLayoutRect, 1.0f, D2D1_INTERPOLATION_MODE_LINEAR, (D2D1_RECT_F*)0);
        m_pd2dDeviceContext->DrawRectangle(m_TitleButtons[1].d2dLayoutRect, redBrush);
        m_pd2dDeviceContext->DrawBitmap(m_TitleButtons[1].resource, m_TitleButtons[1].d2dLayoutRect, 1.0f, D2D1_INTERPOLATION_MODE_LINEAR, (D2D1_RECT_F*)0);

    }
    else if (Scene == 1) // 로비 씬
    {
        m_pd2dDeviceContext->DrawRectangle(m_LobbyButtons[0].d2dLayoutRect, redBrush);
        m_pd2dDeviceContext->DrawBitmap(m_LobbyButtons[0].resource, m_LobbyButtons[0].d2dLayoutRect, 1.0f, D2D1_INTERPOLATION_MODE_LINEAR, (D2D1_RECT_F*)0);
        
        m_pd2dDeviceContext->DrawRectangle(m_LobbyButtons[1].d2dLayoutRect, redBrush);
        m_pd2dDeviceContext->DrawBitmap(m_LobbyButtons[1].resource, m_LobbyButtons[1].d2dLayoutRect, 1.0f, D2D1_INTERPOLATION_MODE_LINEAR, (D2D1_RECT_F*)0);
        
        m_pd2dDeviceContext->DrawRectangle(m_LobbyButtons[2].d2dLayoutRect, redBrush);
        m_pd2dDeviceContext->DrawBitmap(m_LobbyButtons[2].resource, m_LobbyButtons[2].d2dLayoutRect, 1.0f, D2D1_INTERPOLATION_MODE_LINEAR, (D2D1_RECT_F*)0);
    }
    else if (Scene == 2) // 게임 룸 씬
    {
        m_pd2dDeviceContext->DrawRectangle(m_RoomButtons[0].d2dLayoutRect, redBrush);
        m_pd2dDeviceContext->DrawBitmap(m_RoomButtons[0].resource, m_RoomButtons[0].d2dLayoutRect, 1.0f, D2D1_INTERPOLATION_MODE_LINEAR, (D2D1_RECT_F*)0);
        
        m_pd2dDeviceContext->DrawRectangle(m_RoomButtons[1].d2dLayoutRect, redBrush);
        m_pd2dDeviceContext->DrawBitmap(m_RoomButtons[1].resource, m_RoomButtons[1].d2dLayoutRect, 1.0f, D2D1_INTERPOLATION_MODE_LINEAR, (D2D1_RECT_F*)0);
       
        m_pd2dDeviceContext->DrawRectangle(m_RoomButtons[2].d2dLayoutRect, redBrush);
        m_pd2dDeviceContext->DrawBitmap(m_RoomButtons[2].resource, m_RoomButtons[2].d2dLayoutRect, 1.0f, D2D1_INTERPOLATION_MODE_LINEAR, (D2D1_RECT_F*)0);
    }
}

void UIManager::DrawTextBlock(int32 Scene)
{

    if (Scene == 0)
    {
        m_pd2dDeviceContext->FillRectangle(m_pTextBlocks[0].m_d2dLayoutRect, grayBrush);
        m_pd2dDeviceContext->FillRectangle(m_pTextBlocks[1].m_d2dLayoutRect, grayBrush);
        
        m_pd2dDeviceContext->DrawRectangle(m_pTextBlocks[0].m_d2dLayoutRect, blackBrush,4.0);
        m_pd2dDeviceContext->DrawRectangle(m_pTextBlocks[1].m_d2dLayoutRect, blackBrush,4.0);

        m_pd2dDeviceContext->DrawText(m_pTextBlocks[0].m_pstrText.c_str(), 
            (UINT)wcslen(m_pTextBlocks[0].m_pstrText.c_str()), m_pTextBlocks[0].m_pdwFormat, 
            m_pTextBlocks[0].m_d2dLayoutRect, m_pTextBlocks[0].m_pd2dTextBrush);

        m_pd2dDeviceContext->DrawText(m_pTextBlocks[1].m_pstrText.c_str(),
            (UINT)wcslen(m_pTextBlocks[1].m_pstrText.c_str()), m_pTextBlocks[1].m_pdwFormat,
            m_pTextBlocks[1].m_d2dLayoutRect, m_pTextBlocks[1].m_pd2dTextBrush);
    }
    else if (Scene == 1)
    {
        m_pd2dDeviceContext->DrawRectangle(m_RoomListLayout, blackBrush, 3.0f);
    }
}

D2D1_RECT_F UIManager::GetButtonRect(int32 Scene, int32 idx)
{
    switch (Scene)
    {
    case 0:
        return m_TitleButtons[idx].d2dLayoutRect;
        break;
    case 1:
        return m_LobbyButtons[idx].d2dLayoutRect;
        break;
    case 2:
        return m_RoomButtons[idx].d2dLayoutRect;
        break;
    }
}


ID2D1SolidColorBrush* UIManager::CreateBrush(D2D1::ColorF d2dColor)
{
    ID2D1SolidColorBrush* pd2dDefaultTextBrush = NULL;
    m_pd2dDeviceContext->CreateSolidColorBrush(d2dColor, &pd2dDefaultTextBrush);

    return(pd2dDefaultTextBrush);
}

IDWriteTextFormat* UIManager::CreateTextFormat(const WCHAR* pszFontName, float fFontSize)
{
    IDWriteTextFormat* pdwDefaultTextFormat = NULL;
    m_pd2dWriteFactory->CreateTextFormat(L"맑은 고딕", nullptr, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, fFontSize, L"en-us", &pdwDefaultTextFormat);

    pdwDefaultTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
    pdwDefaultTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
    //m_pd2dWriteFactory->CreateTextFormat(L"Arial", nullptr, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, fSmallFontSize, L"en-us", &m_pdwDefaultTextFormat);

    return(pdwDefaultTextFormat);
}

void UIManager::InitializeDevice(ID3D12Device5* pd3dDevice, ID3D12CommandQueue* pd3dCommandQueue, ID3D12Resource** ppd3dRenderTargets)
{    
    CreateD3D11On12Device(pd3dDevice,pd3dCommandQueue);
    CreateD2DDevice();
    CreateRenderTarget(ppd3dRenderTargets);;
    
    // 배경 리소스들
    m_backGround[0].resource = LoadPngFromFile(L"UI/Title.png");
    m_backGround[1].resource = LoadPngFromFile(L"UI/Lobby.png");
    m_backGround[2].resource = LoadPngFromFile(L"UI/Room.png");

    // 타이틀 씬에 필요한 버튼
    m_TitleButtons[0].resource      = LoadPngFromFile(L"UI/Title_Start.png");  
    m_TitleButtons[1].resource      = LoadPngFromFile(L"UI/Title_Quit.png");
    m_TitleButtons[0].d2dLayoutRect = MakeLayoutRect(CENTER_X + TITLEBUTTON_X_OFFSET, CENTER_Y + TITLEBUTTON_Y_OFFSET,     200, 50);
    m_TitleButtons[1].d2dLayoutRect = MakeLayoutRect(CENTER_X + TITLEBUTTON_X_OFFSET, CENTER_Y + TITLEBUTTON_Y_OFFSET * 2, 200, 50);
    // ID / PW 입력 창
    m_pTextBlocks[0].m_pd2dTextBrush = CreateBrush(D2D1::ColorF::White);
    m_pTextBlocks[0].m_pdwFormat = CreateTextFormat(L"", 20);
    m_pTextBlocks[0].m_pstrText = L"ID:";
    

    m_pTextBlocks[1].m_pd2dTextBrush = CreateBrush(D2D1::ColorF::White);
    m_pTextBlocks[1].m_pdwFormat = CreateTextFormat(L"", 20);
    m_pTextBlocks[1].m_pstrText = L"PW:";

    m_pTextBlocks[0].m_d2dLayoutRect = MakeLayoutRect(CENTER_X, CENTER_Y + 150, 200, FontSize);
    m_pTextBlocks[1].m_d2dLayoutRect = MakeLayoutRect(CENTER_X, CENTER_Y + 200, 200, FontSize);
    

    // 로비 씬에 필요한 버튼
    m_LobbyButtons[0].resource = LoadPngFromFile(L"UI/Enter_Game.png");
    m_LobbyButtons[1].resource = LoadPngFromFile(L"UI/New_Game.png");
    m_LobbyButtons[2].resource = LoadPngFromFile(L"UI/Quit_Lobby.png");

    m_LobbyButtons[0].d2dLayoutRect = MakeLayoutRectByCorner(LOBBYBUTTON_X_OFFSET,LOBBYBUTTON_Y_OFFSET, FRAME_BUFFER_WIDTH / 3.0f,          FRAME_BUFFER_HEIGHT / 4.0);
    m_LobbyButtons[1].d2dLayoutRect = MakeLayoutRectByCorner(0,                   LOBBYBUTTON_Y_OFFSET, FRAME_BUFFER_WIDTH / 3.0f,          FRAME_BUFFER_HEIGHT / 4.0);
    m_LobbyButtons[2].d2dLayoutRect = MakeLayoutRectByCorner(LOBBYBUTTON_X_OFFSET * 2.0f, LOBBYBUTTON_Y_OFFSET, FRAME_BUFFER_WIDTH / 3.0f,  FRAME_BUFFER_HEIGHT / 4.0);
    
    //로비에서 출력할 방 리스트 영역
    m_RoomListLayout = MakeLayoutRectByCorner(LOBBYROOMLIST_X_OFFSET,LOBBYROOMLIST_Y_OFFSET,
        FRAME_BUFFER_WIDTH - (LOBBYROOMLIST_X_OFFSET * 2.0), FRAME_BUFFER_HEIGHT / 2.0f);
    
    // 브러시들
     redBrush = CreateBrush(D2D1::ColorF::Red);
     grayBrush = CreateBrush(D2D1::ColorF::Gray);
     blackBrush = CreateBrush(D2D1::ColorF::Black);
}

void UIManager::Render2D(UINT nFrame, int32 curScene)
{

    ID3D11Resource* ppResources[] = { m_ppd3d11WrappedRenderTargets[nFrame] };

    m_pd2dDeviceContext->SetTarget(m_ppd2dRenderTargets[nFrame]);
    m_pd3d11On12Device->AcquireWrappedResources(ppResources, _countof(ppResources));

    m_pd2dDeviceContext->BeginDraw();
    DrawBackGround(curScene);
    DrawButton(curScene,0);
    DrawButton(curScene,1);
    DrawTextBlock(curScene);
    m_pd2dDeviceContext->EndDraw();

    m_pd3d11On12Device->ReleaseWrappedResources(ppResources, _countof(ppResources));
    m_pd3d11DeviceContext->Flush();
}