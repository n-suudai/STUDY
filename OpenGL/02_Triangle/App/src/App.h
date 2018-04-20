#pragma once


// アプリケーションクラス


class App
{
public:
    App();

    ~App();

    bool Initialize(HWND hWnd, const SIZE& clientSize, const SIZE& adjustSize);

    void Release();

    bool Resize(const SIZE& newSize);

    void Update();

    void Render();

private:
    bool CreateShaderProgram();

    bool CreateVertexBuffer();

private:
    HWND  m_hWnd;
    HDC   m_hDC;
    HGLRC m_hGL;

    GLuint m_ProgramId;
    GLuint m_VBO;
};

