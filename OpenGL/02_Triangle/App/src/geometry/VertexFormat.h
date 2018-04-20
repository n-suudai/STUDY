#pragma once


// 頂点フォーマット


struct Vertex_PositionColor
{
    float Position[4];
    float Color[4];

    static constexpr size_t PositionOffset = 0;
    static constexpr size_t ColorOffset = PositionOffset + sizeof(float)*4;
};


