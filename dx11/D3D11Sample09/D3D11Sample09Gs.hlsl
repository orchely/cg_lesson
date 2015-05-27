#include "D3D11Sample09.hlsli"

[maxvertexcount(3)]
void main(
	triangle float4 input[3] : SV_POSITION,
	uint pID : SV_PrimitiveID,
	inout TriangleStream<PS_INPUT> triangleStream)
{
	PS_INPUT output;

	static const float2 tID[6] = {
		{ 0.0, 0.0 },
		{ 1.0, 0.0 },
		{ 0.0, 1.0 },
		{ 1.0, 0.0 },
		{ 1.0, 1.0 },
		{ 0.0, 1.0 }
	};
	uint tIdx = (pID & 0x01) ? 3 : 0;

	float3 faceEdge = input[0].xyz / input[0].w;
		float3 faceEdgeA = (input[1].xyz / input[1].w) - faceEdge;
		float3 faceEdgeB = (input[2].xyz / input[2].w) - faceEdge;
		output.normal = normalize(cross(faceEdgeA, faceEdgeB));

	for (int i = 0; i < 3; i++) {
		output.viewPosition = input[i].xyz / input[i].w;
		output.position = mul(input[i], projection);
		output.tex = tID[tIdx + i];
		triangleStream.Append(output);
	}
	triangleStream.RestartStrip();
}