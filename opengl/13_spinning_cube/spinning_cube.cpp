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

void startup()
{
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
			-x -------- +x
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
		 0.25f,  0.25f,  0.25f, // (6)
		 0.25f,  0.25f,  0.25f, // (5)

		 0.25f,  0.25f,  0.25f, // (6)
		-0.25f,  0.25f,  0.25f, // (7)
		 0.25f,  0.25f,  0.25f, // (5)

		 0.25f,  0.25f,  0.25f, // (6)
		-0.25f, -0.25f, -0.25f, // (1)
		-0.25f,  0.25f,  0.25f, // (7)

		-0.25f, -0.25f, -0.25f, // (1)
		-0.25f,  0.25f, -0.25f, // (0)
		-0.25f,  0.25f,  0.25f, // (7)

		 0.25f,  0.25f,  0.25f, // (6)
		 0.25f, -0.25f,  0.25f, // (4)
		 0.25f, -0.25f, -0.25f, // (2)

		 0.25f, -0.25f, -0.25f, // (2)
		-0.25f, -0.25f, -0.25f, // (1)
		 0.25f,  0.25f,  0.25f, // (6)

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
}

void render(double currentTime)
{
	const GLfloat color[] = { 0.0f, 0.0f, 1.0f, 1.0f };
	glClearBufferfv(GL_COLOR, 0, color);

	float f = static_cast<float>(currentTime) * static_cast<float>(M_PI)* 0.1f;
	glm::mat4 mv_matrix =
		glm::translate(glm::mat4(), glm::vec3(                 0.0f,                  0.0f,                                  -4.0f)) *
		glm::translate(glm::mat4(), glm::vec3(sinf(2.1f * f) * 0.5f, cosf(1.7f * f) * 0.5f, sinf(1.3f * f) * cosf(1.5f * f) * 2.0f)) *
		glm::rotate(glm::mat4(), static_cast<float>(currentTime) * 45.0f / 360.0f * 2.0f * static_cast<float>(M_PI), glm::vec3(0.0f, 1.0f, 0.0f)) *
		glm::rotate(glm::mat4(), static_cast<float>(currentTime) * 81.0f / 360.0f * 2.0f * static_cast<float>(M_PI), glm::vec3(1.0f, 0.0f, 0.0f));

}

void shutdown()
{
}