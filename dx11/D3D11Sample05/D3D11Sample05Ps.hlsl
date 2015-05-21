#include "D3D11Sample05.hlsli"

float4 main(PS_INPUT input) : SV_TARGET
{
	float3 light = Light - input.viewPosition;
	float bright = 30 * dot(normalize(light), input.normal) / pow(length(light), 2);
	return saturate(bright * input.color);
}
