//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

#include "../stdafx.h"
#include "DxSampleHelper.h"
#include "UILayer.h"
#include "DXSample.h"
#include "D3D12RaytracingRealTimeDenoisedAmbientOcclusion.h"


#include <wincodec.h>
#pragma comment(lib,"windowscodecs.lib")

using namespace std;

D2D1_RECT_F MakeLayoutRect(float cx, float cy, float width, float height)
{
    return D2D1_RECT_F{ cx - width / 2.0f , cy - height / 2.0f , cx + width / 2.0f , cy + height / 2.0f };
}

D2D1_RECT_F MakeLayoutRectByCorner(float left, float top, float width, float height)
{
    return D2D1_RECT_F{ left , top , left + width, top + height };
}

ID2D1SolidColorBrush* UILayer::CreateBrush(D2D1::ColorF d2dColor)
{
    ID2D1SolidColorBrush* pd2dDefaultTextBrush = NULL;
    m_d2dDeviceContext->CreateSolidColorBrush(d2dColor, &pd2dDefaultTextBrush);

    return(pd2dDefaultTextBrush);
}

IDWriteTextFormat* UILayer::CreateTextFormat(const WCHAR* pszFontName, float fFontSize)
{
    IDWriteTextFormat* pdwDefaultTextFormat = NULL;
    m_dwriteFactory->CreateTextFormat(pszFontName, nullptr, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, fFontSize, L"en-us", &pdwDefaultTextFormat);

    pdwDefaultTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
    pdwDefaultTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
    
    return(pdwDefaultTextFormat);
}


UILayer::UILayer(UINT frameCount, ID3D12Device5* pDevice, ID3D12CommandQueue* pCommandQueue) :
    m_width(0.0f),
    m_height(0.0f)
{
    m_wrappedRenderTargets.resize(frameCount);
    m_d2dRenderTargets.resize(frameCount);
    m_textBlocks.resize(1);
    Initialize(pDevice, pCommandQueue);
}

