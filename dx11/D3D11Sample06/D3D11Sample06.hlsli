cbuffer ConstantBuffer : register(b0)
{
	matrix world;
	matrix view;
	matrix projection;
	float3 light;
}

struct PS_INPUT
{
	float4 position : SV_POSITION;
	float3 viewPosition : POSVIEW;
	float3 normal : NORMAL;
	float4 color : COLOR;
};
