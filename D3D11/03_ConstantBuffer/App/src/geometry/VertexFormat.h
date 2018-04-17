#pragma once


// 頂点フォーマット


struct Vertex_PositionColor
{
    float Position[4];
    float Color[4];

    static const D3D11_INPUT_ELEMENT_DESC *pInputElements;
    static const UINT                      InputElementCount;
};

