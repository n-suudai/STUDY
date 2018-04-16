

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

    vs_output.position = vs_input.position;
    vs_output.color = vs_input.color;

	return vs_output;
}