void UILayer::Initialize(ID3D12Device5* pDevice, ID3D12CommandQueue* pCommandQueue)
{
    UINT d3d11DeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
    D2D1_FACTORY_OPTIONS d2dFactoryOptions = {};

#if defined(_DEBUG) || defined(DBG)
    // Enable the D2D debug layer.
    d2dFactoryOptions.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;

    // Enable the D3D11 debug layer.
    d3d11DeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    // Create an 11 device wrapped around the 12 device and share
    // 12's command queue.
    ComPtr<ID3D11Device> d3d11Device;
    ID3D12CommandQueue* ppCommandQueues[] = { pCommandQueue };
    ThrowIfFailed(D3D11On12CreateDevice(
        pDevice,
        d3d11DeviceFlags,
        nullptr,
        0,
        reinterpret_cast<IUnknown**>(ppCommandQueues),
        _countof(ppCommandQueues),
        0,
        &d3d11Device,
        &m_d3d11DeviceContext,
        nullptr
        ));

    // Query the 11On12 device from the 11 device.
    ThrowIfFailed(d3d11Device.As(&m_d3d11On12Device));

#if defined(_DEBUG) || defined(DBG)
    // Filter a debug error coming from the 11on12 layer.
    ComPtr<ID3D12InfoQueue> infoQueue;
    if (SUCCEEDED(pDevice->QueryInterface(IID_PPV_ARGS(&infoQueue))))
    {
        // Suppress messages based on their severity level.
        D3D12_MESSAGE_SEVERITY severities[] =
        {
            D3D12_MESSAGE_SEVERITY_INFO,
        };

        // Suppress individual messages by their ID.
        D3D12_MESSAGE_ID denyIds[] =
        {
            // This occurs when there are uninitialized descriptors in a descriptor table, even when a
            // shader does not access the missing descriptors.
            D3D12_MESSAGE_ID_INVALID_DESCRIPTOR_HANDLE,
        };

        D3D12_INFO_QUEUE_FILTER filter = {};
        filter.DenyList.NumSeverities = _countof(severities);
        filter.DenyList.pSeverityList = severities;
        filter.DenyList.NumIDs = _countof(denyIds);
        filter.DenyList.pIDList = denyIds;

        ThrowIfFailed(infoQueue->PushStorageFilter(&filter));
    }
#endif

    // Create D2D/DWrite components.
    {
        ComPtr<IDXGIDevice> dxgiDevice;
        ThrowIfFailed(m_d3d11On12Device.As(&dxgiDevice));

        ThrowIfFailed(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory3), &d2dFactoryOptions, &m_d2dFactory));
        ThrowIfFailed(m_d2dFactory->CreateDevice(dxgiDevice.Get(), &m_d2dDevice));
        ThrowIfFailed(m_d2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &m_d2dDeviceContext));

        m_d2dDeviceContext->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);
        ThrowIfFailed(m_d2dDeviceContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Goldenrod), &m_textBrush));

        ThrowIfFailed(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), &m_dwriteFactory));
    }



    // 브러시들
    redBrush = CreateBrush(D2D1::ColorF::Red);
    grayBrush = CreateBrush(D2D1::ColorF::Gray);
    blackBrush = CreateBrush(D2D1::ColorF::Black);
    // 폰트
    m_TitleTextFormat = CreateTextFormat(L"맑은 고딕", 40);
    m_LobbyTextFormat = CreateTextFormat(L"맑은 고딕", 40);
    // 배경 리소스들
    // 
    // 배경 리소스들
    m_backGround[0].resource = LoadPngFromFile(L"UI/Title.png");
    m_backGround[1].resource = LoadPngFromFile(L"UI/Lobby.png");
    m_backGround[2].resource = LoadPngFromFile(L"UI/Game.png");

    m_backGround[3].resource = LoadPngFromFile(L"UI/Boss_Result.png");
    m_backGround[4].resource = LoadPngFromFile(L"UI/Emp_Result.png");

    // 타이틀 씬에 필요한 버튼
    m_TitleButtons[0].resource = LoadPngFromFile(L"UI/Title_Start.png");
    m_TitleButtons[1].resource = LoadPngFromFile(L"UI/Title_Quit.png");
    m_TitleButtons[2].resource = LoadPngFromFile(L"UI/Title_Register.png");

    m_TitleButtons[0].d2dLayoutRect = MakeLayoutRect(CENTER_X + TITLEBUTTON_X_OFFSET, CENTER_Y + TITLEBUTTON_Y_OFFSET, 200, 50);
    m_TitleButtons[1].d2dLayoutRect = MakeLayoutRect(CENTER_X, CENTER_Y + TITLEBUTTON_Y_OFFSET * 4.5, 200, 50);
    m_TitleButtons[2].d2dLayoutRect = MakeLayoutRect(CENTER_X - TITLEBUTTON_X_OFFSET, CENTER_Y + TITLEBUTTON_Y_OFFSET, 200, 50);
    // ID / PW 입력 창
    m_IDPWTextBlocks[0].m_pd2dTextBrush = CreateBrush(D2D1::ColorF::White);
    m_IDPWTextBlocks[0].m_pdwFormat = m_TitleTextFormat;
    m_IDPWTextBlocks[0].m_pstrText = L"";


    m_IDPWTextBlocks[1].m_pd2dTextBrush = CreateBrush(D2D1::ColorF::White);
    m_IDPWTextBlocks[1].m_pdwFormat = m_TitleTextFormat;
    m_IDPWTextBlocks[1].m_pstrText = L"";

    m_IDPWTextBlocks[0].m_d2dLayoutRect = MakeLayoutRect(CENTER_X, CENTER_Y + FRAME_BUFFER_HEIGHT / 4.0f, 400, FontSize);
    m_IDPWTextBlocks[1].m_d2dLayoutRect = MakeLayoutRect(CENTER_X, CENTER_Y + FRAME_BUFFER_HEIGHT / 3.0f, 400, FontSize);

    m_LoginResult[0].resource = LoadPngFromFile(L"UI/LOGIN_OK.png");
    m_LoginResult[1].resource = LoadPngFromFile(L"UI/LOGIN_FAIL.png");
    m_LoginResult[2].resource = LoadPngFromFile(L"UI/REG.png");

    for (int i = 0; i < 3; ++i)
    {
        m_LoginResult[i].d2dLayoutRect = MakeLayoutRect(CENTER_X, CENTER_Y, 300, 100);
        m_LoginResult[i].m_hide = true;
    }


    // 로비 씬에 필요한 버튼
    m_LobbyButtons[0].resource = LoadPngFromFile(L"UI/Enter_Room.png");
    m_LobbyButtons[1].resource = LoadPngFromFile(L"UI/Create_Room.png");
    m_LobbyButtons[2].resource = LoadPngFromFile(L"UI/Quit_Lobby.png");

    m_LobbyButtons[0].d2dLayoutRect = MakeLayoutRectByCorner(LOBBYBUTTON_X_OFFSET, LOBBYBUTTON_Y_OFFSET, FRAME_BUFFER_WIDTH / 3.0f, FRAME_BUFFER_HEIGHT / 4.0);
    m_LobbyButtons[1].d2dLayoutRect = MakeLayoutRectByCorner(0, LOBBYBUTTON_Y_OFFSET, FRAME_BUFFER_WIDTH / 3.0f, FRAME_BUFFER_HEIGHT / 4.0);
    m_LobbyButtons[2].d2dLayoutRect = MakeLayoutRectByCorner(LOBBYBUTTON_X_OFFSET * 2.0f, LOBBYBUTTON_Y_OFFSET, FRAME_BUFFER_WIDTH / 3.0f, FRAME_BUFFER_HEIGHT / 4.0);


    m_RoomButtons[0].resource = LoadPngFromFile(L"UI/Ready_Game.png");
    m_RoomButtons[1].resource = LoadPngFromFile(L"UI/Quit_Game.png");
    m_RoomButtons[0].d2dLayoutRect = MakeLayoutRectByCorner(GAMEROOM_BUTTON_X_OFFSET, GAMEROOM_BUTTON_Y_OFFSET, FRAME_BUFFER_WIDTH / 5.0f, FRAME_BUFFER_HEIGHT / 7.0);
    m_RoomButtons[1].d2dLayoutRect = MakeLayoutRectByCorner(GAMEROOM_BUTTON_X_OFFSET + (FRAME_BUFFER_WIDTH / 5.0f), GAMEROOM_BUTTON_Y_OFFSET, FRAME_BUFFER_WIDTH / 5.0f, FRAME_BUFFER_HEIGHT / 7.0);

    m_ReadyBitmaps[0].resource = LoadPngFromFile(L"UI/Ready.png");
    m_ReadyBitmaps[1].resource = LoadPngFromFile(L"UI/Ready2.png");
    m_ReadyBitmaps[2].resource = LoadPngFromFile(L"UI/Ready3.png");
    m_ReadyBitmaps[3].resource = LoadPngFromFile(L"UI/Ready4.png");

    m_ReadyCard[0].resource = LoadPngFromFile(L"UI/READY_CARD1.png");
    m_ReadyCard[1].resource = LoadPngFromFile(L"UI/READY_CARD2.png");
    m_ReadyCard[2].resource = LoadPngFromFile(L"UI/READY_CARD3.png");
    m_ReadyCard[3].resource = LoadPngFromFile(L"UI/READY_CARD4.png");

    m_ReadyBitmaps[0].d2dLayoutRect = MakeLayoutRectByCorner(LOBBYROOMLIST_X_OFFSET,
        LOBBYROOMLIST_Y_OFFSET,
        (FRAME_BUFFER_WIDTH - (LOBBYROOMLIST_X_OFFSET * 2.0)) / 2.0,
        FRAME_BUFFER_HEIGHT / 6.0);
    m_ReadyBitmaps[1].d2dLayoutRect = MakeLayoutRectByCorner(LOBBYROOMLIST_X_OFFSET + ((FRAME_BUFFER_WIDTH - (LOBBYROOMLIST_X_OFFSET * 2.0)) / 2.0),
        LOBBYROOMLIST_Y_OFFSET,
        (FRAME_BUFFER_WIDTH - (LOBBYROOMLIST_X_OFFSET * 2.0)) / 2.0,
        FRAME_BUFFER_HEIGHT / 6.0);

    m_ReadyBitmaps[2].d2dLayoutRect = MakeLayoutRectByCorner(LOBBYROOMLIST_X_OFFSET,
        LOBBYROOMLIST_Y_OFFSET + FRAME_BUFFER_HEIGHT / 6.0,
        (FRAME_BUFFER_WIDTH - (LOBBYROOMLIST_X_OFFSET * 2.0)) / 2.0,
        FRAME_BUFFER_HEIGHT / 6.0);

    m_ReadyBitmaps[3].d2dLayoutRect = MakeLayoutRectByCorner(
        LOBBYROOMLIST_X_OFFSET + (FRAME_BUFFER_WIDTH - (LOBBYROOMLIST_X_OFFSET * 2.0)) / 2.0,
        LOBBYROOMLIST_Y_OFFSET + FRAME_BUFFER_HEIGHT / 6.0,
        (FRAME_BUFFER_WIDTH - (LOBBYROOMLIST_X_OFFSET * 2.0)) / 2.0,
        FRAME_BUFFER_HEIGHT / 6.0f);

    for (int i = 0; i < 4; ++i)
    {
        m_ReadyCard[i].d2dLayoutRect = m_ReadyBitmaps[i].d2dLayoutRect;
        m_ReadyCard[i].m_hide = true;
    }

    //로비에서 출력할 방 리스트 영역
    for (int i = 0; i < m_nRoomListPerPage; ++i)
    {
        m_RoomListLayout[i] = MakeLayoutRectByCorner(LOBBYROOMLIST_X_OFFSET
            , LOBBYROOMLIST_Y_OFFSET + (FRAME_BUFFER_HEIGHT / 2.0f * ((float)i / m_nRoomListPerPage)),
            FRAME_BUFFER_WIDTH - (LOBBYROOMLIST_X_OFFSET * 2.0), FRAME_BUFFER_HEIGHT / 2.0f * (1.0 / m_nRoomListPerPage));
        m_RoomListTextBlock[i].m_d2dLayoutRect = m_RoomListLayout[i];
        m_RoomListTextBlock[i].m_pdwFormat = m_LobbyTextFormat;
        m_RoomListTextBlock[i].m_pstrText = L"ROOMNUM:   MEMBER:   0/4";
    }

    // 인게임 비트맵 로드
    // 캐릭터 프로필
    m_CharProfile[0].resource = LoadPngFromFile(L"UI/Char_UI_1.png"); // Boss
    m_CharProfile[1].resource = LoadPngFromFile(L"UI/Char_UI_2.png"); // Yellow
    m_CharProfile[2].resource = LoadPngFromFile(L"UI/Char_UI_3.png"); // Mask 
    m_CharProfile[3].resource = LoadPngFromFile(L"UI/Char_UI_5.png"); // Goggle
    for (int i = 0; i < PLAYERNUM; ++i)
        m_CharProfile[i].d2dLayoutRect = MakeLayoutRectByCorner(FRAME_BUFFER_WIDTH * 0.01, FRAME_BUFFER_HEIGHT * 0.1 * i, FRAME_BUFFER_WIDTH * 0.1, FRAME_BUFFER_HEIGHT * 0.1);

    // 상태 --> 동적으로 변하는 것이므로 그때 그때 위치를 업데이트하기로 한다. 일단 비트맵 리소스만 로드한다.
    m_CharStatusBitmaps[0] = LoadPngFromFile(L"UI/Normal.png");
    m_CharStatusBitmaps[1] = LoadPngFromFile(L"UI/Danger.png");
    m_CharStatusBitmaps[2] = LoadPngFromFile(L"UI/Dead.png");

    // HP
    m_HpBitmap = LoadPngFromFile(L"UI/HP.png");
    for (int i = 0; i < MAX_HP; ++i)
    {
        m_HPUi[i].resource = m_HpBitmap;
        m_HPUi[i].d2dLayoutRect = MakeLayoutRectByCorner(PROFILE_UI_OFFSET_X + BIG_PROFILE_UI_WIDTH + STATUS_UI_WIDTH * i,
            BIG_PROFILE_UI_OFFSET_Y + BIG_PROFILE_UI_WIDTH / 4.0, STATUS_UI_WIDTH, STATUS_UI_HEIGHT);
    }
    // 크로스 헤드
    m_CharCrossHead.resource = LoadPngFromFile(L"UI/crossHair.png");
    m_CharCrossHead.d2dLayoutRect = MakeLayoutRect(CENTER_X, CENTER_Y, 10.f, 10.f);
    m_CharCrossHead.m_hide = false;

    // 피격 이펙트
    m_AttackedEffect.resource = LoadPngFromFile(L"UI/Attacked.png");
    m_AttackedEffect.d2dLayoutRect = MakeLayoutRectByCorner(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
    m_AttackedEffect.m_hide = false;


    // 발전기 게이지
    for (int i = 0; i < 20; ++i)
    {
        std::wstring filename;
        filename = L"UI/Generator_Gauge_";
        WCHAR num[3];
        _itow_s(i + 1, num, 10);
        num[2] = L'\0';
        filename.append(num);
        filename.append(L".png");
        m_GenerateUIButtons[i].resource = LoadPngFromFile(filename.c_str());
        m_GenerateUIButtons[i].d2dLayoutRect = MakeLayoutRect(FRAME_BUFFER_WIDTH / 2.0, FRAME_BUFFER_HEIGHT * 0.8, FRAME_BUFFER_WIDTH / 2, FRAME_BUFFER_HEIGHT / 4.f);
        m_GenerateUIButtons[i].m_hide = true;
    }
    m_GenerateUIButtons[20].resource = LoadPngFromFile(L"UI/F.png");
    m_GenerateUIButtons[20].d2dLayoutRect = MakeLayoutRect(FRAME_BUFFER_WIDTH / 2.0, FRAME_BUFFER_HEIGHT * 0.9, FRAME_BUFFER_WIDTH * 0.1f, FRAME_BUFFER_HEIGHT * 0.1f);
    m_GenerateUIButtons[20].m_hide = true;

    // 살리기 아이콘
    m_RescueIcon.resource = LoadPngFromFile(L"UI/Rescue.png");
    m_RescueIcon.d2dLayoutRect = m_GenerateUIButtons[20].d2dLayoutRect;
    m_RescueIcon.m_hide = true;

    m_RescueGuage.m_hide = true;
    m_RescueGuage.d2dLayoutRect[0] = MakeLayoutRect(CENTER_X, CENTER_Y, MAX_RESCUE_GUAGE, 50);
    m_RescueGuage.d2dLayoutRect[1] = MakeLayoutRect(CENTER_X, CENTER_Y, MAX_RESCUE_GUAGE, 50);
    //
}

