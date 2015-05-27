#include "D3D11Sample12.hlsli"

Texture2D tex;
SamplerState textureSampler;

float4 main(PS_INPUT input) : SV_TARGET
{
	float bright = lighting(input);
	
	float4 texCol = tex.Sample(textureSampler, input.tex);
	return saturate(bright * texCol);
}