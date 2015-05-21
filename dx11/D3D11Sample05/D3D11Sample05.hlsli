cbuffer NeverChanges : register(b0)
{
	matrix Projection;
};

cbuffer ChangesEveryFrame : register(b1)
{
	matrix View;
	float3 Light;
};

cbuffer ChangesEveryObject : register(b2)
{
	matrix World;
};

struct VS_INPUT
{
	float3 position : POSITION;
	float3 color : COLOR;
};

struct GS_INPUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

struct PS_INPUT
{
	float4 position : SV_POSITION;
	float3 viewPosition : POSVIEW;
	float3 normal : NORMAL;
	float4 color : COLOR;
};