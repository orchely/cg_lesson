#include "D3D11Sample12.hlsli"

Texture2D tex;

float4 main(PS_INPUT input) : SV_TARGET
{
	float bright = lighting(input);
	
	uint width, height;
	tex.GetDimensions(width, height);
	float3 texCoord = float3(input.tex.x * width, input.tex.y * height, 0);
	float4 texCol = tex.Load(texCoord);
	return saturate(bright * texCol);
}