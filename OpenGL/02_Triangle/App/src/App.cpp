

#pragma comment(lib,"OpenGL32.lib")
#pragma comment(lib,"glew32s.lib")


#define BUFFER_OFFSET(offset) ((void *)(offset))
using VertexFormat = Vertex_PositionColor;


VertexFormat g_vertices[] = {
    { { 0.0f, 0.5f, 0.0f, 1.0f }, { 1.0f, 1.0f, 0.0f, 1.0f } },
    { { 0.4f,  -0.25f, 0.0f, 1.0f }, { 1.0f, 1.0f, 0.0f, 1.0f } },
    { { -0.4f, -0.25f, 0.0f, 1.0f },  { 1.0f, 1.0f, 0.0f, 1.0f } },
};


// アプリケーションクラス


App::App()
    : m_hWnd(nullptr)
    , m_hDC(nullptr)
    , m_hGL(nullptr)
    , m_ProgramId(0)
    , m_VBO(0)
{

}

App::~App()
{
    Release();
}

bool App::Initialize(HWND hWnd, const SIZE&, const SIZE&)
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
    wglMakeCurrent(m_hDC, m_hGL); //ここからOpenGL関数（gl～）が使用可能になる


    // GLEW初期化 ※wglMakeCurrentの後に呼び出さないと失敗する
    ThrowIfFalse(glewInit() == GLEW_OK);


    std::string version = "OpenGL Version ";
    version += reinterpret_cast<const char*>(glGetString(GL_VERSION));
    version += "\n";
    OutputDebugStringA(version.c_str());

    // シェーダーを生成
    CreateShaderProgram();

    // 頂点バッファを生成
    CreateVertexBuffer();

    return true;
}

void App::Release()
{
    if (m_VBO != 0)
    {
        glDeleteBuffers(1, &m_VBO);
        ThrowIfGLError("glDeleteBuffers");
        m_VBO = 0;
    }

    if (m_ProgramId != 0)
    {
        glDeleteProgram(m_ProgramId);
        ThrowIfGLError("glDeleteProgram");
        m_ProgramId = 0;
    }

    //削除
    //OpenGLコンテキストの削除
    if (m_hGL != nullptr)
    {
        wglDeleteContext(m_hGL);
        m_hGL = nullptr;
    }

    //デバイスコンテキスト解放
    if (m_hDC != nullptr)
    {
        ReleaseDC(m_hWnd, m_hDC);
        m_hDC = nullptr;
    }
}

bool App::Resize(const SIZE&)
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
    ThrowIfGLError("glClearColor");

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    ThrowIfGLError("glClear");

    glClearDepth(1.0);
    ThrowIfGLError("glClearDepth");

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    ThrowIfGLError("glBindBuffer");

    glDrawArrays(GL_TRIANGLES, 0, _countof(g_vertices));
    ThrowIfGLError("glDrawArrays");

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    ThrowIfGLError("glBindBuffer");

    //描画処理を実行
    glFlush();
    ThrowIfGLError("glFlush");

    //ダブルバッファ（描画バッファと表示バッファ）入れ替え
    //Windows用関数　g_hDC:デバイスコンテキスト
    SwapBuffers(m_hDC);
}


bool App::CreateShaderProgram()
{
    // 頂点シェーダーのコンパイル
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    ThrowIfGLError("glCreateShader");
    
    const char* pVertesShaderSource = GetVertexShaderSource();

    glShaderSource(vertexShader, 1, &pVertesShaderSource, nullptr);
    ThrowIfGLError("glShaderSource");

    glCompileShader(vertexShader);
    ThrowIfGLError("glCompileShader");

    GLint compileSuccess = GL_TRUE;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &compileSuccess);
    ThrowIfGLError("glGetShaderiv");

    if (compileSuccess == GL_FALSE)
    {
        GLint infoLen = 0;
        // エラーメッセージを取得
        glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &infoLen);
        ThrowIfGLError("glGetShaderiv");
        if (infoLen > 1)
        {
            GLchar *message = (GLchar*)calloc(infoLen, sizeof(GLchar));
            glGetShaderInfoLog(vertexShader, infoLen, NULL, message);
            ThrowIfGLError("glGetShaderInfoLog");

            MessageBoxA(m_hWnd, (LPCSTR)message, "GLSL Compile Error", MB_OK);
            free((void*)message);
        }
    }


    // フラグメントシェーダーのコンパイル
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    ThrowIfGLError("glCreateShader");

    const char* pFragmentShaderSource = GetFragmentShaderSource();

    glShaderSource(fragmentShader, 1, &pFragmentShaderSource, nullptr);
    ThrowIfGLError("glShaderSource");

    glCompileShader(fragmentShader);
    ThrowIfGLError("glCompileShader");

    compileSuccess = GL_TRUE;
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &compileSuccess);
    ThrowIfGLError("glGetShaderiv");
    if (compileSuccess == GL_FALSE)
    {
        GLint infoLen = 0;
        // エラーメッセージを取得
        glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &infoLen);
        ThrowIfGLError("glGetShaderiv");
        if (infoLen > 1)
        {
            GLchar *message = (GLchar*)calloc(infoLen, sizeof(GLchar));
            glGetShaderInfoLog(fragmentShader, infoLen, NULL, message);
            ThrowIfGLError("glGetShaderInfoLog");

            MessageBoxA(m_hWnd, (LPCSTR)message, "GLSL Compile Error", MB_OK);
            free((void*)message);
        }
    }

    //プログラムオブジェクトの作成
    m_ProgramId = glCreateProgram();
    ThrowIfGLError("glCreateProgram");

    glAttachShader(m_ProgramId, vertexShader);
    ThrowIfGLError("glAttackShader");

    glAttachShader(m_ProgramId, fragmentShader);
    ThrowIfGLError("glAttackShader");

    // リンク
    glLinkProgram(m_ProgramId);
    ThrowIfGLError("glLinkProgram");

    glUseProgram(m_ProgramId);
    ThrowIfGLError("glUseProgram");

    return true;
}


bool App::CreateVertexBuffer()
{
    glGenBuffers(1, &m_VBO);
    ThrowIfGLError("glGenBuffers");

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    ThrowIfGLError("glBindBuffer");

    glBufferData(GL_ARRAY_BUFFER, sizeof(VertexFormat) * _countof(g_vertices), g_vertices, GL_STATIC_DRAW);
    ThrowIfGLError("glBufferData");

    // position
    {
        GLint position = glGetAttribLocation(m_ProgramId, "position");
        ThrowIfGLError("glGetAttribLocation");

        glEnableVertexAttribArray(position);
        ThrowIfGLError("glEnableVertexAttribArray");

        glVertexAttribPointer(position, 4, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), BUFFER_OFFSET(VertexFormat::PositionOffset));
        ThrowIfGLError("glVertexAttribPointer");
    }

    // color
    {
        GLint color = glGetAttribLocation(m_ProgramId, "color");
        ThrowIfGLError("glGetAttribLocation");

        glEnableVertexAttribArray(color);
        ThrowIfGLError("glEnableVertexAttribArray");

        glVertexAttribPointer(color, 4, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), BUFFER_OFFSET(VertexFormat::ColorOffset));
        ThrowIfGLError("glVertexAttribPointer");
    }

    return true;
}