void UILayer::UpdateLabels(const wstring& uiText)
{
    // Update the UI elements.
    m_textBlocks[0] = { uiText, D2D1::RectF(0.0f, 0.0f, m_width, m_height), m_textFormat.Get() };
}

void UILayer::Render(UINT frameIndex)
{
    ID3D11Resource* ppResources[] = { m_wrappedRenderTargets[frameIndex].Get() };

    m_d2dDeviceContext->SetTarget(m_d2dRenderTargets[frameIndex].Get());

    // Acquire our wrapped render target resource for the current back buffer.
    m_d3d11On12Device->AcquireWrappedResources(ppResources, _countof(ppResources));
    m_d2dDeviceContext->BeginDraw();
    
    m_d2dDeviceContext->EndDraw();

    // Release our wrapped render target resource. Releasing
    // transitions the back buffer resource to the state specified
    // as the OutState when the wrapped resource was created.
    m_d3d11On12Device->ReleaseWrappedResources(ppResources, _countof(ppResources));

    // Flush to submit the 11 command list to the shared command queue.
    m_d3d11DeviceContext->Flush();
}

// Releases resources that reference the DXGI swap chain so that it can be resized.
void UILayer::ReleaseResources()
{
    for (UINT i = 0; i < FrameCount(); i++)
    {
        ID3D11Resource* ppResources[] = { m_wrappedRenderTargets[i].Get() };
        m_d3d11On12Device->ReleaseWrappedResources(ppResources, _countof(ppResources));
    }
    m_d2dDeviceContext->SetTarget(nullptr);
    m_d3d11DeviceContext->Flush();
    for (UINT i = 0; i < FrameCount(); i++)
    {
        m_d2dRenderTargets[i].Reset();
        m_wrappedRenderTargets[i].Reset();
    }
    m_textBrush.Reset();
    m_d2dDeviceContext.Reset();
    m_textFormat.Reset();
    m_dwriteFactory.Reset();
    m_d2dDevice.Reset();
    m_d2dFactory.Reset();
    m_d3d11DeviceContext.Reset();
    m_d3d11On12Device.Reset();
}

