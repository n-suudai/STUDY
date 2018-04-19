

#pragma comment(lib,"OpenGL32.lib")


// アプリケーションクラス


App::App()
    : m_hWnd(nullptr)
    , m_hDC(nullptr)
    , m_hGL(nullptr)
{

}

App::~App()
{
    Release();
}

bool App::Initialize(HWND hWnd, const SIZE& clientSize, const SIZE& adjustSize)
{
    m_hWnd = hWnd;

    //デバイスコンテキスト取得
    m_hDC = GetDC(hWnd);//hwnd:ウインドウハンドル

    //OpenGLが使用可能なピクセルフォーマットを取得
    PIXELFORMATDESCRIPTOR pfdesc;
    ZeroMemory(&pfdesc, sizeof(pfdesc));
    pfdesc.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pfdesc.nVersion = 1;
    pfdesc.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfdesc.iPixelType = PFD_TYPE_RGBA;
    pfdesc.cColorBits = 32;
    pfdesc.cDepthBits = 24;
    pfdesc.cStencilBits = 8;
    int format = ChoosePixelFormat(m_hDC, &pfdesc);

    //format==0 失敗 対応したフォーマットがない
    ThrowIfFalseMessage(format != 0, "ChoosePixelFormat(hDC, &pfdesc)");

    // 取得したピクセルフォーマットを設定
    //　これで指定したウインドウがOpenGLで使用可能に
    SetPixelFormat(m_hDC, format, &pfdesc);

    //OpenGLコンテキスト作成
    m_hGL = wglCreateContext(m_hDC);
    wglMakeCurrent(m_hDC, m_hGL);

    //ここからOpenGL関数（gl～）が使用可能になる	

    return true;
}

void App::Release()
{
    //削除
    //OpenGLコンテキストの削除
    wglDeleteContext(m_hGL);

    //デバイスコンテキスト解放
    ReleaseDC(m_hWnd, m_hDC);
}

bool App::Resize(const SIZE& newSize)
{
    return true;
}

void App::Update()
{

}

void App::Render()
{
    //画面クリア
    glClearColor(0.0f, 0.125f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    ////ポリゴン描画
    //glBegin(GL_TRIANGLES);
    //glColor3f(0.0f, 0.0f, 1.0f);
    //glVertex3f(0.0f, 0.5f, 0.0f);
    //glColor3f(0.0f, 1.0f, 0.0f);
    //glVertex3f(0.5f, -0.5f, 0.0f);
    //glColor3f(1.0f, 0.0f, 0.0f);
    //glVertex3f(-0.5f, -0.5f, 0.0f);
    //glEnd();

    //描画処理を実行
    glFlush();

    //ダブルバッファ（描画バッファと表示バッファ）入れ替え
    //Windows用関数　g_hDC:デバイスコンテキスト
    SwapBuffers(m_hDC);
}

