#include "D3D11Sample06.hlsli"

float4 main(PS_INPUT input) : SV_TARGET
{
	float3 pixelLight = light - input.viewPosition;
	float bright = 30 * dot(normalize(pixelLight), normalize(input.normal)) / pow(length(pixelLight), 2);
	return saturate(bright * input.color);
}
