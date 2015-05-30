#include "D3D11Sample13.hlsli"

struct VS_INPUT
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float2 tex : TEXTURE;
};

PS_INPUT main(VS_INPUT input)
{
	PS_INPUT output;

	float4 pos4 = float4(input.position, 1.0);
	pos4 = mul(pos4, world);
	pos4 = mul(pos4, view);
	output.viewPosition = pos4.xyz / pos4.w;
	output.position = mul(pos4, projection);

	float3 normal = mul(input.normal, (float3x3)world);
	output.normal = mul(normal, (float3x3)view);

	output.tex = input.tex;

	return output;
}