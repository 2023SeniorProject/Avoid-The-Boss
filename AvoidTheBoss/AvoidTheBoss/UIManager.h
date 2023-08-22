#pragma once

// UI매니저 개요
// 게임 프레임워크 시작 시, 각 화면에 필요한 모든 UI 이미지를 로드한다. WCI 컨버터를 활용
// D2D를 활용해서 이미지를 그린다.
// LobbyButton

const int32 MAX_HP = 3;

struct UITextBlock
{
    std::wstring                    m_pstrText; // 출력할 텍스처
    D2D1_RECT_F                     m_d2dLayoutRect; // 출력할 레이아웃 영역
    IDWriteTextFormat*              m_pdwFormat = NULL; // 입력 포맷
    ID2D1SolidColorBrush*           m_pd2dTextBrush= NULL; // 텍스처를 출력할 브러쉬
    bool                            m_hide = false;
};
struct UIButton
{
    D2D1_RECT_F                     d2dLayoutRect; // 출력할 레이아웃 영역
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
    D2D1_RECT_F                     d2dLayoutRect; // 출력할 레이아웃 영역
    ID2D1Bitmap*                    resource = NULL;
};

struct GuageUI
{
    bool                            m_hide;
    D2D1_RECT_F                     d2dLayoutRect[2]; // 테두리랑 다 채운 용도 하나
};

struct InGameUI
{
    D2D1_RECT_F                     d2dLayoutRect; // 출력할 레이아웃 영역
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
    
    
    
    // 배경 레이어 비트맵들
    UIBackGround m_backGround[5];

    // 타이틀 전용
    ANIMButton m_LoginResult[3];

    // 버튼 비트맵들
    int m_nRoomListPerPage = 5;
    UIButton m_TitleButtons[3];
    
    // 로비 전용
    UIButton m_LobbyButtons[3];
    UITextBlock m_RoomListTextBlock[5];
    
    int32 m_lastRoomPage = 0;
    int32 m_selectedLayout = -1;
    D2D1_RECT_F m_RoomListLayout[5];

    // 방 전용
    UIButton m_RoomButtons[2];

    InGameUI m_CharCrossHead;
    InGameUI m_RescueIcon;
    InGameUI m_AttackedEffect;
   
    float m_AttackedOpacity = 1.0;

    // 인 게임 전용
    int32                      m_playerIdx = -1;
    UIButton                   m_GenerateUIButtons[23];
    UIButton                   m_CharProfile[4]; // 다른 캐릭터 초상화 표시
    
    ID2D1Bitmap*               m_CharStatusBitmaps[3]; // 비트맵 리소스를 가져와서 공유한다.
    ID2D1Bitmap*               m_HpBitmap;

    D2D_RECT_F                 m_myProfileLayout; // 자기 캐릭터 레이아웃

    InGameUI                   m_CharStatus[3]; // 캐릭터 상태
    InGameUI                   m_HPUi[MAX_HP];    // 캐릭터 HP
    GuageUI                    m_RescueGuage;

    // 동적으로 바뀌는 텍스트 버튼들 Id,PW 
    UITextBlock                m_IDPWTextBlocks[2];
    // 레디 버튼
    UIButton                   m_ReadyBitmaps[4];
    UIButton                   m_ReadyCard[4];

   
    // 결과창 
    UITextBlock m_ResultTextBlock[2];
    
    // 레이어 위치 출력을 위한 브러시
    ID2D1SolidColorBrush* redBrush; // 빨강
    ID2D1SolidColorBrush* grayBrush; // 회색
    ID2D1SolidColorBrush* blackBrush; // 회색
    ID2D1SolidColorBrush* whiteBrush;
    ID2D1SolidColorBrush* greenBrush;
};

