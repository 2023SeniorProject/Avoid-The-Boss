#pragma once

// UI매니저 개요
// 게임 프레임워크 시작 시, 각 화면에 필요한 모든 UI 이미지를 로드한다. WCI 컨버터를 활용
// D2D를 활용해서 이미지를 그린다.

struct UITextBlock
{
    WCHAR                           m_pstrText[256]; // 출력할 텍스처
    D2D1_RECT_F                     m_d2dLayoutRect; // 출력할 레이아웃 영역
    IDWriteTextFormat*              m_pdwFormat; // 입력 포맷
    ID2D1SolidColorBrush*           m_pd2dTextBrush; // 텍스처를 출력할 브러쉬
};

class UIManager
{
public:
        UIManager(UINT nFrames, UINT nTextBlocks, ID3D12Device* pd3dDevice, ID3D12CommandQueue* pd3dCommandQueue, ID3D12Resource** ppd3dRenderTargets, UINT nWidth, UINT nHeight);

        void CreateD2DDevice();
        void CreateD3D11On12Device(ID3D12Device* pd3dDevice, ID3D12CommandQueue* pd3dCommandQueue);
        void CreateRenderTarget(ID3D12Resource** ppd3dRenderTargets);
    
        void UpdateTextOutputs(UINT nIndex, WCHAR* pstrUIText, D2D1_RECT_F* pd2dLayoutRect, IDWriteTextFormat* pdwFormat, ID2D1SolidColorBrush* pd2dTextBrush);
        void Render2D(UINT nFrame);
        void ReleaseResources();

        ID2D1SolidColorBrush* CreateBrush(D2D1::ColorF d2dColor);
        IDWriteTextFormat* CreateTextFormat(WCHAR* pszFontName, float fFontSize);
public:
        void InitializeDevice(ID3D12Device* pd3dDevice, ID3D12CommandQueue* pd3dCommandQueue, ID3D12Resource** ppd3dRenderTargets);

        float                           m_fWidth = 0.0f;
        float                           m_fHeight = 0.0f;

        // Direct3D11on12
        ComPtr<ID3D11DeviceContext> m_pd3d11DeviceContext = NULL;
        ComPtr<ID3D11On12Device>    m_pd3d11On12Device = NULL;
        ID3D11Resource**     m_ppd3d11WrappedRenderTargets = NULL;

        //DirectX2d Write
        ComPtr<IDWriteFactory>      m_pd2dWriteFactory = NULL;
      

        UINT                            m_nRenderTargets = 0;
        
        
        //DirectX2D
        ID2D1Bitmap1**                  m_ppd2dRenderTargets = NULL;
        ID2D1Factory3* m_pd2dFactory = NULL;
        ID2D1Device2* m_pd2dDevice = NULL;
        ID2D1DeviceContext2* m_pd2dDeviceContext = NULL;

        // TextBlock
        UINT                            m_nTextBlocks = 0;
        UITextBlock*                    m_pTextBlocks = NULL;
 
};

