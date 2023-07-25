#include "pch.h"
#include "GameFramework.h"
#include "UIManager.h"

#include "CBoss.h"
#include "CEmployee.h"
#include "CGenerator.h"

#include "SceneManager.h"

#include "OtherScenes.h"
#include "GameScene.h"

#include <d2d1_1.h>
#include <wincodec.h>

const int32 PROFILE_UI_OFFSET_X = FRAME_BUFFER_WIDTH * 0.01;
const int32 PROFILE_UI_OFFSET_Y = FRAME_BUFFER_HEIGHT * 0.1;

const int32 PROFILE_UI_WIDTH = FRAME_BUFFER_WIDTH * 0.1;
const int32 PROFILE_UI_HEIGHT = FRAME_BUFFER_HEIGHT * 0.1;

const int32 BIG_PROFILE_UI_OFFSET_Y = PROFILE_UI_OFFSET_Y * 7;
const int32 BIG_PROFILE_UI_WIDTH = FRAME_BUFFER_WIDTH * 0.2;
const int32 BIG_PROFILE_UI_HEIGHT = FRAME_BUFFER_HEIGHT * 0.2;

const int32 STATUS_UI_WIDTH = PROFILE_UI_WIDTH * 0.8;
const int32 STATUS_UI_HEIGHT = PROFILE_UI_HEIGHT * 0.8;

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
    //m_RoomListTextBlock[nIndex].m_d2dLayoutRect = pd2dLayoutRect;
    m_RoomListTextBlock[nIndex].m_hide = hide;
}

