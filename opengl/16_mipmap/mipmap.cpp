// original code: https://github.com/openglsuperbible/sb6code/blob/master/src/tunnel/tunnel.cpp
/*
* Copyright © 2012-2013 Graham Sellers
*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the "Software"),
* to deal in the Software without restriction, including without limitation
* the rights to use, copy, modify, merge, publish, distribute, sublicense,
* and/or sell copies of the Software, and to permit persons to whom the
* Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice (including the next
* paragraph) shall be included in all copies or substantial portions of the
* Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
* DEALINGS IN THE SOFTWARE.
*/

#include <math.h>
#include <vector>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <sb6ktx.h>

GLuint rendering_program;
GLuint vertex_array_object;
GLuint wall_texture;
GLuint ceiling_texture;
GLuint floor_texture;
GLuint mvp_matrix_location;
GLuint offset_location;

GLuint compile_shaders(void)
{
	GLuint vertex_shader;
	GLuint fragment_shader;
	GLuint program;

	static const GLchar *vertex_shader_source[] = {
		"#version 430 core																		\n"
		"																						\n"
		"out VS_OUT																				\n"
		"{																						\n"
		"	vec2 texture_coordinate;															\n"
		"} vs_out;																				\n"
		"																						\n"
		"uniform mat4 mvp_matrix;																\n"
		"uniform float offset;																	\n"
		"																						\n"
		"void main(void)																		\n"
		"{																						\n"
		"	const vec2[4] position = vec2[4](vec2(-0.5f, -0.5f),								\n"
		"									 vec2( 0.5f, -0.5f),								\n"
		"									 vec2(-0.5f,  0.5f),								\n"
		"									 vec2( 0.5f,  0.5f));								\n"
		"	vs_out.texture_coordinate = (position[gl_VertexID].xy + vec2(offset, 0.5f))			\n"
		"							  * vec2(30.0f, 1.0f);										\n"
		"	gl_Position = mvp_matrix * vec4(position[gl_VertexID], 0.0f, 1.0f);					\n"
		"}																						\n"
	};

	static const GLchar *fragment_shader_source[] = {
		"#version 430 core									\n"
		"													\n"
		"layout (location = 0) out vec4 color;				\n"
		"													\n"
		"in VS_OUT											\n"
		"{													\n"
		"	vec2 texture_coordinate;						\n"
		"} fs_in;											\n"
		"													\n"
		"layout (binding = 0) uniform sampler2D tex;		\n"
		"													\n"
		"void main(void)									\n"
		"{													\n"
		"	color = texture(tex, fs_in.texture_coordinate);	\n"
		"}													\n"
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

	mvp_matrix_location = glGetUniformLocation(rendering_program, "mvp_matrix");
	offset_location = glGetUniformLocation(rendering_program, "offset");

	wall_texture = sb6::ktx::file::load("../media/textures/brick.ktx");
	ceiling_texture = sb6::ktx::file::load("../media/textures/ceiling.ktx");
	floor_texture = sb6::ktx::file::load("../media/textures/floor.ktx");

	for (auto texture : std::vector<GLuint>({ wall_texture, ceiling_texture, floor_texture })) {
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

	glGenVertexArrays(1, &vertex_array_object);
	glBindVertexArray(vertex_array_object);
}

void render(double currentTime)
{
	int width = glutGet(GLUT_WINDOW_WIDTH);
	int height = glutGet(GLUT_WINDOW_HEIGHT);
	glViewport(0, 0, width, height);

	static const GLfloat black[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	glClearBufferfv(GL_COLOR, 0, black);

	glUseProgram(rendering_program);
	glUniform1f(offset_location, static_cast<float>(currentTime)* 0.003f);

	float fovy = 60.0f / 360.0f * 2 * static_cast<float>(M_PI);
	float aspect = static_cast<float>(width) / static_cast<float>(height);
	glm::mat4 proj_matrix = glm::perspective(fovy, aspect, 0.1f, 100.0f);

	GLuint textures[] = { wall_texture, floor_texture, wall_texture, ceiling_texture };
	for (int i = 0; i < 4; i++) {
		float angle90 = 90.0f / 360.0f * 2.0f * static_cast<float>(M_PI);
		glm::mat4 mv_matrix = glm::rotate(glm::mat4(), angle90 * static_cast<float>(i), glm::vec3(0.0f, 0.0f, 1.0f))
							* glm::translate(glm::mat4(), glm::vec3(-0.5f, 0.0f, -10.0f))
							* glm::rotate(glm::mat4(), angle90, glm::vec3(0.0f, 1.0f, 0.0f))
							* glm::scale(glm::mat4(), glm::vec3(30.0f, 1.0f, 1.0f));
		glm::mat4 mvp_matrix = proj_matrix * mv_matrix;
		glUniformMatrix4fv(mvp_matrix_location, 1, GL_FALSE, glm::value_ptr(mvp_matrix));
		glBindTexture(GL_TEXTURE_2D, textures[i]);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}
}

void on_resize(int width, int height)
{
}

void shutdown(void)
{
	GLuint textures[] = { wall_texture, ceiling_texture, floor_texture };
	glDeleteTextures(3, textures);
	glDeleteVertexArrays(1, &vertex_array_object);
	glDeleteProgram(rendering_program);
}