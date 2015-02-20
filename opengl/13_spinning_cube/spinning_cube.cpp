#define _USE_MATH_DEFINES
#include <math.h>

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

GLuint vao;
GLuint buffer;
GLuint program;
GLuint mv_location;
GLuint proj_location;
glm::mat4 proj_matrix;

GLuint compile_shaders(void)
{
	GLuint vertex_shader;
	GLuint fragment_shader;
	GLuint program;

	static const GLchar *vertex_shader_source[] = {
		"#version 420 core													\n"
		"																	\n"
		"in vec4 position;													\n"
		"																	\n"
		"out VS_OUT															\n"
		"{																	\n"
		"	vec4 color;														\n"
		"} vs_out;															\n"
		"																	\n"
		"uniform mat4 mv_matrix;											\n"
		"uniform mat4 proj_matrix;											\n"
		"																	\n"
		"void main(void)													\n"
		"{																	\n"
		"	gl_Position = proj_matrix * mv_matrix * position;				\n"
		"	vs_out.color = position * 2.0 + vec4(0.5f, 0.5f, 0.5f, 0.0f);	\n"
		"}																	\n"
	};

	static const GLchar *fragment_shader_source[] = {
		"#version 420 core							\n"
		"											\n"
		"out vec4 color;							\n"
		"											\n"
		"in VS_OUT									\n"
		"{											\n"
		"	vec4 color;								\n"
		"} fs_in;									\n"
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


void startup()
{
	program = compile_shaders();

	mv_location = glGetUniformLocation(program, "mv_matrix");
	proj_location = glGetUniformLocation(program, "proj_matrix");

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	static const GLfloat vertex_positions[] = {
		/*
				  (0)---------------(3)
				  /|                /|
				 / |               / |
				/  |              /  |
			   /   |             /   |
			  /    |            /    |
			(7)---------------(5)    |
			 |     |           |     |
			 |     |           |     |
			 |    (1)----------|----(2)
			 |    /            |    /
			 |   /             |   /
			 |  /              |  /
			 | /               | /
			(6)---------------(4)

				  +y  -z
				  |  /
				  | /
				  |/
			-x ---*---- +x
				 /|
				/ |
			   /  |
			 +z  -y
		*/
		-0.25f,  0.25f, -0.25f, // (0)
		-0.25f, -0.25f, -0.25f, // (1)
		 0.25f, -0.25f, -0.25f, // (2)

		 0.25f, -0.25f, -0.25f, // (2)
		 0.25f,  0.25f, -0.25f, // (3)
		-0.25f,  0.25f, -0.25f, // (0)

		 0.25f, -0.25f, -0.25f, // (2)
		 0.25f, -0.25f,  0.25f, // (4)
		 0.25f,  0.25f, -0.25f, // (3)

		 0.25f, -0.25f,  0.25f, // (4)
		 0.25f,  0.25f,  0.25f, // (5)
		 0.25f,  0.25f, -0.25f, // (3)

		 0.25f, -0.25f,  0.25f, // (4)
		-0.25f, -0.25f,  0.25f, // (6)
		 0.25f,  0.25f,  0.25f, // (5)

		-0.25f, -0.25f,  0.25f, // (6)
		-0.25f,  0.25f,  0.25f, // (7)
		 0.25f,  0.25f,  0.25f, // (5)

		-0.25f, -0.25f,  0.25f, // (6)
		-0.25f, -0.25f, -0.25f, // (1)
		-0.25f,  0.25f,  0.25f, // (7)

		-0.25f, -0.25f, -0.25f, // (1)
		-0.25f,  0.25f, -0.25f, // (0)
		-0.25f,  0.25f,  0.25f, // (7)

		-0.25f, -0.25f,  0.25f, // (6)
		 0.25f, -0.25f,  0.25f, // (4)
		 0.25f, -0.25f, -0.25f, // (2)

		 0.25f, -0.25f, -0.25f, // (2)
		-0.25f, -0.25f, -0.25f, // (1)
		-0.25f, -0.25f,  0.25f, // (6)

		-0.25f,  0.25f, -0.25f, // (0)
		 0.25f,  0.25f, -0.25f, // (3)
		 0.25f,  0.25f,  0.25f, // (5)

		 0.25f,  0.25f,  0.25f, // (5)
		-0.25f,  0.25f,  0.25f, // (7)
		-0.25f,  0.25f, -0.25f  // (0)
	};

	// make array buffer and transfer the data
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_positions), vertex_positions, GL_STATIC_DRAW);

	// tell the contents of attribute 0 lies on the head of the buffer
	glVertexAttribPointer(
		0,			// Attribute 0
		3,			// Three components
		GL_FLOAT,	// Floating-point data
		GL_FALSE,	// Not normalized (floating-point data never is)
		0,			// Tightly packed
		nullptr		// Offset zero (the contents lies on the head)
	);
	glEnableVertexAttribArray(0);

	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
}

void render(double currentTime)
{
	static const GLfloat green[] = { 0.0f, 0.25f, 0.0f, 1.0f };
	glClearBufferfv(GL_COLOR, 0, green);

	static const GLfloat one = 1.0f;
	glClearBufferfv(GL_DEPTH, 0, &one);

	glUseProgram(program);

	glUniformMatrix4fv(proj_location, 1, GL_FALSE, glm::value_ptr(proj_matrix));

	for (int i = 0; i < 24; i++) {
		float pi = static_cast<float>(M_PI);
		float t = static_cast<float>(currentTime);
		float p = static_cast<float>(i) + t * pi * 0.1f; // phase
		glm::mat4 mv_matrix = glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, -6.0f))
							* glm::rotate(glm::mat4(), t * 45.0f / 360.0f * 2.0f * pi, glm::vec3(0.0f, 1.0f, 0.0f))
							* glm::rotate(glm::mat4(), t * 21.0f / 360.0f * 2.0f * pi, glm::vec3(1.0f, 0.0f, 0.0f))
							* glm::translate(glm::mat4(), glm::vec3(sinf(2.1f * p) * 0.5f, cosf(1.7f * p) * 0.5f, sinf(1.3f * p) * cosf(1.5f * p) * 2.0f));

		glUniformMatrix4fv(mv_location, 1, GL_FALSE, glm::value_ptr(mv_matrix));

		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
}

void on_resize(int width, int height)
{
	float aspect = static_cast<float>(width) / static_cast<float>(height);
	proj_matrix = glm::perspective(50.9f, aspect, 0.1f, 1000.0f);
}

void shutdown()
{
	glDeleteBuffers(1, &buffer);
	glDisableVertexArrayAttrib(program, 0);
	glDeleteVertexArrays(1, &vao);
	glDeleteProgram(program);
}