#include "D3D11Sample05.hlsli"

[maxvertexcount(3)]
void main(triangle GS_INPUT input[3], inout TriangleStream<PS_INPUT> triangleStream)
{
	PS_INPUT output;
	float3 faceEdge = input[0].position.xyz / input[0].position.w;
		float3 faceEdgeA = (input[1].position.xyz / input[1].position.w) - faceEdge;
		float3 faceEdgeB = (input[2].position.xyz / input[2].position.w) - faceEdge;
		output.normal = normalize(cross(faceEdgeA, faceEdgeB));

	for (int i = 0; i < 3; i++) {
		output.viewPosition = input[i].position.xyz / input[i].position.w;
		output.position = mul(input[i].position, Projection);
		output.color = input[i].color;
		triangleStream.Append(output);
	}
	triangleStream.RestartStrip();
}