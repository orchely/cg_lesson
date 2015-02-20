#include <math.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

GLuint rendering_program;
GLuint vertex_array_object;

GLuint compile_shaders(void)
{
	GLuint vertex_shader;
	GLuint fragment_shader;
	GLuint program;

	static const GLchar *vertex_shader_source[] = {
		"#version 430 core													\n"
		"																	\n"
		"layout (location = 0) in vec4 offset;								\n"
		"layout (location = 1) in vec4 color;								\n"
		"																	\n"
		"out VS_OUT															\n"
		"{																	\n"
		"	vec4 color;														\n"
		"} vs_out;															\n"
		"																	\n"
		"void main(void)													\n"
		"{																	\n"
		"	const vec4 vertices[3] = vec4[3](vec4( 0.25, -0.25, 0.5, 1.0),	\n"
		"	                                 vec4(-0.25, -0.25, 0.5, 1.0),	\n"
		"	                                 vec4( 0.25,  0.25, 0.5, 1.0));	\n"
		"	gl_Position = vertices[gl_VertexID] + offset;					\n"
		"																	\n"
		"	vs_out.color = color;											\n"
		"}																	\n"
	};

	static const GLchar *fragment_shader_source[] = {
		"#version 430 core							\n"
		"											\n"
		"in VS_OUT									\n"
		"{											\n"
		"	vec4 color;								\n"
		"} fs_in;									\n"
		"											\n"
		"out vec4 color;							\n"
		"											\n"
		"void main(void)							\n"
		"{											\n"
		"	color = fs_in.color;					\n"
		"}											\n"
	};

	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, vertex_shader_source, nullptr);
	glCompileShader(vertex_shader);

	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, fragment_shader_source, nullptr);
	glCompileShader(fragment_shader);

	program = glCreateProgram();
	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);
	glLinkProgram(program);

	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	return program;
}

void startup(void)
{
	rendering_program = compile_shaders();
	glGenVertexArrays(1, &vertex_array_object);
	glBindVertexArray(vertex_array_object);
}

// Our rendering function
void render(double currentTime)
{
	const GLfloat bg_color[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	glClearBufferfv(GL_COLOR, 0, bg_color);

	// Use the program object we created earlier for rendering
	glUseProgram(rendering_program);
	GLfloat attrib[] = {
		static_cast<float>(sin(currentTime)) * 0.5f,
		static_cast<float>(cos(currentTime)) * 0.6f,
		0.0f, 0.0f
	};
	// Update the value of input attribute 0
	glVertexAttrib4fv(0, attrib);

	const GLfloat vertex_color[] = {
		static_cast<float>(sin(currentTime)) * 0.5f + 0.5f,
		static_cast<float>(cos(currentTime)) * 0.5f + 0.5f,
		0.0f, 1.0f
	};
	glVertexAttrib4fv(1, vertex_color);

	// Draw one triangle
	glDrawArrays(GL_TRIANGLES, 0, 3);
}

void on_resize(int width, int height)
{
}

void shutdown(void)
{
	glDeleteVertexArrays(1, &vertex_array_object);
	glDeleteProgram(rendering_program);
}