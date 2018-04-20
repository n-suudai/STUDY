

// シンプルシェーダー


static const char* s_pVertexShader = R"#(
    attribute vec4 position;
    attribute vec4 color;
    varying   vec4 vColor;

    void main(void)
    {
        gl_Position = position;
        vColor = color;
    }
)#";
const char* GetVertexShaderSource()
{
    return s_pVertexShader;
}



static const char* s_pFragmentShader = R"#(
    precision mediump float;

    varying vec4 vColor;

    void main(void)
    {
        gl_FragColor = vColor;
    }
)#";
const char* GetFragmentShaderSource()
{
    return s_pFragmentShader;
}

