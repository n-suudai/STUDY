// アプリケーションクラス


#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")


using VertexFormat = Vertex_PositionColorTexture;


VertexFormat g_vertices[] = {
    { { -100.0f, -100.0f, 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 0.0f, 1.0f } },
    { { -100.0f,  100.0f, 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f } },
    { {  100.0f, -100.0f, 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f } },
    { {  100.0f,  100.0f, 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 0.0f } },
};


App::App()
    : m_EnableDebug(true)
    , m_hWnd(NULL)
    , m_AdjustWindowOffset({ 0 })
    , m_ClientWindowSize({ 0 })
    , m_BufferCount(1)
    , m_BufferFormat(DXGI_FORMAT_R8G8B8A8_UNORM)
    , m_SampleDesc({ 0 })
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
            D3D_FEATURE_LEVEL_11_1,
            D3D_FEATURE_LEVEL_11_0,
            D3D_FEATURE_LEVEL_10_1,
            D3D_FEATURE_LEVEL_10_0,
            D3D_FEATURE_LEVEL_9_3,
            D3D_FEATURE_LEVEL_9_2,
            D3D_FEATURE_LEVEL_9_1,
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
        ZeroMemory(&m_SampleDesc, sizeof(m_SampleDesc));
        for (int i = 1; i <= D3D11_MAX_MULTISAMPLE_SAMPLE_COUNT; i <<= 1)
        {
            UINT Quality;
            if (SUCCEEDED(m_Device->CheckMultisampleQualityLevels(DXGI_FORMAT_D24_UNORM_S8_UINT, i, &Quality)))
            {
                if (0 < Quality)
                {
                    m_SampleDesc.Count = i;
                    m_SampleDesc.Quality = Quality - 1;
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
        swapChainDesc.SampleDesc = m_SampleDesc;
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

    // バックバッファを生成
    CreateBackBuffer(m_ClientWindowSize);

    // パイプラインを生成
    m_Pipeline = std::make_unique<Pipeline>(m_Device, m_Context);
    m_Pipeline->Initialize("shader\\VertexShader.cso", "shader\\PixelShader.cso");

    // 頂点バッファを生成
    CreateVertexBuffer();

    // テクスチャを生成
    m_Texture = std::make_unique<Texture>(m_Device, m_Context);
    m_Texture->Initialize("resource\\image\\icon.png");

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
    ConstantBufferData* pData = m_Pipeline->GetConstantBufferData();

    // ワールド変換行列を設定
    {
        static float rot = 0.0f;
        //rot += 0.0005f;

        Eigen::Affine3f affine = MatrixUtil::RotateMatrixZ(rot);
        pData->World = affine.matrix();
    }

    // ビュー変換行列を設定
    {
        Eigen::Vector3f eye(0.0f, 0.0f, -0.75f);
        Eigen::Vector3f target(0.0f, 0.0f, 0.0f);
        Eigen::Vector3f up(0.0f, 1.0f, 0.0f);

        MatrixUtil::SetLookAtMatrix(
            pData->View,
            eye,
            target,
            up
        );
    }

    // プロジェクション行列を設定
    //float aspect = static_cast<float>(m_ClientWindowSize.cx) / static_cast<float>(m_ClientWindowSize.cy);
    MatrixUtil::SetOrthographicMatrix(
        pData->Projection,
        static_cast<float>(m_ClientWindowSize.cx),
        static_cast<float>(m_ClientWindowSize.cy),
        0.0f,
        1.0f
    );
}


void App::Render()
{
    // 指定色で画面クリア
    float clearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f }; // red, green, blue, alpha
    m_Context->ClearRenderTargetView(m_RenderTargetView.Get(), clearColor);

    // 深度ステンシルビュークリア
    m_Context->ClearDepthStencilView(m_DepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    m_Texture->Bind();

    m_Pipeline->Bind();

    m_Context->Draw(_countof(g_vertices), 0);

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

    // 深度ステンシルビューを生成
    {
        DXGI_FORMAT depthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
        DXGI_FORMAT textureFormat = depthStencilFormat;
        DXGI_FORMAT resourceFormat = depthStencilFormat;

        // テクスチャとシェーダリソースビューのフォーマットを適切なものに変更.
        switch(depthStencilFormat)
        {
        case DXGI_FORMAT_D16_UNORM:
            {
                textureFormat  = DXGI_FORMAT_R16_TYPELESS;
                resourceFormat = DXGI_FORMAT_R16_UNORM;
            }
            break;
        
        case DXGI_FORMAT_D24_UNORM_S8_UINT:
            {
                textureFormat  = DXGI_FORMAT_R24G8_TYPELESS;
                resourceFormat = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
            }
            break;
        
        case DXGI_FORMAT_D32_FLOAT:
            {
                textureFormat  = DXGI_FORMAT_R32_TYPELESS;
                resourceFormat = DXGI_FORMAT_R32_FLOAT;
            }
            break;
        
        case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
            {
                textureFormat  = DXGI_FORMAT_R32G8X24_TYPELESS;
                resourceFormat = DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
            }
            break;
        }

        CD3D11_TEXTURE2D_DESC texture2DDesc(
            textureFormat,
            static_cast<UINT>(clientSize.cx),
            static_cast<UINT>(clientSize.cy),
            1,
            1,
            D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE,
            D3D11_USAGE_DEFAULT,
            0,
            m_SampleDesc.Count,
            m_SampleDesc.Quality,
            0
        );

        ThrowIfFailed(
            m_Device->CreateTexture2D(
                &texture2DDesc,
                nullptr,
                &m_DepthStencilTexture
            )
        );

        D3D11_DSV_DIMENSION dimension = D3D11_DSV_DIMENSION_TEXTURE2D;
        if (m_SampleDesc.Count > 0)
        {
            dimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
        }
        CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(
            dimension,
            depthStencilFormat
        );

        ThrowIfFailed(
            m_Device->CreateDepthStencilView(
                m_DepthStencilTexture.Get(),
                &depthStencilViewDesc,
                &m_DepthStencilView
            )
        );
    }

    // レンダーターゲットを設定
    ID3D11RenderTargetView* pRenderTargetViews[] = {
        m_RenderTargetView.Get()
    };
    m_Context->OMSetRenderTargets(_countof(pRenderTargetViews), pRenderTargetViews, m_DepthStencilView.Get());

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


bool App::CreateVertexBuffer()
{
    CD3D11_BUFFER_DESC bufferDesc(
        sizeof(g_vertices),
        D3D11_BIND_VERTEX_BUFFER
    );

    D3D11_SUBRESOURCE_DATA subresourceData;
    subresourceData.pSysMem = g_vertices;

    ThrowIfFailed(
        m_Device->CreateBuffer(
            &bufferDesc,
            &subresourceData,
            &m_VertexBuffer
        )
    );

    // 入力レイアウトに頂点バッファを設定
    UINT stride = sizeof(VertexFormat);
    UINT offset = 0;

    ID3D11Buffer* pVertexBuffers[] = {
        m_VertexBuffer.Get()
    };
    m_Context->IASetVertexBuffers(
        0,
        1,
        pVertexBuffers,
        &stride,
        &offset
    );
    m_Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

    return true;
}



Pipeline::Pipeline(
    const ComPtr<ID3D11Device>& device,
    const ComPtr<ID3D11DeviceContext>& context
)
{
    m_Device = device;
    m_Context = context;

    m_ConstantBuffer = std::make_unique<ConstantBufferClass>(
            m_Device,
            m_Context
        );
}


Pipeline::~Pipeline()
{

}


bool Pipeline::Initialize(const char* vertexShader, const char* pixelShader)
{
    // VertexShader
    {
        std::ifstream file(vertexShader, std::ios::binary);

        file.unsetf(std::ios::skipws);

        std::streampos fileSize;

        file.seekg(0, std::ios::end);
        fileSize = file.tellg();
        file.seekg(0, std::ios::beg);

        std::vector<BYTE> vertexShaderByteCode;
        vertexShaderByteCode.reserve(fileSize);

        vertexShaderByteCode.insert(vertexShaderByteCode.begin(),
                   std::istream_iterator<BYTE>(file),
                   std::istream_iterator<BYTE>());

        ThrowIfFailed(
            m_Device->CreateVertexShader(
                vertexShaderByteCode.data(),
                vertexShaderByteCode.size(),
                nullptr,
                &m_VertexShader
            )
        );

        // InputLayout
        {
            ThrowIfFailed(
                m_Device->CreateInputLayout(
                    VertexFormat::pInputElements,
                    VertexFormat::InputElementCount,
                    vertexShaderByteCode.data(),
                    vertexShaderByteCode.size(),
                    &m_InputLayout
                )
            );
        }
    }

    // PixelShader
    {
        std::ifstream file(pixelShader, std::ios::binary);

        file.unsetf(std::ios::skipws);

        std::streampos fileSize;

        file.seekg(0, std::ios::end);
        fileSize = file.tellg();
        file.seekg(0, std::ios::beg);

        std::vector<BYTE> pixelShaderByteCode;
        pixelShaderByteCode.reserve(fileSize);

        pixelShaderByteCode.insert(pixelShaderByteCode.begin(),
                   std::istream_iterator<BYTE>(file),
                   std::istream_iterator<BYTE>());

        ThrowIfFailed(
            m_Device->CreatePixelShader(
                pixelShaderByteCode.data(),
                pixelShaderByteCode.size(),
                nullptr,
                &m_PixelShader
            )
        );
    }

    // ConstantBuffer
    {
        std::unique_ptr<ConstantBufferData> data = std::make_unique<ConstantBufferData>();
        m_ConstantBuffer->Initialize(std::move(data));
    }

    // SamplerState
    {        
        FLOAT borderColor[4] = { 0.0f };
        CD3D11_SAMPLER_DESC samplerDesc(
            D3D11_FILTER_ANISOTROPIC,       // サンプリング時に使用するフィルタ。ここでは異方性フィルターを使用する
            D3D11_TEXTURE_ADDRESS_WRAP,     // 0 ～ 1 の範囲外にある u テクスチャー座標の描画方法
            D3D11_TEXTURE_ADDRESS_WRAP,     // 0 ～ 1 の範囲外にある v テクスチャー座標
            D3D11_TEXTURE_ADDRESS_WRAP,     // 0 ～ 1 の範囲外にある w テクスチャー座標
            0,                              // 計算されたミップマップ レベルからのバイアス
            16,                             // サンプリングに異方性補間を使用している場合の限界値。有効な値は 1 ～ 16 
            D3D11_COMPARISON_ALWAYS,        // 比較オプション
            borderColor,                    // 境界色
            0,                              // アクセス可能なミップマップの下限値
            D3D11_FLOAT32_MAX               // アクセス可能なミップマップの上限値
        );

        ThrowIfFailed(
            m_Device->CreateSamplerState(
                &samplerDesc,
                &m_SamplerState
            )
        );
    }

    return true;
}


void Pipeline::Bind()
{
    m_Context->IASetInputLayout(m_InputLayout.Get());

    m_Context->VSSetShader(m_VertexShader.Get(), nullptr, 0);
    m_Context->PSSetShader(m_PixelShader.Get(), nullptr, 0);

    // 定数バッファを更新
    m_ConstantBuffer->Update();

    ID3D11SamplerState* pSamplers[] = {
        m_SamplerState.Get()
    };
    m_Context->PSSetSamplers(0, _countof(pSamplers), pSamplers);
}


ConstantBufferData* Pipeline::GetConstantBufferData()
{
    return m_ConstantBuffer->Get();
}



Texture::Texture(
    const ComPtr<ID3D11Device>& device,
    const ComPtr<ID3D11DeviceContext>& context
)
{
    m_Device = device;
    m_Context = context;
}


Texture::~Texture()
{

}


bool Texture::Initialize(const char* pFileName)
{
    std::vector<unsigned char> image;
    DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;
    unsigned int width = 0;
    unsigned int height = 0;

    LoadPng(pFileName, image, format, width, height);

    void* pImage = &image[0];
    CD3D11_TEXTURE2D_DESC texture2DDesc(
        format,
        width,
        height,
        1,
        1,
        D3D11_BIND_SHADER_RESOURCE
    );

    D3D11_SUBRESOURCE_DATA subresourceData;
    ZeroMemory(&subresourceData, sizeof(subresourceData));

    subresourceData.pSysMem = pImage;
    subresourceData.SysMemPitch = DXGIFormatSystemPitch(format, width);
    subresourceData.SysMemSlicePitch = static_cast<UINT>(image.size());

    ThrowIfFailed(
        m_Device->CreateTexture2D(
            &texture2DDesc,
            &subresourceData,
            &m_Texture2D
        )
    );

    CD3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc(
        D3D11_SRV_DIMENSION_TEXTURE2D,
        format,
        0,
        texture2DDesc.MipLevels
    );
    ThrowIfFailed(
        m_Device->CreateShaderResourceView(
            m_Texture2D.Get(),
            &shaderResourceViewDesc,
            &m_ShaderResourceView
        )
    );

    return true;
}


void Texture::Bind()
{
    ID3D11ShaderResourceView* pShaderResourceViews[] = {
        m_ShaderResourceView.Get()
    };
    m_Context->PSSetShaderResources(0, _countof(pShaderResourceViews), pShaderResourceViews);
}


void Texture::LoadPng(
    const char* pFileName,
    std::vector<unsigned char>& image,
    DXGI_FORMAT& format,
    unsigned int& width,
    unsigned int& height
)
{
    std::ifstream ifs(pFileName, std::ios::binary);
    std::vector<unsigned char> png;
    png.resize((unsigned int)ifs.seekg(0, std::ios::end).tellg());
    ifs.seekg(0, std::ios::beg).read((char*)&png[0], static_cast<std::streamsize>(png.size()));

    LodePNGColorType cType = (LodePNGColorType)png[25];
    LodePNGColorType cOutType = LCT_RGBA;

    format = DXGI_FORMAT_R8G8B8A8_UNORM;

    switch (cType)
    {
    case LCT_GREY:
        format = DXGI_FORMAT_R8_UNORM;
        cOutType = cType;
        break;
    case LCT_GREY_ALPHA:
        format = DXGI_FORMAT_A8_UNORM;
        cOutType = cType;
        break;
    case LCT_RGB:
        format = DXGI_FORMAT_R8G8B8A8_UNORM;
        cOutType = LCT_RGBA;
        break;
    default:
        break;
    }

    unsigned error = lodepng::decode(image, width, height, png, cOutType);
    ThrowIfFalseMessage(error == 0, lodepng_error_text(error));
}


