


// 頂点フォーマット


const D3D11_INPUT_ELEMENT_DESC Vertex_PositionColor_InputElements[] = {
    { "POSITION", 0,    DXGI_FORMAT_R32G32B32A32_FLOAT, 0,  0,                  D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "COLOR",    0,    DXGI_FORMAT_R32G32B32A32_FLOAT, 0,  sizeof(float)*4,    D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

const D3D11_INPUT_ELEMENT_DESC* Vertex_PositionColor::pInputElements = Vertex_PositionColor_InputElements;
const UINT Vertex_PositionColor::InputElementCount = _countof(Vertex_PositionColor_InputElements);


