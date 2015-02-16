#include <stddef.h>
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
		"layout (location = 0) in vec3 position;	\n"
		"layout (location = 1) in vec3 color;		\n"
		"											\n"
		"out VS_OUT									\n"
		"{											\n"
		"	vec3 color;								\n"
		"} vs_out;									\n"
		"											\n"
		"void main(void)							\n"
		"{											\n"
		"	gl_Position = vec4(position, 1.0f);		\n"
		"											\n"
		"	vs_out.color = color;					\n"
		"}											\n"
	};

	static const GLchar *fragment_shader_source[] = {
		"#version 430 core							\n"
		"											\n"
		"in VS_OUT									\n"
		"{											\n"
		"	vec3 color;								\n"
		"} fs_in;									\n"
		"											\n"
		"out vec3 color;							\n"
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

struct vertex
{
	// Position
	GLfloat x;
	GLfloat y;
	GLfloat z;

	// Color
	GLfloat r;
	GLfloat g;
	GLfloat b;
};

void startup(void)
{
	rendering_program = compile_shaders();
	glGenVertexArrays(1, &vertex_array_object);
	glBindVertexArray(vertex_array_object);

	static const vertex verticeis[] = {
		{  0.25f, -0.25f, 0.5f, 1.0f, 0.0f, 0.0f },
		{ -0.25f, -0.25f, 0.5f, 0.0f, 1.0f, 0.0f },
		{  0.25f,  0.25f, 0.5f, 0.0f, 0.0f, 1.0f }
	};

	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticeis), verticeis, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), reinterpret_cast<GLvoid*>(offsetof(vertex, x)));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), reinterpret_cast<GLvoid*>(offsetof(vertex, r)));
	glEnableVertexAttribArray(1);

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
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);
	glDeleteVertexArrays(1, &vertex_array_object);
	glDeleteProgram(rendering_program);
	glDeleteBuffers(1, &vertex_buffer);
}