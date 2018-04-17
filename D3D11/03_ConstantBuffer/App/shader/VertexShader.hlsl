
cbuffer CB0 : register(b0)
{
    float4x4 World;
    float4x4 View;
    float4x4 Projection;
};


struct VS_INPUT
{
    float4 position : POSITION0;
    float4 color : COLOR0;
};


struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};


VS_OUTPUT main(VS_INPUT vs_input)
{
    VS_OUTPUT vs_output = (VS_OUTPUT)0;

    vs_output.position = mul(vs_input.position, World);
    vs_output.position = mul(vs_output.position, View);
    vs_output.position = mul(vs_output.position, Projection);
    vs_output.color = vs_input.color;

	return vs_output;
}

