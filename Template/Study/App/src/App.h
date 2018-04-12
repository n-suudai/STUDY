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
};

