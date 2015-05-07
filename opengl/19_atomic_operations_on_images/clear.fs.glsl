#version 430 core
#line 3 "clear.fs.glsl"

layout(binding = 0, r32ui) coherent uniform uimage2D head_pointer;

void main(void)
{
	ivec2 p = ivec2(gl_FragCoord.xy);
	imageStore(head_pointer, p, uvec4(0xffffffff));
}