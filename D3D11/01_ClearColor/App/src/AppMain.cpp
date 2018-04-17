



#define D_INIT_WINDOW_WIDTH  640
#define D_INIT_WINDOW_HEIGHT 480



HINSTANCE g_hInst = NULL;
HWND      g_hWnd = NULL;
TCHAR     g_szTitle[] = _T("Study");
TCHAR     g_szWindowClass[] = _T("Study");
std::unique_ptr<App> m_App = nullptr;



ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);




int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                      _In_opt_ HINSTANCE hPrevInstance,
                      _In_ LPWSTR    lpCmdLine,
                      _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    int result = 0;

    try
    {
        MyRegisterClass(hInstance);

        // アプリケーションの初期化を実行します:
        if (!InitInstance(hInstance, nCmdShow))
        {
            return FALSE;
        }
        MSG msg;

        while (TRUE)
        {
            // メッセージがあるかどうか
            if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
            {
                // メッセージを取得し、WM_QUITかどうか
                if (!GetMessage(&msg, NULL, 0, 0)) break;
                TranslateMessage(&msg);  //キーボード利用を可能にする
                DispatchMessage(&msg);  //制御をWindowsに戻す
            }
            else
            {
                m_App->Update();
                m_App->Render();
            }
        }
        result = (int)msg.wParam;
    }
    catch (AppException& e)
    {
        e.ShowMessageBox(g_hWnd);
        result = -1;
    }

    m_App.reset();
    return result;
}




ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = NULL;
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = g_szWindowClass;
    wcex.hIconSm = NULL;

    return RegisterClassExW(&wcex);
}



BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    g_hInst = hInstance;

    RECT WinRect{ 0, 0, D_INIT_WINDOW_WIDTH, D_INIT_WINDOW_HEIGHT };

    DWORD Style = WS_SYSMENU | WS_VISIBLE | WS_MINIMIZEBOX | WS_CAPTION;
    AdjustWindowRect(&WinRect, Style, TRUE);

    HWND hWnd = CreateWindow(
        g_szWindowClass,
        g_szTitle,
        WS_OVERLAPPEDWINDOW,
        0, 0, WinRect.right - WinRect.left, WinRect.bottom - WinRect.top,
        nullptr,
        nullptr,
        hInstance,
        nullptr);

    if (!hWnd)
    {
        return false;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    SIZE winSize{ D_INIT_WINDOW_WIDTH, D_INIT_WINDOW_HEIGHT };
    SIZE adjustSize{ WinRect.right - WinRect.left - D_INIT_WINDOW_WIDTH, WinRect.bottom - WinRect.top - D_INIT_WINDOW_HEIGHT };

    m_App = std::make_unique<App>();
    if (!m_App->Initialize(hWnd, winSize, adjustSize))
    {
        return false;
    }

    return true;
}



bool GetClientCursorPos(HWND hWnd, POINT *dest)
{
    assert(hWnd);
    assert(dest);

    if (GetCursorPos(dest))
    {
        if (ScreenToClient(hWnd, dest))
            return true;
    }
    return false;
}



LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    //POINT point;
    //int iWheel;
    //unsigned int iKeys;

    switch (message)
    {
    case WM_EXITSIZEMOVE:
    {
        RECT Rect;
        GetWindowRect(hWnd, &Rect);
        SIZE newSize{ Rect.right - Rect.left, Rect.bottom - Rect.top };
        m_App->Resize(newSize);
    }
    break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    //case WM_LBUTTONDOWN:
    //    GetClientCursorPos(hWnd, &point);
    //    iKeys = GET_KEYSTATE_WPARAM(wParam);
    //    m_App->MouseEvent(CApp::MEK_LBUTTONDOWN, point, 0, iKeys);
    //    SetCapture(hWnd);
    //    break;
    //case WM_RBUTTONDOWN:
    //    GetClientCursorPos(hWnd, &point);
    //    iKeys = GET_KEYSTATE_WPARAM(wParam);
    //    m_App->MouseEvent(CApp::MEK_RBUTTONDOWN, point, 0, iKeys);
    //    SetCapture(hWnd);
    //    break;
    //case WM_MBUTTONDOWN:
    //    GetClientCursorPos(hWnd, &point);
    //    iKeys = GET_KEYSTATE_WPARAM(wParam);
    //    m_App->MouseEvent(CApp::MEK_MBUTTONDOWN, point, 0, iKeys);
    //    SetCapture(hWnd);
    //    break;
    //case WM_LBUTTONUP:
    //    GetClientCursorPos(hWnd, &point);
    //    iKeys = GET_KEYSTATE_WPARAM(wParam);
    //    m_App->MouseEvent(CApp::MEK_LBUTTONUP, point, 0, iKeys);
    //    ReleaseCapture();
    //    break;
    //case WM_RBUTTONUP:
    //    GetClientCursorPos(hWnd, &point);
    //    iKeys = GET_KEYSTATE_WPARAM(wParam);
    //    m_App->MouseEvent(CApp::MEK_RBUTTONUP, point, 0, iKeys);
    //    ReleaseCapture();
    //    break;
    //case WM_MBUTTONUP:
    //    GetClientCursorPos(hWnd, &point);
    //    iKeys = GET_KEYSTATE_WPARAM(wParam);
    //    m_App->MouseEvent(CApp::MEK_MBUTTONUP, point, 0, iKeys);
    //    ReleaseCapture();
    //    break;
    //case WM_MOUSEMOVE:
    //    GetClientCursorPos(hWnd, &point);
    //    iKeys = GET_KEYSTATE_WPARAM(wParam);
    //    m_App->MouseEvent(CApp::MEK_MOUSEMOVE, point, 0, iKeys);
    //    break;
    //case WM_MOUSEWHEEL:
    //    GetClientCursorPos(hWnd, &point);
    //    iKeys = GET_KEYSTATE_WPARAM(wParam);
    //    iWheel = GET_WHEEL_DELTA_WPARAM(wParam);
    //    m_App->MouseEvent(CApp::MEK_MOUSEWHEEL, point, iWheel, iKeys);
    //    break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