void UILayer::Resize(ComPtr<ID3D12Resource>* ppRenderTargets, UINT width, UINT height)
{
    m_width = static_cast<float>(width);
    m_height = static_cast<float>(height);

    // Query the desktop's dpi settings, which will be used to create
    // D2D's render targets.
    D2D1_BITMAP_PROPERTIES1 bitmapProperties = D2D1::BitmapProperties1(
        D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
        D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED));

    // Create a wrapped 11On12 resource of this back buffer. Since we are 
    // rendering all D3D12 content first and then all D2D content, we specify 
    // the In resource state as RENDER_TARGET - because D3D12 will have last 
    // used it in this state - and the Out resource state as PRESENT. When 
    // ReleaseWrappedResources() is called on the 11On12 device, the resource 
    // will be transitioned to the PRESENT state.
    for (UINT i = 0; i < FrameCount(); i++)
    {
        D3D11_RESOURCE_FLAGS d3d11Flags = { D3D11_BIND_RENDER_TARGET };
        ThrowIfFailed(m_d3d11On12Device->CreateWrappedResource(
            ppRenderTargets[i].Get(),
            &d3d11Flags,
            D3D12_RESOURCE_STATE_RENDER_TARGET,
            D3D12_RESOURCE_STATE_PRESENT,
            IID_PPV_ARGS(&m_wrappedRenderTargets[i])));

        // Create a render target for D2D to draw directly to this back buffer.
        ComPtr<IDXGISurface> surface;
        ThrowIfFailed(m_wrappedRenderTargets[i].As(&surface));
        ThrowIfFailed(m_d2dDeviceContext->CreateBitmapFromDxgiSurface(
            surface.Get(),
            &bitmapProperties,
            &m_d2dRenderTargets[i]));
    }

    ThrowIfFailed(m_d2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &m_d2dDeviceContext));
    m_d2dDeviceContext->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);
    ThrowIfFailed(m_d2dDeviceContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Goldenrod), &m_textBrush));

    // Create DWrite text format objects.
    const float fontSize = m_height / 30.0f;
    const float smallFontSize = m_height / 60.0f;

    ThrowIfFailed(m_dwriteFactory->CreateTextFormat(
        L"Arial",
        nullptr,
        DWRITE_FONT_WEIGHT_NORMAL,
        DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL,
        fontSize,
        L"en-us",
        &m_textFormat));

    ThrowIfFailed(m_textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER));
    ThrowIfFailed(m_textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR));

    ThrowIfFailed(m_dwriteFactory->CreateTextFormat(
        L"Arial",
        nullptr,
        DWRITE_FONT_WEIGHT_NORMAL,
        DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL,
        smallFontSize,
        L"en-us",
        &m_textFormat));
}

