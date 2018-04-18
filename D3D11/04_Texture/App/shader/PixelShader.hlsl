


Texture2D diffuseTexture : register(t0);
SamplerState linearSampler : register(s0);


struct PS_INPUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 tex : TEXCOORD0;
};


float4 main(PS_INPUT ps_input) : SV_TARGET
{
    float4 texColor = diffuseTexture.Sample(linearSampler, ps_input.tex);
	return ps_input.color * texColor;
}

