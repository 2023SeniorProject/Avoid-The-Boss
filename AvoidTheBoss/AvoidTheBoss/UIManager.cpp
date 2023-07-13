#include "pch.h"
#include "GameFramework.h"
#include "UIManager.h"
#include "SceneManager.h"
#include "OtherScenes.h"
#include <d2d1_1.h>
#include <wincodec.h>

#pragma comment(lib,"windowscodecs.lib")

D2D1_RECT_F MakeLayoutRect(float cx, float cy , float width, float height)
{
    return D2D1_RECT_F{ cx - width / 2.0f , cy - height / 2.0f , cx + width / 2.0f , cy + height / 2.0f };
}

D2D1_RECT_F MakeLayoutRectByCorner(float left, float top, float width, float height)
{
    return D2D1_RECT_F{ left , top , left + width, top + height};
}

UIManager::UIManager(UINT nFrames, ID3D12Device5* pd3dDevice, ID3D12CommandQueue* pd3dCommandQueue, ID3D12Resource** ppd3dRenderTargets, UINT nWidth, UINT nHeight)
{

    m_fWidth = static_cast<float>(nWidth);
    m_fHeight = static_cast<float>(nHeight);
    m_nRenderTargets = nFrames; // ���� Ÿ��
    m_ppd3d11WrappedRenderTargets = new ID3D11Resource * [nFrames];
    m_ppd2dRenderTargets = new ID2D1Bitmap1 * [nFrames];

    InitializeDevice(pd3dDevice, pd3dCommandQueue, ppd3dRenderTargets);

}



void UIManager::CreateD3D11On12Device(ID3D12Device5* pd3dDevice, ID3D12CommandQueue* pd3dCommandQueue)
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

