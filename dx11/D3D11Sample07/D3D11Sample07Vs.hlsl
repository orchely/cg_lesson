#include "D3D11Sample07.hlsli"

float4 main(uint vID : SV_VertexID) : SV_POSITION
{
	float4 pos = float4(0.0, 0.0, 0.0, 1.0);

	static const uint pID[36] = {
		0, 1, 3,
		1, 2, 3,
		1, 5, 2,
		5, 6, 2,
		5, 4, 6,
		4, 7, 6,
		4, 5, 0,
		5, 1, 0,
		4, 0, 7,
		0, 3, 7,
		3, 2, 7,
		2, 6, 7
	};

	switch (pID[vID]) {
	case 0: pos = float4(-1.0,  1.0, -1.0,  1.0); break;
	case 1: pos = float4( 1.0,  1.0, -1.0,  1.0); break;
	case 2: pos = float4( 1.0, -1.0, -1.0,  1.0); break;
	case 3: pos = float4(-1.0, -1.0, -1.0,  1.0); break;
	case 4: pos = float4(-1.0,  1.0,  1.0,  1.0); break;
	case 5: pos = float4( 1.0,  1.0,  1.0,  1.0); break;
	case 6: pos = float4( 1.0, -1.0,  1.0,  1.0); break;
	case 7: pos = float4(-1.0, -1.0,  1.0,  1.0); break;
	}

	pos = mul(pos, world);
	pos = mul(pos, view);

	return pos;
}