void UIManager::UpdateRoomText()
{
    
    CLobbyScene* ls = static_cast<CLobbyScene*>(mainGame.m_SceneManager->GetSceneByIdx((int32)CGameFramework::SCENESTATE::LOBBY));
    int32 curPage = ls->GetCurPage();
    bool hide = false;
    WCHAR temp[3];
    
    
    D2D1_RECT_F newRect{ 0,0,0,0 };
    // ��ü �������� �����Ѵ�.
   
   for(int i = 0; i < m_nRoomListPerPage; ++i)
   { 
       std::wstring newText = L"ROOM [";
       int32 mem = ls->GetRoom((curPage * 5 + i)).member;
       ROOM_STATUS rs = ls->GetRoom(curPage * 5 + i).status;
           
       _itow_s(curPage * 5 + i, temp, 10); // ��� ��ȯ
       temp[2] = '\0';

       newText.append(temp);
       newText.append(L"]");
       temp[0] = L'\0';
       _itow_s(mem, temp, 10);
       temp[2] = L'\0';

       newText.append(temp);
       newText.append(L"/4");

       if (rs == ROOM_STATUS::FULL || rs == ROOM_STATUS::EMPTY || rs == ROOM_STATUS::INGAME)
       {
          UpdateRoomTextBlocks(i, L"", newRect, true);      
          continue;
       }
       else UpdateRoomTextBlocks(i, newText.c_str(), newRect, false);
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

void UIManager::DrawOtherSceneBackGround(int32 Scene)
{
    switch (Scene)
    {
    case 0:
    case 1:
    case 2:
        m_pd2dDeviceContext->DrawBitmap(m_backGround[Scene].resource, D2D1_RECT_F{ 0,0,m_fWidth,m_fHeight });
        break;
    case 3:
        break;
    }
}

void UIManager::DrawOtherSceneUI(int32 Scene,int32 idx)
{
    if (Scene == 0) // Ÿ��Ʋ ��
    {
        //m_pd2dDeviceContext->DrawRectangle(m_TitleButtons[0].d2dLayoutRect, redBrush);
        m_pd2dDeviceContext->DrawBitmap(m_TitleButtons[0].resource, m_TitleButtons[0].d2dLayoutRect);
        //m_pd2dDeviceContext->DrawRectangle(m_TitleButtons[1].d2dLayoutRect, redBrush);
        m_pd2dDeviceContext->DrawBitmap(m_TitleButtons[1].resource, m_TitleButtons[1].d2dLayoutRect);

    }
    else if (Scene == 1) // �κ� ��
    {
        //m_pd2dDeviceContext->DrawRectangle(m_LobbyButtons[0].d2dLayoutRect, redBrush);
        m_pd2dDeviceContext->DrawBitmap(m_LobbyButtons[0].resource, m_LobbyButtons[0].d2dLayoutRect);
        
        //m_pd2dDeviceContext->DrawRectangle(m_LobbyButtons[1].d2dLayoutRect, redBrush);
        m_pd2dDeviceContext->DrawBitmap(m_LobbyButtons[1].resource, m_LobbyButtons[1].d2dLayoutRect);
        
        //m_pd2dDeviceContext->DrawRectangle(m_LobbyButtons[2].d2dLayoutRect, redBrush);
        m_pd2dDeviceContext->DrawBitmap(m_LobbyButtons[2].resource, m_LobbyButtons[2].d2dLayoutRect);
    }
    else if (Scene == 2) // ���� �� ��
    {
        CRoomScene* rs = static_cast<CRoomScene*>(mainGame.m_SceneManager->GetSceneByIdx((int32)CGameFramework::SCENESTATE::ROOM));
        m_pd2dDeviceContext->DrawRectangle(m_RoomButtons[0].d2dLayoutRect, redBrush);
        m_pd2dDeviceContext->DrawBitmap(m_RoomButtons[0].resource, m_RoomButtons[0].d2dLayoutRect);
        
        m_pd2dDeviceContext->DrawRectangle(m_RoomButtons[1].d2dLayoutRect, redBrush);
        m_pd2dDeviceContext->DrawBitmap(m_RoomButtons[1].resource, m_RoomButtons[1].d2dLayoutRect);
      
        for (int i = 0; i < 4; ++i)
        {
            m_pd2dDeviceContext->DrawRectangle(m_ReadyBitmaps[i].d2dLayoutRect,blackBrush, 6.0f);
            m_pd2dDeviceContext->FillRectangle(m_ReadyBitmaps[i].d2dLayoutRect, grayBrush);
            if(rs->m_members[i].isReady)  
                m_pd2dDeviceContext->DrawBitmap(m_ReadyBitmaps[i].resource, m_ReadyBitmaps[i].d2dLayoutRect);
        }
    }
}

void UIManager::DrawOtherSceneUITextBlock(int32 Scene)
{

    if (Scene == 0)
    {
        m_pd2dDeviceContext->FillRectangle(m_IDPWTextBlocks[0].m_d2dLayoutRect, grayBrush);
        m_pd2dDeviceContext->FillRectangle(m_IDPWTextBlocks[1].m_d2dLayoutRect, grayBrush);
        
        m_pd2dDeviceContext->DrawRectangle(m_IDPWTextBlocks[0].m_d2dLayoutRect, blackBrush,4.0);
        m_pd2dDeviceContext->DrawRectangle(m_IDPWTextBlocks[1].m_d2dLayoutRect, blackBrush,4.0);

        if(!m_IDPWTextBlocks[0].m_hide) m_pd2dDeviceContext->DrawText(m_IDPWTextBlocks[0].m_pstrText.c_str(),
            (UINT)wcslen(m_IDPWTextBlocks[0].m_pstrText.c_str()), m_IDPWTextBlocks[0].m_pdwFormat, 
            m_IDPWTextBlocks[0].m_d2dLayoutRect, m_IDPWTextBlocks[0].m_pd2dTextBrush);

        if (!m_IDPWTextBlocks[1].m_hide) m_pd2dDeviceContext->DrawText(m_IDPWTextBlocks[1].m_pstrText.c_str(),
            (UINT)wcslen(m_IDPWTextBlocks[1].m_pstrText.c_str()), m_IDPWTextBlocks[1].m_pdwFormat,
            m_IDPWTextBlocks[1].m_d2dLayoutRect, m_IDPWTextBlocks[1].m_pd2dTextBrush);
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
        if (m_selectedLayout >= 0)  m_pd2dDeviceContext->DrawRectangle(m_RoomListLayout[m_selectedLayout], redBrush, 4.0f);
    }
}

void UIManager::InitGameSceneUI(CGameScene* gc)
{
    m_CharProfile[gc->m_playerIdx].m_hide = true;
    for (auto& i : m_GenerateUIButtons) i.m_hide = true;
    m_playerIdx = gc->m_playerIdx;
    // �÷��̾� ���� UI ���
    switch (m_playerIdx)
    {
        case 0:
        {
           // ������� 3�� ��� ���
            for (int i = 1; i < 4; ++i)
            {
                m_CharProfile[i].d2dLayoutRect =
                    MakeLayoutRectByCorner(PROFILE_UI_OFFSET_X, PROFILE_UI_OFFSET_Y + PROFILE_UI_OFFSET_Y * (i - 1),
                        PROFILE_UI_WIDTH, PROFILE_UI_HEIGHT);
            }
            m_CharStatus[0].d2dLayoutRect =
                MakeLayoutRectByCorner(PROFILE_UI_OFFSET_X + PROFILE_UI_WIDTH, PROFILE_UI_OFFSET_Y, STATUS_UI_WIDTH, STATUS_UI_HEIGHT);
            m_CharStatus[1].d2dLayoutRect =
                MakeLayoutRectByCorner(PROFILE_UI_OFFSET_X + PROFILE_UI_WIDTH, PROFILE_UI_OFFSET_Y * 2, STATUS_UI_WIDTH, STATUS_UI_HEIGHT);
            m_CharStatus[2].d2dLayoutRect =
                MakeLayoutRectByCorner(PROFILE_UI_OFFSET_X + PROFILE_UI_WIDTH, PROFILE_UI_OFFSET_Y * 3, STATUS_UI_WIDTH, STATUS_UI_HEIGHT);
            break;
        }
        // �������� �ڽ��� ������ 2�� ���
        case 1:
        {
            
            m_CharProfile[2].d2dLayoutRect =
                MakeLayoutRectByCorner(PROFILE_UI_OFFSET_X, PROFILE_UI_OFFSET_Y, PROFILE_UI_WIDTH, PROFILE_UI_HEIGHT);
            
            m_CharProfile[3].d2dLayoutRect =
                MakeLayoutRectByCorner(PROFILE_UI_OFFSET_X, PROFILE_UI_OFFSET_Y * 2, PROFILE_UI_WIDTH, PROFILE_UI_HEIGHT);
            
            // 1 2 3 --> 0 1 2
            m_CharStatus[0].m_hide = true;
            m_CharStatus[1].d2dLayoutRect =
                MakeLayoutRectByCorner(PROFILE_UI_OFFSET_X + PROFILE_UI_WIDTH, PROFILE_UI_OFFSET_Y, STATUS_UI_WIDTH, STATUS_UI_HEIGHT);
            m_CharStatus[2].d2dLayoutRect =
                MakeLayoutRectByCorner(PROFILE_UI_OFFSET_X + PROFILE_UI_WIDTH, PROFILE_UI_OFFSET_Y * 2, STATUS_UI_WIDTH, STATUS_UI_HEIGHT);
        }
        break;
        case 2:
        {
   
            m_CharProfile[1].d2dLayoutRect = 
                MakeLayoutRectByCorner(PROFILE_UI_OFFSET_X, PROFILE_UI_OFFSET_Y, PROFILE_UI_WIDTH, PROFILE_UI_HEIGHT);
            m_CharProfile[3].d2dLayoutRect =
                MakeLayoutRectByCorner(PROFILE_UI_OFFSET_X, PROFILE_UI_OFFSET_Y * 2, PROFILE_UI_WIDTH, PROFILE_UI_HEIGHT);
            
            m_CharStatus[1].m_hide = true;
            m_CharStatus[0].d2dLayoutRect =
                MakeLayoutRectByCorner(PROFILE_UI_OFFSET_X + PROFILE_UI_WIDTH, PROFILE_UI_OFFSET_Y, STATUS_UI_WIDTH, STATUS_UI_HEIGHT);
            m_CharStatus[2].d2dLayoutRect =
                MakeLayoutRectByCorner(PROFILE_UI_OFFSET_X + PROFILE_UI_WIDTH, PROFILE_UI_OFFSET_Y * 2, STATUS_UI_WIDTH, STATUS_UI_HEIGHT);
        }
        break;
        
        case 3:
        {
            m_CharProfile[1].d2dLayoutRect = 
                MakeLayoutRectByCorner(PROFILE_UI_OFFSET_X, PROFILE_UI_OFFSET_Y, PROFILE_UI_WIDTH, PROFILE_UI_HEIGHT);
            m_CharProfile[2].d2dLayoutRect = 
                MakeLayoutRectByCorner(PROFILE_UI_OFFSET_X, PROFILE_UI_OFFSET_Y * 2, PROFILE_UI_WIDTH, PROFILE_UI_HEIGHT);

            m_CharStatus[2].m_hide = true;
            
            m_CharStatus[0].d2dLayoutRect =
                MakeLayoutRectByCorner(PROFILE_UI_OFFSET_X + PROFILE_UI_WIDTH, PROFILE_UI_OFFSET_Y, STATUS_UI_WIDTH, STATUS_UI_HEIGHT);
            m_CharStatus[1].d2dLayoutRect =
                MakeLayoutRectByCorner(PROFILE_UI_OFFSET_X + PROFILE_UI_WIDTH, PROFILE_UI_OFFSET_Y * 2, STATUS_UI_WIDTH, STATUS_UI_HEIGHT);
        }
        break;
    }

    for (int i = 0; i < 3; ++i)
    {
        m_CharStatus[i].resource =  m_CharStatusBitmaps[0]; // normal status�� ����
    }
    
    m_myProfileLayout = MakeLayoutRectByCorner(PROFILE_UI_OFFSET_X,BIG_PROFILE_UI_OFFSET_Y, BIG_PROFILE_UI_WIDTH, BIG_PROFILE_UI_HEIGHT);

    // �÷��̾� hp ���

}

void UIManager::UpdateGameSceneUI(CGameScene* gc)
{
  // ������ �ش��ϴ� ���� UI�� hp, status
    for (int i = 0; i < PLAYERNUM; ++i)
    {
        if (!gc->GetScenePlayerByIdx(i)) continue;
        if (i == 0) continue;

        if (i != m_playerIdx) // ���� ���� 
        {
            if (MAX_HP == gc->GetScenePlayerByIdx(i)->m_hp)
            {
                m_CharStatus[gc->GetScenePlayerByIdx(i)->m_idx - 1].resource = m_CharStatusBitmaps[0]; // normal
            }
            else if (0 == gc->GetScenePlayerByIdx(i)->m_hp)
            {
                m_CharStatus[gc->GetScenePlayerByIdx(i)->m_idx - 1].resource = m_CharStatusBitmaps[2]; // death
            }
            else m_CharStatus[gc->GetScenePlayerByIdx(i)->m_idx - 1].resource = m_CharStatusBitmaps[1]; // death
        }
        else if (i == m_playerIdx) // HP , �ǰ� ����Ʈ
        {
            CEmployee* mp = static_cast<CEmployee*>(gc->GetScenePlayerByIdx(i));
            if (mp->m_bIsInvincibility)
            {
                m_AttackedEffect.m_hide = false;
                if (mp->m_UICoolTime >= 0.0f) m_AttackedOpacity = mp->m_UICoolTime;
            }
            else m_AttackedEffect.m_hide = true;

            for (auto& i : m_HPUi) i.m_hide = true;
            for (int k = 0; k < gc->GetScenePlayerByIdx(i)->m_hp; ++k)
            {
                m_HPUi[k].m_hide = false;
            }
        }
    }

    // ������
    
    if (gc->GetScenePlayerByIdx(m_playerIdx))
    {
        if (m_playerIdx != 0)
        {
            CEmployee* myPlayer = static_cast<CEmployee*>(gc->GetScenePlayerByIdx(m_playerIdx));
           
            if (myPlayer->GetIsInGenArea())
            {
                m_GenerateUIButtons[20].m_hide = false;
            }
            else if (myPlayer->GetIsInGenArea() == false) m_GenerateUIButtons[20].m_hide = true;

            if (myPlayer->GetIsPlayerOnGenInter())
            {
                
                CGenerator* targetGen = myPlayer->GetAvailGen();
                if(targetGen && (int32)((((int32)targetGen->m_curGuage) % 100) / 5) <= 19) 
                    m_GenerateUIButtons[(int32)((((int32)targetGen->m_curGuage ) % 100) / 5)].m_hide = false;
            }
            else
            {
                for (int i = 0; i < 20; ++i)  m_GenerateUIButtons[i].m_hide = true;
            }
        }
    }
}

void UIManager::DrawGameSceneUI(int32 Scene)
{
    if (3 != Scene) return;
    // ���� ������
    // �ٸ� ĳ���� �ʻ�ȭ , �� ĳ���� �ʻ�ȭ
    for (auto i : m_CharProfile)
    {
        if(!i.m_hide) m_pd2dDeviceContext->DrawBitmap(i.resource, i.d2dLayoutRect);
    }

    for (auto i : m_GenerateUIButtons)
        if(!i.m_hide) m_pd2dDeviceContext->DrawBitmap(i.resource, i.d2dLayoutRect);

    for(auto i : m_CharStatus)
        if(!i.m_hide) m_pd2dDeviceContext->DrawBitmap(i.resource, i.d2dLayoutRect);
    
    // ū �ʻ�ȭ �׸���
    m_pd2dDeviceContext->DrawBitmap(m_CharProfile[m_playerIdx].resource, m_myProfileLayout);
    // HP �׸���
    if (m_playerIdx != 0)
    {
        for(auto i : m_HPUi)  if (!i.m_hide) m_pd2dDeviceContext->DrawBitmap(i.resource, i.d2dLayoutRect);
        if (!m_AttackedEffect.m_hide) m_pd2dDeviceContext->DrawBitmap(m_AttackedEffect.resource, m_AttackedEffect.d2dLayoutRect, m_AttackedOpacity);
    }

    if (m_playerIdx == 0) m_pd2dDeviceContext->DrawBitmap(m_CharCrossHead.resource, m_CharCrossHead.d2dLayoutRect);
  
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
    m_pd2dWriteFactory->CreateTextFormat(pszFontName, nullptr, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, fFontSize, L"en-us", &pdwDefaultTextFormat);

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
    m_TitleTextFormat = CreateTextFormat(L"Headliner No. 45", 40);
    m_LobbyTextFormat = CreateTextFormat(L"���� ���", 40);
    // ��� ���ҽ���
    m_backGround[0].resource = LoadPngFromFile(L"UI/Title.png");
    m_backGround[1].resource = LoadPngFromFile(L"UI/Lobby.png");
    m_backGround[2].resource = LoadPngFromFile(L"UI/Game.png");

    // Ÿ��Ʋ ���� �ʿ��� ��ư
    m_TitleButtons[0].resource      = LoadPngFromFile(L"UI/Title_Start.png");  
    m_TitleButtons[1].resource      = LoadPngFromFile(L"UI/Title_Quit.png");
    m_TitleButtons[0].d2dLayoutRect = MakeLayoutRect(CENTER_X + TITLEBUTTON_X_OFFSET, CENTER_Y + TITLEBUTTON_Y_OFFSET,     200, 50);
    m_TitleButtons[1].d2dLayoutRect = MakeLayoutRect(CENTER_X + TITLEBUTTON_X_OFFSET, CENTER_Y + TITLEBUTTON_Y_OFFSET * 2, 200, 50);
    // ID / PW �Է� â
    m_IDPWTextBlocks[0].m_pd2dTextBrush = CreateBrush(D2D1::ColorF::White);
    m_IDPWTextBlocks[0].m_pdwFormat = m_TitleTextFormat;
    m_IDPWTextBlocks[0].m_pstrText = L"ID:";
    

    m_IDPWTextBlocks[1].m_pd2dTextBrush = CreateBrush(D2D1::ColorF::White);
    m_IDPWTextBlocks[1].m_pdwFormat = m_TitleTextFormat;
    m_IDPWTextBlocks[1].m_pstrText = L"PW:";

    m_IDPWTextBlocks[0].m_d2dLayoutRect = MakeLayoutRect(CENTER_X, CENTER_Y + 150, 200, FontSize);
    m_IDPWTextBlocks[1].m_d2dLayoutRect = MakeLayoutRect(CENTER_X, CENTER_Y + 200, 200, FontSize);
    

    // �κ� ���� �ʿ��� ��ư
    m_LobbyButtons[0].resource = LoadPngFromFile(L"UI/Enter_Room.png");
    m_LobbyButtons[1].resource = LoadPngFromFile(L"UI/Create_Room.png");
    m_LobbyButtons[2].resource = LoadPngFromFile(L"UI/Quit_Lobby.png");

    m_LobbyButtons[0].d2dLayoutRect = MakeLayoutRectByCorner(LOBBYBUTTON_X_OFFSET,        LOBBYBUTTON_Y_OFFSET, FRAME_BUFFER_WIDTH / 3.0f, FRAME_BUFFER_HEIGHT / 4.0);
    m_LobbyButtons[1].d2dLayoutRect = MakeLayoutRectByCorner(0,                           LOBBYBUTTON_Y_OFFSET, FRAME_BUFFER_WIDTH / 3.0f, FRAME_BUFFER_HEIGHT / 4.0);
    m_LobbyButtons[2].d2dLayoutRect = MakeLayoutRectByCorner(LOBBYBUTTON_X_OFFSET * 2.0f, LOBBYBUTTON_Y_OFFSET, FRAME_BUFFER_WIDTH / 3.0f, FRAME_BUFFER_HEIGHT / 4.0);
    

    m_RoomButtons[0].resource = LoadPngFromFile(L"UI/Ready_Game.png");
    m_RoomButtons[1].resource = LoadPngFromFile(L"UI/Quit_Game.png");
    m_RoomButtons[0].d2dLayoutRect = MakeLayoutRectByCorner(0,                        LOBBYBUTTON_Y_OFFSET, FRAME_BUFFER_WIDTH / 3.0f, FRAME_BUFFER_HEIGHT / 4.0);
    m_RoomButtons[1].d2dLayoutRect = MakeLayoutRectByCorner(LOBBYBUTTON_X_OFFSET * 2.0f, LOBBYBUTTON_Y_OFFSET, FRAME_BUFFER_WIDTH / 3.0f, FRAME_BUFFER_HEIGHT / 4.0);

    m_ReadyBitmaps[0].resource = LoadPngFromFile(L"UI/Ready.png");
    m_ReadyBitmaps[1].resource = LoadPngFromFile(L"UI/Ready2.png");
    m_ReadyBitmaps[2].resource = LoadPngFromFile(L"UI/Ready3.png");
    m_ReadyBitmaps[3].resource = LoadPngFromFile(L"UI/Ready4.png");

    m_ReadyBitmaps[0].d2dLayoutRect = MakeLayoutRectByCorner(LOBBYROOMLIST_X_OFFSET, 
        LOBBYROOMLIST_Y_OFFSET,
        (FRAME_BUFFER_WIDTH - (LOBBYROOMLIST_X_OFFSET * 2.0)) / 2.0,
        FRAME_BUFFER_HEIGHT / 4.0);
    m_ReadyBitmaps[1].d2dLayoutRect = MakeLayoutRectByCorner( LOBBYROOMLIST_X_OFFSET + ((FRAME_BUFFER_WIDTH - (LOBBYROOMLIST_X_OFFSET * 2.0)) / 2.0),
        LOBBYROOMLIST_Y_OFFSET,
        (FRAME_BUFFER_WIDTH - (LOBBYROOMLIST_X_OFFSET * 2.0)) / 2.0,
        FRAME_BUFFER_HEIGHT / 4.0);
    
    m_ReadyBitmaps[2].d2dLayoutRect = MakeLayoutRectByCorner(LOBBYROOMLIST_X_OFFSET,
        LOBBYROOMLIST_Y_OFFSET + FRAME_BUFFER_HEIGHT / 4.0,
        (FRAME_BUFFER_WIDTH - (LOBBYROOMLIST_X_OFFSET * 2.0)) / 2.0,
        FRAME_BUFFER_HEIGHT / 4.0);

    m_ReadyBitmaps[3].d2dLayoutRect = MakeLayoutRectByCorner(
        LOBBYROOMLIST_X_OFFSET + (FRAME_BUFFER_WIDTH - (LOBBYROOMLIST_X_OFFSET * 2.0)) / 2.0,
        LOBBYROOMLIST_Y_OFFSET + FRAME_BUFFER_HEIGHT / 4.0,
        (FRAME_BUFFER_WIDTH - (LOBBYROOMLIST_X_OFFSET * 2.0)) / 2.0,
        FRAME_BUFFER_HEIGHT / 4.0);

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

    // �ΰ��� ��Ʈ�� �ε�
    // ĳ���� ������
    m_CharProfile[0].resource = LoadPngFromFile(L"UI/Char_UI_1.png"); // Boss
    m_CharProfile[1].resource = LoadPngFromFile(L"UI/Char_UI_2.png"); // Yellow
    m_CharProfile[2].resource = LoadPngFromFile(L"UI/Char_UI_3.png"); // Mask 
    m_CharProfile[3].resource = LoadPngFromFile(L"UI/Char_UI_5.png"); // Goggle
    for (int i = 0; i < PLAYERNUM; ++i)
        m_CharProfile[i].d2dLayoutRect = MakeLayoutRectByCorner(FRAME_BUFFER_WIDTH * 0.01, FRAME_BUFFER_HEIGHT * 0.1 * i, FRAME_BUFFER_WIDTH * 0.1, FRAME_BUFFER_HEIGHT * 0.1);

    // ���� --> �������� ���ϴ� ���̹Ƿ� �׶� �׶� ��ġ�� ������Ʈ�ϱ�� �Ѵ�. �ϴ� ��Ʈ�� ���ҽ��� �ε��Ѵ�.
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
    // ũ�ν� ���
    m_CharCrossHead.resource = LoadPngFromFile(L"UI/crossHair.png");
    m_CharCrossHead.d2dLayoutRect = MakeLayoutRect(CENTER_X, CENTER_Y,10.f,10.f);
    m_CharCrossHead.m_hide = false;

    // �ǰ� ����Ʈ
    m_AttackedEffect.resource = LoadPngFromFile(L"UI/Attacked.png");
    m_AttackedEffect.d2dLayoutRect = MakeLayoutRectByCorner(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
    m_AttackedEffect.m_hide = false;

    // ������ ������
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
}

void UIManager::Render2D(UINT nFrame, int32 curScene)
{

    ID3D11Resource* ppResources[] = { m_ppd3d11WrappedRenderTargets[nFrame] };

    m_pd2dDeviceContext->SetTarget(m_ppd2dRenderTargets[nFrame]);
    m_pd3d11On12Device->AcquireWrappedResources(ppResources, _countof(ppResources));

    m_pd2dDeviceContext->BeginDraw();
    DrawOtherSceneBackGround(curScene);
    DrawOtherSceneUI(curScene,0);
    DrawOtherSceneUI(curScene,1);
    DrawOtherSceneUITextBlock(curScene);
    DrawGameSceneUI(curScene);
    m_pd2dDeviceContext->EndDraw();
    m_pd3d11On12Device->ReleaseWrappedResources(ppResources, _countof(ppResources));
    m_pd3d11DeviceContext->Flush();
}