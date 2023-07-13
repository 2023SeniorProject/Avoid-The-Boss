#pragma once

// UI�Ŵ��� ����
// ���� �����ӿ�ũ ���� ��, �� ȭ�鿡 �ʿ��� ��� UI �̹����� �ε��Ѵ�. WCI �����͸� Ȱ��
// D2D�� Ȱ���ؼ� �̹����� �׸���.

struct UITextBlock
{
    std::wstring                    m_pstrText; // ����� �ؽ�ó
    D2D1_RECT_F                     m_d2dLayoutRect; // ����� ���̾ƿ� ����
    IDWriteTextFormat*              m_pdwFormat; // �Է� ����
    ID2D1SolidColorBrush*           m_pd2dTextBrush; // �ؽ�ó�� ����� �귯��
};
struct UIButton
{
    D2D1_RECT_F                     d2dLayoutRect; // ����� ���̾ƿ� ����
    ID2D1Bitmap*                    resource = NULL;
    int32 type; // ��ư Ÿ��
};
struct UIBackGround
{
    D2D1_RECT_F                     d2dLayoutRect; // ����� ���̾ƿ� ����
    ID2D1Bitmap*                    resource = NULL;
};

class UIManager
{
    enum class ButtonType { MAKE_ROOM, ENTER_ROOM, EXIT_GAME, };

public:
    UIManager(UINT nFrames, UINT nTextBlocks, ID3D12Device5* pd3dDevice, ID3D12CommandQueue* pd3dCommandQueue, ID3D12Resource** ppd3dRenderTargets, UINT nWidth, UINT nHeight);

    void CreateD2DDevice();
    ID2D1Bitmap1* LoadPngFromFile(const wchar_t* filePath);
    void CreateD3D11On12Device(ID3D12Device5* pd3dDevice, ID3D12CommandQueue* pd3dCommandQueue);
    void CreateRenderTarget(ID3D12Resource** ppd3dRenderTargets);

    void UpdateTextOutputs(UINT nIndex, WCHAR* pstrUIText, D2D1_RECT_F* pd2dLayoutRect, IDWriteTextFormat* pdwFormat, ID2D1SolidColorBrush* pd2dTextBrush);
    void Render2D(UINT nFrame,int32 curScene);
    void ReleaseResources();
    void DrawBackGround(int32 Scene);
    void DrawButton(int32, int32 idx);
    void DrawTextBlock(int32);

    D2D1_RECT_F GetButtonRect(int32, int32);

    ID2D1SolidColorBrush* CreateBrush(D2D1::ColorF d2dColor);
    IDWriteTextFormat* CreateTextFormat(const WCHAR* pszFontName, float fFontSize);
public:
    void InitializeDevice(ID3D12Device5* pd3dDevice, ID3D12CommandQueue* pd3dCommandQueue, ID3D12Resource** ppd3dRenderTargets);

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
    ID2D1Bitmap1**              m_ppd2dRenderTargets = NULL;
    ID2D1Factory3*              m_pd2dFactory = NULL;
    ID2D1Device2*               m_pd2dDevice = NULL;
    ID2D1DeviceContext2*        m_pd2dDeviceContext = NULL;

    // TextBlock
    UINT                           m_nTextBlocks = 0;
   
    // BitmapResource
    UINT                           m_nBitmaps = 20;
    ID2D1Bitmap*                   m_bitmaps[20];
    
    // ��� ���̾� ��Ʈ�ʵ�
    UIBackGround m_backGround[3];

    // ��ư ��Ʈ�ʵ�
    UINT m_nButtons = 10;
    UIButton m_TitleButtons[2];
    
    UIButton m_LobbyButtons[3];
    D2D1_RECT_F m_RoomListLayout;

    UIButton m_RoomButtons[3];

    // �������� �ٲ�� �ؽ�Ʈ ��ư�� Id,PW 
    UITextBlock* m_pTextBlocks;

    // ���̾� ��ġ ����� ���� �귯��
    ID2D1SolidColorBrush* redBrush; // ����
    ID2D1SolidColorBrush* grayBrush; // ȸ��
    ID2D1SolidColorBrush* blackBrush; // ȸ��
};