void UILayer::DrawOtherSceneBackGround(int32 Scene, D3D12RaytracingRealTimeDenoisedAmbientOcclusion* sample)
{
    switch (Scene)
    {
    case 0:
    case 1:
    case 2:
        m_d2dDeviceContext->DrawBitmap(m_backGround[Scene].resource, D2D1_RECT_F{ 0,0,m_width,m_height });
        break;
    case 3:
        break;
    case 4:
        if (static_cast<CResultScene*>(sample->GetSceneManager()->GetSceneByIdx(4))->m_case == 1)
        {
            m_d2dDeviceContext->DrawBitmap(m_backGround[3].resource, D2D1_RECT_F{ 0,0,m_width,m_height });
        }
        else m_d2dDeviceContext->DrawBitmap(m_backGround[4].resource, D2D1_RECT_F{ 0,0,m_width,m_height });

        break;
    }
}

void UILayer::DrawOtherSceneUI(int32 Scene, int32 idx)
{
}

void UILayer::DrawOtherSceneUITextBlock(int32 Scene)
{
}

void UILayer::InitGameSceneUI(CGameScene*)
{
}

void UILayer::UpdateGameSceneUI(CGameScene*)
{
}

void UILayer::DrawGameSceneUI(int32 Scene)
{
}

ID2D1Bitmap1* UILayer::LoadPngFromFile(const wchar_t* filePath)
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
                    if (S_OK == m_d2dDeviceContext->CreateBitmapFromWicBitmap(pConverter, NULL, &bit)) result = 1;  // 성공적으로 생성한 경우 
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

    if (result) return bit;  // PNG 파일을 읽은 결과를 반환한다.

    return nullptr;
}
