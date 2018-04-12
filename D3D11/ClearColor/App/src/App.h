#pragma once


// アプリケーションクラス


class App
{
public:
    App();

    ~App();

    bool Initialize(HWND hWnd, const SIZE& clientSize, const SIZE& adjustSize);

    bool Resize(const SIZE& newAppWindowSize);

    void Update();

    void Render();



private:
    bool ResizeBackBuffer(const SIZE& clientSize, bool forceUpdate = false);

    bool CreateBackBuffer(const SIZE& clientSize);

    void ReleaseBackBuffer();



private:
    bool m_EnableDebug;
    HWND m_hWnd;
    SIZE m_AdjustWindowOffset;
    SIZE m_ClientWindowSize;
    UINT m_BufferCount;

    DXGI_FORMAT       m_BufferFormat;
    D3D_FEATURE_LEVEL m_FeatureLevel;

    ComPtr<IDXGIFactory>   m_Factory;
    ComPtr<IDXGIAdapter>   m_Adapter;
    ComPtr<IDXGIOutput>    m_Output;
    ComPtr<IDXGISwapChain> m_SwapChain;

    ComPtr<ID3D11Device>           m_Device;
    ComPtr<ID3D11DeviceContext>    m_Context;
    ComPtr<ID3D11RenderTargetView> m_RenderTargetView;

};

