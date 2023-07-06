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
struct UIButton
{
    D2D1_RECT_F                     d2dLayoutRect; // 출력할 레이아웃 영역
    ID2D1Bitmap*                    resource = NULL;
    int32 type; // 버튼 타입
};
struct UIBackGround
{
    D2D1_RECT_F                     d2dLayoutRect; // 출력할 레이아웃 영역
    ID2D1Bitmap*                    resource = NULL;
};

class UIManager
{
    enum class ButtonType { MAKE_ROOM, ENTER_ROOM, EXIT_GAME, };

public:
    UIManager(UINT nFrames, UINT nTextBlocks, ID3D12Device* pd3dDevice, ID3D12CommandQueue* pd3dCommandQueue, ID3D12Resource** ppd3dRenderTargets, UINT nWidth, UINT nHeight);

    void CreateD2DDevice();
    ID2D1Bitmap1* LoadPngFromFile(const wchar_t* filePath);
    void CreateD3D11On12Device(ID3D12Device* pd3dDevice, ID3D12CommandQueue* pd3dCommandQueue);
    void CreateRenderTarget(ID3D12Resource** ppd3dRenderTargets);

    void UpdateTextOutputs(UINT nIndex, WCHAR* pstrUIText, D2D1_RECT_F* pd2dLayoutRect, IDWriteTextFormat* pdwFormat, ID2D1SolidColorBrush* pd2dTextBrush);
    void Render2D(UINT nFrame,int32 curScene);
    void ReleaseResources();
    void DrawBackGround(int32 Scene);
    void DrawButton(int32 idx);

    ID2D1SolidColorBrush* CreateBrush(D2D1::ColorF d2dColor);
    IDWriteTextFormat* CreateTextFormat(WCHAR* pszFontName, float fFontSize);
public:
    void InitializeDevice(ID3D12Device* pd3dDevice, ID3D12CommandQueue* pd3dCommandQueue, ID3D12Resource** ppd3dRenderTargets);

    // WindowInfo
    float                           m_fWidth = 0.0f;
    float                           m_fHeight = 0.0f;

    // Direct3D11on12
    ComPtr<ID3D11DeviceContext> m_pd3d11DeviceContext = NULL;
    ComPtr<ID3D11On12Device>    m_pd3d11On12Device = NULL;


    //DirectX2d Write
    ComPtr<IDWriteFactory>      m_pd2dWriteFactory = NULL;

    // RenderTarget
    ID3D11Resource**            m_ppd3d11WrappedRenderTargets = NULL;
    UINT                        m_nRenderTargets = 0;

    //DirectX2D
    ID2D1Bitmap1**       m_ppd2dRenderTargets = NULL;
    ID2D1Factory3*       m_pd2dFactory = NULL;
    ID2D1Device2*        m_pd2dDevice = NULL;
    ID2D1DeviceContext2* m_pd2dDeviceContext = NULL;

    // TextBlock
    UINT                           m_nTextBlocks = 0;
   
    // BitmapResource
    UINT                           m_nBitmaps = 20;
    ID2D1Bitmap*                   m_bitmaps[20];
    

    // 배경 레이어 비트맵들
    UIBackGround m_backGround[3];

    // 버튼 비트맵들
    UINT m_nButtons = 10;
    UIButton m_buttons[10];

    // 동적으로 바뀌는 텍스트 버튼들
    UITextBlock* m_pTextBlocks = NULL;
};

