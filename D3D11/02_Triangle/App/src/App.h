#pragma once


// アプリケーションクラス


class Pipeline;


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

    bool CreateVertexBuffer();

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

    ComPtr<ID3D11VertexShader> m_VertexShader;
    ComPtr<ID3D11PixelShader>  m_PixelShader;

    ComPtr<ID3D11Buffer> m_VertexBuffer;

    std::unique_ptr<Pipeline> m_Pipeline;
};


class Pipeline
{
public:
    Pipeline(
        const ComPtr<ID3D11Device>& device,
        const ComPtr<ID3D11DeviceContext>& context
    );

    ~Pipeline();

    bool Initialize(const char* vertexShader, const char* pixelShader);

    void Bind();

private:
    ComPtr<ID3D11Device>        m_Device;
    ComPtr<ID3D11DeviceContext> m_Context;
    ComPtr<ID3D11InputLayout>   m_InputLayout;
    ComPtr<ID3D11VertexShader>  m_VertexShader;
    ComPtr<ID3D11PixelShader>   m_PixelShader;
};

