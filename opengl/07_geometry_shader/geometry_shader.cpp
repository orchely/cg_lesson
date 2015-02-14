#include <GL/glew.h>
#include <GL/freeglut.h>

GLuint rendering_program;
GLuint vertex_array_object;

GLuint compile_shaders(void)
{
	GLuint vertex_shader;
	GLuint tessellation_control_shader;
	GLuint tessellation_evaluation_shader;
	GLuint geometory_shader;
	GLuint fragment_shader;
	GLuint program;

	static const GLchar *vertex_shader_source[] = {
		"#version 430 core													\n"
		"																	\n"
		"void main(void)													\n"
		"{																	\n"
		"	const vec4 vertices[3] = vec4[3](vec4( 0.25, -0.25, 0.5, 1.0),	\n"
		"	                                 vec4(-0.25, -0.25, 0.5, 1.0),	\n"
		"	                                 vec4( 0.25,  0.25, 0.5, 1.0));	\n"
		"	gl_Position = vertices[gl_VertexID];							\n"
		"}																	\n"
	};

	static const GLchar *tessellation_control_shader_soure[] = {
		"#version 430 core																\n"
		"																				\n"
		"layout (vertices = 3) out;														\n"
		"																				\n"
		"void main(void)																\n"
		"{																				\n"
		"	if (gl_InvocationID == 0)													\n"
		"	{																			\n"
		"		gl_TessLevelInner[0] = 5.0;												\n"
		"		gl_TessLevelOuter[0] = 5.0;												\n"
		"		gl_TessLevelOuter[1] = 5.0;												\n"
		"		gl_TessLevelOuter[2] = 5.0;												\n"
		"	}																			\n"
		"	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;	\n"
		"}																				\n"
	};

	static const GLchar *tessellation_evaluation_shader_source[] = {
		"#version 430 core																\n"
		"																				\n"
		"layout (triangles, equal_spacing, cw) in;										\n"
		"																				\n"
		"void main(void)																\n"
		"{																				\n"
		"	gl_Position = (gl_TessCoord.x * gl_in[0].gl_Position +						\n"
		"	               gl_TessCoord.y * gl_in[1].gl_Position +						\n"
		"	               gl_TessCoord.z * gl_in[2].gl_Position);						\n"
		"}																				\n"
	};

	static const GLchar *geometry_shader_source[] = {
		"#version 430 core								\n"
		"												\n"
		"layout (triangles) in;							\n"
		"layout (points, max_vertices = 3) out;			\n"
		"												\n"
		"void main(void)								\n"
		"{												\n"
		"	int i;										\n"
		"												\n"
		"	for (i = 0; i < gl_in.length(); i++)		\n"
		"	{											\n"
		"		gl_Position = gl_in[i]. gl_Position;	\n"
		"		EmitVertex();							\n"
		"	}											\n"
		"}												\n"
	};

	static const GLchar *fragment_shader_source[] = {
		"#version 430 core							\n"
		"											\n"
		"out vec4 color;							\n"
		"											\n"
		"void main(void)							\n"
		"{											\n"
		"	color = vec4(0.0, 0.8, 1.0, 1.0);		\n"
		"}											\n"
	};

	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, vertex_shader_source, nullptr);
	glCompileShader(vertex_shader);

	tessellation_control_shader = glCreateShader(GL_TESS_CONTROL_SHADER);
	glShaderSource(tessellation_control_shader, 1, tessellation_control_shader_soure, nullptr);
	glCompileShader(tessellation_control_shader);

	tessellation_evaluation_shader = glCreateShader(GL_TESS_EVALUATION_SHADER);
	glShaderSource(tessellation_evaluation_shader, 1, tessellation_evaluation_shader_source, nullptr);
	glCompileShader(tessellation_evaluation_shader);

	geometory_shader = glCreateShader(GL_GEOMETRY_SHADER);
	glShaderSource(geometory_shader, 1, geometry_shader_source, nullptr);
	glCompileShader(geometory_shader);

	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, fragment_shader_source, nullptr);
	glCompileShader(fragment_shader);

	program = glCreateProgram();
	glAttachShader(program, vertex_shader);
	glAttachShader(program, tessellation_control_shader);
	glAttachShader(program, tessellation_evaluation_shader);
	glAttachShader(program, geometory_shader);
	glAttachShader(program, fragment_shader);
	glLinkProgram(program);

	glDeleteShader(vertex_shader);
	glDeleteShader(tessellation_control_shader);
	glDeleteShader(tessellation_evaluation_shader);
	glDeleteShader(geometory_shader);
	glDeleteShader(fragment_shader);

	return program;
}

void startup(void)
{
	rendering_program = compile_shaders();
	glGenVertexArrays(1, &vertex_array_object);
	glBindVertexArray(vertex_array_object);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glPointSize(5.0f);
}

void render(double currentTime)
{
	const GLfloat color[] = { 0.0f, 0.2f, 0.0f, 1.0f };
	glClearBufferfv(GL_COLOR, 0, color);
	glUseProgram(rendering_program);
	glDrawArrays(GL_PATCHES, 0, 3);
}

void shutdown(void)
{
	glDeleteVertexArrays(1, &vertex_array_object);
	glDeleteProgram(rendering_program);
}