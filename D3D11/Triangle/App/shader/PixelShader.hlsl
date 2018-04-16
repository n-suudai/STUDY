

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};


float4 main(PS_INPUT ps_input) : SV_TARGET
{
	return ps_input.color;
}