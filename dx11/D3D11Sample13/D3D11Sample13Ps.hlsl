#include "D3D11Sample13.hlsli"

Texture2D tex;
SamplerState texSampler : register(s0);

float lighting(PS_INPUT input)
{
	float3 pixelLight = light - input.viewPosition;
	return 30 * dot(normalize(pixelLight), normalize(input.normal)) / pow(length(pixelLight), 2);
}

float4 main(PS_INPUT input) : SV_TARGET
{
	float bright = lighting(input);
	float4 texColor = tex.Sample(texSampler, input.tex);
	return saturate(bright * texColor * diffuse);
}


