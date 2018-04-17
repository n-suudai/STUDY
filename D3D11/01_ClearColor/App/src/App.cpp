// アプリケーションクラス


#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")


App::App()
    : m_EnableDebug(true)
    , m_hWnd(NULL)
    , m_AdjustWindowOffset({0})
    , m_ClientWindowSize({0})
    , m_BufferCount(1)
    , m_BufferFormat(DXGI_FORMAT_R8G8B8A8_UNORM)
{

}


App::~App()
{

}


bool App::Initialize(HWND hWnd, const SIZE& clientSize, const SIZE& adjustSize)
{
    m_hWnd = hWnd;

    m_AdjustWindowOffset = adjustSize;

    m_ClientWindowSize = clientSize;

    // ファクトリーを生成
    ThrowIfFailed(
        CreateDXGIFactory(IID_PPV_ARGS(&m_Factory))
    );

    // デフォルトアダプターを取得
    ThrowIfFailed(
        m_Factory->EnumAdapters(0, &m_Adapter)
    );

    // デフォルトディスプレイを取得
    ThrowIfFailed(
        m_Adapter->EnumOutputs(0, &m_Output)
    );

    // デバイス&コンテキストを生成
    {
        // BGRAサポートを有効化
        uint32_t createDeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

        if (m_EnableDebug)
        {
            createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
        }

        D3D_FEATURE_LEVEL featureLevels[] = {
            D3D_FEATURE_LEVEL_9_1,
            D3D_FEATURE_LEVEL_9_2,
            D3D_FEATURE_LEVEL_9_3,
            D3D_FEATURE_LEVEL_10_0,
            D3D_FEATURE_LEVEL_10_1,
            D3D_FEATURE_LEVEL_11_0,
            D3D_FEATURE_LEVEL_11_1,
        };

        ThrowIfFailed(
            D3D11CreateDevice(
                m_Adapter.Get(),
                D3D_DRIVER_TYPE_UNKNOWN,
                nullptr,
                createDeviceFlags,
                featureLevels,
                _countof(featureLevels),
                D3D11_SDK_VERSION,
                &m_Device,
                &m_FeatureLevel,
                &m_Context
            )
        );
    }

    {
        // 使用可能なMSAAを取得
        DXGI_SAMPLE_DESC sampleDesc;
        ZeroMemory(&sampleDesc, sizeof(sampleDesc));
        for (int i = 1; i <= D3D11_MAX_MULTISAMPLE_SAMPLE_COUNT; i <<= 1)
        {
            UINT Quality;
            if (SUCCEEDED(m_Device->CheckMultisampleQualityLevels(DXGI_FORMAT_D24_UNORM_S8_UINT, i, &Quality)))
            {
                if (0 < Quality)
                {
                    sampleDesc.Count = i;
                    sampleDesc.Quality = Quality - 1;
                }
            }
        }

        // スワップチェインを生成
        DXGI_SWAP_CHAIN_DESC swapChainDesc;
        ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
        swapChainDesc.BufferDesc.Width = static_cast<UINT>(m_ClientWindowSize.cx);
        swapChainDesc.BufferDesc.Height = static_cast<UINT>(m_ClientWindowSize.cy);
        swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
        swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
        swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
        swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
        swapChainDesc.BufferDesc.Format = m_BufferFormat;
        swapChainDesc.SampleDesc = sampleDesc;
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.BufferCount = m_BufferCount;
        swapChainDesc.Windowed = TRUE;
        swapChainDesc.OutputWindow = m_hWnd;
        swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

        ThrowIfFailed(
            m_Factory->CreateSwapChain(
                m_Device.Get(),
                &swapChainDesc,
                &m_SwapChain
            )
        );
    }

    CreateBackBuffer(m_ClientWindowSize);

    return true;
}


bool App::Resize(const SIZE& newAppWindowSize)
{
    SIZE newClientSize = {
        newAppWindowSize.cx - m_AdjustWindowOffset.cx,
        newAppWindowSize.cy - m_AdjustWindowOffset.cy
    };

    return ResizeBackBuffer(newClientSize);
}


void App::Update()
{

}


void App::Render()
{
    // 指定色で画面クリア
    float clearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f }; // red, green, blue, alpha
    m_Context->ClearRenderTargetView(m_RenderTargetView.Get(), clearColor);

    //結果をウインドウに反映
    ThrowIfFailed(
        m_SwapChain->Present(0, 0)
    );
}


bool App::ResizeBackBuffer(const SIZE& clientSize, bool forceUpdate)
{
    if (forceUpdate ||
        clientSize.cx != m_ClientWindowSize.cx ||
        clientSize.cy != m_ClientWindowSize.cy)
    {
        ReleaseBackBuffer();

        ThrowIfFailed(
            m_SwapChain->ResizeBuffers(
                m_BufferCount,
                static_cast<UINT>(m_ClientWindowSize.cx),
                static_cast<UINT>(m_ClientWindowSize.cy),
                m_BufferFormat,
                DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH
            )
        );

        return CreateBackBuffer(clientSize);
    }

    return true;
}


bool App::CreateBackBuffer(const SIZE& clientSize)
{
    m_ClientWindowSize = clientSize;

    // レンダーターゲットを生成
    {
        ComPtr<ID3D11Texture2D> backBuffer;

        ThrowIfFailed(
            m_SwapChain->GetBuffer(
                0,
                IID_PPV_ARGS(&backBuffer)
            )
        );

        HRESULT hr = m_Device->CreateRenderTargetView(
            backBuffer.Get(),
            nullptr,
            &m_RenderTargetView
        );
        ThrowIfFailed(
            hr
        );
    }

    // レンダーターゲットを設定
    ID3D11RenderTargetView* pRenderTargetViews[] = {
        m_RenderTargetView.Get()
    };
    m_Context->OMSetRenderTargets(_countof(pRenderTargetViews), pRenderTargetViews, nullptr);

    // ビューポートを設定
    D3D11_VIEWPORT viewport;
    viewport.Width = static_cast<FLOAT>(clientSize.cx);
    viewport.Height = static_cast<FLOAT>(clientSize.cy);
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;
    m_Context->RSSetViewports(1, &viewport);

    return true;
}


void App::ReleaseBackBuffer()
{
    m_Context->OMSetRenderTargets(0, nullptr, nullptr);
    m_RenderTargetView.Reset();
}

