// Senior_project_ver2.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "pch.h"
#include "AvoidTheBoss.h"
#include "clientIocpCore.h"
#include "SocketUtil.h"
#include "ThreadManager.h"



#define MAX_LOADSTRING 100

// 전역 변수:

HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

BOOL CALLBACK MyDialogBox(HWND hWndDlg, UINT message, WPARAM wParam, LPARAM lParam);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    ::SetConsoleTitle(L"Client");
   
    MSG msg;
    
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
   //SocketUtil::Init();
   GCThreadManager = new ThreadManager;
    int retval = DialogBox(hInstance, MAKEINTRESOURCE(IDD_LOGINDIALOG), NULL, reinterpret_cast<DLGPROC>(MyDialogBox));
    if (retval == -1 || retval == 2)
    {
        SocketUtil::Close(clientIocpCore._client->_sock);
        SocketUtil::Clear();
        return 0;
    }
    // 전역 문자열을 초기화합니다.
    ::LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    ::LoadString(hInstance, IDC_AVOIDTHEBOSS, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_AVOIDTHEBOSS));

    // 기본 메시지 루프입니다:
    GCThreadManager->Launch([=]()
        {
            while (true)
            {
                if (!clientIocpCore.Processing()) break;
                std::this_thread::sleep_for(0ms);
            }
        }
    );
    while (true)
    {
        if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
            {
                SocketUtil::Close(clientIocpCore._client->_sock);
                break;
            }
            if (!::TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
            {
                ::TranslateMessage(&msg);
                ::DispatchMessage(&msg);
            }
        }
        else
        {
          
           clientIocpCore.GameLoop(); // 처리할 윈도우 메세지가 큐에 없을 때 게임프로그램이 CPU사용
        }
        std::this_thread::sleep_for(0ms);
    }
 
    GCThreadManager->Join();
   // SocketUtil::Clear();
    
    delete GCThreadManager;
 
    return (int)msg.wParam;
}


ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = ::LoadIcon(hInstance, MAKEINTRESOURCE(IDI_AVOIDTHEBOSS));
    wcex.hCursor = ::LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    //주 윈도우의 메뉴가 나타나지 않도록 한다.
    wcex.lpszMenuName = NULL; //AKEINTRESOURCEW(IDC_AVOIDTHEBOSS);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = ::LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return ::RegisterClassEx(&wcex);
}


BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

    RECT rc = { 0, 0,FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT };
    DWORD dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU |
        WS_BORDER;
    AdjustWindowRect(&rc, dwStyle, FALSE);   //윈도우가 원하는 클라이언트 영역 크기 가지도록 윈도우크기 계산
    HWND hMainWnd = CreateWindow(szWindowClass, szTitle, dwStyle, CW_USEDEFAULT,
        CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, hInst,
        NULL);
    if (!hMainWnd)return (FALSE);

    //----프레임워크 객체 초기화
   clientIocpCore.InitGameLoop(hInst, hMainWnd);

    ShowWindow(hMainWnd, nCmdShow);
    UpdateWindow(hMainWnd);

#ifdef _WITH_SWAPCHAIN_FULLSCREEN_STATE
    gGameFramework.ChangeSwapChainState();
#endif

    return (TRUE);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int wmId, wmEvent;
    PAINTSTRUCT ps;
    HDC hdc;

    switch (message)
    {
    case WM_SIZE:
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
    case WM_MOUSEMOVE:
    case WM_KEYDOWN:
    case WM_KEYUP:
        clientIocpCore.InputProcessing(hWnd, message, wParam, lParam);
        break;
    case WM_COMMAND:
    {
        wmId = LOWORD(wParam);
        wmEvent = HIWORD(wParam);
        // 메뉴 선택을 구문 분석합니다:
        switch (wmId)
        {
        case IDM_ABOUT:
            ::DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return ::DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_PAINT:
    {
        hdc = BeginPaint(hWnd, &ps);
        // TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다...
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        break;
    default:
        return(::DefWindowProc(hWnd, message, wParam, lParam));
    }

    return 0;
}

// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;
    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}


BOOL CALLBACK MyDialogBox(HWND hWndDlg, UINT message, WPARAM wParam, LPARAM lParam) 
{


    switch (message)
    {
    case WM_INITDIALOG:
    {
      
    }
    case WM_COMMAND:
    {
        switch (wParam)
        {
        case IDOK:
            {
            C2S_LOGIN loginPacket;
            loginPacket.size = sizeof(C2S_LOGIN);
            loginPacket.type = C_PACKET_TYPE::ACQ_LOGIN;
            GetDlgItemText(hWndDlg, IDC_ID, loginPacket.name, 10);
            GetDlgItemText(hWndDlg, IDC_PW, loginPacket.pw, 10);
            clientIocpCore.InitConnect("127.0.0.1");
            clientIocpCore.DoConnect(&loginPacket);
            while (true)
            {
                if (clientIocpCore.Processing())
                {
                    if (clientIocpCore._client->_curScene == 0)
                    {
                        EndDialog(hWndDlg, 1);
                        return TRUE;
                    }
                    else if (clientIocpCore._client->_curScene == -2)
                    {
                        EndDialog(hWndDlg, -1);
                        return TRUE;
                    }
                }
            }
            return TRUE;
            }
        case IDCANCEL:
            EndDialog(hWndDlg, IDCANCEL);
            return TRUE;
       
        }

    }
    default:
        return DefWindowProc(hWndDlg, message, wParam, lParam);
    return FALSE;
    }
}