ID2D1Bitmap1* UIManager::LoadPngFromFile(const wchar_t* filePath)
{
    ID2D1Bitmap1* bit = NULL;
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
                    
                    // IWICBitmap ������ ��Ʈ������ ID2D1Bitmap ��ü�� ����
                    if (S_OK == m_pd2dDeviceContext->CreateBitmapFromWicBitmap(pConverter, NULL, &bit)) result = 1;  // ���������� ������ ��� 
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

    if(result) return bit;  // PNG ������ ���� ����� ��ȯ�Ѵ�.

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


void UIManager::UpdateRoomTextBlocks(UINT nIndex,const WCHAR* pstrUIText, const D2D1_RECT_F& pd2dLayoutRect, bool hide)
{
    m_RoomListTextBlock[nIndex].m_pstrText.erase();
    m_RoomListTextBlock[nIndex].m_pstrText.append(pstrUIText);
    m_RoomListTextBlock[nIndex].m_d2dLayoutRect = pd2dLayoutRect;
    m_RoomListTextBlock[nIndex].m_hide = hide;
}

void UIManager::UpdateRoomText()
{
    
    CLobbyScene* ls = static_cast<CLobbyScene*>(mainGame.m_SceneManager->GetSceneByIdx((int32)CGameFramework::SCENESTATE::LOBBY));
    int32 curPage = ls->GetCurPage();
    int32 tempidx = -1;
    bool hide = false;
    WCHAR temp[2];
    
    std::wstring newText = L"RM:";
    D2D1_RECT_F newRect{ 0,0,0,0 };
    // ��ü �������� �����Ѵ�.
   
   for(int i = 0; i < m_nRoomListPerPage; ++i)
   { 
       int32 mem = ls->GetRoom((curPage * 5 + i)).member;
       ROOM_STATUS rs = ls->GetRoom(curPage * 5 + i).status;
           
       _itow_s(curPage * 5 + i, temp, 10); // ��� ��ȯ
       temp[2] = '\0';

       newText.append(temp);
       newText.append(L" ");
       temp[0] = L'\0';
       _itow_s(mem, temp, 10);
       newText.append(temp);
       newText.append(L"/4");

       newRect = MakeLayoutRectByCorner(LOBBYROOMLIST_X_OFFSET
           , LOBBYROOMLIST_Y_OFFSET + (FRAME_BUFFER_HEIGHT / 2.0f * ((float)i / m_nRoomListPerPage)),
           FRAME_BUFFER_WIDTH - (LOBBYROOMLIST_X_OFFSET * 2.0), FRAME_BUFFER_HEIGHT / 2.0f * (1.0 / m_nRoomListPerPage));

       if (rs == ROOM_STATUS::FULL || rs == ROOM_STATUS::EMPTY)
            {
                tempidx = i;
                hide = true;
                UpdateRoomTextBlocks(i, newText.c_str(), newRect, hide);
                hide = false;
                continue;
            }
       
       if (tempidx != -1)
       {
           newRect = MakeLayoutRectByCorner(LOBBYROOMLIST_X_OFFSET
               , LOBBYROOMLIST_Y_OFFSET + (FRAME_BUFFER_HEIGHT / 2.0f * ((float)tempidx / m_nRoomListPerPage)),
               FRAME_BUFFER_WIDTH - (LOBBYROOMLIST_X_OFFSET * 2.0), FRAME_BUFFER_HEIGHT / 2.0f * (1.0 / m_nRoomListPerPage));
           tempidx += 1;
       }
       UpdateRoomTextBlocks(i, newText.c_str(), newRect, hide);
   }
    
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
   
    if (Scene == 0) // Ÿ��Ʋ ��
    {
        m_pd2dDeviceContext->DrawRectangle(m_TitleButtons[0].d2dLayoutRect, redBrush);
        m_pd2dDeviceContext->DrawBitmap(m_TitleButtons[0].resource, m_TitleButtons[0].d2dLayoutRect, 1.0f, D2D1_INTERPOLATION_MODE_LINEAR, (D2D1_RECT_F*)0);
        m_pd2dDeviceContext->DrawRectangle(m_TitleButtons[1].d2dLayoutRect, redBrush);
        m_pd2dDeviceContext->DrawBitmap(m_TitleButtons[1].resource, m_TitleButtons[1].d2dLayoutRect, 1.0f, D2D1_INTERPOLATION_MODE_LINEAR, (D2D1_RECT_F*)0);

    }
    else if (Scene == 1) // �κ� ��
    {
        m_pd2dDeviceContext->DrawRectangle(m_LobbyButtons[0].d2dLayoutRect, redBrush);
        m_pd2dDeviceContext->DrawBitmap(m_LobbyButtons[0].resource, m_LobbyButtons[0].d2dLayoutRect, 1.0f, D2D1_INTERPOLATION_MODE_LINEAR, (D2D1_RECT_F*)0);
        
        m_pd2dDeviceContext->DrawRectangle(m_LobbyButtons[1].d2dLayoutRect, redBrush);
        m_pd2dDeviceContext->DrawBitmap(m_LobbyButtons[1].resource, m_LobbyButtons[1].d2dLayoutRect, 1.0f, D2D1_INTERPOLATION_MODE_LINEAR, (D2D1_RECT_F*)0);
        
        m_pd2dDeviceContext->DrawRectangle(m_LobbyButtons[2].d2dLayoutRect, redBrush);
        m_pd2dDeviceContext->DrawBitmap(m_LobbyButtons[2].resource, m_LobbyButtons[2].d2dLayoutRect, 1.0f, D2D1_INTERPOLATION_MODE_LINEAR, (D2D1_RECT_F*)0);
    }
    else if (Scene == 2) // ���� �� ��
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
        m_pd2dDeviceContext->FillRectangle(m_pIDPWTextBlocks[0].m_d2dLayoutRect, grayBrush);
        m_pd2dDeviceContext->FillRectangle(m_pIDPWTextBlocks[1].m_d2dLayoutRect, grayBrush);
        
        m_pd2dDeviceContext->DrawRectangle(m_pIDPWTextBlocks[0].m_d2dLayoutRect, blackBrush,4.0);
        m_pd2dDeviceContext->DrawRectangle(m_pIDPWTextBlocks[1].m_d2dLayoutRect, blackBrush,4.0);

        if(!m_pIDPWTextBlocks[0].m_hide) m_pd2dDeviceContext->DrawText(m_pIDPWTextBlocks[0].m_pstrText.c_str(),
            (UINT)wcslen(m_pIDPWTextBlocks[0].m_pstrText.c_str()), m_pIDPWTextBlocks[0].m_pdwFormat, 
            m_pIDPWTextBlocks[0].m_d2dLayoutRect, m_pIDPWTextBlocks[0].m_pd2dTextBrush);

        if (!m_pIDPWTextBlocks[1].m_hide) m_pd2dDeviceContext->DrawText(m_pIDPWTextBlocks[1].m_pstrText.c_str(),
            (UINT)wcslen(m_pIDPWTextBlocks[1].m_pstrText.c_str()), m_pIDPWTextBlocks[1].m_pdwFormat,
            m_pIDPWTextBlocks[1].m_d2dLayoutRect, m_pIDPWTextBlocks[1].m_pd2dTextBrush);
    }
    else if (Scene == 1)
    {
        for (int i = 0; i < m_nRoomListPerPage; ++i)
        {
            m_pd2dDeviceContext->DrawRectangle(m_RoomListLayout[i], blackBrush, 4.0f);
            if(!m_RoomListTextBlock[i].m_hide) m_pd2dDeviceContext->DrawText(m_RoomListTextBlock[i].m_pstrText.c_str()
                , (UINT)wcslen(m_RoomListTextBlock[i].m_pstrText.c_str()), m_RoomListTextBlock[i].m_pdwFormat
                , m_RoomListTextBlock[i].m_d2dLayoutRect, blackBrush);
        }
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
    m_pd2dWriteFactory->CreateTextFormat(L"���� ���", nullptr, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, fFontSize, L"en-us", &pdwDefaultTextFormat);

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
    
    // �귯�õ�
    redBrush = CreateBrush(D2D1::ColorF::Red);
    grayBrush = CreateBrush(D2D1::ColorF::Gray);
    blackBrush = CreateBrush(D2D1::ColorF::Black);
    // ��Ʈ
    m_TitleTextFormat = CreateTextFormat(L"", FRAME_BUFFER_HEIGHT / 30);
    m_LobbyTextFormat = CreateTextFormat(L"", FRAME_BUFFER_HEIGHT / 20);
    // ��� ���ҽ���
    m_backGround[0].resource = LoadPngFromFile(L"UI/Title.png");
    m_backGround[1].resource = LoadPngFromFile(L"UI/Lobby.png");
    m_backGround[2].resource = LoadPngFromFile(L"UI/Room.png");

    // Ÿ��Ʋ ���� �ʿ��� ��ư
    m_TitleButtons[0].resource      = LoadPngFromFile(L"UI/Title_Start.png");  
    m_TitleButtons[1].resource      = LoadPngFromFile(L"UI/Title_Quit.png");
    m_TitleButtons[0].d2dLayoutRect = MakeLayoutRect(CENTER_X + TITLEBUTTON_X_OFFSET, CENTER_Y + TITLEBUTTON_Y_OFFSET,     200, 50);
    m_TitleButtons[1].d2dLayoutRect = MakeLayoutRect(CENTER_X + TITLEBUTTON_X_OFFSET, CENTER_Y + TITLEBUTTON_Y_OFFSET * 2, 200, 50);
    // ID / PW �Է� â
    m_pIDPWTextBlocks[0].m_pd2dTextBrush = CreateBrush(D2D1::ColorF::White);
    m_pIDPWTextBlocks[0].m_pdwFormat = m_TitleTextFormat;
    m_pIDPWTextBlocks[0].m_pstrText = L"ID:";
    

    m_pIDPWTextBlocks[1].m_pd2dTextBrush = CreateBrush(D2D1::ColorF::White);
    m_pIDPWTextBlocks[1].m_pdwFormat = m_TitleTextFormat;
    m_pIDPWTextBlocks[1].m_pstrText = L"PW:";

    m_pIDPWTextBlocks[0].m_d2dLayoutRect = MakeLayoutRect(CENTER_X, CENTER_Y + 150, 200, FontSize);
    m_pIDPWTextBlocks[1].m_d2dLayoutRect = MakeLayoutRect(CENTER_X, CENTER_Y + 200, 200, FontSize);
    

    // �κ� ���� �ʿ��� ��ư
    m_LobbyButtons[0].resource = LoadPngFromFile(L"UI/Enter_Game.png");
    m_LobbyButtons[1].resource = LoadPngFromFile(L"UI/New_Game.png");
    m_LobbyButtons[2].resource = LoadPngFromFile(L"UI/Quit_Lobby.png");

    m_LobbyButtons[0].d2dLayoutRect = MakeLayoutRectByCorner(LOBBYBUTTON_X_OFFSET,LOBBYBUTTON_Y_OFFSET, FRAME_BUFFER_WIDTH / 3.0f,          FRAME_BUFFER_HEIGHT / 4.0);
    m_LobbyButtons[1].d2dLayoutRect = MakeLayoutRectByCorner(0,                   LOBBYBUTTON_Y_OFFSET, FRAME_BUFFER_WIDTH / 3.0f,          FRAME_BUFFER_HEIGHT / 4.0);
    m_LobbyButtons[2].d2dLayoutRect = MakeLayoutRectByCorner(LOBBYBUTTON_X_OFFSET * 2.0f, LOBBYBUTTON_Y_OFFSET, FRAME_BUFFER_WIDTH / 3.0f,  FRAME_BUFFER_HEIGHT / 4.0);
    
    //�κ񿡼� ����� �� ����Ʈ ����
    for (int i = 0; i < m_nRoomListPerPage; ++i)
    {
        m_RoomListLayout[i] = MakeLayoutRectByCorner(LOBBYROOMLIST_X_OFFSET
            , LOBBYROOMLIST_Y_OFFSET + (FRAME_BUFFER_HEIGHT / 2.0f * ((float)i / m_nRoomListPerPage)),
            FRAME_BUFFER_WIDTH - (LOBBYROOMLIST_X_OFFSET * 2.0), FRAME_BUFFER_HEIGHT / 2.0f * (1.0 / m_nRoomListPerPage));
        m_RoomListTextBlock[i].m_d2dLayoutRect = m_RoomListLayout[i];
        m_RoomListTextBlock[i].m_pdwFormat = m_LobbyTextFormat;
        m_RoomListTextBlock[i].m_pstrText =  L"ROOMNUM:   MEMBER:   0/4";
    }

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