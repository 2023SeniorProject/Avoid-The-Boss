#pragma once

// UI�Ŵ��� ����
// ���� �����ӿ�ũ ���� ��, �� ȭ�鿡 �ʿ��� ��� UI �̹����� �ε��Ѵ�. WCI �����͸� Ȱ��
// D2D�� Ȱ���ؼ� �̹����� �׸���.
// LobbyButton

const int32 MAX_HP = 3;

struct UITextBlock
{
    std::wstring                    m_pstrText; // ����� �ؽ�ó
    D2D1_RECT_F                     m_d2dLayoutRect; // ����� ���̾ƿ� ����
    IDWriteTextFormat*              m_pdwFormat = NULL; // �Է� ����
    ID2D1SolidColorBrush*           m_pd2dTextBrush= NULL; // �ؽ�ó�� ����� �귯��
    bool                            m_hide = false;
};
struct UIButton
{
    D2D1_RECT_F                     d2dLayoutRect; // ����� ���̾ƿ� ����
    ID2D1Bitmap*                    resource = NULL;
    bool                            m_hide = false;
};

struct ANIMButton
{
    D2D1_RECT_F d2dLayoutRect;
    ID2D1Bitmap* resource = NULL;
    bool m_hide = false;
    float animTime = 1.0;
};
struct UIBackGround
{
    D2D1_RECT_F                     d2dLayoutRect; // ����� ���̾ƿ� ����
    ID2D1Bitmap*                    resource = NULL;
};

struct GuageUI
{
    bool                            m_hide;
    D2D1_RECT_F                     d2dLayoutRect[2]; // �׵θ��� �� ä�� �뵵 �ϳ�
};

struct InGameUI
{
    D2D1_RECT_F                     d2dLayoutRect; // ����� ���̾ƿ� ����
    ID2D1Bitmap*                    resource = NULL;
    bool                            m_hide = false;
};

class CGameScene;
class UIManager
{
    enum class ButtonType { MAKE_ROOM, ENTER_ROOM, EXIT_GAME, };

public:
    UIManager(UINT nFrames, ID3D12Device5* pd3dDevice, ID3D12CommandQueue* pd3dCommandQueue, ID3D12Resource** ppd3dRenderTargets, UINT nWidth, UINT nHeight);

    void CreateD2DDevice();
    ID2D1Bitmap1* LoadPngFromFile(const wchar_t* filePath);
    void CreateD3D11On12Device(ID3D12Device5* pd3dDevice, ID3D12CommandQueue* pd3dCommandQueue);
    void CreateRenderTarget(ID3D12Resource** ppd3dRenderTargets);

    void UpdateRoomTextBlocks(UINT nIndex,const WCHAR* pstrUIText, const D2D1_RECT_F& pd2dLayoutRect, bool hide);
    void UpdateRoomText();

   

    void Render2D(UINT nFrame,int32 curScene);
    void ReleaseResources();

    void DrawOtherSceneBackGround(int32 Scene);
    void DrawOtherSceneUI(int32 Scene, int32 idx);
    void DrawOtherSceneUITextBlock(int32 Scene);
    void InitGameSceneUI(CGameScene*);
    void UpdateGameSceneUI(CGameScene*);
    void DrawGameSceneUI(int32 Scene);

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
    IDWriteTextFormat*             m_TitleTextFormat;
    IDWriteTextFormat*             m_LobbyTextFormat;
    
    
    
    // ��� ���̾� ��Ʈ�ʵ�
    UIBackGround m_backGround[5];

    // Ÿ��Ʋ ����
    ANIMButton m_LoginResult[3];

    // ��ư ��Ʈ�ʵ�
    int m_nRoomListPerPage = 5;
    UIButton m_TitleButtons[3];
    
    // �κ� ����
    UIButton m_LobbyButtons[3];
    UITextBlock m_RoomListTextBlock[5];
    
    int32 m_lastRoomPage = 0;
    int32 m_selectedLayout = -1;
    D2D1_RECT_F m_RoomListLayout[5];

    // �� ����
    UIButton m_RoomButtons[2];

    InGameUI m_CharCrossHead;
    InGameUI m_RescueIcon;
    InGameUI m_AttackedEffect;
   
    float m_AttackedOpacity = 1.0;

    // �� ���� ����
    int32                      m_playerIdx = -1;
    UIButton                   m_GenerateUIButtons[23];
    UIButton                   m_CharProfile[4]; // �ٸ� ĳ���� �ʻ�ȭ ǥ��
    
    ID2D1Bitmap*               m_CharStatusBitmaps[3]; // ��Ʈ�� ���ҽ��� �����ͼ� �����Ѵ�.
    ID2D1Bitmap*               m_HpBitmap;

    D2D_RECT_F                 m_myProfileLayout; // �ڱ� ĳ���� ���̾ƿ�

    InGameUI                   m_CharStatus[3]; // ĳ���� ����
    InGameUI                   m_HPUi[MAX_HP];    // ĳ���� HP
    GuageUI                    m_RescueGuage;

    // �������� �ٲ�� �ؽ�Ʈ ��ư�� Id,PW 
    UITextBlock                m_IDPWTextBlocks[2];
    // ���� ��ư
    UIButton                   m_ReadyBitmaps[4];
    UIButton                   m_ReadyCard[4];

   
    // ���â 
    UITextBlock m_ResultTextBlock[2];
    
    // ���̾� ��ġ ����� ���� �귯��
    ID2D1SolidColorBrush* redBrush; // ����
    ID2D1SolidColorBrush* grayBrush; // ȸ��
    ID2D1SolidColorBrush* blackBrush; // ȸ��
    ID2D1SolidColorBrush* whiteBrush;
    ID2D1SolidColorBrush* greenBrush;
};

