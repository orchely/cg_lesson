#version 430 core
#line 3 "clear.vs.glsl"

void main(void)
{
	const vec4 vertices[] = vec4[](
		vec4(-1.0f, -1.0f, 0.5f, 1.0f),
		vec4( 1.0f, -1.0f, 0.5f, 1.0f),
		vec4(-1.0f,  1.0f, 0.5f, 1.0f),
		vec4( 1.0f,  1.0f, 0.5f, 1.0f));
	gl_Position = vertices[gl_VertexID];
}