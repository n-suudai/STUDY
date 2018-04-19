#pragma once


// アプリケーションクラス


class Pipeline;
struct ConstantBufferData;
template<class T>
class ConstantBuffer;
class Texture;


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
    DXGI_SAMPLE_DESC m_SampleDesc;

    DXGI_FORMAT       m_BufferFormat;
    D3D_FEATURE_LEVEL m_FeatureLevel;

    ComPtr<IDXGIFactory>   m_Factory;
    ComPtr<IDXGIAdapter>   m_Adapter;
    ComPtr<IDXGIOutput>    m_Output;
    ComPtr<IDXGISwapChain> m_SwapChain;

    ComPtr<ID3D11Device>            m_Device;
    ComPtr<ID3D11DeviceContext>     m_Context;
    ComPtr<ID3D11RenderTargetView>  m_RenderTargetView;
    ComPtr<ID3D11DepthStencilView>  m_DepthStencilView;

    ComPtr<ID3D11Texture2D> m_DepthStencilTexture;

    ComPtr<ID3D11Buffer>        m_VertexBuffer;

    std::unique_ptr<Pipeline>   m_Pipeline;
    std::unique_ptr<Texture>    m_Texture;
    
};



// 描画パイプラインクラス
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

    ConstantBufferData* GetConstantBufferData();

private:
    typedef ConstantBuffer<ConstantBufferData> ConstantBufferClass;

    ComPtr<ID3D11Device>        m_Device;
    ComPtr<ID3D11DeviceContext> m_Context;
    ComPtr<ID3D11InputLayout>   m_InputLayout;
    ComPtr<ID3D11VertexShader>  m_VertexShader;
    ComPtr<ID3D11PixelShader>   m_PixelShader;
    ComPtr<ID3D11SamplerState>  m_SamplerState;
    ComPtr<ID3D11BlendState>    m_BlendState;
    std::unique_ptr<ConstantBufferClass> m_ConstantBuffer;
};



// 定数バッファデータ
struct ConstantBufferData
{
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW;

    Eigen::Matrix4f World;
    Eigen::Matrix4f View;
    Eigen::Matrix4f Projection;
};



// 定数バッファクラス
template<class T>
class ConstantBuffer
{
public:
    inline ConstantBuffer(
        const ComPtr<ID3D11Device>& device,
        const ComPtr<ID3D11DeviceContext>& context
    )
    {
        m_Device = device;
        m_Context = context;
    }

    inline ~ConstantBuffer()
    {

    }

    inline bool Initialize(std::unique_ptr<T>&& data)
    {
        m_Data = std::move(data);

        CD3D11_BUFFER_DESC bufferDesc(
            sizeof(T),
            D3D11_BIND_CONSTANT_BUFFER
        );

        ThrowIfFailed(
            m_Device->CreateBuffer(
                &bufferDesc,
                nullptr,
                &m_ConstantBuffer
            )
        );

        return true;
    }

    inline T* Get()
    {
        return m_Data.get();
    }

    inline void Update()
    {
        // サブリソースを更新
        m_Context->UpdateSubresource(m_ConstantBuffer.Get(), 0, nullptr, m_Data.get(), 0, 0);

        ID3D11Buffer* pConstantBuffers[] = {
            m_ConstantBuffer.Get()
        };

        // 頂点シェーダーに定数バッファを設定
        m_Context->VSSetConstantBuffers(0, _countof(pConstantBuffers), pConstantBuffers);
    }

private:
    ComPtr<ID3D11Device>        m_Device;
    ComPtr<ID3D11DeviceContext> m_Context;
    ComPtr<ID3D11Buffer>        m_ConstantBuffer;
    std::unique_ptr<T>          m_Data;
};


class Texture
{
public:
    Texture(
        const ComPtr<ID3D11Device>& device,
        const ComPtr<ID3D11DeviceContext>& context
    );

    ~Texture();

    bool Initialize(const char* pFileName);

    void Bind();

private:
    static void LoadPng(
        const char* pFileName,
        std::vector<unsigned char>& image,
        DXGI_FORMAT& format,
        unsigned int& width,
        unsigned int& height
    );

private:
    ComPtr<ID3D11Device>             m_Device;
    ComPtr<ID3D11DeviceContext>      m_Context;
    ComPtr<ID3D11Texture2D>          m_Texture2D;
    ComPtr<ID3D11ShaderResourceView> m_ShaderResourceView;
};

