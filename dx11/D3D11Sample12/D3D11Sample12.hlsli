cbuffer ConstantBuffer : register(b0)
{
	matrix world;
	matrix view;
	matrix projection;
	float3 light;
};

struct PS_INPUT
{
	float4 position : SV_POSITION;
	float3 viewPosition : POSVIEW;
	float3 normal : NORMAL;
	float2 tex : TEXTURE;
};

float lighting(PS_INPUT input)
{
	float3 pixelLight = light - input.viewPosition;
	return 30 * dot(normalize(pixelLight), normalize(input.normal)) / pow(length(pixelLight), 2);
}
