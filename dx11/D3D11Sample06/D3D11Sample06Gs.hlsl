#include "D3D11Sample06.hlsli"

[maxvertexcount(3)]
void main(
triangle float4 input[3] : SV_POSITION,
uint pID : SV_PrimitiveID,
inout TriangleStream<PS_INPUT> triangleStream)
{
	PS_INPUT output;
	output.color.r = (pID & 0x01) ? 1.0 : 0.3;
	output.color.g = (pID & 0x02) ? 1.0 : 0.3;
	output.color.b = (pID & 0x04) ? 1.0 : 0.3;
	output.color.a = 1.0;

	float3 faceEdge = input[0].xyz / input[0].w;
	float3 faceEdgeA = (input[1].xyz / input[1].w) - faceEdge;
	float3 faceEdgeB = (input[2].xyz / input[2].w) - faceEdge;
	output.normal = normalize(cross(faceEdgeA, faceEdgeB));

	for (int i = 0; i < 3; i++) {
		output.viewPosition = input[i].xyz / input[i].w;
		output.position = mul(input[i], projection);
		triangleStream.Append(output);
	}
	triangleStream.RestartStrip();
}