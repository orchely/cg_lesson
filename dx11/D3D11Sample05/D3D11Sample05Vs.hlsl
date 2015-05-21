#include "D3D11Sample05.hlsli"

GS_INPUT main(VS_INPUT input)
{
	GS_INPUT output;
	float4 pos4 = float4(input.position, 1.0);
	output.position = mul(pos4, World);
	output.position = mul(output.position, View);
	output.color = float4(input.color, 1.0);
	return output;
}