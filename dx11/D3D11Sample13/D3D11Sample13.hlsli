cbuffer ConstantBuffer : register(b0)
{
	matrix world;
	matrix view;
	matrix projection;
	float4 diffuse;
	float3 light;
};

struct PS_INPUT
{
	float4 position : SV_POSITION;
	float3 viewPosition : POSVIEW;
	float3 normal : NORMAL;
	float2 tex : TEXTURE;
};