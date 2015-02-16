#include <GL/glew.h>
#include <GL/freeglut.h>

GLuint vertex_buffer;
GLuint rendering_program;
GLuint vertex_array_object;

GLuint compile_shaders(void)
{
	GLuint vertex_shader;
	GLuint fragment_shader;
	GLuint program;

	static const GLchar *vertex_shader_source[] = {
		"#version 430 core							\n"
		"											\n"
		"layout (location = 0) in vec4 position;	\n"
		"											\n"
		"void main(void)							\n"
		"{											\n"
		"	gl_Position = position;					\n"
		"}											\n"
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
	static const float vertex_data[] = {
		 0.25f, -0.25f, 0.5f, 1.0f,
		-0.25f, -0.25f, 0.5f, 1.0f,
		 0.25f,  0.25f, 0.5f, 1.0f
	};

	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data), vertex_data, GL_STATIC_DRAW),

	rendering_program = compile_shaders();
	glGenVertexArrays(1, &vertex_array_object);
	glBindVertexArray(vertex_array_object);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(0);
}

void render(double currentTime)
{
	const GLfloat color[] = { 0.0f, 0.2f, 0.0f, 1.0f };
	glClearBufferfv(GL_COLOR, 0, color);
	glUseProgram(rendering_program);
	glDrawArrays(GL_TRIANGLES, 0, 3);
}

void shutdown(void)
{
	glDisableVertexAttribArray(0);
	glDeleteVertexArrays(1, &vertex_array_object);
	glDeleteProgram(rendering_program);
	glDeleteBuffers(1, &vertex_buffer);
}